/*  =========================================================================
 ingescape - public library header

 Copyright (c) the Contributors as noted in the AUTHORS file.
 This file is part of Ingescape, see https://github.com/zeromq/ingescape.

 This Source Code Form is subject to the terms of the Mozilla Public
 License, v. 2.0. If a copy of the MPL was not distributed with this
 file, You can obtain one at http://mozilla.org/MPL/2.0/.
 =========================================================================
 */

#ifndef ingescape_h
#define ingescape_h

//External dependencies
#include <stdbool.h>
#include <stddef.h>
#include <czmq.h>
#include <zyre.h>

#ifndef CZMQ_BUILD_DRAFT_API
#define CZMQ_BUILD_DRAFT_API
#endif
#ifndef ZYRE_BUILD_DRAFT_API
#define ZYRE_BUILD_DRAFT_API
#endif
#if CZMQ_VERSION < 40202
#error "ingescape needs CZMQ/4.2.2 or later"
#endif

//  INGESCAPE version macros for compile-time API detection
#define INGESCAPE_VERSION_MAJOR 3
#define INGESCAPE_VERSION_MINOR 5
#define INGESCAPE_VERSION_PATCH 4

#define INGESCAPE_MAKE_VERSION(major, minor, patch) \
((major) * 10000 + (minor) * 100 + (patch))
#define INGESCAPE_VERSION \
INGESCAPE_MAKE_VERSION(INGESCAPE_VERSION_MAJOR, INGESCAPE_VERSION_MINOR, INGESCAPE_VERSION_PATCH)

#if defined (__WINDOWS__)
#   if defined INGESCAPE_FROM_PRI
#       define INGESCAPE_EXPORT
#   elif defined INGESCAPE_STATIC
#       define INGESCAPE_EXPORT
#   elif defined INGESCAPE_INTERNAL_BUILD
#       if defined DLL_EXPORT
#           define INGESCAPE_EXPORT __declspec(dllexport)
#       else
#           define INGESCAPE_EXPORT
#       endif
#   elif defined INGESCAPE_EXPORTS
#       define INGESCAPE_EXPORT __declspec(dllexport)
#   else
#       define INGESCAPE_EXPORT __declspec(dllimport)
#   endif
#   define INGESCAPE_PRIVATE
#elif defined (__CYGWIN__)
#   define INGESCAPE_EXPORT
#   define INGESCAPE_PRIVATE
#else
#   if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define INGESCAPE_PRIVATE __attribute__ ((visibility ("hidden")))
#       define INGESCAPE_EXPORT __attribute__ ((visibility ("default")))
#   else
#       define INGESCAPE_PRIVATE
#       define INGESCAPE_EXPORT
#   endif
#endif

// GCC and clang can validate format strings for functions that act like printf
// this is used to check the logging functions
#if defined (__GNUC__) && (__GNUC__ >= 2)
#   define CHECK_PRINTF(a)   __attribute__((format (printf, a, a + 1)))
#else
#   define CHECK_PRINTF(a)
#endif

//Macro to avoid "unused parameter" warnings
#define IGS_UNUSED(x) (void)x;

//Opaque class structures to allow forward references
//These classes are stable or legacy and built in all releases
typedef struct _igs_json_t igs_json_t;
typedef struct _igs_json_node_t igs_json_node_t;
typedef struct _igs_service_arg_t igs_service_arg_t;

#define IGS_MAX_PATH_LENGTH 4096             //
#define IGS_MAX_IOP_NAME_LENGTH 1024         //
#define IGS_AGENT_UUID_LENGTH 32             //
#define IGS_MAX_AGENT_NAME_LENGTH 1024       //
#define IGS_MAX_DESCRIPTION_LENGTH 4096      //
#define IGS_MAX_FAMILY_LENGTH 64             //
#define IGS_MAX_VERSION_LENGTH 64            //
#define IGS_MAX_LOG_LENGTH 4096              //
#define IGS_COMMAND_LINE_LENGTH 4096         //
#define IGS_NETWORK_DEVICE_LENGTH 1024       //
#define IGS_IP_ADDRESS_LENGTH 1024           //
#define IGS_MAX_PEER_ID_LENGTH 128           //
#define IGS_DEFAULT_IPC_FOLDER_PATH "/tmp/ingescape/"  //
#define IGS_MAX_STRING_MSG_LENGTH 4096       //
#define IGS_DEFAULT_WORKER_CREDIT 3          //
#define IGS_DEFAULT_LOG_DIR "~/Documents/Ingescape/logs/"  //

#ifdef __cplusplus
extern "C" {
#endif

typedef enum{
    IGS_SUCCESS = 0,
    IGS_FAILURE = -1
} igs_result_t;


///////////////////////////////////////////
// Agent initialization, control and events

//start & stop ingescape
INGESCAPE_EXPORT igs_result_t igs_start_with_device(const char *network_device, unsigned int port);
INGESCAPE_EXPORT igs_result_t igs_start_with_ip(const char *ip_address, unsigned int port);
INGESCAPE_EXPORT void igs_stop(void);
INGESCAPE_EXPORT bool igs_is_started(void);

/*Ingescape can be stopped either from the applications itself
 or from the network. When ingescape is stopped from the network,
 the application can be notified and take actions such as
 stopping, entering a specific mode, etc.

 To stop ingescape from its hosting application,
 just call igs_stop().

 To be notified that Ingescape has been stopped,you can:
 - read the pipe to ingescape and expect a "LOOP_STOPPED" message
 - register a callabck with igs_observe_forced_stop. WARNING: this
 callback will be executed from the ingescape thread with potential
 thread-safety issues depending on your application structure.
 - periodically check the value returned by igs_is_started()
 In any case, igs_stop() MUST NEVER BE CALLED directly from any
 Ingescape callback, because it would create a deadlock between
 the main thread and the ingescape thread.
 */
typedef void (igs_forced_stop_fn)(void *my_data);
INGESCAPE_EXPORT void igs_observe_forced_stop(igs_forced_stop_fn cb,
                                              void *my_data);
//zeromq pipe to receive stop event and other messages from the ingescape thread
INGESCAPE_EXPORT zsock_t * igs_pipe_to_ingescape(void);
//zeromq pipe to send messages from the ingescape thread, e.g. from inside ingescape callbacks
INGESCAPE_EXPORT zsock_t * igs_pipe_inside_ingescape(void);

//agent name set and get
INGESCAPE_EXPORT void igs_agent_set_name(const char *name);
INGESCAPE_EXPORT char * igs_agent_name(void); //caller owns returned value

//agent uuid, read-only
INGESCAPE_EXPORT const char * igs_agent_uuid(void); //caller owns returned value

//control agent state
INGESCAPE_EXPORT void igs_agent_set_state(const char *state);
INGESCAPE_EXPORT char * igs_agent_state(void); //caller owns returned value

/*agent family - optional
 32 characters canonical UUID format is commonly expected,
 default is an empty string, max length is 64 characters*/
INGESCAPE_EXPORT void igs_agent_set_family(const char *family);
INGESCAPE_EXPORT char * igs_agent_family(void); //caller owns returned value

//mute the agent
INGESCAPE_EXPORT void igs_agent_mute(void);
INGESCAPE_EXPORT void igs_agent_unmute(void);
INGESCAPE_EXPORT bool igs_agent_is_muted(void);

typedef void (igs_mute_fn)(bool is_muted,
                           void *my_data);
INGESCAPE_EXPORT void igs_observe_mute(igs_mute_fn cb, void *my_data);

/*freeze the agent
 When freezed, agent will not send anything on its outputs and
 its inputs are not reactive to external data.
 NB: the internal semantics of freeze and unfreeze for a given agent
 are up to the developer and can be controlled using callbacks and igs_observe_freeze*/
INGESCAPE_EXPORT void igs_freeze(void);
INGESCAPE_EXPORT bool igs_is_frozen(void);
INGESCAPE_EXPORT void igs_unfreeze(void);

typedef void (igs_freeze_fn)(bool is_paused,
                             void *my_data);
INGESCAPE_EXPORT void igs_observe_freeze(igs_freeze_fn cb, void *my_data);

/*observe agents/peers events on the network
 IGS_PEER_ENTERED: a new peer has entered the network
 IGS_PEER_EXITED: a peer has left the network (clean exit or timeout)
 IGS_AGENT_ENTERED: an agent carried by a peer has entered the network
 IGS_AGENT_UPDATED_DEFINITION: an agent has updated its definition
 IGS_AGENT_KNOWS_US: an agent is fully ready to communicate with us (services can be called)
 IGS_AGENT_EXITED: an agent has left the netork (peer has left or agent has been deactivated)
 IGS_AGENT_UPDATED_MAPPING: an agent has updated its mapping
 IGS_AGENT_WON_ELECTION: THIS AGENT has won an election (via its peer)
 IGS_AGENT_LOST_ELECTION: THIS AGENT has lost an election (via its peer)
 */
typedef enum {
    IGS_PEER_ENTERED = 1, //event_data are the peer headers as a zhash_t*
    IGS_PEER_EXITED, //event_data is NULL
    IGS_AGENT_ENTERED, //event_data is the JSON string for agent definition as a char*
    IGS_AGENT_UPDATED_DEFINITION, //event_data is the JSON string for agent definition as a char*
    IGS_AGENT_KNOWS_US, //event_data is NULL
    IGS_AGENT_EXITED, //event_data is NULL
    IGS_AGENT_UPDATED_MAPPING, //event_data is the JSON string for agent mapping as a char*
    IGS_AGENT_WON_ELECTION, //event_data is the election name as a char*
    IGS_AGENT_LOST_ELECTION //event_data is the election name as a char*
} igs_agent_event_t;
typedef void (igs_agent_events_fn)(igs_agent_event_t event,
                                   const char *uuid,
                                   const char *name,
                                   void *event_data,
                                   void *my_data);
INGESCAPE_EXPORT void igs_observe_agent_events (igs_agent_events_fn cb, void *my_data);


//////////////////////////////////////////////////////////////////////////////////
// Editing & inspecting definitions, adding and removing inputs/outputs/parameters

typedef enum {
    IGS_INPUT_T = 1,
    IGS_OUTPUT_T,
    IGS_PARAMETER_T
} igs_iop_type_t;

typedef enum {
    IGS_INTEGER_T = 1,
    IGS_DOUBLE_T,
    IGS_STRING_T,
    IGS_BOOL_T,
    IGS_IMPULSION_T,
    IGS_DATA_T,
    IGS_UNKNOWN_T
} igs_iop_value_type_t;

//load / set / get definition
INGESCAPE_EXPORT igs_result_t igs_definition_load_str (const char* json_str);
INGESCAPE_EXPORT igs_result_t igs_definition_load_file (const char* file_path);
INGESCAPE_EXPORT void igs_clear_definition(void); //clears definition data for the agent
INGESCAPE_EXPORT char * igs_definition_json(void); //returns json string, caller owns returned value
INGESCAPE_EXPORT char * igs_definition_description(void); //caller owns returned value
INGESCAPE_EXPORT char * igs_definition_version(void); //caller owns returned value
INGESCAPE_EXPORT void igs_definition_set_description(const char *description);
INGESCAPE_EXPORT void igs_definition_set_version(const char *version);

//edit the definition
INGESCAPE_EXPORT igs_result_t igs_input_create(const char *name,
                                               igs_iop_value_type_t value_type,
                                               void *value,
                                               size_t size);
INGESCAPE_EXPORT igs_result_t igs_output_create(const char *name,
                                                igs_iop_value_type_t value_type,
                                                void *value,
                                                size_t size);
INGESCAPE_EXPORT igs_result_t igs_parameter_create(const char *name,
                                                   igs_iop_value_type_t value_type,
                                                   void *value,
                                                   size_t size);
INGESCAPE_EXPORT igs_result_t igs_input_remove(const char *name);
INGESCAPE_EXPORT igs_result_t igs_output_remove(const char *name);
INGESCAPE_EXPORT igs_result_t igs_parameter_remove(const char *name);

//check IOP type, list and existence
INGESCAPE_EXPORT igs_iop_value_type_t igs_input_type(const char *name);
INGESCAPE_EXPORT igs_iop_value_type_t igs_output_type(const char *name);
INGESCAPE_EXPORT igs_iop_value_type_t igs_parameter_type(const char *name);

INGESCAPE_EXPORT size_t igs_input_count(void);
INGESCAPE_EXPORT size_t igs_output_count(void);
INGESCAPE_EXPORT size_t igs_parameter_count(void);

INGESCAPE_EXPORT char ** igs_input_list(size_t *inputs_nbr); //returned char** must be freed using igs_free_iop_list
INGESCAPE_EXPORT char** igs_output_list(size_t *outputs_nbr); //returned char** must be freed using igs_free_iop_list
INGESCAPE_EXPORT char** igs_parameter_list(size_t *parameters_nbr); //returned char** must be freed using igs_free_iop_list
INGESCAPE_EXPORT void igs_free_iop_list(char **list, size_t iop_nbr);

INGESCAPE_EXPORT bool igs_input_exists(const char *name);
INGESCAPE_EXPORT bool igs_output_exists(const char *name);
INGESCAPE_EXPORT bool igs_parameter_exists(const char *name);


////////////////////////////////////////////////////////////
// Reading and writing inputs/outputs/parameters, a.k.a IOPs

//read IOPs per value type
INGESCAPE_EXPORT bool igs_input_bool(const char *name);
INGESCAPE_EXPORT int igs_input_int(const char *name);
INGESCAPE_EXPORT double igs_input_double(const char *name);
INGESCAPE_EXPORT char * igs_input_string(const char *name); //caller owns returned value
INGESCAPE_EXPORT igs_result_t igs_input_data(const char *name, void **data, size_t *size); //caller owns returned value

INGESCAPE_EXPORT bool igs_output_bool(const char *name);
INGESCAPE_EXPORT int igs_output_int(const char *name);
INGESCAPE_EXPORT double igs_output_double(const char *name);
INGESCAPE_EXPORT char * igs_output_string(const char *name); //caller owns returned value
INGESCAPE_EXPORT igs_result_t igs_output_data(const char *name, void **data, size_t *size); //caller owns returned value

INGESCAPE_EXPORT bool igs_parameter_bool(const char *name);
INGESCAPE_EXPORT int igs_parameter_int(const char *name);
INGESCAPE_EXPORT double igs_parameter_double(const char *name);
INGESCAPE_EXPORT char * igs_parameter_string(const char *name); //caller owns returned value
INGESCAPE_EXPORT igs_result_t igs_parameter_data(const char *name, void **data, size_t *size); //caller owns returned value

//write IOPs per value type
INGESCAPE_EXPORT igs_result_t igs_input_set_bool(const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igs_input_set_int(const char *name, int value);
INGESCAPE_EXPORT igs_result_t igs_input_set_double(const char *name, double value);
INGESCAPE_EXPORT igs_result_t igs_input_set_string(const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igs_input_set_impulsion(const char *name);
INGESCAPE_EXPORT igs_result_t igs_input_set_data(const char *name, void *value, size_t size);

INGESCAPE_EXPORT igs_result_t igs_output_set_bool(const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igs_output_set_int(const char *name, int value);
INGESCAPE_EXPORT igs_result_t igs_output_set_double(const char *name, double value);
INGESCAPE_EXPORT igs_result_t igs_output_set_string(const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igs_output_set_impulsion(const char *name);
INGESCAPE_EXPORT igs_result_t igs_output_set_data(const char *name, void *value, size_t size);

INGESCAPE_EXPORT igs_result_t igs_parameter_set_bool(const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igs_parameter_set_int(const char *name, int value);
INGESCAPE_EXPORT igs_result_t igs_parameter_set_double(const char *name, double value);
INGESCAPE_EXPORT igs_result_t igs_parameter_set_string(const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igs_parameter_set_data(const char *name, void *value, size_t size);

/*Constraints on IOPs
 Constraints enable verifications upon sending or receiving information
 with inputs and outputs. The syntax for the constraints is global but
 some constraints only apply to certain types:
 Integers and doubles:
    - "max 10.123"  : applies a max allowed value on the IOP
    - "min -10" : applies a min allowed value on the IOP
    - "[-10, .1]" : applies min and max allowed values on the IOP
 Strings
    - "~ regular_expression", e.g. "~ \\d+(\.\\d+)?)":
        IOP of type STRING must match the regular expression

 Regular expressions are absed on CZMQ integration of SLRE with the
 following syntax:
^               Match beginning of a buffer
$               Match end of a buffer
()              Grouping and substring capturing
[...]           Match any character from set,
    caution: range-based syntax such as [0..9] is NOT supported
[^...]          Match any character but ones from set
\s              Match whitespace
\S              Match non-whitespace
\d              Match decimal digit
\r              Match carriage return
\n              Match newline
+               Match one or more times (greedy)
+?              Match one or more times (non-greedy)
*               Match zero or more times (greedy)
*?              Match zero or more times (non-greedy)
?               Match zero or once
\xDD            Match byte with hex value 0xDD
\meta           Match one of the meta character: ^$().[*+?\

 */
INGESCAPE_EXPORT void igs_constraints_enforce(bool enforce); //default is false, i.e. disabled
INGESCAPE_EXPORT igs_result_t igs_input_add_constraint(const char *name, const char *constraint);
INGESCAPE_EXPORT igs_result_t igs_output_add_constraint(const char *name, const char *constraint);
INGESCAPE_EXPORT igs_result_t igs_parameter_add_constraint(const char *name, const char *constraint);

//IOP descriptions
INGESCAPE_EXPORT void igs_input_set_description(const char *name, const char *description);
INGESCAPE_EXPORT void igs_output_set_description(const char *name, const char *description);
INGESCAPE_EXPORT void igs_parameter_set_description(const char *name, const char *description);

/*These two functions enable sending and receiving DATA on
 inputs/outputs by using zmsg_t structures. zmsg_t structures
 offer advanced functionalities for data serialization.
 More can be found here: http://czmq.zeromq.org/manual:zmsg */
INGESCAPE_EXPORT igs_result_t igs_output_set_zmsg(const char *name, zmsg_t *msg);
INGESCAPE_EXPORT igs_result_t igs_input_zmsg(const char *name, zmsg_t **msg); //msg is owned by caller

/*Clear IOP data in memory without having to write an empty value
 into the IOP. Especially useful for IOPs handling large strings and data.*/
INGESCAPE_EXPORT void igs_clear_input(const char *name);
INGESCAPE_EXPORT void igs_clear_output(const char *name);
INGESCAPE_EXPORT void igs_clear_parameter(const char *name);

//observe changes to an IOP
typedef void (igs_iop_fn)(igs_iop_type_t iop_type,
                          const char *name,
                          igs_iop_value_type_t value_type,
                          void *value,
                          size_t value_size,
                          void *my_data);
INGESCAPE_EXPORT void igs_observe_input(const char *name, igs_iop_fn cb, void *my_data);
INGESCAPE_EXPORT void igs_observe_output(const char *name, igs_iop_fn cb, void *my_data);
INGESCAPE_EXPORT void igs_observe_parameter(const char *name, igs_iop_fn cb, void *my_data);

//mute or unmute an output
INGESCAPE_EXPORT void igs_output_mute(const char *name);
INGESCAPE_EXPORT void igs_output_unmute(const char *name);
INGESCAPE_EXPORT bool igs_output_is_muted(const char *name);


////////////////////////////////
// Mapping edition & inspection

//load / set / get mapping
INGESCAPE_EXPORT igs_result_t igs_mapping_load_str(const char* json_str);
INGESCAPE_EXPORT igs_result_t igs_mapping_load_file(const char* file_path);
INGESCAPE_EXPORT char * igs_mapping_json(void); //returns json string, caller owns returned value
INGESCAPE_EXPORT size_t igs_mapping_count(void); //number of entries in the mapping output type

//clear mappings
INGESCAPE_EXPORT void igs_clear_mappings(void); //clears all our mappings with all agents
INGESCAPE_EXPORT void igs_clear_mappings_with_agent(const char *agent_name); //clears our mappings with this agent
INGESCAPE_EXPORT void igs_clear_mappings_for_input (const char *input_name); //clear all mappings for this input

//edit our mappings
INGESCAPE_EXPORT uint64_t igs_mapping_add(const char *from_our_input,
                                          const char *to_agent,
                                          const char *with_output); //returns mapping id or zero if creation failed
INGESCAPE_EXPORT igs_result_t igs_mapping_remove_with_id(uint64_t id);
INGESCAPE_EXPORT igs_result_t igs_mapping_remove_with_name(const char *from_our_input,
                                                           const char *to_agent,
                                                           const char *with_output);

//edit our splits
INGESCAPE_EXPORT size_t igs_split_count(void); //number of splits entries
INGESCAPE_EXPORT uint64_t igs_split_add(const char *from_our_input,
                                        const char *to_agent,
                                        const char *with_output); //returns split id or zero if creation failed
INGESCAPE_EXPORT igs_result_t igs_split_remove_with_id(uint64_t the_id);
INGESCAPE_EXPORT igs_result_t igs_split_remove_with_name(const char *from_our_input,
                                                         const char *to_agent,
                                                         const char *with_output);

/*When mapping other agents, it is possible to ask the mapped
 agents to send us their current output values through a dedicated
 message for our initialization.
 By default, this behavior is disabled.*/
INGESCAPE_EXPORT void igs_mapping_set_outputs_request(bool notify);
INGESCAPE_EXPORT bool igs_mapping_outputs_request(void);


////////////////////////////////
// Services edition & inspection

/*NOTES:
 - one and only one mandatory callback per service, set using igs_service_init :
 generates a warning if the callback missing when loading definition or receiving service
 - service names shall be unique for a given agent
 - names for optional replies shall be unique for a given service */

//services arguments
//When a service call is received, service arguments are provided as a chained list.
struct _igs_service_arg_t{
    char *name;
    igs_iop_value_type_t type;
    union{
        bool b;
        int i;
        double d;
        char *c;
        void *data;
    };
    size_t size;
    struct _igs_service_arg_t *next;
};

//Arguments list are initialized to NULL and then filled by calling igs_service_args_add_*
//Example:
//   igs_service_arg_t *list = NULL;
//   igs_service_args_add_int(&list, 10);

INGESCAPE_EXPORT void igs_service_args_add_int(igs_service_arg_t **list, int value);
INGESCAPE_EXPORT void igs_service_args_add_bool(igs_service_arg_t **list, bool value);
INGESCAPE_EXPORT void igs_service_args_add_double(igs_service_arg_t **list, double value);
INGESCAPE_EXPORT void igs_service_args_add_string(igs_service_arg_t **list, const char *value);
INGESCAPE_EXPORT void igs_service_args_add_data(igs_service_arg_t **list, void *value, size_t size);
INGESCAPE_EXPORT void igs_service_args_destroy(igs_service_arg_t **list);
INGESCAPE_EXPORT igs_service_arg_t * igs_service_args_clone(igs_service_arg_t *list);

/*call a service hosted by another agent
 Requires to pass an agent name or UUID, a service name and a list of arguments specific to the service.
 Token is an optional information to help routing replies.
 Passed arguments list will be deallocated and destroyed by the call. */
INGESCAPE_EXPORT igs_result_t igs_service_call (const char *agent_name_or_uuid,
                                                const char *service_name,
                                                igs_service_arg_t **list,
                                                const char *token);

/*create /remove / edit a service offered by our agent
 Warning: only one callback can be attached to a service
 (further attempts will be ignored and signaled by an error log). */
typedef void (igs_service_fn)(const char *sender_agent_name,
                              const char *sender_agent_uuid,
                              const char *service_name,
                              igs_service_arg_t *first_argument,
                              size_t args_nbr,
                              const char *token,
                              void* my_data);

INGESCAPE_EXPORT igs_result_t igs_service_init(const char *name, igs_service_fn cb, void *my_data);
INGESCAPE_EXPORT igs_result_t igs_service_remove(const char *name);
INGESCAPE_EXPORT igs_result_t igs_service_arg_add(const char *service_name, const char *arg_name, igs_iop_value_type_t type);
INGESCAPE_EXPORT igs_result_t igs_service_arg_remove(const char *service_name,
                                                     const char *arg_name); //removes first occurence of an argument with this name

//replies are optional and used for specification purposes
INGESCAPE_EXPORT igs_result_t igs_service_reply_add(const char *service_name, const char *reply_name);
INGESCAPE_EXPORT igs_result_t igs_service_reply_remove(const char *service_name, const char *reply_name);
INGESCAPE_EXPORT igs_result_t igs_service_reply_arg_add(const char *service_name, const char *reply_name, const char *arg_name, igs_iop_value_type_t type);
INGESCAPE_EXPORT igs_result_t igs_service_reply_arg_remove(const char *service_name,
                                                           const char *reply_name,
                                                           const char *arg_name);//removes first occurence of an argument with this name

//introspection for services, their arguments and optional replies
INGESCAPE_EXPORT size_t igs_service_count(void);
INGESCAPE_EXPORT bool igs_service_exists(const char *name);
INGESCAPE_EXPORT char ** igs_service_list(size_t *services_nbr);//returned char** must be freed using igs_free_services_list
INGESCAPE_EXPORT void igs_free_services_list(char **list, size_t services_nbr);

INGESCAPE_EXPORT igs_service_arg_t * igs_service_args_first(const char *service_name);
INGESCAPE_EXPORT size_t igs_service_args_count(const char *service_name);
INGESCAPE_EXPORT bool igs_service_arg_exists(const char *service_name, const char *arg_name);

INGESCAPE_EXPORT bool igs_service_has_replies(const char *service_name);
INGESCAPE_EXPORT bool igs_service_has_reply(const char *service_name, const char *reply_name);
INGESCAPE_EXPORT char ** igs_service_reply_names(const char *service_name, size_t *service_replies_nbr); //returned char** must be freed using igs_free_services_list
INGESCAPE_EXPORT igs_service_arg_t * igs_service_reply_args_first(const char *service_name, const char *reply_name);
INGESCAPE_EXPORT size_t igs_service_reply_args_count(const char *service_name, const char *reply_name);
INGESCAPE_EXPORT bool igs_service_reply_arg_exists(const char *service_name, const char *reply_name, const char *arg_name);


/////////
// Timers

/*Timers can be created to call code a certain number of times,
 each time after a certain delay. 0 times means repeating forever.
 Delay is expressed in milliseconds.
 WARNING: Timers MUST be created after starting an agent. */

typedef void (igs_timer_fn) (int timer_id,
                             void *my_data);
INGESCAPE_EXPORT int igs_timer_start(size_t delay,
                                     size_t times,
                                     igs_timer_fn cb,
                                     void *my_data); //returns timer id or -1 if error
INGESCAPE_EXPORT void igs_timer_stop(int timer_id);


///////////////////////////////////////////////////////////
// Communicating via channels (a.k.a Zyre groups and peers)

/* Channels are provided by zyre
 More can be found here : https://github.com/zeromq/zyre */
typedef void (igs_channels_fn) (const char *event,
                                 const char *peer_id,
                                 const char *peer_name,
                                 const char *address,
                                 const char *channel,
                                 zhash_t *headers,
                                 zmsg_t *msg,
                                 void *my_data);
INGESCAPE_EXPORT void igs_observe_channels(igs_channels_fn cb, void *my_data);

//join or leave a channel
INGESCAPE_EXPORT igs_result_t igs_channel_join(const char *channel);
INGESCAPE_EXPORT void igs_channel_leave(const char *channel);

//send message to a channel
INGESCAPE_EXPORT igs_result_t igs_channel_shout_str(const char *channel, const char *msg, ...);
INGESCAPE_EXPORT igs_result_t igs_channel_shout_data(const char *channel, void *data, size_t size);
INGESCAPE_EXPORT igs_result_t igs_channel_shout_zmsg(const char *channel, zmsg_t **msg_p); //destroys message after sending it

/*send a message to an agent by name or by uuid
 NB: peer ids and names are also supported by these functions but are used only if no agent is found first
 NB: if several agents share the same name, all will receive the message if addressed by name */
INGESCAPE_EXPORT igs_result_t igs_channel_whisper_str(const char *agent_name_or_agent_id_or_peer_id, const char *msg, ...);
INGESCAPE_EXPORT igs_result_t igs_channel_whisper_data(const char *agent_name_or_agent_id_or_peer_id, void *data, size_t size);
INGESCAPE_EXPORT igs_result_t igs_channel_whisper_zmsg(const char *agent_name_or_agent_id_or_peer_id, zmsg_t **msg_p); //destroys message after sending it

//set zyre headers
INGESCAPE_EXPORT igs_result_t igs_peer_add_header(const char *key, const char *value);
INGESCAPE_EXPORT igs_result_t igs_peer_remove_header(const char *key);


//////////////////////////////////////////////////////////////////////
// Brokers (when self-discovery is not possible, which should be rare)

/*BROKERS VS. SELF-DISCOVERY
 igs_start_with_device and igs_start_with_ip enable the agents to self-discover
 using UDP broadcast messages on the passed port. UDP broadcast messages can
 be blocked on some networks and can make things complex on networks with
 sub-networks.
 That is why ingescape also supports the use of brokers to relay discovery
 using TCP connections. Any agent can be a broker and agents using brokers
 simply have to use a list of broker endpoints. One broker is enough but
 several brokers can be set for robustness.

 For clarity, it is better if brokers are well identified on your platform,
 started before any agent, and serve only as brokers. But any other architecture
 is permitted and brokers can be restarted at any time.

 Endpoints have the form tcp://ip_address:port
 • igs_brokers_add is used to add brokers to connect to. Add
 as many brokers as you want. At least one declared broker is necessary to
 use igs_start_with_brokers. Use igs_clear_brokers to remove all the current
 brokers.
 • The endpoint in igs_broker_set_endpoint is the broker address we should be reached
 at as a broker if we want to be one. Using igs_broker_set_endpoint makes us a broker
 when starting.
 • The endpoint in igs_broker_set_advertized_endpoint replaces the one declared in
 igs_start_with_brokers for the registration to the brokers. This function enables
 passing through NAT and using a public address. Attention: this public address
 shall make sense to all the agents that will connect to us, independently from
 their local network.
 • Our agent endpoint in igs_start_with_brokers gives the address and port our
 agent can be reached at. This endpoint must be valid in the actual network
 configuration.
 */
INGESCAPE_EXPORT igs_result_t igs_broker_add(const char *broker_endpoint);
INGESCAPE_EXPORT void igs_clear_brokers(void);
INGESCAPE_EXPORT void igs_broker_enable_with_endpoint(const char *our_broker_endpoint);
INGESCAPE_EXPORT void igs_broker_set_advertized_endpoint(const char *advertised_endpoint); //parameter can be NULL
INGESCAPE_EXPORT igs_result_t igs_start_with_brokers(const char *agent_endpoint);


/////////////////////////////////////////////
// Security : identity, end-to-end encryption

/* Security is about authentification of other agents and encrypted communications.
 Both are offered by Ingescape with a public/private certificates mechanism relying
 on ZeroMQ. Security is activated optionally.
 When security is enabled :
 • When private certificates are generated on the fly by Ingescape, it provides the
 same protection as TLS for HTTPS communications. Thirdparties cannot steal identities
 and  communications are encrypted end-to-end. But any Ingescape agent with security
 enabled can join a platform.
 • When private certificates are file-based and secretly owned by each agent, no third
 party can join a platform without providing an identity that is well-known by the other
 agents using public certificates. This is safer but requires securing private certificates
 individually and sharing public certificates between all agents.

 Security is enabled by calling igs_enable_security.
 • If private_certificate_file is NULL, our private certificate is generated on the fly, and
 any agent with security enabled will be able to connect to us. Any value provided for
 public_certificates_directory will be ignored. This is the equivalent of HTTPS/TLS end-to-end
 encryption without access restriction.
 • If private_certificate_file is NOT NULL, the private certificate at private_certificate_file
 path will be used and only agents whose public certificates are in public_certificates_directory
 will be able to connect to us: this is end-to-end encryption + authentication.
 NB: if private_certificate_file is NOT NULL and public_certificates_directory is NULL or does not
 exist, security will not be enabled and our agent will not start.
 */
INGESCAPE_EXPORT igs_result_t igs_enable_security(const char *private_certificate_file,
                                                  const char *public_certificates_directory);
INGESCAPE_EXPORT void igs_disable_security(void);
INGESCAPE_EXPORT igs_result_t igs_broker_add_secure(const char *broker_endpoint,
                                                    const char *path_to_public_certificate_for_broker);
INGESCAPE_EXPORT zactor_t * igs_zmq_authenticator(void);


//////////////////////////////////////////
// Elections and leadership between agents

/* ELECTIONS between agents
 Create named elections between agents and designate a winner,
 as soon as they are two agents or more participating.
 • IGS_AGENT_WON_ELECTION agent event means that the election is
 over and this agent has WON
 • IGS_AGENT_LOST_ELECTION agent event means that the election is
 over and this agent has LOST
 • The election happens only when at least two agents participate.
 Nothing happens if only one agent participates.
 • When only one agent remains in an election after several have
 joined and left, it is declared winner.
 At startup, it is up to the developer to decide if an agent shall be
 considered as winner or wait for a certain amount of time to trigger
 some behavior. Do not forget that elections take at least some
 millisconds to be concluded.
 Agents in the same peer cannot compete one with another. Elections are
 reserved to agents running on separate peers/processes. If several
 agents in the same peer participate in the same election, they will
 all be declared winners or losers all together.
 The IGS_AGENT_WON_ELECTION and IGS_AGENT_LOST_ELECTION agent events
 can be triggered MULTIPLE TIMES in a row. Please adjust your agent
 behavior accordingly.
 */
INGESCAPE_EXPORT igs_result_t igs_election_join(const char *election_name);
INGESCAPE_EXPORT igs_result_t igs_election_leave(const char *election_name);


///////////////////////////////////////////////////////
// Administration, logging, configuration and utilities

/* LOGS POLICY
 - fatal : Events that force application termination.
 - error : Events that are fatal to the current operation but not the whole application.
 - warning : Events that can potentially cause application anomalies but that can be recovered automatically (by circumventing or retrying).
 - info : Generally useful information to log (service start/stop, configuration assumptions, etc.).
 - debug : Information that is diagnostically helpful to people more than just developers but useless for system monitoring.
 - trace : Information about parts of functions, for detailed diagnostic only.
 */
typedef enum {
    IGS_LOG_TRACE = 0,
    IGS_LOG_DEBUG,
    IGS_LOG_INFO,
    IGS_LOG_WARN,
    IGS_LOG_ERROR,
    IGS_LOG_FATAL
} igs_log_level_t;


//LOG ALIASES
INGESCAPE_EXPORT void igs_log(igs_log_level_t level,
                              const char *function,
                              const char *format, ...) CHECK_PRINTF (3);
#define igs_trace(...) igs_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
#define igs_debug(...) igs_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
#define igs_info(...)  igs_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
#define igs_warn(...)  igs_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
#define igs_error(...) igs_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
#define igs_fatal(...) igs_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)


//PROTOCOL AND VERSION
INGESCAPE_EXPORT int igs_version(void);
INGESCAPE_EXPORT int igs_protocol(void);


/*COMMAND LINE
 Agent command line can be passed here to be used by ingescapeLauncher. If not set,
 command line is initialized with exec path without any parameter.*/
INGESCAPE_EXPORT void igs_set_command_line(const char *line);
INGESCAPE_EXPORT void igs_set_command_line_from_args(int argc, const char * argv[]); //first element is replaced by absolute exec path on UNIX systems
INGESCAPE_EXPORT char * igs_command_line(void); //caller owns returned value


//LOGS MANAGEMENT
INGESCAPE_EXPORT void igs_log_set_console(bool); //enable logs in console (ERROR and FATAL are always displayed)
INGESCAPE_EXPORT bool igs_log_console(void);
INGESCAPE_EXPORT void igs_log_set_syslog(bool); //enable system logs on UNIX boxes (not working on Windows yet)
INGESCAPE_EXPORT bool igs_log_syslog(void);
INGESCAPE_EXPORT void igs_log_set_console_color(bool); //use colors in console
INGESCAPE_EXPORT bool igs_log_console_color(void);
INGESCAPE_EXPORT void igs_log_set_console_level (igs_log_level_t level); //set log level in console, default is IGS_LOG_INFO
INGESCAPE_EXPORT igs_log_level_t igs_log_console_level(void);
INGESCAPE_EXPORT void igs_log_set_stream(bool); //enable logs in socket stream
INGESCAPE_EXPORT bool igs_log_stream(void);
INGESCAPE_EXPORT void igs_log_set_file(bool, const char*); //enable logs in file. If path is NULL, uses default path (~/Documents/Ingescape/logs).
INGESCAPE_EXPORT void igs_log_set_file_level (igs_log_level_t level);
INGESCAPE_EXPORT void igs_log_set_file_max_line_length (size_t size);
INGESCAPE_EXPORT bool igs_log_file(void);
INGESCAPE_EXPORT void igs_log_set_file_path(const char *path); //default directory is ~/ on UNIX systems and current PATH on Windows
INGESCAPE_EXPORT char * igs_log_file_path(void); // caller owns returned value

INGESCAPE_EXPORT void igs_log_include_data(bool enable); //log details of data IOPs in log files , default is false.
INGESCAPE_EXPORT void igs_log_include_services(bool enable); //log details about call/excecute services in log files, default is false.
INGESCAPE_EXPORT void igs_log_no_warning_if_undefined_service(bool enable); //warns or not if an unknown service is called on this agent, default is warning (false).

/*DEFINITION & MAPPING FILE MANAGEMENT
 These functions enable to write definition and mapping on disk
 for our agent. Definition and mapping paths are initialized with
 igs_definition_load_file and igs_mappings_load_file. But they can
 also be configured using these functions to store current definitions. */
INGESCAPE_EXPORT void igs_definition_set_path(const char *path);
INGESCAPE_EXPORT void igs_definition_save(void);
INGESCAPE_EXPORT void igs_mapping_set_path(const char *path);
INGESCAPE_EXPORT void igs_mapping_save(void);


/*ADVANCED TRANSPORTS
 Ingescape automatically detects agents on the same computer
 and then uses optimized inter-process communication protocols
 depending on the operating system.
 • On UNIX systems, UNIX domain sockets are used that require a
 path whose default value is '/tmp/ingescape/' completed by the
 agent UUID.
 • On Microsoft Windows systems, the loopback is used.
 Advanced transports are allowed by default and can be disabled
 using igs_set_ipc.*/
INGESCAPE_EXPORT void igs_set_ipc(bool allow);
INGESCAPE_EXPORT bool igs_has_ipc(void);
#if defined (__UNIX__)
//set IPC folder path on UNIX systems (default is /tmp/ingescape/)
INGESCAPE_EXPORT void igs_set_ipc_dir(const char *path);
INGESCAPE_EXPORT const char * igs_ipc_dir(void);
#endif


/*NETWORK DEVICES
 Utility functions to detect network adapters with broadcast capabilities
 to be used in igs_start_with_device and igs_start_with_ip*/
INGESCAPE_EXPORT char ** igs_net_devices_list(int *nb);
INGESCAPE_EXPORT void igs_free_net_devices_list(char **devices, int nb);
INGESCAPE_EXPORT char ** igs_net_addresses_list(int *nb);
INGESCAPE_EXPORT void igs_free_net_addresses_list(char **addresses, int nb);


//NETWORK CONFIGURATION
INGESCAPE_EXPORT void igs_net_set_publishing_port(unsigned int port);
INGESCAPE_EXPORT void igs_net_set_log_stream_port(unsigned int port);
INGESCAPE_EXPORT void igs_net_set_discovery_interval(unsigned int interval); //in milliseconds
INGESCAPE_EXPORT void igs_net_set_timeout(unsigned int duration); //in milliseconds
INGESCAPE_EXPORT void  igs_net_raise_sockets_limit(void); //UNIX only, to be called before any ingescape or ZeroMQ activity
//Set high water marks (HWM) for the publish/subscribe sockets.
//Setting HWM to 0 means that they are disabled.
INGESCAPE_EXPORT void igs_net_set_high_water_marks(int hwm_value);


/*PERFORMANCE CHECK
 sends number of messages with defined size and displays performance
 information when finished (information displayed as INFO-level log)*/
INGESCAPE_EXPORT void igs_net_performance_check(const char *peer_id, size_t msg_size, size_t msgs_nbr);


/*NETWORK MONITORING
 Ingescape provides an integrated monitor to detect events relative to the network.
 NB: once igs_monitor_start has been called, igs_monitor_stop must be
 called to actually stop the monitor. If not stopped, it may cause an error when
 an agent terminates.*/
INGESCAPE_EXPORT void igs_monitor_start(unsigned int period); //in milliseconds
INGESCAPE_EXPORT void igs_monitor_start_with_network(unsigned int period,
                                                     const char* network_device,
                                                     unsigned int port);
INGESCAPE_EXPORT void igs_monitor_stop(void);
INGESCAPE_EXPORT bool igs_monitor_is_running(void);
/* When the monitor is started and igs_monitor_set_start_stop is set to true :
 - IP change will cause the agent to restart on the new IP (same device, same port)
 - Network device disappearance will cause the agent to stop. Agent will restart when device is back.*/
INGESCAPE_EXPORT void igs_monitor_set_start_stop(bool flag);

typedef enum {
    IGS_NETWORK_OK = 1, //when the network is OK
    IGS_NETWORK_DEVICE_NOT_AVAILABLE, //when our network device is not available
    IGS_NETWORK_ADDRESS_CHANGED, //when the IP address of our network device has changed
    IGS_NETWORK_OK_AFTER_MANUAL_RESTART //when our agent has been manually restarted and networkis now OK
} igs_monitor_event_t;

typedef void (igs_monitor_fn)(igs_monitor_event_t event,
                              const char *device,
                              const char *ip_address,
                              void *my_data);
INGESCAPE_EXPORT void igs_observe_monitor(igs_monitor_fn cb, void *my_data);


/*CONTEXT CLEANING
 Use this function when you absolutely need to clean the whole Ingescape context
 and you cannot stop your application to do so. This function SHALL NOT be used
 in production environments.*/
INGESCAPE_EXPORT void igs_clear_context(void);


/* LOGS REPLAY
 Ingescape logs contain all the necessary information for an agent to replay
 its changes for inputs, outputs, parameters and services.

 Replay happens in a dedicated thread created after calling igs_replay_init:
 • log_file_path : path to the log file to be read
 • speed : replay speed. Default is zero, meaning as fast as possible.
 • start_time : with format hh:mm::s, specifies the time when speed shall be used.
 Replay as fast as possible before that.
 • wait_for_start : waits for a call to igs_replay_start before starting the replay. Default is false.
 • replay_mode : a boolean composition of igs_replay_mode_t value to decide what shall be replayed.
 If mode is zero, all IOP and services are replayed.
 • agent : an OPTIONAL agent name serving as filter when the logs contain activity for multiple agents.

 igs_replay_terminate cleans the thread and requires calling igs_replay_init again.
 Replay thread is cleaned automatically also when the log file has been read completely.
 NB: replay is still under heavy development, use at your own risk...*/
typedef enum {
    IGS_REPLAY_INPUT = 1,
    IGS_REPLAY_OUTPUT = 2,
    IGS_REPLAY_PARAMETER = 4,
    IGS_REPLAY_EXECUTE_SERVICE= 8,
    IGS_REPLAY_CALL_SERVICE = 16

} igs_replay_mode_t;
INGESCAPE_EXPORT void igs_replay_init(const char *log_file_path,
                                      size_t speed,
                                      const char *start_time,
                                      bool wait_for_start,
                                      uint replay_mode,
                                      const char *agent);
INGESCAPE_EXPORT void igs_replay_start(void);
INGESCAPE_EXPORT void igs_replay_pause(bool pause);
INGESCAPE_EXPORT void igs_replay_terminate(void);


//////////////////////////////
// JSON parsing and generation
//NB: JSON parsing is based on YAJL

// parse JSON string or file based on parsing events and a callback function
typedef enum {
    IGS_JSON_STRING = 1,
    IGS_JSON_NUMBER, //int or double
    IGS_JSON_MAP,
    IGS_JSON_ARRAY,
    IGS_JSON_TRUE, //not used in parsing callback
    IGS_JSON_FALSE, //not used in parsing callback
    IGS_JSON_NULL,
    IGS_JSON_KEY = 9, //not used in tree queries
    IGS_JSON_MAP_END, //not used in tree queries
    IGS_JSON_ARRAY_END, //not used in tree queries
    IGS_JSON_BOOL //not used in tree queries
} igs_json_value_type_t;

typedef void (igs_json_fn) (igs_json_value_type_t type,
                            void *value,
                            size_t size,
                            void *data);
INGESCAPE_EXPORT void igs_json_parse_from_file (const char *path, igs_json_fn cb, void *data);
INGESCAPE_EXPORT void igs_json_parse_from_str (const char *path, igs_json_fn cb, void *data);

//generate JSON
INGESCAPE_EXPORT igs_json_t * igs_json_new (void);
INGESCAPE_EXPORT void igs_json_destroy (igs_json_t **self_p);
INGESCAPE_EXPORT void igs_json_open_map (igs_json_t *self);
INGESCAPE_EXPORT void igs_json_close_map (igs_json_t *self);
INGESCAPE_EXPORT void igs_json_open_array (igs_json_t *self);
INGESCAPE_EXPORT void igs_json_close_array (igs_json_t *self);
INGESCAPE_EXPORT void igs_json_add_null (igs_json_t *self);
INGESCAPE_EXPORT void igs_json_add_bool (igs_json_t *self, bool value);
INGESCAPE_EXPORT void igs_json_add_int (igs_json_t *self, int64_t value);
INGESCAPE_EXPORT void igs_json_add_double (igs_json_t *self, double value);
INGESCAPE_EXPORT void igs_json_add_string (igs_json_t *self, const char *value);
INGESCAPE_EXPORT void igs_json_print (igs_json_t *self);
INGESCAPE_EXPORT char * igs_json_dump (igs_json_t *self);//caller owns returned value
INGESCAPE_EXPORT char * igs_json_compact_dump (igs_json_t *self);//caller owns returned value

struct _igs_json_node_t {
    igs_json_value_type_t type;
    union {
        char * string;
        struct {
            long long i; //integer value, if representable
            double  d;   //double value, if representable
            char   *r;   //unparsed number in string form
            unsigned int flags; //flags to manage double and int values
        } number;
        struct {
            const char **keys; //array of keys
            struct _igs_json_node_t **values; //array of nodes
            size_t len; //number of key-node-pairs
        } object;
        struct {
            struct _igs_json_node_t **values; //array of nodes
            size_t len; //number of nodes
        } array;
    } u;
};

//convert between JSON and JSON nodes
INGESCAPE_EXPORT void igs_json_insert_node (igs_json_t *self, igs_json_node_t *node);
INGESCAPE_EXPORT igs_json_node_t * igs_json_node_for (igs_json_t *self);

// manipulate json nodes
INGESCAPE_EXPORT igs_json_node_t * igs_json_node_parse_from_file(const char *path);
INGESCAPE_EXPORT igs_json_node_t * igs_json_node_parse_from_str(const char *content);
INGESCAPE_EXPORT void igs_json_node_destroy(igs_json_node_t **node);
INGESCAPE_EXPORT igs_json_node_t * igs_json_node_dup(igs_json_node_t *node); //caller owns returned value
INGESCAPE_EXPORT char * igs_json_node_dump(igs_json_node_t *node); //caller owns returned value

//insert node in an existing structure
//Key must be non-NULL to insert in a map node.
//Does not take ownership of node_to_insert (duplicates it).
INGESCAPE_EXPORT void igs_json_node_insert(igs_json_node_t *parent, const char *key, igs_json_node_t *node_to_insert);

/* Nodes support queries to retrieve sub-nodes
 Important notes :
 - returned value must NOT be freed manually : it is owned by the node
 - returned structure contains a type that shall be checked to handle actual contained value(s)
 */
INGESCAPE_EXPORT igs_json_node_t * igs_json_node_find(igs_json_node_t *node, const char **path);

//JSON parsing creates number values.
//Use these two additional functions to check them as int and double values.
//NB: int values are considered both int and double
INGESCAPE_EXPORT bool igs_json_node_is_integer(igs_json_node_t *node);
INGESCAPE_EXPORT bool igs_json_node_is_double(igs_json_node_t *node);

#ifdef __cplusplus
}
#endif

#endif /* ingescape_h */
