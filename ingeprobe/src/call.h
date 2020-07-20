//
//  call.h
//  ingeprobe
//
//  Created by Stephane Vales on 19/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#ifndef call_h
#define call_h

#include <stdio.h>
#include <stdbool.h>
#include <czmq.h>
#include "uthash/uthash.h"
#include "uthash/utlist.h"

typedef enum {
    IGS_INTEGER_T = 1,
    IGS_DOUBLE_T,
    IGS_STRING_T,
    IGS_BOOL_T,
    IGS_IMPULSION_T,
    IGS_DATA_T
} iopType_t;

typedef struct callArgument{
    char *name;
    iopType_t type;
    struct callArgument *next;
} callArgument_t;


typedef struct call{
    char * name;
    char * description;
    callArgument_t *arguments;
    UT_hash_handle hh;
} call_t;

void parseCallsFromDefinition(const char *definition, call_t **calls);
void freeCalls(call_t **calls);
bool addArgumentsToCallMessage(zmsg_t *msg, call_t *call, const char *arguments);

#endif /* call_h */
