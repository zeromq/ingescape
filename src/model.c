//
//  model.c
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Modified by Mathieu Poirier
//  Modified by Stephane Vales
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include "unixfunctions.h"
#endif
#include "mastic_private.h"
#include "uthash/utlist.h"

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void model_runObserveCallbacksForIOP(agent_iop_t *iop, void *value, long valueSize){
    mtic_observe_callback_t *cb;
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

static int model_observe(const char* name, iop_t iopType, mtic_observeCallback cb, void* myData){

    //find the iop
    agent_iop_t *iop = model_findIopByName((char*) name, iopType);

    // Check if the input has been returned.
    if(iop == NULL){
        mtic_error("Cannot find %s with type %d", name, iopType);
        return 0;
    }

    //callback not defined
    if(cb == NULL) {
        mtic_error("Callback cannot be NULL (called for %s)", name);
        return 0;
    }

    mtic_observe_callback_t *new_callback = malloc(sizeof(mtic_observe_callback_t));
    new_callback->callback_ptr = cb;
    new_callback->data = myData;
    DL_APPEND(iop->callbacks, new_callback);

    //mtic_trace("observe iop with name %s and type %d\n", name, iopType);

    return 1;
}

iopType_t model_getTypeForIOP(const char *name, iop_t type){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_error("Name cannot be NULL or empty");
        return 0;
    }
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return -1;
    }
    
    agent_iop_t *iop = NULL;
    if (type == INPUT_T){
        HASH_FIND_STR(mtic_internal_definition->inputs_table, name, iop);
        if(iop == NULL){
            mtic_error("Input %s cannot be found", name);
            return -1;
        }
    } else if (type == OUTPUT_T){
        HASH_FIND_STR(mtic_internal_definition->outputs_table, name, iop);
        if(iop == NULL){
            mtic_error("Output %s cannot be found", name);
            return -1;
        }
    } else if (type == PARAMETER_T){
        HASH_FIND_STR(mtic_internal_definition->params_table, name, iop);
        if(iop == NULL){
            mtic_error("Parameter %s cannot be found", name);
            return -1;
        }
    }else{
        mtic_error("Unknown IOP type %d", type);
        return -1;
    }
    
    return iop->value_type;
}

agent_iop_t *model_findInputByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && mtic_internal_definition != NULL){
        HASH_FIND_STR( mtic_internal_definition->inputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            mtic_error("Input name cannot be NULL or empty");
        }else{
            mtic_error("Definition is NULL");
        }
    }
    return found;
}

agent_iop_t *model_findOutputByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && mtic_internal_definition != NULL){
        HASH_FIND_STR( mtic_internal_definition->outputs_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            mtic_error("Output name cannot be NULL or empty");
        }else{
            mtic_error("Definition is NULL");
        }
    }
    return found;
}

agent_iop_t *model_findParameterByName(const char *name){
    agent_iop_t *found = NULL;
    if(name != NULL && mtic_internal_definition != NULL){
        HASH_FIND_STR( mtic_internal_definition->params_table, name, found );
    }else{
        if (name == NULL || strlen(name) == 0){
            mtic_error("Parameter name cannot be NULL or empty");
        }else{
            mtic_error("Definition is NULL");
        }
    }
    return found;
}

void* model_getValueFor(const char *name, iop_t type){
    agent_iop_t *iop = model_findIopByName((char*) name,type);
    if(iop == NULL){
        mtic_error("%s not found", name);
        return NULL;
    }
    switch (iop->value_type) {
        case INTEGER_T:
            return &iop->value.i;
            break;
        case DOUBLE_T:
            return &iop->value.d;
            break;
        case BOOL_T:
            return &iop->value.b;
            break;
        case STRING_T:
            return iop->value.s;
            break;
        case IMPULSION_T:
            return NULL;
            break;
        case DATA_T:
            return iop->value.data;
            break;
        default:
            mtic_error("Unknown value type for %s", name);
            break;
    }
    return NULL;
}

int mtic_readIOP(const char *name, iop_t type, void **value, long *size){
    agent_iop_t *iop = model_findIopByName((char*) name, type);
    if(iop == NULL){
        mtic_error("%s not found", name);
        return 0;
    }
    if (iop->value_type == IMPULSION_T){
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
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                return iop->value.b;
                break;
                
            case INTEGER_T:
                mtic_warn("Implicit conversion from int to bool for %s", name);
                return (iop->value.i == 0)?false:true;
                break;
                
            case DOUBLE_T:
                mtic_warn("Implicit conversion from double to bool for %s", name);
                return (iop->value.d == 0)?false:true;
                break;
                
            case STRING_T:
                if (strcmp(iop->value.s, "true") == 0){
                    mtic_warn("Implicit conversion from string to bool for %s", name);
                    return true;
                }
                else if (strcmp(iop->value.s, "false") == 0){
                    mtic_warn("Implicit conversion from string to bool for %s", name);
                    return false;
                }else{
                    mtic_warn("Implicit conversion from double to bool for %s (string value is %s and false was returned)", name, iop->value.s);
                    return false;
                }
                break;
                
            default:
                mtic_error("No implicit conversion possible for %s (false was returned)", name);
                return false;
                break;
        }
    }else{
        mtic_error("%s not found", name);
        return false;
    }
}

int model_readIopAsInt (const char *name, iop_t type){
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                mtic_warn("Implicit conversion from bool to int for %s", name);
                return (iop->value.b)?1:0;
                break;
                
            case INTEGER_T:
                return iop->value.i;
                break;
                
            case DOUBLE_T:
                mtic_warn("Implicit conversion from double to int for %s", name);
                if(iop->value.d < 0) {
                    return (int) (iop->value.d - 0.5);
                }else {
                    return (int) (iop->value.d + 0.5);
                }
                break;
                
            case STRING_T:
                mtic_warn("Implicit conversion from string %s to int for %s", iop->value.s, name);
                return atoi(iop->value.s);
                break;
                
            default:
                mtic_error("No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        mtic_error("%s not found", name);
        return 0;
    }
}

double model_readIopAsDouble (const char *name, iop_t type){
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                mtic_warn("Implicit conversion from bool to double for %s", name);
                return (iop->value.b)?1:0;
                break;
                
            case INTEGER_T:
                mtic_warn("Implicit conversion from int to double for %s", name);
                return iop->value.i;
                break;
                
            case DOUBLE_T:
                return iop->value.d;
                break;
                
            case STRING_T:
                mtic_warn("Implicit conversion from string %s to double for %s", iop->value.s, name);
                return atof(iop->value.s);
                break;
                
            default:
                mtic_error("No implicit conversion possible for %s (0 was returned)", name);
                return 0;
                break;
        }
    }else{
        mtic_error("%s not found", name);
        return 0;
    }
}

char *model_readIopAsString (const char *name, iop_t type){
    agent_iop_t *iop = model_findIopByName(name, type);
    if(iop != NULL){
        switch(iop->value_type){
            case STRING_T:
                return strdup(iop->value.s);
                break;
                
            case BOOL_T:
                mtic_warn("Implicit conversion from bool to string for %s", name);
                return iop->value.b ? strdup("true") : strdup("false");
                break;
                
            case INTEGER_T:
                mtic_warn("Implicit conversion from int to string for %s", name);
                return model_intToString(iop->value.i);
                break;
                
            case DOUBLE_T:
                mtic_warn("Implicit conversion from double to string for %s", name);
                return model_doubleToString(iop->value.d);
                break;
                
            default:
                mtic_error("No implicit conversion possible for %s (NULL was returned)", name);
                return NULL;
                break;
        }
    }else{
        mtic_error("%s not found", name);
        return NULL;
    }
}

int model_readIopAsData (const char *name, iop_t type, void **value, long *size){
    agent_iop_t *iop = model_findIopByName((char*) name, type);
    if(iop == NULL){
        mtic_error("%s not found", name);
        *value = NULL;
        *size = 0;
        return 0;
    }
    if(iop->value_type != DATA_T){
        mtic_error("No implicit conversion possible for %s (NULL was returned)", name);
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
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return false;
    }
    HASH_FIND_STR(hash, name, iop);
    if(iop == NULL){
        return false;
    }
    return true;
}

char **model_getIopList(long *nbOfElements, iop_t type){
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return NULL;
    }
    agent_iop_t *hash = NULL;
    switch (type) {
        case INPUT_T:
            hash = mtic_internal_definition->inputs_table;
            break;
        case OUTPUT_T:
            hash = mtic_internal_definition->outputs_table;
            break;
        case PARAMETER_T:
            hash = mtic_internal_definition->params_table;
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
            case INTEGER_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255,"%i",iop->value.i);
                break;
            case DOUBLE_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255,"%lf",iop->value.d);
                break;
            case BOOL_T:
                str_value = calloc(1, 8);
                if(iop->value.b == true){
                    snprintf(str_value, 8, "%s","true");
                } else {
                    snprintf(str_value, 8, "%s","false");
                }
                break;
            case STRING_T:
                str_value = calloc(1, strlen(iop->value.s)+1);
                snprintf(str_value, strlen(iop->value.s)+1, "%s",iop->value.s);
                break;
            case IMPULSION_T:
                break;
            case DATA_T:
                str_value = calloc(1, 256);
                snprintf(str_value, 255, "%s", (char*) iop->value.data);
                break;
            default:
                break;
        }
    }
    return str_value;
}

int model_writeIOP (const char *iopName, iop_t iopType, iopType_t valType, void* value, long size){
    agent_iop_t *iop = model_findIopByName((char*) iopName, iopType);
    if(iop == NULL){
        mtic_error("%s not found for writing", iopName);
        return 0;
    }
    int ret = 1;
    
    //TODO optimize if value is NULL
    switch (valType) {
        case INTEGER_T:{
            switch (iop->value_type) {
                case INTEGER_T:
                    iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:*(int*)(value);
                    mtic_info("set %s to %i", iopName, iop->value.i);
                    break;
                case DOUBLE_T:
                    iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(int*)(value);
                    mtic_info("set %s to %lf", iopName, iop->value.d);
                    break;
                case BOOL_T:
                    iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:((*(int*)(value))?true:false);
                    mtic_info("set %s to %i", iopName, iop->value.b);
                    break;
                case STRING_T:
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
                    iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    mtic_info("set %s to %s (length: %d)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IMPULSION_T:
                    //nothing to do
                    iop->valueSize = 0;
                    mtic_info("set impulsion %s", iopName);
                    break;
                case DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(int));
                    memcpy(iop->value.data, value, sizeof(int));
                    iop->valueSize = sizeof(int);
                    mtic_info("set %s data (length: %d)", iopName, iop->valueSize);
                }
                    break;
                default:
                    mtic_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case DOUBLE_T:{
            switch (iop->value_type) {
                case INTEGER_T:
                    iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:*(int*)(value);
                    mtic_info("set %s to %i", iopName, iop->value.i);
                    break;
                case DOUBLE_T:
                    iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(double*)(value);
                    mtic_info("set %s to %lf", iopName, iop->value.d);
                    break;
                case BOOL_T:
                    iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:((*(int*)(value))?true:false);
                    mtic_info("set %s to %i", iopName, iop->value.b);
                    break;
                case STRING_T:
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
                    iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    mtic_info("set %s to %s (length: %d)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IMPULSION_T:
                    //nothing to do
                    iop->valueSize = 0;
                    mtic_info("set impulsion %s", iopName);
                    break;
                case DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(double));
                    memcpy(iop->value.data, value, sizeof(double));
                    iop->valueSize = sizeof(double);
                    mtic_info("set %s data (length: %d)", iopName, iop->valueSize);
                }
                    break;
                default:
                    mtic_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case BOOL_T:{
            switch (iop->value_type) {
                case INTEGER_T:
                    iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:*(bool*)(value);
                    mtic_info("set %s to %i", iopName, iop->value.i);
                    break;
                case DOUBLE_T:
                    iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:*(bool*)(value);
                    mtic_info("set %s to %lf", iopName, iop->value.d);
                    break;
                case BOOL_T:
                    iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:*(bool*)value;
                    mtic_info("set %s to %i", iopName, iop->value.b);
                    break;
                case STRING_T:
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
                    iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    mtic_info("set %s to %s (length: %d)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IMPULSION_T:
                    //nothing to do
                    iop->valueSize = 0;
                    mtic_info("set impulsion %s", iopName);
                    break;
                case DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, sizeof(bool));
                    memcpy(iop->value.data, value, sizeof(bool));
                    iop->valueSize = sizeof(bool);
                    mtic_info("set %s data (length: %d)", iopName, iop->valueSize);
                }
                    break;
                default:
                    mtic_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case STRING_T:{
            switch (iop->value_type) {
                case INTEGER_T:
                    iop->valueSize = sizeof(int);
                    iop->value.i = (value == NULL)?0:atoi((char*)value);
                    mtic_info("set %s to %i", iopName, iop->value.i);
                    break;
                case DOUBLE_T:
                    iop->valueSize = sizeof(double);
                    iop->value.d = (value == NULL)?0:atof((char*)value);
                    mtic_info("set %s to %lf", iopName, iop->value.d);
                    break;
                case BOOL_T:
                    iop->valueSize = sizeof(bool);
                    iop->value.b = (value == NULL)?false:(atoi((char*)value)?true:false);
                    mtic_info("set %s to %i", iopName, iop->value.b);
                    break;
                case STRING_T:
                {
                    if (iop->value.s != NULL){
                        free(iop->value.s);
                    }
                    if (value == NULL){
                        iop->value.s = strdup("");
                    }else{
                        iop->value.s = strdup((char *)value);
                    }
                    iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
                    mtic_info("set %s to %s (length: %d)", iopName, iop->value.s, iop->valueSize - 1);
                }
                    break;
                case IMPULSION_T:
                    //nothing to do
                    iop->valueSize = 0;
                    mtic_info("set impulsion %s", iopName);
                    break;
                case DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    size_t s = (strlen(iop->value.s) + 1)*sizeof(char);
                    iop->value.data = calloc (1, s);
                    memcpy(iop->value.data, value, s);
                    iop->valueSize = s;
                    mtic_info("set %s data (length: %d)", iopName, s);
                }
                    break;
                default:
                    mtic_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        case IMPULSION_T:{
            //nothing to do
            iop->valueSize = 0;
            mtic_info("set impulsion %s", iopName);
        }
            break;
        case DATA_T:{
            switch (iop->value_type) {
                case INTEGER_T:
                    mtic_warn("Cannot write data into integer IOP %s", iopName);
                    ret = 0;
                    break;
                case DOUBLE_T:
                    mtic_warn("Cannot write data into double IOP %s", iopName);
                    ret = 0;
                    break;
                case BOOL_T:
                    mtic_warn("Cannot write data into boolean IOP %s", iopName);
                    ret = 0;
                    break;
                case STRING_T:
                {
                    mtic_warn("Cannot write data into string IOP %s", iopName);
                    ret = 0;
                }
                    break;
                case IMPULSION_T:
                    //nothing to do
                    iop->valueSize = 0;
                    mtic_info("set impulsion %s", iopName);
                    break;
                case DATA_T:
                {
                    if (iop->value.data != NULL){
                        free(iop->value.data);
                    }
                    iop->value.data = NULL;
                    iop->value.data = calloc (1, size);
                    memcpy(iop->value.data, value, size);
                    iop->valueSize = size;
                    mtic_info("set %s data (length: %d)", iopName, size);
                }
                    break;
                default:
                    mtic_error("%s has an invalid value type %d", iopName, iop->value_type);
                    ret = 0;
                    break;
            }
        }
            break;
        default:
            break;
    }
    
    if (ret){
        model_runObserveCallbacksForIOP(iop, value, size);
    }
    return ret;
}

agent_iop_t * model_findIopByName(const char *name, iop_t type){
    agent_iop_t *found = NULL;
    
    switch (type) {
        case INPUT_T:
            return model_findInputByName(name);
            break;
        case OUTPUT_T:
            return model_findOutputByName(name);
            break;
        case PARAMETER_T:
            return model_findParameterByName(name);
            break;
        default:
            mtic_error("Unknown IOP type %d", type);
            break;
    }
    
    return found;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

// --------------------------------  READ ------------------------------------//

int mtic_readInput(const char *name, void **value, long *size){
    return mtic_readIOP(name, INPUT_T, value, size);
}

int mtic_readOutput(const char *name, void **value, long *size){
    return mtic_readIOP(name, OUTPUT_T, value, size);
}

int mtic_readParameter(const char *name, void **value, long *size){
    return mtic_readIOP(name, PARAMETER_T, value, size);
}

bool mtic_readInputAsBool(const char *name){
    return model_readIopAsBool(name, INPUT_T);
}

int mtic_readInputAsInt(const char *name){
    return model_readIopAsInt(name, INPUT_T);
}

double mtic_readInputAsDouble(const char *name){
    return model_readIopAsDouble(name, INPUT_T);
}

char* mtic_readInputAsString(const char *name){
    return model_readIopAsString(name, INPUT_T);
}

int mtic_readInputAsData(const char *name, void **data, long *size){
    return model_readIopAsData(name, INPUT_T, data, size);
}

bool mtic_readOutputAsBool(const char *name){
    return model_readIopAsBool(name, OUTPUT_T);
}

int mtic_readOutputAsInt(const char *name){
    return model_readIopAsInt(name, OUTPUT_T);
}

double mtic_readOutputAsDouble(const char *name){
    return model_readIopAsDouble(name, OUTPUT_T);
}

char* mtic_readOutputAsString(const char *name){
    return model_readIopAsString(name, OUTPUT_T);
}

int mtic_readOutputAsData(const char *name, void **data, long *size){
    return model_readIopAsData(name, OUTPUT_T, data, size);
}

bool mtic_readParameterAsBool(const char *name){
    return model_readIopAsBool(name, PARAMETER_T);
}

int mtic_readParameterAsInt(const char *name){
    return model_readIopAsInt(name, PARAMETER_T);
}

double mtic_readParameterAsDouble(const char *name){
    return model_readIopAsDouble(name, PARAMETER_T);
}

char* mtic_readParameterAsString(const char *name){
    return model_readIopAsString(name, PARAMETER_T);
}

int mtic_readParameterAsData(const char *name, void **data, long *size){
    return model_readIopAsData(name, PARAMETER_T, data, size);
}

// --------------------------------  WRITE ------------------------------------//

int mtic_writeInput(const char *name, char *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, DATA_T, value, size);
}

int mtic_writeOutput(const char *name, char *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, OUTPUT_T, DATA_T, value, size);
}

int mtic_writeParameter(const char *name, char *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, DATA_T, value, size);
}

int mtic_writeInputAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, BOOL_T, &value, sizeof(bool));
}

int mtic_writeInputAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, INTEGER_T, &value, sizeof(int));
}

int mtic_writeInputAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, DOUBLE_T, &value, sizeof(double));
}

int mtic_writeInputAsString(const char *name, char *value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, STRING_T, value, strlen(value)+1);
}

int mtic_writeInputAsImpulsion(const char *name){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, IMPULSION_T, NULL, 0);
}

int mtic_writeInputAsData(const char *name, void *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, INPUT_T, DATA_T, value, size);
}

int mtic_writeOutputAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, BOOL_T, &value, sizeof(bool));
    network_publishOutput(name);

    return ret;
}

int mtic_writeOutputAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, INTEGER_T, &value, sizeof(int));
    network_publishOutput(name);

    return ret;

}

int mtic_writeOutputAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, DOUBLE_T, &value, sizeof(double));
    network_publishOutput(name);

    return ret;
}

int mtic_writeOutputAsString(const char *name, char *value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, STRING_T, value, strlen(value)+1);
    network_publishOutput(name);

    return ret;
}

int mtic_writeOutputAsImpulsion(const char *name){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, IMPULSION_T, NULL, 0);
    network_publishOutput(name);

    return ret;
}

int mtic_writeOutputAsData(const char *name, void *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    int ret = model_writeIOP(name, OUTPUT_T, DATA_T, value, size);
    network_publishOutput(name);
    
    return ret;
}

int mtic_writeParameterAsBool(const char *name, bool value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, BOOL_T, &value, sizeof(bool));
}

int mtic_writeParameterAsInt(const char *name, int value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, INTEGER_T, &value, sizeof(int));
}

int mtic_writeParameterAsDouble(const char *name, double value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, DOUBLE_T, &value, sizeof(double));
}

int mtic_writeParameterAsString(const char *name, char *value){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, STRING_T, value, strlen(value)+1);
}

int mtic_writeParameterAsData(const char *name, void *value, long size){
    if (name == NULL || strlen(name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_writeIOP(name, PARAMETER_T, DATA_T, value, size);
}

// --------------------------------  INTROSPECTION ------------------------------------//

iopType_t mtic_getTypeForInput(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_error("Input name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, INPUT_T);
}

iopType_t mtic_getTypeForOutput(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_error("Output name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, OUTPUT_T);
}

iopType_t mtic_getTypeForParameter(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_error("Parameter name cannot be NULL or empty");
        return 0;
    }
    return model_getTypeForIOP(name, PARAMETER_T);
}

int mtic_getInputsNumber(){
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return -1;
    }
    return HASH_COUNT(mtic_internal_definition->inputs_table);
}

int mtic_getOutputsNumber(){
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return -1;
    }
    return HASH_COUNT(mtic_internal_definition->outputs_table);
}

int mtic_getParametersNumber(){
    if(mtic_internal_definition == NULL){
        mtic_error("Definition is NULL");
        return -1;
    }
    return HASH_COUNT(mtic_internal_definition->params_table);
}

char ** mtic_getInputsList(long *nbOfElements){
    return model_getIopList(nbOfElements, INPUT_T);
}

char ** mtic_getOutputsList(long *nbOfElements){
    return model_getIopList(nbOfElements, OUTPUT_T);
}

char ** mtic_getParametersList(long *nbOfElements){
    return model_getIopList(nbOfElements, PARAMETER_T);
}

bool mtic_checkInputExistence(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_error("Input name cannot be NULL or empty\n");
        return false;
    }
    return model_checkIOPExistence(name, mtic_internal_definition->inputs_table);
}

bool mtic_checkOutputExistence(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_warn("Output name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(name, mtic_internal_definition->outputs_table);
}

bool mtic_checkParameterExistence(const char *name){
    if((name == NULL) || (strlen(name) == 0)){
        mtic_warn("Parameter name cannot be NULL or empty");
        return false;
    }
    return model_checkIOPExistence(name, mtic_internal_definition->params_table);
}

// --------------------------------  OBSERVE ------------------------------------//

int mtic_observeInput(const char *name, mtic_observeCallback cb, void *myData){
    return model_observe(name, INPUT_T, cb, myData);
}

int mtic_observeOutput(const char *name, mtic_observeCallback cb, void * myData){
    return model_observe(name, OUTPUT_T, cb, myData);
}

int mtic_observeParameter(const char *name, mtic_observeCallback cb, void * myData){
    return model_observe(name, PARAMETER_T, cb, myData);
}

// --------------------------------  MUTE ------------------------------------//


int mtic_muteOutput(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name, OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_warn("Output '%s' not found", name);
        return 0;
    }
    iop->is_muted = true;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "OUTPUT_MUTED %s", name);
    }
    return 1;
}

int mtic_unmuteOutput(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name,OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_warn("Output '%s' not found", name);
        return 0;
    }
    iop->is_muted = false;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "OUTPUT_UNMUTED %s", name);
    }
    return 1;
}

bool mtic_isOutputMuted(const char *name){
    agent_iop_t *iop = model_findIopByName((char*) name,OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_warn("Output '%s' not found", name);
        return 0;
    }
    return iop->is_muted;
}
