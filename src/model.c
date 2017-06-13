//
//  mtic_model.c
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
//#include "mastic.h"
#include "mastic_private.h"


//Define the pointer on the callback function
typedef void (*calback_ptr_t)( agent_iop* );

//Hashable structure which associate the name of one 'iop' and the pointer of one callback
typedef struct callbacks {
    const char * iop_name;          //Need to be unique : the table hash key
    calback_ptr_t callback_ptr;   //pointer on the callback
    UT_hash_handle hh;
} callbacks;

//The variable which will contain all the callbacks associated to 'iop'
callbacks *agent_callbacks;

// ------------ main functions use in 'MODEL' --------------- //

agent_iop * mtic_find_iop_by_name_on_definition(const char *name, definition* definition, model_state *code){
    agent_iop *found = NULL;
    
    if(name != NULL && definition != NULL){
        //find the input agent_iop
        HASH_FIND_STR( definition->inputs_table, name, found );
        if(found == NULL){
            // look in the outputs
            HASH_FIND_STR( definition->outputs_table, name, found );
        }else
        {
            *code = TYPE_INPUT;
            return found;
        }
        
        if(found == NULL){
            //find the parameters agent_iop
            HASH_FIND_STR( definition->params_table, name, found );
        }else{
            *code = TYPE_OUTPUT;
            return found;
        }
        
        if(found == NULL){
            fprintf(stderr, "ERROR : The name of the iop {%s} doesn't exist. \n",name);
            *code = NOK;
            return found;
        }else
        {
            *code = TYPE_PARAM;
            return found;
        }
    }else{
        fprintf(stderr, "ERROR : The name of the IOP is empty. \n");
        *code = NAME_EMPTY;
        return found;
    }
}

/*
 * Function: mtic_find_iop_by_name
 * ----------------------------
 *
 */
agent_iop * mtic_find_iop_by_name(const char *name, model_state *code){
    
    return mtic_find_iop_by_name_on_definition(name,mtic_definition_live,code);
}


void update_value(agent_iop *iop, void* value){
    switch (iop->type) {
        case INTEGER_T:
            iop->old_value.i = iop->value.i;
            iop->value.i = *(int*)(value);
            break;
        case DOUBLE_T:
            iop->old_value.d = iop->value.d;
            iop->value.d = *(double*)(value);
            break;
        case BOOL_T:
            iop->old_value.b = iop->value.b;
            iop->value.b = *(bool*)(value);
            break;
        case STRING_T:
            free(iop->old_value.s);
            iop->old_value.s = strdup(iop->value.s);
            free(iop->value.s);
            iop->value.s = strdup(value);
            break;
        case IMPULSION_T:
            free(iop->old_value.impuls);
            iop->old_value.impuls = strdup(iop->value.impuls);
            free(iop->value.impuls);
            iop->value.impuls = strdup(value);
        break;
        case DATA_T:
            free(iop->old_value.data);
            iop->old_value.data = strdup(iop->value.data);
            free(iop->value.data);
            iop->value.data = strdup(value);
        break;
        default:
            break;
    }
}

/*
 * Function: observe :
 * ----------------------------
 *  Observe the iop and associate a callback to it.
 *  When the iop value will change by calling the 'mtic_set' function, the associated callback will be called.
 *
 *  name_iop        : Name of the 'input/output/parameter'
 *
 *  callback_fct    : The pointer to the associated callback
 *
 *  return : model_state (cf. enum) return error is occur
 */
model_state mtic_observe(const char *iop_name,
             void (*callback_fct)(agent_iop *input_iop)){

    //1) find the iop
    model_state code;
    agent_iop *iop = mtic_find_iop_by_name((char*)iop_name,&code);

    if(iop == NULL)
        return code;

    //2) Associate the input name with the callback which it will be called in SET
    if(callback_fct != NULL){
        callbacks *new_callback = malloc(sizeof(callbacks));
        new_callback->iop_name = strdup(iop_name);  //assign the key
        new_callback->callback_ptr = callback_fct;
        HASH_ADD_STR( agent_callbacks, iop_name,  new_callback);
    }else
        return NO_CALLBACK;

    return OK;
}

/*
 * Function: mtic_get
 * ----------------------------
 *  Find the 'input/output/parameter' value.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  state       : The pointer on the state (cf. enum)
 *
 *  return : pointer on the value OR null
 */
void * mtic_get(const char *name_iop, model_state *state){

    agent_iop *iop = mtic_find_iop_by_name((char*) name_iop, state);

    if(iop == NULL)
        return state;

    //Return the value corresponding
    switch (iop->type) {
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
            return iop->value.impuls;
            break;
        case DATA_T:
            return iop->value.data;
            break;
        default:
            break;
    }

    return NULL;
}

/*
 * Function: mtic_set
 * ----------------------------
 *  Set the new value to the associated iop.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  new_value   : The new value
 *
 *  return : model_state (cf. enum) return error is occur
 */
model_state mtic_set(const char *iop_name, void *new_value){

    //1) find the iop
    model_state code;
    agent_iop *iop = mtic_find_iop_by_name((char*) iop_name,&code);

    if(iop == NULL)
        return code;

    //2) update the value in the iop_live structure
    update_value(iop, new_value);
    
    // Let us know the value has changed
    char* str_value = mtic_iop_value_to_string(iop);
    mtic_debug("SET(%s,%s).\n",iop_name,str_value);
    free(str_value);

    //3) Callback associated from 'observe' function
    //Check ma liste de callback et appellé celle concerné
    callbacks *fct_to_call;
    HASH_FIND_STR(agent_callbacks,iop_name,fct_to_call);
    if(fct_to_call != NULL)
    {
        fct_to_call->callback_ptr(iop);
    }

    /*
     * If the iop is an output  : publish
     */
    if(code == TYPE_OUTPUT){
        
        // Publish the new value
        network_publishOutput(iop_name);
    }

    return OK;
}

/*
 * Function: mute
 * ----------------------------
 *  Mute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly muted, -1 otherwise.
 */

int mtic_mute_internal(const char* iop_name)
{
    int result = -1;
    model_state code;
    // mtic_get iop object
    agent_iop *iop = mtic_find_iop_by_name((char*)iop_name,&code);
    
    if(iop != NULL)
    {
        // update the is_muted state
        iop->is_muted = true;
        result = 0;
    }
    
    return result;
}

/*
 * Function: unmute
 * ----------------------------
 *  Unmute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly unmuted, -1 otherwise.
 */
int mtic_unmute_internal(const char* iop_name)
{
    int result = -1;
    model_state code;
    // mtic_get iop object
    agent_iop *iop = mtic_find_iop_by_name((char*)iop_name,&code);
    
    if(iop != NULL)
    {
        // update the is_muted state
        iop->is_muted = false;
        result = 0;
    }
    
    return result;
}

/*
 * Function: mtic_muteAll
 * ----------------------------
 *  Mute all outputs.
 *
 *  return : 0 is the outputs have been properly muted, -1 otherwise.
 */
int mtic_muteAll()
{
    int result = 0,result_tmp = 0;

    // Go through the agent outpust to mute them
    struct agent_iop *current_iop, *tmp_iop;
    HASH_ITER(hh, mtic_definition_live->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_mute_internal(current_iop->name);
            // If one one the output has not been muted, we notice it
            if(result_tmp != 0)
            {
                result = result_tmp;
            }
        }
    }
    
    return result;
}

/*
 * Function: unmtic_muteAll
 * ----------------------------
 *  Unmute all outputs.
 *
 *  return : 0 is the outputs have been properly unmuted, -1 otherwise.
 */
int mtic_unmuteAll()
{
    int result = 0,result_tmp = 0;
    
    // Go through the agent outpust to mute them
    struct agent_iop *current_iop, *tmp_iop;
    HASH_ITER(hh, mtic_definition_live->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_unmute_internal(current_iop->name);
            // If one one the output has not been unmuted, we notice it
            if(result_tmp != 0)
            {
                result = result_tmp;
            }
        }
    }
    
    return result;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
//à remplir ou déplacer ici



////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

//read/write IOP using void*
//generic typeless functions (requires developer to check IOP type for type casting)
//for IMPULSION_T value is always 0
//size is passed by Mastic based on type (for bool, double, int and string) or metadata (for data)
void mtic_readInput(const char *name, void *value, long *size){
    
}
void mtic_readOutput(const char *name, void *value, long *size){
    
}
void mtic_readParameter(const char *name, void *value, long *size){
    
}

//read per type
//implicit conversions are possible. Some need to raise warnings.
//we need to make things clear on structures
//for IMPULSION_T value is always 0
//for DATA_T, size is passed by Mastic

/**
 * \fn mtic_readInputAsBool(char *name)
 * \brief Find the Agent's input by name and return the state value as a Boolean.
 * \param The name of the input to read as it has been defined in the definition.
 * \return The state value as true or false.
 */
bool mtic_readInputAsBool(const char *name){

    //Get the pointer IOP Agent selected by name.
    model_state state;
    agent_iop *iop = mtic_find_iop_by_name((char*) name, &state);

    // Check if an IOP Agent has been returned.
    if(iop != NULL){
        //Check the value type.
        if(iop->type == BOOL_T){
            return iop->value.b;
        }else{
             //Handle the case: the input is not a Boolean.
            mtic_debug("mtic_readInputAsBool : iop %s is not a Boolean!", name);
            return false;
        }
    }
    else{
        //Handle the case: the input is not found.
        mtic_debug("mtic_readInputAsBool : Angent's input %s cannot be found!", name);
        return false;
    }

}

int mtic_readInputAsInt(const char *name){
    return 1;
}

double mtic_readInputAsDouble(const char *name){
    return 1;
}

char* mtic_readInputAsString(const char *name){
    return NULL;
}

void mtic_readInputAsData(const char *name, void *data, long *size){ //allocs data structure to be disposed by caller
    
}
bool mtic_readOutputAsBool(const char *name){
    return 1;
}
int mtic_readOutputAsInt(const char *name){
    return 1;
}
double mtic_readOutputAsDouble(const char *name){
    return 1;
}
char* mtic_readOutputAsString(const char *name){
    return NULL;
}
void mtic_readOutputAsData(const char *name, void *data, long *size){ //allocs data structure to be disposed by caller
    
}
bool mtic_readParameterAsBool(const char *name){
    return 1;
}
int mtic_readParameterAsInt(const char *name){
    return 1;
}
double mtic_readParameterAsDouble(const char *name){
    return 1;
}
char* mtic_readParameterAsString(const char *name){
    return NULL;
}
void mtic_readParameterAsData(const char *name, void *data, long *size){ //allocs data structure to be disposed by caller
    
}
//write using void*
//for IMPULSION_T value is just ignored
//for DATA_T, these functions should be forbidden (need to know datra size)
//size shall be given to Mastic
//Mastic shall clone value and shall dispose of it when stopped
int mtic_writeInput(const char *name, void *value, long size){
    return 1;
}
int mtic_writeOutput(const char *name, void *value, long size){
    return 1;
}
int mtic_writeParameter(const char *name, void *value, long size){
    return 1;
}

//write using internal conversions (Mastic does the conversion job)
//we need to make things clear on structures
//for IMPULSION_T value is just ignored
//Mastic shall clone value and shall dispose of it when stopped
int mtic_writeInputAsBool(const char *name, bool value){
    return 1;
}
int mtic_writeInputAsInt(const char *name, int value){
    return 1;
}
int mtic_writeInputAsDouble(const char *name, double value){
    return 1;
}
int mtic_writeInputAsString(const char *name, char *value){
    return 1;
}
int mtic_writeInputAsData(const char *name, void *value, long size){
    return 1;
}

int mtic_writeOutputAsBool(const char *name, bool value){
    return 1;
}
int mtic_writeOutputAsInt(const char *name, int value){
    return 1;
}
int mtic_writeOutputAsDouble(const char *name, double value){
    return 1;
}
int mtic_writeOutputAsString(const char *name, char *value){
    return 1;
}
int mtic_writeOutputAsData(const char *name, void *value, long size){
    return 1;
}

int mtic_writeParameterAsBool(const char *name, bool value){
    return 1;
}
int mtic_writeParameterAsInt(const char *name, int value){
    return 1;
}
int mtic_writeParameterAsDouble(const char *name, double value){
    return 1;
}
int mtic_writeParameterAsString(const char *name, char *value){
    return 1;
}
int mtic_writeParameterAsData(const char *name, void *value, long size){
    return 1;
}


//check IOP type, lists and existence
iopType_t mtic_getTypeForInput(const char *name){
    return INTEGER_T;
}
iopType_t mtic_getTypeForOutput(const char *name){
    return INTEGER_T;
}
iopType_t mtic_getTypeForParameter(const char *name){
    return INTEGER_T;
}

void mtic_getInputsList(char **list, long nbOfElements){
    
}
void mtic_getOutputsList(char **list, long nbOfElements){
    
}
void mtic_getParametersList(char **list, long nbOfElements){
    
}

bool mtic_checkInputExistence(const char *name){
    return 1;
}
bool mtic_checkOutputExistence(const char *name){
    return 1;
}
bool mtic_checkParameterExistence(const char *name){
    return 1;
}

//observe IOP
//calback format for IOP observation
typedef void (*mtic_observeCallback)(iop_t iop, const char *name, iopType_t valueType, void *value, void * myData);
int mtic_observeInput(const char *name, mtic_observeCallback cb, void *myData){
    return 1;
}
int mtic_observeOutput(const char *name, mtic_observeCallback cb, void * myData){
    return 1;
}
int mtic_observeParameter(const char *name, mtic_observeCallback cb, void * myData){
    return 1;
}


//mute or unmute an IOP
int mtic_muteOutput(const char *name){
    return 1;
}
int mtic_unmuteOutput(const char *name){
    return 1;
}
bool mtic_isOutputMuted(const char *name){
    return 1;
}

//mute the agent
int mtic_mute(){
    return 1;
}

int mtic_unmute(){
    return 1;
}

bool mtic_isMuted(){
    return 1;
}

