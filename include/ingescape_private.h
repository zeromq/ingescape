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

typedef enum {
    IGS_TIMESTAMPED_INTEGER_T = IGS_DATA_T + 1,
    IGS_TIMESTAMPED_DOUBLE_T,
    IGS_TIMESTAMPED_STRING_T,
    IGS_TIMESTAMPED_BOOL_T,
    IGS_TIMESTAMPED_IMPULSION_T,
    IGS_TIMESTAMPED_DATA_T
} igs_io_value_type_extended_t;

//////////////////  IOP/SERVICE STRUCTURES AND ENUMS   //////////////////

typedef struct igs_observe_wrapper{
    igsagent_io_fn *callback_ptr;
    void* data;
} igs_observe_io_wrapper_t;

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
} igs_constraint_t;

typedef struct igs_io{
    char* name;
    char *description;
    char *detailed_type;
    char *specification;
    igs_io_value_type_t value_type;
    igs_io_type_t type;
    union {
        int i;
        double d;
        char* s;
        bool b;
        void* data;
    } value;
    size_t value_size;
    bool is_muted;
    zlist_t *io_callbacks; //igs_observe_io_wrapper_t
    igs_constraint_t *constraint;
} igs_io_t;

typedef struct igs_service{
    char * name;
    char * description;
    igsagent_service_fn *service_cb;
    void *cb_data;
    igs_service_arg_t *arguments;
    zlist_t *replies_names_ordered; // char*, to keep insertion order
    zhashx_t *replies; //struct igs_service
} igs_service_t;

typedef struct igs_definition{
    char* my_class;
    char* package;
    char* name;
    char* description;
    char* version;
    char *json;
    char *json_legacy_v3;
    char *json_legacy_v4;
    char* family;
    zlist_t* attributes_names_ordered; // char*, to keep insertion order
    zhashx_t* attributes_table; //igs_io_t
    zlist_t* inputs_names_ordered; // char*, to keep insertion order
    zhashx_t* inputs_table; //igs_io_t
    zlist_t* outputs_names_ordered; // char*, to keep insertion order
    zhashx_t* outputs_table; //igs_io_t
    zlist_t* services_names_ordered; // char*, to keep insertion order
    zhashx_t *services_table; //igs_service_t
} igs_definition_t;

typedef struct igs_map{
    uint64_t id;
    char* from_input;
    char* to_agent;
    char* to_output;
} igs_map_t;

typedef struct igs_split{
    uint64_t id;
    char* from_input;
    char* to_agent;
    char* to_output;
} igs_split_t;

typedef struct igs_mapping{
    char *json;
    char *json_legacy;
    zlist_t* map_elements; //igs_map_t
    zlist_t* split_elements; //igs_split_t
} igs_mapping_t;

typedef struct igs_mapping_filter {
    char *filter;
} igs_mapping_filter_t;

typedef struct igs_worker{
    char *input_name;
    char *agent_uuid;
    int credit;
    int uses;
}igs_worker_t;

typedef struct igs_queued_works{
    igs_io_value_type_t value_type;
    union {
        int i;
        double d;
        char* s;
        bool b;
        void* data;
    } value;
    size_t value_size;
}igs_queued_work_t;

typedef struct igs_splitter{
    char *agent_uuid;
    char *output_name;
    zlist_t *workers; //igs_worker_t
    zlist_t *queued_works; //igs_queued_work_t
}igs_splitter_t;

//////////////////  NETWORK  STRUCTURES AND ENUMS   //////////////////

typedef struct igs_zyre_peer {
    char *peer_id;
    char *name;
    zsock_t *subscriber; //link to the peer's publisher socket
    int reconnected;
    bool has_joined_private_channel;
    char *protocol;
} igs_zyre_peer_t;

// remote agent we are subscribing to
typedef struct igs_remote_agent{
    char *uuid;
    igs_zyre_peer_t *peer;
    igs_core_context_t *context;
    igs_definition_t *definition;
    bool shall_send_outputs_request;
    igs_mapping_t *mapping;
    zlist_t *mapping_filters; //igs_mapping_filter_t
    int timer_id;
} igs_remote_agent_t;

typedef struct igs_timer{
    int timer_id;
    igs_timer_fn *cb;
    void *my_data;
} igs_timer_t;


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
} igs_monitor_wrapper_t;

typedef struct igs_mute_wrapper {
    igsagent_mute_fn *callback_ptr;
    void *my_data;
} igs_mute_wrapper_t;

typedef struct igs_freeze_wrapper {
    igs_freeze_fn *callback_ptr;
    void *my_data;
} igs_freeze_wrapper_t;

typedef struct igs_channels_wrapper {
    igs_channels_fn *callback_ptr;
    void *my_data;
} igs_channels_wrapper_t;

typedef struct igs_forced_stop_wrapper {
    igs_forced_stop_fn *callback_ptr;
    void *my_data;
} igs_forced_stop_wrapper_t;

typedef struct igsagent_wrapper {
    igsagent_fn *callback_ptr;
    void *my_data;
} igsagent_wrapper_t;

typedef struct igs_agent_event_wrapper {
    igsagent_agent_events_fn *callback_ptr;
    void *my_data;
} igs_agent_event_wrapper_t;

//////////////////  CALLBACK WRAPPERS   //////////////////
typedef struct observe_io_cb_wrapper
{
    igs_io_fn *cb;
    void *my_data;
} observe_io_cb_wrapper_t;

typedef struct observed_io
{
    char *name;
    zlist_t *observed_io_wrappers;
} observed_io_t;

typedef struct
{
    char *name;
    igs_service_fn *cb;
    void *my_data;
} service_cb_wrapper_t;

typedef struct observe_mute_cb_wrapper
{
    igs_mute_fn *cb;
    void *my_data;
} observe_mute_cb_wrapper_t;

typedef struct observe_agent_events_cb_wrapper
{
    igs_agent_events_fn *cb;
    void *my_data;
} observe_agent_events_cb_wrapper_t;


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
     The concept of virtual agent is used by igsProxy. A virtual
     agent represents an existing agent which is executed somewhere
     else. Thus virtual agents should not propagate published
     outputs or call services on other agents in the same context (i.e.
     all represented inside the same igsProxy instance).
     */
    bool is_virtual;

    igs_core_context_t *context;
    char *igs_channel;

    zlist_t *activate_callbacks; //igsagent_wrapper_t
    zlist_t *agent_event_callbacks; //igs_agent_event_wrapper_t
    bool enforce_constraints;

    // definition
    char *definition_path;
    igs_definition_t* definition;

    // mapping
    char *mapping_path;
    igs_mapping_t *mapping;

    //real-time
    bool rt_timestamps_enabled;
    int64_t rt_current_timestamp_microseconds;
    bool rt_synchronous_mode_enabled;
    
    //network
    bool network_need_to_send_definition_update;
    bool network_need_to_send_mapping_update;
    bool network_request_outputs_from_mapped_agents;
    bool network_activation_during_runtime;

    bool is_whole_agent_muted;
    zlist_t *mute_callbacks; //igs_mute_wrapper_t

    zlist_t *elections;
};

/*
 The core context hosts eveything needed by an agent or
 a set of agents at a process level.
 */
typedef struct igs_core_context {

    ////////////////////////////////////////////
    // persisting data with setters and getters or
    // managed automatically
    
    // channels
    zhash_t *peer_headers; //igs_peer_header_t

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
    zhashx_t *observed_inputs; //observed_io_t
    zhashx_t *observed_outputs; //observed_io_t
    zhashx_t *observed_attributes; //observed_io_t
    zhashx_t *service_cb_wrappers; //service_cb_wrapper_t
    zlist_t *mute_cb_wrappers; //observe_mute_cb_wrapper_t
    zlist_t *agent_event_cb_wrappers; //observe_agent_events_cb_wrapper_t
    zlist_t *freeze_callbacks; //igs_freeze_wrapper_t
    zlist_t *external_stop_calbacks; //igs_forced_stop_wrapper_t

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
    zhash_t *brokers;
    char *advertised_endpoint;
    char *our_broker_endpoint;
    
    //real-time : current time is common to all agents in the peer
    int64_t rt_current_microseconds;

    // security
    bool security_is_enabled;
    zactor_t *security_auth;
    zcert_t *security_cert;
    char *security_public_certificates_directory;

    // performance
    bool unbind_pipe; //removes HWM on PAIR pipe between main thread and ingescape thread
    bool monitor_pipe_stack; //counts piled messages in the pipe in real-time
    size_t performance_msg_counter;
    size_t performance_msg_count_target;
    size_t performance_msg_size;
    int64_t performance_start;
    int64_t performance_stop;

    // network monitor
    igs_monitor_t *monitor;
    zlist_t *monitor_callbacks; //igs_monitor_wrapper_t
    bool monitor_shall_start_stop_agent;

    // elections
    zhashx_t *elections;

    // initiated at start, cleaned at stop
    char *network_device;
    char *ip_address;
    char *our_agent_endpoint;

    // initiated at s_init_loop, cleaned at loop stop
    char *command_line;
    char *replay_channel;
    zlist_t *timers; // igs_timer_t
    int process_id;
    char *network_ipc_folder_path;
    char *network_ipc_full_path;
    char *network_ipc_endpoint;
    zhashx_t *zyre_peers; //igs_zyre_peer_t
    zlist_t *zyre_callbacks; //igs_channels_wrapper_t
    zhashx_t *agents; //igsagent_t, all active agents we own
    zhashx_t *created_agents; //igsagent_t, all created agents we own (some may be inactive)
    zhashx_t *remote_agents; //igs_remote_agent_t, all known external agents
    zlist_t *splitters; //igs_splitter_t
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

/*
 These two functions respectively initiate the context and the defautl agent.
 The context is necessary for all agents. And the default agent, named core_agent,
 shall be created to make the high-level API usable. It is already possible to
 create several independent agents. Future evolutions could enable the creation
 of several independent contexts but, at the moment, it is easier and safer
 to create separate agents instead.
 core_init_context is called at the beginning of core functions to lazily initiate
 the contexte, whereas core_init_agent is called at the beginning of the high-level
 API functions to initiate both the context and the default agent.
 */
INGESCAPE_EXPORT void core_init_context(void);
INGESCAPE_EXPORT void core_init_agent(void);

// definition
INGESCAPE_EXPORT void definition_free_definition (igs_definition_t **definition);
INGESCAPE_EXPORT void definition_free_constraint (igs_constraint_t **constraint);
INGESCAPE_EXPORT void definition_update_json (igs_definition_t *definition);

// mapping
INGESCAPE_EXPORT void mapping_free_mapping (igs_mapping_t **map);
INGESCAPE_EXPORT igs_map_t* mapping_create_mapping_element(const char * from_input,
                                                           const char *to_agent,
                                                           const char* to_output);
INGESCAPE_EXPORT igs_split_t* mapping_create_split_element(const char * from_input,
                                                         const char *to_agent,
                                                         const char* to_output);
INGESCAPE_EXPORT bool mapping_is_equal(const char *first_str, const char *second_str);
INGESCAPE_EXPORT uint64_t mapping_djb2_hash (unsigned char *str);
INGESCAPE_EXPORT bool mapping_check_input_output_compatibility(igsagent_t *agent, igs_io_t *found_input, igs_io_t *found_output);
INGESCAPE_EXPORT void mapping_update_json (igs_mapping_t *mapping);

// split
/*
 Splits are a load balancing mechanism replacing classic dataflows in Ingescape.
 Theyt create specific connections between inputs and outputs, relying on credits
 and availability, so that publications on an ouput are split (or balanced) between
 receiving agents (on one or several on their inputs).
 
 Splits are declarations similar to mapping links, involving an input of an agent
 and an output of a remote agent. Splitters are structures on a publishing agent,
 orchestrating publications on its outputs to actually balance them. This balance
 is achieved between a set of declared Workers, which are remote agents, ready
 to receive publications on their inputs, using the balancing mechanism, instead
 of classic dataflow. A Splitter and its Workers use a mechanism of credits and
 number of uses to define the available workers and the one that should be used
 in priority. A Splitter stores a list of Works to be distributed between the
 available Workers depending on the defined splits.
 
 Splits are created either using the API, a mapping file or instuctions sent
 through private messages (coming from Circle, using ADD_SPLIT_ENTRY_MSG). They
 are then part of the model and stored in agent->mapping->split_elements.
 
 If a split is created  while the agent is started, a WORKER_HELLO_MSG is sent
 immeditaley to relevant remote agents (i.e. remote agents invovled in the splitter).
 In the mean time, when a remote agent enters, if it is involed in a split, it also
 receives a WORKER_HELLO_MSG.
 
 Upon receiving a WORKER_HELLO_MSG, an agent calls s_split_add_credit_to_worker,
 which searches for or creates a splitter if needed. The splitter is dedicated
 to one of our outputs. Then a worker is searched for or created and receives
 new credits, meaning it is available for work. Workers are dedicated to the
 relation with a specific input of a remote agent involved in the split.
 For an output of our agent are a set of Splitters. For an input of a remote
 agent are a set of Workers.
 
 At the end of the call to s_split_add_credit_to_worker,
 s_split_trigger_send_message_to_worker is called for our output. This call
 iterates on our Splitters, Workers and Works, using the credits and number
 of uses to execute the Works with the best available Workers. It sends a
 SPLITTER_WORK_MSG to the remote agents corresponding to the selected Worker
 for a given Work, i.e. a publication with its value type and data.
 
 s_split_trigger_send_message_to_worker is called by :
 - s_split_add_credit_to_worker (upon receiving WORKER_HELLO_MSG or WORKER_READY_MSG),
 - split_add_work_to_queue called from network_publish_output
 
 Upon receiving SPLITTER_WORK_MSG, split_message_from_splitter is called.
 This function writes the received data on our proper input. It then sends
 a WORKER_READY_MSG to the sender, indicating that we are ready for new
 Work.
 
 Upon receiving WORKER_READY_MSG, our agent calls s_split_add_credit_to_worker,
 which runs again the machanism to send our Works to available Workers.
 
 WORKER_GOODBYE_MSG is sent when an agent removes a given Split.
 Upon receiving WORKER_GOODBYE_MSG, split_remove_worker is called
 with the effect of removing our Workers for this agent in all our
 Splitters. NB: if a Splitter looses its last Worker, it is destroyed
 as well and so are the pending Works. The Splitter will be recreated
 if a new Worker appears.
 
 NB: s_split_trigger_send_message_to_worker contains a call to
 igs_channel_whisper_zmsg which needs to bu unclocked. At the moment,
 all the calls to s_split_trigger_send_message_to_worker are done
 inside a lock. That is why s_split_trigger_send_message_to_worker
 unlkocks its call to igs_channel_whisper_zmsg. Future evolutions
 of the code need to check and maintain this.
 
 */
INGESCAPE_EXPORT void split_free_split_element (igs_split_t **split_elmt);
INGESCAPE_EXPORT void split_free_splitter (igs_splitter_t **splitter);
INGESCAPE_EXPORT igs_split_t* split_create_split_element(const char * from_input,
                                                         const char *to_agent,
                                                         const char* to_output);
INGESCAPE_EXPORT void split_add_work_to_queue(igs_core_context_t *context, char* agent_uuid, const igs_io_t *output);
INGESCAPE_EXPORT void split_remove_worker(igs_core_context_t *context, char *worker_uuid, char *input_name);
INGESCAPE_EXPORT int split_message_from_worker(char *command, zmsg_t *msg, igs_core_context_t *context);
INGESCAPE_EXPORT int split_message_from_splitter(zmsg_t *msg, igs_core_context_t *context);

// model
/*
 Note on thread safety and model mutexes
 ---------------------------------------
 Ingescape model thread-safety is protected by the two mutex functions below.
 The mutexes protect the model on all possible accesses and modifications involving:
 - public functions
 - readers and timers attached to zloops
 Public functions are prefixed either by igs_ or igsagent_. When an igs_
 function has an igsagent_ equivalent, the latter shall be protected because
 it wraps the former.
 Here is the list of reader and timer functions (which also include mutexes):
 - s_manage_parent: listening to parent thread messages and internal agent publications
 - s_manage_received_publication: listening to all SUB sockets receiving data
 - s_manage_zyre_incoming: listening to zyre incoming messages from other peers/agents
 - s_trigger_outputs_request_to_newcomer: asynchronous request to remote agent for its output values (reply received in s_manage_zyre_incoming)
 - s_trigger_definition_update: send our definition to peers when it has changed
 - s_trigger_mapping_update: send our mapping to peers when it has changed
 - s_manage_network_timer: execute callbacks for timers attached to the ingescape zloop
 
 Functions handling callbacks are the ones creating risks of deadlocks because they are reentrant
 and mutexes shall be unlocked when entering them. A specific care shall be given to check that
 their objects are not destroyed when used.
 Here is the list of the callback structures and functions actually executing them:
 - io_callbacks (in each io object)
    executed in model_write and model_LOCKED_handle_io_callbacks
 - service_cb (in each service object)
    - executed by s_manage_zyre_incoming (CALL_SERVICE_MSG), igsagent_service_call
 - igs_monitor_wrapper_t (monitor_callbacks)
    - executed by igs_monitor_trigger_network_check (#core_context->network_device)
 - igs_mute_wrapper_t (mute_callbacks)
    - executed by igsagent_mute, igsagent_unmute (#current agent)
 - igs_freeze_wrapper_t (freeze_callbacks)
    - executed by igs_freeze, igs_unfreeze (#core_context->is_frozen)
 - igs_channels_wrapper_t (zyre_callbacks)
    - executed by s_manage_zyre_incoming (#nothing)
 - igs_forced_stop_wrapper_t (external_stop_calbacks)
    - executed by s_run_loop (loop stopping) (#nothing)
 - igsagent_wrapper_t (activate_callbacks)
    igsagent_activate, igsagent_deactivate (#current agent)
 - igs_agent_event_wrapper_t (agent_event_callbacks)
    s_manage_zyre_incoming (KNOWS_US, WON_ELECTION, LOST_ELECTION) (#current agent), igsagent_activate (#current agent)
        agent_LOCKED_propagate_agent_event (#core_context->agents to find the proper agent, might be improved to focus on specific agent): s_manage_zyre_incoming (PEER_ENTERED, AGENT_EXITED,AGENT_ENTERED, KNOWS_US, UPDATE_DEF, UPDATE_MAP, AGENT_EXITED, PEER_EXITED), s_trigger_definition_update, s_trigger_mapping_update, igsagent_activate, igsagent_deactivate
 
 To improve thread-safety, despite the need to unlock callbacks, we always duplicate iterators involving
 call backs and we duplicate or check validity of involved arguments when required.
 
 
 context ->loop and ->node shall be protected by s_network_lock/s_network_unlock
 s_manage_network_timer shall be protected also, to be analyzed... possible reentry through the called callback
 */
#define IGS_MODEL_READ_WRITE_MUTEX_DEBUG 0
INGESCAPE_EXPORT void model_read_write_lock(const char *function, int line);
INGESCAPE_EXPORT void model_read_write_unlock(const char *function, int line);
INGESCAPE_EXPORT size_t model_clean_string(char *string, int64_t max); //returns number of changes
INGESCAPE_EXPORT bool model_check_string(const char *string, int64_t max); //false if invalid, no limit if max <= 0
INGESCAPE_EXPORT uint8_t *model_string_to_bytes (char *string);
INGESCAPE_EXPORT igs_io_t* model_write (igsagent_t *agent, const char *io_name, igs_io_type_t type,
                                        igs_io_value_type_t val_type, void* value, size_t size);
INGESCAPE_EXPORT void model_LOCKED_handle_io_callbacks (igsagent_t *agent, igs_io_t *io);
INGESCAPE_EXPORT igs_io_t* model_find_io_by_name(igsagent_t *agent, const char* name, igs_io_type_t type);
INGESCAPE_EXPORT igs_constraint_t* model_parse_constraint(igs_io_value_type_t type, const char *expression, char **error);

// network
#define IGS_PRIVATE_CHANNEL "INGESCAPE_PRIVATE"
#define IGS_DEFAULT_AGENT_NAME "no_name"
INGESCAPE_EXPORT igs_result_t network_publish_output (igsagent_t *agent, const igs_io_t *io);

// parser
INGESCAPE_EXPORT igs_definition_t *parser_parse_definition_from_node (igs_json_node_t **json);
INGESCAPE_EXPORT igs_definition_t* parser_load_definition (const char* json_str);
INGESCAPE_EXPORT igs_definition_t* parser_load_definition_from_path (const char* file_path);
INGESCAPE_EXPORT char* parser_export_definition(igs_definition_t* def);
INGESCAPE_EXPORT char* parser_export_definition_legacy_v4(igs_definition_t* def);
INGESCAPE_EXPORT char* parser_export_definition_legacy_v3(igs_definition_t* def);
INGESCAPE_EXPORT char* parser_export_mapping(igs_mapping_t* mapping);
INGESCAPE_EXPORT char* parser_export_mapping_legacy(igs_mapping_t* mapping);
INGESCAPE_EXPORT igs_mapping_t* parser_load_mapping (const char* json_str);
INGESCAPE_EXPORT igs_mapping_t* parser_load_mapping_from_path (const char* load_file);

// admin
INGESCAPE_EXPORT void admin_make_file_path(const char *from, char *to, size_t size_of_to);
INGESCAPE_EXPORT void admin_log(igsagent_t *agent, igs_log_level_t, const char *function, const char *format, ...)  CHECK_PRINTF (4);

// channels
#define IGS_ZYRE_PEER_MUTEX_DEBUG 0
INGESCAPE_EXPORT void s_lock_zyre_peer(const char *function, int line);
INGESCAPE_EXPORT void s_unlock_zyre_peer(const char *function, int line);

// service
INGESCAPE_EXPORT void service_free_service(igs_service_t **t);
INGESCAPE_EXPORT igs_result_t service_make_values_to_arguments_from_message(igs_service_arg_t **args,
                                                                            igs_service_t *service, zmsg_t *msg);
INGESCAPE_EXPORT void service_free_values_in_arguments(zlist_t *args);
INGESCAPE_EXPORT void service_log_received_service(igsagent_t *agent, const char *caller_agent_name, const char *caller_agentuuid,
                                                   const char *service_name, igs_service_arg_t *args, int64_t timestamp);

// agent
INGESCAPE_EXPORT void agent_LOCKED_propagate_agent_event(igs_agent_event_t event, const char *uuid, const char *name, void *event_data);

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
#define GET_CURRENT_ATTRIBUTES_MSG "GET_CURRENT_ATTRIBUTES"
#define CURRENT_ATTRIBUTES_MSG "CURRENT_ATTRIBUTES"
#define GET_CURRENT_ATTRIBUTES_MSG_DEPRECATED "GET_CURRENT_PARAMETERS"
#define CURRENT_ATTRIBUTES_MSG_DEPRECATED "CURRENT_PARAMETERS"

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
#define SET_ATTRIBUTE_MSG "SET_ATTRIBUTE"
#define SET_ATTRIBUTE_MSG_DEPRECATED "SET_PARAMETER"
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

#define RT_SET_TIME_MSG "RT_SET_TIME "

#ifdef __cplusplus
}
#endif

#endif /* ingescape_private_h */
