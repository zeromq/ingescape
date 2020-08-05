//
//  callback_igs.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 20/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/callback_igs.h"

ThreadsafeContext *headThreadsafeContexts = NULL;

////////////////////////////////////
//  Observe external stop
//

// Function to callback external stop JS function 
static void cbExternalStop_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    napi_ref ref_myData = (napi_ref) data;
    napi_value argv[1];

    // convert args to napi
    if (ref_myData == NULL) {
        convert_null_to_napi(env, &argv[0]);
    }
    else {
        status = napi_get_reference_value(env, ref_myData, &argv[0]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 1, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined forced stop C function
void externalStopCallback (void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // call our threadsafe function cbExternalStop_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, threadsafeContext->ref_myData, napi_tsfn_nonblocking);
}

// Wrapper for : 
// PUBLIC void igs_observeExternalStop(igs_externalStopCallback cb, void *myData);
napi_value node_igs_observeExternalStop(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate struct
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackExternalStop", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbExternalStop_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }  

    // call igs function
    igs_observeExternalStop(externalStopCallback, threadsafeContext);
    return NULL;
}


////////////////////////////////////
//  Observe mute
//

// Function to callback mute JS function 
static void cbMute_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackMuteJS * argsCallback = (ArgsCallbackMuteJS *) data;
    napi_value argv[2];

    // Convert args to napi
    convert_bool_to_napi(env, argsCallback->isMuted, &argv[0]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[1]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct
    free(argsCallback);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined mute C function
void muteCallback(bool isMuted, void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe JS function
    ArgsCallbackMuteJS * argsCallback =  calloc(1, sizeof(ArgsCallbackMuteJS));
    argsCallback->isMuted = isMuted;
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbMute_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_blocking);
}

// Wrapper for : 
// PUBLIC int igs_observeMute(igs_muteCallback cb, void *myData);
napi_value node_igs_observeMute(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate threadsafe context
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    // Create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackMute", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbMute_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }  

    // call igs function
    igs_observeMute(muteCallback, threadsafeContext);
    return NULL;
}


////////////////////////////////////
//  Observe freeze
//

// Function to callback freeze JS function 
static void cbFreeze_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackFreezeJS * argsCallback = (ArgsCallbackFreezeJS *) data;
    napi_value argv[2];

    // Convert args to napi
    convert_bool_to_napi(env, argsCallback->isPaused, &argv[0]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[1]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct
    free(argsCallback);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined freeze C function
void freezeCallback(bool isPaused, void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe JS function
    ArgsCallbackFreezeJS * argsCallback =  calloc(1, sizeof(ArgsCallbackFreezeJS));
    argsCallback->isPaused = isPaused;
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbFreeze_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_blocking);
}

// Wrapper for : 
// PUBLIC int igs_observeFreeze(igs_freezeCallback cb, void *myData);
napi_value node_igs_observeFreeze(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate struct
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackFreeze", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbFreeze_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }  

    // call igs function
    igs_observeFreeze(freezeCallback, threadsafeContext);
    return NULL;
}


////////////////////////////////////
//  Observe IOPs
//

// Function to callback IOP JS function 
static void cbIOP_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackIopJS * argsCallback = (ArgsCallbackIopJS *) data;
    napi_value argv[5];

    // Convert args to napi
    convert_int_to_napi(env, argsCallback->iopType, &argv[0]);
    convert_string_to_napi(env, argsCallback->name, &argv[1]);
    convert_int_to_napi(env, argsCallback->valueType, &argv[2]);
    convert_value_IOP_into_napi(env, argsCallback->valueType, argsCallback->value, argsCallback->valueSize, &argv[3]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[4]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[4]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct
    free(argsCallback->name);
    free(argsCallback->value);
    free(argsCallback);
    
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined IOP C function
void observeCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe JS function
    ArgsCallbackIopJS * argsCallback =  calloc(1, sizeof(ArgsCallbackIopJS));
    argsCallback->iopType = iopType;
    argsCallback->name = strdup(name);
    argsCallback->valueType = valueType;
    argsCallback->value = calloc(1, valueSize);
    memcpy(argsCallback->value, value, valueSize);
    argsCallback->valueSize = valueSize;
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbIOP_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_blocking);
}

// Wrapper for : 
// PUBLIC int igs_observeInput(const char *name, igs_observeCallback cb, void *myData);
napi_value node_igs_observeInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe context
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackInput", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    } 

    // call igs function
    igs_observeInput(name, observeCallback, threadsafeContext);
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData);
napi_value node_igs_observeOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe function
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackOutput", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    } 

    // call igs function
    igs_observeOutput(name, observeCallback, threadsafeContext);
    free(name);
    return NULL;
}

// Wrapper for : 
// PUBLIC int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData);
napi_value node_igs_observeParameter(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe function
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackParameter", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }

    // call igs function
    igs_observeParameter(name, observeCallback, threadsafeContext);
    free(name);
    return NULL;
}


////////////////////////////////////
//  Observe License
//

// Function to callback License JS function 
static void cbLicense_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackLicense * argsCallback = (ArgsCallbackLicense *) data;
    napi_value argv[2];

    // Convert args to napi
    convert_int_to_napi(env, argsCallback->limit, &argv[0]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[1]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct 
    free(argsCallback);
    
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined licence C function
void licenseCallback (igs_license_limit_t limit, void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

     // Initiate all arguments for our threadsafe JS function
    ArgsCallbackLicense * argsCallback = calloc(1, sizeof(ArgsCallbackLicense)); 
    argsCallback->limit = limit;
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbLicense_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_nonblocking);
}   

// Wrapper for : 
// PUBLIC int igs_observeLicense(igs_licenseCallback cb, void *myData);
napi_value node_igs_observeLicense (napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate threadsafe context
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackLicense", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbLicense_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }

    // Call igs function
    igs_observeLicense(licenseCallback, threadsafeContext);
    return NULL;
}


////////////////////////////////////
//  Observe Calls
//

// Function to callback call JS function 
static void cbCall_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackCall * argsCallback = (ArgsCallbackCall *) data;
    napi_value argv[5];

    // Convert args to napi
    convert_string_to_napi(env, argsCallback->senderAgentName, &argv[0]);
    convert_string_to_napi(env, argsCallback->senderAgentUUID, &argv[1]);
    convert_string_to_napi(env, argsCallback->callName, &argv[2]);    
    getArrayJSFromCallArgumentList(env, argsCallback->firstArgument, &argv[3]); //convert chained list to arraybuffer
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[4]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[4]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct
    free(argsCallback->senderAgentName);
    free(argsCallback->senderAgentUUID);
    free(argsCallback->callName);
    igs_destroyArgumentsList(&(argsCallback->firstArgument));
    free(argsCallback);
    
    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined call C function
void callCallback (const char *senderAgentName, const char *senderAgentUUID,
    const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
    void* myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe function
    ArgsCallbackCall* argsCallback = calloc(1, sizeof(ArgsCallbackCall));
    argsCallback->senderAgentName = strdup(senderAgentName);
    argsCallback->senderAgentUUID = strdup(senderAgentUUID);
    argsCallback->callName = strdup(callName);
    argsCallback->firstArgument = igs_cloneArgumentsList(firstArgument); //clone it to keep it
    argsCallback->nbArgs = nbArgs;
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbCall_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_nonblocking);
}   

// Wrapper for : 
// PUBLIC int igs_initCall(const char *name, igs_callFunction cb, void *myData);
napi_value node_igs_initCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate threadsafe contexte
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackCall", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbCall_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "3rd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }

    // Call igs function
    int res = igs_initCall(name, callCallback, threadsafeContext);
    free(name);

    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}


////////////////////////////////////
//  Observe Monitoring
//

// Function to callback monitor JS function 
static void cbMonitor_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackMonitor * argsCallback = (ArgsCallbackMonitor *) data;
    napi_value argv[4];

    // Convert args to napi
    convert_int_to_napi(env, argsCallback->event, &argv[0]);
    convert_string_to_napi(env, argsCallback->device, &argv[1]);
    convert_string_to_napi(env, argsCallback->ipAddress, &argv[2]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[3]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[3]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct 
    free(argsCallback->device);
    free(argsCallback->ipAddress);
    free(argsCallback);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 4, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined monitor C function
void monitorCallback (igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe JS function
    ArgsCallbackMonitor* argsCallback = calloc(1, sizeof(ArgsCallbackMonitor)); 
    argsCallback->event = event;
    argsCallback->device = strdup(device);
    argsCallback->ipAddress = strdup(ipAddress);
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbMonitor_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_nonblocking);
}   

// Wrapper for
// PUBLIC void igs_monitor(igs_monitorCallback cb, void *myData);
napi_value node_igs_monitor(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate threadsafe context
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackMonitor", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbMonitor_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }

    // call igs function
    igs_monitor(monitorCallback, threadsafeContext);
    return NULL;
}

////////////////////////////////////
//  Observe Agent Events
//

// Function to callback agent event JS function 
static void cbAgentEvent_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    ArgsCallbackAgentEvent * argsCallback = (ArgsCallbackAgentEvent *) data;
    napi_value argv[4];

    // Convert args to napi
    convert_int_to_napi(env, argsCallback->event, &argv[0]);
    convert_string_to_napi(env, argsCallback->uuid, &argv[1]);
    convert_string_to_napi(env, argsCallback->name, &argv[2]);
    if (argsCallback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[3]);
    }
    else {
        status = napi_get_reference_value(env, argsCallback->ref_myData, &argv[3]);
        if (status != napi_ok) {
            triggerException(env, NULL, "N-API : Unable to get reference value.");
        }
    }

    // Free our struct 
    free(argsCallback->uuid);
    free(argsCallback->name);
    free(argsCallback);

    // Since a function call must have a receiver, we use undefined
    napi_value undefined;
    status = napi_get_undefined(env, &undefined);
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to get undefined.");
    }

    // Callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 4, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// Type defined agent event C function
void agentEventCallback (igs_agent_event_t event, const char *uuid, const char *name, void *myData) {
    ThreadsafeContext * threadsafeContext = (ThreadsafeContext *) myData;

    // Initiate all arguments for our threadsafe JS function
    ArgsCallbackAgentEvent* argsCallback = calloc(1, sizeof(ArgsCallbackAgentEvent)); 
    argsCallback->event = event;
    argsCallback->uuid = strdup(uuid);
    argsCallback->name = strdup(name);
    argsCallback->ref_myData = threadsafeContext->ref_myData;

    // Call our threadsafe function cbMonitor_into_js
    napi_call_threadsafe_function(threadsafeContext->threadsafe_func, argsCallback, napi_tsfn_nonblocking);
}   

// Wrapper for
// PUBLIC void igs_observeAgentEvents(igs_agentEventCallback cb, void *myData)
napi_value node_igs_observeAgentEvents(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // Get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate threadsafe context
    ThreadsafeContext * threadsafeContext = calloc(1, sizeof(ThreadsafeContext));
    DL_APPEND(headThreadsafeContexts, threadsafeContext);

    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackAgentEvent", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbAgentEvent_into_js, &(threadsafeContext->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // Create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if ((value_type == napi_null) || (value_type == napi_undefined)) {
        threadsafeContext->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(threadsafeContext->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null or undefined.");
        }
    }

    // Call igs function
    igs_observeAgentEvents(agentEventCallback, threadsafeContext);
    return NULL;
}


////////////////////////////////////
//  Misc
//

// Free allocated memory for callbacks during lifetime of the agent
void free_data_cb() {
    ThreadsafeContext *elt, *tmp;
    DL_FOREACH_SAFE(headThreadsafeContexts, elt, tmp) {
        DL_DELETE(headThreadsafeContexts, elt);
        free(elt);
    }
}

// Allow callback for observe ingescape code 
napi_value init_callback_igs(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_observeExternalStop, "observeExternalStop", exports);
    exports = enable_callback_into_js(env, node_igs_observeMute, "observeMute", exports);
    exports = enable_callback_into_js(env, node_igs_observeFreeze, "observeFreeze", exports);
    exports = enable_callback_into_js(env, node_igs_observeInput, "observeInput", exports);
    exports = enable_callback_into_js(env, node_igs_observeOutput, "observeOutput", exports);
    exports = enable_callback_into_js(env, node_igs_observeParameter, "observeParameter", exports);
    exports = enable_callback_into_js(env, node_igs_observeLicense, "observeLicense", exports);
    exports = enable_callback_into_js(env, node_igs_initCall, "initCall", exports);
    exports = enable_callback_into_js(env, node_igs_monitor, "monitor", exports);
    exports = enable_callback_into_js(env, node_igs_observeAgentEvents, "observeAgentEvents", exports);
    return exports;
}
