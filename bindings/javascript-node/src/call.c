//
//  call.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 05/09/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/call.h"

// Wrapper for : 
// PUBLIC void igs_addDoubleToArgumentsList(igs_callArgument_t **list, double value);
napi_value node_igs_addNumberToArgumentsList(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    igs_callArgument_t * list = NULL;
 	getCallArgumentListFromArrayJS(env, argv[0], &list);
    double value;
    convert_napi_to_double(env, argv[1], &value);

    // call igs function
    igs_addDoubleToArgumentsList(&list, value);

    // return new array buffer
    napi_value arrayReturn;
	getArrayJSFromCallArgumentList(env, list, &arrayReturn);
	igs_destroyArgumentsList(&list);
    return arrayReturn;
}

// Wrapper for : 
// PUBLIC void igs_addBoolToArgumentsList(igs_callArgument_t **list, bool value);
napi_value node_igs_addBoolToArgumentsList(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    igs_callArgument_t * list = NULL;
 	getCallArgumentListFromArrayJS(env, argv[0], &list);
    bool value;
    convert_napi_to_bool(env, argv[1], &value);

    // call igs function
    igs_addBoolToArgumentsList(&list, value);

    // return new array buffer
    napi_value arrayReturn;
	getArrayJSFromCallArgumentList(env, list, &arrayReturn);
	igs_destroyArgumentsList(&list);
    return arrayReturn;
}

// Wrapper for : 
// PUBLIC void igs_addStringToArgumentsList(igs_callArgument_t **list, const char *value);
napi_value node_igs_addStringToArgumentsList(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    igs_callArgument_t * list = NULL;
 	getCallArgumentListFromArrayJS(env, argv[0], &list);
    char * value = convert_napi_to_string(env, argv[1]);

    // call igs function
    igs_addStringToArgumentsList(&list, value);
    free(value);

    // return new array buffer
    napi_value arrayReturn;
	getArrayJSFromCallArgumentList(env, list, &arrayReturn);
	igs_destroyArgumentsList(&list);
    return arrayReturn;
}

// Wrapper for : 
// PUBLIC void igs_addDataToArgumentsList(igs_callArgument_t **list, void *value, size_t size);
napi_value node_igs_addDataToArgumentsList(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    igs_callArgument_t * list = NULL;
 	getCallArgumentListFromArrayJS(env, argv[0], &list);
 	void * value;
 	size_t size;
    if (convert_napi_to_data(env, argv[1], &value, &size)) {
        // call igs function
        igs_addDataToArgumentsList(&list, value, size);

        // return new array buffer
        napi_value arrayReturn;
        getArrayJSFromCallArgumentList(env, list, &arrayReturn);
        igs_destroyArgumentsList(&list);
        return arrayReturn;
    }
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list);
// igs_callArgument_t * list = NULL;
napi_value node_igs_sendCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * agentNameOrUUID = convert_napi_to_string(env, argv[0]);
    char * callName = convert_napi_to_string(env, argv[1]);
    igs_callArgument_t * list = NULL;
 	getCallArgumentListFromArrayJS(env, argv[2], &list);

    // call igs function
    int res = igs_sendCall(agentNameOrUUID, callName, &list);
    free(agentNameOrUUID);
    free(callName);

   	// convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// initCall is in callback_igs.c

// Wrapper for : 
// PUBLIC int igs_removeCall(const char *name);
napi_value node_igs_removeCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    int res = igs_removeCall(name);
    free(name);

	// convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type);
napi_value node_igs_addArgumentToCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * callName = convert_napi_to_string(env, argv[0]);
    char * argName = convert_napi_to_string(env, argv[1]);
	int type;
    convert_napi_to_int(env, argv[2], &type);

    // call igs function
	int res = igs_addArgumentToCall(callName, argName, get_iop_type_t_from_iop_type_js(type));
    free(callName);
    free(argName);

	// convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_removeArgumentFromCall(const char *callName, const char *argName); //removes first occurence with this name
napi_value node_igs_removeArgumentFromCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * callName = convert_napi_to_string(env, argv[0]);
    char * argName = convert_napi_to_string(env, argv[1]);

    // call igs function
	int res = igs_removeArgumentFromCall(callName, argName);
    free(callName);
    free(argName);
    
	// convert return value into N-API value
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Allow callback for call/reply ingescape code 
napi_value init_call(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_addNumberToArgumentsList, "addNumberToArgumentsList", exports);
    exports = enable_callback_into_js(env, node_igs_addBoolToArgumentsList, "addBoolToArgumentsList", exports);
    exports = enable_callback_into_js(env, node_igs_addStringToArgumentsList, "addStringToArgumentsList", exports);
    exports = enable_callback_into_js(env, node_igs_addDataToArgumentsList, "addDataToArgumentsList", exports);
    exports = enable_callback_into_js(env, node_igs_sendCall, "sendCall", exports);
    exports = enable_callback_into_js(env, node_igs_removeCall, "removeCall", exports);
    exports = enable_callback_into_js(env, node_igs_addArgumentToCall, "addArgumentToCall", exports);
    exports = enable_callback_into_js(env, node_igs_removeArgumentFromCall, "removeArgumentFromCall", exports);
    return exports;
}
