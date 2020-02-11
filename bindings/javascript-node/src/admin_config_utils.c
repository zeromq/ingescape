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

// Wrapper for : 
// PUBLIC void igs_monitoringEnable(unsigned int period); //in milliseconds
napi_value node_igs_monitoringEnable(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    int period;
    convert_napi_to_int(env, argv[0], &period);
    igs_monitoringEnable(period);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port);
napi_value node_igs_monitoringEnableWithExpectedDevice(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types & call igs function
    int period;
    convert_napi_to_int(env, argv[0], &period);
    char * networkDevice = convert_napi_to_string(env, argv[1]);
    int port;
    convert_napi_to_int(env, argv[2], &port);
    igs_monitoringEnableWithExpectedDevice(period, networkDevice, port);
    free(networkDevice);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_monitoringDisable(void);
napi_value node_igs_monitoringDisable(napi_env env, napi_callback_info info) {
    // call igs function
    igs_monitoringDisable();
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_isMonitoringEnabled(void);
napi_value node_igs_isMonitoringEnabled(napi_env env, napi_callback_info info) {
    // call igs function
    bool return_value = igs_isMonitoringEnabled();

    // convert return value into N-API value
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return; 
}

// Wrapper for : 
// PUBLIC void igs_monitoringShallStartStopAgent(bool flag);
napi_value node_igs_monitoringShallStartStopAgent(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];
  
    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);
    
    // convert infos into C types & call igs function 
    bool flag;
    convert_napi_to_bool(env, argv[0], &flag);
    igs_monitoringShallStartStopAgent(flag);
    return NULL;
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
    exports = enable_callback_into_js(env, node_igs_monitoringEnable, "monitoringEnable", exports);
    exports = enable_callback_into_js(env, node_igs_monitoringEnableWithExpectedDevice, "monitoringEnableWithExpectedDevice", exports);
    exports = enable_callback_into_js(env, node_igs_monitoringDisable, "monitoringDisable", exports);
    exports = enable_callback_into_js(env, node_igs_isMonitoringEnabled, "isMonitoringEnabled", exports);
    exports = enable_callback_into_js(env, node_igs_monitoringShallStartStopAgent, "monitoringShallStartStopAgent", exports);
    return exports;
}
