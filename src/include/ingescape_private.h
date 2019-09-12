//
//  ingescape_private.h
//  ingescape - https://ingescape.com
//
//  Created by Stephane Vales on 27/04/2017.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_private_h
#define ingescape_private_h

#if (defined WIN32 || defined _WIN32)
#if defined INGESCAPE
#define PUBLIC __declspec(dllexport)
#elif defined INGESCAPE_FROM_PRI
#define PUBLIC
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

#include <stdbool.h>
#include <string.h>
#include <zyre.h>

#include "uthash/uthash.h"
#include "uthash/utlist.h"

#if (defined WIN32 || defined _WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#endif

#include "ingescape.h"
#include "ingescape_advanced.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IGS_MAX_PATH 4096
#define MAX_IOP_NAME_LENGTH 256
#define MAX_AGENT_NAME_LENGTH 256
#define MAX_DEFINITION_NAME_LENGTH 1024
#define MAX_MAPPING_NAME_LENGTH 1024
#define MAX_DESCRIPTION_LENGTH 4096
#define MAX_MAPPING_DESCRIPTION_LENGTH 4096

//////////////////  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_observe_callback {
    igs_observeCallback callback_ptr;
    void* data;
    struct igs_observe_callback *prev;
    struct igs_observe_callback *next;
} igs_observe_callback_t;

/*
 * Define the structure agent_iop_t (input, output, parameter) :
 * 'name'       : the input/output/parameter's name. Need to be unique in each type of iop (input/output/parameter)
 * 'value_type' : the type of the value (int, double, string, impulsion ...)
 * 'type'       : the type of the iop : input / output / parameter
 * 'value'      : the input/output/parameter'value
 * 'valueSize'  : the size of the value
 * 'is_muted'   : flag indicated if the iop is muted (specially used for outputs)
 */
typedef struct agent_iop {
    const char* name;
    iopType_t value_type;
    iop_t type;          //Size of pointer on data
    union {
        int i;                  //in accordance to type IGS_INTEGER_T ex. '10'
        double d;               //in accordance to type IGS_DOUBLE_T ex. '10.01'
        char* s;                //in accordance to type IGS_STRING_T ex. 'display the image'
        bool b;                 //in accordante to type IGS_BOOL_T ex. 0 or 1
        void* data;             //in accordance to type IGS_DATA_T i.e. binary memory space
    } value;
    size_t valueSize;
    bool is_muted;
    igs_observe_callback_t *callbacks;
    UT_hash_handle hh;         /* makes this structure hashable */
} agent_iop_t;

typedef struct call{
    char * name;
    char * description;
    igs_callFunction cb;
    void *cbData;
    igs_callArgument_t *arguments;
    struct call *reply;
    UT_hash_handle hh;
} igs_call_t;

/*
 * Define the structure DEFINITION :
 * 'name'                   : agent name
 * 'description'            : human readable description of the agent
 * 'version'                : the version of the agent
 * 'parameters'             : list of parameters contains by the agent
 * 'inputs'                 : list of inputs contains by the agent
 * 'outputs'                : list of outputs contains by the agent
 */
typedef struct definition {
    const char* name; //hash key
    const char* description;
    const char* version;
//    category* categories;
    agent_iop_t* params_table;
    agent_iop_t* inputs_table;
    agent_iop_t* outputs_table;
    igs_call_t *calls_table;
    UT_hash_handle hh;
} definition;

/*
 * Define the structure 'mapping_element' which contains mapping between an input and an (one or all) external agent's output :
 * 'map_id'                 : the key of the table. Need to be unique : the table hash key
 * 'input name'             : agent's input name to connect
 * 'agent name to connect'  : external agent's name to connect with (one or all)
 * 'output name to connect' : external agent(s) output name to connect with
 */
typedef struct mapping_element {
    unsigned long id;
    char* input_name;
    char* agent_name;
    char* output_name;
    UT_hash_handle hh;
} mapping_element_t;

/*
 * Define the structure 'mapping' which contains the json description of all mapping (output & category):
 * 'name'           : The name of the mapping. Need to be unique
 * 'description     :
 * 'version'        :
 * 'mapping out'    : the table of the mapping output
 * 'mapping cat'    : the table of the mapping category
 */
typedef struct mapping {
    char* name;
    char* description;
    char* version;
    mapping_element_t* map_elements;
    UT_hash_handle hh;
} mapping_t;

#define MAX_FILTER_SIZE 1024
typedef struct mappingFilter {
    char filter[MAX_FILTER_SIZE];
    struct mappingFilter *next, *prev;
} mappingFilter_t;

//network internal structures
typedef struct subscriber_s{
    const char *agentName;
    const char *agentPeerId;
    zsock_t *subscriber;
    zmq_pollitem_t *pollItem;
    definition *definition;
    bool mappedNotificationToSend;
    mapping_t *mapping;
    mappingFilter_t *mappingsFilters;
    int timerId;
    UT_hash_handle hh;
} subscriber_t;

#define NETWORK_DEVICE_LENGTH 256
#define IP_ADDRESS_LENGTH 256
typedef struct zyreloopElements{
    char networkDevice[NETWORK_DEVICE_LENGTH];
    char ipAddress[IP_ADDRESS_LENGTH];
    char brokerEndPoint[IP_ADDRESS_LENGTH];
    int zyrePort;
    zactor_t *agentActor;
    zyre_t *node;
    zsock_t *publisher;
    zsock_t *ipcPublisher;
    zsock_t *logger;
    zloop_t *loop;
} zyreloopElements_t;

//zyre agents storage
#define NAME_BUFFER_SIZE 256
typedef struct zyreAgent {
    char peerId[NAME_BUFFER_SIZE];
    char name[NAME_BUFFER_SIZE];
    subscriber_t *subscriber;
    int reconnected;
    bool hasJoinedPrivateChannel;
    UT_hash_handle hh;
} zyreAgent_t;

//zyre headers for service description
typedef struct serviceHeader {
    char *key;
    char *value;
    UT_hash_handle hh;
} serviceHeader_t;
    
//license
typedef struct licenseForAgent {
    char *agentId;
    char *agentName;
} licenseForAgent_t;

typedef struct license {
    char *id;
    char *customer;
    char *order;
    long licenseExpirationDate;
    bool isLicenseValid;
    int platformNbAgents;
    int platformNbIOPs;
    char *editorOwner;
    long editorExpirationDate;
    bool isEditorLicenseValid;
    zhash_t *features;
    zhash_t *agents;
    zlist_t *licenseDetails; //license struct for each detected license file
    char *fileName; //used only in each detail
} license_t;
    
typedef struct licenseEnforcement {
    long currentIOPNb;
    long currentAgentsNb;
} licenseEnforcement_t;

typedef struct license_callback {
    igs_licenseCallback callback_ptr;
    void* data;
    struct license_callback *prev;
    struct license_callback *next;
} license_callback_t;
    

//////////////////  FUNCTIONS  //////////////////

//  definition
extern char definitionPath[IGS_MAX_PATH];
extern definition* igs_internal_definition;
PUBLIC void definition_freeDefinition (definition* definition);

//  mapping
extern char mappingPath[IGS_MAX_PATH];
extern mapping_t *igs_internal_mapping;
PUBLIC void mapping_freeMapping (mapping_t* map);
mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name);
unsigned long djb2_hash (unsigned char *str);
bool mapping_checkCompatibilityInputOutput(agent_iop_t *foundInput, agent_iop_t *foundOutput);

// model
extern bool isWholeAgentMuted;
const agent_iop_t* model_writeIOP (const char *iopName, iop_t iopType, iopType_t valType, void* value, size_t size);
agent_iop_t* model_findIopByName(const char* name, iop_t type);
char* model_getIOPValueAsString (agent_iop_t* iop); //returned value must be freed by user
void model_readWriteLock(void);
void model_readWriteUnlock(void);

// network
#define CHANNEL "INGESCAPE_PRIVATE"
#define AGENT_NAME_DEFAULT "igs_noname"

PUBLIC extern zyreAgent_t *zyreAgents;
extern bool network_needToSendDefinitionUpdate;
extern bool network_needToUpdateMapping;
extern subscriber_t *subscribers;
extern zyreloopElements_t *agentElements;
extern char *ipcFolderPath;
int network_publishOutput (const agent_iop_t *iop);

// parser
PUBLIC definition* parser_loadDefinition (const char* json_str);
PUBLIC definition* parser_loadDefinitionFromPath (const char* file_path);
char* parser_export_definition (definition* def);
PUBLIC char* parser_export_mapping(mapping_t* mapp);
mapping_t* parser_LoadMap (const char* json_str);
mapping_t* parser_LoadMapFromPath (const char* load_file);

// admin
extern bool admin_logInStream;
extern bool admin_logInFile;
extern char admin_logFile[4096];

//bus
extern serviceHeader_t *serviceHeaders;

//call
void call_freeCall(igs_call_t *t);
bool call_addValuesToArgumentsFromMessage(const char *name, igs_callArgument_t *arg, zmsg_t *msg);
int call_freeValuesInArguments(igs_callArgument_t *arg);

//license
#define ENABLE_LICENSE_ENFORCEMENT 1
#define MAX_NB_OF_AGENTS 50
#define MAX_NB_OF_IOP 1000
#define MAX_EXEC_DURATION_DURING_EVAL 300
extern license_t *license;
extern license_callback_t *licenseCallbacks;
#if !TARGET_OS_IOS
void license_cleanLicense(void);
void license_readLicense(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ingescape_private_h */
