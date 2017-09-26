//
//  model.c
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Modified by Mathieu Poirier
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

/**
  * \file ../../src/include/mastic.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "mastic_private.h"
#include "uthash/utlist.h"

void model_setIopValue(agent_iop *iop, void* value, long size){
    if (iop == NULL){
        mtic_debug("model_setIopValue called with NULL iop\n");
        return;
    }
    
    switch (iop->value_type) {
        case INTEGER_T:
            iop->valueSize = sizeof(int);
            iop->value.i = *(int*)(value);
            mtic_debug("set %s to %i\n", iop->name, iop->value.i);
            break;
        case DOUBLE_T:
            iop->valueSize = sizeof(double);
            iop->value.d = *(double*)(value);
            mtic_debug("set %s to %f\n", iop->name, iop->value.d);
            break;
        case BOOL_T:
            iop->valueSize = sizeof(bool);
            iop->value.b = *(bool*)(value);
            mtic_debug("set %s to %i\n", iop->name, iop->value.b);
            break;
        case STRING_T:
        {
            if (iop->value.s != NULL){
                free(iop->value.s);
            }
            if (value == NULL){
                iop->value.s = "";
            }else{
                iop->value.s = strdup(value);
            }
            iop->valueSize = (strlen(iop->value.s) + 1)*sizeof(char);
            mtic_debug("set %s to %s (length: %d)\n", iop->name, iop->value.s, iop->valueSize - 1);
        }
            break;
        case IMPULSION_T:
            //nothing to do
            iop->valueSize = 0;
            mtic_debug("set impulsion %s\n", iop->name);
        break;
        case DATA_T:
        {
            if (iop->value.data != NULL){
                free(iop->value.data);
            }
            iop->value.data = NULL;
            iop->value.data = calloc (1, size);
            memcpy(iop->value.data,value,size);
            iop->valueSize = size;
            mtic_debug("set %s data (length: %d)\n", iop->name, iop->valueSize);
        }
            break;
        default:
            break;
    }
}


int mtic_mute_internal(const char* iop_name, iop_t type)
{
    int result = -1;
    // mtic_get iop object
    agent_iop *iop = model_findIopByName((char*)iop_name,type);
    
    if(iop != NULL)
    {
        // update the is_muted state
        iop->is_muted = true;
        result = 0;
    }
    
    return result;
}

int mtic_unmute_internal(const char* iop_name, iop_t type)
{
    int result = -1;
    // mtic_get iop object
    agent_iop *iop = model_findIopByName((char*)iop_name,type);
    
    if(iop != NULL)
    {
        // update the is_muted state
        iop->is_muted = false;
        result = 0;
    }
    
    return result;
}

int mtic_muteAll()
{
    int result = 0,result_tmp = 0;

    // Go through the agent outpust to mute them
    struct agent_iop *current_iop, *tmp_iop;
    HASH_ITER(hh, mtic_internal_definition->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_mute_internal(current_iop->name,OUTPUT_T);
            // If one one the output has not been muted, we notice it
            if(result_tmp != 0)
            {
                result = result_tmp;
            }
        }
    }
    
    return result;
}

int mtic_unmuteAll()
{
    int result = 0,result_tmp = 0;
    
    // Go through the agent outpust to mute them
    struct agent_iop *current_iop, *tmp_iop;
    HASH_ITER(hh, mtic_internal_definition->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_unmute_internal(current_iop->name,OUTPUT_T);
            // If one one the output has not been unmuted, we notice it
            if(result_tmp != 0)
            {
                result = result_tmp;
            }
        }
    }
    
    return result;
}

char* model_IntToString(const int value)
{
    // Compute the size of allocate for str.
   int length = snprintf( NULL, 0, "%d", value);

   if(length == 0)
   {
       mtic_debug("value double is NULL. Its length is zero.");
       return NULL;
   }

   // Allocate the memory.
   char* str = malloc(length+1);
   // Write the value into str.
   snprintf( str, length + 1, "%d", value);

   return str;
}

char* model_DoubleToString(const double value)
{
    // Compute the size of allocate for str.
    int length = snprintf( NULL, 0, "%fl", value);

    if(length == 0)
    {
        mtic_debug("value double is NULL. Its length is zero.");
        return NULL;
    }
    // Allocate the memory.
    char* str = malloc(length+1);
    // Write the value into str.
    snprintf( str, length + 1, "%lf", value);

    return str;
}

static int model_observe(const char* name, iop_t iopType, mtic_observeCallback cb, void* myData){

    //find the iop
    agent_iop *iop = model_findIopByName((char*) name, iopType);

    // Check if the input has been returned.
    if(iop == NULL){
        mtic_debug("%s : no iop with name %s and type %d", __FUNCTION__, name, iopType);
        return 0;
    }

    //callback not defined
    if(cb == NULL) {
        mtic_debug("%s: the observe callback for %s is null", __FUNCTION__, name);
        return 0;
    }

    mtic_observe_callback_t *new_callback = malloc(sizeof(mtic_observe_callback_t));
    new_callback->callback_ptr = cb;
    new_callback->data = myData;
    DL_APPEND(iop->callbacks, new_callback);

    mtic_debug("OBSERVE iop with name %s and type %d\n", name, iopType);

    return 1;
}

void runObserveCallbacksForIOP(agent_iop *iop, void *value, long valueSize)
{
    mtic_observe_callback_t *cb;
    DL_FOREACH(iop->callbacks, cb){
        cb->callback_ptr(iop->type, iop->name, iop->value_type, value, valueSize, cb->data);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
agent_iop * model_find_iop_by_name_in_definition(const char *name, definition* definition){
    agent_iop *found = NULL;
    
    if(name != NULL && definition != NULL){
        //find the input agent_iop
        HASH_FIND_STR( definition->inputs_table, name, found );
        if(found == NULL){
            // look in the outputs
            HASH_FIND_STR( definition->outputs_table, name, found );
        }else
        {
            return found;
        }
        
        if(found == NULL){
            //find the parameters agent_iop
            HASH_FIND_STR( definition->params_table, name, found );
        }else{
            return found;
        }
        
        if(found == NULL){
            fprintf(stderr, "ERROR : The name of the iop {%s} doesn't exist. \n",name);
            return found;
        }else
        {
            return found;
        }
    }else{
        fprintf(stderr, "ERROR : The name of the IOP is empty. \n");
        return found;
    }
}

agent_iop * model_findIopByName(const char *name, iop_t type){
    agent_iop *found = NULL;
    
    switch (type) {
        case INPUT_T:
            if(name != NULL && mtic_internal_definition != NULL){
                //find the input agent_iop
                HASH_FIND_STR( mtic_internal_definition->inputs_table, name, found );
            }else{
                fprintf(stderr, "ERROR : The name of the input is empty or mtic_internal_definition is NULL\n");
            }
            break;
        case OUTPUT_T:
            if(name != NULL && mtic_internal_definition != NULL){
                //find the input agent_iop
                HASH_FIND_STR( mtic_internal_definition->outputs_table, name, found );
            }else{
                fprintf(stderr, "ERROR : The name of the output is empty or mtic_internal_definition is NULL\n");
            }
            break;
        case PARAMETER_T:
            if(name != NULL && mtic_internal_definition != NULL){
                //find the input agent_iop
                HASH_FIND_STR( mtic_internal_definition->params_table, name, found );
            }else{
                fprintf(stderr, "ERROR : The name of the parameter is empty or mtic_internal_definition is NULL\n");
            }
            break;
        default:
            break;
    }
    
    return found;
}

agent_iop *model_findInputByName(const char *name)
{
    agent_iop *found = NULL;
    
    if(name != NULL && mtic_internal_definition != NULL){
        //find the input agent_iop
        HASH_FIND_STR( mtic_internal_definition->inputs_table, name, found );
    }else{
        fprintf(stderr, "ERROR : The name of the input is empty or mtic_internal_definition is NULL\n");
    }
    
    return found;
}

agent_iop *model_findOutputByName(const char *name)
{
    agent_iop *found = NULL;
    
    if(name != NULL && mtic_internal_definition != NULL){
        //find the input agent_iop
        HASH_FIND_STR( mtic_internal_definition->outputs_table, name, found );
    }else{
        fprintf(stderr, "ERROR : The name of the output is empty or mtic_internal_definition is NULL\n");
    }
    
    return found;
}

agent_iop *model_findParameterByName(const char *name)
{
    agent_iop *found = NULL;
    
    if(name != NULL && mtic_internal_definition != NULL){
        //find the input agent_iop
        HASH_FIND_STR( mtic_internal_definition->params_table, name, found );
    }else{
        fprintf(stderr, "ERROR : The name of the parameter is empty or mtic_internal_definition is NULL\n");
    }
    
    return found;
}

void * model_get(const char *name_iop, iop_t type){
    
    agent_iop *iop = model_findIopByName((char*) name_iop,type);
    
    if(iop == NULL)
        return NULL;
    
    //Return the value corresponding
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
            break;
    }
    
    return NULL;
}



////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

/**
 *  \defgroup readfct Agent's inputs/outputs/parameters: Read functions
 *
 */

/**
 * \fn void mtic_readInput(const char *name, void *value, long *size)
 * \ingroup readfct
 * \brief Find the Agent's input by name and return the input value.
 * \param name is the name of the input to read as it has been defined in the definition.
 * \param value is the return value of the input depending on the type of input
 * \param size is the size of the value (only use for data input)
 * \return Return 1 if ok else 0.
 */
int mtic_readInput(const char *name, void **value, long *size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        *(int*) value = mtic_readInputAsInt(name);
        break;
    case DOUBLE_T:
        *(double*) value = mtic_readInputAsDouble(name);
        break;
    case BOOL_T:
        *(bool*) value = mtic_readInputAsBool(name);
        break;
    case STRING_T:
        *value = mtic_readInputAsString(name);
        break;
    case IMPULSION_T:
        mtic_debug("%s: Agent's input %s is a impulsion and has nothing to read. It use the callback\n.", __FUNCTION__,  name);
        return 0;
        break;
    case DATA_T:
        mtic_readInputAsData(name, value, size);
        break;
    default:
        mtic_debug("%s: Agent's input %s has not a known type\n.", __FUNCTION__,  name);
        return 0;
        break;

    }

    return 1;
}

/**
 * \fn void mtic_readOutput(const char *name, void *value, long *size)
 * \ingroup readfct
 * \brief Find the Agent's output by name and return the output value.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \param value is the return value of the output depending on the type of output
 * \param size is the size of the value (only use for data output)
 * \return Return 1 if ok else 0.
 */
int mtic_readOutput(const char *name, void **value, long *size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        *(int*) value = mtic_readOutputAsInt(name);
        break;
    case DOUBLE_T:
        *(double*) value = mtic_readOutputAsDouble(name);
        break;
    case BOOL_T:
        *(bool*) value = mtic_readOutputAsBool(name);
        break;
    case STRING_T:
        *value = mtic_readOutputAsString(name);
        break;
    case IMPULSION_T:
        mtic_debug("%s: Agent's output %s is a impulsion and has nothing to read. It use the callback\n.", __FUNCTION__,  name);
        return 0;
        break;
    case DATA_T:
        mtic_readOutputAsData(name, value, size);
        break;
    default:
        mtic_debug("%s: Agent's output %s has not a known type\n.", __FUNCTION__,  name);
        return 0;
        break;

    }

    return 1;
}

/**
 * \fn void mtic_readParameter(const char *name, void *value, long *size)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and return the parameter value.
 * \param name is the name of the parameter to read as it has been defined in the definition.
 * \param value is the return value of the parameter depending on the typeof parameter
 * \param size is the size of the value (only use for data parameter)
 * \return Return 1 if ok else 0.
 */
int mtic_readParameter(const char *name, void **value, long *size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        *(int*) value = mtic_readParameterAsInt(name);
        break;
    case DOUBLE_T:
        *(double*) value = mtic_readParameterAsDouble(name);
        break;
    case BOOL_T:
        *(bool*) value = mtic_readParameterAsBool(name);
        break;
    case STRING_T:
        *value = mtic_readParameterAsString(name);
        break;
    case IMPULSION_T:
        mtic_debug("%s: Agent's parameter %s is a impulsion and has nothing to read. It use the callback\n.", __FUNCTION__,  name);
        return 0;
        break;
    case DATA_T:
        mtic_readParameterAsData(name, value, size);
        break;
    default:
        mtic_debug("%s: Agent's parameter %s has not a known type\n.", __FUNCTION__,  name);
        return 0;
        break;

    }

    return 1;
}

/**
 * \fn mtic_readInputAsBool(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's input by name and return the input value as a Boolean.
 * \param name is the name of the input to read as it has been defined in the definition.
 * \return Return the input value as true or false.
 */
bool mtic_readInputAsBool(const char *name){

    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the input has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                return iop->value.b;
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to bool.
                mtic_debug("mtic_readInputAsBool : Implicit conversion from int to bool for the input {%s}.\n", name);
                return (bool) iop->value.i;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to bool.
                mtic_debug("mtic_readInputAsBool : Implicit conversion from double to bool for the input {%s}.\n", name);
                return (bool) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to bool.
                // Test first if the input value is {true}, then {false}. Finally if the two previous test fail the function returns {false}.
                if (!strcmp(iop->value.s, "true")){
                    mtic_debug("mtic_readInputAsBool : Implicit conversion from string to bool for the input {%s}.\n", name);
                    return true;
                }
                else if (!strcmp(iop->value.s, "false")){
                    mtic_debug("mtic_readInputAsBool : Implicit conversion from string to bool for the input {%s}.\n", name);
                    return false;
                }
                else{
                    mtic_debug("mtic_readInputAsBool : Implicit conversion fails to attempt for the input {%s}. String value is different from {true , false}.\n", name);
                    return false;
                }
                break;

            default:
                //Handle the case: the input cannot be handled.
                mtic_debug("mtic_readInputAsBool : Agent's input {%s} cannot be returned as a Boolean! By default {false} is returned.\n", name);
                return false;
                break;
        }
    }
    else{
        //Handle the case: the input is not found.
        mtic_debug("mtic_readInputAsBool : Any Agent's input {%s} cannot be found. By default {false} is returned.\n", name);
        return false;
    }
}

/**
 * \fn mtic_readInputAsInt(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's input by name and return the input value as an integer.
 * \param name is the name of the input to read as it has been defined in the definition.
 * \return Return the input value as an integer.
 */
int mtic_readInputAsInt(const char *name){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);
    int value = 0;
    int test = 0;

    // Check if the input has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case INTEGER_T:
                return iop->value.i;
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to int.
                mtic_debug("mtic_readInputAsInt : Implicit conversion from bool to int for the input {%s}.\n", name);
                return (int) iop->value.b;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to int.
                mtic_debug("mtic_readInputAsInt : Implicit conversion from double to int for the input {%s}.\n", name);
                return (int) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to int.
                //Try to convert to int (1 expected value)
                test = sscanf(iop->value.s, "%d", &value);

                // Check if one value is returned.
                if(test == 1){
                    mtic_debug("mtic_readInputAsInt : Implicit conversion from string to int for the input {%s}.\n", name);
                    return value;
                }
                else{
                    mtic_debug("mtic_readInputAsInt : Implicit conversion fails to attempt for the input {%s}. String value is different from an integer.\n", name);
                    return value;
                }
                break;

            default:
                //Handle the case: the input cannot be handled.
                mtic_debug("mtic_readInputAsInt : Agent's input {%s} cannot be returned as an integer! By default {0} is returned.\n", name);
                return 0;
                break;
        }
    }
    else{
        //Handle the case: the input is not found.
        mtic_debug("mtic_readInputAsInt : Any Agent's {%s} input cannot be found. By default {0} is returned.\n", name);
        return 0;
    }
}

/**
 * \fn mtic_readInputAsDouble(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's input by name and return the input value as a double.
 * \param name is the name of the input to read as it has been defined in the definition.
 * \return Return the input value as double.
 */
double mtic_readInputAsDouble(const char *name){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);
    double value = 0.0;
    int test =0;

    // Check if the input has been returned.
    if(iop != NULL){
         switch(iop->value_type){
             case DOUBLE_T:
                 return iop->value.d;
                 break;

             case BOOL_T:
                 //Handle the case: An implicit conversion can be done from bool to double.
                 mtic_debug("mtic_readInputAsDouble : Implicit conversion from bool to double for the input {%s}.\n", name);
                 return (double) iop->value.b;
                 break;

             case INTEGER_T:
                 //Handle the case: An implicit conversion can be done from int to double.
                 mtic_debug("mtic_readInputAsDouble : Implicit conversion from int to double for the input {%s}.\n", name);
                 return (double) iop->value.i;
                 break;

             case STRING_T:
                 //Handle the case: An implicit conversion can be done from string to double.
                 //Try to convert to double (1 expected value)
                 test = sscanf(iop->value.s, "%lf", &value);

                 // Check if one value is returned.
                 if(test == 1){
                     mtic_debug("mtic_readInputAsDouble : Implicit conversion from string to double for the input {%s}.\n", name);
                     return value;
                 }
                 else{
                     mtic_debug("mtic_readInputAsDouble : Implicit conversion fails to attempt for the input {%s}. String value is different from a double.\n", name);
                     return value;
                 }
                 break;

             default:
                 //Handle the case: the input cannot be handled.
                 mtic_debug("mtic_readInputAsDouble : Agent's input {%s} cannot be returned as a double! By default {0.0} is returned.\n", name);
                 return 0.0;
                 break;
         }
     }
     else{
         //Handle the case: the input is not found.
         mtic_debug("mtic_readInputAsDouble : Agent's input {%s} cannot be found. By default {0.0} is returned.\n", name);
         return 0.0;
     }
}

/**
 * \fn mtic_readInputAsString(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's input by name and return the input value as a string.
 * \warning Allocating memory that must be free after use.
 * \param name is the name of the input to read as it has been defined in the definition.
 * \return Return the input value as a string.
 */
char* mtic_readInputAsString(const char *name){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the input has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case STRING_T:
                return strdup(iop->value.s);
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to string.
                mtic_debug("mtic_readInputAsString : Implicit conversion from bool to string for the input {%s}.", name);
                return iop->value.b ? strdup("true") : strdup("false");
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to string.
                mtic_debug("mtic_readInputAsString : Implicit conversion from int to string for the input {%s}.", name);
                return model_IntToString(iop->value.i);
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to string.
                mtic_debug("mtic_readInputAsString : Implicit conversion from double to string for the input {%s}.", name);
                return model_DoubleToString(iop->value.d);
                break;

            default:
                //Handle the case: the input cannot be handled.
                mtic_debug("mtic_readInputAsDouble : Agent's input {%s} cannot be return as a string! By default {NULL} is return.", name);
                return NULL;
                break;
        }
    }
    else{
        //Handle the case: the input is not found.
        mtic_debug("mtic_readInputAsString : Agent's input {%s} cannot be found. By default {NULL} is returned.", name);
        return NULL;
    }
}

/**
 * \fn int mtic_readInputAsData(const char *name, void *data, long *size)
 * \ingroup readfct
 * \brief Find the Agent's input by name and get the pointer on the data.
 * \warning The input as to be data type
 * \warning Allocating memory before calling this function that must be free after use.
 * \param name The input's name
 * \param data The pointer on the struct to get
 * \param size The size of the data read
 * \return Return 1 if it is OK or 0 if not.
 */
int mtic_readInputAsData(const char *name, void **data, long *size){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's input '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Get the pointer on the structure data
    void * value = model_get(name,INPUT_T);

    //get size
    *size = iop->valueSize;

    //Copy the data
    *data = calloc(1, iop->valueSize);
    memcpy(*data, value, *size);

    return 1;
}

/**
 * \fn mtic_readOutputAsBool(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's output by name and return the output value as a Boolean.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \return Return the output value as true or false.
 */
bool mtic_readOutputAsBool(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);

    // Check if the output has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                return iop->value.b;
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to bool.
                mtic_debug("mtic_readOutputAsBool : Implicit conversion from int to bool for the output {%s}.", name);
                return (bool) iop->value.i;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to bool.
                mtic_debug("mtic_readOutputAsBool : Implicit conversion from double to bool for the output {%s}.", name);
                return (bool) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to bool.
                // Test first if the output value is {true}, then {false}. Finally if the two previous test fail the function returns {false}.
                if (!strcmp(iop->value.s, "true")){
                    mtic_debug("mtic_readOutputAsBool : Implicit conversion from string to bool for the output {%s}.", name);
                    return true;
                }
                else if (!strcmp(iop->value.s, "false")){
                    mtic_debug("mtic_readOutputAsBool : Implicit conversion from string to bool for the output {%s}.", name);
                    return false;
                }
                else{
                    mtic_debug("mtic_readOutputAsBool : Implicit conversion fails to attempt for the output {%s}. String value is different from {true , false}.", name);
                    return false;
                }
                break;

            default:
                //Handle the case: the output cannot be handled.
                mtic_debug("mtic_readOutputAsBool : Agent's output {%s} cannot be returned as a Boolean! By default {false} is returned.", name);
                return false;
                break;
        }
    }
    else{
        //Handle the case: the output is not found.
        mtic_debug("mtic_readOutputAsBool : Any Agent's output {%s} has been returned", name);
        return false;
    }
}

/**
 * \fn mtic_readOutputAsInt(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's output by name and return the output value as an integer.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \return Return the output value as an integer.
 */
int mtic_readOutputAsInt(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);
    int value = 0;
    int test =0;

    // Check if the output has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case INTEGER_T:
                return iop->value.i;
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to int.
                mtic_debug("mtic_readOutputAsInt : Implicit conversion from bool to int for the output {%s}.", name);
                return (int) iop->value.b;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to int.
                mtic_debug("mtic_readOutputAsInt : Implicit conversion from double to int for the output {%s}.", name);
                return (int) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to int.
                //Try to convert to int (1 expected value)
                test = sscanf(iop->value.s, "%d", &value);

                // Check if one value is returned.
                if(test == 1){
                    mtic_debug("mtic_readOutputAsInt : Implicit conversion from string to int for the output {%s}.", name);
                    return value;
                }
                else{
                    mtic_debug("mtic_readOutputAsInt : Implicit conversion fails to attempt for the output {%s}. String value is different from an integer.", name);
                    return value;
                }
                break;

            default:
                //Handle the case: the output cannot be handled.
                mtic_debug("mtic_readOutputAsInt : Agent's output {%s} cannot be returned as an integer! By default {0} is returned.", name);
                return 0;
                break;
        }
    }
    else{
        //Handle the case: the output is not found.
        mtic_debug("mtic_readOutputAsInt : Any Agent's output {%s} has been returned\n", name);
        return 0;
    }
}

/**
 * \fn mtic_readOutputAsDouble(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's output by name and return the output value as a double.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \return Return the output value as a double.
 */
double mtic_readOutputAsDouble(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);
    double value = 0.0;
    int test = 0;

    // Check if the output has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case DOUBLE_T:
                return iop->value.d;
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to double.
                mtic_debug("mtic_readOutputAsDouble : Implicit conversion from bool to double for the output {%s}.", name);
                return (double) iop->value.b;
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to double.
                mtic_debug("mtic_readOutputAsDouble : Implicit conversion from int to double for the output {%s}.", name);
                return (double) iop->value.i;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to int.

                //Try to convert to double (1 expected value)
                test = sscanf(iop->value.s, "%lf", &value);

                // Check if one value is returned.
                if(test == 1){
                    mtic_debug("mtic_readOutputAsDouble : Implicit conversion from string to double for the output {%s}.", name);
                    return value;
                }
                else{
                    mtic_debug("mtic_readOutputAsDouble : Implicit conversion fails to attempt for the output {%s}. String value is different from a double.", name);
                    return value;
                }
                break;

            default:
                //Handle the case: the output cannot be handled.
                mtic_debug("mtic_readOutputAsDouble : Agent's output {%s} cannot be returned as a double! By default {0.0} is returned.", name);
                return 0.0;
                break;
        }
    }
    else{
        //Handle the case: the output is not found.
        mtic_debug("mtic_readOutputAsDouble : Any Agent's output {%s} has been returned\n", name);
        return 0.0;
    }
}

/**
 * \fn mtic_readOutputAsString(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's output by name and return the output value as a string.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \return Return the output value as a string.
 * \warning  Allocate memory that must be freed by the user.
 */
char* mtic_readOutputAsString(const char *name){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);

    // Check if the output has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case STRING_T:
                return strdup(iop->value.s);
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to string.
                mtic_debug("mtic_readOutputAsString : Implicit conversion from bool to string for the output {%s}.", name);
                return iop->value.b ? strdup("true") : strdup("false");
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to string.
                mtic_debug("mtic_readOutputAsString : Implicit conversion from int to string for the output {%s}.", name);
                return model_IntToString(iop->value.i);
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to string.
                mtic_debug("mtic_readOutputAsString : Implicit conversion from double to string for the output {%s}.", name);
                return model_DoubleToString(iop->value.d);
                break;

            default:
                //Handle the case: the output cannot be handled.
                mtic_debug("mtic_readOutputAsString : Agent's output {%s} cannot be returned as a string! By default {NULL} is return.", name);
                return NULL;
                break;
        }
    }
    else{
        //Handle the case: the output is not found.
        mtic_debug("mtic_readOutputAsString : Agent's output {%s} cannot be found", name);
        return NULL;
    }
}
/**
 * \fn int mtic_readOutputAsData(const char *name, void *data, long *size)
 * \ingroup readfct
 * \brief Find the Agent's output by name and get the pointer on the data.
 * \warning The output as to be data type
 * \warning Allocating memory before calling this function that must be free after use.
 * \param name The output's name
 * \param data The pointer on the struct to get
 * \param size The size of the data read
 * \return Return 1 if it is OK or 0 if not.
 */
int mtic_readOutputAsData(const char *name, void **data, long *size){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's output '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Get the pointer on the structure data
    void * value = model_get(name,OUTPUT_T);

    //get size
    *size = iop->valueSize;

    //Copy the data
    *data = calloc(1, iop->valueSize);
    memcpy(*data, value, *size);

    return 1;
}
/**
 * \fn mtic_readParameterAsBool(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and return the output value as a Boolean.
 * \param name is the name of the parameter to read as it has been defined in the definition.
 * \return Return the parameter value as true or false.
 */
bool mtic_readParameterAsBool(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the parameter has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case BOOL_T:
                return iop->value.b;
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to bool.
                mtic_debug("mtic_readParameterAsBool : Implicit conversion from int to bool for the output {%s}.", name);
                return (bool) iop->value.i;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to bool.
                mtic_debug("mtic_readParameterAsBool : Implicit conversion from double to bool for the parameter {%s}.", name);
                return (bool) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to bool.
                // Test first if the output value is {true}, then {false}. Finally if the two previous test fail the function returns {false}.
                if (!strcmp(iop->value.s, "true")){
                    mtic_debug("mtic_readParameterAsBool : Implicit conversion from string to bool for the parameter {%s}.", name);
                    return true;
                }
                else if (!strcmp(iop->value.s, "false")){
                    mtic_debug("mtic_readParameterAsBool : Implicit conversion from string to bool for the parameter {%s}.", name);
                    return false;
                }
                else{
                    mtic_debug("mtic_readParameterAsBool : Implicit conversion fails to attempt for the parameter {%s}. String value is different from {true , false}.", name);
                    return false;
                }
                break;

            default:
                //Handle the case: the parameter cannot be handled.
                mtic_debug("mtic_readParameterAsBool : Agent's parameter {%s} cannot be returned as a Boolean! By default {false} is returned.", name);
                return false;
                break;
        }
    }
    else{
        //Handle the case: the parameter is not found.
        mtic_debug("mtic_readParameterAsBool : Any Agent's parameter {%s} has been returned", name);
        return false;
    }
}

/**
 * \fn mtic_readParameterAsInt(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and return the parameter value as an integer.
 * \param name is the name of the parameter to read as it has been defined in the definition.
 * \return Return the parameter value as an integer.
 */
int mtic_readParameterAsInt(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);
    int value = 0;
    int test =0;

    // Check if the parameter has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case INTEGER_T:
                return iop->value.i;
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to int.
                mtic_debug("mtic_readParameterAsInt : Implicit conversion from bool to int for the parameter {%s}.", name);
                return (int) iop->value.b;
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to int.
                mtic_debug("mtic_readParameterAsInt : Implicit conversion from double to int for the parameter {%s}.", name);
                return (int) iop->value.d;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to int.
                //Try to convert to int (1 expected value)
                test = sscanf(iop->value.s, "%d", &value);

                // Check if one value is returned.
                if(test == 1){
                    mtic_debug("mtic_readParameterAsInt : Implicit conversion from string to int for the parameter {%s}.", name);
                    return value;
                }
                else{
                    mtic_debug("mtic_readParameterAsInt : Implicit conversion fails to attempt for the parameter {%s}. String value is different from an integer.", name);
                    return value;
                }
                break;

            default:
                //Handle the case: the parameter cannot be handled.
                mtic_debug("mtic_readParameterAsInt : Agent's parameter {%s} cannot be returned as an integer! By default {0} is returned.", name);
                return 0;
                break;
        }
    }
    else{
        //Handle the case: the parameter is not found.
        mtic_debug("mtic_readParameterAsInt : Any Agent's parameter {%s} has been returned", name);
        return 0;
    }
}

/**
 * \fn mtic_readParameterAsDouble(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and return the output value as a double.
 * \param name is the name of the output to read as it has been defined in the definition.
 * \return Return the parameter value as a double.
 */
double mtic_readParameterAsDouble(const char *name){
    //Get the pointer IOP Agent selected by name.
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);
    double value = 0.0;
    int test = 0;

    // Check if the parameter has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case DOUBLE_T:
                return iop->value.d;
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to double.
                mtic_debug("mtic_readParameterAsDouble : Implicit conversion from bool to double for the parameter {%s}.", name);
                return (double) iop->value.b;
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to double.
                mtic_debug("mtic_readParameterAsDouble : Implicit conversion from int to double for the parameter {%s}.", name);
                return (double) iop->value.i;
                break;

            case STRING_T:
                //Handle the case: An implicit conversion can be done from string to int.
                //Try to convert to double (1 expected value)
                test = sscanf(iop->value.s, "%lf", &value);

                // Check if one value is returned.
                if(test == 1){
                    mtic_debug("mtic_readParameterAsDouble : Implicit conversion from string to double for the parameter {%s}.", name);
                    return value;
                }
                else{
                    mtic_debug("mtic_readParameterAsDouble : Implicit conversion fails to attempt for the parameter {%s}. String value is different from a double.", name);
                    return value;
                }
                break;

            default:
                //Handle the case: the parameter cannot be handled.
                mtic_debug("mtic_readParameterAsDouble : Agent's parameter {%s} cannot be returned as a double! By default {0.0} is returned.", name);
                return 0.0;
                break;
        }
    }
    else{
        //Handle the case: the parameter is not found.
        mtic_debug("mtic_readParameterAsDouble : Any Agent's parameter {%s} has been returned", name);
        return 0.0;
    }
}

/**
 * \fn mtic_readParameterAsString(const char *name)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and return the output value as a string.
 *        WARNING: Allocating memory that must be free after use.
 * \param name is the name of the parameter to read as it has been defined in the definition.
 * \return Return the parameter value as a string.
 */
char* mtic_readParameterAsString(const char *name){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the parameter has been returned.
    if(iop != NULL){
        switch(iop->value_type){
            case STRING_T:
                return strdup(iop->value.s);
                break;

            case BOOL_T:
                //Handle the case: An implicit conversion can be done from bool to string.
                mtic_debug("mtic_readParameterAsString : Implicit conversion from bool to string for the parameter {%s}.", name);
                return iop->value.b ? strdup("true") : strdup("false");
                break;

            case INTEGER_T:
                //Handle the case: An implicit conversion can be done from int to string.
                mtic_debug("mtic_readParameterAsString : Implicit conversion from int to string for the parameter {%s}.", name);
                return model_IntToString(iop->value.i);
                break;

            case DOUBLE_T:
                //Handle the case: An implicit conversion can be done from double to string.
                mtic_debug("mtic_readParameterAsString : Implicit conversion from double to string for the parameter {%s}.", name);
                return model_DoubleToString(iop->value.d);
                break;

            default:
                //Handle the case: the parameter cannot be handled.
                mtic_debug("mtic_readParameterAsString : Agent's parameter {%s} cannot be returned as a string! By default {NULL} is return.", name);;
                return NULL;
                break;
        }
    }
    else{
        //Handle the case: the input is not found.
        mtic_debug("mtic_readParameterAsString : Agent's parameter {%s} cannot be found", name);
        return NULL;
    }
}
/**
 * \fn int mtic_readParameterAsData(const char *name, void *data, long *size)
 * \ingroup readfct
 * \brief Find the Agent's parameter by name and get the pointer on the data.
 * \warning The parameter as to be data type
 * \warning Allocating memory before calling this function that must be free after use.
 * \param name The parameter's name
 * \param data The pointer on the struct to get
 * \param size The size of the data read
 * \return Return 1 if it is OK or 0 if not.
 */
int mtic_readParameterAsData(const char *name, void **data, long *size){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's parameter '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Get the pointer on the structure data
    void * value = model_get(name,PARAMETER_T);

    //get size
    *size = iop->valueSize;

    //Copy the data
    *data = calloc(1, iop->valueSize);
    memcpy(*data, value, *size);

    return 1;
}

// --------------------------------  WRITE ------------------------------------//


/**
 *  \defgroup writefct Agent's inputs/outputs/parameters: Write functions
 *
 */

/**
 * \fn int mtic_writeInput(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value into an agent's input.
 *
 * \param name is the name of the input to write
 * \param value is the value to write in the input in a string format
 * \param size is the size of the value only for data type
 * \return 1 if ok else 0
 */
int mtic_writeInput(const char *name, char *value, long size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);
    int ret = 0;

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        {
            // string convertion to integer
            int int_value = atoi(value);

            ret = mtic_writeInputAsInt(name, int_value);
            break;
        }
    case DOUBLE_T:
        {
            // string convertion to double
            double dbl_value = atof(value);

            ret = mtic_writeInputAsDouble(name, dbl_value);
            break;
        }
    case BOOL_T:
        {
            // string convertion to bool
            bool bool_value = false;
            if(strncmp((char*)value,"true",strlen("true")) == 0)
            {
                bool_value = true;
            }

            ret = mtic_writeInputAsBool(name, bool_value);
            break;
        }
    case STRING_T:
        ret = mtic_writeInputAsString(name, value);
        break;
    case IMPULSION_T:
        ret = mtic_writeInputAsImpulsion(name);
        break;
    case DATA_T:
        ret = mtic_writeInputAsData(name, value, size);
        break;
    default:
        mtic_debug("%s: Agent's input %s has not a known type\n.", __FUNCTION__,  name);
        break;

    }

    return ret;
}

/**
 * \fn int mtic_writeOutput(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value into an agent's output.
 *
 * \param name is the name of the output to write
 * \param value is the value to write in the output in a string format
 * \param size is the size of the value only for data type
 * \return 1 if ok else 0
 */
int mtic_writeOutput(const char *name, char *value, long size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);
    int ret = 0;

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        {
            // string convertion to integer
            int int_value = atoi(value);

            ret = mtic_writeOutputAsInt(name, int_value);
            break;
        }
    case DOUBLE_T:
        {
            // string convertion to double
            double dbl_value = atof(value);

            ret = mtic_writeOutputAsDouble(name, dbl_value);
            break;
        }
    case BOOL_T:
        {
            // string convertion to bool
            bool bool_value = false;
            if(strncmp(value,"true",strlen("true")) == 0)
            {
                bool_value = true;
            }

            ret = mtic_writeOutputAsBool(name, bool_value);
            break;
        }
    case STRING_T:
        ret = mtic_writeOutputAsString(name, value);
        break;
    case IMPULSION_T:
        ret = mtic_writeOutputAsImpulsion(name);
        break;
    case DATA_T:
        ret = mtic_writeOutputAsData(name, value, size);
        break;
    default:
        mtic_debug("%s: Agent's output %s has not a known type\n.", __FUNCTION__,  name);
        break;

    }

    return ret;
}

/**
 * \fn int mtic_writeParameter(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value into an agent's parameter.
 *
 * \param name is the name of the parameter to write
 * \param value is the value to write in the parameter in a string format
 * \param size is the size of the value only for data type
 * \return 1 if ok else 0
 */
int mtic_writeParameter(const char *name, char *value, long size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);
    int ret = 0;

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter %s cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    switch (iop->value_type){
    case INTEGER_T:
        {
            // string convertion to integer
            int int_value = atoi(value);

            ret = mtic_writeParameterAsInt(name, int_value);
            break;
        }
    case DOUBLE_T:
        {
            // string convertion to double
            double dbl_value = atof(value);

            ret = mtic_writeParameterAsDouble(name, dbl_value);
            break;
        }
    case BOOL_T:
        {
            // string convertion to bool
            bool bool_value = false;
            if(strncmp(value,"true",strlen("true")) == 0)
            {
                bool_value = true;
            }

            ret = mtic_writeParameterAsBool(name, bool_value);
            break;
        }
    case STRING_T:
        ret = mtic_writeParameterAsString(name, value);
        break;
    case DATA_T:
        ret = mtic_writeParameterAsData(name, value, size);
        break;
    case IMPULSION_T:
    default:
        mtic_debug("%s: Agent's parameter %s has not a known type\n.", __FUNCTION__,  name);
        break;

    }

    return ret;
}

/**
 * \fn int mtic_writeInputAsBool(const char *name, bool value)
 * \ingroup writefct
 * \brief write a value as bool into an agent's input.
 *
 * \param name is the name of the input bool to write
 * \param value is a bool to write in the input
 * \return 1 if ok else 0
 */
int mtic_writeInputAsBool(const char *name, bool value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a bool.
    if(iop->value_type != BOOL_T){
        mtic_debug("%s: Agent's input '%s' is not a bool", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(bool));

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(bool));

    return 1;

}

/**
 * \fn int mtic_writeInputAsInt(const char *name, int value)
 * \ingroup writefct
 * \brief write a value as integer into an agent's input.
 *
 * \param name is the name of the input integer to write
 * \param value is an integer to write in the input
 * \return 1 if ok else 0
 */
int mtic_writeInputAsInt(const char *name, int value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an integer.
    if(iop->value_type != INTEGER_T){
        mtic_debug("%s: Agent's input '%s' is not a integer", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(int));

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(int));
    
    return 1;
}

/**
 * \fn int mtic_writeInputAsDouble(const char *name, double value)
 * \ingroup writefct
 * \brief write a value as double into an agent's input.
 *
 * \param name is the name of the input double to write
 * \param value is a double to write in the input
 * \return 1 if ok else 0
 */
int mtic_writeInputAsDouble(const char *name, double value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a double.
    if(iop->value_type != DOUBLE_T){
        mtic_debug("%s: Agent's input '%s' is not a double\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(double));

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(double));
    
    return 1;
}

/**
 * \fn int mtic_writeInputAsString(const char *name, char *value)
 * \ingroup writefct
 * \brief write a value as string (char*) into an agent's input.
 *
 * \param name is the name of the input string to write
 * \param value is a string (char*) to write in the input
 * \return 1 if ok else 0
 */
int mtic_writeInputAsString(const char *name, char *value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a string.
    if(iop->value_type != STRING_T){
        mtic_debug("%s: Agent's input '%s' is not a string\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) value, (strlen(value)+1)*sizeof(char));

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, (strlen(value)+1)*sizeof(char));
    
    return 1;
}

/**
 * \fn int mtic_writeInputAsImpulsion(const char *name)
 * \ingroup writefct
 * \brief write an impulsion into an agent's input.
 *
 * \param name is the name of the input impulsion to write
 * \return 1 if ok else 0
 */
int mtic_writeInputAsImpulsion(const char *name){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != IMPULSION_T){
        mtic_debug("%s: Agent's input '%s' is not an impulsion\n", __FUNCTION__,  name);
        return 0;
    }

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, NULL, 0);

    return 1;
}

/**
 * \fn int mtic_writeInputAsData(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value as data into an agent's input.
 *
 * \param name is the name of the input data to write
 * \param value is a data to write in the input
 * \param size the size of the value
 * \return 1 if ok else 0
 */
int mtic_writeInputAsData(const char *name, void *value, long size){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,INPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's input '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's input '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Update the value in the definition
    model_setIopValue(iop,value,size);

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, value, size);

    return 1;
}

/**
 * \fn int mtic_writeOutputAsBool(const char *name, bool value)
 * \ingroup writefct
 * \brief write a value as bool into an agent's output.
 *
 * \param name is the name of the output bool to write.
 * \param value is a bool to write in the output.
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsBool(const char *name, bool value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a bool.
    if(iop->value_type != BOOL_T){
        mtic_debug("%s: Agent's output '%s' is not a bool\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(bool));

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(bool));

    // iop is output : publish
    network_publishOutput(name);

    return 1;
}

/**
 * \fn int mtic_writeOutputAsInt(const char *name, int value)
 * \ingroup writefct
 * \brief write a value as integer into an agent's output.
 *
 * \param name is the name of the output integer to write.
 * \param value is an integer to write in the output.
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsInt(const char *name, int value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a integer.
    if(iop->value_type != INTEGER_T){
        mtic_debug("%s: Agent's output '%s' is not a integer\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(int));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(int));

    // iop is output : publish
    network_publishOutput(name);

    return 1;

}

/**
 * \fn int mtic_writeOutputAsDouble(const char *name, double value)
 * \ingroup writefct
 * \brief write a value as double into an agent's output.
 *
 * \param name is the name of the output double to write.
 * \param value is a double to write in the output.
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsDouble(const char *name, double value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a double.
    if(iop->value_type != DOUBLE_T){
        mtic_debug("%s: Agent's output '%s' is not a double\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(double));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(double));

    // iop is output : publish
    network_publishOutput(name);

    return 1;
}

/**
 * \fn int mtic_writeOutputAsString(const char *name, char *value)
 * \ingroup writefct
 * \brief write a value as string (char*) into an agent's output.
 *
 * \param name is the name of the output string to write.
 * \param value is a string (char*) to write in the output.
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsString(const char *name, char *value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a string.
    if(iop->value_type != STRING_T){
        mtic_debug("%s: Agent's output '%s' is not a string\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) value, (strlen(value)+1)*sizeof(char));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, (strlen(value)+1)*sizeof(char));

    // iop is output : publish
    network_publishOutput(name);

    return 1;
}

/**
 * \fn int mtic_writeOutputAsImpulsion(const char *name)
 * \ingroup writefct
 * \brief write an impulsion into an agent's output.
 *
 * \param name is the name of the output impulsion to write
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsImpulsion(const char *name){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != IMPULSION_T){
        mtic_debug("%s: Agent's output '%s' is not a impulsion\n", __FUNCTION__,  name);
        return 0;
    }

    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, NULL, 0);

    // iop is output : publish
    network_publishOutput(name);

    return 1;
}

/**
 * \fn int mtic_writeOutputAsData(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value as data into an agent's output.
 *
 * \param name is the name of the output data to write
 * \param value is a data to write in the output
 * \param size the size of the value
 * \return 1 if ok else 0
 */
int mtic_writeOutputAsData(const char *name, void *value, long size){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's output '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's output '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Update the value in the definition
    model_setIopValue(iop,value,size);
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, value, size);

    // iop is output : publish
    network_publishOutput(name);
    
    return 1;
}

/**
 * \fn int mtic_writeParameterAsBool(const char *name, bool value)
 * \ingroup writefct
 * \brief write a value as bool into an agent's parameter.
 *
 * \param name is the name of the parameter bool to write.
 * \param value is a bool to write in the parameter.
 * \return 1 if ok else 0
 */
int mtic_writeParameterAsBool(const char *name, bool value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a bool.
    if(iop->value_type != BOOL_T){
        mtic_debug("%s: Agent's output '%s' is not a bool\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(bool));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(bool));

    return 1;
}

/**
 * \fn int mtic_writeParameterAsInt(const char *name, int value)
 * \ingroup writefct
 * \brief write a value as integer into an agent's parameter.
 *
 * \param name is the name of the parameter integer to write.
 * \param value is an integer to write in the parameter.
 * \return 1 if ok else 0
 */
int mtic_writeParameterAsInt(const char *name, int value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a int.
    if(iop->value_type != INTEGER_T){
        mtic_debug("%s: Agent's output '%s' is not a integer\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(int));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(int));

    return 1;
}

/**
 * \fn int mtic_writeParameterAsDouble(const char *name, double value)
 * \ingroup writefct
 * \brief write a value as double into an agent's parameter.
 *
 * \param name is the name of the parameter double to write.
 * \param value is a double to write in the parameter.
 * \return 1 if ok else 0
 */
int mtic_writeParameterAsDouble(const char *name, double value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a double.
    if(iop->value_type != DOUBLE_T){
        mtic_debug("%s: Agent's output '%s' is not a double\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) &value, sizeof(double));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, sizeof(double));

    return 1;
}

/**
 * \fn int mtic_writeParameterAsString(const char *name, char *value)
 * \ingroup writefct
 * \brief write a value as string (char*) into an agent's parameter.
 *
 * \param name is the name of the parameter string to write.
 * \param value is a string (char*) to write in the parameter.
 * \return 1 if ok else 0
 */
int mtic_writeParameterAsString(const char *name, char *value){

    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as a string.
    if(iop->value_type != STRING_T){
        mtic_debug("%s: Agent's output '%s' is not a string\n", __FUNCTION__,  name);
        return 0;
    }

    // update the value in the iop_live structure
    model_setIopValue(iop, (void*) value, (strlen(value)+1)*sizeof(char));
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, (void*) &value, (strlen(value)+1)*sizeof(char));

    return 1;
}


/**
 * \fn int mtic_writeParameterAsData(const char *name, void *value, long size)
 * \ingroup writefct
 * \brief write a value as data into an agent's parameter.
 *
 * \param name is the name of the parameter data to write
 * \param value is a data to write in the parameter
 * \param size the size of the value
 * \return 1 if ok else 0
 */
int mtic_writeParameterAsData(const char *name, void *value, long size){
    //Get the pointer IOP Agent selected by name
    agent_iop *iop = model_findIopByName((char*) name,PARAMETER_T);

    // Check if the iop has been returned.
    if(iop == NULL){
        mtic_debug("%s : Agent's parameter '%s' cannot be found\n", __FUNCTION__, name);
        return 0;
    }

    //Check the value type as an impulsion.
    if(iop->value_type != DATA_T){
        mtic_debug("%s: Agent's parameter '%s' is not an data\n", __FUNCTION__,  name);
        return 0;
    }

    //Update the value in the definition
    model_setIopValue(iop,value,size);
    
    // call the callbacks associated to if it exist
    runObserveCallbacksForIOP(iop, value, size);

    return 1;
}

/**
 *  \defgroup getTypeFct Agent's inputs/outputs/parameters: Get Type / Number / List functions
 */

/**
 * \fn iopType_t mtic_getTypeForInput(const char *name)
 * \ingroup getTypeFct
 * \brief this function returns the value type of the input (integer, bool, etc .)
 * \param name The string which contains the name of the input. Can't be NULL or empty.
 * \return The Input value type.
 * 0 The parameter name cannot be NULL or empty.
 * -1 The definition live is NULL.
 * -2 The parameter named is not found
 */
iopType_t mtic_getTypeForInput(const char *name){

    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The input name cannot be NULL or empty\n");
        return 0;
    }

    //Find the Iop
    agent_iop *inputFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->inputs_table, name, inputFound);

    if(inputFound == NULL){
        mtic_debug("The input named : %s is not found in the table.\n", name);
        return -2;
    }

    //Return the type of the value
    return inputFound->value_type;
}

/**
 * \fn iopType_t mtic_getTypeForOutput(const char *name)
 * \ingroup getTypeFct
 * \brief this function returns the value type of the output (integer, bool, etc .)
 * \param name The string which contains the name of the output. Can't be NULL or empty.
 * \return The output value type.
 * 0 The parameter name cannot be NULL or empty.
 * -1 The definition live is NULL.
 * -2 The parameter named is not found
 */
iopType_t mtic_getTypeForOutput(const char *name){

    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The output name cannot be NULL or empty\n");
        return 0;
    }

    //Find the Iop
    agent_iop *outputFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find the output by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->outputs_table, name, outputFound);

    if(outputFound == NULL){
        mtic_debug("The output named : %s is not found in the table.\n", name);
        return -2;
    }

    //Return the type of the value
    return outputFound->value_type;
}

/**
 * \fn iopType_t mtic_getTypeForParameter(const char *name)
 * \ingroup getTypeFct
 * \brief this function returns the value type of the parameter (integer, bool, etc .)
 * \param name The string which contains the name of the parameter. Can't be NULL or empty.
 * \return The parameter value type.
 * 0 The parameter name cannot be NULL or empty.
 * -1 The definition live is NULL.
 * -2 The parameter named is not found
 */
iopType_t mtic_getTypeForParameter(const char *name){

    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The parameter name cannot be NULL or empty\n");
        return 0;
    }

    //Find the Iop
    agent_iop *parameterFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find the output by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->params_table, name, parameterFound);

    if(parameterFound == NULL){
        mtic_debug("The parameter named : %s is not found in the table.\n", name);
        return -2;
    }

    //Return the type of the value
    return parameterFound->value_type;
}

/**
 * \fn int mtic_getInputsNumber()
 * \ingroup getTypeFct
 * \brief This function return the number of inputs.
 * \return The number of inputs. -1 the definition live is NULL. If an error occurs a mtic_debug will be set.
 */
int mtic_getInputsNumber()
{
    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    int number = HASH_COUNT(mtic_internal_definition->inputs_table);

    return number;
}

/**
 * \fn int mtic_getOutputsNumber()
 * \ingroup getTypeFct
 * \brief This function return the number of outputs.
 * \return The number of outputs. -1 the definition live is NULL. If an error occurs a mtic_debug will be set.
 */
int mtic_getOutputsNumber()
{
    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    int number = HASH_COUNT(mtic_internal_definition->outputs_table);

    return number;
}

/**
 * \fn int mtic_getParametersNumber()
 * \ingroup getTypeFct
 * \brief This function return the number of parameters.
 *
 * \return The number of parameters. -1 the definition live is NULL. If an error occurs a mtic_debug will be set.
 */
int mtic_getParametersNumber()
{
    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    int number = HASH_COUNT(mtic_internal_definition->params_table);

    return number;
}

/**
 * \fn char** mtic_getInputsList(long *nbOfElements)
 * \ingroup getTypeFct
 * \brief This function return a two dimensions table to get the list of input's name and get the number of elements in this table.
 *
 * \param nbOfElements The pointer on the number of elements.
 * \return list The pointer on the two dimensions table which will get the list of input's name. If an error occurs a mtic_debug will be set and the return is NULL.
 */
char ** mtic_getInputsList(long *nbOfElements){

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return NULL;
    }

    //Get the number of element
    (*nbOfElements) = HASH_COUNT(mtic_internal_definition->inputs_table);
    long N = (*nbOfElements);

    if(N < 1)
        return NULL;

    //Initialize the number of input name in the list
    char ** list = (char**) malloc( N * sizeof(char*));

    //Fill the table of input
    agent_iop *current_iop;
    int index = 0;
    for(current_iop = mtic_internal_definition->inputs_table; current_iop != NULL; current_iop = current_iop->hh.next) {
        list[index] = strdup(current_iop->name);
        index++;
    }

    return list;
}

/**
 * \fn char** mtic_getOutputsList(long *nbOfElements)
 * \ingroup getTypeFct
 * \brief This function return a two dimensions table to get the list of output's name and get the number of elements in this table.
 *
 * \param nbOfElements The pointer on the number of elements.
 * \return list The pointer on the two dimensions table which will get the list of output's name. If an error occurs a mtic_debug will be set and the return is NULL.
 */
char ** mtic_getOutputsList(long *nbOfElements){

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return NULL;
    }

    //Get the number of element
    (*nbOfElements) = HASH_COUNT(mtic_internal_definition->outputs_table);
    long N = (*nbOfElements);

    if(N < 1)
        return NULL;

    //Initialize the number of input name in the list
    char ** list = (char**) malloc( N * sizeof(char*));

    //Fill the table of input
    agent_iop *current_iop;
    int index = 0;
    for(current_iop = mtic_internal_definition->outputs_table; current_iop != NULL; current_iop = current_iop->hh.next) {
        list[index] = strdup(current_iop->name);
        index++;
    }

    return list;
}

/**
 * \fn char** mtic_getParametersList(long *nbOfElements)
 * \ingroup getTypeFct
 * \brief This function return a two dimensions table to get the list of parameter's name and get the number of elements in this table.
 *
 * \param nbOfElements The pointer on the number of elements.
 * \return list The pointer on the two dimensions table which will get the list of parameter's name. If an error occurs a mtic_debug will be set and the return is NULL.
 */
char ** mtic_getParametersList(long *nbOfElements){

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return NULL;
    }

    //Get the number of element
    (*nbOfElements) = HASH_COUNT(mtic_internal_definition->params_table);
    long N = (*nbOfElements);

    if(N < 1)
        return NULL;

    //Initialize the number of input name in the list
    char ** list = (char**) malloc( N * sizeof(char*));

    //Fill the table of input
    agent_iop *current_iop;
    int index = 0;
    for(current_iop = mtic_internal_definition->params_table; current_iop != NULL; current_iop = current_iop->hh.next) {
        list[index] = strdup(current_iop->name);
        index++;
    }

    return list;
}

/**
 *  \defgroup checkfct Agent's inputs/outputs/parameters: Check functions
 */

/**
 * \fn bool mtic_checkInputExistence(const char *name)
 * \ingroup checkfct
 * \brief this function returns the state of the input existence.
 *
 * \param name The string which contains the name of the input. Can't be NULL or empty.
 * \return A boolean "true" if the input exist.
 */
bool mtic_checkInputExistence(const char *name){

    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The input name cannot be NULL or empty\n");
        return false;
    }

    //Find the Iop
    agent_iop *inputFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return false;
    }

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->inputs_table, name, inputFound);

    if(inputFound == NULL){
        mtic_debug("The input named : %s is not found in the table.\n", name);
        return false;
    }

    return true;
}

/**
 * \fn bool mtic_checkOutputExistence(const char *name)
 * \ingroup checkfct
 * \brief this function returns the state of the output existence.
 *
 * \param name The string which contains the name of the output. Can't be NULL or empty.
 * \return A boolean "true" if the output exist.
 */
bool mtic_checkOutputExistence(const char *name){

    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The output name cannot be NULL or empty\n");
        return false;
    }

    //Find the Iop
    agent_iop *outputFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return false;
    }

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->outputs_table, name, outputFound);

    if(outputFound == NULL){
        mtic_debug("The output named : %s is not found in the table.\n", name);
        return false;
    }

    return true;
}

/**
 * \fn bool mtic_checkParameterExistence(const char *name)
 * \ingroup checkfct
 * \brief this function returns the state of the parameter existence.
 *
 * \param name The string which contains the name of the parameter. Can't be NULL or empty.
 * \return A boolean "true" if the output exist.
 */
bool mtic_checkParameterExistence(const char *name){
    //Check the name
    if((name == NULL) || (strlen(name) == 0)){
        mtic_debug("The parameter name cannot be NULL or empty\n");
        return false;
    }

    //Find the Iop
    agent_iop *parameterFound = NULL;

    //check if def live iexist
    if(mtic_internal_definition == NULL){
        mtic_debug("Definition live is NULL.");
        return false;
    }

    //Find the input by the name in the table of the my agent's definition
    HASH_FIND_STR(mtic_internal_definition->params_table, name, parameterFound);

    if(parameterFound == NULL){
        mtic_debug("The parameter named : %s is not found in the table.\n", name);
        return false;
    }

    return true;
}

/**
 *  \defgroup observefct Agent's inputs/outputs/parameters: Observe functions
 *
 */

/**
 * \fn int mtic_observeInput(const char *name, mtic_observeCallback cb, void *myData)
 * \ingroup observefct
 * \brief Observe a input and associate a callback to it.
 * When the input value will change the associated callback will be called.
 *
 * \param name is the name of the agent's input
 * \param cb is a pointer to the associated callback
 * \param myData is a pointer to user's data if needed
 *
 * return 1 if correct or 0
 */
int mtic_observeInput(const char *name, mtic_observeCallback cb, void *myData){
    return model_observe(name, INPUT_T, cb, myData);
}

/**
 * \fn int mtic_observeOutput(const char *name, mtic_observeCallback cb, void * myData)
 * \ingroup observefct
 * \brief Observe a output and associate a callback to it.
 * When the output value will change the associated callback will be called.
 *
 * \param name is the name of the agent's output
 * \param cb is a pointer to the associated callback
 * \param myData is a pointer to user's data if needed
 *
 * return 1 if correct or 0
 */
int mtic_observeOutput(const char *name, mtic_observeCallback cb, void * myData){
    return model_observe(name, OUTPUT_T, cb, myData);
}

/**
 * \fn int mtic_observeParameter(const char *name, mtic_observeCallback cb, void * myData)
 * \ingroup observefct
 * \brief Observe a parameter and associate a callback to it.
 * When the output value will change the associated callback will be called.
 *
 * \param name is the name of the agent's parameter
 * \param cb is a pointer to the associated callback
 * \param myData is a pointer to user's data if needed
 *
 * return 1 if correct or 0
 */
int mtic_observeParameter(const char *name, mtic_observeCallback cb, void * myData){
    return model_observe(name, PARAMETER_T, cb, myData);
}

/**
 *  \defgroup muteiopFct Agent's inputs/outputs/parameters: mute functions
 *
 *
 */

/**
 * \fn int mtic_muteOutput(const char *name)
 * \ingroup muteiopFct
 * \brief mute an agent output
 * \param name is the name of the agent output
 * \return 1 if correct or 0
 */
int mtic_muteOutput(const char *name){
    agent_iop *iop = model_findIopByName((char*) name, OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_debug("%s : output '%s' cannot be found", __FUNCTION__, name);
        return 0;
    }
    iop->is_muted = true;
    return 1;
}

/**
 * \fn int mtic_unmuteOutput(const char *name)
 * \ingroup muteiopFct
 * \brief unmute an agent output
 * \param name is the name of the agent output
 * \return 1 if correct or 0
 */
int mtic_unmuteOutput(const char *name){
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_debug("%s : output '%s' cannot be found", __FUNCTION__, name);
        return 0;
    }
    iop->is_muted = false;
    return 1;
}

/**
 * \fn bool mtic_isOutputMuted(const char *name)
 * \ingroup muteiopFct
 * \brief Give the state of an agent output (mute/unmute)
 * \param name is the name of the agent output
 * \return true if muted else false
 */
bool mtic_isOutputMuted(const char *name){
    agent_iop *iop = model_findIopByName((char*) name,OUTPUT_T);
    if(iop == NULL || iop->type != OUTPUT_T){
        mtic_debug("%s : output '%s' cannot be found", __FUNCTION__, name);
        return 0;
    }
    return iop->is_muted;
}
