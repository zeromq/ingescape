//
//  init_control.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 26/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/init_control.h"

// Wrapper for : 
// PUBLIC int igs_startWithDevice(const char *networkDevice, unsigned int port);
napi_value node_igs_startWithDevice(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    int port;
    convert_napi_to_int(env, argv[1], &port);

    // call igs function
    int res = igs_startWithDevice(name, port);
    free(name);

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_startWithIP(const char *ipAddress, unsigned int port);
napi_value node_igs_startWithIP(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * ipAdress = convert_napi_to_string(env, argv[0]);
    int port;
    convert_napi_to_int(env, argv[1], &port);

    // call igs function
    int res = igs_startWithIP(ipAdress, port);
    free(ipAdress);

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_stop(void);
napi_value node_igs_stop(napi_env env, napi_callback_info info) {
    // call igs function
    igs_stop();

    // free callbacks memory
    free_data_cb();
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_setAgentName(const char *name);
napi_value node_igs_setAgentName(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv [nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_setAgentName(name);
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC char *igs_getAgentName(void); 
napi_value node_igs_getAgentName(napi_env env, napi_callback_info info) {
    // call igs function
    char * name = igs_getAgentName();

    // convert name into N-API value
    napi_value napi_name;
    convert_string_to_napi(env, name, &napi_name);
    free(name);
    return napi_name;
}

// Wrapper for : 
// PUBLIC int igs_setAgentState(const char *state);
napi_value node_igs_setAgentState(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv [nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * state = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_setAgentState(state);
    free(state);
    return NULL;
}

// Wrapper for : 
// PUBLIC char *igs_getAgentState(void); 
napi_value node_igs_getAgentState(napi_env env, napi_callback_info info) {
    // call igs function
    char * state = igs_getAgentState();

    // convert state into N-API value
    napi_value napi_state;
    convert_string_to_napi(env, state, &napi_state);
    free(state);
    return napi_state;
}

// Wrapper for : 
// PUBLIC int igs_mute(void);
napi_value node_igs_mute(napi_env env, napi_callback_info info) {
    // call igs function
    igs_mute();
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_unmute(void);
napi_value node_igs_unmute(napi_env env, napi_callback_info info) {
    // call igs function
    igs_unmute();
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_isMuted(void);
napi_value node_igs_isMuted(napi_env env, napi_callback_info info) {
    // call igs function
    bool is_muted = igs_isMuted();

    // convert is_muted into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, is_muted, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC int igs_freeze(void);
napi_value node_igs_freeze(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_freeze();

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC bool igs_isFrozen(void);
napi_value node_igs_isFrozen(napi_env env, napi_callback_info info) {
    // call igs function
    bool is_muted = igs_isFrozen();

    // convert is_muted into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, is_muted, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC int igs_unfreeze(void);
napi_value node_igs_unfreeze(napi_env env, napi_callback_info info) {
    // call igs function
    igs_unfreeze();
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setPublishingPort(unsigned int port);
napi_value node_igs_setPublishingPort(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    int port;
    convert_napi_to_int(env, argv[0], &port);

    // call igs function
    igs_setPublishingPort(port);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setDiscoveryInterval(unsigned int interval); //in milliseconds
napi_value node_igs_setDiscoveryInterval(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    int interval;
    convert_napi_to_int(env, argv[0], &interval);

    // call igs function
    igs_setDiscoveryInterval(interval);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setAgentTimeout(unsigned int duration); //in milliseconds
napi_value node_igs_setAgentTimeout(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    int duration;
    convert_napi_to_int(env, argv[0], &duration);

    // call igs function
    igs_setAgentTimeout(duration);
    return NULL;
}

// Allow callback for initialization and control ingescape code 
napi_value init_initialization(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_startWithDevice, "startWithDevice", exports);
    exports = enable_callback_into_js(env, node_igs_startWithIP, "startWithIP", exports);
    exports = enable_callback_into_js(env, node_igs_stop, "stop", exports);
    exports = enable_callback_into_js(env, node_igs_setAgentName, "setAgentName", exports);
    exports = enable_callback_into_js(env, node_igs_getAgentName, "getAgentName", exports);
    exports = enable_callback_into_js(env, node_igs_setAgentState, "setAgentState", exports);
    exports = enable_callback_into_js(env, node_igs_getAgentState, "getAgentState", exports);
    exports = enable_callback_into_js(env, node_igs_mute, "mute", exports);
    exports = enable_callback_into_js(env, node_igs_unmute, "unmute", exports);
    exports = enable_callback_into_js(env, node_igs_isMuted, "isMuted", exports);
    exports = enable_callback_into_js(env, node_igs_freeze, "freeze", exports);
    exports = enable_callback_into_js(env, node_igs_isFrozen, "isFrozen", exports);
    exports = enable_callback_into_js(env, node_igs_unfreeze, "unfreeze", exports);
    exports = enable_callback_into_js(env, node_igs_setPublishingPort, "setPublishingPort", exports);
    exports = enable_callback_into_js(env, node_igs_setDiscoveryInterval, "setDiscoveryInterval", exports);
    exports = enable_callback_into_js(env, node_igs_setAgentTimeout, "setAgentTimeout", exports);
    return exports;
}
