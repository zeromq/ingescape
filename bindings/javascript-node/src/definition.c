//
//  definition.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 23/04/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/definition.h"

// Wrapper for : 
// PUBLIC int igs_loadDefinition (const char* json_str);
napi_value node_igs_loadDefinition(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * json_str = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_loadDefinition(json_str);
    free(json_str);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_loadDefinitionFromPath (const char* file_path);
napi_value node_igs_loadDefinitionFromPath(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * file_path = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_loadDefinitionFromPath(file_path);
    free(file_path);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_clearDefinition(void); 
napi_value node_igs_clearDefinition(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_clearDefinition();

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char* igs_getDefinition(void); 
napi_value node_igs_getDefinition(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getDefinition();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getDefinitionName(void);
napi_value node_igs_getDefinitionName(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getDefinitionName();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getDefinitionDescription(void); 
napi_value node_igs_getDefinitionDescription(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getDefinitionDescription();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char *igs_getDefinitionVersion(void);
napi_value node_igs_getDefinitionVersion(napi_env env, napi_callback_info info) {
    // call igs function
    char * res = igs_getDefinitionVersion();

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setDefinitionName(const char *name);
napi_value node_igs_setDefinitionName(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setDefinitionName(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setDefinitionDescription(const char *description);
napi_value node_igs_setDefinitionDescription(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * desc = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setDefinitionDescription(desc);
    free(desc);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_setDefinitionVersion(const char *version);
napi_value node_igs_setDefinitionVersion(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * version = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_setDefinitionVersion(version);
    free(version); 

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// to convert napi value into C value according to iopType value
void * convertValueWithGoodType(napi_env env, napi_value value, iopType_js type, size_t* size_convert) {
    void * p_value;
    *size_convert = 0;
    switch(type) {
        case IGS_NUMBER_JS  :
            *size_convert = sizeof(double);
            p_value = (double *) malloc(*size_convert);
            convert_napi_to_double(env, value, p_value);
            break;
        case IGS_STRING_JS  :
            p_value = (char *) convert_napi_to_string(env, value);
            *size_convert = (strlen(p_value)+1) * sizeof(char);
            break;
        case IGS_BOOL_JS  :
            *size_convert = sizeof(bool);
            p_value = (bool *) malloc(*size_convert);
            convert_napi_to_bool(env, value, p_value);
            break;
        case IGS_IMPULSION_JS  :
            *size_convert = 1 * sizeof(char);
            p_value = (char*) "";
            break;
        case IGS_DATA_JS  :
            convert_napi_to_data(env, value, &p_value, size_convert);
            break;
        default : 
            *size_convert = 0;
            p_value = NULL;
            triggerException(env, NULL, "Unknow iopType_js.");
    }
    return p_value;
}

// Wrapper for : 
// PUBLIC int igs_createInput(const char *name, iopType_t value_type, void *value, size_t size);
napi_value node_igs_createInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    int type;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void * p_value = convertValueWithGoodType(env, argv[2], type, &size);

    // call igs function
    int res = igs_createInput(name, get_iop_type_t_from_iop_type_js(type), p_value, size);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_createOutput(const char *name, iopType_t type, void *value, size_t size);
napi_value node_igs_createOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    int type;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void * p_value = convertValueWithGoodType(env, argv[2], type, &size);

    // call igs function
    int res = igs_createOutput(name, get_iop_type_t_from_iop_type_js(type), p_value, size);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_createParameter(const char *name, iopType_t type, void *value, size_t size);
napi_value node_igs_createParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    int type = 0;
    convert_napi_to_int(env, argv[1], &type);
    size_t size;
    void * p_value = convertValueWithGoodType(env, argv[2], type, &size);

    // call igs function
    int res = igs_createParameter(name, get_iop_type_t_from_iop_type_js(type), p_value, size);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeInput(const char *name);
napi_value node_igs_removeInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_removeInput(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeOutput(const char *name);
napi_value node_igs_removeOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_removeOutput(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeParameter(const char *name);
napi_value node_igs_removeParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_removeParameter(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Allow callback for definition ingescape code 
napi_value init_definition(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_loadDefinition, "loadDefinition", exports);
    exports = enable_callback_into_js(env, node_igs_loadDefinitionFromPath, "loadDefinitionFromPath", exports);
    exports = enable_callback_into_js(env, node_igs_clearDefinition, "clearDefinition", exports);
    exports = enable_callback_into_js(env, node_igs_getDefinition, "getDefinition", exports);
    exports = enable_callback_into_js(env, node_igs_getDefinitionName, "getDefinitionName", exports);
    exports = enable_callback_into_js(env, node_igs_getDefinitionDescription, "getDefinitionDescription", exports);
    exports = enable_callback_into_js(env, node_igs_getDefinitionVersion, "getDefinitionVersion", exports);
    exports = enable_callback_into_js(env, node_igs_setDefinitionName, "setDefinitionName", exports);
    exports = enable_callback_into_js(env, node_igs_setDefinitionDescription, "setDefinitionDescription", exports);
    exports = enable_callback_into_js(env, node_igs_setDefinitionVersion, "setDefinitionVersion", exports);
    exports = enable_callback_into_js(env, node_igs_createInput, "createInput", exports);
    exports = enable_callback_into_js(env, node_igs_createOutput, "createOutput", exports);
    exports = enable_callback_into_js(env, node_igs_createParameter, "createParameter", exports);
    exports = enable_callback_into_js(env, node_igs_removeInput, "removeInput", exports);
    exports = enable_callback_into_js(env, node_igs_removeOutput, "removeOutput", exports);
    exports = enable_callback_into_js(env, node_igs_removeParameter, "removeParameter", exports);
    return exports;
}