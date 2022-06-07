/*  =========================================================================
    private - utils for napi conversions

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "../include/private.h"

void trigger_exception(napi_env env, const char *code, const char *message) {
    napi_value error_code, error_msg;
    if (code == NULL)
        error_code = NULL;
    else 
        convert_string_to_napi(env, code, &error_code);
    if (message == NULL)
        error_msg = NULL;
    else
        convert_string_to_napi(env, message, &error_msg);

    napi_value error;
    napi_create_error(env, error_code, error_msg, &error);

    // Trigger an 'uncaughtException' in JavaScript
    napi_fatal_exception(env, error);
    return;
}

char * convert_napi_to_string(napi_env env, napi_value value) {
    napi_status status;
    size_t length_value = 0;
    status = napi_get_value_string_utf8(env, value, NULL, 0, &length_value);
    if (status != napi_ok) {
    	trigger_exception(env, NULL, "Invalid string was passed as argument.");
        return NULL;
    }
    char* converted_value = malloc(sizeof(char) * (length_value + 1));
    napi_get_value_string_utf8(env, value, converted_value, length_value + 1, &length_value);
    return converted_value;
}

int convert_napi_to_bool(napi_env env, napi_value value, bool *converted_value) {
    napi_status status;
    status = napi_get_value_bool(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Invalid bool was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_int(napi_env env, napi_value value, int *converted_value) {
    napi_status status;
    status = napi_get_value_int32(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Invalid int was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_uint64(napi_env env, napi_value value, uint64_t *convertedValue) {
    napi_status status;
    bool lossless;
    status = napi_get_value_bigint_uint64(env, value, convertedValue, &lossless);;
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Invalid bigint was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_double(napi_env env, napi_value value, double *converted_value) {
    napi_status status;
    status = napi_get_value_double(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Invalid double was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_data(napi_env env, napi_value value, void **converted_value, size_t *size_converted_value) {
    napi_status status;
    napi_valuetype value_type;
    status = napi_typeof(env, value, &value_type);
    if (status != napi_ok) 
        trigger_exception(env, NULL, "N-API : Unable to get napi value type of element.");

    if ((value_type == napi_undefined) || (value_type == napi_null)) {
        *converted_value = NULL;
        *size_converted_value = 0;
    }
    else {
        status = napi_get_arraybuffer_info(env, value, converted_value, size_converted_value);
        if (status != napi_ok) {
            trigger_exception(env, NULL, "Invalid array buffer was passed as argument (null and undefined also accepted).");
            return 0;
        }
    }
    return 1;
}

int convert_int_to_napi(napi_env env, int value, napi_value *converted_value) {
    napi_status status;
    status = napi_create_int32(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to convert int value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_uint64_to_napi(napi_env env, uint64_t value, napi_value *converted_value) {
    napi_status status;
    status = napi_create_bigint_uint64(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to convert uint64_t value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_bool_to_napi(napi_env env, bool value, napi_value *converted_value) {
    napi_status status;
    status = napi_get_boolean(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to convert boolean value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_string_to_napi(napi_env env, const char *value, napi_value *converted_value) {
    napi_status status;
    if (value == NULL)
        status = napi_create_string_utf8(env, "", 0, converted_value);
    else
       status = napi_create_string_utf8(env, value, strlen(value), converted_value);

    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to convert string value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_double_to_napi(napi_env env, double value, napi_value *converted_value) {
    napi_status status;
    status = napi_create_double(env, value, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to convert double value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_null_to_napi(napi_env env, napi_value *converted_value) {
    napi_status status;
    status = napi_get_null(env, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to get null value into napi_value.");
        return 0;
    }
    return 1;
}

void array_buffer_collected(napi_env env, void *finalize_data, void *finalize_hint) {
    free(finalize_data);
    finalize_data = NULL;
}

int convert_data_to_napi(napi_env env, void *value, size_t size, napi_value *converted_value) {
    napi_status status;
    if (value == NULL)
        return convert_null_to_napi(env, converted_value);
    else {
        // Duplicate data value to keep reference to data while array buffer is not garbage collected
        void * copy_value = calloc(1, size);
        memcpy(copy_value, value, size);

        status = napi_create_external_arraybuffer(env, copy_value, size, array_buffer_collected, NULL, converted_value);
        if (status != napi_ok) {
            trigger_exception(env, NULL, "N-API : Unable to create array buffer into napi_value.");
            return 0;
        }
        return 1;
    }    
}

int convert_string_list_to_napi_array(napi_env env, char **list, size_t length, napi_value *converted_value) {
    napi_status status;
    status = napi_create_array_with_length(env, length, converted_value);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "N-API : Unable to create array.");
        return 0;
    }

    for (size_t i = 0; i < length; i++) {
        napi_value string_conv;
        convert_string_to_napi(env, list[i], &string_conv);
        status = napi_set_element(env, *converted_value, i, string_conv);
        if (status != napi_ok) {
            trigger_exception(env, NULL, "Unable to write element into array.");
            return 0;
        }
    }
    return 1;
}

int convert_value_IOP_into_napi(napi_env env, igs_iop_value_type_t type, void *value, size_t size, napi_value *value_napi) {
    if (value == NULL) {
        convert_null_to_napi(env, value_napi);
        return 1;
    }

    switch(type) {
        case IGS_INTEGER_T  :
            convert_int_to_napi(env, *((int *) value), value_napi);
            break;
        case IGS_DOUBLE_T  : 
            convert_double_to_napi(env, *((double *) value), value_napi);
            break;
        case IGS_STRING_T  :
            convert_string_to_napi(env, (char *) value, value_napi);
            break;
        case IGS_BOOL_T  : 
            convert_bool_to_napi(env, *((bool *) value), value_napi);
            break;
        case IGS_IMPULSION_T  :
            convert_null_to_napi(env, value_napi);
            break;
        case IGS_DATA_T  : {
            convert_data_to_napi(env, value, size, value_napi);
            break;
        }
        default : 
            trigger_exception(env, NULL, "Unknown igs_iop_value_type_t.");
            return 0;
    }
    return 1;
}

// to convert napi value into C value according to igs_iop_value_type_t value
void * convert_value_with_good_type(napi_env env, napi_value value, igs_iop_value_type_t type, size_t *size_convert) {
    void *c_value;
    *size_convert = 0;
    switch(type) {
        case IGS_INTEGER_T  :
            *size_convert = sizeof(int);
            c_value = (int *) malloc(*size_convert);
            convert_napi_to_int(env, value, c_value);
            break;
        case IGS_DOUBLE_T  :
            *size_convert = sizeof(double);
            c_value = (double *) malloc(*size_convert);
            convert_napi_to_double(env, value, c_value);
            break;
        case IGS_STRING_T  :
            c_value = (char *) convert_napi_to_string(env, value);
            *size_convert = (strlen(c_value)+1) * sizeof(char);
            break;
        case IGS_BOOL_T  :
            *size_convert = sizeof(bool);
            c_value = (bool *) malloc(*size_convert);
            convert_napi_to_bool(env, value, c_value);
            break;
        case IGS_IMPULSION_T  :
            *size_convert = 1 * sizeof(char);
            c_value = (char*) "";
            break;
        case IGS_DATA_T  :
            convert_napi_to_data(env, value, &c_value, size_convert);
            break;
        default : 
            *size_convert = 0;
            c_value = NULL;
            trigger_exception(env, NULL, "Unknow iop_type_js.");
    }
    return c_value;
}

void service_args_c_from_js(napi_env env, napi_value array, igs_service_arg_t **first_argument) {
    napi_status status;
    // Translate all the array elements to create list
    uint32_t length;
    status = napi_get_array_length(env, array, &length);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to get array length.");
    uint32_t i = 0;
    napi_value elt, service_type, service_value;
    int service_type_c;
    napi_valuetype elt_type;
    void *service_value_c = NULL;
    size_t size;    
    while (i < length) {
        status = napi_get_element(env, array, i, &elt);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get element in array.");
        status = napi_typeof(env, elt, &elt_type);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get napi value type of element.");
        if (elt_type != napi_object)
            trigger_exception(env, NULL, "N-API : Array must be composed of javascript object.");

        // Get napi service type and value
        status = napi_get_named_property(env, elt, "type", &service_type);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get service argument type (missing 'type' in one array object)");
        status = napi_get_named_property(env, elt, "value", &service_value);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to get service argument value (missing 'value' in one array object)");

        convert_napi_to_int(env, service_type, &service_type_c);
        service_value_c = NULL;
        switch (service_type_c) {
            case IGS_INTEGER_T:
                service_value_c = (int *) malloc(sizeof(int));
                convert_napi_to_int(env, service_value, service_value_c);
                igs_service_args_add_int(first_argument, *(int *)service_value_c);
                free(service_value_c);
                break;
            case IGS_DOUBLE_T:
                service_value_c = (double *) malloc(sizeof(double));
                convert_napi_to_double(env, service_value, service_value_c);
                igs_service_args_add_double(first_argument, *(double *)service_value_c);
                free(service_value_c);
                break;
            case IGS_BOOL_T:
                service_value_c = (bool *) malloc(sizeof(bool));
                convert_napi_to_bool(env, service_value, service_value_c);
                igs_service_args_add_bool(first_argument, *(bool *)service_value_c);
                free(service_value_c);
                break;
            case IGS_STRING_T:
                service_value_c = convert_napi_to_string(env, service_value);
                igs_service_args_add_string(first_argument, service_value_c);
                free(service_value_c);
                break;
            case IGS_IMPULSION_T:
                trigger_exception(env, NULL, "Type IMPULSION is not handled by a service.");
                break;
            case IGS_DATA_T:
                convert_napi_to_data(env, service_value, &service_value_c, &size);
                igs_service_args_add_data(first_argument, service_value_c, size);
                break;
            default :
                trigger_exception(env, NULL, "N-API : Unknown service argument type ('type' field)");
        }
        i++;
    }
}

void service_args_js_from_c(napi_env env, igs_service_arg_t *first_argument, napi_value *arrayJS) {
    napi_status status = napi_create_array(env, arrayJS);
    if (status != napi_ok)
        trigger_exception(env, NULL, "N-API : Unable to create array.");

    igs_service_arg_t *elt, *tmp;
    igs_service_arg_t *head = first_argument;
    napi_value service_arg, arg_name, arg_type, value_arg;
    int i = 0;
    LL_FOREACH_SAFE(head, elt, tmp) {
        status = napi_create_object(env, &service_arg);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to create object.");

        // Add argument's name & argument's type
        convert_string_to_napi(env, elt->name, &arg_name);
        status = napi_set_named_property(env, service_arg, "name", arg_name);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set service argument's name.");

        convert_int_to_napi(env, elt->type, &arg_type);
        status = napi_set_named_property(env, service_arg, "type", arg_type);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set service argument's type.");

        // Add argument's value
        switch (elt->type) {
            case IGS_INTEGER_T:
                convert_int_to_napi(env, elt->i, &value_arg);
                break;
            case IGS_DOUBLE_T:
                convert_double_to_napi(env, elt->d, &value_arg);
                break;
            case IGS_STRING_T:
                convert_string_to_napi(env, elt->c, &value_arg);
                break;
            case IGS_BOOL_T:
                convert_bool_to_napi(env, elt->b, &value_arg);
                break;
            case IGS_IMPULSION_T:
                trigger_exception(env, NULL, "Type IMPULSION is not handled by a service.");
                break;
            case IGS_DATA_T:
                if (elt->size == 0)
                    convert_null_to_napi(env, &value_arg);
                else
                    convert_data_to_napi(env, elt->data, elt->size, &value_arg);
                break;
            default : 
                trigger_exception(env, NULL, "Unknown type.");
        }
        status = napi_set_named_property(env, service_arg, "value", value_arg);
        if (status != napi_ok)
            trigger_exception(env, NULL, "N-API : Unable to set service argument's value.");

        // Add object to array
        status = napi_set_element(env, *arrayJS, i, service_arg);
        if (status != napi_ok) 
            trigger_exception(env, NULL, "N-API : Unable to set element in array.");
        i++;
    }
}

int get_function_arguments(napi_env env, napi_callback_info info, size_t argc, napi_value * argv) {
    napi_status status;
    size_t nb_infos = argc;
    status = napi_get_cb_info(env, info, &nb_infos, argv, NULL, NULL);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Unable to get function arguments.");
        return 0;
    }
    if (nb_infos != argc) {
        trigger_exception(env, NULL, "Wrong number of arguments pass in function.");
        return 0;
    }
    return 1;
}

napi_value enable_callback_into_js(napi_env env, napi_callback cb, const char * js_name, napi_value exports) {
    napi_status status;
    napi_value fn;
    status = napi_create_function(env, NULL, 0, cb, NULL, &fn);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Unable to wrap native function.");
        return NULL;
    }
    status = napi_set_named_property(env, exports, js_name, fn);
    if (status != napi_ok) {
        trigger_exception(env, NULL, "Unable to populate exports.");
        return NULL;
    }
    return exports;
}

void free_threadsafe_context (napi_env env, threadsafe_context_t **threadsafe_context) {
    napi_delete_reference(env, (*threadsafe_context)->my_data_ref);
    napi_delete_reference(env, (*threadsafe_context)->this_ref);
    free (*threadsafe_context);
}

void free_threadsafe_context_hash (napi_env env, threadsafe_context_hash_t **threadsafe_context_hash) {
    assert (threadsafe_context_hash);
    assert (*threadsafe_context_hash);
    if ((*threadsafe_context_hash)->key != NULL)
        free ((*threadsafe_context_hash)->key);

    threadsafe_context_t *threadsafe_context, *threadsafe_context_tmp;
    LL_FOREACH_SAFE ((*threadsafe_context_hash)->list, threadsafe_context, threadsafe_context_tmp) {
        LL_DELETE ((*threadsafe_context_hash)->list, threadsafe_context);
        free_threadsafe_context(env, &threadsafe_context);
    }
    free (*threadsafe_context_hash);
}
