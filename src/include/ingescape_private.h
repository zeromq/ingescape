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

#include "ingescape_agent.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IGS_MAX_PATH_LENGTH 4096
#define IGS_MAX_IOP_NAME_LENGTH 1024
#define IGS_MAX_AGENT_NAME_LENGTH 1024
#define IGS_MAX_DEFINITION_NAME_LENGTH 1024
#define IGS_MAX_MAPPING_NAME_LENGTH 1024
#define IGS_MAX_DESCRIPTION_LENGTH 4096
#define IGS_MAX_LOG_LENGTH 4096
#define IGS_COMMAND_LINE_LENGTH 4096
#define IGS_NETWORK_DEVICE_LENGTH 1024
#define IGS_IP_ADDRESS_LENGTH 1024
#define IGS_MAX_PEER_ID_LENGTH 128
#define IGS_DEFAULT_IPC_FOLDER_PATH "/tmp/"


typedef struct igs_core_context igs_core_context_t;

//////////////////  IOP/CALL  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_observe_callback{
    igsAgent_observeCallback callback_ptr;
    void* data;
    struct igs_observe_callback *prev;
    struct igs_observe_callback *next;
} igs_observe_callback_t;

typedef struct igs_iop{
    const char* name;
    iopType_t value_type;
    iop_t type;
    union {
        int i;
        double d;
        char* s;
        bool b;
        void* data;
    } value;
    size_t valueSize;
    bool is_muted;
    igs_observe_callback_t *callbacks;
    UT_hash_handle hh;         /* makes this structure hashable */
} igs_iop_t;

typedef struct igs_call{
    char * name;
    char * description;
    igsAgent_callFunction cb;
    void *cbData;
    igs_callArgument_t *arguments;
    struct igs_call *reply;
    UT_hash_handle hh;
} igs_call_t;

typedef struct igs_definition{
    const char* name; //hash key
    const char* description;
    const char* version;
    igs_iop_t* params_table;
    igs_iop_t* inputs_table;
    igs_iop_t* outputs_table;
    igs_call_t *calls_table;
    UT_hash_handle hh;
} igs_definition_t;

typedef struct igs_mapping_element{
    unsigned long id;
    char* input_name;
    char* agent_name;
    char* output_name;
    UT_hash_handle hh;
} igs_mapping_element_t;

typedef struct igs_mapping{
    char* name;
    char* description;
    char* version;
    igs_mapping_element_t* map_elements;
    UT_hash_handle hh;
} igs_mapping_t;

typedef struct igs_mappings_filter {
    char *filter;
    struct igs_mappings_filter *next, *prev;
} igs_mappings_filter_t;

//////////////////  NETWORK  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_zyre_peer {
    char *peerId;
    char *name;
    zsock_t *subscriber; //link to the peer's publisher socket
    int reconnected;
    bool hasJoinedPrivateChannel;
    UT_hash_handle hh;
} igs_zyre_peer_t;

//remote agent we are subscribing to
typedef struct igs_remote_agent{
    char *uuid;
    char *name;
    igs_zyre_peer_t *peer;
    igs_core_context_t *context;
    igs_definition_t *definition;
    bool shallSendOutputsRequest;
    igs_mapping_t *mapping;
    igs_mappings_filter_t *mappingsFilters;
    int timerId;
    UT_hash_handle hh;
} igs_remote_agent_t;

typedef struct igs_timer{
    int timerId;
    igs_timerCallback *cb;
    void *myData;
    UT_hash_handle hh;
} igs_timer_t;

typedef struct igs_service_header {
    char *key;
    char *value;
    UT_hash_handle hh;
} igs_service_header_t;


//////////////////  LICENSE  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_license_for_agent {
    char *agentId;
    char *agentName;
} igs_license_for_agent_t;

typedef struct igs_license {
    char *id;
    char *customer;
    char *order;
    time_t licenseExpirationDate;
    bool isLicenseValid;
    int platformNbAgents;
    int platformNbIOPs;
    char *editorOwner;
    time_t editorExpirationDate;
    bool isEditorLicenseValid;
    zhash_t *features;
    zhash_t *agents;
    zlist_t *licenseDetails; //license struct for each detected license file
    char *fileName; //used only in each detail
} igs_license_t;
    
typedef struct igs_license_enforcement {
    long currentIOPNb;
    long currentAgentsNb;
} igs_license_enforcement_t;

typedef struct igs_license_callback {
    igs_licenseCallback *callback_ptr;
    void* data;
    struct igs_license_callback *prev;
    struct igs_license_callback *next;
} igs_license_callback_t;


//////////////////  UTILITY  STRUCTURES  //////////////////

typedef struct igs_monitor {
    unsigned int period;
    igs_monitorEvent_t status;
    zactor_t *monitorActor;
    zloop_t *loop;
    unsigned int port;
    char *networkDevice;
} igs_monitor_t;

typedef struct igs_monitor_callback {
    igs_monitorCallback callback_ptr;
    void *myData;
    struct igs_monitor_callback *prev;
    struct igs_monitor_callback *next;
} igs_monitor_callback_t;

typedef struct igs_mute_callback {
    igsAgent_muteCallback callback_ptr;
    void *myData;
    struct igs_mute_callback *prev;
    struct igs_mute_callback *next;
} igs_mute_callback_t;

typedef struct igs_freeze_callback {
    igs_freezeCallback callback_ptr;
    void *myData;
    struct igs_freeze_callback *prev;
    struct igs_freeze_callback *next;
} igs_freeze_callback_t;

typedef struct igs_zyre_callback {
    igs_BusMessageIncoming callback_ptr;
    void *myData;
    struct igs_zyre_callback *prev;
    struct igs_zyre_callback *next;
} igs_zyre_callback_t;

typedef struct igs_external_stop_calback {
    igs_externalStopCallback callback_ptr;
    void *myData;
    struct igs_external_stop_calback *prev;
    struct igs_external_stop_calback *next;
} igs_external_stop_calback_t;

typedef struct igs_activate_calback {
    igsAgent_activateCallback callback_ptr;
    void *myData;
    struct igs_activate_calback *prev;
    struct igs_activate_calback *next;
} igs_activate_calback_t;

typedef struct igs_agentEvent_calback {
    igsAgent_agentEventCallback callback_ptr;
    void *myData;
    struct igs_agentEvent_calback *prev;
    struct igs_agentEvent_calback *next;
} igs_agent_event_callback_t;


//////////////////  MAIN  STRUCTURES   //////////////////

/*
 The core context hosts eveything needed by an agent or
 a set of agents at a process level.
 */
typedef struct igs_core_context{
    
    ////////////////////////////////////////////
    //persisting data with setters and getters or
    //managed automatically
    //
    //license
    igs_license_t *license;
    igs_license_callback_t *licenseCallbacks;
    char *licensePath;
    void *licenseData; //overrides licence files
    size_t licenseDataSize;
    
    //bus
    igs_service_header_t *serviceHeaders;
    
    //admin
    FILE *logFile;
    bool logInStream;
    bool logInFile;
    bool logInConsole;
    bool useColorInConsole;
    igs_logLevel_t logLevel;
    char logFilePath[IGS_MAX_PATH_LENGTH];
    int logNbOfEntries; //for fflush rotation
    
    //network
    bool network_allowIpc;
    bool network_allowInproc;
    int network_zyrePort;
    int network_hwmValue;
    unsigned int network_discoveryInterval;
    unsigned int network_agentTimeout;
    unsigned int network_publishingPort;
    unsigned int network_logStreamPort;
    bool network_shallRaiseFileDescriptorsLimit;
    bool externalStop;
    bool isFrozen;
    igs_freeze_callback_t *freezeCallbacks;
    igs_external_stop_calback_t *externalStopCalbacks;
    zhash_t *brokers;
    char *advertisedEndpoint;
    char *ourBrokerEndpoint;
    
    //security
    bool security_isEnabled;
    zactor_t *security_auth;
    zcert_t *security_cert;
    char *security_publicKeysDirectory;
    
    //performance
    size_t performanceMsgCounter;
    size_t performanceMsgCountTarget;
    size_t performanceMsgSize;
    int64_t performanceStart;
    int64_t performanceStop;
    
    //network monitor
    igs_monitor_t *monitor;
    igs_monitor_callback_t *monitorCallbacks;
    bool monitor_shallStartStopAgent;
    
    //initiated at start, cleaned at stop
    char *networkDevice;
    char *ipAddress;
    char *ourAgentEndpoint;
    
    //initiated at initLoop, cleaned at loop stop
    char *commandLine;
    char *replayChannel;
    char *callsChannel;
    igs_license_enforcement_t *licenseEnforcement;
    igs_timer_t *timers; //set manually, destroyed automatically
    int processId;
    char *network_ipcFolderPath;
    char *network_ipcFullPath;
    char *network_ipcEndpoint;
    igs_zyre_peer_t *zyrePeers;
    igs_zyre_callback_t *zyreCallbacks;
    igs_agent_t *agents;
    zhash_t *createdAgents;
    igs_remote_agent_t *remoteAgents; //those our agents subscribed to
    zactor_t *networkActor;
    zyre_t *node;
    zsock_t *publisher;
    zsock_t *ipcPublisher;
    zsock_t *inprocPublisher;
    zsock_t *logger;
    zloop_t *loop;
    
} igs_core_context_t;

/*
 The igs_agent structure hosts eveything specifically
 needed by an agent. It relies on the core_context
 for all the shared resources.
 */
typedef struct igs_agent {
    char *uuid;
    char *name;
    char *state;
    
    igs_core_context_t *context;
    
    igs_activate_calback_t *activateCallbacks;
    igs_agent_event_callback_t *agentEventCallbacks;
    
    //definition
    char *definitionPath;
    igs_definition_t* definition;
    
    //mapping
    char *mappingPath;
    igs_mapping_t *mapping;
    
    //network
    bool network_needToSendDefinitionUpdate;
    bool network_needToUpdateMapping;
    bool network_requestOutputsFromMappedAgents;
    
    bool isWholeAgentMuted;
    igs_mute_callback_t *muteCallbacks;
    
    UT_hash_handle hh;
} igs_agent_t;


//////////////////  SHARED FUNCTIONS  AND  VARIABLES //////////////////

//default context and agent
PUBLIC extern igs_core_context_t *coreContext;
PUBLIC extern igs_agent_t *coreAgent;
void core_initAgent(void);
void core_initContext(void);

//  definition
PUBLIC void definition_freeDefinition (igs_definition_t **definition);

//  mapping
PUBLIC void mapping_freeMapping (igs_mapping_t **map);
igs_mapping_element_t * mapping_createMappingElement(const char * input_name,
                                                 const char *agent_name,
                                                 const char* output_name);
unsigned long djb2_hash (unsigned char *str);
bool mapping_checkInputOutputCompatibility(igs_agent_t *agent, igs_iop_t *foundInput, igs_iop_t *foundOutput);

// model
const igs_iop_t* model_writeIOP (igs_agent_t *agent, const char *iopName, iop_t iopType, iopType_t valType, void* value, size_t size);
igs_iop_t* model_findIopByName(igs_agent_t *agent, const char* name, iop_t type);
char* model_getIOPValueAsString (igs_iop_t* iop); //returned value must be freed by user
void model_readWriteLock(void);
void model_readWriteUnlock(void);

// network
#define IGS_PRIVATE_CHANNEL "INGESCAPE_PRIVATE"
#define IGS_DEFAULT_AGENT_NAME "no_name"
igs_result_t network_publishOutput (igs_agent_t *agent, const igs_iop_t *iop);

// parser
PUBLIC igs_definition_t* parser_loadDefinition (const char* json_str);
PUBLIC igs_definition_t* parser_loadDefinitionFromPath (const char* file_path);
PUBLIC char* parser_export_definition (igs_definition_t* def);
PUBLIC char* parser_export_mapping(igs_mapping_t* mapp);
PUBLIC igs_mapping_t* parser_loadMapping (const char* json_str);
PUBLIC igs_mapping_t* parser_loadMappingFromPath (const char* load_file);

// admin
void admin_makeFilePath(const char *from, char *to, size_t size_of_to);
void admin_log(igs_agent_t *agent, igs_logLevel_t, const char *function, const char *format, ...)  CHECK_PRINTF (4);

//bus
void bus_zyreLock(void);
void bus_zyreUnlock(void);

//call
void call_freeCall(igs_call_t *t);
igs_result_t call_addValuesToArgumentsFromMessage(const char *name, igs_callArgument_t *arg, zmsg_t *msg);
igs_result_t call_copyArguments(igs_callArgument_t *source, igs_callArgument_t *destination);
void call_freeValuesInArguments(igs_callArgument_t *arg);

//license
#define ENABLE_LICENSE_ENFORCEMENT 1
#define MAX_NB_OF_AGENTS 50
#define MAX_NB_OF_IOP 1000
#define IGS_MAX_EXEC_DURATION_DURING_EVAL 300
#if !TARGET_OS_IOS
PUBLIC void license_cleanLicense(igs_core_context_t *context);
PUBLIC void license_readLicense(igs_core_context_t *context);
#endif

//agent
void agent_propagateAgentEvent(igs_agent_event_t event, const char *uuid, const char *name);

#ifdef __cplusplus
}
#endif

#endif /* ingescape_private_h */
