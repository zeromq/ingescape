//
//  callback_igs.h
//  wrapperNode
//
//  Created by Chloe Roumieu on 20/05/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include "./global.h"
#include "./uthash/utlist.h"

#ifndef CALLBACK_IGS
#define CALLBACK_IGS

typedef struct ThreadsafeContext {
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct ThreadsafeContext *prev, *next;
} ThreadsafeContext;


typedef struct ArgsCallbackMuteJS {
	bool isMuted;
    napi_ref ref_myData;
} ArgsCallbackMuteJS;

typedef struct ArgsCallbackFreezeJS {
	bool isPaused;
    napi_ref ref_myData;
} ArgsCallbackFreezeJS;

typedef struct ArgsCallbackIopJS {
	iop_t iopType;
	char* name;
	iopType_t valueType;
	void* value;
	size_t valueSize;
    napi_ref ref_myData;
} ArgsCallbackIopJS;

typedef struct ArgsCallbackLicense {
    igs_license_limit_t limit;
    napi_ref ref_myData;
} ArgsCallbackLicense;

typedef struct ArgsCallbackCall {
    char *senderAgentName;
    char *senderAgentUUID;
    char *callName;
    igs_callArgument_t *firstArgument;
    size_t nbArgs;
    napi_ref ref_myData;
} ArgsCallbackCall;

typedef struct ArgsCallbackMonitor {
    igs_monitorEvent_t event;
    char *device;
    char *ipAddress;
    napi_ref ref_myData;
} ArgsCallbackMonitor;
#endif

// Free allocated memory for callbacks during lifetime of the agent
void free_data_cb();

// Allow callback for observe ingescape code 
napi_value init_callback_igs(napi_env env, napi_value exports);