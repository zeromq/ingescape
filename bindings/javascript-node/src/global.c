//
//  global.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 16/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/global.h"

void triggerException(napi_env env, const char * code, const char * message) {
    // Convert error attributes into napi values
    napi_value error_code, error_msg;
    if (code == NULL) {
        error_code = NULL;
    }
    else {
        convert_string_to_napi(env, code, &error_code);
    }
    if (message == NULL) {
        error_msg = NULL;
    }
    else {
        convert_string_to_napi(env, message, &error_msg);
    }

    // Create error object
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
    	triggerException(env, NULL, "Invalid string was passed as argument.");
        return NULL;
    }
    char* value_converted = malloc(sizeof(char) * (length_value + 1));
    napi_get_value_string_utf8(env, value, value_converted, length_value + 1, &length_value);
    return value_converted;
}

int convert_napi_to_bool(napi_env env, napi_value value, bool* value_converted) {
    napi_status status;
    status = napi_get_value_bool(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid bool was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_int(napi_env env, napi_value value, int* value_converted) {
    napi_status status;
    status = napi_get_value_int32(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid int was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_double(napi_env env, napi_value value, double* value_converted) {
    napi_status status;
    status = napi_get_value_double(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid double was passed as argument.");
        return 0;
    }
    return 1;
}

int convert_napi_to_data(napi_env env, napi_value value, void ** value_converted, size_t* size_value_converted) {
    napi_status status;
    napi_valuetype valueType;
    status = napi_typeof(env, value, &valueType);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of element.");
    }

    if ((valueType == napi_undefined) || (valueType == napi_null)) {
        *value_converted = NULL;
        *size_value_converted = 0;
    }
    else {
        status = napi_get_arraybuffer_info(env, value, value_converted, size_value_converted);
        if (status != napi_ok) {
            triggerException(env, NULL, "Invalid array buffer was passed as argument (null and undefined also accepted).");
            return 0;
        }
    }
    return 1;
}


int convert_int_to_napi(napi_env env, int value, napi_value* value_converted) {
    napi_status status;
    status = napi_create_int32(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to convert int value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_bool_to_napi(napi_env env, bool value, napi_value* value_converted) {
    napi_status status;
    status = napi_get_boolean(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to convert boolean value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_string_to_napi(napi_env env, const char * value, napi_value* value_converted) {
    napi_status status;
    if (value == NULL) {
        status = napi_get_null(env, value_converted);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get null value into napi_value.");
            return 0;
        }
    }

    status = napi_create_string_utf8(env, value, strlen(value), value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to convert string value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_double_to_napi(napi_env env, double value, napi_value* value_converted) {
    napi_status status;
    status = napi_create_double(env, value, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to convert double value into napi_value.");
        return 0;
    }
    return 1;
}

int convert_null_to_napi(napi_env env, napi_value* value_converted) {
    napi_status status;
    status = napi_get_null(env, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get null value into napi_value.");
        return 0;
    }
    return 1;
}

void arrayBufferCollected(napi_env env, void * finalize_data, void * finalize_hint) {
    free(finalize_data);
    finalize_data = NULL;
}

int convert_data_to_napi(napi_env env, void * value, size_t size, napi_value* value_converted) {
    napi_status status;

    // Duplicate data value to keep reference to data while array buffer is not garbage collected
    void * copyValue = calloc(1, size);
    memcpy(copyValue, value, size);

    status = napi_create_external_arraybuffer(env, copyValue, size, arrayBufferCollected, NULL, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to create array buffer into napi_value.");
        return 0;
    }
    return 1;
}

int convert_string_list_to_napi_array(napi_env env, char ** list, size_t length, napi_value* value_converted) {
    napi_status status;
    status = napi_create_array_with_length(env, length, value_converted);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to create array.");
        return 0;
    }

    for (size_t i = 0; i < length; i++) {
        napi_value string_conv;
        convert_string_to_napi(env, list[i], &string_conv);
        status = napi_set_element(env, *value_converted, i, string_conv);
        if (status != napi_ok) {
            triggerException(env, NULL, "Unable to write element into array.");
            return 0;
        }
    }
    return 1;
}

int convert_value_IOP_into_napi(napi_env env, iopType_t type, void * value, size_t size, napi_value * value_napi) {
    if (value == NULL) {
        convert_null_to_napi(env, value_napi);
        return 1;
    }

    // convert in good type napi value
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
            triggerException(env, NULL, "Unknown iopType_t.");
            return 0;
    }
    return 1;
}

int get_function_arguments(napi_env env, napi_callback_info info, size_t argc, napi_value * argv) {
    napi_status status;
    size_t nb_infos = argc;
    status = napi_get_cb_info(env, info, &nb_infos, argv, NULL, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to get function arguments.");
        return 0;
    }
    if (nb_infos != argc) {
        triggerException(env, NULL, "Wrong number of arguments pass in function.");
        return 0;
    }
    return 1;
}

napi_value enable_callback_into_js(napi_env env, napi_callback cb, const char * js_name, napi_value exports) {
    napi_status status;
    napi_value fn;
    status = napi_create_function(env, NULL, 0, cb, NULL, &fn);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to wrap native function.");
        return NULL;
    }
    status = napi_set_named_property(env, exports, js_name, fn);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to populate exports.");
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

void getArrayJSFromCallArgumentList(napi_env env, igs_callArgument_t *firstArgument, napi_value *arrayJS) {
    napi_status status;

    // Create array
    status = napi_create_array(env, arrayJS);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to create array.");
    }

    igs_callArgument_t * elt, * tmp;
    igs_callArgument_t * head = firstArgument;
    napi_value argumentCall, nameArg, typeArg, valueArg;
    int i = 0;

    LL_FOREACH_SAFE(head, elt, tmp) {
        // Create argument call JS object 
        status = napi_create_object(env, &argumentCall);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to create object.");
        }

        // Add argument's name & argument's type
        convert_string_to_napi(env, elt->name, &nameArg);
        status = napi_set_named_property(env, argumentCall, "name", nameArg);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set name of argument call.");
        }

        convert_int_to_napi(env, get_iop_type_js_from_iop_type_t(elt->type), &typeArg);
        status = napi_set_named_property(env, argumentCall, "type", typeArg);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set type of argument call.");
        }

        // Add argument's value
        switch (elt->type) {
            case IGS_INTEGER_T :
                convert_int_to_napi(env, elt->i, &valueArg);
                break;
            case IGS_DOUBLE_T  :
                convert_double_to_napi(env, elt->d, &valueArg);
                break;
            case IGS_STRING_T  :
                convert_string_to_napi(env, elt->c, &valueArg);
                break;
            case IGS_BOOL_T  :
                convert_bool_to_napi(env, elt->b, &valueArg);
                break;
            case IGS_IMPULSION_T  :
                triggerException(env, NULL, "Type IMPULSION is not handling by calls.");
                break;
            case IGS_DATA_T  :
                convert_data_to_napi(env, elt->data, elt->size, &valueArg);
                break;
            default : 
                triggerException(env, NULL, "Type not handling in binding.");
        }
        status = napi_set_named_property(env, argumentCall, "value", valueArg);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set value of argument call.");
        }

        // Add object to array
        status = napi_set_element(env, *arrayJS, i, argumentCall);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set element in array.");
        }
        i++;
    }
}

void getCallArgumentListFromArrayJS(napi_env env, napi_value array, igs_callArgument_t **firstArgument) {
    napi_status status;
    napi_valuetype value_type;
    status = napi_typeof(env, array, &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type.");
    }

    *firstArgument = NULL;
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        *firstArgument = NULL;
    }
    else {
        bool isArray;
        status = napi_is_array(env, array, &isArray);
        if (status != napi_ok) {
            triggerException(env, NULL, "Invalid array buffer was passed as argument (null and undefined also accepted).");
        }

        // Rebuild list of igs_callArgument_t
        uint32_t length;
        status = napi_get_array_length(env, array, &length);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get array length.");
        }

        // Translate all the array elements to create list
        uint32_t i = 0;
        napi_value elt;
        napi_valuetype typeElt;
        void *eltC = NULL;
        size_t size;

        while (i < length) {
            status = napi_get_element(env, array, i, &elt);
            if (status != napi_ok) {
                triggerException(env, NULL, "N-API : Unable to get element in array.");
            }
            status = napi_typeof(env, elt, &typeElt);
            if (status != napi_ok) {
                triggerException(env, NULL, "N-API : Unable to get napi value type of element.");
            }

            eltC = NULL;
            switch (typeElt) {
                case napi_number :
                    // convert to double
                    eltC = (double *) malloc(sizeof(double));
                    convert_napi_to_double(env, elt, eltC);
                    igs_addDoubleToArgumentsList(firstArgument, *(double *)eltC);
                    break;
                case napi_boolean :
                    // convert to bool 
                    eltC = (bool *) malloc(sizeof(bool));
                    convert_napi_to_bool(env, elt, eltC);
                    igs_addBoolToArgumentsList(firstArgument, *(bool *)eltC);
                    break;
                case napi_string :
                    // convert to string 
                    eltC = convert_napi_to_string(env, elt);
                    igs_addStringToArgumentsList(firstArgument, eltC);
                    free(eltC);
                    break;
                case napi_undefined : 
                    // convert to data
                    igs_addDataToArgumentsList(firstArgument, NULL, 0);
                case napi_null :
                    // convert to data
                    igs_addDataToArgumentsList(firstArgument, NULL, 0);
                default :
                    //check if it is an array buffer
                    status = napi_get_arraybuffer_info(env, elt, &eltC, &size);
                    if (status == napi_ok) {
                        // convert to data
                        convert_napi_to_data(env, elt, &eltC, &size);
                        igs_addDataToArgumentsList(firstArgument, eltC, size);
                    }
                    else {
                        triggerException(env, NULL, "Wrong type of element in array, accept undefined, null, string, number, boolean and ArrayBuffer objects.");
                    }
            }
            i++;
        }
    } 
}
