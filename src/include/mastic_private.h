//
//  mastic_private.h
//  mastic
//
//  Created by Stephane Vales on 27/04/2017.
//  Modified by Mathieu Poirier
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#ifndef mastic_private_h
#define mastic_private_h

#if defined WINDOWS
#if defined MASTIC
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllexport)
#else
#define MASTICAPI_COMMON_DLLSPEC __declspec(dllimport)
#endif
#else
#define MASTICAPI_COMMON_DLLSPEC
#endif

#include <stdbool.h>
#include <string.h>
#include <zyre.h>

#include "uthash/uthash.h"

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <iphlpapi.h>
#else
#define MAX_PATH 260
#endif

#include "mastic.h"

extern char definitionPath[1024];
extern char mappingPath[1024];

//////////////////  STRUCTURES AND ENUMS   //////////////////

typedef struct mtic_observe_callback {
    mtic_observeCallback callback_ptr;
    void* data;
    struct mtic_observe_callback *prev;
    struct mtic_observe_callback *next;
} mtic_observe_callback_t;

/*
 * Define the structure agent_iop (input, output, parameter) :
 * 'name'       : the input/output/parameter's name. Need to be unique in each type of iop (input/output/parameter)
 * 'value_type' : the type of the value (int, double, string, impulsion ...)
 * 'type'       : the type of the iop : input / output / parameter
 * 'value'      : the input/output/parameter'value
 * 'valueSize'  : the size of the value
 * 'is_muted'   : flag indicated if the iop is muted (specially used for outputs)
 */
struct agent_iop {
    const char* name;
    iopType_t value_type;
    iop_t type;          //Size of pointer on data
    struct {
        int i;                  //in accordance to type INTEGER_T ex. '10'
        double d;               //in accordance to type DOUBLE_T ex. '10.01'
        char* s;                //in accordance to type STRING_T ex. 'display the image'
        bool b;                 //in accordante to type BOOL_T ex. 'true' or 'false'
        void* data;             //in accordance to type DATA_T ex. '{int:x, int:y, string:gesture_name} <=> {int:10, int:45, string:swap}
    } value;
    long valueSize;
    bool is_muted;
    mtic_observe_callback_t *callbacks;
    UT_hash_handle hh;         /* makes this structure hashable */
};

typedef struct agent_iop agent_iop;
typedef struct category category;
typedef struct definition definition;

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
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} definition;

/*
 * Define the structure CATEGORY :
 * 'name'                   : name of the category
 * 'version'                : version of the category
 * 'parameters'             : list of parameters which describe the category
 * 'inputs'                 : list of inputs which describe the category
 * 'outputs'                : list of outputs which describe the category
 */
typedef struct category {
    const char* name;
    const char* version;
    agent_iop* params_table;
    agent_iop* inputs_table;
    agent_iop* outputs_table;
    UT_hash_handle hh;
} category;

/*
 * Define the state during the checking of a category
 */
typedef enum {
    GLOBAL_CAT,
    OUTPUT_CAT,
    INPUT_CAT
} category_check_type;


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
 * Define the structure 'mapping_cat' which contains mapping between an input and an (one or all) external agent's category :
 * 'map_id'                         : the key of the table. Need to be unique : the table hash key
 * 'agent name to connect'          : external agent's name to connect with (one or all)
 * 'category unique name to connect': external agent(s) category to connect with
 */
//typedef struct mapping_cat {
//    int map_cat_id;
//    char* agent_name;
//    char*category_name;
//    map_state state;
//    UT_hash_handle hh;
//} mapping_cat_t;

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
//    mapping_cat* map_cat;
    UT_hash_handle hh;
} mapping_t;

#define MAX_FILTER_SIZE 1024
typedef struct mappingFilter {
    char filter[MAX_FILTER_SIZE];
    struct mappingFilter *next, *prev;
} mappingFilter_t;

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

//network internal structure
#define NETWORK_DEVICE_LENGTH 16
#define IP_ADDRESS_LENGTH 256
typedef struct zyreloopElements{
    char networkDevice[NETWORK_DEVICE_LENGTH];
    char ipAddress[IP_ADDRESS_LENGTH];
    int zyrePort;
    zactor_t *agentActor;
    zyre_t *node;
    zsock_t *publisher;
    zloop_t *loop;
} zyreloopElements_t;

//////////////////  FUNCTIONS  //////////////////

//  definition

extern definition* mtic_internal_definition;

char* definition_getIOPValueAsString (agent_iop* iop);
void definition_freeDefinition (definition* definition);


//  mapping

extern mapping_t *mtic_internal_mapping;

void mapping_freeMapping (mapping_t* map);
mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name);
unsigned long djb2_hash (unsigned char *str);
bool mapping_checkCompatibilityInputOutput(agent_iop *foundInput, agent_iop *foundOutput);

// model

extern bool isWholeAgentMuted;

void model_setIopValue(agent_iop *iop, void* value, long size);
agent_iop* model_findIopByName(const char* name, iop_t type);
agent_iop* model_findInputByName(const char* name);
agent_iop* model_findOutputByName(const char* name);
agent_iop* model_findParameterByName(const char* name);
void* model_getValueFor(const char*name_iop, iop_t type);


// network
#define CHANNEL "MASTIC_PRIVATE"

extern bool network_needToSendDefinitionUpdate;
extern bool network_needToUpdateMapping;
extern bool network_isEditor;
extern subscriber_t *subscribers;
extern zyreloopElements_t *agentElements;
//DO NOT DESTROY THE ZYRE_EVENT INSIDE THE CALLBACK
typedef int (*network_zyreIncoming) (const zyre_event_t *zyre_event, void *arg);

int network_observeZyre(network_zyreIncoming cb, void *myData);
#define AGENT_NAME_DEFAULT "mtic_noname"
int network_publishOutput (const char* output_name);

// parser

definition* parser_loadDefinition (const char* json_str);
definition* parser_loadDefinitionFromPath (const char* file_path);
char* parser_export_definition (definition* def);
char* parser_export_mapping(mapping_t* mapp);
mapping_t* parser_LoadMap (const char* json_str);
mapping_t* parser_LoadMapFromPath (const char* load_file);

#endif /* mastic_private_h */
