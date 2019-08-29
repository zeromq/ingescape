//
//  global.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 16/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/global.h"

char * convert_napi_to_string(napi_env env, napi_value value) {
    napi_status status;
    size_t length_value = 0;
    status = napi_get_value_string_utf8(env, value, NULL, 0, &length_value);
    if (status != napi_ok) {
    	napi_throw_error(env, NULL, "Invalid string was passed as argument");
    }
    char* value_converted = malloc(sizeof(char) * (length_value + 1));
    napi_get_value_string_utf8(env, value, value_converted, length_value + 1, &length_value);
    return value_converted;
}

int convert_napi_to_bool(napi_env env, napi_value value, bool* value_converted) {
    napi_status status;
    status = napi_get_value_bool(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid bool was passed as argument");
        return 0;
    }
    return 1;
}

int convert_napi_to_int(napi_env env, napi_value value, int* value_converted) {
    napi_status status;
    status = napi_get_value_int32(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid int was passed as argument");
        return 0;
    }
    return 1;
}

int convert_napi_to_double(napi_env env, napi_value value, double* value_converted) {
    napi_status status;
    status = napi_get_value_double(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid double was passed as argument");
    	return 0;
    }
    return 1;
}

int convert_napi_to_data(napi_env env, napi_value value, void ** value_converted, size_t* size_value_converted) {
    napi_status status;
    napi_valuetype valueType;
    status = napi_typeof(env, value, &valueType);

    if ((valueType == napi_undefined) || (valueType == napi_null)) {
        *value_converted = NULL;
        *size_value_converted = 0;
    }
    else {
        status = napi_get_arraybuffer_info(env, value, value_converted, size_value_converted);
        if (status != napi_ok) {
            napi_throw_error(env, NULL, "Invalid array buffer was passed as argument");
            return 0;
        }
    }
    return 1;
}

int convert_int_to_napi(napi_env env, int value, napi_value* value_converted) {
    napi_status status;
    status = napi_create_int32(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "N-API : Unable to convert int value into napi_value");
        return 0;
    }
    return 1;
}

int convert_bool_to_napi(napi_env env, bool value, napi_value* value_converted) {
    napi_status status;
    status = napi_get_boolean(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "N-API : Unable to convert boolean value into napi_value");
        return 0;
    }
    return 1;
}

int convert_string_to_napi(napi_env env, const char * value, napi_value* value_converted) {
    napi_status status;
    status = napi_create_string_utf8(env, value, strlen(value), value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "N-API : Unable to convert string value into napi_value");
        return 0;
    }
    return 1;
}

int convert_double_to_napi(napi_env env, double value, napi_value* value_converted) {
    napi_status status;
    status = napi_create_double(env, value, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "N-API : Unable to convert double value into napi_value");
        return 0;
    }
    return 1;
}

int convert_null_to_napi(napi_env env, napi_value* value_converted) {
    napi_status status;
    status = napi_get_null(env, value_converted);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "N-API : Unable to get null value into napi_value");
        return 0;
    }
    return 1;
}

int get_function_arguments(napi_env env, napi_callback_info info, size_t argc, napi_value * argv) {
    napi_status status;
    size_t nb_infos = argc;
    status = napi_get_cb_info(env, info, &nb_infos, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to get function arguments");
        return 0;
    }
    if (nb_infos != argc) {
        napi_throw_error(env, NULL, "Wrong number of arguments pass in function");
        return 0;
    }
    return 1;
}

napi_value enable_callback_into_js(napi_env env, napi_callback cb, const char * js_name, napi_value exports) {
    napi_status status;
    napi_value fn;
    status = napi_create_function(env, NULL, 0, cb, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
        return NULL;
    }
    status = napi_set_named_property(env, exports, js_name, fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
        return NULL;
    }
    return exports;
}

iopType_t get_iop_type_t_from_iop_type_js(iopType_js type_js) {
    switch(type_js) {
        case IGS_NUMBER_JS  :
            return IGS_DOUBLE_T;
        case IGS_STRING_JS  :
            return IGS_STRING_T;
        case IGS_BOOL_JS  :
            return IGS_BOOL_T;
        case IGS_IMPULSION_JS  :
            return IGS_IMPULSION_T;
        case IGS_DATA_JS  :
            return IGS_DATA_T;
        default : 
            return IGS_UNKNOWN_T;
    }
}

iopType_js get_iop_type_js_from_iop_type_t(iopType_t type) {
    switch(type) {
        case IGS_INTEGER_T  :
            return IGS_NUMBER_JS;
        case IGS_DOUBLE_T  :
            return IGS_NUMBER_JS;
        case IGS_STRING_T  :
            return IGS_STRING_JS;
        case IGS_BOOL_T  :
            return IGS_BOOL_JS;
        case IGS_IMPULSION_T  :
            return IGS_IMPULSION_JS;
        case IGS_DATA_T  :
            return IGS_DATA_JS;
        default : 
            return -1;
    }
}