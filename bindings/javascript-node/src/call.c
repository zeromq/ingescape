//
//  call.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 05/09/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/call.h"

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
	int res = igs_addArgumentToCall(callName, argName, type);
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

// Wrapper for : 
// PUBLIC size_t igs_getNumberOfCalls(void);
napi_value node_igs_getNumberOfCalls(napi_env env, napi_callback_info info) { 
    // call igs function
    size_t res = igs_getNumberOfCalls();

    // convert result into napi_value & return
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC bool igs_checkCallExistence(const char *name);
napi_value node_igs_checkCallExistence(napi_env env, napi_callback_info info) { 
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    bool res = igs_checkCallExistence(name);
    free(name);

    // convert result into napi_value & return
    napi_value res_convert;
    convert_bool_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC char** igs_getCallsList(size_t *nbOfElements); //returned char** shall be freed by caller
napi_value node_igs_getCallsList(napi_env env, napi_callback_info info) { 
    size_t nbElement = 0;

    // call igs function
    char ** callsList = igs_getCallsList(&nbElement);

    // convert char** into napi_value
    napi_value arrayCalls;
    convert_string_list_to_napi_array(env, callsList, nbElement, &arrayCalls);
    //free char ** 
    igs_freeCallsList(&callsList, nbElement);
    return arrayCalls;
}

// Wrapper for : 
// PUBLIC igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName);
napi_value node_igs_getArgumentsForCall(napi_env env, napi_callback_info info) { 
    napi_status status;
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // call igs function
    igs_callArgument_t * head = igs_getFirstArgumentForCall(name);
    free(name);

    //Create array of argument's name
    napi_value arrayJS;
    status = napi_create_array(env, &arrayJS);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to create array.");
    }

    igs_callArgument_t * elt, * tmp;
    int i = 0;
    napi_value nameArg, typeArg, argumentCall;
    
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

        convert_int_to_napi(env, elt->type, &typeArg);
        status = napi_set_named_property(env, argumentCall, "type", typeArg);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set type of argument call.");
        }

        status = napi_set_element(env, arrayJS, i, argumentCall);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to set argument call in array.");
        }
        i++;
    }
    return arrayJS;
}

// Wrapper for : 
// PUBLIC size_t igs_getNumberOfArgumentsForCall(const char *callName);
napi_value node_igs_getNumberOfArgumentsForCall(napi_env env, napi_callback_info info) { 
    size_t nb_arguments = 1;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);
    
    // call igs function
    size_t nbArg = igs_getNumberOfArgumentsForCall(name);
    free(name);

    // convert nbArg into napi_value & return
    napi_value res_convert;
    convert_int_to_napi(env, nbArg, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC bool igs_checkCallArgumentExistence(const char *callName, const char *argName);
napi_value node_igs_checkCallArgumentExistence(napi_env env, napi_callback_info info) { 
    size_t nb_arguments = 2;
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * callName = convert_napi_to_string(env, argv[0]);
    char * argName = convert_napi_to_string(env, argv[1]);

    // call igs function
    bool exist = igs_checkCallArgumentExistence(callName, argName);
    free(callName);
    free(argName);

    // convert result into napi_value & return
    napi_value res_convert;
    convert_bool_to_napi(env, exist, &res_convert);
    return res_convert;
}

// Allow callback for calls ingescape code 
napi_value init_call(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_sendCall, "sendCall", exports);
    exports = enable_callback_into_js(env, node_igs_removeCall, "removeCall", exports);
    exports = enable_callback_into_js(env, node_igs_addArgumentToCall, "addArgumentToCall", exports);
    exports = enable_callback_into_js(env, node_igs_removeArgumentFromCall, "removeArgumentFromCall", exports);
    exports = enable_callback_into_js(env, node_igs_getNumberOfCalls, "getNumberOfCalls", exports);
    exports = enable_callback_into_js(env, node_igs_checkCallExistence, "checkCallExistence", exports);
    exports = enable_callback_into_js(env, node_igs_getCallsList, "getCallsList", exports);
    exports = enable_callback_into_js(env, node_igs_getArgumentsForCall, "getArgumentsForCall", exports);
    exports = enable_callback_into_js(env, node_igs_getNumberOfArgumentsForCall, "getNumberOfArgumentsForCall", exports);
    exports = enable_callback_into_js(env, node_igs_checkCallArgumentExistence, "checkCallArgumentExistence", exports);
    return exports;
}
