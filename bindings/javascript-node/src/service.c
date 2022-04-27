/*  =========================================================================
    service - create/edit/remove services and services arguments

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

threadsafe_context_hash_t *service_contexts = NULL;

napi_value node_igs_service_args_add_int(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    napi_status status;
    get_function_arguments(env, info, 2, argv);
    bool is_array;
    status = napi_is_array(env, argv[0], &is_array);
    if ((status != napi_ok) || !is_array)
        trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
    int test;
    status = napi_get_value_int32(env, argv[1], &test);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid int was passed as argument.");

    uint32_t length;
    status = napi_get_array_length(env, argv[0], &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");

    napi_value service_arg, arg_type;
    status = napi_create_object(env, &service_arg);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create object.");
    convert_int_to_napi(env, IGS_INTEGER_T, &arg_type);
    status = napi_set_named_property(env, service_arg, "type", arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");
    status = napi_set_named_property(env, service_arg, "value", argv[1]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

    // Add object to array
    status = napi_set_element(env, argv[0], length, service_arg);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to set element in array.");
    return argv[0];
}

napi_value node_igs_service_args_add_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    napi_status status;
    get_function_arguments(env, info, 2, argv);
    bool is_array;
    status = napi_is_array(env, argv[0], &is_array);
    if ((status != napi_ok) || !is_array)
        trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
    bool test;
    status = napi_get_value_bool(env, argv[1], &test);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid bool was passed as argument.");

    uint32_t length;
    status = napi_get_array_length(env, argv[0], &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");

    napi_value service_arg, arg_type;
    status = napi_create_object(env, &service_arg);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create object.");
    convert_int_to_napi(env, IGS_BOOL_T, &arg_type);
    status = napi_set_named_property(env, service_arg, "type", arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");
    status = napi_set_named_property(env, service_arg, "value", argv[1]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

    // Add object to array
    status = napi_set_element(env, argv[0], length, service_arg);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to set element in array.");
    return argv[0];
}

napi_value node_igs_service_args_add_double(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    napi_status status;
    get_function_arguments(env, info, 2, argv);
    bool is_array;
    status = napi_is_array(env, argv[0], &is_array);
    if ((status != napi_ok) || !is_array)
        trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
    double test;
    status = napi_get_value_double(env, argv[1], &test);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid double was passed as argument.");

    uint32_t length;
    status = napi_get_array_length(env, argv[0], &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");

    napi_value service_arg, arg_type;
    status = napi_create_object(env, &service_arg);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create object.");
    convert_int_to_napi(env, IGS_DOUBLE_T, &arg_type);
    status = napi_set_named_property(env, service_arg, "type", arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");
    status = napi_set_named_property(env, service_arg, "value", argv[1]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

    // Add object to array
    status = napi_set_element(env, argv[0], length, service_arg);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to set element in array.");
    return argv[0];
}

napi_value node_igs_service_args_add_string(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    napi_status status;
    get_function_arguments(env, info, 2, argv);
    bool is_array;
    status = napi_is_array(env, argv[0], &is_array);
    if ((status != napi_ok) || !is_array)
        trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
    size_t test = 0;
    status = napi_get_value_string_utf8(env, argv[1], NULL, 0, &test);
    if (status != napi_ok)
    	trigger_exception(env, NULL, "Invalid string was passed as argument.");

    uint32_t length;
    status = napi_get_array_length(env, argv[0], &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");

    napi_value service_arg, arg_type;
    status = napi_create_object(env, &service_arg);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create object.");
    convert_int_to_napi(env, IGS_STRING_T, &arg_type);
    status = napi_set_named_property(env, service_arg, "type", arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");
    status = napi_set_named_property(env, service_arg, "value", argv[1]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

    // Add object to array
    status = napi_set_element(env, argv[0], length, service_arg);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to set element in array.");
    return argv[0];
}

napi_value node_igs_service_args_add_data(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    napi_status status;
    get_function_arguments(env, info, 2, argv);
    bool is_array;
    status = napi_is_array(env, argv[0], &is_array);
    if ((status != napi_ok) || !is_array)
        trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of element.");
    if (value_type != napi_null) {
        void *test;
        size_t test_size;
        status = napi_get_arraybuffer_info(env, argv[1], &test, &test_size);
        if (status != napi_ok) {
            trigger_exception(env, NULL, "Invalid array buffer was passed as argument (null also accepted).");
            return 0;
        }
    }

    uint32_t length;
    status = napi_get_array_length(env, argv[0], &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");

    napi_value service_arg, arg_type;
    status = napi_create_object(env, &service_arg);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create object.");
    convert_int_to_napi(env, IGS_DATA_T, &arg_type);
    status = napi_set_named_property(env, service_arg, "type", arg_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");
    status = napi_set_named_property(env, service_arg, "value", argv[1]);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

    // Add object to array
    status = napi_set_element(env, argv[0], length, service_arg);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to set element in array.");
    return argv[0];
}

napi_value node_igs_service_call(napi_env env, napi_callback_info info) {
    napi_value argv[4];
    get_function_arguments(env, info, 4, argv);
    char *agent_name_or_uuid = convert_napi_to_string(env, argv[0]);
    char *service_name = convert_napi_to_string(env, argv[1]);
    char *token = convert_napi_to_string(env, argv[3]);
    napi_status status;
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type.");

    int success;
    if ((value_type != napi_null) && (value_type != napi_undefined)) {
        bool is_array;
        status = napi_is_array(env, argv[2], &is_array);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Invalid array was passed as argument.");
        if (!is_array)
            trigger_exception(env, NULL, "N-API: Argument must be an array or null or undefined");

        uint32_t length;
        status = napi_get_array_length(env, argv[2], &length);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get array length.");

        if (length == 0) 
            success = igs_service_call(agent_name_or_uuid, service_name, NULL, token);
        else {
            igs_service_arg_t * list = NULL;
            service_args_c_from_js(env, argv[2], &list);
            success = igs_service_call(agent_name_or_uuid, service_name, &list, token);
        }        
    }
    else
        success = igs_service_call(agent_name_or_uuid, service_name, NULL, token);   
    free(agent_name_or_uuid);
    free(service_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

static void cb_service_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    service_callback_args_t * callback_arg = (service_callback_args_t *) data;
    napi_value argv[6];
    convert_string_to_napi(env, callback_arg->sender_agent_name, &argv[0]);
    convert_string_to_napi(env, callback_arg->sender_agent_uuid, &argv[1]);
    convert_string_to_napi(env, callback_arg->service_name, &argv[2]);    
    service_args_js_from_c(env, callback_arg->first_argument, &argv[3]); //convert chained list to arraybuffer
    convert_string_to_napi(env, callback_arg->token, &argv[4]);    
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[5]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[5]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }
    free(callback_arg->sender_agent_name);
    free(callback_arg->sender_agent_uuid);
    free(callback_arg->service_name);
    igs_service_args_destroy(&(callback_arg->first_argument));
    free(callback_arg->token);
    free(callback_arg);
    
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to get undefined.");

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 6, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void service_callback (const char *sender_agent_name, const char *sender_agent_uuid,
    const char *service_name, igs_service_arg_t *first_argument, size_t args_nb,
    const char *token, void* my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    service_callback_args_t* callback_arg = calloc(1, sizeof(service_callback_args_t));
    callback_arg->sender_agent_name = strdup(sender_agent_name);
    callback_arg->sender_agent_uuid = strdup(sender_agent_uuid);
    callback_arg->service_name = strdup(service_name);
    if (first_argument != NULL)
        callback_arg->first_argument = igs_service_args_clone(first_argument); //clone it to keep it
    else
        callback_arg->first_argument = NULL;
    callback_arg->args_nb = args_nb;
    if (token != NULL)
        callback_arg->token = strdup(token);
    else
        callback_arg->token = NULL; 
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}   

napi_value node_igs_service_init(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe function
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (service_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/serviceCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cb_service_into_js, &(threadsafe_context->threadsafe_func));
    if (status != napi_ok)
        trigger_exception(env, NULL, "Impossible to create threadsafe function.");

    // Create reference for callback arguments if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    if ((value_type == napi_null) || (value_type == napi_undefined))
        threadsafe_context->my_data_ref = NULL;
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafe_context->my_data_ref));
        if (status != napi_ok)
            trigger_exception(env, NULL, "3rd argument must be a javascript Object or an Array or null or undefined.");
    }

    int success = igs_service_init(name, service_callback, threadsafe_context);
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_service_remove(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_service_remove(name);
    if (success == IGS_SUCCESS) {
        // Free associated callback data
        threadsafe_context_hash_t *threadsafe_context_hash = NULL;
        HASH_FIND_STR (service_contexts, name, threadsafe_context_hash);
        if (threadsafe_context_hash != NULL) {
            HASH_DEL (service_contexts, threadsafe_context_hash);
            free_threadsafe_context_hash(env, &threadsafe_context_hash);
        }
    }
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_service_arg_add(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *service_name = convert_napi_to_string(env, argv[0]);
    char *arg_name = convert_napi_to_string(env, argv[1]);
	int type;
    convert_napi_to_int(env, argv[2], &type);
	int success = igs_service_arg_add(service_name, arg_name, type);
    free(service_name);
    free(arg_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_service_arg_remove(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *service_name = convert_napi_to_string(env, argv[0]);
    char *arg_name = convert_napi_to_string(env, argv[1]);
	int success = igs_service_arg_remove(service_name, arg_name);
    free(service_name);
    free(arg_name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_service_count(napi_env env, napi_callback_info info) { 
    size_t count = igs_service_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_service_exists(napi_env env, napi_callback_info info) { 
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igs_service_exists(name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igs_service_list(napi_env env, napi_callback_info info) { 
    size_t elements_nb = 0;
    char **services_list = igs_service_list(&elements_nb);
    napi_value napi_services;
    convert_string_list_to_napi_array(env, services_list, elements_nb, &napi_services);
    if (services_list != NULL)
        igs_free_services_list(services_list, elements_nb);
    return napi_services;
}

napi_value node_igs_service_args_list(napi_env env, napi_callback_info info) { 
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_service_arg_t * head = igs_service_args_first(name);
    free(name);
    napi_status status;
    napi_value args_list_js;
    status = napi_create_array(env, &args_list_js);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create array.");

    igs_service_arg_t * elt, * tmp;
    int i = 0;
    napi_value arg_name, arg_type, service_argument;
    LL_FOREACH_SAFE(head, elt, tmp) {
        status = napi_create_object(env, &service_argument);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to create object.");

        // Add argument's name & argument's type
        convert_string_to_napi(env, elt->name, &arg_name);
        status = napi_set_named_property(env, service_argument, "name", arg_name);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set name of service argument.");

        convert_int_to_napi(env, elt->type, &arg_type);
        status = napi_set_named_property(env, service_argument, "type", arg_type);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set type of service argument.");

        status = napi_set_element(env, args_list_js, i, service_argument);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set service argument in array.");
        i++;
    }
    return args_list_js;
}

napi_value node_igs_service_args_count(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    size_t args_count = igs_service_args_count(name);
    free(name);
    napi_value args_count_js;
    convert_int_to_napi(env, args_count, &args_count_js);
    return args_count_js;
}

napi_value node_igs_service_arg_exists(napi_env env, napi_callback_info info) { 
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *service_name = convert_napi_to_string(env, argv[0]);
    char *arg_name = convert_napi_to_string(env, argv[1]);
    bool exists = igs_service_arg_exists(service_name, arg_name);
    free(service_name);
    free(arg_name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value init_service(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_service_args_add_int, "serviceArgsAddInt", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_add_bool, "serviceArgsAddBool", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_add_double, "serviceArgsAddDouble", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_add_string, "serviceArgsAddString", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_add_data, "serviceArgsAddData", exports);
    exports = enable_callback_into_js(env, node_igs_service_call, "serviceCall", exports);
    exports = enable_callback_into_js(env, node_igs_service_init, "serviceInit", exports);
    exports = enable_callback_into_js(env, node_igs_service_remove, "serviceRemove", exports);
    exports = enable_callback_into_js(env, node_igs_service_arg_add, "serviceArgAdd", exports);
    exports = enable_callback_into_js(env, node_igs_service_arg_remove, "serviceArgRemove", exports);
    exports = enable_callback_into_js(env, node_igs_service_count, "serviceCount", exports);
    exports = enable_callback_into_js(env, node_igs_service_exists, "serviceExists", exports);
    exports = enable_callback_into_js(env, node_igs_service_list, "serviceList", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_list, "serviceArgsList", exports);
    exports = enable_callback_into_js(env, node_igs_service_args_count, "serviceArgsCount", exports);
    exports = enable_callback_into_js(env, node_igs_service_arg_exists, "serviceArgExists", exports);
    return exports;
}
