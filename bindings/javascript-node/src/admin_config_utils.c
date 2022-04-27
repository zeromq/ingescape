/*  =========================================================================
    admin_config_utils - administration and logging

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

threadsafe_context_t *timer_contexts = NULL;

napi_value node_igs_version(napi_env env, napi_callback_info info) {
    int version = igs_version();
    napi_value version_js;
    convert_int_to_napi(env, version, &version_js);
    return version_js;
}

napi_value node_igs_protocol(napi_env env, napi_callback_info info) {
    int protocol = igs_protocol();
    napi_value protocol_convert;
    convert_int_to_napi(env, protocol, &protocol_convert);
    return protocol_convert;
}

napi_value node_igs_net_devices_list(napi_env env, napi_callback_info info) {
    int devices_nb = 0;
    char **devices = igs_net_devices_list(&devices_nb);
    napi_value devices_js;
    convert_string_list_to_napi_array(env, devices, devices_nb, &devices_js);
    igs_free_net_devices_list(devices, devices_nb);
    return devices_js;
}

napi_value node_igs_net_addresses_list(napi_env env, napi_callback_info info) {
    int addresses_nb = 0;
    char **addresses = igs_net_addresses_list(&addresses_nb);
    napi_value addresses_js;
    convert_string_list_to_napi_array(env, addresses, addresses_nb, &addresses_js);
    igs_free_net_addresses_list(addresses, addresses_nb);
    return addresses_js;
}

napi_value node_igs_command_line(napi_env env, napi_callback_info info) {
    char *command_line = igs_command_line();
    napi_value command_line_js;
    convert_string_to_napi(env, command_line, &command_line_js);
    free(command_line);
    return command_line_js;
}

napi_value node_igs_set_command_line(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *command_line = convert_napi_to_string(env, argv[0]);
    igs_set_command_line(command_line);
    free(command_line);
    return NULL;
}

napi_value node_igs_mapping_set_outputs_request(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool outputs_request;
    convert_napi_to_bool(env, argv[0], &outputs_request);
    igs_mapping_set_outputs_request(outputs_request);
    return NULL;
}

napi_value node_igs_mapping_outputs_request(napi_env env, napi_callback_info info) {
    bool outputs_request = igs_mapping_outputs_request();
    napi_value outputs_request_js;
    convert_bool_to_napi(env, outputs_request, &outputs_request_js);
    return outputs_request_js;
}

napi_value node_igs_log_set_console(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool log_console;
    convert_napi_to_bool(env, argv[0], &log_console);
    igs_log_set_console(log_console);
    return NULL;
}

napi_value node_igs_log_console(napi_env env, napi_callback_info info) {
    bool log_console = igs_log_console();
    napi_value log_console_js;
    convert_bool_to_napi(env, log_console, &log_console_js);
    return log_console_js;
}

napi_value node_igs_log_set_console_level(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int log_console_level;
    convert_napi_to_int(env, argv[0], &log_console_level);
    igs_log_set_console_level(log_console_level);
    return NULL;
}

napi_value node_igs_log_console_level(napi_env env, napi_callback_info info) {
    int log_console_level = igs_log_console_level();
    napi_value log_console_level_js;
    convert_int_to_napi(env, log_console_level, &log_console_level_js);
    return log_console_level_js;
}

napi_value node_igs_log_set_console_color(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool log_console_color;
    convert_napi_to_bool(env, argv[0], &log_console_color);
    igs_log_set_console_color(log_console_color);
    return NULL;
}

napi_value node_igs_log_console_color(napi_env env, napi_callback_info info) {
    bool log_console_color = igs_log_console_color();
    napi_value log_console_color_js;
    convert_bool_to_napi(env, log_console_color, &log_console_color_js);
    return log_console_color_js;
}

napi_value node_igs_log_set_stream(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool log_stream;
    convert_napi_to_bool(env, argv[0], &log_stream);
    igs_log_set_stream(log_stream);
    return NULL;
}

napi_value node_igs_log_stream(napi_env env, napi_callback_info info) {
    bool log_stream = igs_log_stream();
    napi_value log_stream_js;
    convert_bool_to_napi(env, log_stream, &log_stream_js);
    return log_stream_js;
}

napi_value node_igs_log_set_file(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    bool log_in_file;
    char *log_path = NULL;
    napi_status status;
    napi_valuetype arg2_type;

    convert_napi_to_bool(env, argv[0], &log_in_file);
    status = napi_typeof(env, argv[1], &arg2_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");
    if ((arg2_type != napi_null) && (arg2_type != napi_undefined))
        log_path = convert_napi_to_string(env, argv[1]);
    igs_log_set_file(log_in_file, log_path);
    if (log_path != NULL)
        free(log_path);
    return NULL;
}

napi_value node_igs_log_file(napi_env env, napi_callback_info info) {
    bool return_value = igs_log_file();
    napi_value napi_return;
    convert_bool_to_napi(env, return_value, &napi_return);
    return napi_return;
}

napi_value node_igs_log_set_file_path(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *file_path = convert_napi_to_string(env, argv[0]);
    igs_log_set_file_path(file_path);
    free(file_path);
    return NULL;
}

napi_value node_igs_log_file_path(napi_env env, napi_callback_info info) {
    char *file_path = igs_log_file_path();
    napi_value file_path_js;
    convert_string_to_napi(env, file_path, &file_path_js);
    free(file_path);
    return file_path_js;
}


napi_value node_igs_log_set_file_level(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int log_file_level;
    convert_napi_to_int(env, argv[0], &log_file_level);
    igs_log_set_file_level(log_file_level);
    return NULL;
}

napi_value node_igs_log_set_file_max_line_length(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int max_line_length;
    convert_napi_to_int(env, argv[0], &max_line_length);
    igs_log_set_file_max_line_length(max_line_length);
    return NULL;
}

napi_value node_igs_log_include_data(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool log_include_data;
    convert_napi_to_bool(env, argv[0], &log_include_data);
    igs_log_include_data(log_include_data);
    return NULL;
}

napi_value node_igs_log_include_services(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool log_include_services;
    convert_napi_to_bool(env, argv[0], &log_include_services);
    igs_log_include_services(log_include_services);
    return NULL;
}

napi_value node_igs_trace(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_trace("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_debug(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_debug("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_info(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_info("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_warn(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_warn("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_error(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_error("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_fatal(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *log = convert_napi_to_string(env, argv[0]);
    igs_fatal("%s", log);
    free(log);
    return NULL;
}

napi_value node_igs_definition_set_path(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *definition_path = convert_napi_to_string(env, argv[0]);
    igs_definition_set_path(definition_path); 
    free(definition_path);
    return NULL;
}

napi_value node_igs_mapping_set_path(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *mapping_path = convert_napi_to_string(env, argv[0]);
    igs_mapping_set_path(mapping_path); 
    free(mapping_path);
    return NULL;
}

napi_value node_igs_definition_save(napi_env env, napi_callback_info info) {
    igs_definition_save();
    return NULL;
}

napi_value node_igs_mapping_save(napi_env env, napi_callback_info info) {
    igs_mapping_save();
    return NULL;
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
napi_value node_igs_set_ipc_dir(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *ipc_path = convert_napi_to_string(env, argv[0]);
    igs_set_ipc_dir(ipc_path); 
    free(ipc_path);
    return NULL;
}

napi_value node_igs_ipc_dir(napi_env env, napi_callback_info info) {
    const char *ipc_path = igs_ipc_dir();
    napi_value ipc_path_js;
    convert_string_to_napi(env, ipc_path, &ipc_path_js);
    return ipc_path_js;
}
#endif

napi_value node_igs_set_ipc(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool ipc;
    convert_napi_to_bool(env, argv[0], &ipc);
    igs_set_ipc(ipc);
    return NULL;
}

napi_value node_igs_has_ipc(napi_env env, napi_callback_info info) {
    bool ipc = igs_has_ipc();
    napi_value ipc_js;
    convert_bool_to_napi(env, ipc, &ipc_js);
    return ipc_js; 
}

static void cb_timer_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    timer_callback_args_t *callback_arg = (timer_callback_args_t *) data;
    napi_value argv[2];
    convert_int_to_napi(env, callback_arg->timer_id, &argv[0]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[1]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[1]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }

    free(callback_arg);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void timer_fn (int timer_id, void *my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    timer_callback_args_t* callback_arg = calloc(1, sizeof(timer_callback_args_t)); 
    callback_arg->timer_id = timer_id;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igs_timer_start(napi_env env, napi_callback_info info) {
    napi_value argv[4];
    get_function_arguments(env, info, 4, argv);
    int delay, times;
    convert_napi_to_int(env, argv[0], &delay);
    convert_napi_to_int(env, argv[1], &times);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    DL_APPEND(timer_contexts, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/timerCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");

    status = napi_create_threadsafe_function(env, argv[2], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cb_timer_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok) 
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[3], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 3rd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    }

    int id = igs_timer_start(delay, times, timer_fn, threadsafe_context);
    napi_value id_js;
    convert_int_to_napi(env, id, &id_js);
    return id_js;
}

napi_value node_igs_timer_stop(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    int id;
    convert_napi_to_int(env, argv[0], &id);
    igs_timer_stop(id);
    return NULL;
}

napi_value init_admin_config_utils(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_version, "version", exports);
    exports = enable_callback_into_js(env, node_igs_protocol, "protocol", exports);
    exports = enable_callback_into_js(env, node_igs_net_devices_list, "netDevicesList", exports);
    exports = enable_callback_into_js(env, node_igs_net_addresses_list, "netAddressesList", exports);
    exports = enable_callback_into_js(env, node_igs_command_line, "commandLine", exports);
    exports = enable_callback_into_js(env, node_igs_set_command_line, "setCommandLine", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_set_outputs_request, "mappingSetOutputsRequest", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_outputs_request, "mappingOutputsRequest", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_console, "logSetConsole", exports);
    exports = enable_callback_into_js(env, node_igs_log_console, "logConsole", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_console_level, "logSetConsoleLevel", exports);
    exports = enable_callback_into_js(env, node_igs_log_console_level, "logConsoleLevel", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_console_color, "logSetConsoleColor", exports);
    exports = enable_callback_into_js(env, node_igs_log_console_color, "logConsoleColor", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_stream, "logSetStream", exports);    
    exports = enable_callback_into_js(env, node_igs_log_stream, "logStream", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_file, "logSetFile", exports);
    exports = enable_callback_into_js(env, node_igs_log_file, "logFile", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_file_path, "logSetFilePath", exports);
    exports = enable_callback_into_js(env, node_igs_log_file_path, "logFilePath", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_file_level, "logSetFileLevel", exports);
    exports = enable_callback_into_js(env, node_igs_log_set_file_max_line_length, "logSetFileMaxLineLength", exports);
    exports = enable_callback_into_js(env, node_igs_log_include_services, "logIncludeServices", exports);
    exports = enable_callback_into_js(env, node_igs_log_include_data, "logIncludeData", exports);
    exports = enable_callback_into_js(env, node_igs_trace, "trace", exports);
    exports = enable_callback_into_js(env, node_igs_debug, "debug", exports);
    exports = enable_callback_into_js(env, node_igs_info, "info", exports);
    exports = enable_callback_into_js(env, node_igs_warn, "warn", exports);
    exports = enable_callback_into_js(env, node_igs_error, "error", exports);
    exports = enable_callback_into_js(env, node_igs_fatal, "fatal", exports);
    exports = enable_callback_into_js(env, node_igs_definition_set_path, "definitionSetPath", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_set_path, "mappingSetPath", exports);
    exports = enable_callback_into_js(env, node_igs_definition_save, "definitionSave", exports);
    exports = enable_callback_into_js(env, node_igs_mapping_save, "mappingSave", exports);
#if defined __unix__ || defined __APPLE__ || defined __linux__
    exports = enable_callback_into_js(env, node_igs_set_ipc_dir, "setIpcDir", exports);
    exports = enable_callback_into_js(env, node_igs_ipc_dir, "ipcDir", exports);
#endif
    exports = enable_callback_into_js(env, node_igs_set_ipc, "setIpc", exports);
    exports = enable_callback_into_js(env, node_igs_has_ipc, "hasIpc", exports);
    exports = enable_callback_into_js(env, node_igs_timer_start, "timerStart", exports);
    exports = enable_callback_into_js(env, node_igs_timer_stop, "timerStop", exports);
    return exports;
}
