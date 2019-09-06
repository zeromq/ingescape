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
typedef struct CallbackForcedStopJS {
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackForcedStopJS *prev, *next;
} CallbackForcedStopJS;

typedef struct CallbackMuteJS {
	bool isMuted;
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackMuteJS *prev, *next;
} CallbackMuteJS;

typedef struct CallbackFreezeJS {
	bool isPaused;
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackFreezeJS *prev, *next;
} CallbackFreezeJS;

typedef struct CallbackIopJS {
	iop_t iopType;
	char* name;
	iopType_t valueType;
	void* value;
	size_t valueSize;
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackIopJS *prev, *next;
} CallbackIopJS;

typedef struct CallbackLicense {
    igs_license_limit_t limit;
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackLicense *prev, *next;
} CallbackLicense;

typedef struct CallbackCall {
    char *senderAgentName;
    char *senderAgentUUID;
    char *callName;
    igs_callArgument_t *firstArgument;
    size_t nbArgs;
    
    napi_threadsafe_function threadsafe_func;
    napi_ref ref_myData;
    struct CallbackCall *prev, *next;
} CallbackCall;
#endif

// Free allocated memory for callbacks during lifetime of the agent
void free_data_cb();

// Allow callback for observe ingescape code 
napi_value init_callback_igs(napi_env env, napi_value exports);