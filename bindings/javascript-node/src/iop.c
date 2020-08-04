//
//  iop.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 26/03/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/iop.h"

// Wrapper for : 
// PUBLIC int igs_readInput(const char *name, void **value, size_t *size);
napi_value node_igs_readInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    void * value = NULL;
    size_t size;

    //call igs function
    igs_readInput(name, &value, &size);

    // convert into good type JavaScript
    napi_value value_napi;
    iopType_t type = igs_getTypeForInput(name);
    convert_value_IOP_into_napi(env, type, value, size, &value_napi);

    free(value);
    free(name);
    return value_napi;
}

// Wrapper for : 
// PUBLIC int igs_readOutput(const char *name, void **value, size_t *size);
napi_value node_igs_readOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    void * value = NULL;
    size_t size;

    //call igs function
    igs_readOutput(name, &value, &size);

    // convert into good type JavaScript
    napi_value value_napi;
    iopType_t type = igs_getTypeForOutput(name);
    convert_value_IOP_into_napi(env, type, value, size, &value_napi);

    free(value);
    free(name);
    return value_napi;
}

// Wrapper for : 
// PUBLIC int igs_readParameter(const char *name, void **value, size_t *size);
napi_value node_igs_readParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    void * value = NULL;
    size_t size;

    //call igs function
    igs_readParameter(name, &value, &size);

    // convert into good type JavaScript
    napi_value value_napi;
    iopType_t type = igs_getTypeForParameter(name);
    convert_value_IOP_into_napi(env, type, value, size, &value_napi);

    free(value);
    free(name);
    return value_napi;
}

// Wrapper for : 
// PUBLIC bool igs_readInputAsBool(const char *name);
napi_value node_igs_readInputAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_readInputAsBool(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC double igs_readInputAsDouble(const char *name);
napi_value node_igs_readInputAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    double res = igs_readInputAsDouble(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_double_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char* igs_readInputAsString(const char *name);
napi_value node_igs_readInputAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    char * res = igs_readInputAsString(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_readInputAsData(const char *name, void **data, size_t *size); 
napi_value node_igs_readInputAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    void * data = NULL;
    size_t size;
    igs_readInputAsData(name, &data, &size); 
    free(name);

    // convert res into napi_value
    napi_value napi_array_buffer;
    convert_data_to_napi(env, data, size, &napi_array_buffer);
    free(data);
    return napi_array_buffer;
}

// Wrapper for : 
// PUBLIC bool igs_readOutputAsBool(const char *name);
napi_value node_igs_readOutputAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_readOutputAsBool(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC double igs_readOutputAsDouble(const char *name);
napi_value node_igs_readOutputAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    double res = igs_readOutputAsDouble(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_double_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char* igs_readOutputAsString(const char *name);
napi_value node_igs_readOutputAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    char * res = igs_readOutputAsString(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_readOutputAsData(const char *name, void **data, size_t *size);
napi_value node_igs_readOutputAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    void * data = NULL;
    size_t size;
    igs_readOutputAsData(name, &data, &size); 
    free(name);

    // convert res into napi_value
    napi_value napi_array_buffer;
    convert_data_to_napi(env, data, size, &napi_array_buffer);
    free(data);
    return napi_array_buffer;
}

// Wrapper for : 
// PUBLIC bool igs_readParameterAsBool(const char *name);
napi_value node_igs_readParameterAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_readParameterAsBool(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC double igs_readParameterAsDouble(const char *name);
napi_value node_igs_readParameterAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    double res = igs_readParameterAsDouble(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_double_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char* igs_readParameterAsString(const char *name);
napi_value node_igs_readParameterAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    char * res = igs_readParameterAsString(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_string_to_napi(env, res, &res_convert);
    free(res);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_readParameterAsData(const char *name, void **data, size_t *size); 
napi_value node_igs_readParameterAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    void * data = NULL;
    size_t size;
    igs_readParameterAsData(name, &data, &size); 
    free(name);

    // convert res into napi_value
    napi_value napi_array_buffer;
    convert_data_to_napi(env, data, size, &napi_array_buffer);
    free(data);
    return napi_array_buffer;
}

// Wrapper for : 
// PUBLIC int igs_writeInputAsBool(const char *name, bool value);
napi_value node_igs_writeInputAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);

    // call igs function
    int res = igs_writeInputAsBool(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeInputAsDouble(const char *name, double value);
napi_value node_igs_writeInputAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);
    
    // call igs function
    int res = igs_writeInputAsDouble(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeInputAsString(const char *name, const char *value);
napi_value node_igs_writeInputAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    char * value = convert_napi_to_string(env, argv[1]);

    // call igs function
    int res = igs_writeInputAsString(name, value); 
    free(name);
    free(value);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeInputAsImpulsion(const char *name);
napi_value node_igs_writeInputAsImpulsion(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_writeInputAsImpulsion(name); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeInputAsData(const char *name, void *value, size_t size);
napi_value node_igs_writeInputAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    size_t length_value;
    void * data;
    convert_napi_to_data(env, argv[1], &data, &length_value);

    // call igs function
    int res = igs_writeInputAsData(name, data, length_value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeOutputAsBool(const char *name, bool value);
napi_value node_igs_writeOutputAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);

    // call igs function
    int res = igs_writeOutputAsBool(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeOutputAsDouble(const char *name, double value);
napi_value node_igs_writeOutputAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);

    // call igs function
    int res = igs_writeOutputAsDouble(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeOutputAsString(const char *name, const char *value);
napi_value node_igs_writeOutputAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    char * value = convert_napi_to_string(env, argv[1]);

    // call igs function
    int res = igs_writeOutputAsString(name, value); 

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    free(name);
    free(value);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeOutputAsImpulsion(const char *name);
napi_value node_igs_writeOutputAsImpulsion(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_writeOutputAsImpulsion(name); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeOutputAsData(const char *name, void *value, size_t size);
napi_value node_igs_writeOutputAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    void * data;
    size_t length_value;
    convert_napi_to_data(env, argv[1], &data, &length_value);

    // call igs function
    int res = igs_writeOutputAsData(name, data, length_value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeParameterAsBool(const char *name, bool value);
napi_value node_igs_writeParameterAsBool(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);

    // call igs function
    int res = igs_writeParameterAsBool(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeParameterAsDouble(const char *name, double value);
napi_value node_igs_writeParameterAsDouble(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    double value;
    convert_napi_to_double(env, argv[1], &value);

    // call igs function
    int res = igs_writeParameterAsDouble(name, value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeParameterAsString(const char *name, const char *value);
napi_value node_igs_writeParameterAsString(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    char * value = convert_napi_to_string(env, argv[1]);

    // call igs function
    int res = igs_writeParameterAsString(name, value); 
    free(name);
    free(value);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_writeParameterAsData(const char *name, void *value, size_t size);
napi_value node_igs_writeParameterAsData(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    void * data;
    size_t length_value;
    convert_napi_to_data(env, argv[1], &data, &length_value);

    // call igs function
    int res = igs_writeParameterAsData(name, data, length_value); 
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC void igs_clearDataForInput(const char *name);
napi_value node_igs_clearDataForInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_clearDataForInput(name); 
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_clearDataForOutput(const char *name);
napi_value node_igs_clearDataForOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_clearDataForOutput(name); 
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC void igs_clearDataForParameter(const char *name);
napi_value node_igs_clearDataForParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_clearDataForParameter(name); 
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_muteOutput(const char *name);
napi_value node_igs_muteOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_muteOutput(name);
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_unmuteOutput(const char *name);
napi_value node_igs_unmuteOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_unmuteOutput(name);
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC bool igs_isOutputMuted(const char *name);
napi_value node_igs_isOutputMuted(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_isOutputMuted(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC iopType_t igs_getTypeForInput(const char *name);
napi_value node_igs_getTypeForInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int type = igs_getTypeForInput(name);
    free(name);

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, type, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC iopType_t igs_getTypeForOutput(const char *name);
napi_value node_igs_getTypeForOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int type = igs_getTypeForOutput(name);
    free(name);

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, type, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC iopType_t igs_getTypeForParameter(const char *name);
napi_value node_igs_getTypeForParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int type = igs_getTypeForParameter(name);
    free(name);

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, type, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_getInputsNumber(void);
napi_value node_igs_getInputsNumber(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_getInputsNumber();

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_getOutputsNumber(void);
napi_value node_igs_getOutputsNumber(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_getOutputsNumber();

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_getParametersNumber(void);
napi_value node_igs_getParametersNumber(napi_env env, napi_callback_info info) {
    // call igs function
    int res = igs_getParametersNumber();

    // convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char** igs_getInputsList(long *nbOfElements);
napi_value node_igs_getInputsList(napi_env env, napi_callback_info info) {
    // call igs function
    size_t nbInputs = 0;
    char** inputs = igs_getInputsList(&nbInputs);

    // convert char** into napi_value
    napi_value arrayInputs;
    convert_string_list_to_napi_array(env, inputs, nbInputs, &arrayInputs);
    //free char ** 
    igs_freeIOPList(&inputs, nbInputs);
    return arrayInputs;
}

// Wrapper for : 
// PUBLIC char** igs_getOutputsList(long *nbOfElements);
napi_value node_igs_getOutputsList(napi_env env, napi_callback_info info) {
    // call igs function
    size_t nbOutputs = 0;
    char** outputs = igs_getOutputsList(&nbOutputs);

    // convert char** into napi_value
    napi_value arrayOutputs;
    convert_string_list_to_napi_array(env, outputs, nbOutputs, &arrayOutputs);
    //free char ** 
    igs_freeIOPList(&outputs, nbOutputs);
    return arrayOutputs;
}

// Wrapper for : 
// PUBLIC char** igs_getParametersList(long *nbOfElements);
napi_value node_igs_getParametersList(napi_env env, napi_callback_info info) {
    // call igs function
    size_t nbParams = 0;
    char** params = igs_getParametersList(&nbParams);

    // convert char** into napi_value
    napi_value arrayParams;
    convert_string_list_to_napi_array(env, params, nbParams, &arrayParams);
    //free char ** 
    igs_freeIOPList(&params, nbParams);
    return arrayParams;
}

// Wrapper for : 
// PUBLIC bool igs_checkInputExistence(const char *name);
napi_value node_igs_checkInputExistence(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_checkInputExistence(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC bool igs_checkOutputExistence(const char *name);
napi_value node_igs_checkOutputExistence(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_checkOutputExistence(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC bool igs_checkParameterExistence(const char *name);
napi_value node_igs_checkParameterExistence(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_checkParameterExistence(name);
    free(name);

    // convert result into napi_value
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Allow callback for iop ingescape code 
napi_value init_iop(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_readInput, "readInput", exports);
    exports = enable_callback_into_js(env, node_igs_readOutput, "readOutput", exports);
    exports = enable_callback_into_js(env, node_igs_readParameter, "readParameter", exports);
    exports = enable_callback_into_js(env, node_igs_readInputAsBool, "readInputAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_readInputAsDouble, "readInputAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_readInputAsString, "readInputAsString", exports);
    exports = enable_callback_into_js(env, node_igs_readInputAsData, "readInputAsData", exports);
    exports = enable_callback_into_js(env, node_igs_readOutputAsBool, "readOutputAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_readOutputAsDouble, "readOutputAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_readOutputAsString, "readOutputAsString", exports);
    exports = enable_callback_into_js(env, node_igs_readOutputAsData, "readOutputAsData", exports);
    exports = enable_callback_into_js(env, node_igs_readParameterAsBool, "readParameterAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_readParameterAsDouble, "readParameterAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_readParameterAsString, "readParameterAsString", exports);
    exports = enable_callback_into_js(env, node_igs_readParameterAsData, "readParameterAsData", exports);
    exports = enable_callback_into_js(env, node_igs_writeInputAsBool, "writeInputAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_writeInputAsDouble, "writeInputAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_writeInputAsString, "writeInputAsString", exports);
    exports = enable_callback_into_js(env, node_igs_writeInputAsImpulsion, "writeInputAsImpulsion", exports);
    exports = enable_callback_into_js(env, node_igs_writeInputAsData, "writeInputAsData", exports);
    exports = enable_callback_into_js(env, node_igs_writeOutputAsBool, "writeOutputAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_writeOutputAsDouble, "writeOutputAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_writeOutputAsString, "writeOutputAsString", exports);
    exports = enable_callback_into_js(env, node_igs_writeOutputAsImpulsion, "writeOutputAsImpulsion", exports);
    exports = enable_callback_into_js(env, node_igs_writeOutputAsData, "writeOutputAsData", exports);
    exports = enable_callback_into_js(env, node_igs_writeParameterAsBool, "writeParameterAsBool", exports);
    exports = enable_callback_into_js(env, node_igs_writeParameterAsDouble, "writeParameterAsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_writeParameterAsString, "writeParameterAsString", exports);
    exports = enable_callback_into_js(env, node_igs_writeParameterAsData, "writeParameterAsData", exports);
    exports = enable_callback_into_js(env, node_igs_clearDataForInput, "clearDataForInput", exports);
    exports = enable_callback_into_js(env, node_igs_clearDataForOutput, "clearDataForOutput", exports);
    exports = enable_callback_into_js(env, node_igs_clearDataForParameter, "clearDataForParameter", exports);
    exports = enable_callback_into_js(env, node_igs_muteOutput, "muteOutput", exports);
    exports = enable_callback_into_js(env, node_igs_unmuteOutput, "unmuteOutput", exports);
    exports = enable_callback_into_js(env, node_igs_isOutputMuted, "isOutputMuted", exports);
    exports = enable_callback_into_js(env, node_igs_getTypeForInput, "getTypeForInput", exports);
    exports = enable_callback_into_js(env, node_igs_getTypeForOutput, "getTypeForOutput", exports);
    exports = enable_callback_into_js(env, node_igs_getTypeForParameter, "getTypeForParameter", exports);
    exports = enable_callback_into_js(env, node_igs_getInputsNumber, "getInputsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_getOutputsNumber, "getOutputsNumber", exports);
    exports = enable_callback_into_js(env, node_igs_getParametersNumber, "getParametersNumber", exports);
    exports = enable_callback_into_js(env, node_igs_getInputsList, "getInputsList", exports);
    exports = enable_callback_into_js(env, node_igs_getOutputsList, "getOutputsList", exports);
    exports = enable_callback_into_js(env, node_igs_getParametersList, "getParametersList", exports);
    exports = enable_callback_into_js(env, node_igs_checkInputExistence, "checkInputExistence", exports);
    exports = enable_callback_into_js(env, node_igs_checkOutputExistence, "checkOutputExistence", exports);
    exports = enable_callback_into_js(env, node_igs_checkParameterExistence, "checkParameterExistence", exports);  
    return exports;
}
