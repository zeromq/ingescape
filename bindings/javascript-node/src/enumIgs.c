//
//  enumIgs.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 10/10/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/enumIgs.h"

//  Get enum types for iop types in js
napi_value node_get_logLevels_js(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value traceType;
    convert_int_to_napi(env, IGS_LOG_TRACE, &traceType);
    napi_set_named_property(env, object, "IGS_LOG_TRACE", traceType);

    napi_value debugType;
    convert_int_to_napi(env, IGS_LOG_DEBUG, &debugType);
    napi_set_named_property(env, object, "IGS_LOG_DEBUG", debugType);

    napi_value infoType;
    convert_int_to_napi(env, IGS_LOG_INFO, &infoType);
    napi_set_named_property(env, object, "IGS_LOG_INFO", infoType);

    napi_value warnType;
    convert_int_to_napi(env, IGS_LOG_WARN, &warnType);
    napi_set_named_property(env, object, "IGS_LOG_WARN", warnType);

    napi_value errorType;
    convert_int_to_napi(env, IGS_LOG_ERROR, &errorType);
    napi_set_named_property(env, object, "IGS_LOG_ERROR", errorType);

    napi_value fatalType;
    convert_int_to_napi(env, IGS_LOG_FATAL, &fatalType);
    napi_set_named_property(env, object, "IGS_LOG_FATAL", fatalType);

    return object;
}

//  Get enum types for iop types in js
napi_value node_get_iopTypes_js(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value integerType;
    convert_int_to_napi(env, IGS_INTEGER_T, &integerType);
    napi_set_named_property(env, object, "IGS_INTEGER_T", integerType);

    napi_value doubleType;
    convert_int_to_napi(env, IGS_DOUBLE_T, &doubleType);
    napi_set_named_property(env, object, "IGS_DOUBLE_T", doubleType);

    napi_value stringType;
    convert_int_to_napi(env, IGS_STRING_T, &stringType);
    napi_set_named_property(env, object, "IGS_STRING_T", stringType);

    napi_value boolType;
    convert_int_to_napi(env, IGS_BOOL_T, &boolType);
    napi_set_named_property(env, object, "IGS_BOOL_T", boolType);

    napi_value impulsionType;
    convert_int_to_napi(env, IGS_IMPULSION_T, &impulsionType);
    napi_set_named_property(env, object, "IGS_IMPULSION_T", impulsionType);

    napi_value dataType;
    convert_int_to_napi(env, IGS_DATA_T, &dataType);
    napi_set_named_property(env, object, "IGS_DATA_T", dataType);

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

//  Get enum types for license limit  
napi_value node_get_license_limit_types(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value timeoutType;
    convert_int_to_napi(env, IGS_LICENSE_TIMEOUT, &timeoutType);
    napi_set_named_property(env, object, "IGS_LICENSE_TIMEOUT", timeoutType);

    napi_value tooManyAgent;
    convert_int_to_napi(env, IGS_LICENSE_TOO_MANY_AGENTS, &tooManyAgent);
    napi_set_named_property(env, object, "IGS_LICENSE_TOO_MANY_AGENTS", tooManyAgent);

    napi_value tooManyIOPs;
    convert_int_to_napi(env, IGS_LICENSE_TOO_MANY_IOPS, &tooManyIOPs);
    napi_set_named_property(env, object, "IGS_LICENSE_TOO_MANY_IOPS", tooManyIOPs);

    return object;
}

// Get enum types for monitor events
napi_value node_get_monitor_event_types(napi_env env, napi_callback_info info) {
    napi_value object;
    napi_create_object(env, &object);

    napi_value okType;
    convert_int_to_napi(env, IGS_NETWORK_OK, &okType);
    napi_set_named_property(env, object, "IGS_NETWORK_OK", okType);

    napi_value deviceNotAvailable;
    convert_int_to_napi(env, IGS_NETWORK_DEVICE_NOT_AVAILABLE, &deviceNotAvailable);
    napi_set_named_property(env, object, "IGS_NETWORK_DEVICE_NOT_AVAILABLE", deviceNotAvailable);

    napi_value addressChanged;
    convert_int_to_napi(env, IGS_NETWORK_ADDRESS_CHANGED, &addressChanged);
    napi_set_named_property(env, object, "IGS_NETWORK_ADDRESS_CHANGED", addressChanged);

    napi_value okAftertManualRestart;
    convert_int_to_napi(env, IGS_NETWORK_OK_AFTER_MANUAL_RESTART, &okAftertManualRestart);
    napi_set_named_property(env, object, "IGS_NETWORK_OK_AFTER_MANUAL_RESTART", okAftertManualRestart);

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

    napi_value agentEnteredType;
    convert_int_to_napi(env, IGS_AGENT_ENTERED, &agentEnteredType);
    napi_set_named_property(env, object, "IGS_AGENT_ENTERED", agentEnteredType);

    napi_value agentExitedType;
    convert_int_to_napi(env, IGS_AGENT_EXITED, &agentExitedType);
    napi_set_named_property(env, object, "IGS_AGENT_EXITED", agentExitedType);

    napi_value peerEnteredType;
    convert_int_to_napi(env, IGS_PEER_ENTERED, &peerEnteredType);
    napi_set_named_property(env, object, "IGS_PEER_ENTERED", peerEnteredType);

    napi_value peerExitedType;
    convert_int_to_napi(env, IGS_PEER_EXITED, &peerExitedType);
    napi_set_named_property(env, object, "IGS_PEER_EXITED", peerExitedType);

    return object;
}


// Allow callback for ingescape enums
napi_value init_enums(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_get_logLevels_js, "getLogLevels", exports);  
    exports = enable_callback_into_js(env, node_get_iopTypes_js, "getIopValueTypes", exports);  
    exports = enable_callback_into_js(env, node_get_iops_js, "getIopTypes", exports);  
    exports = enable_callback_into_js(env, node_get_license_limit_types, "getLicenseLimitTypes", exports);
    exports = enable_callback_into_js(env, node_get_monitor_event_types, "getMonitorEventTypes", exports);
    exports = enable_callback_into_js(env, node_get_igs_result_types, "getIgsResultTypes", exports);
    exports = enable_callback_into_js(env, node_get_agent_event_types, "getAgentEventTypes", exports);
    return exports;
}
