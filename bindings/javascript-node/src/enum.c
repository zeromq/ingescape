/*  =========================================================================
    enum - ingescape enums to json object

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

//  Get enum types for log levels in js
napi_value node_get_log_levels_js(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value trace_type;
    convert_int_to_napi(env, IGS_LOG_TRACE, &trace_type);
    napi_set_named_property(env, object, "IGS_LOG_TRACE", trace_type);

    napi_value debug_type;
    convert_int_to_napi(env, IGS_LOG_DEBUG, &debug_type);
    napi_set_named_property(env, object, "IGS_LOG_DEBUG", debug_type);

    napi_value info_type;
    convert_int_to_napi(env, IGS_LOG_INFO, &info_type);
    napi_set_named_property(env, object, "IGS_LOG_INFO", info_type);

    napi_value warn_type;
    convert_int_to_napi(env, IGS_LOG_WARN, &warn_type);
    napi_set_named_property(env, object, "IGS_LOG_WARN", warn_type);

    napi_value error_type;
    convert_int_to_napi(env, IGS_LOG_ERROR, &error_type);
    napi_set_named_property(env, object, "IGS_LOG_ERROR", error_type);

    napi_value fatal_type;
    convert_int_to_napi(env, IGS_LOG_FATAL, &fatal_type);
    napi_set_named_property(env, object, "IGS_LOG_FATAL", fatal_type);

    return object;
}

//  Get enum types for iop value types in js
napi_value node_get_iop_types_js(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value integer_type;
    convert_int_to_napi(env, IGS_INTEGER_T, &integer_type);
    napi_set_named_property(env, object, "IGS_INTEGER_T", integer_type);

    napi_value double_type;
    convert_int_to_napi(env, IGS_DOUBLE_T, &double_type);
    napi_set_named_property(env, object, "IGS_DOUBLE_T", double_type);

    napi_value string_type;
    convert_int_to_napi(env, IGS_STRING_T, &string_type);
    napi_set_named_property(env, object, "IGS_STRING_T", string_type);

    napi_value bool_type;
    convert_int_to_napi(env, IGS_BOOL_T, &bool_type);
    napi_set_named_property(env, object, "IGS_BOOL_T", bool_type);

    napi_value impulsion_type;
    convert_int_to_napi(env, IGS_IMPULSION_T, &impulsion_type);
    napi_set_named_property(env, object, "IGS_IMPULSION_T", impulsion_type);

    napi_value data_type;
    convert_int_to_napi(env, IGS_DATA_T, &data_type);
    napi_set_named_property(env, object, "IGS_DATA_T", data_type);

    napi_value unknown_type;
    convert_int_to_napi(env, IGS_UNKNOWN_T, &unknown_type);
    napi_set_named_property(env, object, "IGS_UNKNOWN_T", unknown_type);

    return object;
}

//  Get enum types for iop types in js
napi_value node_get_iops_js(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value input;
    convert_int_to_napi(env, IGS_INPUT_T, &input);
    napi_set_named_property(env, object, "IGS_INPUT_T", input);

    napi_value output;
    convert_int_to_napi(env, IGS_OUTPUT_T, &output);
    napi_set_named_property(env, object, "IGS_OUTPUT_T", output);

    napi_value parameter;
    convert_int_to_napi(env, IGS_PARAMETER_T, &parameter);
    napi_set_named_property(env, object, "IGS_PARAMETER_T", parameter);

    return object;
}

// Get enum types for observe_monitor events
napi_value node_get_monitor_event_types(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value ok_type;
    convert_int_to_napi(env, IGS_NETWORK_OK, &ok_type);
    napi_set_named_property(env, object, "IGS_NETWORK_OK", ok_type);

    napi_value device_not_available;
    convert_int_to_napi(env, IGS_NETWORK_DEVICE_NOT_AVAILABLE, &device_not_available);
    napi_set_named_property(env, object, "IGS_NETWORK_DEVICE_NOT_AVAILABLE", device_not_available);

    napi_value address_changed;
    convert_int_to_napi(env, IGS_NETWORK_ADDRESS_CHANGED, &address_changed);
    napi_set_named_property(env, object, "IGS_NETWORK_ADDRESS_CHANGED", address_changed);

    napi_value ok_aftert_manual_restart;
    convert_int_to_napi(env, IGS_NETWORK_OK_AFTER_MANUAL_RESTART, &ok_aftert_manual_restart);
    napi_set_named_property(env, object, "IGS_NETWORK_OK_AFTER_MANUAL_RESTART", ok_aftert_manual_restart);

    return object;
}

// Get enum types for igs results (function returns)
napi_value node_get_igs_result_types(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value success;
    convert_int_to_napi(env, IGS_SUCCESS, &success);
    napi_set_named_property(env, object, "IGS_SUCCESS", success);

    napi_value failure;
    convert_int_to_napi(env, IGS_FAILURE, &failure);
    napi_set_named_property(env, object, "IGS_FAILURE", failure);

    return object;
}

// Get enum types for agent events
napi_value node_get_agent_event_types(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value peer_entered_type;
    convert_int_to_napi(env, IGS_PEER_ENTERED, &peer_entered_type);
    napi_set_named_property(env, object, "IGS_PEER_ENTERED", peer_entered_type);

    napi_value peer_exited_type;
    convert_int_to_napi(env, IGS_PEER_EXITED, &peer_exited_type);
    napi_set_named_property(env, object, "IGS_PEER_EXITED", peer_exited_type);

    napi_value agent_entered_type;
    convert_int_to_napi(env, IGS_AGENT_ENTERED, &agent_entered_type);
    napi_set_named_property(env, object, "IGS_AGENT_ENTERED", agent_entered_type);

    napi_value agent_updated_definition_type;
    convert_int_to_napi(env, IGS_AGENT_UPDATED_DEFINITION, &agent_updated_definition_type);
    napi_set_named_property(env, object, "IGS_AGENT_UPDATED_DEFINITION", agent_updated_definition_type);

    napi_value agent_knows_us_type;
    convert_int_to_napi(env, IGS_AGENT_KNOWS_US, &agent_knows_us_type);
    napi_set_named_property(env, object, "IGS_AGENT_KNOWS_US", agent_knows_us_type);

    napi_value agent_exited_type;
    convert_int_to_napi(env, IGS_AGENT_EXITED, &agent_exited_type);
    napi_set_named_property(env, object, "IGS_AGENT_EXITED", agent_exited_type);

    napi_value agent_updated_mapping_type;
    convert_int_to_napi(env, IGS_AGENT_UPDATED_MAPPING, &agent_updated_mapping_type);
    napi_set_named_property(env, object, "IGS_AGENT_UPDATED_MAPPING", agent_updated_mapping_type);

    napi_value agent_won_election_type;
    convert_int_to_napi(env, IGS_AGENT_WON_ELECTION, &agent_won_election_type);
    napi_set_named_property(env, object, "IGS_AGENT_WON_ELECTION", agent_won_election_type);

    napi_value agent_lost_election_type;
    convert_int_to_napi(env, IGS_AGENT_LOST_ELECTION, &agent_lost_election_type);
    napi_set_named_property(env, object, "IGS_AGENT_LOST_ELECTION", agent_lost_election_type);

    return object;
}

// Get enum modes for replay
napi_value node_get_replay_modes(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value replay_input_mode;
    convert_int_to_napi(env, IGS_REPLAY_INPUT, &replay_input_mode);
    napi_set_named_property(env, object, "IGS_REPLAY_INPUT", replay_input_mode);

    napi_value replay_output_mode;
    convert_int_to_napi(env, IGS_REPLAY_OUTPUT, &replay_output_mode);
    napi_set_named_property(env, object, "IGS_REPLAY_OUTPUT", replay_output_mode);

    napi_value replay_parameter_mode;
    convert_int_to_napi(env, IGS_REPLAY_PARAMETER, &replay_parameter_mode);
    napi_set_named_property(env, object, "IGS_REPLAY_PARAMETER", replay_parameter_mode);

    napi_value replay_execute_service_mode;
    convert_int_to_napi(env, IGS_REPLAY_EXECUTE_SERVICE, &replay_execute_service_mode);
    napi_set_named_property(env, object, "IGS_REPLAY_EXECUTE_SERVICE", replay_execute_service_mode);

    napi_value replay_call_service_mode;
    convert_int_to_napi(env, IGS_REPLAY_CALL_SERVICE, &replay_call_service_mode);
    napi_set_named_property(env, object, "IGS_REPLAY_CALL_SERVICE", replay_call_service_mode);

    return object;
}


napi_value init_enum(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_get_log_levels_js, "logLevels", exports);  
    exports = enable_callback_into_js(env, node_get_iop_types_js, "iopValueTypes", exports);  
    exports = enable_callback_into_js(env, node_get_iops_js, "iopTypes", exports);  
    exports = enable_callback_into_js(env, node_get_monitor_event_types, "monitorEventTypes", exports);
    exports = enable_callback_into_js(env, node_get_igs_result_types, "resultTypes", exports);
    exports = enable_callback_into_js(env, node_get_agent_event_types, "agentEventTypes", exports);
    exports = enable_callback_into_js(env, node_get_replay_modes, "replayModes", exports);
    return exports;
}
