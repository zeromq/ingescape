//
//  admin_config_utils.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 06/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/admin_config_utils.h"

// Wrapper for : 
// PUBLIC int igs_version(void);
napi_value node_igs_version(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_version();

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC void igs_getNetdevicesList(char ***devices, int *nb);
napi_value node_igs_getNetdevicesList(napi_env env, napi_callback_info info) {
    // call igs function
    char **devices = NULL;
    int nb = 0;
    igs_getNetdevicesList(&devices, &nb);

    // convert char ** into napi_value
    napi_value arrayNetdevices;
    convert_string_list_to_napi_array(env, devices, nb, &arrayNetdevices);
    igs_freeNetdevicesList(devices, nb); // free devices
    return arrayNetdevices;
}

// Wrapper for :
// PUBLIC void igs_setCommandLine(const char *line);
napi_value node_igs_setCommandLine(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * line = convert_napi_to_string(env, argv[0]);
    igs_setCommandLine(line);
    free(line);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setRequestOutputsFromMappedAgents(bool notify);
napi_value node_igs_setRequestOutputsFromMappedAgents(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    bool reqOutputsFromMappedAgents;
    convert_napi_to_bool(env, argv[0], &reqOutputsFromMappedAgents);
    igs_setRequestOutputsFromMappedAgents(reqOutputsFromMappedAgents);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_getRequestOutputsFromMappedAgents(void);
napi_value node_igs_getRequestOutputsFromMappedAgents(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_getRequestOutputsFromMappedAgents();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC void igs_setVerbose(bool verbose);
napi_value node_igs_setVerbose(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function 
    bool verbose;
    convert_napi_to_bool(env, argv[0], &verbose);
    igs_setVerbose(verbose);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_isVerbose(void);
napi_value node_igs_isVerbose(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_isVerbose();

    // convert return value into N-API value 
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC void igs_setUseColorVerbose(bool useColor); 
napi_value node_igs_setUseColorVerbose(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function 
    bool useColor;
    convert_napi_to_bool(env, argv[0], &useColor);
    igs_setUseColorVerbose(useColor);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_getUseColorVerbose(void);
napi_value node_igs_getUseColorVerbose(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_getUseColorVerbose();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC void igs_setLogStream(bool useLogStream); 
napi_value node_igs_setLogStream(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function 
    bool logStream;
    convert_napi_to_bool(env, argv[0], &logStream);
    igs_setLogStream(logStream);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_getLogStream(void);
napi_value node_igs_getLogStream(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_getLogStream();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC void igs_setLogInFile(bool useLogFile);
napi_value node_igs_setLogInFile(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    bool logInFile;
    convert_napi_to_bool(env, argv[0], &logInFile);
    igs_setLogInFile(logInFile);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_getLogInFile(void);
napi_value node_igs_getLogInFile(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_getLogInFile();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

// Wrapper for : 
// PUBLIC void igs_setLogPath(const char *path);
napi_value node_igs_setLogPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * path = convert_napi_to_string(env, argv[0]);
    igs_setLogPath(path);
    free(path);
    return NULL;
}

// Wrapper for : 
// PUBLIC char* igs_getLogPath(void);
napi_value node_igs_getLogPath(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getLogPath();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC void igs_setLogLevel(igs_logLevel_t level);
napi_value node_igs_setLogLevel(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    int level;
    convert_napi_to_int(env, argv[0], &level);
    igs_setLogLevel(level);
    return NULL;
}

// Wrapper for : 
// PUBLIC igs_logLevel_t igs_getLogLevel(void);
napi_value node_igs_getLogLevel(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_getLogLevel();

    // convert result into napi_value & return
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// #define igs_trace(...) igs_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
napi_value node_igs_trace(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * log = convert_napi_to_string(env, argv[0]);
    igs_trace("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// #define igs_debug(...) igs_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
napi_value node_igs_debug(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * log = convert_napi_to_string(env, argv[0]);
    igs_debug("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// #define igs_info(...)  igs_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
napi_value node_igs_info(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * log = convert_napi_to_string(env, argv[0]);
    igs_info("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// #define igs_warn(...)  igs_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
napi_value node_igs_warn(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * log = convert_napi_to_string(env, argv[0]);
    igs_warn("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// #define igs_error(...) igs_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
napi_value node_igs_error(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * log = convert_napi_to_string(env, argv[0]);
    igs_error("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// #define igs_fatal(...) igs_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)
napi_value node_igs_fatal(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function 
    char * log = convert_napi_to_string(env, argv[0]);
    igs_fatal("%s\n", log);
    free(log);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setDefinitionPath(const char *path);
napi_value node_igs_setDefinitionPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * def_path = convert_napi_to_string(env, argv[0]);
    igs_setDefinitionPath(def_path); 
    free(def_path);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setMappingPath(const char *path);
napi_value node_igs_setMappingPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    char * map_path = convert_napi_to_string(env, argv[0]);
    igs_setMappingPath(map_path); 
    free(map_path);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_writeDefinitionToPath(void);
napi_value node_igs_writeDefinitionToPath(napi_env env, napi_callback_info info) {
    // call igs function
    igs_writeDefinitionToPath();
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_writeMappingToPath(void);
napi_value node_igs_writeMappingToPath(napi_env env, napi_callback_info info) {
    // call igs function
    igs_writeMappingToPath();
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_setIpcFolderPath(char *path);
napi_value node_igs_setIpcFolderPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function 
    char * ipc_path = convert_napi_to_string(env, argv[0]);
    igs_setIpcFolderPath(ipc_path); 
    free(ipc_path);
    return NULL;
}

// Wrapper for : 
// PUBLIC const char* igs_getIpcFolderPath(void);
napi_value node_igs_getIpcFolderPath(napi_env env, napi_callback_info info) {
    // call igs function
    const char * res = igs_getIpcFolderPath();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC void igs_setAllowIpc(bool allow);
napi_value node_igs_setAllowIpc(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);
    
    // convert infos into C types & call igs function 
    bool ipc;
    convert_napi_to_bool(env, argv[0], &ipc);
    igs_setAllowIpc(ipc);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_getAllowIpc(void);
napi_value node_igs_getAllowIpc(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_getAllowIpc();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return; 
}

//  Get enum types for iop types in js
napi_value node_get_logLevel_js(napi_env env, napi_callback_info info) {
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

// Allow callback for admin, config & utils ingescape code 
napi_value init_admin_config_utils(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_version, "igsVersion", exports);
    exports = enable_callback_into_js(env, node_igs_getNetdevicesList, "getNetdevicesList", exports);
    exports = enable_callback_into_js(env, node_igs_setCommandLine, "setCommandLine", exports);
    exports = enable_callback_into_js(env, node_igs_setRequestOutputsFromMappedAgents, "setRequestOutputsFromMappedAgents", exports);
    exports = enable_callback_into_js(env, node_igs_getRequestOutputsFromMappedAgents, "getRequestOutputsFromMappedAgents", exports);
    exports = enable_callback_into_js(env, node_igs_setVerbose, "setVerbose", exports);
    exports = enable_callback_into_js(env, node_igs_isVerbose, "isVerbose", exports);
    exports = enable_callback_into_js(env, node_igs_setUseColorVerbose, "setUseColorVerbose", exports);
    exports = enable_callback_into_js(env, node_igs_getUseColorVerbose, "getUseColorVerbose", exports);
    exports = enable_callback_into_js(env, node_igs_setLogStream, "setLogStream", exports);    
    exports = enable_callback_into_js(env, node_igs_getLogStream, "getLogStream", exports);
    exports = enable_callback_into_js(env, node_igs_setLogInFile, "setLogInFile", exports);
    exports = enable_callback_into_js(env, node_igs_getLogInFile, "getLogInFile", exports);
    exports = enable_callback_into_js(env, node_igs_setLogPath, "setLogPath", exports);
    exports = enable_callback_into_js(env, node_igs_getLogPath, "getLogPath", exports);
    exports = enable_callback_into_js(env, node_igs_setLogLevel, "setLogLevel", exports);
    exports = enable_callback_into_js(env, node_igs_getLogLevel, "getLogLevel", exports);
    exports = enable_callback_into_js(env, node_igs_trace, "trace", exports);
    exports = enable_callback_into_js(env, node_igs_debug, "debug", exports);
    exports = enable_callback_into_js(env, node_igs_info, "info", exports);
    exports = enable_callback_into_js(env, node_igs_warn, "warn", exports);
    exports = enable_callback_into_js(env, node_igs_error, "error", exports);
    exports = enable_callback_into_js(env, node_igs_fatal, "fatal", exports);
    exports = enable_callback_into_js(env, node_igs_setDefinitionPath, "setDefinitionPath", exports);
    exports = enable_callback_into_js(env, node_igs_setMappingPath, "setMappingPath", exports);
    exports = enable_callback_into_js(env, node_igs_writeDefinitionToPath, "writeDefinitionToPath", exports);
    exports = enable_callback_into_js(env, node_igs_writeMappingToPath, "writeMappingToPath", exports);
    exports = enable_callback_into_js(env, node_igs_setIpcFolderPath, "setIpcFolderPath", exports);
    exports = enable_callback_into_js(env, node_igs_getIpcFolderPath, "getIpcFolderPath", exports);
    exports = enable_callback_into_js(env, node_igs_setAllowIpc, "setAllowIpc", exports);
    exports = enable_callback_into_js(env, node_igs_getAllowIpc, "getAllowIpc", exports);
    exports = enable_callback_into_js(env, node_get_logLevel_js, "getLogLevels", exports);  
    return exports;
}