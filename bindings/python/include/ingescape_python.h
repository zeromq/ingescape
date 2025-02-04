/*  =========================================================================
 * ingescape_python.h - Global functions
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#ifndef ingescape_python_h
#define ingescape_python_h

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <frameobject.h>

#include "compat.h"

#include <ingescape/ingescape.h>
#include <czmq.h>

PyObject * start_with_device_wrapper(PyObject * self, PyObject * args);
PyObject * start_with_ip_wrapper(PyObject * self, PyObject * args);
PyObject * stop_wrapper(PyObject * self, PyObject * args);
PyObject * is_started_wrapper(PyObject * self, PyObject * args);


typedef struct stopCallback{
    PyObject *call;
    PyObject *argstopList;
    struct stopCallback *next;
    struct stopCallback *prev;
}stopCallback_t;
extern stopCallback_t *stopList;
PyObject * observe_forced_stop_wrapper(PyObject *self, PyObject *args);


PyObject * agent_set_name_wrapper(PyObject * self, PyObject * args);
PyObject * agent_name_wrapper(PyObject * self, PyObject * args);

PyObject * agent_uuid_wrapper(PyObject * self, PyObject * args);

PyObject * agent_set_state_wrapper(PyObject * self, PyObject * args);
PyObject * agent_state_wrapper(PyObject * self, PyObject * args);


PyObject * agent_family_wrapper(PyObject * self, PyObject * args);
PyObject * agent_set_family_wrapper(PyObject * self, PyObject * args);

PyObject * agent_mute_wrapper(PyObject * self, PyObject * args);
PyObject * agent_unmute_wrapper(PyObject * self, PyObject * args);
PyObject * agent_is_muted_wrapper(PyObject * self, PyObject * args);

typedef struct mute_cb {
    PyObject *callback;
    PyObject *my_data;
    struct mute_cb *next;
    struct mute_cb *prev;
} mute_cb_t;
extern mute_cb_t *observe_mute_cbList;
PyObject *observe_mute_wrapper(PyObject *self, PyObject *args);

PyObject * freeze_wrapper(PyObject * self, PyObject * args);
PyObject * unfreeze_wrapper(PyObject * self, PyObject * args);
PyObject * is_frozen_wrapper(PyObject * self, PyObject * args);

typedef struct freezeCallback{
    PyObject *call;
    PyObject *arglist;
    struct freezeCallback *next;
    struct freezeCallback *prev;
}freezeCallback_t;
extern freezeCallback_t *freezeList;
PyObject * observe_freeze_wrapper(PyObject *self, PyObject *args);

typedef struct agentEventCallback{
    PyObject *callback;
    PyObject *my_data;
    struct agentEventCallback *next;
    struct agentEventCallback *prev;
}agentEventCallback_t;
extern agentEventCallback_t *agentEventCallbackList;
PyObject * observe_agent_events_wrapper(PyObject *self, PyObject *args);

PyObject * definition_load_str_wrapper(PyObject * self, PyObject * args);
PyObject * definition_load_file_wrapper(PyObject * self, PyObject * args);
PyObject * clear_definition_wrapper(PyObject * self, PyObject * args);
PyObject * definition_json_wrapper(PyObject * self, PyObject * args);
PyObject * definition_set_class_wrapper(PyObject * self, PyObject * args);
PyObject * definition_class_wrapper(PyObject * self, PyObject * args);
PyObject * definition_set_package_wrapper(PyObject * self, PyObject * args);
PyObject * definition_package_wrapper(PyObject * self, PyObject * args);
PyObject * definition_set_description_wrapper(PyObject * self, PyObject * args);
PyObject * definition_description_wrapper(PyObject * self, PyObject * args);
PyObject * definition_set_version_wrapper(PyObject * self, PyObject * args);
PyObject * definition_version_wrapper(PyObject * self, PyObject * args);

PyObject * input_create_wrapper(PyObject * self, PyObject * args);
PyObject * output_create_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_create_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_create_wrapper(PyObject * self, PyObject * args);

PyObject * input_remove_wrapper(PyObject * self, PyObject * args);
PyObject * output_remove_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_remove_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_remove_wrapper(PyObject * self, PyObject * args);

PyObject * input_type_wrapper(PyObject * self, PyObject * args);
PyObject * output_type_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_type_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_type_wrapper(PyObject * self, PyObject * args);

PyObject * input_count_wrapper(PyObject * self, PyObject * args);
PyObject * output_count_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_count_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_count_wrapper(PyObject * self, PyObject * args);

PyObject * input_list_wrapper(PyObject * self, PyObject * args);
PyObject * output_list_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_list_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_list_wrapper(PyObject * self, PyObject * args);

PyObject * input_exists_wrapper(PyObject * self, PyObject * args);
PyObject * output_exists_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_exists_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_exists_wrapper(PyObject * self, PyObject * args);

PyObject * input_bool_wrapper(PyObject * self, PyObject * args);
PyObject * input_int_wrapper(PyObject * self, PyObject * args);
PyObject * input_double_wrapper(PyObject * self, PyObject * args);
PyObject * input_string_wrapper(PyObject * self, PyObject * args);
PyObject * input_data_wrapper(PyObject * self, PyObject * args);

PyObject * output_bool_wrapper(PyObject * self, PyObject * args);
PyObject * output_int_wrapper(PyObject * self, PyObject * args);
PyObject * output_double_wrapper(PyObject * self, PyObject * args);
PyObject * output_string_wrapper(PyObject * self, PyObject * args);
PyObject * output_data_wrapper(PyObject * self, PyObject * args);

PyObject * attribute_bool_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_int_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_double_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_string_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_data_wrapper(PyObject * self, PyObject * args);

PyObject * parameter_bool_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_int_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_double_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_string_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_data_wrapper(PyObject * self, PyObject * args);

PyObject * input_set_bool_wrapper(PyObject * self, PyObject * args);
PyObject * input_set_int_wrapper(PyObject * self, PyObject * args);
PyObject * input_set_double_wrapper(PyObject * self, PyObject * args);
PyObject * input_set_string_wrapper(PyObject * self, PyObject * args);
PyObject * input_set_impulsion_wrapper(PyObject * self, PyObject * args);
PyObject * input_set_data_wrapper(PyObject * self, PyObject * args);

PyObject * output_set_bool_wrapper(PyObject * self, PyObject * args);
PyObject * output_set_int_wrapper(PyObject * self, PyObject * args);
PyObject * output_set_double_wrapper(PyObject * self, PyObject * args);
PyObject * output_set_string_wrapper(PyObject * self, PyObject * args);
PyObject * output_set_impulsion_wrapper(PyObject * self, PyObject * args);
PyObject * output_set_data_wrapper(PyObject * self, PyObject * args);

PyObject * attribute_set_bool_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_set_int_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_set_double_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_set_string_wrapper(PyObject * self, PyObject * args);
PyObject * attribute_set_data_wrapper(PyObject * self, PyObject * args);

PyObject * parameter_set_bool_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_set_int_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_set_double_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_set_string_wrapper(PyObject * self, PyObject * args);
PyObject * parameter_set_data_wrapper(PyObject * self, PyObject * args);

PyObject * igs_constraints_enforce_wrapper(PyObject *self, PyObject *args);
PyObject * igs_input_add_constraint_wrapper(PyObject *self, PyObject *args);
PyObject * igs_output_add_constraint_wrapper(PyObject *self, PyObject *args);
PyObject * igs_attribute_add_constraint_wrapper(PyObject *self, PyObject *args);
PyObject * igs_parameter_add_constraint_wrapper(PyObject *self, PyObject *args);

PyObject * igs_input_set_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_input_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_output_set_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_output_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_attribute_set_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_attribute_description_wrapper(PyObject *self, PyObject *args);
PyObject * igs_parameter_set_description_wrapper(PyObject *self, PyObject *args);

PyObject * igs_input_set_detailed_type_wrapper(PyObject *self, PyObject *args);
PyObject * igs_output_set_detailed_type_wrapper(PyObject *self, PyObject *args);
PyObject * igs_attribute_set_detailed_type_wrapper(PyObject *self, PyObject *args);

PyObject * clear_input_wrapper(PyObject * self, PyObject * args);
PyObject * clear_output_wrapper(PyObject * self, PyObject * args);
PyObject * clear_attribute_wrapper(PyObject * self, PyObject * args);
PyObject * clear_parameter_wrapper(PyObject * self, PyObject * args);

typedef struct observe_io_cb {
    char *nameArg;
    igs_io_type_t ioType;
    PyObject *callback;
    PyObject *my_data;
    struct observe_io_cb *next;
    struct observe_io_cb *prev;
} observe_io_cb_t;
extern observe_io_cb_t *observe_io_cbList;
PyObject *observe_input_wrapper(PyObject *self, PyObject *args);
PyObject *observe_output_wrapper(PyObject *self, PyObject *args);
PyObject *observe_attribute_wrapper(PyObject *self, PyObject *args);
PyObject *observe_parameter_wrapper(PyObject *self, PyObject *args);

PyObject * output_mute_wrapper(PyObject * self, PyObject * args);
PyObject * output_unmute_wrapper(PyObject * self, PyObject * args);
PyObject * output_is_muted_wrapper(PyObject * self, PyObject * args);


PyObject * mapping_load_str_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_load_file_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_json_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_count_wrapper(PyObject * self, PyObject * args);

PyObject * clear_mappings_wrapper(PyObject * self, PyObject * args);
PyObject * clear_mappings_with_agent_wrapper(PyObject *self, PyObject *args);
PyObject * clear_mappings_for_input_wrapper(PyObject *self, PyObject *args);

PyObject * mapping_add_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_remove_with_id_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_remove_with_name_wrapper(PyObject * self, PyObject * args);

PyObject *split_count_wrapper(PyObject *self, PyObject *args);
PyObject *split_add_wrapper(PyObject *self, PyObject *args);
PyObject *split_remove_with_id_wrapper(PyObject *self, PyObject *args);
PyObject *split_remove_with_name_wrapper(PyObject *self, PyObject *args);

PyObject * mapping_set_outputs_request_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_outputs_request_wrapper(PyObject * self, PyObject * args);


PyObject *service_call_wrapper(PyObject * self, PyObject * args);

typedef struct callCallback {
    char *callName;
    PyObject *call;
    PyObject *arglist;
    struct callCallback *next;
    struct callCallback *prev;
}callCallback_t;
extern callCallback_t *callList;

PyObject * service_init_wrapper(PyObject *self, PyObject *args);
PyObject * service_remove_wrapper(PyObject * self, PyObject * args);
PyObject * service_arg_add_wrapper(PyObject * self, PyObject * args);
PyObject * service_arg_remove_wrapper(PyObject * self, PyObject * args);

PyObject * service_set_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_arg_set_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_arg_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_set_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_arg_set_description_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_arg_description_wrapper(PyObject * self, PyObject * args);

PyObject * service_reply_add_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_remove_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_arg_add_wrapper(PyObject * self, PyObject * args);
PyObject * service_reply_arg_remove_wrapper(PyObject * self, PyObject * args);

PyObject * service_count_wrapper(PyObject * self, PyObject * args);
PyObject * service_exists_wrapper(PyObject * self, PyObject * args);
PyObject *service_list_wrapper(PyObject * self, PyObject * args);

PyObject *service_args_list_wrapper(PyObject * self, PyObject * args);
PyObject *service_args_count_wrapper(PyObject * self, PyObject * args);
PyObject *service_arg_exists_wrapper(PyObject * self, PyObject * args);

PyObject *service_has_replies_wrapper(PyObject * self, PyObject * args);
PyObject *service_has_reply_wrapper(PyObject * self, PyObject * args);
PyObject *service_reply_names_wrapper(PyObject * self, PyObject * args);
PyObject *service_reply_args_wrapper(PyObject * self, PyObject * args);

PyObject *service_reply_args_list_wrapper(PyObject * self, PyObject * args);
PyObject *service_reply_args_count_wrapper(PyObject * self, PyObject * args);
PyObject *service_reply_arg_exists_wrapper(PyObject * self, PyObject * args);

typedef struct timer_callback_element{
    PyObject *callback;
    PyObject *my_data;
} timer_callback_element_t;

PyObject * igs_timer_start_wrapper(PyObject *self, PyObject *args);
PyObject * igs_timer_stop_wrapper(PyObject *self, PyObject *args);

PyObject * channel_join_wrapper(PyObject *self, PyObject *args);
PyObject * channel_leave_wrapper(PyObject *self, PyObject *args);

PyObject * channel_shout_str_wrapper(PyObject *self, PyObject *args);
PyObject * channel_shout_data_wrapper(PyObject *self, PyObject *args);

PyObject * channel_whisper_str_wrapper(PyObject *self, PyObject *args);
PyObject * channel_whisper_data_wrapper(PyObject *self, PyObject *args);

PyObject * peer_add_header_wrapper(PyObject *self, PyObject *args);
PyObject * peer_remove_header_wrapper(PyObject *self, PyObject *args);


PyObject * igs_broker_add_wrapper(PyObject *self, PyObject *args);
PyObject * igs_clear_brokers_wrapper(PyObject *self, PyObject *args);
PyObject * igs_broker_enable_with_endpoint_wrapper(PyObject *self, PyObject *args);
PyObject * igs_broker_set_advertized_endpoint_wrapper(PyObject *self, PyObject *args);
PyObject * igs_start_with_brokers_wrapper(PyObject *self, PyObject *args);

PyObject * igs_enable_security_wrapper(PyObject *self, PyObject *args);
PyObject * igs_disable_security_wrapper(PyObject *self, PyObject *args);
PyObject * igs_broker_add_secure_wrapper(PyObject *self, PyObject *args);

PyObject * election_join_wrapper(PyObject * self, PyObject * args);
PyObject * election_leave_wrapper(PyObject * self, PyObject * args);


// Real-time APIs
PyObject * rt_get_current_timestamp_wrapper(PyObject * self, PyObject * args);
PyObject * rt_set_timestamps_wrapper(PyObject * self, PyObject * args);
PyObject * rt_timestamps_wrapper(PyObject * self, PyObject * args);
PyObject * rt_set_time_wrapper(PyObject * self, PyObject * args);
PyObject * rt_time_wrapper(PyObject * self, PyObject * args);
PyObject * rt_set_synchronous_mode_wrapper(PyObject * self, PyObject * args);
PyObject * rt_synchronous_mode_wrapper(PyObject * self, PyObject * args);


PyObject * trace_wrapper(PyObject * self, PyObject * args);
PyObject * debug_wrapper(PyObject * self, PyObject * args);
PyObject * info_wrapper(PyObject * self, PyObject * args);
PyObject * warn_wrapper(PyObject * self, PyObject * args);
PyObject * error_wrapper(PyObject * self, PyObject * args);
PyObject * fatal_wrapper(PyObject * self, PyObject * args);

PyObject * version_wrapper(PyObject * self, PyObject * args);
PyObject * protocol_wrapper(PyObject * self, PyObject * args);

PyObject * set_command_line_wrapper(PyObject * self, PyObject * args);
PyObject * command_line_wrapper(PyObject *self, PyObject *args);

PyObject * log_set_console_wrapper(PyObject * self, PyObject * args);
PyObject * log_console_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_syslog_wrapper(PyObject * self, PyObject * args);
PyObject * log_syslog_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_console_color_wrapper(PyObject * self, PyObject * args);
PyObject * log_console_color_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_console_level_wrapper(PyObject * self, PyObject * args);
PyObject * log_console_level_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_stream_wrapper(PyObject * self, PyObject * args);
PyObject * log_stream_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_file_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_file_level_wrapper(PyObject *self, PyObject *args);
PyObject * log_set_file_max_line_length_wrapper(PyObject *self, PyObject *args);
PyObject * log_file_wrapper(PyObject * self, PyObject * args);
PyObject * log_set_file_path_wrapper(PyObject * self, PyObject * args);
PyObject * log_file_path_wrapper(PyObject * self, PyObject * args);

PyObject * log_include_data_wrapper(PyObject *self, PyObject *args);
PyObject * igs_log_include_services_wrapper(PyObject *self, PyObject *args);
PyObject * igs_log_no_warning_if_undefined_service_wrapper(PyObject *self, PyObject *args);

PyObject * definition_set_path_wrapper(PyObject * self, PyObject * args);
PyObject * definition_save_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_set_path_wrapper(PyObject * self, PyObject * args);
PyObject * mapping_save_wrapper(PyObject * self, PyObject * args);

PyObject * igs_set_ipc_wrapper(PyObject *self, PyObject *args);
PyObject * igs_has_ipc_wrapper(PyObject *self, PyObject *args);
#if defined (__UNIX__)
PyObject * igs_set_ipc_dir_wrapper(PyObject *self, PyObject *args);
PyObject * igs_ipc_dir_wrapper(PyObject *self, PyObject *args);
#endif

PyObject * net_devices_list_wrapper(PyObject * self, PyObject * args);
PyObject * net_addresses_list_wrapper(PyObject * self, PyObject * args);


PyObject * net_set_publishing_port_wrapper(PyObject *self, PyObject *args);
PyObject * igs_net_set_log_stream_port_wrapper(PyObject *self, PyObject *args);
PyObject * net_set_discovery_interval_wrapper(PyObject *self, PyObject *args);
PyObject * igs_net_set_timeout_wrapper(PyObject *self, PyObject *args);
PyObject * igs_net_raise_sockets_limit_wrapper(PyObject *self, PyObject *args);
PyObject * igs_net_set_high_water_marks_wrapper(PyObject *self, PyObject *args);


PyObject * igs_clear_context_wrapper(PyObject *self, PyObject *args);


PyObject * igs_monitor_start_wrapper(PyObject *self, PyObject *args);
PyObject * igs_monitor_start_with_network_wrapper(PyObject *self, PyObject *args);
PyObject * igs_monitor_stop_wrapper(PyObject *self, PyObject *args);
PyObject * igs_monitor_is_running_wrapper(PyObject *self, PyObject *args);
PyObject * igs_monitor_set_start_stop_wrapper(PyObject *self, PyObject *args);

typedef struct monitor_cb {
    PyObject *callback;
    PyObject *my_data;
    struct monitor_cb *next;
    struct monitor_cb *prev;
} monitor_cb_t;
extern monitor_cb_t *observe_monitor_cbList;
PyObject * igs_observe_monitor_wrapper(PyObject *self, PyObject *args);

PyObject * igs_net_performance_check_wrapper(PyObject *self, PyObject *args);

#endif /* ingescape_python_h */
