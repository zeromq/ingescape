//
//  model.c
//
//  Created by Patxi Berard & Stephane Vales
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <czmq.h>
#include "ingescape_private.h"
#include "uthash/utlist.h"

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
#include <pthread.h>
#endif
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *model_readWriteMutex = NULL;
#else
#define W_OK 02
pthread_mutex_t model_readWriteMutex = NULL;
#endif

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void model_readWriteLock(void)   {
#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
    if (model_readWriteMutex == NULL){
        model_readWriteMutex = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(model_readWriteMutex, NULL) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (model_readWriteMutex == NULL){
        if (pthread_mutex_init(&model_readWriteMutex) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#endif
    pthread_mutex_lock(model_readWriteMutex);
}

void model_readWriteUnlock(void) {
    if (model_readWriteMutex != NULL){
        pthread_mutex_unlock(model_readWriteMutex);
    }else{
        igs_fatal("mutex was NULL");
        assert(false);
    }
}

void model_runObserveCallbacksForIOP(igs_agent_t *agent, igs_iop_t *iop, void *value, size_t valueSize){
    igs_observe_callback_t *cb;
    DL_FOREACH(iop->callbacks, cb){
        cb->callback_ptr(agent, iop->type, iop->name, iop->value_type, value, valueSize, cb->data);
    }
}

char* model_intToString(const int value){
    int length = snprintf( NULL, 0, "%d", value);
    if(length == 0){
        return NULL;
    }
    char* str = calloc(1,length+1);
    snprintf( str, length + 1, "%d", value);
    return str;
}

char* model_doubleToString(const double value){
    int length = snprintf( NULL, 0, "%lf", value);
    if(length == 0){
        return NULL;
    }
    char* str = calloc(1,length+1);
    snprintf( str, length + 1, "%lf", value);
    return str;
}

static int model_observe(igs_agent_t *agent, const char* name, iop_t iopType, igsAgent_observeCallback cb, void* myData){

    //find the iop
    igs_iop_t *iop = model_findIopByName(agent, (char*) name, iopType);

    // Check if the input has been returned.
    if(iop == NULL){
        const char *t = NULL;
        switch (iopType) {
            case IGS_INPUT_T:
                t = "input";
                break;
            case IGS_OUTPUT_T:
                t = "output";
                break;
            case IGS_PARAMETER_T:
                t = "parameter";
                break;
                
            default:
                break;
        }
        igsAgent_error(agent, "Cannot find %s %s", t, name);
        return 0;
    }

    //callback not defined
    if(cb == NULL) {
        igsAgent_error(agent, "Callback cannot be NULL (called for %s)", name);
        return 0;
    }

    igs_observe_callback_t *new_callback = malloc(sizeof(igs_observe_callback_t));
    new_callback->callback_ptr = cb;
    new_callback->data = myData;
    DL_APPEND(iop->callbacks, new_callback);

    //igsAgent_debug(agent, "observe iop with name %s and type %d\n", name, iopType);

    return 1;
}

iopType_t model_getTypeForIOP(igs_agent_t *agent, const char *name, iop_t type){
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Name cannot be NULL or empty");
        return 0;
    }
    if(agent->definition == NULL){
        igsAgent_error(agent, "Definition is NULL");
        return 0;
    }
    
    igs_iop_t *iop = NULL;
    if (type == IGS_INPUT_T){
        HASH_FIND_STR(agent->definition->inputs_table, name, iop);
        if(iop == NULL){
            igsAgent_error(agent, "Input %s cannot be found", name);
            return 0;
        }
    } else if (type == IGS_OUTPUT_T){
        HASH_FIND_STR(agent->definition->outputs_table, name, iop);
        if(iop == NULL){
            igsAgent_error(agent, "Output %s cannot be found", name);
            return 0;
        }
    } else if (type == IGS_PARAMETER_T){
        HASH_FIND_STR(agent->definition->params_table, name, iop);
        if(iop == NULL){
            igsAgent_error(agent, "Parameter %s cannot be found", name);
            return 0;
        }
    }else{
        igsAgent_error(agent, "Unknown IOP type %d", type);
        return 0;
    }
    
    return iop->value_type;
}

igs_iop_t *model_findInputByName(igs_agent_t *agent, const char *name){
    igs_iop_t *found = NULL;
    if(name != NULL && agent->definition != NULL){
        HASH_FIND_STR( agent->definition->inputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igsAgent_error(agent, "Input name cannot be NULL or empty");
        }else{
            igsAgent_error(agent, "Definition is NULL");
        }
    }
    return found;
}

igs_iop_t *model_findOutputByName(igs_agent_t *agent, const char *name){
    igs_iop_t *found = NULL;
    if(name != NULL && agent->definition != NULL){
        HASH_FIND_STR( agent->definition->outputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igsAgent_error(agent, "Output name cannot be NULL or empty");
        }else{
            igsAgent_error(agent, "Definition is NULL");
        }
    }
    return found;
}

igs_iop_t *model_findParameterByName(igs_agent_t *agent, const char *name){
    igs_iop_t *found = NULL;
    if(name != NULL && agent->definition != NULL){
        HASH_FIND_STR( agent->definition->params_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        }else{
            igsAgent_error(agent, "Definition is NULL");
        }
    }
    return found;
}

void* model_getValueFor(igs_agent_t *agent, const char *name, iop_t type){
    igs_iop_t *iop = model_findIopByName(agent, (char*) name,type);
    if(iop == NULL){
        igsAgent_error(agent, "%s not found", name);
        return NULL;
    }
    switch (iop->value_type) {
        case IGS_INTEGER_T:
            return &iop->value.i;
            break;
        case IGS_DOUBLE_T:
            return &iop->value.d;
            break;
        case IGS_BOOL_T:
            return &iop->value.b;
            break;
        case IGS_STRING_T:
            return iop->value.s;
            break;
        case IGS_IMPULSION_T:
            return NULL;
            break;
        case IGS_DATA_T:
            return iop->value.data;
            break;
        default:
            igsAgent_error(agent, "Unknown value type for %s", name);
            break;
    }
    return NULL;
}

igs_result_t igsAgent_readIOP(igs_agent_t *agent, const char *name, iop_t type, void **value, size_t *size){
    igs_iop_t *iop = model_findIopByName(agent, (char*) name, type);
    if(iop == NULL){
        igsAgent_error(agent, "%s not found", name);
        return IGS_FAILURE;
    }
    if (iop->value_type == IGS_IMPULSION_T
        || (iop->value_type == IGS_STRING_T && iop->value.s == NULL)
        || (iop->value_type == IGS_DATA_T && iop->value.data == NULL)){
        *value = NULL;
        *size = 0;
    }else{
        *value = calloc(1, iop->valueSize);
        memcpy(*value, model_getValueFor(agent, name, type), iop->valueSize);
        *size = iop->valueSize;
    }
    return IGS_SUCCESS;
}

bool model_readIopAsBool (igs_agent_t *agent, const char *name, iop_t type){
    bool res = false;
    igs_iop_t *iop = model_findIopByName(agent, name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                res = iop->value.b;
                return res;
                break;
                
            case IGS_INTEGER_T:
                igsAgent_warn(agent, "Implicit conversion from int to bool for %s", name);
                res = (iop->value.i == 0)?false:true;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igsAgent_warn(agent, "Implicit conversion from double to bool for %s", name);
                res = (iop->value.d >= 0 && iop->value.d <= 0)?false:true;
                return res;
                break;
                
            case IGS_STRING_T:
                if (strcmp(iop->value.s, "true") == 0){
                    igsAgent_warn(agent, "Implicit conversion from string to bool for %s", name);
                    return true;
                }
                else if (strcmp(iop->value.s, "false") == 0){
                    igsAgent_warn(agent, "Implicit conversion from string to bool for %s", name);
                    return false;
                }else{
                    igsAgent_warn(agent, "Implicit conversion from double to bool for %s (string value is %s and false was returned)", name, iop->value.s);
                    return false;
                }
                break;
                
            default:
                igsAgent_error(agent, "No implicit conversion possible for %s (false was returned)", name);
                return false;
                break;
        }
    }else{
        igsAgent_error(agent, "%s not found", name);
        return false;
    }
}

int model_readIopAsInt (igs_agent_t *agent, const char *name, iop_t type){
    int res = 0;
    igs_iop_t *iop = model_findIopByName(agent, name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                igsAgent_warn(agent, "Implicit conversion from bool to int for %s", name);
                res =  (iop->value.b)?1:0;
                return res;
                break;
                
            case IGS_INTEGER_T:
                res = iop->value.i;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igsAgent_warn(agent, "Implicit conversion from double to int for %s", name);
                if(iop->value.d < 0) {
                    res = (int) (iop->value.d - 0.5);
                    return res;
                }else {
                    res = (int) (iop->value.d + 0.5);
                    return res;
                }
                break;
                
            case IGS_STRING_T:
                igsAgent_warn(agent, "Implicit conversion from string %s to int for %s", iop->value.s, name);
                res = atoi(iop->value.s);
                return res;
                break;
                
            default:
                igsAgent_error(agent, "No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        igsAgent_error(agent, "%s not found", name);
        return 0;
    }
}

double model_readIopAsDouble (igs_agent_t *agent, const char *name, iop_t type){
    double res = 0;
    igs_iop_t *iop = model_findIopByName(agent, name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                igsAgent_warn(agent, "Implicit conversion from bool to double for %s", name);
                res = (iop->value.b)?1:0;
                return res;
                break;
                
            case IGS_INTEGER_T:
                igsAgent_warn(agent, "Implicit conversion from int to double for %s", name);
                res = iop->value.i;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                res = iop->value.d;
                return res;
                break;
                
            case IGS_STRING_T:
                igsAgent_warn(agent, "Implicit conversion from string %s to double for %s", iop->value.s, name);
                res = atof(iop->value.s);
                return res;
                break;
                
            default:
                igsAgent_error(agent, "No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        igsAgent_error(agent, "%s not found", name);
        return 0;
    }
}

char *model_readIopAsString (igs_agent_t *agent, const char *name, iop_t type){
    char *res = NULL;
    igs_iop_t *iop = model_findIopByName(agent, name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_STRING_T:
                res = strdup(iop->value.s);
                return res;
                break;
                
            case IGS_BOOL_T:
                igsAgent_warn(agent, "Implicit conversion from bool to string for %s", name);
                res = iop->value.b ? strdup("true") : strdup("false");
                return res;
                break;
                
            case IGS_INTEGER_T:
                igsAgent_warn(agent, "Implicit conversion from int to string for %s", name);
                res = model_intToString(iop->value.i);
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igsAgent_warn(agent, "Implicit conversion from double to string for %s", name);
                res = model_doubleToString(iop->value.d);
                return res;
                break;
                
            default:
                igsAgent_error(agent, "No implicit conversion possible for %s (NULL was returned)", name);
                return NULL;
                break;
        }
    }else{
        igsAgent_error(agent, "%s not found", name);
        return NULL;
    }
}

igs_result_t model_readIopAsData (igs_agent_t *agent, const char *name, iop_t type, void **value, size_t *size){
    assert(agent);
    assert(value);
    assert(size);
    igs_iop_t *iop = model_findIopByName(agent, (char*) name, type);
    bool failed = false;
    if(iop == NULL){
        igsAgent_error(agent, "%s not found", name);
        failed = true;
    }else if(iop->value_type != IGS_DATA_T){
        igsAgent_error(agent, "No implicit conversion possible for %s (NULL was returned)", name);
        failed = true;
    }
    *value = NULL;
    *size = 0;
    if (failed){
        return IGS_FAILURE;
    }else if(iop->value.data == NULL && iop->valueSize == 0){
        return IGS_SUCCESS;
    }
    *size = iop->valueSize;
    *value = calloc(1, iop->valueSize);
    memcpy(*value, model_getValueFor(agent, name, type), *size);
    return IGS_SUCCESS;
}

bool model_checkIOPExistence(igs_agent_t *agent, const char *name, igs_iop_t *hash){
    igs_iop_t *iop = NULL;
    if(agent->definition == NULL){
        igsAgent_error(agent, "Definition is NULL");
        return false;
    }
    HASH_FIND_STR(hash, name, iop);
    if(iop == NULL){
        return false;
    }
    return true;
}

char **model_getIopList(igs_agent_t *agent, size_t *nbOfElements, iop_t type){
    if(agent->definition == NULL){
        igsAgent_warn(agent, "Definition is NULL");
        *nbOfElements = 0;
        return NULL;
    }
    igs_iop_t *hash = NULL;
    switch (type) {
        case IGS_INPUT_T:
            hash = agent->definition->inputs_table;
            break;
        case IGS_OUTPUT_T:
            hash = agent->definition->outputs_table;
            break;
        case IGS_PARAMETER_T:
            hash = agent->definition->params_table;
            break;
            
        default:
            break;
    }
    long N = (*nbOfElements) = HASH_COUNT(hash);
    if(N < 1)
        return NULL;
    
    char ** list = (char**) malloc( N * sizeof(char*));
    igs_iop_t *current_iop;
    int index = 0;
    for(current_iop = hash; current_iop != NULL; current_iop = current_iop->hh.next) {
        list[index] = strdup(current_iop->name);
        index++;
    }
    return list;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
char* model_getIOPValueAsString (igs_iop_t* iop){
    char *str_value = NULL;
    if(iop != NULL){
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255,"%i",iop->value.i);
                break;
            case IGS_DOUBLE_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255,"%lf",iop->value.d);
                break;
            case IGS_BOOL_T:
                str_value = calloc(1, 8);
                if(iop->value.b == true){
                    snprintf(str_value, 8, "%s","true");
                } else {
                    snprintf(str_value, 8, "%s","false");
                }
                break;
            case IGS_STRING_T:
                str_value = calloc(1, strlen(iop->value.s)+1);
                snprintf(str_value, strlen(iop->value.s)+1, "%s",iop->value.s);
                break;
            case IGS_IMPULSION_T:
                break;
            case IGS_DATA_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255, "%s", (char*) iop->value.data);
                break;
            default:
                break;
        }
    }
    return str_value;
}

const igs_iop_t* model_writeIOP (igs_agent_t *agent, const char *iopName, iop_t iopType, iopType_t valType, void* value, size_t size){
    igs_iop_t *iop = model_findIopByName(agent, (char*) iopName, iopType);
    if(iop == NULL){
        igsAgent_error(agent, "%s not found for writing", iopName);
        return NULL;
    }
    int ret = 1;
    void *outValue = NULL;
    size_t outSize = 0;
    
    model_readWriteLock();
    //TODO optimize if value is NULL
    switch (valType) {
        case IGS_INTEGER_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    outSize = iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:*(int*)(value);
                    outValue = &(iop->value.i);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(int*)(value);
                    outValue = &(iop->value.d);
                    igsAgent_debug(agent, "set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:((*(int*)(value))?true:false);
                    outValue = &(iop->value.b);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    if (value == NULL){
                        iop->value.s = strdup("");
                    }else{
                        char buf[32] = "";
                        snprintf(buf, 31, "%d", (value == NULL)?0:*(int*)(value));
                        iop->value.s = strdup(buf);
                    }
                    outSize = iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    outValue = iop->value.s;
                    igsAgent_debug(agent, "set %s to %s (length: %zu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(int));
                    memcpy(iop->value.data, value, sizeof(int));
                    outSize = iop->valueSize = sizeof(int);
                    outValue = iop->value.data;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case IGS_DOUBLE_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    outSize = iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:(int)(*(double*)(value));
                    outValue = &(iop->value.i);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:(double)(*(double*)(value));
                    outValue = &(iop->value.d);
                    igsAgent_debug(agent, "set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:(((int)(*(double*)(value)))?true:false);
                    outValue = &(iop->value.b);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    if (value == NULL){
                        iop->value.s = strdup("");
                    }else{
                        char buf[64] = "";
                        snprintf(buf, 63, "%lf", (value == NULL)?0:*(double*)(value));
                        iop->value.s = strdup(buf);
                    }
                    outSize = iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    outValue = iop->value.s;
                    igsAgent_debug(agent, "set %s to %s (length: %zu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(double));
                    memcpy(iop->value.data, value, sizeof(double));
                    outSize = iop->valueSize = sizeof(double);
                    outValue = iop->value.data;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case IGS_BOOL_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    outSize = iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:*(bool*)(value);
                    outValue = &(iop->value.i);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(bool*)(value);
                    outValue = &(iop->value.d);
                    igsAgent_debug(agent, "set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:*(bool*)value;
                    outValue = &(iop->value.b);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    if (value == NULL){
                        iop->value.s = strdup("");
                    }else{
                        char buf[32] = "";
                        snprintf(buf, 31, "%d", (value == NULL)?0:*(bool*)value);
                        iop->value.s = strdup(buf);
                    }
                    outSize = iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    outValue = iop->value.s;
                    igsAgent_debug(agent, "set %s to %s (length: %zu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(bool));
                    memcpy(iop->value.data, value, sizeof(bool));
                    outSize = iop->valueSize = sizeof(bool);
                    outValue = iop->value.data;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case IGS_STRING_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    outSize = iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:atoi((char*)value);
                    outValue = &(iop->value.i);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:atof((char*)value);
                    outValue = &(iop->value.d);
                    igsAgent_debug(agent, "set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                {
                    char *v = (char *)value;
                    if (v == NULL){
                        iop->value.b = false;
                    }else if (strcmp(v, "false") == 0||
                              strcmp(v, "False") == 0||
                              strcmp(v, "FALSE") == 0){
                        iop->value.b = false;
                    }else if (strcmp(v, "true") == 0||
                              strcmp(v, "True") == 0||
                              strcmp(v, "TRUE") == 0){
                        iop->value.b = true;
                    }else{
                        iop->value.b = atoi(v)?true:false;
                    }
                }
                    outSize = iop->valueSize = sizeof(bool);
                    outValue = &(iop->value.b);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    if (value == NULL){
                        iop->value.s = strdup("");
                    }else{
                        iop->value.s = strdup((char *)value);
                    }
                    outSize = iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    outValue = iop->value.s;
                    igsAgent_debug(agent, "set %s to %s (length: %zu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    size_t s = 0;
                    if (value != NULL){
                        //warning : when copying string to data, we remove the final '\0'
                        s = size;
                        iop->value.data = calloc (1, s);
                        memcpy(iop->value.data, value, s);
                    }
                    outSize = iop->valueSize = s;
                    outValue = iop->value.data;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, s);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case IGS_IMPULSION_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    outSize = iop->valueSize = sizeof(int);
                    iop->value.i = 0;
                    outValue = &(iop->value.i);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = 0;
                    outValue = &(iop->value.d);
                    igsAgent_debug(agent, "set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = false;
                    outValue = &(iop->value.b);
                    igsAgent_debug(agent, "set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    iop->value.s = strdup("");
                    outSize = iop->valueSize = sizeof(char);
                    outValue = iop->value.s;
                    igsAgent_debug(agent, "set %s to %s (length: %zu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    outSize = iop->valueSize = 0;
                    outValue = NULL;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
            outSize = iop->valueSize = 0;
        }
            break;
        case IGS_DATA_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    igsAgent_warn(agent, "Cannot write data into integer IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_DOUBLE_T:
                    igsAgent_warn(agent, "Cannot write data into double IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_BOOL_T:
                    igsAgent_warn(agent, "Cannot write data into boolean IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_STRING_T:
                {
                    igsAgent_warn(agent, "Cannot write data into string IOP %s", iopName);
                    ret = 0;
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igsAgent_debug(agent, "set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, size);
                    memcpy(iop->value.data, value, size);
                    outSize = iop->valueSize = size;
                    outValue = iop->value.data;
                    igsAgent_debug(agent, "set %s data (length: %zu)", iopName, size);
                }
                    break;
                default:
                    igsAgent_error(agent, "%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        default:
            break;
    }
    model_readWriteUnlock();
    
    if (ret){
        model_runObserveCallbacksForIOP(agent, iop, outValue, outSize);
    }
    return iop;
}

igs_iop_t * model_findIopByName(igs_agent_t *agent, const char *name, iop_t type){
    igs_iop_t *found = NULL;
    
    switch (type) {
        case IGS_INPUT_T:
            return model_findInputByName(agent, name);
            break;
        case IGS_OUTPUT_T:
            return model_findOutputByName(agent, name);
            break;
        case IGS_PARAMETER_T:
            return model_findParameterByName(agent, name);
            break;
        default:
            igsAgent_error(agent, "Unknown IOP type %d", type);
            break;
    }
    
    return found;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

// --------------------------------  READ ------------------------------------//

int igsAgent_readInput(igs_agent_t *agent, const char *name, void **value, size_t *size){
    return igsAgent_readIOP(agent, name, IGS_INPUT_T, value, size);
}

int igsAgent_readOutput(igs_agent_t *agent, const char *name, void **value, size_t *size){
    return igsAgent_readIOP(agent, name, IGS_OUTPUT_T, value, size);
}

int igsAgent_readParameter(igs_agent_t *agent, const char *name, void **value, size_t *size){
    return igsAgent_readIOP(agent, name, IGS_PARAMETER_T, value, size);
}

bool igsAgent_readInputAsBool(igs_agent_t *agent, const char *name){
    return model_readIopAsBool(agent, name, IGS_INPUT_T);
}

int igsAgent_readInputAsInt(igs_agent_t *agent, const char *name){
    return model_readIopAsInt(agent, name, IGS_INPUT_T);
}

double igsAgent_readInputAsDouble(igs_agent_t *agent, const char *name){
    return model_readIopAsDouble(agent, name, IGS_INPUT_T);
}

char* igsAgent_readInputAsString(igs_agent_t *agent, const char *name){
    return model_readIopAsString(agent, name, IGS_INPUT_T);
}

igs_result_t igsAgent_readInputAsData(igs_agent_t *agent, const char *name, void **data, size_t *size){
    return model_readIopAsData(agent, name, IGS_INPUT_T, data, size);
}

igs_result_t igsAgent_readInputAsZMQMsg(igs_agent_t *agent, const char *name, zmsg_t **msg){
    void *data = NULL;
    size_t size = 0;
    int ret = model_readIopAsData(agent, name, IGS_INPUT_T, &data, &size);
    zframe_t *frame = zframe_new(data, size);
    free(data);
    *msg = zmsg_decode(frame);
    zframe_destroy(&frame);
    return ret;
}

bool igsAgent_readOutputAsBool(igs_agent_t *agent, const char *name){
    return model_readIopAsBool(agent, name, IGS_OUTPUT_T);
}

int igsAgent_readOutputAsInt(igs_agent_t *agent, const char *name){
    return model_readIopAsInt(agent, name, IGS_OUTPUT_T);
}

double igsAgent_readOutputAsDouble(igs_agent_t *agent, const char *name){
    return model_readIopAsDouble(agent, name, IGS_OUTPUT_T);
}

char* igsAgent_readOutputAsString(igs_agent_t *agent, const char *name){
    return model_readIopAsString(agent, name, IGS_OUTPUT_T);
}

igs_result_t igsAgent_readOutputAsData(igs_agent_t *agent, const char *name, void **data, size_t *size){
    return model_readIopAsData(agent, name, IGS_OUTPUT_T, data, size);
}

bool igsAgent_readParameterAsBool(igs_agent_t *agent, const char *name){
    return model_readIopAsBool(agent, name, IGS_PARAMETER_T);
}

int igsAgent_readParameterAsInt(igs_agent_t *agent, const char *name){
    return model_readIopAsInt(agent, name, IGS_PARAMETER_T);
}

double igsAgent_readParameterAsDouble(igs_agent_t *agent, const char *name){
    return model_readIopAsDouble(agent, name, IGS_PARAMETER_T);
}

char* igsAgent_readParameterAsString(igs_agent_t *agent, const char *name){
    return model_readIopAsString(agent, name, IGS_PARAMETER_T);
}

igs_result_t igsAgent_readParameterAsData(igs_agent_t *agent, const char *name, void **data, size_t *size){
    return model_readIopAsData(agent, name, IGS_PARAMETER_T, data, size);
}

// --------------------------------  WRITE ------------------------------------//

igs_result_t igsAgent_writeInputAsBool(igs_agent_t *agent, const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_BOOL_T, &value, sizeof(bool));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeInputAsInt(igs_agent_t *agent, const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_INTEGER_T, &value, sizeof(int));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeInputAsDouble(igs_agent_t *agent, const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_DOUBLE_T, &value, sizeof(double));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeInputAsString(igs_agent_t *agent, const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    size_t valueLength = (value == NULL) ? 0 : strlen(value)+1;
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_STRING_T, (char *)value, valueLength);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeInputAsImpulsion(igs_agent_t *agent, const char *name){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_IMPULSION_T, NULL, 0);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeInputAsData(igs_agent_t *agent, const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_INPUT_T, IGS_DATA_T, value, size);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsBool(igs_agent_t *agent, const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_BOOL_T, &value, sizeof(bool));
    if (iop)
        network_publishOutput(agent, iop);
    
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsInt(igs_agent_t *agent, const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_INTEGER_T, &value, sizeof(int));
    if (iop)
        network_publishOutput(agent, iop);

    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsDouble(igs_agent_t *agent, const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_DOUBLE_T, &value, sizeof(double));
    if (iop)
        network_publishOutput(agent, iop);

    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsString(igs_agent_t *agent, const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    size_t length = (value == NULL)?0:strlen(value)+1;
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_STRING_T, (char *)value, length);
    if (iop)
        network_publishOutput(agent, iop);

    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsImpulsion(igs_agent_t *agent, const char *name){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_IMPULSION_T, NULL, 0);
    if (iop)
        network_publishOutput(agent, iop);

    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsData(igs_agent_t *agent, const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (iop)
        network_publishOutput(agent, iop);
    
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeOutputAsZMQMsg(igs_agent_t *agent, const char *name, zmsg_t *msg){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    zframe_t *frame = zmsg_encode(msg);
    void *value = zframe_data(frame);
    size_t size = zframe_size(frame);
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    if (iop)
        network_publishOutput(agent, iop);
    zframe_destroy(&frame);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeParameterAsBool(igs_agent_t *agent, const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_PARAMETER_T, IGS_BOOL_T, &value, sizeof(bool));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeParameterAsInt(igs_agent_t *agent, const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_PARAMETER_T, IGS_INTEGER_T, &value, sizeof(int));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeParameterAsDouble(igs_agent_t *agent, const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_PARAMETER_T, IGS_DOUBLE_T, &value, sizeof(double));
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeParameterAsString(igs_agent_t *agent, const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    size_t valueLength = (value == NULL) ? 0 : strlen(value)+1;
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_PARAMETER_T, IGS_STRING_T, (char *)value, valueLength);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

igs_result_t igsAgent_writeParameterAsData(igs_agent_t *agent, const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_FAILURE;
    }
    const igs_iop_t *iop = model_writeIOP(agent, name, IGS_PARAMETER_T, IGS_DATA_T, value, size);
    return (iop == NULL)?IGS_FAILURE : IGS_SUCCESS;
}

void igsAgent_clearDataForInput(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, name, IGS_INPUT_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igsAgent_error(agent, "%s is not a known input", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igsAgent_error(agent, "%s type is not IGS_DATA_T", name);
            }
        }
    }
}

void igsAgent_clearDataForOutput(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, name, IGS_OUTPUT_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igsAgent_error(agent, "%s is not a known output", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igsAgent_error(agent, "%s type is not IGS_DATA_T", name);
            }
        }
    }
}

void igsAgent_clearDataForParameter(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, name, IGS_PARAMETER_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igsAgent_error(agent, "%s is not a known parameter", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igsAgent_error(agent, "%s type is not IGS_DATA_T", name);
            }
        }
    }
}

// --------------------------------  INTROSPECTION ------------------------------------//

iopType_t igsAgent_getTypeForInput(igs_agent_t *agent, const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return model_getTypeForIOP(agent, name, IGS_INPUT_T);
}

iopType_t igsAgent_getTypeForOutput(igs_agent_t *agent, const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return model_getTypeForIOP(agent, name, IGS_OUTPUT_T);
}

iopType_t igsAgent_getTypeForParameter(igs_agent_t *agent, const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return IGS_UNKNOWN_T;
    }
    return model_getTypeForIOP(agent, name, IGS_PARAMETER_T);
}

int igsAgent_getInputsNumber(igs_agent_t *agent){
    if(agent->definition == NULL){
        igsAgent_warn(agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT(agent->definition->inputs_table);
}

int igsAgent_getOutputsNumber(igs_agent_t *agent){
    if(agent->definition == NULL){
        igsAgent_warn(agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT(agent->definition->outputs_table);
}

int igsAgent_getParametersNumber(igs_agent_t *agent){
    if(agent->definition == NULL){
        igsAgent_warn(agent, "definition is NULL");
        return 0;
    }
    return HASH_COUNT(agent->definition->params_table);
}

char ** igsAgent_getInputsList(igs_agent_t *agent, size_t *nbOfElements){
    return model_getIopList(agent, nbOfElements, IGS_INPUT_T);
}

char ** igsAgent_getOutputsList(igs_agent_t *agent, size_t *nbOfElements){
    return model_getIopList(agent, nbOfElements, IGS_OUTPUT_T);
}

char ** igsAgent_getParametersList(igs_agent_t *agent, size_t *nbOfElements){
    return model_getIopList(agent, nbOfElements, IGS_PARAMETER_T);
}

void igs_freeIOPList(char ***list, size_t nbOfElements){
    //FIXME: secure this function if nbOfElements exceeds allocated value
    if (list != NULL && *list != NULL){
        if (nbOfElements < 1)
            return;
        int i = 0;
        for (i = 0; i < nbOfElements; i++){
            if ((*list)[i] != NULL){
                free((*list)[i]);
            }
        }
        free(*list);
        *list = NULL;
    }
}

bool igsAgent_checkInputExistence(igs_agent_t *agent, const char *name){
    if (agent->definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Input name cannot be NULL or empty\n");
        return false;
    }
    return model_checkIOPExistence(agent, name, agent->definition->inputs_table);
}

bool igsAgent_checkOutputExistence(igs_agent_t *agent, const char *name){
    if (agent->definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_warn(agent, "Output name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(agent, name, agent->definition->outputs_table);
}

bool igsAgent_checkParameterExistence(igs_agent_t *agent, const char *name){
    if (agent->definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igsAgent_warn(agent, "Parameter name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(agent, name, agent->definition->params_table);
}

// --------------------------------  OBSERVE ------------------------------------//

int igsAgent_observeInput(igs_agent_t *agent, const char *name, igsAgent_observeCallback cb, void *myData){
    return model_observe(agent, name, IGS_INPUT_T, cb, myData);
}

int igsAgent_observeOutput(igs_agent_t *agent, const char *name, igsAgent_observeCallback cb, void * myData){
    return model_observe(agent, name, IGS_OUTPUT_T, cb, myData);
}

int igsAgent_observeParameter(igs_agent_t *agent, const char *name, igsAgent_observeCallback cb, void * myData){
    return model_observe(agent, name, IGS_PARAMETER_T, cb, myData);
}

// --------------------------------  MUTE ------------------------------------//


igs_result_t igsAgent_muteOutput(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, (char*) name, IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igsAgent_warn(agent, "Output '%s' not found", name);
        return IGS_FAILURE;
    }
    iop->is_muted = true;
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "OUTPUT_MUTED");
        zmsg_addstr(msg, name);
        zmsg_addstr(msg, agent->uuid);
        zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
        bus_zyreUnlock();
    }
    return IGS_SUCCESS;
}

igs_result_t igsAgent_unmuteOutput(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, (char*) name,IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igsAgent_warn(agent, "Output '%s' not found", name);
        return IGS_FAILURE;
    }
    iop->is_muted = false;
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "OUTPUT_UNMUTED");
        zmsg_addstr(msg, name);
        zmsg_addstr(msg, agent->uuid);
        zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
        bus_zyreUnlock();
    }
    return IGS_SUCCESS;
}

bool igsAgent_isOutputMuted(igs_agent_t *agent, const char *name){
    igs_iop_t *iop = model_findIopByName(agent, (char*) name,IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igsAgent_warn(agent, "Output '%s' not found", name);
        return 0;
    }
    return iop->is_muted;
}
