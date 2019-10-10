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

    napi_value numberType;
    convert_int_to_napi(env, IGS_NUMBER_JS, &numberType);
    napi_set_named_property(env, object, "IGS_NUMBER_T", numberType);

    napi_value stringType;
    convert_int_to_napi(env, IGS_STRING_JS, &stringType);
    napi_set_named_property(env, object, "IGS_STRING_T", stringType);

    napi_value boolType;
    convert_int_to_napi(env, IGS_BOOL_JS, &boolType);
    napi_set_named_property(env, object, "IGS_BOOL_T", boolType);

    napi_value impulsionType;
    convert_int_to_napi(env, IGS_IMPULSION_JS, &impulsionType);
    napi_set_named_property(env, object, "IGS_IMPULSION_T", impulsionType);

    napi_value dataType;
    convert_int_to_napi(env, IGS_DATA_JS, &dataType);
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

//  Get enum types for license limit types 
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

// Allow callback for ingescape enums
napi_value init_enums(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_get_logLevels_js, "getLogLevels", exports);  
    exports = enable_callback_into_js(env, node_get_iopTypes_js, "getIopValueTypes", exports);  
    exports = enable_callback_into_js(env, node_get_iops_js, "getIopTypes", exports);  
    exports = enable_callback_into_js(env, node_get_license_limit_types, "getLicenseLimitTypes", exports);
    return exports;
}
