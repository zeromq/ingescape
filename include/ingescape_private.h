/*  =========================================================================
    ingescape private - private header defining private classes and functions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef ingescape_private_h
#define ingescape_private_h

#include <stdbool.h>
#include <string.h>
#include <zyre.h>

#include "uthash/uthash.h"
#include "uthash/utlist.h"

#if defined (__WINDOWS__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <iphlpapi.h>
    #define timegm _mkgmtime
#endif

#include "ingescape.h"
#include "igsagent.h"

#ifdef __cplusplus
extern "C" {
#endif

// strndup utility function (for availability on all platforms)
extern char *
s_strndup(const char *str, size_t chars);

//  Mutex macros
#if defined (__UNIX__)
typedef pthread_mutex_t igs_mutex_t;
#   define IGS_MUTEX_INIT(m)    pthread_mutex_init (&m, NULL)
#   define IGS_MUTEX_LOCK(m)    pthread_mutex_lock (&m)
#   define IGS_MUTEX_UNLOCK(m)  pthread_mutex_unlock (&m)
#   define IGS_MUTEX_DESTROY(m) pthread_mutex_destroy (&m)
#elif defined (__WINDOWS__)
typedef CRITICAL_SECTION igs_mutex_t;
#   define IGS_MUTEX_INIT(m)    InitializeCriticalSection (&m)
#   define IGS_MUTEX_LOCK(m)    EnterCriticalSection (&m)
#   define IGS_MUTEX_UNLOCK(m)  LeaveCriticalSection (&m)
#   define IGS_MUTEX_DESTROY(m) DeleteCriticalSection (&m)
#endif

typedef struct igs_core_context igs_core_context_t;

//////////////////  IOP/SERVICE STRUCTURES AND ENUMS   //////////////////

typedef struct igs_observe_wrapper{
    igsagent_iop_fn *callback_ptr;
    void* data;
    struct igs_observe_wrapper *prev;
    struct igs_observe_wrapper *next;
} igs_observe_wrapper_t;

typedef enum {
    IGS_CONSTRAINT_MIN = 0,
    IGS_CONSTRAINT_MAX,
    IGS_CONSTRAINT_RANGE,
    IGS_CONSTRAINT_REGEXP
} igs_constraint_type_t;

typedef struct igs_constraint{
    igs_constraint_type_t type;
    union {
        struct {
            int min;
        } min_int;
        struct {
            int max;
        } max_int;
        struct {
            int min;
            int max;
        } range_int;
        struct {
            double min;
        } min_double;
        struct {
            double max;
        } max_double;
        struct {
            double min;
            double max;
        } range_double;
        struct {
            zrex_t *rex;
            char *string;
        } regexp;
    };
    struct igs_constraint *prev;
    struct igs_constraint *next;
} igs_constraint_t;

typedef struct igs_iop{
    char* name;
    char *description;
    igs_iop_value_type_t value_type;
    igs_iop_type_t type;
    union {
        int i;
        double d;
        char* s;
        bool b;
        void* data;
    } value;
    size_t value_size;
    bool is_muted;
    igs_observe_wrapper_t *callbacks;
    igs_constraint_t *constraint;
    UT_hash_handle hh;         /* makes this structure hashable */
} igs_iop_t;

typedef struct igs_service{
    char * name;
    char * description;
    igsagent_service_fn *cb;
    void *cb_data;
    igs_service_arg_t *arguments;
    struct igs_service *replies;
    UT_hash_handle hh;
} igs_service_t;

typedef struct igs_definition{
    char* name;
    char* family;
    char* description;
    char* version;
    igs_iop_t* params_table;
    igs_iop_t* inputs_table;
    igs_iop_t* outputs_table;
    igs_service_t *services_table;
} igs_definition_t;

typedef struct igs_map{
    uint64_t id;
    char* from_input;
    char* to_agent;
    char* to_output;
    UT_hash_handle hh;
} igs_map_t;

typedef struct igs_split{
    uint64_t id;
    char* from_input;
    char* to_agent;
    char* to_output;
    UT_hash_handle hh;
} igs_split_t;

typedef struct igs_mapping{
    igs_map_t* map_elements;
    igs_split_t* split_elements;
} igs_mapping_t;

typedef struct igs_mapping_filter {
    char *filter;
    struct igs_mapping_filter *next, *prev;
} igs_mapping_filter_t;

typedef struct igs_worker{
    char *input_name;
    char *agent_uuid;
    int credit;
    struct igs_worker *next;
    int uses;
}igs_worker_t;

typedef struct igs_queued_works{
    igs_iop_value_type_t value_type;
    union {
        int i;
        double d;
        char* s;
        bool b;
        void* data;
    } value;
    size_t value_size;
    struct igs_queued_works *next;
}igs_queued_work_t;

typedef struct igs_splitter{
    char *agent_uuid;
    char *output_name;
    igs_worker_t *workers_list;
    igs_queued_work_t *queued_works;
    struct igs_splitter *next;
}igs_splitter_t;

//////////////////  NETWORK  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_zyre_peer {
    char *peer_id;
    char *name;
    zsock_t *subscriber; //link to the peer's publisher socket
    int reconnected;
    bool has_joined_private_channel;
    char *protocol;
    UT_hash_handle hh;
} igs_zyre_peer_t;

// remote agent we are subscribing to
typedef struct igs_remote_agent{
    char *uuid;
    igs_zyre_peer_t *peer;
    igs_core_context_t *context;
    igs_definition_t *definition;
    bool shall_send_outputs_request;
    igs_mapping_t *mapping;
    igs_mapping_filter_t *mapping_filters;
    int timer_id;
    UT_hash_handle hh;
} igs_remote_agent_t;

typedef struct igs_timer{
    int timer_id;
    igs_timer_fn *cb;
    void *my_data;
    UT_hash_handle hh;
} igs_timer_t;

typedef struct igs_peer_header {
    char *key;
    char *value;
    UT_hash_handle hh;
} igs_peer_header_t;


//////////////////  UTILITY  STRUCTURES  //////////////////

typedef struct igs_observe_monitor {
    unsigned int period;
    igs_monitor_event_t status;
    zactor_t *monitor_actor;
    zloop_t *loop;
    unsigned int port;
    char *network_device;
} igs_monitor_t;

typedef struct igs_monitor_wrapper {
    igs_monitor_fn *callback_ptr;
    void *my_data;
    struct igs_monitor_wrapper *prev;
    struct igs_monitor_wrapper *next;
} igs_monitor_wrapper_t;

typedef struct igs_mute_wrapper {
    igsagent_mute_fn *callback_ptr;
    void *my_data;
    struct igs_mute_wrapper *prev;
    struct igs_mute_wrapper *next;
} igs_mute_wrapper_t;

typedef struct igs_freeze_wrapper {
    igs_freeze_fn *callback_ptr;
    void *my_data;
    struct igs_freeze_wrapper *prev;
    struct igs_freeze_wrapper *next;
} igs_freeze_wrapper_t;

typedef struct igs_channels_wrapper {
    igs_channels_fn *callback_ptr;
    void *my_data;
    struct igs_channels_wrapper *prev;
    struct igs_channels_wrapper *next;
} igs_channels_wrapper_t;

typedef struct igs_forced_stop_wrapper {
    igs_forced_stop_fn *callback_ptr;
    void *my_data;
    struct igs_forced_stop_wrapper *prev;
    struct igs_forced_stop_wrapper *next;
} igs_forced_stop_wrapper_t;

typedef struct igsagent_wrapper {
    igsagent_fn *callback_ptr;
    void *my_data;
    struct igsagent_wrapper *prev;
    struct igsagent_wrapper *next;
} igsagent_wrapper_t;

typedef struct igs_agent_event_wrapper {
    igsagent_agent_events_fn *callback_ptr;
    void *my_data;
    struct igs_agent_event_wrapper *prev;
    struct igs_agent_event_wrapper *next;
} igs_agent_event_wrapper_t;


//////////////////  MAIN  STRUCTURES   //////////////////

/*
 The igs_agent structure hosts eveything specifically
 needed by an agent. It relies on the core_context
 for all the shared resources.
 */
struct _igsagent_t {
    char *uuid;
    char *state;

    /*
     The concept of virtual agent is used by igs_proxy. Virtual
     agents represent n existing agent which is executed somewhere
     else. Thus virtual agents should not propagate published
     outputs or call services on other agents in the same context (i.e.
     all represented inside the same igs_proxy instance).
     */
    bool is_virtual;

    igs_core_context_t *context;
    char *igs_channel;

    igsagent_wrapper_t *activate_callbacks;
    igs_agent_event_wrapper_t *agent_event_callbacks;
    bool enforce_constraints;

    // definition
    char *definition_path;
    igs_definition_t* definition;

    // mapping
    char *mapping_path;
    igs_mapping_t *mapping;

    //network
    bool network_need_to_send_definition_update;
    bool network_need_to_send_mapping_update;
    bool network_request_outputs_from_mapped_agents;
    bool network_activation_during_runtime;

    bool is_whole_agent_muted;
    igs_mute_wrapper_t *mute_callbacks;

    zlist_t *elections;

    UT_hash_handle hh;
};

/*
 The core context hosts eveything needed by an agent or
 a set of agents at a process level.
 */
typedef struct igs_core_context {

    ////////////////////////////////////////////
    // persisting data with setters and getters or
    // managed automatically
    //
    // channels
    igs_peer_header_t *peer_headers;

    // admin
    FILE *log_file;
    bool log_in_stream;
    bool log_in_file;
    bool log_in_console;
    bool log_in_syslog;
    bool use_color_in_console;
    bool enable_data_logging;
    bool enable_service_logging;
    igs_log_level_t log_level;
    igs_log_level_t log_file_level;
    size_t log_file_max_line_length;
    char log_file_path[IGS_MAX_PATH_LENGTH];
    int log_nb_of_entries; //for fflush rotation
    
    //model
    bool allow_undefined_services;

    // network
    bool network_allow_ipc;
    bool network_allow_inproc;
    int network_zyre_port;
    int network_hwm_value;
    unsigned int network_discovery_interval;
    unsigned int network_agent_timeout;
    unsigned int network_publishing_port;
    unsigned int network_log_stream_port;
    bool network_shall_raise_file_descriptors_limit;
    bool external_stop;
    bool is_frozen;
    igs_freeze_wrapper_t *freeze_callbacks;
    igs_forced_stop_wrapper_t *external_stop_calbacks;
    zhash_t *brokers;
    char *advertised_endpoint;
    char *our_broker_endpoint;

    // security
    bool security_is_enabled;
    zactor_t *security_auth;
    zcert_t *security_cert;
    char *security_public_certificates_directory;

    // performance
    size_t performance_msg_counter;
    size_t performance_msg_count_target;
    size_t performance_msg_size;
    int64_t performance_start;
    int64_t performance_stop;

    // network monitor
    igs_monitor_t *monitor;
    igs_monitor_wrapper_t *monitor_callbacks;
    bool monitor_shall_start_stop_agent;

    // elections
    zhash_t *elections;

    // initiated at start, cleaned at stop
    char *network_device;
    char *ip_address;
    char *our_agent_endpoint;

    // initiated at s_init_loop, cleaned at loop stop
    char *command_line;
    char *replay_channel;
    igs_timer_t *timers; // set manually, destroyed automatically
    int process_id;
    char *network_ipc_folder_path;
    char *network_ipc_full_path;
    char *network_ipc_endpoint;
    igs_zyre_peer_t *zyre_peers;
    igs_channels_wrapper_t *zyre_callbacks;
    igsagent_t *agents;
    zhash_t *created_agents;
    igs_remote_agent_t *remote_agents; // those our agents subscribed to
    igs_splitter_t *splitters;
    zactor_t *network_actor;
    zsock_t *internal_pipe;
    zyre_t *node;
    zsock_t *publisher;
    zsock_t *ipc_publisher;
    zsock_t *inproc_publisher;
    zsock_t *logger;
    zloop_t *loop;

} igs_core_context_t;



//////////////////  SHARED FUNCTIONS  AND  VARIABLES //////////////////

// default context and agent
INGESCAPE_EXPORT extern igs_core_context_t *core_context;
INGESCAPE_EXPORT extern igsagent_t *core_agent;
void core_init_agent(void);
void core_init_context(void);

// definition
INGESCAPE_EXPORT void definition_free_definition (igs_definition_t **definition);
INGESCAPE_EXPORT void definition_free_constraint (igs_constraint_t **constraint);

// mapping
INGESCAPE_EXPORT void mapping_free_mapping (igs_mapping_t **map);
igs_map_t* mapping_create_mapping_element(const char * from_input,
                                          const char *to_agent,
                                          const char* to_output);
INGESCAPE_EXPORT bool mapping_is_equal(const char *first_str, const char *second_str);

uint64_t s_djb2_hash (unsigned char *str);
bool mapping_check_input_output_compatibility(igsagent_t *agent, igs_iop_t *found_input, igs_iop_t *found_output);

// split
void split_free_split_element (igs_split_t **split_elmt);
igs_split_t* split_create_split_element(const char * from_input,
                                        const char *to_agent,
                                        const char* to_output);
void split_add_work_to_queue(igs_core_context_t *context, char* agent_uuid, const igs_iop_t *output);
void split_remove_worker(igs_core_context_t *context, char *worker_uuid, char *input_name);
int split_message_from_worker(char *command, zmsg_t *msg, igs_core_context_t *context);
int split_message_from_splitter(zmsg_t *msg, igs_core_context_t *context);

// model
uint8_t* s_model_string_to_bytes (char* string);
const igs_iop_t* model_write_iop (igsagent_t *agent, const char *iop_name, igs_iop_type_t type,
                                  igs_iop_value_type_t val_type, void* value, size_t size);
igs_iop_t* model_find_iop_by_name(igsagent_t *agent, const char* name, igs_iop_type_t type);
char* model_get_iop_value_as_string (igs_iop_t* iop); //caller owns returned value
#define IGS_MODEL_READ_WRITE_MUTEX_DEBUG 0
INGESCAPE_EXPORT void model_read_write_lock(const char *function, int line);
INGESCAPE_EXPORT void model_read_write_unlock(const char *function, int line);
igs_constraint_t* s_model_parse_constraint(igs_iop_value_type_t type,
                                           const char *expression,char **error);

// network
#define IGS_PRIVATE_CHANNEL "INGESCAPE_PRIVATE"
#define IGS_DEFAULT_AGENT_NAME "no_name"
igs_result_t network_publish_output (igsagent_t *agent, const igs_iop_t *iop);

// parser
INGESCAPE_EXPORT igs_definition_t *parser_parse_definition_from_node (igs_json_node_t **json);
INGESCAPE_EXPORT igs_definition_t* parser_load_definition (const char* json_str);
INGESCAPE_EXPORT igs_definition_t* parser_load_definition_from_path (const char* file_path);
INGESCAPE_EXPORT char* parser_export_definition(igs_definition_t* def);
INGESCAPE_EXPORT char* parser_export_definition_legacy(igs_definition_t* def);
INGESCAPE_EXPORT char* parser_export_mapping(igs_mapping_t* mapping);
INGESCAPE_EXPORT char* parser_export_mapping_legacy(igs_mapping_t* mapping);
INGESCAPE_EXPORT igs_mapping_t* parser_load_mapping (const char* json_str);
INGESCAPE_EXPORT igs_mapping_t* parser_load_mapping_from_path (const char* load_file);

// admin
void s_admin_make_file_path(const char *from, char *to, size_t size_of_to);
void admin_log(igsagent_t *agent, igs_log_level_t, const char *function, const char *format, ...)  CHECK_PRINTF (4);

// channels
#define IGS_ZYRE_PEER_MUTEX_DEBUG 0
INGESCAPE_EXPORT void s_lock_zyre_peer(const char *function, int line);
INGESCAPE_EXPORT void s_unlock_zyre_peer(const char *function, int line);

// service
void service_free_service(igs_service_t *t);
INGESCAPE_EXPORT igs_result_t service_add_values_to_arguments_from_message(const char *name, igs_service_arg_t *arg, zmsg_t *msg);
igs_result_t service_copy_arguments(igs_service_arg_t *source, igs_service_arg_t *destination);
void service_free_values_in_arguments(igs_service_arg_t *arg);
void service_log_received_service(igsagent_t *agent, const char *caller_agent_name, const char *caller_agentuuid,
                                  const char *service_name, igs_service_arg_t *list);

// agent
void s_agent_propagate_agent_event(igs_agent_event_t event, const char *uuid, const char *name, void *event_data);

// protocol messages
#define REMOTE_AGENT_EXIT_MSG "REMOTE_AGENT_EXIT"
#define REMOTE_PEER_KNOWS_AGENT_MSG "REMOTE_PEER_KNOWS_AGENT"

#define EXTERNAL_DEFINITION_MSG "EXTERNAL_DEFINITION#"
#define EXTERNAL_MAPPING_MSG "EXTERNAL_MAPPING#"

#define LOAD_DEFINITION_MSG "LOAD_THIS_DEFINITION#"
#define LOAD_MAPPING_MSG "LOAD_THIS_MAPPING#"

#define GET_CURRENT_OUTPUTS_MSG "GET_CURRENT_OUTPUTS"
#define CURRENT_OUTPUTS_MSG "CURRENT_OUTPUTS"
#define GET_CURRENT_INPUTS_MSG "GET_CURRENT_INPUTS"
#define CURRENT_INPUTS_MSG "CURRENT_INPUTS"
#define GET_CURRENT_PARAMETERS_MSG "GET_CURRENT_PARAMETERS"
#define CURRENT_PARAMETERS_MSG "CURRENT_PARAMETERS"

#define STATE_MSG "STATE"

#define START_AGENT_MSG "START_AGENT"
#define STOP_AGENT_MSG "STOP_AGENT"
#define STOP_PEER_MSG "STOP_PEER"

#define FREEZE_MSG "FREEZE"
#define UNFREEZE_MSG "UNFREEZE"
#define FROZEN_MSG "FROZEN"

#define MUTE_ALL_MSG "MUTE_ALL"
#define UNMUTE_ALL_MSG "UNMUTE_ALL"

#define MUTE_AGENT_MSG "MUTE_AGENT"
#define UNMUTE_AGENT_MSG "UNMUTE_AGENT"
#define AGENT_MUTED_MSG "MUTED"

#define MUTE_OUTPUT_MSG "MUTE"
#define UNMUTE_OUTPUT_MSG "UNMUTE"
#define OUTPUT_MUTED_MSG "OUTPUT_MUTED"
#define OUTPUT_UNMUTED_MSG "OUTPUT_UNMUTED"

#define SET_INPUT_MSG "SET_INPUT"
#define SET_OUTPUT_MSG "SET_OUTPUT"
#define SET_PARAMETER_MSG "SET_PARAMETER"
#define CALL_SERVICE_MSG "SERVICE"
#define CALL_SERVICE_MSG_DEPRECATED "CALL" // DEPRECATED since ingescape 3.0 that uses protocol v4


#define MAP_MSG "MAP"
#define UNMAP_MSG "UNMAP"
#define CLEAR_MAPPING_MSG "CLEAR_MAPPING"

#define ADD_SPLIT_ENTRY_MSG "ADD_SPLIT"
#define REMOVE_SPLIT_ENTRY_MSG "REMOVE_SPLIT"

#define WORKER_HELLO_MSG "WORKER_HELLO"
#define WORKER_GOODBYE_MSG "WORKER_GOODBYE"
#define WORKER_READY_MSG "WORKER_READY"
#define SPLITTER_WORK_MSG "SPLITTER_WORK"

#define SET_DEFINITION_PATH_MSG "SET_DEFINITION_PATH"
#define DEFINITION_FILE_PATH_MSG "DEFINITION_FILE_PATH"
#define SAVE_DEFINITION_TO_PATH_MSG "SAVE_DEFINITION_TO_PATH"

#define SET_MAPPING_PATH_MSG "SET_MAPPING_PATH"
#define MAPPING_FILE_PATH_MSG "MAPPING_FILE_PATH"
#define SAVE_MAPPING_TO_PATH_MSG "SAVE_MAPPING_TO_PATH"

#define ENABLE_LOG_STREAM_MSG "ENABLE_LOG_STREAM"
#define DISABLE_LOG_STREAM_MSG "DISABLE_LOG_STREAM"
#define LOG_IN_STREAM_MSG "LOG_IN_STREAM"

#define ENABLE_LOG_FILE_MSG "ENABLE_LOG_FILE"
#define DISABLE_LOG_FILE_MSG "DISABLE_LOG_FILE"
#define LOG_IN_FILE_MSG "LOG_IN_FILE"

#define SET_LOG_PATH_MSG "SET_LOG_PATH"
#define LOG_FILE_PATH_MSG "LOG_FILE_PATH"

#define PING_MSG "PING"
#define PONG_MSG "PONG"

#ifdef __cplusplus
}
#endif

#endif /* ingescape_private_h */
