/*  =========================================================================
    io - io API

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

threadsafe_context_hash_t *observed_input_contexts = NULL;
threadsafe_context_hash_t *observed_output_contexts = NULL;
threadsafe_context_hash_t *observed_attribute_contexts = NULL;

napi_value node_igs_input_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igs_input_bool(name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_input_int(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igs_input_int(name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_input_double(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igs_input_double(name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_input_string(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igs_input_string(name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igs_input_data(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igs_input_data(name, &data, &size); 
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igs_output_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igs_output_bool(name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_output_int(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igs_output_int(name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_output_double(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igs_output_double(name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_output_string(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igs_output_string(name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igs_output_data(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igs_output_data(name, &data, &size); 
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igs_attribute_bool(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value = igs_attribute_bool(name);
    free(name);
    napi_value value_js;
    convert_bool_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_parameter_bool(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_bool instead.");
    return node_igs_attribute_bool(env, info);
}

napi_value node_igs_attribute_int(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value = igs_attribute_int(name);
    free(name);
    napi_value value_js;
    convert_int_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_parameter_int(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_int instead.");
    return node_igs_attribute_int(env, info);
}

napi_value node_igs_attribute_double(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value = igs_attribute_double(name);
    free(name);
    napi_value value_js;
    convert_double_to_napi(env, value, &value_js);
    return value_js;
}

napi_value node_igs_parameter_double(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_double instead.");
    return node_igs_attribute_double(env, info);
}

napi_value node_igs_attribute_string(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = igs_attribute_string(name);
    free(name);
    napi_value value_js;
    convert_string_to_napi(env, value, &value_js);
    if (value)
        free(value);
    return value_js;
}

napi_value node_igs_parameter_string(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_string instead.");
    return node_igs_attribute_string(env, info);
}
 
napi_value node_igs_attribute_data(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data = NULL;
    size_t size;
    igs_attribute_data(name, &data, &size); 
    free(name);
    napi_value data_js;
    convert_data_to_napi(env, data, size, &data_js);
    if (data != NULL)
        free(data);
    return data_js;
}

napi_value node_igs_parameter_data(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_data instead.");
    return node_igs_attribute_data(env, info);
}

napi_value node_igs_input_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igs_input_set_bool(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igs_input_set_int(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igs_input_set_double(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igs_input_set_string(name, value); 
    free(name);
    free(value);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_set_impulsion(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_input_set_impulsion(name); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    size_t length_value;
    void *data;
    convert_napi_to_data(env, argv[1], &data, &length_value);
    int success = igs_input_set_data(name, data, length_value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igs_output_set_bool(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igs_output_set_int(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igs_output_set_double(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igs_output_set_string(name, value); 
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    free(name);
    free(value);
    return success_js;
}

napi_value node_igs_output_set_impulsion(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int success = igs_output_set_impulsion(name); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data;
    size_t size;
    convert_napi_to_data(env, argv[1], &data, &size);
    int success = igs_output_set_data(name, data, size); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_attribute_set_bool(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);
    int success = igs_attribute_set_bool(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_set_bool(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_bool instead.");
    return node_igs_attribute_set_bool(env, info);
}

napi_value node_igs_attribute_set_int(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int value;
    convert_napi_to_int(env, argv[1], &value);
    int success = igs_attribute_set_int(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_set_int(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_int instead.");
    return node_igs_attribute_set_int(env, info);
}

napi_value node_igs_attribute_set_double(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    int success = igs_attribute_set_double(name, value); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_set_double(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_double instead.");
    return node_igs_attribute_set_double(env, info);
}

napi_value node_igs_attribute_set_string(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *value = convert_napi_to_string(env, argv[1]);
    int success = igs_attribute_set_string(name, value); 
    free(name);
    free(value);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_set_string(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_string instead.");
    return node_igs_attribute_set_string(env, info);
}

napi_value node_igs_attribute_set_data(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    void *data;
    size_t size;
    convert_napi_to_data(env, argv[1], &data, &size);
    int success = igs_attribute_set_data(name, data, size); 
    free(name);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_set_data(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_data instead.");
    return node_igs_attribute_set_data(env, info);
}

napi_value node_igs_clear_input(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_clear_input(name); 
    free(name);
    return NULL;
}

napi_value node_igs_clear_output(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_clear_output(name); 
    free(name);
    return NULL;
}

napi_value node_igs_clear_attribute(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_clear_attribute(name); 
    free(name);
    return NULL;
}

napi_value node_igs_clear_parameter(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_clear_attribute instead.");
    return node_igs_clear_attribute(env, info);
}

static void cbIO_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    io_callback_args_t * callback_arg = (io_callback_args_t *) data;
    napi_value argv[5];
    convert_int_to_napi(env, callback_arg->io_type, &argv[0]);
    convert_string_to_napi(env, callback_arg->name, &argv[1]);
    convert_int_to_napi(env, callback_arg->value_type, &argv[2]);
    convert_value_IO_into_napi(env, callback_arg->value_type, callback_arg->value, callback_arg->value_size, &argv[3]);
    if (callback_arg->my_data_ref == NULL)
        convert_null_to_napi(env, &argv[4]);
    else {
        status = napi_get_reference_value(env, callback_arg->my_data_ref, &argv[4]);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get reference value.");
    }
    free(callback_arg->name);
    free(callback_arg->value);
    free(callback_arg);
    
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Impossible to get undefined.");
    }

    // Callback into javascript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Unable to call javascript function.");
}

void io_fn(igs_io_type_t io_type, const char* name, igs_io_value_type_t value_type, void* value, size_t value_size, void* my_data) {
    threadsafe_context_t *threadsafe_context = (threadsafe_context_t *) my_data;
    io_callback_args_t *callback_arg =  calloc(1, sizeof(io_callback_args_t));
    callback_arg->io_type = io_type;
    callback_arg->name = strdup(name);
    callback_arg->value_type = value_type;
    callback_arg->value = calloc(1, value_size);
    memcpy(callback_arg->value, value, value_size);
    callback_arg->value_size = value_size;
    callback_arg->my_data_ref = threadsafe_context->my_data_ref;
    napi_call_threadsafe_function(threadsafe_context->threadsafe_func, callback_arg, napi_tsfn_nonblocking);
}

napi_value node_igs_observe_input(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe context
    threadsafe_context_t *threadsafe_context = calloc(1, sizeof(threadsafe_context_t));
    threadsafe_context_hash_t *threadsafe_hash = NULL;
    HASH_FIND_STR (threadsafe_hash, name, threadsafe_hash);
    if (threadsafe_hash == NULL) {
        threadsafe_hash = (threadsafe_context_hash_t *) zmalloc (sizeof (threadsafe_context_hash_t));
        threadsafe_hash->key = strdup (name);
        threadsafe_hash->list = NULL;
        HASH_ADD_STR (observed_input_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/inputCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIO_into_js, &(threadsafe_context->threadsafe_func));
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
        if (status != napi_ok) {
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
        }
    } 

    igs_observe_input(name, io_fn, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igs_observe_output(napi_env env, napi_callback_info info) {
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
        HASH_ADD_STR (observed_output_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/outputCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIO_into_js, &(threadsafe_context->threadsafe_func));
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
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    } 

    igs_observe_output(name, io_fn, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igs_observe_attribute(napi_env env, napi_callback_info info) {
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
        HASH_ADD_STR (observed_attribute_contexts, key, threadsafe_hash);
    }
    LL_APPEND (threadsafe_hash->list, threadsafe_context);

    napi_status status;
    napi_value async_name;
    status = napi_create_string_utf8(env, "ingescape/attributeCallback", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok)
        trigger_exception(env, NULL, "Invalid name for async_name napi_value.");
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIO_into_js, &(threadsafe_context->threadsafe_func));
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
            trigger_exception(env, NULL, "2nd argument must be a javascript Object or an Array or null or undefined.");
    }

    igs_observe_attribute(name, io_fn, threadsafe_context);
    free(name);
    return NULL;
}

napi_value node_igs_observe_parameter(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_observe_attribute instead.");
    return node_igs_observe_attribute(env, info);
}

napi_value node_igs_output_mute(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_output_mute(name);
    free(name);
    return NULL;
}

napi_value node_igs_output_unmute(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    igs_output_unmute(name);
    free(name);
    return NULL;
}

napi_value node_igs_output_is_muted(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool is_muted = igs_output_is_muted(name);
    free(name);
    napi_value is_muted_js;
    convert_bool_to_napi(env, is_muted, &is_muted_js);
    return is_muted_js;
}

napi_value node_igs_input_type(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igs_input_type(name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igs_output_type(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igs_output_type(name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igs_attribute_type(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    int type = igs_attribute_type(name);
    free(name);
    napi_value type_js;
    convert_int_to_napi(env, type, &type_js);
    return type_js;
}

napi_value node_igs_parameter_type(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_type instead.");
    return node_igs_attribute_type(env, info);
}

napi_value node_igs_input_count(napi_env env, napi_callback_info info) {
    int count = igs_input_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_output_count(napi_env env, napi_callback_info info) {
    int count = igs_output_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_attribute_count(napi_env env, napi_callback_info info) {
    int count = igs_attribute_count();
    napi_value count_js;
    convert_int_to_napi(env, count, &count_js);
    return count_js;
}

napi_value node_igs_parameter_count(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_count instead.");
    return node_igs_attribute_count(env, info);
}

napi_value node_igs_input_list(napi_env env, napi_callback_info info) {
    size_t inputs_nb = 0;
    char **inputs = igs_input_list(&inputs_nb);
    napi_value inputs_js;
    convert_string_list_to_napi_array(env, inputs, inputs_nb, &inputs_js);
    if (inputs != NULL)
        igs_free_io_list(inputs, inputs_nb);
    return inputs_js;
}

napi_value node_igs_output_list(napi_env env, napi_callback_info info) {
    size_t outputs_nb = 0;
    char **outputs = igs_output_list(&outputs_nb);
    napi_value outputs_js;
    convert_string_list_to_napi_array(env, outputs, outputs_nb, &outputs_js);
    if (outputs != NULL)
        igs_free_io_list(outputs, outputs_nb);
    return outputs_js;
}

napi_value node_igs_attribute_list(napi_env env, napi_callback_info info) {
    size_t attributes_nb = 0;
    char **attributes = igs_attribute_list(&attributes_nb);
    napi_value attributes_js;
    convert_string_list_to_napi_array(env, attributes, attributes_nb, &attributes_js);
    if (attributes != NULL)
        igs_free_io_list(attributes, attributes_nb);
    return attributes_js;
}

napi_value node_igs_parameter_list(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_list instead.");
    return node_igs_attribute_list(env, info);
}

napi_value node_igs_input_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igs_input_exists(name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igs_output_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igs_output_exists(name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igs_attribute_exists(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    bool exists = igs_attribute_exists(name);
    free(name);
    napi_value exists_js;
    convert_bool_to_napi(env, exists, &exists_js);
    return exists_js;
}

napi_value node_igs_parameter_exists(napi_env env, napi_callback_info info) {
      igs_warn("this function is deprecated, please use node_igs_attribute_exists instead.");
    return node_igs_attribute_exists(env, info);
}

napi_value node_igs_constraints_enforce(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    bool enforce;
    convert_napi_to_bool(env, argv[0], &enforce);
    igs_constraints_enforce(enforce);
    return NULL;
}

napi_value node_igs_input_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igs_input_add_constraint(name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igs_output_add_constraint(name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_attribute_add_constraint(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *constraint = convert_napi_to_string(env, argv[1]);
    int success = igs_attribute_add_constraint(name, constraint);
    free(name);
    free(constraint);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_parameter_add_constraint(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_add_constraint instead."); 
    return node_igs_attribute_add_constraint(env, info);   
}

napi_value node_igs_input_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    int success = igs_input_set_description(name, description);
    free(name);
    free(description);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_input_description(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = igs_input_description(name);
    free(name);
    napi_value description_js;
    convert_string_to_napi(env, description, &description_js);
    free(description);
    return description_js;
}

napi_value node_igs_output_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    int success = igs_output_set_description(name, description);
    free(name);
    free(description);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_description(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = igs_output_description(name);
    free(name);
    napi_value description_js;
    convert_string_to_napi(env, description, &description_js);
    free(description);
    return description_js;
}

napi_value node_igs_attribute_set_description(napi_env env, napi_callback_info info) {
    napi_value argv[2];
    get_function_arguments(env, info, 2, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = convert_napi_to_string(env, argv[1]);
    int success = igs_attribute_set_description(name, description);
    free(name);
    free(description);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_attribute_description(napi_env env, napi_callback_info info) {
    napi_value argv[1];
    get_function_arguments(env, info, 1, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *description = igs_attribute_description(name);
    free(name);
    napi_value description_js;
    convert_string_to_napi(env, description, &description_js);
    free(description);
    return description_js;
}

napi_value node_igs_parameter_set_description(napi_env env, napi_callback_info info) {
    igs_warn("this function is deprecated, please use node_igs_attribute_set_description instead.");
    return node_igs_attribute_set_description(env, info);
}

napi_value node_igs_input_set_detailed_type(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *type_name = convert_napi_to_string(env, argv[1]);
    char *specification = convert_napi_to_string(env, argv[2]);
    int success = igs_input_set_detailed_type(name, type_name, specification);
    free(name);
    free(type_name);
    free(specification);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_output_set_detailed_type(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *type_name = convert_napi_to_string(env, argv[1]);
    char *specification = convert_napi_to_string(env, argv[2]);
    int success = igs_output_set_detailed_type(name, type_name, specification);
    free(name);
    free(type_name);
    free(specification);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value node_igs_attribute_set_detailed_type(napi_env env, napi_callback_info info) {
    napi_value argv[3];
    get_function_arguments(env, info, 3, argv);
    char *name = convert_napi_to_string(env, argv[0]);
    char *type_name = convert_napi_to_string(env, argv[1]);
    char *specification = convert_napi_to_string(env, argv[2]);
    int success = igs_attribute_set_detailed_type(name, type_name, specification);
    free(name);
    free(type_name);
    free(specification);
    napi_value success_js;
    convert_int_to_napi(env, success, &success_js);
    return success_js;
}

napi_value init_io(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_input_bool, "inputBool", exports);
    exports = enable_callback_into_js(env, node_igs_input_int, "inputInt", exports);
    exports = enable_callback_into_js(env, node_igs_input_double, "inputDouble", exports);
    exports = enable_callback_into_js(env, node_igs_input_string, "inputString", exports);
    exports = enable_callback_into_js(env, node_igs_input_data, "inputData", exports);
    exports = enable_callback_into_js(env, node_igs_output_bool, "outputBool", exports);
    exports = enable_callback_into_js(env, node_igs_output_int, "outputInt", exports);
    exports = enable_callback_into_js(env, node_igs_output_double, "outputDouble", exports);
    exports = enable_callback_into_js(env, node_igs_output_string, "outputString", exports);
    exports = enable_callback_into_js(env, node_igs_output_data, "outputData", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_bool, "attributeBool", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_bool, "parameterBool", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_int, "attributeInt", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_int, "parameterInt", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_double, "attributeDouble", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_double, "parameterDouble", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_string, "attributeString", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_string, "parameterString", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_data, "attributeData", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_data, "parameterData", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_bool, "inputSetBool", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_int, "inputSetInt", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_double, "inputSetDouble", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_string, "inputSetString", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_impulsion, "inputSetImpulsion", exports);
    exports = enable_callback_into_js(env, node_igs_input_set_data, "inputSetData", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_bool, "outputSetBool", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_int, "outputSetInt", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_double, "outputSetDouble", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_string, "outputSetString", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_impulsion, "outputSetImpulsion", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_data, "outputSetData", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_bool, "attributeSetBool", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_set_bool, "parameterSetBool", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_int, "attributeSetInt", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_set_int, "parameterSetInt", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_double, "attributeSetDouble", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_set_double, "parameterSetDouble", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_string, "attributeSetString", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_set_string, "parameterSetString", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_data, "attributeSetData", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_set_data, "parameterSetData", exports);
    exports = enable_callback_into_js(env, node_igs_clear_input, "clearInput", exports);
    exports = enable_callback_into_js(env, node_igs_clear_output, "clearOutput", exports);
    exports = enable_callback_into_js(env, node_igs_clear_parameter, "clearParameter", exports);
    exports = enable_callback_into_js(env, node_igs_clear_attribute, "clearAttribute", exports);
    exports = enable_callback_into_js(env, node_igs_observe_input, "observeInput", exports);
    exports = enable_callback_into_js(env, node_igs_observe_output, "observeOutput", exports);
    exports = enable_callback_into_js(env, node_igs_observe_parameter, "observeParameter", exports);
    exports = enable_callback_into_js(env, node_igs_observe_attribute, "observeAttribute", exports);
    exports = enable_callback_into_js(env, node_igs_output_mute, "outputMute", exports);
    exports = enable_callback_into_js(env, node_igs_output_unmute, "outputUnmute", exports);
    exports = enable_callback_into_js(env, node_igs_output_is_muted, "outputIsMuted", exports);
    exports = enable_callback_into_js(env, node_igs_input_type, "inputType", exports);
    exports = enable_callback_into_js(env, node_igs_output_type, "outputType", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_type, "parameterType", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_type, "attributeType", exports);
    exports = enable_callback_into_js(env, node_igs_input_count, "inputCount", exports);
    exports = enable_callback_into_js(env, node_igs_output_count, "outputCount", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_count, "parameterCount", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_count, "attributeCount", exports);
    exports = enable_callback_into_js(env, node_igs_input_list, "inputList", exports);
    exports = enable_callback_into_js(env, node_igs_output_list, "outputList", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_list, "parameterList", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_list, "attributeList", exports);
    exports = enable_callback_into_js(env, node_igs_input_exists, "inputExists", exports);
    exports = enable_callback_into_js(env, node_igs_output_exists, "outputExists", exports);
    exports = enable_callback_into_js(env, node_igs_parameter_exists, "parameterExists", exports); 
    exports = enable_callback_into_js(env, node_igs_attribute_exists, "attributeExists", exports);   
    exports = enable_callback_into_js(env, node_igs_constraints_enforce, "constraintsEnforce", exports);  
    exports = enable_callback_into_js(env, node_igs_input_add_constraint, "inputAddConstraint", exports);  
    exports = enable_callback_into_js(env, node_igs_output_add_constraint, "outputAddConstraint", exports);  
    exports = enable_callback_into_js(env, node_igs_parameter_add_constraint, "parameterAddConstraint", exports);  
    exports = enable_callback_into_js(env, node_igs_attribute_add_constraint, "attributeAddConstraint", exports);  
    exports = enable_callback_into_js(env, node_igs_input_set_description, "inputSetDescription", exports);  
    exports = enable_callback_into_js(env, node_igs_input_description, "inputDescription", exports);  
    exports = enable_callback_into_js(env, node_igs_output_set_description, "outputSetDescription", exports);  
    exports = enable_callback_into_js(env, node_igs_output_description, "outputDescription", exports);  
    exports = enable_callback_into_js(env, node_igs_parameter_set_description, "parameterSetDescription", exports);  
    exports = enable_callback_into_js(env, node_igs_attribute_set_description, "attributeSetDescription", exports); 
    exports = enable_callback_into_js(env, node_igs_attribute_description, "attributeDescription", exports); 
    exports = enable_callback_into_js(env, node_igs_input_set_detailed_type, "inputSetDetailedType", exports);
    exports = enable_callback_into_js(env, node_igs_output_set_detailed_type, "outputSetDetailedType", exports);
    exports = enable_callback_into_js(env, node_igs_attribute_set_detailed_type, "attributeSetDetailedType", exports);
    //NOTE: igs_parameter_set_detailed_type is not binded because it was already obsolete (in favor of igs_attribute_set_detailed_type) when the binding was updated

    return exports;
}
