/*  =========================================================================
 * ingescape_python.c - Main binding file, define all functions, classes
 * for the binding
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <Python.h>
#include "uthash/utlist.h"

// Ingescape headers
#include "ingescape_python.h"
#include "ingescape_agent_python.h"


////////////////////////////////////////////////////
// Defining all the methods of the Ingescape module

static PyMethodDef Ingescape_methods[] =
{
    //start & stop the agent
    {"start_with_device",start_with_device_wrapper, METH_VARARGS, "start_with_device(device, port, )\n--\n\n "},
    {"start_with_ip", start_with_ip_wrapper, METH_VARARGS, "start_with_ip(ip_address, port, )\n--\n\n "},
    {"stop", stop_wrapper, METH_NOARGS, "stop()\n--\n\n "},
    {"is_started", is_started_wrapper, METH_NOARGS, "is_started()\n--\n\n "},

    //agent name set and get
    {"agent_set_name", agent_set_name_wrapper, METH_VARARGS, "agent_set_name(agent_name, )\n--\n\n "},
    {"agent_name", agent_name_wrapper, METH_NOARGS, "agent_name()\n--\n\n "},
    {"agent_family", agent_family_wrapper, METH_NOARGS,  "agent_family()\n--\n\n "},
    {"agent_set_family", agent_set_family_wrapper, METH_VARARGS, "agent_set_family(agent_family, )\n--\n\n "},
    {"agent_uuid", agent_uuid_wrapper, METH_NOARGS, "agent_uuid()\n--\n\n "},

    //control agent state
    {"agent_set_state", agent_set_state_wrapper, METH_VARARGS,  "agent_set_state(agent_state, )\n--\n\n "},
    {"agent_state", agent_state_wrapper, METH_NOARGS,  "agent_state()\n--\n\n "},

    //mute the agent ouputs
    {"agent_mute", agent_mute_wrapper, METH_NOARGS, "agent_mute()\n--\n\n "},
    {"agent_unmute", agent_unmute_wrapper, METH_NOARGS, "agent_unmute()\n--\n\n "},
    {"agent_is_muted", agent_is_muted_wrapper, METH_NOARGS, "agent_is_muted()\n--\n\n "},

    //freeze and unfreeze the agent
    {"freeze", freeze_wrapper, METH_NOARGS, "freeze()\n--\n\n "},
    {"unfreeze", unfreeze_wrapper, METH_NOARGS, "unfreeze()\n--\n\n "},
    {"is_frozen", is_frozen_wrapper, METH_NOARGS, "is_frozen()\n--\n\n "},

    //read input per type
    {"input_bool", input_bool_wrapper, METH_VARARGS, "input_bool(input_name, )\n--\n\n "},
    {"input_int", input_int_wrapper, METH_VARARGS, "input_int(input_name, )\n--\n\n "},
    {"input_double", input_double_wrapper, METH_VARARGS, "input_double(input_name, )\n--\n\n "},
    {"input_string", input_string_wrapper, METH_VARARGS, "input_string(input_name, )\n--\n\n "},
    {"input_data", input_data_wrapper, METH_VARARGS, "input_data(input_name, )\n--\n\n "},

    //read output per type
    {"output_bool", output_bool_wrapper, METH_VARARGS, "output_bool(output_name, )\n--\n\n "},
    {"output_int", output_int_wrapper, METH_VARARGS, "output_int(output_name, )\n--\n\n "},
    {"output_double", output_double_wrapper, METH_VARARGS, "output_double(output_name, )\n--\n\n "},
    {"output_string", output_string_wrapper, METH_VARARGS, "output_string(output_name, )\n--\n\n "},
    {"output_data", output_data_wrapper, METH_VARARGS, "output_data(output_name, )\n--\n\n "},

    //read parameter per type
    {"parameter_bool", parameter_bool_wrapper, METH_VARARGS, "parameter_bool(parameter_name, )\n--\n\n "},
    {"parameter_int", parameter_int_wrapper, METH_VARARGS, "parameter_int(parameter_name, )\n--\n\n "},
    {"parameter_double", parameter_double_wrapper, METH_VARARGS, "parameter_double(parameter_name, )\n--\n\n "},
    {"parameter_string", parameter_string_wrapper, METH_VARARGS, "parameter_string(parameter_name, )\n--\n\n "},
    {"parameter_data", parameter_data_wrapper, METH_VARARGS, "parameter_data(parameter_name, )\n--\n\n "},

    //write input per type
    {"input_set_bool", input_set_bool_wrapper, METH_VARARGS, "input_set_bool(input_name, value, )\n--\n\n "},
    {"input_set_int", input_set_int_wrapper, METH_VARARGS,  "input_set_int(input_name, value, )\n--\n\n "},
    {"input_set_double", input_set_double_wrapper, METH_VARARGS,  "input_set_double(input_name, value, )\n--\n\n "},
    {"input_set_string", input_set_string_wrapper, METH_VARARGS,  "input_set_string(input_name, value, )\n--\n\n "},
    {"input_set_impulsion", input_set_impulsion_wrapper, METH_VARARGS,  "input_set_impulsion(input_name, value, )\n--\n\n "},
    {"input_set_data", input_set_data_wrapper, METH_VARARGS,  "input_set_data(input_name, value, )\n--\n\n "},

    //write output per type
    {"output_set_bool", output_set_bool_wrapper, METH_VARARGS, "output_set_bool(output_name, value, )\n--\n\n "},
    {"output_set_int", output_set_int_wrapper, METH_VARARGS, "output_set_int(output_name, value, )\n--\n\n "},
    {"output_set_double", output_set_double_wrapper, METH_VARARGS, "output_set_double(output_name, value, )\n--\n\n "},
    {"output_set_string", output_set_string_wrapper, METH_VARARGS, "output_set_string(output_name, value, )\n--\n\n "},
    {"output_set_impulsion", output_set_impulsion_wrapper, METH_VARARGS, "output_set_impulsion(output_name, value, )\n--\n\n "},
    {"output_set_data", output_set_data_wrapper, METH_VARARGS, "output_set_data(output_name, value, )\n--\n\n "},

    //write Parameter per type
    {"parameter_set_bool", parameter_set_bool_wrapper, METH_VARARGS, "parameter_set_bool(parameter_name, value, )\n--\n\n "},
    {"parameter_set_int", parameter_set_int_wrapper, METH_VARARGS, "parameter_set_int(parameter_name, value, )\n--\n\n "},
    {"parameter_set_double", parameter_set_double_wrapper, METH_VARARGS, "parameter_set_double(parameter_name, value, )\n--\n\n "},
    {"parameter_set_string", parameter_set_string_wrapper, METH_VARARGS, "parameter_set_string(parameter_name, value, )\n--\n\n "},
    {"parameter_set_data", parameter_set_data_wrapper, METH_VARARGS, "parameter_set_data(parameter_name, value, )\n--\n\n "},

    //check IOP type
    {"input_type", input_type_wrapper, METH_VARARGS, "input_type(input_name, )\n--\n\n "},
    {"output_type", output_type_wrapper, METH_VARARGS, "output_type(output_name, )\n--\n\n "},
    {"parameter_type", parameter_type_wrapper, METH_VARARGS, "parameter_type(parameter_name, )\n--\n\n "},

    //get number of IOP
    {"input_count", input_count_wrapper, METH_NOARGS, "input_count()\n--\n\n "},
    {"output_count", output_count_wrapper, METH_NOARGS, "output_count()\n--\n\n "},
    {"parameter_count", parameter_count_wrapper, METH_NOARGS, "parameter_count()\n--\n\n "},

    //check existence of IOP
    {"input_exists", input_exists_wrapper, METH_VARARGS, "input_exists(input_name, )\n--\n\n "},
    {"output_exists", output_exists_wrapper, METH_VARARGS, "output_exists(output_name, )\n--\n\n "},
    {"parameter_exists", parameter_exists_wrapper, METH_VARARGS, "parameter_exists(parameter_name, )\n--\n\n "},

    // get Iop list
    {"input_list", input_list_wrapper, METH_NOARGS, "input_list()\n--\n\n "},
    {"output_list", output_list_wrapper, METH_NOARGS, "output_list()\n--\n\n "},
    {"parameter_list", parameter_list_wrapper, METH_NOARGS, "parameter_list()\n--\n\n "},

    //mute or unmute an IOP
    {"output_mute", output_mute_wrapper, METH_VARARGS, "output_mute(output_name, )\n--\n\n "},
    {"output_unmute", output_unmute_wrapper, METH_VARARGS, "output_unmute(output_name, )\n--\n\n "},
    {"output_is_muted", output_is_muted_wrapper, METH_VARARGS,  "output_is_muted(output_name, )\n--\n\n "},

    //load definition
    {"definition_load_str", definition_load_str_wrapper, METH_VARARGS, "definition_load_str(json_str, )\n--\n\n "},
    {"definition_load_file", definition_load_file_wrapper, METH_VARARGS, "definition_load_file(file_path, )\n--\n\n "},
    {"clear_definition", clear_definition_wrapper, METH_NOARGS, "clear_definition()\n--\n\n "},

    //get information about definition
    {"definition_json", definition_json_wrapper, METH_NOARGS, "definition_json()\n--\n\n "},
    {"definition_description", definition_description_wrapper, METH_NOARGS, "definition_description()\n--\n\n "},
    {"definition_version", definition_version_wrapper, METH_NOARGS, "definition_version()\n--\n\n "},

    //set definition
    {"definition_set_description", definition_set_description_wrapper, METH_VARARGS, "definition_set_description(description, )\n--\n\n "},
    {"definition_set_version", definition_set_version_wrapper, METH_VARARGS, "definition_set_version(version, )\n--\n\n "},

    //remove IOP
    {"input_remove", input_remove_wrapper, METH_VARARGS, "input_remove(input_name, )\n--\n\n "},
    {"output_remove", output_remove_wrapper, METH_VARARGS, "output_remove(output_name, )\n--\n\n "},
    {"parameter_remove", parameter_remove_wrapper, METH_VARARGS, "parameter_remove(parameter_name, )\n--\n\n "},

    //createIOP
    {"input_create", input_create_wrapper, METH_VARARGS, "input_create(input_name, value_type, value, )\n--\n\n "},
    {"output_create", output_create_wrapper, METH_VARARGS, "output_create(output_name, value_type, value, )\n--\n\n "},
    {"parameter_create", parameter_create_wrapper, METH_VARARGS, "parameter_create(parameter_name, value_type, value, )\n--\n\n "},

    //clear IOP
    {"clear_input", clear_input_wrapper, METH_VARARGS, "clear_input(input_name, )\n--\n\n "},
    {"clear_output", clear_output_wrapper, METH_VARARGS, "clear_output(output_name, )\n--\n\n "},
    {"clear_parameter", clear_parameter_wrapper, METH_VARARGS, "clear_parameter(parameter_name, )\n--\n\n "},
    {"clear_context", igs_clear_context_wrapper, METH_NOARGS, "clear_context()\n--\n\n "},

    //load mapping
    {"mapping_load_str", mapping_load_str_wrapper, METH_VARARGS,"mapping_load_str(json_str, )\n--\n\n "},
    {"mapping_load_file", mapping_load_file_wrapper, METH_VARARGS, "mapping_load_file(file_path, )\n--\n\n "},
    {"clear_mappings", clear_mappings_wrapper, METH_NOARGS, "clear_mappings()\n--\n\n "},

    //get information about mapping
    {"mapping_json", mapping_json_wrapper, METH_NOARGS, "mapping_json()\n--\n\n "},
    {"mapping_count", mapping_count_wrapper, METH_NOARGS, "mapping_count()\n--\n\n "},
    {"mapping_add", mapping_add_wrapper, METH_VARARGS, "mapping_add(from_our_input, to_agent, with_output, )\n--\n\n "},

    //remove mapping
    {"mapping_remove_with_id", mapping_remove_with_id_wrapper, METH_VARARGS, "mapping_remove_with_id(id, )\n--\n\n "},
    {"mapping_remove_with_name", mapping_remove_with_name_wrapper, METH_VARARGS, "mapping_remove_with_name(from_our_input, to_agent, with_output, )\n--\n\n "},
    {"clear_mappings_with_agent", clear_mappings_with_agent_wrapper, METH_VARARGS, "clear_mappings_with_agent(agent_name, )\n--\n\n "},
    {"clear_mappings_for_input", clear_mappings_for_input_wrapper, METH_VARARGS, "clear_mappings_for_input(input_name, )\n--\n\n "},
    {"mapping_set_outputs_request", mapping_set_outputs_request_wrapper, METH_VARARGS, "mapping_set_outputs_request(notify, )\n--\n\n "},
    {"mapping_outputs_request", mapping_outputs_request_wrapper, METH_NOARGS, "mapping_outputs_request()\n--\n\n "},

    //split management
    {"split_count", (PyCFunction) split_count_wrapper, METH_NOARGS, "split_count()\n--\n\n "},
    {"split_add", (PyCFunction) split_add_wrapper, METH_VARARGS, "split_add(from_our_input, to_agent, with_output, )\n--\n\n "},
    {"split_remove_with_id", (PyCFunction) split_remove_with_id_wrapper, METH_VARARGS, "split_remove_with_id(id, )\n--\n\n "},
    {"split_remove_with_name", (PyCFunction) split_remove_with_name_wrapper, METH_VARARGS, "split_remove_with_name(from_our_input, to_agent, with_output, )\n--\n\n "},

    //Command line for the agent can be passed here for inclusion in the agent's headers. If not set, header is initialized with exec path.
    {"set_command_line", set_command_line_wrapper, METH_VARARGS, "set_command_line(command_line, )\n--\n\n "},
    {"command_line", command_line_wrapper, METH_NOARGS, "command_line()\n--\n\n "},

    //Observe Iop, freeze and forced stop
    {"observe_input", (PyCFunction) observe_input_wrapper, METH_VARARGS, "observe_input(input_name, callback, my_data, )\n--\n\n "},
    {"observe_output", (PyCFunction) observe_output_wrapper, METH_VARARGS, "observe_output(output_name, callback, my_data, )\n--\n\n "},
    {"observe_parameter", (PyCFunction) observe_parameter_wrapper, METH_VARARGS, "observe_parameter(parameter_name, callback, my_data, )\n--\n\n "},
    {"observe_freeze", observe_freeze_wrapper, METH_VARARGS, "observe_freeze(callback, my_data, )\n--\n\n "},
    {"observe_forced_stop", observe_forced_stop_wrapper, METH_VARARGS, "observe_forced_stop(callback, my_data, )\n--\n\n "},
    {"observe_mute", observe_mute_wrapper, METH_VARARGS, "observe_mute(callback, my_data, )\n--\n\n "},
    // AgentEvent
    {"observe_agent_events", observe_agent_events_wrapper, METH_VARARGS, "observe_agent_events(callback, my_data, )\n--\n\n "},

    //Resources file management
    {"definition_set_path", definition_set_path_wrapper, METH_VARARGS, "definition_set_path(path, )\n--\n\n "},
    {"mapping_set_path", mapping_set_path_wrapper, METH_VARARGS, "mapping_set_path(path, )\n--\n\n "},
    {"definition_save", definition_save_wrapper, METH_NOARGS, "definition_save()\n--\n\n "},
    {"mapping_save", mapping_save_wrapper, METH_NOARGS, "mapping_save()\n--\n\n "},
    {"version", version_wrapper, METH_NOARGS, "version()\n--\n\n "},
    {"protocol", protocol_wrapper, METH_NOARGS, "protocol()\n--\n\n "},

    // Advanced
    {"net_set_publishing_port", net_set_publishing_port_wrapper, METH_VARARGS, "net_set_publishing_port(port, )\n--\n\n "},
    {"net_set_discovery_interval", net_set_discovery_interval_wrapper, METH_VARARGS, "net_set_discovery_interval(interval, )\n--\n\n "},
    {"channel_join", channel_join_wrapper, METH_VARARGS, "channel_join(channel, )\n--\n\n "},
    {"channel_leave", channel_leave_wrapper, METH_VARARGS, "channel_leave(channel, )\n--\n\n "},
    {"channel_shout_str", channel_shout_str_wrapper, METH_VARARGS, "channel_shout_str(channel, msg, )\n--\n\n "},
    {"channel_shout_data", channel_shout_data_wrapper, METH_VARARGS, "channel_shout_data(channel, data, )\n--\n\n "},
    {"channel_whisper_str", channel_whisper_str_wrapper, METH_VARARGS, "channel_whisper_str(agent_name_or_agent_id_or_peer_id, msg, )\n--\n\n "},
    {"channel_whisper_data", channel_whisper_data_wrapper, METH_VARARGS, "channel_whisper_data(agent_name_or_agent_id_or_peer_id, data, )\n--\n\n "},
    {"peer_add_header", peer_add_header_wrapper, METH_VARARGS, "peer_add_header(key, value, )\n--\n\n "},
    {"peer_remove_header", peer_remove_header_wrapper, METH_VARARGS, "peer_remove_header(channel, )\n--\n\n "},
    // {"observe_channels", observe_channels_wrapper, METH_VARARGS, ""}, FIXME: callback is very ZMQ dependent (zhash and zmsg).
    {"net_devices_list", net_devices_list_wrapper, METH_NOARGS, "net_devices_list()\n--\n\n "},
    {"net_addresses_list", net_addresses_list_wrapper, METH_NOARGS, "net_addresses_list()\n--\n\n "},

    // Services
    {"service_call", service_call_wrapper, METH_VARARGS, "service_call(agent_name_or_uuid, service_name, tuple_arguments, token, )\n--\n\n "},
    {"service_init", service_init_wrapper, METH_VARARGS, "service_init(service_name, callback, my_data, )\n--\n\n "},
    {"service_remove", service_remove_wrapper, METH_VARARGS, "service_remove(service_name, )\n--\n\n "},
    {"service_arg_add", service_arg_add_wrapper, METH_VARARGS, "service_arg_add(service_name, argument_name, argument_type, )\n--\n\n "},
    {"service_arg_remove", service_arg_remove_wrapper, METH_VARARGS, "service_arg_remove(service_name, arg_name, )\n--\n\n "},

    {"service_reply_add", service_reply_add_wrapper, METH_VARARGS, "service_reply_add(service_name, reply_name, )\n--\n\n "},
    {"service_reply_remove", service_reply_remove_wrapper, METH_VARARGS, "service_reply_remove(service_name, reply_name, )\n--\n\n "},
    {"service_reply_arg_add", service_reply_arg_add_wrapper, METH_VARARGS, "service_reply_arg_add(service_name, reply_name, arg_name, type, )\n--\n\n "},
    {"service_reply_arg_remove", service_reply_arg_remove_wrapper, METH_VARARGS, "service_reply_arg_remove(service_name, reply_name, arg_name, )\n--\n\n "},

    {"service_count", service_count_wrapper, METH_VARARGS, "service_count()\n--\n\n "},
    {"service_exists", service_exists_wrapper, METH_VARARGS, "service_exists(service_name, )\n--\n\n "},
    {"service_list", service_list_wrapper, METH_VARARGS, "service_list()\n--\n\n "},
    {"service_args_count", service_args_count_wrapper, METH_VARARGS, "service_args_count(service_name, )\n--\n\n "},
    {"service_args_list", service_args_list_wrapper, METH_VARARGS, "service_args_list(service_name, )\n--\n\n "},
    {"service_arg_exists", service_arg_exists_wrapper, METH_VARARGS, "service_arg_exists(service_name, argument_name, )\n--\n\n "},
    {"service_has_replies", service_has_replies_wrapper, METH_VARARGS, "service_has_replies(service_name, )\n--\n\n "},
    {"service_has_reply", service_has_reply_wrapper, METH_VARARGS, "service_has_reply(service_name, reply_name, )\n--\n\n "},
    {"service_reply_names", service_reply_names_wrapper, METH_VARARGS, "service_reply_names(service_name, )\n--\n\n "},
    {"service_reply_args_count", service_reply_args_count_wrapper, METH_VARARGS, "service_args_count(service_name, )\n--\n\n "},
    {"service_reply_args_list", service_reply_args_list_wrapper, METH_VARARGS, "service_args_list(service_name, )\n--\n\n "},
    {"service_reply_arg_exists", service_reply_arg_exists_wrapper, METH_VARARGS, "service_arg_exists(service_name, argument_name, )\n--\n\n "},


    //logs and debug messages
    {"log_set_console", log_set_console_wrapper, METH_VARARGS, "log_set_console(verbose, )\n--\n\n "},
    {"log_console", log_console_wrapper, METH_NOARGS, "log_console()\n--\n\n "},
    {"log_set_syslog", log_set_syslog_wrapper, METH_VARARGS, "log_set_syslog(verbose, )\n--\n\n "},
    {"log_syslog", log_syslog_wrapper, METH_NOARGS, "log_syslog()\n--\n\n "},
    {"log_set_stream", log_set_stream_wrapper, METH_VARARGS, "log_set_stream(verbose, )\n--\n\n "},
    {"log_stream", log_stream_wrapper, METH_NOARGS, "log_stream()\n--\n\n "},
    {"log_set_file", log_set_file_wrapper, METH_VARARGS, "log_set_file(verbose, file, )\n--\n\n "},
    {"log_file", log_file_wrapper, METH_NOARGS, "log_file()\n--\n\n "},
    {"log_set_console_color", log_set_console_color_wrapper, METH_VARARGS, "log_set_console_color(verbose, )\n--\n\n "},
    {"log_console_color", log_console_color_wrapper, METH_NOARGS, "log_console_color()\n--\n\n "},
    {"log_set_file_path", log_set_file_path_wrapper, METH_VARARGS, "log_set_file_path(path, )\n--\n\n "},
    {"log_file_path", log_file_path_wrapper, METH_NOARGS, "log_file_path()\n--\n\n "},
    {"log_set_console_level", log_set_console_level_wrapper, METH_VARARGS, "log_set_console_level(level, )\n--\n\n "},
    {"log_console_level", log_console_level_wrapper, METH_VARARGS, "log_console_level()\n--\n\n "},
    {"log_set_file_level", log_set_file_level_wrapper, METH_VARARGS, "log_set_file_level(log_set_file_level, )\n--\n\n "},
    {"log_set_file_max_line_length", log_set_file_max_line_length_wrapper, METH_VARARGS,
        "log_set_file_max_line_length(size, )\n--\n\n "},
    {"log_include_data", log_include_data_wrapper, METH_VARARGS, "log_include_data(enable, )\n--\n\n "},
    {"log_include_services", igs_log_include_services_wrapper, METH_VARARGS, "log_include_services(enable, )\n--\n\n "},
    {"log_no_warning_if_undefined_service", igs_log_no_warning_if_undefined_service_wrapper, METH_VARARGS, "log_no_warning_if_undefined_service(enable, )\n--\n\n "},
    {"trace", trace_wrapper, METH_VARARGS, "trace(value, )\n--\n\n "},
    {"debug", debug_wrapper, METH_VARARGS, "debug(value, )\n--\n\n "},
    {"info", info_wrapper, METH_VARARGS, "info(value, )\n--\n\n "},
    {"warn", warn_wrapper, METH_VARARGS, "warn(value, )\n--\n\n "},
    {"error", error_wrapper, METH_VARARGS, "error(value, )\n--\n\n "},
    {"fatal", fatal_wrapper, METH_VARARGS, "fatal(value, )\n--\n\n "},

    // Election
    {"election_join", election_join_wrapper, METH_VARARGS, "election_join(election_name, )\n--\n\n "},
    {"election_leave", election_leave_wrapper, METH_VARARGS, "election_leave(election_name, )\n--\n\n "},

    // Broker
    {"broker_add", igs_broker_add_wrapper, METH_VARARGS, "broker_add(broker_endpoint, )\n--\n\n "},
    {"clear_brokers", igs_clear_brokers_wrapper, METH_NOARGS, "clear_brokers()\n--\n\n "},
    {"broker_enable_with_endpoint", igs_broker_enable_with_endpoint_wrapper, METH_VARARGS, "broker_enable_with_endpoint(our_broker_endpoint, )\n--\n\n "},
    {"broker_set_advertized_endpoint", igs_broker_set_advertized_endpoint_wrapper, METH_VARARGS, "broker_set_advertized_endpoint(advertised_endpoint, )\n--\n\n "},
    {"start_with_brokers", igs_start_with_brokers_wrapper, METH_VARARGS, "start_with_brokers(agent_endpoint, )\n--\n\n "},

    // Security
    {"enable_security", igs_enable_security_wrapper, METH_VARARGS, "enable_security(private_certificate_file, public_certificates_directory, )\n--\n\n "},
    {"disable_security", igs_disable_security_wrapper, METH_NOARGS, "disable_security()\n--\n\n "},
    {"broker_add_secure", igs_broker_add_secure_wrapper, METH_VARARGS, "broker_add_secure(broker_endpoint, path_to_public_certificate_for_broker, )\n--\n\n "},

    // IPC
    {"set_ipc", igs_set_ipc_wrapper, METH_VARARGS, "set_ipc(allow, )\n--\n\n "},
    {"has_ipc", igs_has_ipc_wrapper, METH_NOARGS, "has_ipc()\n--\n\n "},
#if defined (__UNIX__)
    {"set_ipc_dir", igs_set_ipc_dir_wrapper, METH_VARARGS, "set_ipc_dir(path, )\n--\n\n "},
    {"ipc_dir", igs_ipc_dir_wrapper, METH_NOARGS, "ipc_dir()\n--\n\n "},
#endif

    // net
    {"net_set_log_stream_port", igs_net_set_log_stream_port_wrapper, METH_VARARGS, "net_set_log_stream_port(port, )\n--\n\n "},
    {"net_set_timeout", igs_net_set_timeout_wrapper, METH_VARARGS, "net_set_timeout(duration, )\n--\n\n "},
#if defined (__UNIX__)
    {"net_raise_sockets_limit", igs_net_raise_sockets_limit_wrapper, METH_NOARGS, "net_raise_sockets_limit()\n--\n\n "},
#endif
    {"net_set_high_water_marks", igs_net_set_high_water_marks_wrapper, METH_VARARGS, "net_set_high_water_marks(hwm_value, )\n--\n\n "},
    {"net_performance_check", igs_net_performance_check_wrapper, METH_VARARGS, "net_performance_check(peer_id, msg_size, msg_nbr, )\n--\n\n "},

    // Monitor
    {"monitor_start", igs_monitor_start_wrapper, METH_VARARGS, "monitor_start(period, )\n--\n\n "},
    {"monitor_start_with_network", igs_monitor_start_with_network_wrapper, METH_VARARGS, "monitor_start_with_network(period, network_device, port, )\n--\n\n "},
    {"monitor_stop", igs_monitor_stop_wrapper, METH_NOARGS, "monitor_stop()\n--\n\n "},
    {"monitor_is_running", igs_monitor_is_running_wrapper, METH_NOARGS, "monitor_is_running()\n--\n\n "},
    {"monitor_set_start_stop", igs_monitor_set_start_stop_wrapper, METH_VARARGS, "monitor_set_start_stop(flag, )\n--\n\n "},
    {"observe_monitor", igs_observe_monitor_wrapper, METH_VARARGS, "observe_monitor(callback, my_data, )\n--\n\n "},

    // Replay
    {"replay_init", igs_replay_init_wrapper, METH_VARARGS,
        "replay_init(log_file_path, speed, start_time, wait_for_start, replay_mode, agent, )\n--\n\n "},
    {"replay_start", igs_replay_start_wrapper, METH_NOARGS, "replay_start()\n--\n\n "},
    {"replay_pause", igs_replay_pause_wrapper, METH_VARARGS, "replay_pause(pause, )\n--\n\n "},
    {"replay_terminate", igs_replay_terminate_wrapper, METH_NOARGS, "replay_terminate()\n--\n\n "},

    // Constraints and description
    {"constraints_enforce", igs_constraints_enforce_wrapper, METH_VARARGS, "constraints_enforce(enforce, )\n--\n\n "},
    {"input_add_constraint", igs_input_add_constraint_wrapper, METH_VARARGS, "input_add_constraint(name, constraint, )\n--\n\n "},
    {"output_add_constraint", igs_output_add_constraint_wrapper, METH_VARARGS, "output_add_constraint(name, constraint, )\n--\n\n "},
    {"parameter_add_constraint", igs_parameter_add_constraint_wrapper, METH_VARARGS, "parameter_add_constraint(name, constraint, )\n--\n\n "},
    {"input_set_description", igs_input_set_description_wrapper, METH_VARARGS, "input_set_description(name, description, )\n--\n\n "},
    {"output_set_description", igs_output_set_description_wrapper, METH_VARARGS, "output_set_description(name, description, )\n--\n\n "},
    {"parameter_set_description", igs_parameter_set_description_wrapper, METH_VARARGS, "parameter_set_description(name, description, )\n--\n\n "},

    // Timers
    {"timer_start", igs_timer_start_wrapper, METH_VARARGS, "timer_start(delay,times, callback, my_data, )\n--\n\n "},
    {"timer_stop", igs_timer_stop_wrapper, METH_VARARGS, "timer_stop(id, )\n--\n\n "},

    // FIXME: igs_set_command_line_from_args is redundant with igs_set_command_line and python's argc argv is not compatible with c's one
    // FIXME: igs_pipe_to_ingescape is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igs_output_set_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igs_input_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igs_channel_shout_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igs_channel_whisper_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igs_zmq_authenticator is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented

    {NULL, NULL, 0, NULL}       /* Sentinel */
};


////////////////////////////////////////////////////
// Defining all the methods of the Agent Object
// First new, destroy and __init__ the ingescape's methods

static void Agent_dealloc(AgentObject *self)
{
    if(self->agent)
    {
        if (igsagent_is_activated(self->agent))
            igsagent_deactivate(self->agent);
        igsagent_destroy(&(self->agent));
    }

    {
        agentObserveEventsCB_t *it = NULL;
        do {
            DL_FOREACH(agentObserveEventsCBList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentObserveEventsCBList, it);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
    }
    {
        agentobserve_iop_cb_t *it = NULL;
        do {
            DL_FOREACH(agentobserve_iop_cbList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentobserve_iop_cbList, it);
                free(it->nameArg);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
    }
    {
        agentObserveCB_t *it = NULL;
        do {
            DL_FOREACH(agentObserveCBList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentObserveCBList, it);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
    }
    {
        agentMuteCB_t *it = NULL;
        do {
            DL_FOREACH(agentMuteCBList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentMuteCBList, it);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
    }
    {
        agentServiceCB_t *it = NULL;
        do {
            DL_FOREACH(agentServiceCBList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentServiceCBList, it);
                free(it->serviceName);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
    }

    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *Agent_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    AgentObject *self;
    self = (AgentObject *) type->tp_alloc(type, 0);
    if (self != NULL)
        self->agent = NULL;
    return (PyObject *)self;
}

static int Agent_init(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name", "activate_immediately", NULL};
    char *name = NULL;
    PyObject* activate_immediately = Py_False;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "s|O", kwlist,
                                     &name, &activate_immediately))
        return -1;
    if(activate_immediately == Py_True)
        self->agent = igsagent_new(name, true);
    else
        self->agent = igsagent_new(name, false);
    return 0;
}

static PyMethodDef Agent_methods[] = {

    ////////////////////////////////////////
    // Agent creation/destruction/activation
    {"activate", (PyCFunction) Agent_activate, METH_NOARGS, "activate(self, )\n--\n\n "},
    {"deactivate", (PyCFunction) Agent_deactivate, METH_NOARGS, "deactivate(self, )\n--\n\n "},
    {"is_activated", (PyCFunction) Agent_is_activated, METH_NOARGS, "is_activated(self, )\n--\n\n "},
    {"observe", (PyCFunction) Agent_observe, METH_VARARGS, "observe(self, callback, my_data, )\n--\n\n "},

    ////////////////
    // Agent logging
    {"trace", (PyCFunction) Agent_trace, METH_VARARGS, "trace(self, msg, )\n--\n\n "},
    {"debug", (PyCFunction) Agent_debug, METH_VARARGS, "debug(self, msg, )\n--\n\n "},
    {"info", (PyCFunction) Agent_info, METH_VARARGS, "info(self, msg, )\n--\n\n "},
    {"warn", (PyCFunction) Agent_warn, METH_VARARGS, "warn(self, msg, )\n--\n\n "},
    {"error", (PyCFunction) Agent_error, METH_VARARGS, "error(self, msg, )\n--\n\n "},
    {"fatal", (PyCFunction) Agent_fatal, METH_VARARGS, "fatal(self, msg, )\n--\n\n "},

    ///////////////////////////////////////////
    // Agent initialization, control and events
    {"name", (PyCFunction) Agent_name, METH_NOARGS, "name(self, )\n--\n\n "},
    {"set_name", (PyCFunction) Agent_set_name, METH_VARARGS, "set_name(self, name, )\n--\n\n "},
    {"family", (PyCFunction) Agent_family, METH_NOARGS, "family(self, )\n--\n\n "},
    {"set_family", (PyCFunction) Agent_set_family, METH_VARARGS, "set_family(self, family, )\n--\n\n "},
    {"uuid", (PyCFunction) Agent_uuid, METH_NOARGS, "uuid(self, )\n--\n\n "},
    {"state", (PyCFunction) Agent_state, METH_NOARGS, "state(self, )\n--\n\n "},
    {"set_state", (PyCFunction) Agent_set_state, METH_VARARGS, "set_state(self, state, )\n--\n\n "},
    {"mute", (PyCFunction) Agent_mute, METH_NOARGS, "mute(self, )\n--\n\n "},
    {"unmute", (PyCFunction) Agent_unmute, METH_NOARGS, "unmute(self, )\n--\n\n "},
    {"is_muted", (PyCFunction) Agent_is_muted, METH_NOARGS, "is_muted(self, )\n--\n\n "},
    {"observe_mute", (PyCFunction) Agent_observe_mute, METH_VARARGS, "observe_mute(self, callback, my_data, )\n--\n\n "},
    {"observe_agent_event", (PyCFunction) Agent_observe_agent_event, METH_VARARGS, "observe_agent_event(self, callback, my_data, )\n--\n\n "},

    //////////////////////////////////////////////////////////////////////////////////
    // Editing & inspecting definitions, adding and removing inputs/outputs/parameters
    {"definition_load_str", (PyCFunction) Agent_definition_load_str, METH_VARARGS, "definition_load_str(self, definition_json, )\n--\n\n "},
    {"definition_load_file", (PyCFunction) Agent_definition_load_file, METH_VARARGS, "definition_load_file(self, path, )\n--\n\n "},
    {"clear_definition", (PyCFunction) Agent_clear_definition, METH_NOARGS, "clear_definition(self, )\n--\n\n "},
    {"definition_json", (PyCFunction) Agent_definition_json, METH_NOARGS, "definition_json(self, )\n--\n\n "},
    {"definition_description", (PyCFunction) Agent_definition_description, METH_NOARGS, "definition_description(self, )\n--\n\n "},
    {"definition_version", (PyCFunction) Agent_definition_version, METH_NOARGS, "definition_version(self, )\n--\n\n "},
    {"definition_set_description", (PyCFunction) Agent_definition_set_description, METH_VARARGS, "definition_set_description(self, description, )\n--\n\n "},
    {"definition_set_version", (PyCFunction) Agent_definition_set_version, METH_VARARGS, "definition_set_version(self, version, )\n--\n\n "},

    {"input_create", (PyCFunction) Agent_input_create, METH_VARARGS, "input_create(self, name, value_type, value, )\n--\n\n "},
    {"output_create", (PyCFunction) Agent_output_create, METH_VARARGS, "output_create(self, name, value_type, value, )\n--\n\n "},
    {"parameter_create", (PyCFunction) Agent_parameter_create, METH_VARARGS, "parameter_create(self, name, value_type, value, )\n--\n\n "},

    {"input_remove", (PyCFunction) Agent_input_remove, METH_VARARGS, "input_remove(self, name, )\n--\n\n "},
    {"output_remove", (PyCFunction) Agent_output_remove, METH_VARARGS, "output_remove(self, name, )\n--\n\n "},
    {"parameter_remove", (PyCFunction) Agent_parameter_remove, METH_VARARGS, "parameter_remove(self, name, )\n--\n\n "},

    {"input_type", (PyCFunction) Agent_input_type, METH_VARARGS, "input_type(self, name, )\n--\n\n "},
    {"output_type", (PyCFunction) Agent_output_type, METH_VARARGS, "output_type(self, name, )\n--\n\n "},
    {"parameter_type", (PyCFunction) Agent_parameter_type, METH_VARARGS, "parameter_type(self, name, )\n--\n\n "},

    {"input_count", (PyCFunction) Agent_input_count, METH_NOARGS, "input_count(self, )\n--\n\n "},
    {"output_count", (PyCFunction) Agent_output_count, METH_NOARGS, "output_count(self, )\n--\n\n "},
    {"parameter_count", (PyCFunction) Agent_parameter_count, METH_NOARGS, "parameter_count(self, )\n--\n\n "},

    {"input_list", (PyCFunction) Agent_input_list, METH_NOARGS, "input_list(self, )\n--\n\n "},
    {"output_list", (PyCFunction) Agent_output_list, METH_NOARGS, "output_list(self, )\n--\n\n "},
    {"parameter_list", (PyCFunction) Agent_parameter_list, METH_NOARGS, "parameter_list(self, )\n--\n\n "},

    {"input_exists", (PyCFunction) Agent_input_exists, METH_VARARGS, "input_exists(self, name, )\n--\n\n "},
    {"output_exists", (PyCFunction) Agent_output_exists, METH_VARARGS, "output_exists(self, name, )\n--\n\n "},
    {"parameter_exists", (PyCFunction) Agent_parameter_exists, METH_VARARGS, "parameter_exists(self, name, )\n--\n\n "},

    ////////////////////////////////////////////////////////////
    // Reading and writing inputs/outputs/parameters, a.k.a IOPs
    {"input_bool", (PyCFunction) Agent_input_bool, METH_VARARGS, "input_bool(self, name, )\n--\n\n "},
    {"input_int", (PyCFunction) Agent_input_int, METH_VARARGS, "input_int(self, name, )\n--\n\n "},
    {"input_double", (PyCFunction) Agent_input_double, METH_VARARGS, "input_double(self, name, )\n--\n\n "},
    {"input_string", (PyCFunction) Agent_input_string, METH_VARARGS, "input_string(self, name, )\n--\n\n "},
    {"input_data", (PyCFunction) Agent_input_data, METH_VARARGS, "input_data(self, name, )\n--\n\n "},

    {"output_bool", (PyCFunction) Agent_output_bool, METH_VARARGS, "output_bool(self, name, )\n--\n\n "},
    {"output_int", (PyCFunction) Agent_output_int, METH_VARARGS, "output_int(self, name, )\n--\n\n "},
    {"output_double", (PyCFunction) Agent_output_double, METH_VARARGS, "output_double(self, name, )\n--\n\n "},
    {"output_string", (PyCFunction) Agent_output_string, METH_VARARGS, "output_string(self, name, )\n--\n\n "},
    {"output_data", (PyCFunction) Agent_output_data, METH_VARARGS, "output_data(self, name, )\n--\n\n "},

    {"parameter_bool", (PyCFunction) Agent_parameter_bool, METH_VARARGS, "parameter_bool(self, name, )\n--\n\n "},
    {"parameter_int", (PyCFunction) Agent_parameter_int, METH_VARARGS, "parameter_int(self, name, )\n--\n\n "},
    {"parameter_double", (PyCFunction) Agent_parameter_double, METH_VARARGS, "parameter_double(self, name, )\n--\n\n "},
    {"parameter_string", (PyCFunction) Agent_parameter_string, METH_VARARGS, "parameter_string(self, name, )\n--\n\n "},
    {"parameter_data", (PyCFunction) Agent_parameter_data, METH_VARARGS, "parameter_data(self, name, )\n--\n\n "},

    {"input_set_bool", (PyCFunction) Agent_input_set_bool, METH_VARARGS, "input_set_bool(self, name, value, )\n--\n\n "},
    {"input_set_int", (PyCFunction) Agent_input_set_int, METH_VARARGS, "input_set_int(self, name, value, )\n--\n\n "},
    {"input_set_double", (PyCFunction) Agent_input_set_double, METH_VARARGS, "input_set_double(self, name, value, )\n--\n\n "},
    {"input_set_string", (PyCFunction) Agent_input_set_string, METH_VARARGS, "input_set_string(self, name, value, )\n--\n\n "},
    {"input_set_impulsion", (PyCFunction) Agent_input_set_impulsion, METH_VARARGS, "input_set_impulsion(self, name, )\n--\n\n "},
    {"input_set_data", (PyCFunction) Agent_input_set_data, METH_VARARGS, "input_set_data(self, name, value, )\n--\n\n "},

    {"output_set_bool", (PyCFunction) Agent_output_set_bool, METH_VARARGS, "output_set_bool(self, name, value, )\n--\n\n "},
    {"output_set_int", (PyCFunction) Agent_output_set_int, METH_VARARGS, "output_set_int(self, name, value, )\n--\n\n "},
    {"output_set_double", (PyCFunction) Agent_output_set_double, METH_VARARGS, "output_set_double(self, name, value, )\n--\n\n "},
    {"output_set_string", (PyCFunction) Agent_output_set_string, METH_VARARGS, "output_set_string(self, name, value, )\n--\n\n "},
    {"output_set_impulsion", (PyCFunction) Agent_output_set_impulsion, METH_VARARGS, "output_set_impulsion(self, name, )\n--\n\n "},
    {"output_set_data", (PyCFunction) Agent_output_set_data, METH_VARARGS, "output_set_data(self, name, value, )\n--\n\n "},

    {"parameter_set_bool", (PyCFunction) Agent_parameter_set_bool, METH_VARARGS, "parameter_set_bool(self, name, value, )\n--\n\n "},
    {"parameter_set_int", (PyCFunction) Agent_parameter_set_int, METH_VARARGS, "parameter_set_int(self, name, value, )\n--\n\n "},
    {"parameter_set_double", (PyCFunction) Agent_parameter_set_double, METH_VARARGS, "parameter_set_double(self, name, value, )\n--\n\n "},
    {"parameter_set_string", (PyCFunction) Agent_parameter_set_string, METH_VARARGS, "parameter_set_string(self, name, value, )\n--\n\n "},
    {"parameter_set_data", (PyCFunction) Agent_parameter_set_data, METH_VARARGS, "parameter_set_data(self, name, value, )\n--\n\n "},

    {"clear_input", (PyCFunction) Agent_clear_input, METH_VARARGS, "clear_input(self, name, )\n--\n\n "},
    {"clear_output", (PyCFunction) Agent_clear_output, METH_VARARGS, "clear_output(self, name, )\n--\n\n "},
    {"clear_parameter", (PyCFunction) Agent_clear_parameter, METH_VARARGS, "clear_parameter(self, name, )\n--\n\n "},

    {"observe_input", (PyCFunction) Agent_observe_input, METH_VARARGS, "observe_input(self, name, callback, my_data, )\n--\n\n "},
    {"observe_output", (PyCFunction) Agent_observe_output, METH_VARARGS, "observe_output(self, name, callback, my_data, )\n--\n\n "},
    {"observe_parameter", (PyCFunction) Agent_observe_parameter, METH_VARARGS, "observe_parameter(self, name, callback, my_data, )\n--\n\n "},

    {"output_mute", (PyCFunction) Agent_output_mute, METH_VARARGS, "output_mute(self, name, )\n--\n\n "},
    {"output_unmute", (PyCFunction) Agent_output_unmute, METH_VARARGS, "output_unmute(self, name, )\n--\n\n "},
    {"output_is_muted", (PyCFunction) Agent_output_is_muted, METH_VARARGS, "output_is_muted(self, name, )\n--\n\n "},

    ////////////////////////////////
    // Mapping edition & inspection
    {"mapping_load_str", (PyCFunction) Agent_mapping_load_str, METH_VARARGS, "mapping_load_str(self, mapping_json, )\n--\n\n "},
    {"mapping_load_file", (PyCFunction) Agent_mapping_load_file, METH_VARARGS, "mapping_load_file(self, path, )\n--\n\n "},
    {"mapping_json", (PyCFunction) Agent_mapping_json, METH_NOARGS, "mapping_json(self, )\n--\n\n "},
    {"mapping_count", (PyCFunction) Agent_mapping_count, METH_NOARGS, "mapping_count(self, )\n--\n\n "},

    {"clear_mappings", (PyCFunction) Agent_clear_mappings, METH_NOARGS, "clear_mappings(self, )\n--\n\n "},
    {"clear_mappings_with_agent", (PyCFunction) Agent_clear_mappings_with_agent, METH_VARARGS, "clear_mappings_with_agent(self, agent_name, )\n--\n\n "},

    {"mapping_add", (PyCFunction) Agent_mapping_add, METH_VARARGS, "mapping_add(self, from_our_input, to_agent, with_output, )\n--\n\n "},
    {"mapping_remove_with_id", (PyCFunction) Agent_mapping_remove_with_id, METH_VARARGS, "mapping_remove_with_id(self, id, )\n--\n\n "},
    {"mapping_remove_with_name", (PyCFunction) Agent_mapping_remove_with_name, METH_VARARGS, "mapping_remove_with_name(self, from_our_input, to_agent, with_output, )\n--\n\n "},

    {"split_count", (PyCFunction) Agent_split_count, METH_NOARGS, "split_count(self, )\n--\n\n "},
    {"split_add", (PyCFunction) Agent_split_add, METH_VARARGS, "split_add(self, from_our_input, to_agent, with_output, )\n--\n\n "},
    {"split_remove_with_id", (PyCFunction) Agent_split_remove_with_id, METH_VARARGS, "split_remove_with_id(self, id, )\n--\n\n "},
    {"split_remove_with_name", (PyCFunction) Agent_split_remove_with_name, METH_VARARGS, "split_remove_with_name(self, from_our_input, to_agent, with_output, )\n--\n\n "},

    {"mapping_outputs_request", (PyCFunction) Agent_mapping_outputs_request, METH_VARARGS, "mapping_outputs_request(self, )\n--\n\n "},
    {"mapping_set_outputs_request", (PyCFunction) Agent_mapping_set_outputs_request, METH_NOARGS, "mapping_set_outputs_request(self, notify, )\n--\n\n "},

    ////////////////////////////////
    // Services edition & inspection
    {"service_call", (PyCFunction) Agent_service_call, METH_VARARGS, "service_call(self, agent_name_or_uuid, service_name, argument_tuple, token, )\n--\n\n "},
    {"service_init", (PyCFunction) Agent_service_init, METH_VARARGS, "service_init(self, service_name, callback, my_data, )\n--\n\n "},
    {"service_remove", (PyCFunction) Agent_service_remove, METH_VARARGS, "service_remove(self, service_name, )\n--\n\n "},
    {"service_reply_add", (PyCFunction) Agent_service_reply_add, METH_VARARGS, "service_reply_add(self, service_name, reply_name, )\n--\n\n "},
    {"service_reply_remove", (PyCFunction) Agent_service_reply_remove, METH_VARARGS, "service_reply_remove(self, service_name, reply_name, )\n--\n\n "},
    {"service_reply_arg_add", (PyCFunction) Agent_service_reply_arg_add, METH_VARARGS, "service_reply_arg_add(self, service_name, reply_name, arg_name, type, )\n--\n\n "},
    {"service_reply_arg_remove", (PyCFunction) Agent_service_reply_arg_remove, METH_VARARGS, "service_reply_arg_remove(self, service_name, reply_name, arg_name, )\n--\n\n "},
    {"service_has_replies", (PyCFunction) Agent_service_has_replies, METH_VARARGS, "service_has_replies(self, service_name, )\n--\n\n "},
    {"service_has_reply", (PyCFunction) Agent_service_has_reply, METH_VARARGS, "service_has_reply(self, service_name, reply_name, )\n--\n\n "},
    {"service_reply_names", (PyCFunction) Agent_service_reply_names, METH_VARARGS, "service_reply_names(self, service_name, )\n--\n\n "},
    {"service_reply_args_count", (PyCFunction) Agent_service_reply_args_count, METH_VARARGS, "service_args_count(self, service_name, )\n--\n\n "},
    {"service_reply_args_list", (PyCFunction) Agent_service_reply_args_list, METH_VARARGS, "service_args_list(self, service_name, )\n--\n\n "},
    {"service_reply_arg_exists", (PyCFunction) Agent_service_reply_arg_exists, METH_VARARGS, "service_arg_exists(self, service_name, argument_name, )\n--\n\n "},
    {"service_arg_add", (PyCFunction) Agent_service_arg_add, METH_VARARGS, "service_arg_add(self, service_name, arg_name, value_type, )\n--\n\n "},
    {"service_arg_remove", (PyCFunction) Agent_service_arg_remove, METH_VARARGS, "service_arg_remove(self, service_name, arg_name )\n--\n\n "},
    {"service_count", (PyCFunction) Agent_service_count, METH_NOARGS, "service_count(self, )\n--\n\n "},
    {"service_exists", (PyCFunction) Agent_service_exists, METH_VARARGS, "service_exists(self, service_name, )\n--\n\n "},
    {"service_list", (PyCFunction) Agent_service_list, METH_NOARGS, "service_list(self, )\n--\n\n "},
    {"service_args_count", (PyCFunction) Agent_service_args_count, METH_VARARGS, "service_args_count(self, service_name, )\n--\n\n "},
    {"service_args_list", (PyCFunction) Agent_service_args_list, METH_VARARGS, "service_args_list(self, service_name, )\n--\n\n "},
    {"service_arg_exists", (PyCFunction) Agent_service_args_exists, METH_VARARGS, "service_arg_exists(self, service_name, arg_name, )\n--\n\n "},

    //////////////////////////////////////////
    // Elections and leadership between agents
    {"election_join", (PyCFunction) Agent_election_join, METH_VARARGS, "election_join(self, election_name, )\n--\n\n "},
    {"election_leave", (PyCFunction) Agent_election_leave, METH_VARARGS, "election_leave(self, election_name, )\n--\n\n "},


    ///////////////////////////////////////////////////////
    // Administration, logging, configuration and utilities
    {"definition_set_path", (PyCFunction) Agent_definition_set_path, METH_VARARGS, "definition_set_path(self, path, )\n--\n\n "},
    {"definition_save", (PyCFunction) Agent_definition_save, METH_NOARGS, "definition_save(self, )\n--\n\n "},
    {"mapping_set_path", (PyCFunction) Agent_mapping_set_path, METH_VARARGS, "mapping_set_path(self, path, )\n--\n\n "},
    {"mapping_save", (PyCFunction) Agent_mapping_save, METH_NOARGS, "mapping_save(self, )\n--\n\n "},

    ///////////////////////////////////////////////////////
    // IOP constraints and descriptions
    {"constraints_enforce", (PyCFunction) Agent_constraints_enforce, METH_VARARGS, "constraints_enforce(self, enforce, )\n--\n\n "},
    {"input_add_constraint", (PyCFunction) Agent_input_add_constraint, METH_VARARGS, "input_add_constraint(self, name, constraint, )\n--\n\n "},
    {"output_add_constraint", (PyCFunction) Agent_output_add_constraint, METH_VARARGS, "output_add_constraint(self, name, constraint, )\n--\n\n "},
    {"parameter_add_constraint", (PyCFunction) Agent_parameter_add_constraint, METH_VARARGS, "parameter_add_constraint(self, name, constraint, )\n--\n\n "},

    {"input_set_description", (PyCFunction) Agent_input_set_description, METH_VARARGS, "input_set_description(self, name, description, )\n--\n\n "},
    {"output_set_description", (PyCFunction) Agent_output_set_description, METH_VARARGS, "output_set_description(self, name, description, )\n--\n\n "},
    {"parameter_set_description", (PyCFunction) Agent_parameter_set_description, METH_VARARGS, "parameter_set_description(self, name, description, )\n--\n\n "},

    // FIXME: igsagent_input_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented
    // FIXME: igsagent_output_set_zmsg is to dependent on ZeroMQ, it might be added if full ZeroMQ suuport is implemented

    {NULL}  /* Sentinel */
};

//Definition of the Agent type
static PyTypeObject AgentType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "ingescape.Agent",
    .tp_doc = "Agent(name, activate_immediately = True, )\n--\n\n ",
    .tp_basicsize = sizeof(AgentObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Agent_new,
    .tp_init = (initproc) Agent_init,
    .tp_dealloc = (destructor) Agent_dealloc,
    .tp_methods = Agent_methods,
};


////////////////////////////////////////////////////
// Module creation

static struct PyModuleDef init_ingescape_wrapper =
{
    PyModuleDef_HEAD_INIT,
    "ingescape", //    Name of the python module
    "IngeScape", //    docstring of the module
    -1,
    Ingescape_methods
};

PyMODINIT_FUNC PyInit_ingescape(void)
{
    Py_Initialize();

    PyObject *module_ingescape = PyModule_Create(&init_ingescape_wrapper);
    if (module_ingescape == NULL)
        return NULL;

    ////////////////////////////////////////////////////
    // Constant definition
    PyModule_AddIntConstant(module_ingescape, "SUCCESS", 0);
    PyModule_AddIntConstant(module_ingescape, "FAILURE", -1);

    PyModule_AddIntConstant(module_ingescape, "PEER_ENTERED", 1);
    PyModule_AddIntConstant(module_ingescape, "PEER_EXITED", 2);
    PyModule_AddIntConstant(module_ingescape, "AGENT_ENTERED", 3);
    PyModule_AddIntConstant(module_ingescape, "AGENT_UPDATED_DEFINITION", 4);
    PyModule_AddIntConstant(module_ingescape, "AGENT_KNOWS_US", 5);
    PyModule_AddIntConstant(module_ingescape, "AGENT_EXITED", 6);
    PyModule_AddIntConstant(module_ingescape, "AGENT_UPDATED_MAPPING", 7);
    PyModule_AddIntConstant(module_ingescape, "AGENT_WON_ELECTION", 8);
    PyModule_AddIntConstant(module_ingescape, "AGENT_LOST_ELECTION", 9);

    PyModule_AddIntConstant(module_ingescape, "INPUT_T", 1);
    PyModule_AddIntConstant(module_ingescape, "OUTPUT_T", 2);
    PyModule_AddIntConstant(module_ingescape, "PARAMETER_T", 3);

    PyModule_AddIntConstant(module_ingescape, "INTEGER_T", 1);
    PyModule_AddIntConstant(module_ingescape, "DOUBLE_T", 2);
    PyModule_AddIntConstant(module_ingescape, "STRING_T", 3);
    PyModule_AddIntConstant(module_ingescape, "BOOL_T", 4);
    PyModule_AddIntConstant(module_ingescape, "IMPULSION_T", 5);
    PyModule_AddIntConstant(module_ingescape, "DATA_T", 6);
    PyModule_AddIntConstant(module_ingescape, "UNKNOWN_T", 7);

    PyModule_AddIntConstant(module_ingescape, "LOG_TRACE", 0);
    PyModule_AddIntConstant(module_ingescape, "LOG_DEBUG", 1);
    PyModule_AddIntConstant(module_ingescape, "LOG_INFO", 2);
    PyModule_AddIntConstant(module_ingescape, "LOG_WARN", 3);
    PyModule_AddIntConstant(module_ingescape, "LOG_ERROR", 4);
    PyModule_AddIntConstant(module_ingescape, "LOG_FATAL", 5);

    PyModule_AddIntConstant(module_ingescape, "NETWORK_OK", 1);
    PyModule_AddIntConstant(module_ingescape, "NETWORK_DEVICE_NOT_AVAILABLE", 2);
    PyModule_AddIntConstant(module_ingescape, "NETWORK_ADDRESS_CHANGED", 3);
    PyModule_AddIntConstant(module_ingescape, "NETWORK_OK_AFTER_MANUAL_RESTART", 4);

    PyModule_AddIntConstant(module_ingescape, "REPLAY_INPUT", 1);
    PyModule_AddIntConstant(module_ingescape, "REPLAY_OUTPUT", 2);
    PyModule_AddIntConstant(module_ingescape, "REPLAY_PARAMETER", 4);
    PyModule_AddIntConstant(module_ingescape, "REPLAY_EXECUTE_SERVICE", 8);
    PyModule_AddIntConstant(module_ingescape, "REPLAY_CALL_SERVICE", 16);

    if (PyType_Ready(&AgentType) < 0)
            return NULL;

    Py_INCREF(&AgentType);
    if (PyModule_AddObject(module_ingescape, "Agent", (PyObject *) &AgentType) < 0) {
        Py_DECREF(&AgentType);
        Py_DECREF(module_ingescape);
        return NULL;
    }

    return module_ingescape;
}
