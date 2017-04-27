//
//  mtic_model.c
//
//  Created by Patxi Berard
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

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

agent_iop * mtic_find_iop_by_name(const char *name, model_state *code){
    
    return mtic_find_iop_by_name_on_definition(name,mtic_definition_live,code);
}


void update_value(agent_iop *iop, void* value){
    switch (iop->type) {
        case INTEGER:
            iop->old_value.i = iop->value.i;
            iop->value.i = *(int*)(value);
            break;
        case DOUBLE_TYPE:
            iop->old_value.d = iop->value.d;
            iop->value.d = *(double*)(value);
            break;
        case BOOL_TYPE:
            iop->old_value.b = iop->value.b;
            iop->value.b = *(bool*)(value);
            break;
        case STRING:
            free(iop->old_value.s);
            iop->old_value.s = strdup(iop->value.s);
            free(iop->value.s);
            iop->value.s = strdup(value);
            break;
        case IMPULSION:
            free(iop->old_value.impuls);
            iop->old_value.impuls = strdup(iop->value.impuls);
            free(iop->value.impuls);
            iop->value.impuls = strdup(value);
        break;
        case STRUCTURE:
            free(iop->old_value.strct);
            iop->old_value.strct = strdup(iop->value.strct);
            free(iop->value.strct);
            iop->value.strct = strdup(value);
        break;
        default:
            break;
    }
}


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


void * mtic_get(const char *name_iop, model_state *state){

    agent_iop *iop = mtic_find_iop_by_name((char*) name_iop, state);

    if(iop == NULL)
        return state;

    //Return the value corresponding
    switch (iop->type) {
        case INTEGER:
            return &iop->value.i;
            break;
        case DOUBLE_TYPE:
            return &iop->value.d;
            break;
        case BOOL_TYPE:
            return &iop->value.b;
            break;
        case STRING:
            return iop->value.s;
            break;
        case IMPULSION:
            return iop->value.impuls;
            break;
        case STRUCTURE:
            return iop->value.strct;
            break;
        default:
            break;
    }

    return NULL;
}


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
        publish_output(iop_name);
    }

    return OK;
}

int mtic_mute(const char* iop_name)
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

int mtic_unmute(const char* iop_name)
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

int mtic_muteAll()
{
    int result = 0,result_tmp = 0;

    // Go through the agent outpust to mute them
    struct agent_iop *current_iop, *tmp_iop;
    HASH_ITER(hh, mtic_definition_live->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_mute(current_iop->name);
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
    HASH_ITER(hh, mtic_definition_live->outputs_table, current_iop, tmp_iop) {
        if(current_iop != NULL)
        {
            result_tmp = mtic_unmute(current_iop->name);
            // If one one the output has not been unmuted, we notice it
            if(result_tmp != 0)
            {
                result = result_tmp;
            }
        }
    }
    
    return result;
}
