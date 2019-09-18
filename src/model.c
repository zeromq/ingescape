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
            igs_error("mutex init failed");
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (model_readWriteMutex == NULL){
        if (pthread_mutex_init(&model_readWriteMutex) != 0){
            igs_error("mutex init failed");
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
        igs_error("mutex was NULL");
    }
}

void model_runObserveCallbacksForIOP(agent_iop_t *iop, void *value, size_t valueSize){
    igs_observe_callback_t *cb;
    DL_FOREACH(iop->callbacks, cb){
        cb->callback_ptr(iop->type, iop->name, iop->value_type, value, valueSize, cb->data);
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

static int model_observe(const char* name, iop_t iopType, igs_observeCallback cb, void* myData){

    //find the iop
    agent_iop_t *iop = model_findIopByName((char*) name, iopType);

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
        igs_error("Cannot find %s %s", t, name);
        return 0;
    }

    //callback not defined
    if(cb == NULL) {
        igs_error("Callback cannot be NULL (called for %s)", name);
        return 0;
    }

    igs_observe_callback_t *new_callback = malloc(sizeof(igs_observe_callback_t));
    new_callback->callback_ptr = cb;
    new_callback->data = myData;
    DL_APPEND(iop->callbacks, new_callback);

    //igs_trace("observe iop with name %s and type %d\n", name, iopType);

    return 1;
}

iopType_t model_getTypeForIOP(const char *name, iop_t type){
    if((name == NULL) || (strlen(name) == 0)){
        igs_error("Name cannot be NULL or empty");
        return 0;
    }
    if(igs_internal_definition == NULL){
        igs_error("Definition is NULL");
        return 0;
    }
    
    agent_iop_t *iop = NULL;
    if (type == IGS_INPUT_T){
        HASH_FIND_STR(igs_internal_definition->inputs_table, name, iop);
        if(iop == NULL){
            igs_error("Input %s cannot be found", name);
            return 0;
        }
    } else if (type == IGS_OUTPUT_T){
        HASH_FIND_STR(igs_internal_definition->outputs_table, name, iop);
        if(iop == NULL){
            igs_error("Output %s cannot be found", name);
            return 0;
        }
    } else if (type == IGS_PARAMETER_T){
        HASH_FIND_STR(igs_internal_definition->params_table, name, iop);
        if(iop == NULL){
            igs_error("Parameter %s cannot be found", name);
            return 0;
        }
    }else{
        igs_error("Unknown IOP type %d", type);
        return 0;
    }
    
    return iop->value_type;
}

agent_iop_t *model_findInputByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && igs_internal_definition != NULL){
        HASH_FIND_STR( igs_internal_definition->inputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igs_error("Input name cannot be NULL or empty");
        }else{
            igs_error("Definition is NULL");
        }
    }
    return found;
}

agent_iop_t *model_findOutputByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && igs_internal_definition != NULL){
        HASH_FIND_STR( igs_internal_definition->outputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igs_error("Output name cannot be NULL or empty");
        }else{
            igs_error("Definition is NULL");
        }
    }
    return found;
}

agent_iop_t *model_findParameterByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && igs_internal_definition != NULL){
        HASH_FIND_STR( igs_internal_definition->params_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            igs_error("Parameter name cannot be NULL or empty");
        }else{
            igs_error("Definition is NULL");
        }
    }
    return found;
}

void* model_getValueFor(const char *name, iop_t type){
    agent_iop_t *iop = model_findIopByName((char*) name,type);
    if(iop == NULL){
        igs_error("%s not found", name);
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
            igs_error("Unknown value type for %s", name);
            break;
    }
    return NULL;
}

int igs_readIOP(const char *name, iop_t type, void **value, size_t *size){
    agent_iop_t *iop = model_findIopByName((char*) name, type);
    if(iop == NULL){
        igs_error("%s not found", name);
        return 0;
    }
    if (iop->value_type == IGS_IMPULSION_T){
        *value = NULL;
        *size = 0;
    }else{
        *value = calloc(1, iop->valueSize);
        memcpy(*value, model_getValueFor(name, type), iop->valueSize);
        *size = iop->valueSize;
    }
    return 1;
}

bool model_readIopAsBool (const char *name, iop_t type){
    bool res = false;
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                res = iop->value.b;
                return res;
                break;
                
            case IGS_INTEGER_T:
                igs_warn("Implicit conversion from int to bool for %s", name);
                res = (iop->value.i == 0)?false:true;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igs_warn("Implicit conversion from double to bool for %s", name);
                res = (iop->value.d >= 0 && iop->value.d <= 0)?false:true;
                return res;
                break;
                
            case IGS_STRING_T:
                if (strcmp(iop->value.s, "true") == 0){
                    igs_warn("Implicit conversion from string to bool for %s", name);
                    return true;
                }
                else if (strcmp(iop->value.s, "false") == 0){
                    igs_warn("Implicit conversion from string to bool for %s", name);
                    return false;
                }else{
                    igs_warn("Implicit conversion from double to bool for %s (string value is %s and false was returned)", name, iop->value.s);
                    return false;
                }
                break;
                
            default:
                igs_error("No implicit conversion possible for %s (false was returned)", name);
                return false;
                break;
        }
    }else{
        igs_error("%s not found", name);
        return false;
    }
}

int model_readIopAsInt (const char *name, iop_t type){
    int res = 0;
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                igs_warn("Implicit conversion from bool to int for %s", name);
                res =  (iop->value.b)?1:0;
                return res;
                break;
                
            case IGS_INTEGER_T:
                res = iop->value.i;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igs_warn("Implicit conversion from double to int for %s", name);
                if(iop->value.d < 0) {
                    res = (int) (iop->value.d - 0.5);
                    return res;
                }else {
                    res = (int) (iop->value.d + 0.5);
                    return res;
                }
                break;
                
            case IGS_STRING_T:
                igs_warn("Implicit conversion from string %s to int for %s", iop->value.s, name);
                res = atoi(iop->value.s);
                return res;
                break;
                
            default:
                igs_error("No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        igs_error("%s not found", name);
        return 0;
    }
}

double model_readIopAsDouble (const char *name, iop_t type){
    double res = 0;
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_BOOL_T:
                igs_warn("Implicit conversion from bool to double for %s", name);
                res = (iop->value.b)?1:0;
                return res;
                break;
                
            case IGS_INTEGER_T:
                igs_warn("Implicit conversion from int to double for %s", name);
                res = iop->value.i;
                return res;
                break;
                
            case IGS_DOUBLE_T:
                res = iop->value.d;
                return res;
                break;
                
            case IGS_STRING_T:
                igs_warn("Implicit conversion from string %s to double for %s", iop->value.s, name);
                res = atof(iop->value.s);
                return res;
                break;
                
            default:
                igs_error("No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        igs_error("%s not found", name);
        return 0;
    }
}

char *model_readIopAsString (const char *name, iop_t type){
    char *res = NULL;
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case IGS_STRING_T:
                res = strdup(iop->value.s);
                return res;
                break;
                
            case IGS_BOOL_T:
                igs_warn("Implicit conversion from bool to string for %s", name);
                res = iop->value.b ? strdup("true") : strdup("false");
                return res;
                break;
                
            case IGS_INTEGER_T:
                igs_warn("Implicit conversion from int to string for %s", name);
                res = model_intToString(iop->value.i);
                return res;
                break;
                
            case IGS_DOUBLE_T:
                igs_warn("Implicit conversion from double to string for %s", name);
                res = model_doubleToString(iop->value.d);
                return res;
                break;
                
            default:
                igs_error("No implicit conversion possible for %s (NULL was returned)", name);
                return NULL;
                break;
        }
    }else{
        igs_error("%s not found", name);
        return NULL;
    }
}

int model_readIopAsData (const char *name, iop_t type, void **value, size_t *size){
    agent_iop_t *iop = model_findIopByName((char*) name, type);
    if(iop == NULL){
        igs_error("%s not found", name);
        *value = NULL;
        *size = 0;
        return 0;
    }
    if(iop->value_type != IGS_DATA_T){
        igs_error("No implicit conversion possible for %s (NULL was returned)", name);
        *value = NULL;
        *size = 0;
        return 0;
    }
    *size = iop->valueSize;
    *value = calloc(1, iop->valueSize);
    memcpy(*value, model_getValueFor(name, type), *size);
    return 1;
}

bool model_checkIOPExistence(const char *name, agent_iop_t *hash){
    agent_iop_t *iop = NULL;
    if(igs_internal_definition == NULL){
        igs_error("Definition is NULL");
        return false;
    }
    HASH_FIND_STR(hash, name, iop);
    if(iop == NULL){
        return false;
    }
    return true;
}

char **model_getIopList(long *nbOfElements, iop_t type){
    if(igs_internal_definition == NULL){
        igs_warn("Definition is NULL");
        *nbOfElements = 0;
        return NULL;
    }
    agent_iop_t *hash = NULL;
    switch (type) {
        case IGS_INPUT_T:
            hash = igs_internal_definition->inputs_table;
            break;
        case IGS_OUTPUT_T:
            hash = igs_internal_definition->outputs_table;
            break;
        case IGS_PARAMETER_T:
            hash = igs_internal_definition->params_table;
            break;
            
        default:
            break;
    }
    long N = (*nbOfElements) = HASH_COUNT(hash);
    if(N < 1)
        return NULL;
    
    char ** list = (char**) malloc( N * sizeof(char*));
    agent_iop_t *current_iop;
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
char* model_getIOPValueAsString (agent_iop_t* iop){
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

const agent_iop_t* model_writeIOP (const char *iopName, iop_t iopType, iopType_t valType, void* value, size_t size){
    agent_iop_t *iop = model_findIopByName((char*) iopName, iopType);
    if(iop == NULL){
        igs_error("%s not found for writing", iopName);
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
                    igs_debug("set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(int*)(value);
                    outValue = &(iop->value.d);
                    igs_debug("set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:((*(int*)(value))?true:false);
                    outValue = &(iop->value.b);
                    igs_debug("set %s to %i", iopName, iop->value.b);
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
                    igs_debug("set %s to %s (length: %lu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
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
                    igs_debug("set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
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
                    igs_debug("set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:(double)(*(double*)(value));
                    outValue = &(iop->value.d);
                    igs_debug("set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:(((int)(*(double*)(value)))?true:false);
                    outValue = &(iop->value.b);
                    igs_debug("set %s to %i", iopName, iop->value.b);
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
                    igs_debug("set %s to %s (length: %lu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
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
                    igs_debug("set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
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
                    igs_debug("set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(bool*)(value);
                    outValue = &(iop->value.d);
                    igs_debug("set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:*(bool*)value;
                    outValue = &(iop->value.b);
                    igs_debug("set %s to %i", iopName, iop->value.b);
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
                    igs_debug("set %s to %s (length: %lu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
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
                    igs_debug("set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
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
                    igs_debug("set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:atof((char*)value);
                    outValue = &(iop->value.d);
                    igs_debug("set %s to %lf", iopName, iop->value.d);
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
                    igs_debug("set %s to %i", iopName, iop->value.b);
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
                    igs_debug("set %s to %s (length: %lu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
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
                    igs_debug("set %s data (length: %zu)", iopName, s);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
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
                    igs_debug("set %s to %i", iopName, iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    outSize = iop->valueSize = sizeof(double);
                    iop->value.d = 0;
                    outValue = &(iop->value.d);
                    igs_debug("set %s to %lf", iopName, iop->value.d);
                    break;
                case IGS_BOOL_T:
                    outSize = iop->valueSize = sizeof(bool);
                    iop->value.b = false;
                    outValue = &(iop->value.b);
                    igs_debug("set %s to %i", iopName, iop->value.b);
                    break;
                case IGS_STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    iop->value.s = strdup("");
                    outSize = iop->valueSize = sizeof(char);
                    outValue = iop->value.s;
                    igs_debug("set %s to %s (length: %lu)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
                    break;
                case IGS_DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    outSize = iop->valueSize = 0;
                    outValue = NULL;
                    igs_debug("set %s data (length: %zu)", iopName, iop->valueSize);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
            outSize = iop->valueSize = 0;
            igs_debug("set impulsion %s", iopName);
        }
            break;
        case IGS_DATA_T:{
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    igs_warn("Cannot write data into integer IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_DOUBLE_T:
                    igs_warn("Cannot write data into double IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_BOOL_T:
                    igs_warn("Cannot write data into boolean IOP %s", iopName);
                    ret = 0;
                    break;
                case IGS_STRING_T:
                {
                    igs_warn("Cannot write data into string IOP %s", iopName);
                    ret = 0;
                }
                    break;
                case IGS_IMPULSION_T:
                    //nothing to do
                    outSize = iop->valueSize = 0;
                    igs_debug("set impulsion %s", iopName);
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
                    igs_debug("set %s data (length: %zu)", iopName, size);
                }
                    break;
                default:
                    igs_error("%s has an invalid value type %d", iopName, iop->value_type);
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
        model_runObserveCallbacksForIOP(iop, outValue, outSize);
    }
    return iop;
}

agent_iop_t * model_findIopByName(const char *name, iop_t type){
    agent_iop_t *found = NULL;
    
    switch (type) {
        case IGS_INPUT_T:
            return model_findInputByName(name);
            break;
        case IGS_OUTPUT_T:
            return model_findOutputByName(name);
            break;
        case IGS_PARAMETER_T:
            return model_findParameterByName(name);
            break;
        default:
            igs_error("Unknown IOP type %d", type);
            break;
    }
    
    return found;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

// --------------------------------  READ ------------------------------------//

int igs_readInput(const char *name, void **value, size_t *size){
    return igs_readIOP(name, IGS_INPUT_T, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    return igs_readIOP(name, IGS_OUTPUT_T, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    return igs_readIOP(name, IGS_PARAMETER_T, value, size);
}

bool igs_readInputAsBool(const char *name){
    return model_readIopAsBool(name, IGS_INPUT_T);
}

int igs_readInputAsInt(const char *name){
    return model_readIopAsInt(name, IGS_INPUT_T);
}

double igs_readInputAsDouble(const char *name){
    return model_readIopAsDouble(name, IGS_INPUT_T);
}

char* igs_readInputAsString(const char *name){
    return model_readIopAsString(name, IGS_INPUT_T);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    return model_readIopAsData(name, IGS_INPUT_T, data, size);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    void *data = NULL;
    size_t size = 0;
    int ret = model_readIopAsData(name, IGS_INPUT_T, &data, &size);
    zframe_t *frame = zframe_new(data, size);
    free(data);
    *msg = zmsg_decode(frame);
    zframe_destroy(&frame);
    return ret;
}

bool igs_readOutputAsBool(const char *name){
    return model_readIopAsBool(name, IGS_OUTPUT_T);
}

int igs_readOutputAsInt(const char *name){
    return model_readIopAsInt(name, IGS_OUTPUT_T);
}

double igs_readOutputAsDouble(const char *name){
    return model_readIopAsDouble(name, IGS_OUTPUT_T);
}

char* igs_readOutputAsString(const char *name){
    return model_readIopAsString(name, IGS_OUTPUT_T);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    return model_readIopAsData(name, IGS_OUTPUT_T, data, size);
}

bool igs_readParameterAsBool(const char *name){
    return model_readIopAsBool(name, IGS_PARAMETER_T);
}

int igs_readParameterAsInt(const char *name){
    return model_readIopAsInt(name, IGS_PARAMETER_T);
}

double igs_readParameterAsDouble(const char *name){
    return model_readIopAsDouble(name, IGS_PARAMETER_T);
}

char* igs_readParameterAsString(const char *name){
    return model_readIopAsString(name, IGS_PARAMETER_T);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    return model_readIopAsData(name, IGS_PARAMETER_T, data, size);
}

// --------------------------------  WRITE ------------------------------------//

int igs_writeInputAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_BOOL_T, &value, sizeof(bool));
    return (iop == NULL)?0:1;
}

int igs_writeInputAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_INTEGER_T, &value, sizeof(int));
    return (iop == NULL)?0:1;
}

int igs_writeInputAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_DOUBLE_T, &value, sizeof(double));
    return (iop == NULL)?0:1;
}

int igs_writeInputAsString(const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_STRING_T, (char *)value, strlen(value)+1);
    return (iop == NULL)?0:1;
}

int igs_writeInputAsImpulsion(const char *name){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_IMPULSION_T, NULL, 0);
    return (iop == NULL)?0:1;
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_INPUT_T, IGS_DATA_T, value, size);
    return (iop == NULL)?0:1;
}

int igs_writeOutputAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_BOOL_T, &value, sizeof(bool));
    network_publishOutput(iop);
    
    return (iop == NULL)?0:1;
}

int igs_writeOutputAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_INTEGER_T, &value, sizeof(int));
    network_publishOutput(iop);

    return (iop == NULL)?0:1;
}

int igs_writeOutputAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_DOUBLE_T, &value, sizeof(double));
    network_publishOutput(iop);

    return (iop == NULL)?0:1;
}

int igs_writeOutputAsString(const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    size_t length = (value == NULL)?0:strlen(value)+1;
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_STRING_T, (char *)value, length);
    network_publishOutput(iop);

    return (iop == NULL)?0:1;
}

int igs_writeOutputAsImpulsion(const char *name){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_IMPULSION_T, NULL, 0);
    network_publishOutput(iop);

    return (iop == NULL)?0:1;
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    network_publishOutput(iop);
    
    return (iop == NULL)?0:1;
}

int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    if (name == NULL || strlen(name) == 0){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    zframe_t *frame = zmsg_encode(msg);
    void *value = zframe_data(frame);
    size_t size = zframe_size(frame);
    const agent_iop_t *iop = model_writeIOP(name, IGS_OUTPUT_T, IGS_DATA_T, value, size);
    network_publishOutput(iop);
    zframe_destroy(&frame);
    return (iop == NULL)?0:1;
}

int igs_writeParameterAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_PARAMETER_T, IGS_BOOL_T, &value, sizeof(bool));
    return (iop == NULL)?0:1;
}

int igs_writeParameterAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_PARAMETER_T, IGS_INTEGER_T, &value, sizeof(int));
    return (iop == NULL)?0:1;
}

int igs_writeParameterAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_PARAMETER_T, IGS_DOUBLE_T, &value, sizeof(double));
    return (iop == NULL)?0:1;
}

int igs_writeParameterAsString(const char *name, const char *value){
    if (name == NULL || strlen(name) == 0){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_PARAMETER_T, IGS_STRING_T, (char *)value, strlen(value)+1);
    return (iop == NULL)?0:1;
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    if (name == NULL || strlen(name) == 0){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    const agent_iop_t *iop = model_writeIOP(name, IGS_PARAMETER_T, IGS_DATA_T, value, size);
    return (iop == NULL)?0:1;
}

void igs_clearDataForInput(const char *name){
    agent_iop_t *iop = model_findIopByName(name, IGS_INPUT_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igs_error("%s is not a known input", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igs_error("%s type is not IGS_DATA_T", name);
            }
        }
    }
}

void igs_clearDataForOutput(const char *name){
    agent_iop_t *iop = model_findIopByName(name, IGS_OUTPUT_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igs_error("%s is not a known output", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igs_error("%s type is not IGS_DATA_T", name);
            }
        }
    }
}

void igs_clearDataForParameter(const char *name){
    agent_iop_t *iop = model_findIopByName(name, IGS_PARAMETER_T);
    if (iop != NULL && iop->value_type == IGS_DATA_T && iop->value.data != NULL){
        free(iop->value.data);
        iop->value.data = NULL;
        iop->valueSize = 0;
    }else{
        if (iop == NULL){
            igs_error("%s is not a known parameter", name);
        }else{
            if (iop->value_type != IGS_DATA_T){
                igs_error("%s type is not IGS_DATA_T", name);
            }
        }
    }
}

// --------------------------------  INTROSPECTION ------------------------------------//

iopType_t igs_getTypeForInput(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igs_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, IGS_INPUT_T);
}

iopType_t igs_getTypeForOutput(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igs_error("Output name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, IGS_OUTPUT_T);
}

iopType_t igs_getTypeForParameter(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        igs_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, IGS_PARAMETER_T);
}

int igs_getInputsNumber(){
    if(igs_internal_definition == NULL){
        igs_warn("definition is NULL");
        return 0;
    }
    return HASH_COUNT(igs_internal_definition->inputs_table);
}

int igs_getOutputsNumber(){
    if(igs_internal_definition == NULL){
        igs_warn("definition is NULL");
        return 0;
    }
    return HASH_COUNT(igs_internal_definition->outputs_table);
}

int igs_getParametersNumber(){
    if(igs_internal_definition == NULL){
        igs_warn("definition is NULL");
        return 0;
    }
    return HASH_COUNT(igs_internal_definition->params_table);
}

char ** igs_getInputsList(long *nbOfElements){
    return model_getIopList(nbOfElements, IGS_INPUT_T);
}

char ** igs_getOutputsList(long *nbOfElements){
    return model_getIopList(nbOfElements, IGS_OUTPUT_T);
}

char ** igs_getParametersList(long *nbOfElements){
    return model_getIopList(nbOfElements, IGS_PARAMETER_T);
}

void igs_freeIOPList(char ***list, long nbOfElements){
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

bool igs_checkInputExistence(const char *name){
    if (igs_internal_definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igs_error("Input name cannot be NULL or empty\n");
        return false;
    }
    return model_checkIOPExistence(name, igs_internal_definition->inputs_table);
}

bool igs_checkOutputExistence(const char *name){
    if (igs_internal_definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igs_warn("Output name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(name, igs_internal_definition->outputs_table);
}

bool igs_checkParameterExistence(const char *name){
    if (igs_internal_definition == NULL){
        return false;
    }
    if((name == NULL) || (strlen(name) == 0)){
        igs_warn("Parameter name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(name, igs_internal_definition->params_table);
}

// --------------------------------  OBSERVE ------------------------------------//

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    return model_observe(name, IGS_INPUT_T, cb, myData);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    return model_observe(name, IGS_OUTPUT_T, cb, myData);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    return model_observe(name, IGS_PARAMETER_T, cb, myData);
}

// --------------------------------  MUTE ------------------------------------//


int igs_muteOutput(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name, IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igs_warn("Output '%s' not found", name);
        return 0;
    }
    iop->is_muted = true;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "OUTPUT_MUTED %s", name);
    }
    return 1;
}

int igs_unmuteOutput(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name,IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igs_warn("Output '%s' not found", name);
        return 0;
    }
    iop->is_muted = false;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "OUTPUT_UNMUTED %s", name);
    }
    return 1;
}

bool igs_isOutputMuted(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name,IGS_OUTPUT_T);
    if(iop == NULL || iop->type != IGS_OUTPUT_T){
        igs_warn("Output '%s' not found", name);
        return 0;
    }
    return iop->is_muted;
}
