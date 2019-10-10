//
//  callback_igs.c
//  wrapperNode
//
//  Created by Chloe Roumieu on 20/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "../headers/callback_igs.h"

CallbackForcedStopJS *headForcedStopObserved = NULL;

// Function to callback forced stop JS function 
static void cbForcedStop_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackForcedStopJS * callback = (CallbackForcedStopJS *) data;
    napi_value argv[1];

    // convert args to napi
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[0]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[0]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 1, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined forced stop C function
void forcedStopCallback(void *myData) {
    // call threadsafe function
    CallbackForcedStopJS * callback = (CallbackForcedStopJS *) myData;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_blocking);
}

// Wrapper for : 
// PUBLIC void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData);
napi_value node_igs_observeForcedStop(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate struct
    CallbackForcedStopJS * callback = calloc(1, sizeof(CallbackForcedStopJS));
    DL_APPEND(headForcedStopObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackForcedStop", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbForcedStop_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    // create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    }  

    // call igs function
    igs_observeForcedStop(forcedStopCallback, callback);
    return NULL;
}

CallbackMuteJS *headMuteObserved = NULL;

// Function to callback mute JS function 
static void cbMute_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackMuteJS * callback = (CallbackMuteJS *) data;
    napi_value argv[2];

    // convert args to napi
    convert_bool_to_napi(env, callback->isMuted, &argv[0]);
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[1]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined mute C function
void muteCallback(bool isMuted, void *myData) {
    // call threadsafe function
    CallbackMuteJS * callback = (CallbackMuteJS *) myData;
    callback->isMuted = isMuted;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_blocking);
}

// Wrapper for : 
// PUBLIC int igs_observeMute(igs_muteCallback cb, void *myData);
napi_value node_igs_observeMute(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // Initiate struct
    CallbackMuteJS * callback = calloc(1, sizeof(CallbackMuteJS));
    DL_APPEND(headMuteObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackMute", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbMute_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    }  

    // call igs function
    int res = igs_observeMute(muteCallback, callback);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

CallbackFreezeJS *headFreezeObserved = NULL;

// Function to callback freeze JS function 
static void cbFreeze_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackFreezeJS * callback = (CallbackFreezeJS *) data;
    napi_value argv[2];

    //convert args to napi
    convert_bool_to_napi(env, callback->isPaused, &argv[0]);
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[1]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined freeze C function
void freezeCallback(bool isPaused, void *myData) {
    // call threadsafe function
    CallbackFreezeJS * callback = (CallbackFreezeJS *) myData;
    callback->isPaused = isPaused;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_blocking);
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
    CallbackFreezeJS * callback = calloc(1, sizeof(CallbackFreezeJS));
    DL_APPEND(headFreezeObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackFreeze", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbFreeze_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    }  

    // call igs function
    int res = igs_observeFreeze(freezeCallback, callback);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

CallbackIopJS *headIopObserved = NULL;

// convert C value into napi value according to iopType value
napi_value createValueWithGoodType(napi_env env, void * value, size_t size, iopType_t type) {
    napi_value res = NULL;

    switch(type) {
        case IGS_INTEGER_T  :
            convert_int_to_napi(env, *(int *)value, &res);
            break;
        case IGS_DOUBLE_T  :
            convert_double_to_napi(env, *((double *)value), &res);
            break;
        case IGS_STRING_T  :
            convert_string_to_napi(env, (char *) value, &res);
            break;
        case IGS_BOOL_T  :
            convert_bool_to_napi(env, *(bool *)value, &res);
            break;
        case IGS_IMPULSION_T  :
            convert_null_to_napi(env, &res);
            break;
        case IGS_DATA_T  :
            convert_data_to_napi(env, value, size, &res);
            break;
        default : 
            triggerException(env, NULL, "Unknow iopType_t passed as type.");
    }
    return res;
}

// Function to callback IOP JS function 
static void cbIOP_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackIopJS * callback = (CallbackIopJS *) data;
    napi_value argv[5];

    // convert args to napi
    convert_int_to_napi(env, callback->iopType, &argv[0]);
    convert_string_to_napi(env, callback->name, &argv[1]);
    convert_int_to_napi(env, get_iop_type_js_from_iop_type_t(callback->valueType), &argv[2]);
    argv[3] = createValueWithGoodType(env, callback->value, callback->valueSize, callback->valueType);
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[4]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[4]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined IOP C function
void observeCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData) {
    // call threadsafe function
    CallbackIopJS * callback = (CallbackIopJS *) myData;
    callback->iopType = iopType;
    callback->name = strndup(name, strlen(name));
    callback->valueType = valueType;
    callback->value = value;
    callback->valueSize = valueSize;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_nonblocking);
}

// Wrapper for : 
// PUBLIC int igs_observeInput(const char *name, igs_observeCallback cb, void *myData);
napi_value node_igs_observeInput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate struct
    CallbackIopJS * callback = calloc(1, sizeof(CallbackIopJS));
    DL_APPEND(headIopObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackInput", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    } 

    // call igs function
    int res = igs_observeInput(name, observeCallback, callback);

    free(name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Wrapper for : 
// PUBLIC int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData);
napi_value node_igs_observeOutput(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate struct
    CallbackIopJS * callback = calloc(1, sizeof(CallbackIopJS));
    DL_APPEND(headIopObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackOutput", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    } 

    // call igs function
    int res = igs_observeOutput(name, observeCallback, callback);
    free(name);

    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
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

    // Initiate struct
    CallbackIopJS * callback = calloc(1, sizeof(CallbackIopJS));
    DL_APPEND(headIopObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackParameter", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbIOP_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    }

    // call igs function
    int res = igs_observeParameter(name, observeCallback, callback);
    free(name);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

CallbackLicense *headLicenseObserved = NULL;

// Function to callback License JS function 
static void cbLicense_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackLicense * callback = (CallbackLicense *) data;
    napi_value argv[2];

    // convert args to napi
    convert_int_to_napi(env, callback->limit, &argv[0]);
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[1]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[1]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 2, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined licence C function
void licenseCallback (igs_license_limit_t limit, void *myData) {
    // call threadsafe function
    CallbackLicense * callback = (CallbackLicense *) myData;
    callback->limit = limit;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_nonblocking);
}   

// Wrapper for : 
// PUBLIC int igs_observeLicense(igs_licenseCallback cb, void *myData);
napi_value node_igs_observeLicense (napi_env env, napi_callback_info info) {
    size_t nb_arguments = 2;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    // Initiate struct
    CallbackLicense * callback = calloc(1, sizeof(CallbackLicense));
    DL_APPEND(headLicenseObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackLicense", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[0], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbLicense_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[1], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[1], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "2nd argument must be a JavaScript Object or an Array or null.");
        }
    }

    // call igs function
    int res = igs_observeLicense(licenseCallback, callback);
    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

CallbackCall *headCallObserved = NULL;

static void cbCall_into_js(napi_env env, napi_value js_callback, void* ctx, void* data) {
    napi_status status;
    CallbackCall * callback = (CallbackCall *) data;
    napi_value argv[5];

    // convert args to napi
    convert_string_to_napi(env, callback->senderAgentName, &argv[0]);
    convert_string_to_napi(env, callback->senderAgentUUID, &argv[1]);
    convert_string_to_napi(env, callback->callName, &argv[2]);

    //convert chained list to arraybuffer
    getArrayJSFromCallArgumentList(env, igs_cloneArgumentsList(callback->firstArgument), &argv[3]);
    //get reference data from JS
    if (callback->ref_myData == NULL) {
        convert_null_to_napi(env, &argv[4]);
    }
    else {
        status = napi_get_reference_value(env, callback->ref_myData, &argv[4]);
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

    // callback into JavaScript
    status = napi_call_function(env, undefined, js_callback, 5, argv, NULL);
    if (status != napi_ok) {
        triggerException(env, NULL, "Unable to call javascript function.");
    }
}

// type defined call C function
void callCallback (const char *senderAgentName, const char *senderAgentUUID,
    const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
    void* myData) {
    // call threadsafe function
    CallbackCall * callback = (CallbackCall *) myData;
    callback->senderAgentName = strndup(senderAgentName, strlen(senderAgentName));
    callback->senderAgentUUID = strndup(senderAgentUUID, strlen(senderAgentUUID));
    callback->callName = strndup(callName, strlen(callName));
    callback->firstArgument = igs_cloneArgumentsList(firstArgument); //clone it to keep it
    callback->nbArgs = nbArgs;
    napi_call_threadsafe_function(callback->threadsafe_func, callback, napi_tsfn_nonblocking);
}   

napi_value node_igs_initCall(napi_env env, napi_callback_info info) {
    size_t nb_arguments = 3;
    napi_status status; //to check status of node_api
    napi_value argv[nb_arguments];

    // get infos pass in argument
    get_function_arguments(env, info, nb_arguments, argv);

    // convert infos into C types
    char * name = convert_napi_to_string(env, argv[0]);

    // Initiate struct
    CallbackCall * callback = calloc(1, sizeof(CallbackCall));
    DL_APPEND(headCallObserved, callback);

    //create threadsafe function
    napi_value async_name;
    status = napi_create_string_utf8(env, "Ingescape/CallbackCall", NAPI_AUTO_LENGTH, &async_name);
    if (status != napi_ok) {
        triggerException(env, NULL, "Invalid name for async_name napi_value.");
    }
    status = napi_create_threadsafe_function(env, argv[1], NULL, async_name, 0, 1, NULL, NULL, NULL, 
    cbCall_into_js, &(callback->threadsafe_func));
    if (status != napi_ok) {
        triggerException(env, NULL, "Impossible to create threadsafe function.");
    }

    //create reference for arguments callback if not null
    napi_valuetype value_type;
    status = napi_typeof(env, argv[2], &value_type);
    if (status != napi_ok) {
        triggerException(env, NULL, "N-API : Unable to get napi value type of 2nd argument.");
    }
    if (value_type == napi_null) {
        callback->ref_myData = NULL;
    }
    else {
        status = napi_create_reference(env, argv[2], 1, &(callback->ref_myData));
        if (status != napi_ok) {
            triggerException(env, NULL, "3rd argument must be a JavaScript Object or an Array or null.");
        }
    }

    // call igs function
    int res = igs_initCall(name, callCallback, callback);
    free(name);

    napi_value res_convert;
    convert_int_to_napi(env, res, &res_convert);
    return res_convert;
}

// Free allocated memory for callbacks during lifetime of the agent
void free_data_cb() {
    CallbackForcedStopJS *eltFS, *tmpFS;
    DL_FOREACH_SAFE(headForcedStopObserved, eltFS, tmpFS) {
        DL_DELETE(headForcedStopObserved, eltFS);
    }

    CallbackMuteJS *eltMute, *tmpMute;
    DL_FOREACH_SAFE(headMuteObserved, eltMute, tmpMute) {
        DL_DELETE(headMuteObserved, eltMute);
    }

    CallbackFreezeJS *eltFreeze, *tmpFreeze;
    DL_FOREACH_SAFE(headFreezeObserved, eltFreeze, tmpFreeze) {
        DL_DELETE(headFreezeObserved, eltFreeze);
    }

    CallbackIopJS *elt, *tmp;
    DL_FOREACH_SAFE(headIopObserved, elt, tmp) {
        DL_DELETE(headIopObserved, elt);
    }

    CallbackLicense *eltLicense, *tmpLicense;
    DL_FOREACH_SAFE(headLicenseObserved, eltLicense, tmpLicense) {
        DL_DELETE(headLicenseObserved, eltLicense);
    }

    CallbackCall *eltCall, *tmpCall;
    DL_FOREACH_SAFE(headCallObserved, eltCall, tmpCall) {
        DL_DELETE(headCallObserved, eltCall);
    }
}

// Allow callback for observe ingescape code 
napi_value init_callback_igs(napi_env env, napi_value exports) {
    exports = enable_callback_into_js(env, node_igs_observeForcedStop, "observeForcedStop", exports);
    exports = enable_callback_into_js(env, node_igs_observeMute, "observeMute", exports);
    exports = enable_callback_into_js(env, node_igs_observeFreeze, "observeFreeze", exports);
    exports = enable_callback_into_js(env, node_igs_observeInput, "observeInput", exports);
    exports = enable_callback_into_js(env, node_igs_observeOutput, "observeOutput", exports);
    exports = enable_callback_into_js(env, node_igs_observeParameter, "observeParameter", exports);
    exports = enable_callback_into_js(env, node_igs_observeLicense, "observeLicense", exports);
    exports = enable_callback_into_js(env, node_igs_initCall, "initCall", exports);
    return exports;
}