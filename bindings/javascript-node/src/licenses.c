//
//  licenses.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 26/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/licenses.h"

// Wrapper for : 
// PUBLIC void igs_setLicensePath(const char *path);
napi_value node_igs_setLicensePath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * license_path = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_setLicensePath(license_path);
    free(license_path);

    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_getLicensePath();
napi_value node_igs_getLicensePath(napi_env env, napi_callback_info info) {
    // convert infos into C types
    char * license_path = igs_getLicensePath();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env,license_path, &res_convert);
    free(license_path);
    return res_convert;
}

// Wrapper for
// PUBLIC bool igs_checkLicenseForAgent(const char *agentId);
napi_value node_igs_checkLicenseForAgent(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * agent_id = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_checkLicenseForAgent(agent_id);
    free(agent_id);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env,res, &res_convert);
    return res_convert;
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

// Allow callback for licenses ingescape code 
napi_value init_licenses(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_setLicensePath, "setLicensePath", exports);
    exports = enable_callback_into_js(env, node_igs_getLicensePath, "getLicensePath", exports);
    exports = enable_callback_into_js(env, node_igs_checkLicenseForAgent, "checkLicenseForAgent", exports);
    exports = enable_callback_into_js(env, node_get_license_limit_types, "getLicenseLimitTypes", exports);

    return exports;
}
