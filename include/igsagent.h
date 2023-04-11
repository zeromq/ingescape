/*  =========================================================================
    igsagent - ingescape agent framework

    Copyright (c) 2021 Ingenuity io

    This file is part of the Ingescape library. See https://ingescape.com.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#ifndef IGSAGENT_H_INCLUDED
#define IGSAGENT_H_INCLUDED

#include "ingescape.h"

typedef struct _igsagent_t igsagent_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 The functions here are not commented. They are clone of the main
 Ingescape functions extensively presented in ingescape.h, and add
 a specific agent pointer as their first argument.
 */

////////////////////////////////////////
// Agent creation/destruction/activation
INGESCAPE_EXPORT igsagent_t * igsagent_new(const char *name, bool activate_immediately);
INGESCAPE_EXPORT void igsagent_destroy(igsagent_t **agent);
INGESCAPE_EXPORT igs_result_t igsagent_activate(igsagent_t *agent);
INGESCAPE_EXPORT igs_result_t igsagent_deactivate(igsagent_t *agent);
INGESCAPE_EXPORT bool igsagent_is_activated(igsagent_t *agent);

typedef void (igsagent_fn)(igsagent_t *agent,
                           bool is_activated,
                           void *my_data);
INGESCAPE_EXPORT void igsagent_observe(igsagent_t *agent, igsagent_fn cb, void *my_data);


////////////////
// Agent logging
INGESCAPE_EXPORT void igsagent_log (igs_log_level_t level, const char *function, igsagent_t *agent, const char *format, ...) CHECK_PRINTF (4);
#define igsagent_trace(...) igsagent_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
#define igsagent_debug(...) igsagent_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
#define igsagent_info(...) igsagent_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
#define igsagent_warn(...) igsagent_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
#define igsagent_error(...) igsagent_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
#define igsagent_fatal(...) igsagent_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)


/*
 NOTICE:
 All the functions below behave the same as the functions presented
 in ingescape.h. Their only addition consists in taking an agent instance
 as first argument.
 The rare exceptions in behavior changes are indicated for each affected
 function.
 */

///////////////////////////////////////////
// Agent initialization, control and events

INGESCAPE_EXPORT char * igsagent_name (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT void igsagent_set_name (igsagent_t *self, const char *name);
INGESCAPE_EXPORT char * igsagent_family (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT void igsagent_set_family (igsagent_t *self, const char *family);
INGESCAPE_EXPORT char * igsagent_uuid (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT char * igsagent_state (igsagent_t *self);
INGESCAPE_EXPORT void igsagent_set_state (igsagent_t *self, const char *state);
INGESCAPE_EXPORT void igsagent_mute (igsagent_t *self);
INGESCAPE_EXPORT void igsagent_unmute (igsagent_t *self);
INGESCAPE_EXPORT bool igsagent_is_muted (igsagent_t *self);

typedef void (igsagent_mute_fn) (igsagent_t *agent,
                                 bool mute,
                                 void *data);
INGESCAPE_EXPORT void igsagent_observe_mute (igsagent_t *self, igsagent_mute_fn cb, void *data);

typedef void (igsagent_agent_events_fn) (igsagent_t *agent,
                                         igs_agent_event_t event,
                                         const char *uuid,
                                         const char *name,
                                         void *event_data,
                                         void *data);
INGESCAPE_EXPORT void igsagent_observe_agent_events (igsagent_t *self, igsagent_agent_events_fn cb, void *data);


//////////////////////////////////////////////////////////////////////////////////
// Editing & inspecting definitions, adding and removing inputs/outputs/parameters

INGESCAPE_EXPORT igs_result_t igsagent_definition_load_str (igsagent_t *self, const char *json_str);
INGESCAPE_EXPORT igs_result_t igsagent_definition_load_file (igsagent_t *self, const char *file_path);
INGESCAPE_EXPORT void igsagent_clear_definition (igsagent_t *self);
INGESCAPE_EXPORT char * igsagent_definition_json (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT char * igsagent_definition_description (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT char * igsagent_definition_version (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT void igsagent_definition_set_description (igsagent_t *self, const char *descritpion);
INGESCAPE_EXPORT void igsagent_definition_set_version (igsagent_t *self, const char *version);
INGESCAPE_EXPORT igs_result_t igsagent_input_create (igsagent_t *self, const char *name,
                                                     igs_iop_value_type_t value_type,
                                                     void *value,
                                                     size_t size);
INGESCAPE_EXPORT igs_result_t igsagent_output_create (igsagent_t *self,
                                                      const char *name,
                                                      igs_iop_value_type_t value_type,
                                                      void *value,
                                                      size_t size);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_create (igsagent_t *self, const char *name,
                                                         igs_iop_value_type_t value_type,
                                                         void *value,
                                                         size_t size);
INGESCAPE_EXPORT igs_result_t igsagent_input_remove (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_result_t igsagent_output_remove (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_remove (igsagent_t *self, const char *name);

INGESCAPE_EXPORT igs_iop_value_type_t igsagent_input_type (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_iop_value_type_t igsagent_output_type (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_iop_value_type_t igsagent_parameter_type (igsagent_t *self, const char *name);

INGESCAPE_EXPORT size_t igsagent_input_count (igsagent_t *self);
INGESCAPE_EXPORT size_t igsagent_output_count (igsagent_t *self);
INGESCAPE_EXPORT size_t igsagent_parameter_count (igsagent_t *self);

INGESCAPE_EXPORT char ** igsagent_input_list (igsagent_t *self, size_t *nb_of_elements);//returned char** must be freed using igs_free_iop_list
INGESCAPE_EXPORT char ** igsagent_output_list (igsagent_t *self, size_t *nb_of_elements);//returned char** must be freed using igs_free_iop_list
INGESCAPE_EXPORT char ** igsagent_parameter_list (igsagent_t *self, size_t *nb_of_elements);//returned char** must be freed using igs_free_iop_list

INGESCAPE_EXPORT bool igsagent_input_exists (igsagent_t *self, const char *name);
INGESCAPE_EXPORT bool igsagent_output_exists (igsagent_t *self, const char *name);
INGESCAPE_EXPORT bool igsagent_parameter_exists (igsagent_t *self, const char *name);


////////////////////////////////////////////////////////////
// Reading and writing inputs/outputs/parameters, a.k.a IOPs

INGESCAPE_EXPORT bool igsagent_input_bool (igsagent_t *self, const char *name);
INGESCAPE_EXPORT int igsagent_input_int (igsagent_t *self, const char *name);
INGESCAPE_EXPORT double igsagent_input_double (igsagent_t *self, const char *name);
INGESCAPE_EXPORT char * igsagent_input_string (igsagent_t *self, const char *name);//caller owns returned value
INGESCAPE_EXPORT igs_result_t igsagent_input_data (igsagent_t *self, const char *name, void **data, size_t *size);

INGESCAPE_EXPORT bool igsagent_output_bool (igsagent_t *self, const char *name);
INGESCAPE_EXPORT int igsagent_output_int (igsagent_t *self, const char *name);
INGESCAPE_EXPORT double igsagent_output_double (igsagent_t *self, const char *name);
INGESCAPE_EXPORT char * igsagent_output_string (igsagent_t *self, const char *name);//caller owns returned value
INGESCAPE_EXPORT igs_result_t igsagent_output_data (igsagent_t *self, const char *name, void **data, size_t *size);

INGESCAPE_EXPORT bool igsagent_parameter_bool (igsagent_t *self, const char *name);
INGESCAPE_EXPORT int igsagent_parameter_int (igsagent_t *self, const char *name);
INGESCAPE_EXPORT double igsagent_parameter_double (igsagent_t *self, const char *name);
INGESCAPE_EXPORT char * igsagent_parameter_string (igsagent_t *self, const char *name);//caller owns returned value
INGESCAPE_EXPORT igs_result_t igsagent_parameter_data (igsagent_t *self, const char *name, void **data, size_t *size);

INGESCAPE_EXPORT igs_result_t igsagent_input_set_bool (igsagent_t *self, const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igsagent_input_set_int (igsagent_t *self, const char *name, int value);
INGESCAPE_EXPORT igs_result_t igsagent_input_set_double (igsagent_t *self, const char *name, double value);
INGESCAPE_EXPORT igs_result_t igsagent_input_set_string (igsagent_t *self, const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igsagent_input_set_impulsion (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_result_t igsagent_input_set_data (igsagent_t *self, const char *name, void *value, size_t size);

INGESCAPE_EXPORT igs_result_t igsagent_output_set_bool (igsagent_t *self, const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_int (igsagent_t *self, const char *name, int value);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_double (igsagent_t *self, const char *name, double value);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_string (igsagent_t *self, const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_impulsion (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_data (igsagent_t *self, const char *name, void *value, size_t size);

INGESCAPE_EXPORT igs_result_t igsagent_parameter_set_bool (igsagent_t *self, const char *name, bool value);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_set_int (igsagent_t *self, const char *name, int value);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_set_double (igsagent_t *self, const char *name, double value);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_set_string (igsagent_t *self, const char *name, const char *value);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_set_data (igsagent_t *self, const char *name, void *value, size_t size);

INGESCAPE_EXPORT void igsagent_constraints_enforce(igsagent_t *self, bool enforce); //default is false, i.e. disabled
INGESCAPE_EXPORT igs_result_t igsagent_input_add_constraint(igsagent_t *self, const char *name, const char *constraint);
INGESCAPE_EXPORT igs_result_t igsagent_output_add_constraint(igsagent_t *self, const char *name, const char *constraint);
INGESCAPE_EXPORT igs_result_t igsagent_parameter_add_constraint(igsagent_t *self, const char *name, const char *constraint);

INGESCAPE_EXPORT void igsagent_input_set_description(igsagent_t *self, const char *name, const char *description);
INGESCAPE_EXPORT void igsagent_output_set_description(igsagent_t *self, const char *name, const char *description);
INGESCAPE_EXPORT void igsagent_parameter_set_description(igsagent_t *self, const char *name, const char *description);

/*These two functions enable sending and receiving DATA
 inputs/outputs by using zmsg_t structures. zmsg_t structures
 offer advanced functionalities for data serialization.
 More can be found here: http://czmq.zeromq.org/manual:zmsg */
INGESCAPE_EXPORT igs_result_t igsagent_input_zmsg (igsagent_t *self, const char *name, zmsg_t **msg);
INGESCAPE_EXPORT igs_result_t igsagent_output_set_zmsg (igsagent_t *self, const char *name, zmsg_t *value);

//clear IOP data in memory without having to write an empty value
//into the IOP. Especially useful for IOPs handling large strings and data.
INGESCAPE_EXPORT void igsagent_clear_input (igsagent_t *self, const char *name);
INGESCAPE_EXPORT void igsagent_clear_output (igsagent_t *self, const char *name);
INGESCAPE_EXPORT void igsagent_clear_parameter (igsagent_t *self, const char *name);

typedef void (igsagent_iop_fn) (igsagent_t *agent,
                                igs_iop_type_t type,
                                const char *name,
                                igs_iop_value_type_t value_type,
                                void *value,
                                size_t value_size,
                                void *data);
INGESCAPE_EXPORT void igsagent_observe_input (igsagent_t *self, const char *name, igsagent_iop_fn cb, void *data);
INGESCAPE_EXPORT void igsagent_observe_output (igsagent_t *self, const char *name, igsagent_iop_fn cb, void *data);
INGESCAPE_EXPORT void igsagent_observe_parameter (igsagent_t *self, const char *name, igsagent_iop_fn cb, void *data);

INGESCAPE_EXPORT void igsagent_output_mute (igsagent_t *self, const char *name);
INGESCAPE_EXPORT void igsagent_output_unmute (igsagent_t *self, const char *name);
INGESCAPE_EXPORT bool igsagent_output_is_muted (igsagent_t *self, const char *name);


////////////////////////////////
// Mapping edition & inspection

INGESCAPE_EXPORT igs_result_t igsagent_mapping_load_str (igsagent_t *self, const char *json_str);
INGESCAPE_EXPORT igs_result_t igsagent_mapping_load_file (igsagent_t *self, const char *file_path);
INGESCAPE_EXPORT char * igsagent_mapping_json (igsagent_t *self);//caller owns returned value
INGESCAPE_EXPORT size_t igsagent_mapping_count (igsagent_t *self);

INGESCAPE_EXPORT void igsagent_clear_mappings (igsagent_t *self);
INGESCAPE_EXPORT void igsagent_clear_mappings_with_agent (igsagent_t *self, const char *agent_name);
INGESCAPE_EXPORT void igsagent_clear_mappings_for_input (igsagent_t *self, const char *input_name);

INGESCAPE_EXPORT uint64_t igsagent_mapping_add (igsagent_t *self, const char *from_our_input, const char *to_agent, const char *with_output);
INGESCAPE_EXPORT igs_result_t igsagent_mapping_remove_with_id (igsagent_t *self, uint64_t id);
INGESCAPE_EXPORT igs_result_t igsagent_mapping_remove_with_name (igsagent_t *self,
                                                                 const char *from_our_input,
                                                                 const char *to_agent,
                                                                 const char *with_output);

INGESCAPE_EXPORT size_t igsagent_split_count (igsagent_t *self);
INGESCAPE_EXPORT uint64_t igsagent_split_add (igsagent_t *self, const char *from_our_input, const char *to_agent, const char *with_output);
INGESCAPE_EXPORT igs_result_t igsagent_split_remove_with_id (igsagent_t *self, uint64_t id);
INGESCAPE_EXPORT igs_result_t igsagent_split_remove_with_name (igsagent_t *self,
                                                               const char *from_our_input,
                                                               const char *to_agent,
                                                               const char *with_output);

INGESCAPE_EXPORT bool igsagent_mapping_outputs_request (igsagent_t *self);
INGESCAPE_EXPORT void igsagent_mapping_set_outputs_request (igsagent_t *self, bool notify);

////////////////////////////////
// Services edition & inspection

INGESCAPE_EXPORT igs_result_t igsagent_service_call (igsagent_t *self,
                                                     const char *agent_name_or_uuid,
                                                     const char *service_name,
                                                     igs_service_arg_t **list,
                                                     const char *token);

typedef void (igsagent_service_fn) (igsagent_t *agent,
                                    const char *sender_agent_name,
                                    const char *sender_agent_uuid,
                                    const char *service_name,
                                    igs_service_arg_t *first_argument,
                                    size_t args_nbr,
                                    const char *token,
                                    void *data);
INGESCAPE_EXPORT igs_result_t igsagent_service_init (igsagent_t *self, const char *name, igsagent_service_fn cb, void *data);
INGESCAPE_EXPORT igs_result_t igsagent_service_remove (igsagent_t *self, const char *name);
INGESCAPE_EXPORT igs_result_t igsagent_service_arg_add (igsagent_t *self,
                                                        const char *service_name,
                                                        const char *arg_name,
                                                        igs_iop_value_type_t value_type);
INGESCAPE_EXPORT igs_result_t igsagent_service_arg_remove (igsagent_t *self, const char *service_name, const char *arg_name);
INGESCAPE_EXPORT igs_result_t igsagent_service_reply_add(igsagent_t *self, const char *service_name, const char *reply_name);
INGESCAPE_EXPORT igs_result_t igsagent_service_reply_remove(igsagent_t *self, const char *service_name, const char *reply_name);
INGESCAPE_EXPORT igs_result_t igsagent_service_reply_arg_add(igsagent_t *self, const char *service_name,
                                                             const char *reply_name,
                                                             const char *arg_name, igs_iop_value_type_t type);
INGESCAPE_EXPORT igs_result_t igsagent_service_reply_arg_remove(igsagent_t *self, const char *service_name,
                                                                const char *reply_name,
                                                                const char *arg_name);
INGESCAPE_EXPORT size_t igsagent_service_count (igsagent_t *self);
INGESCAPE_EXPORT bool igsagent_service_exists (igsagent_t *self, const char *service_name);
INGESCAPE_EXPORT char ** igsagent_service_list (igsagent_t *self, size_t *nb_of_elements);//returned char** must be freed using igs_free_services_list
INGESCAPE_EXPORT igs_service_arg_t * igsagent_service_args_first (igsagent_t *self, const char *service_name);
INGESCAPE_EXPORT size_t igsagent_service_args_count (igsagent_t *self, const char *service_name);
INGESCAPE_EXPORT bool igsagent_service_arg_exists (igsagent_t *self, const char *service_name, const char *arg_name);
INGESCAPE_EXPORT bool igsagent_service_has_replies(igsagent_t *self, const char *service_name);
INGESCAPE_EXPORT bool igsagent_service_has_reply(igsagent_t *self, const char *service_name, const char *reply_name);
INGESCAPE_EXPORT char ** igsagent_service_reply_names(igsagent_t *self, const char *service_name, size_t *service_replies_nbr); //returned char** must be freed using igs_free_services_list
INGESCAPE_EXPORT igs_service_arg_t * igsagent_service_reply_args_first(igsagent_t *self, const char *service_name, const char *reply_name);
INGESCAPE_EXPORT size_t igsagent_service_reply_args_count(igsagent_t *self, const char *service_name, const char *reply_name);
INGESCAPE_EXPORT bool igsagent_service_reply_arg_exists(igsagent_t *self, const char *service_name, const char *reply_name, const char *arg_name);

//////////////////////////////////////////
// Elections and leadership between agents

INGESCAPE_EXPORT igs_result_t igsagent_election_join (igsagent_t *self, const char *election_name);
INGESCAPE_EXPORT igs_result_t igsagent_election_leave (igsagent_t *self, const char *election_name);

///////////////////////////////////////////////////////
// Administration, logging, configuration and utilities

INGESCAPE_EXPORT void igsagent_definition_set_path (igsagent_t *self, const char *path);
INGESCAPE_EXPORT void igsagent_definition_save (igsagent_t *self);
INGESCAPE_EXPORT void igsagent_mapping_set_path (igsagent_t *self, const char *path);
INGESCAPE_EXPORT void igsagent_mapping_save (igsagent_t *self);

#ifdef __cplusplus
}
#endif

#endif
