//
//  mtic_model.h
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef MTIC_MODEL_H
#define MTIC_MODEL_H

#include "network.h"

//Define a state to return in functions
typedef enum{
    OK,
    NOK,
    NAME_EMPTY,
    NAME_NOT_EXIST,
    TYPE_INPUT,
    TYPE_OUTPUT,
    TYPE_PARAM,
    NO_CALLBACK
} model_state;

//Define the pointer on the callback function
typedef void (*callback_ptr_t)( agent_iop* );



model_state mtic_observe(const char *iop_name,
                    void (*callback_fct)(agent_iop *input_iop) );


model_state mtic_set(const char* iop_name, void* new_value);


void * mtic_get(const char* name_iop, model_state* state);

int mtic_mute(const char* name_iop);


int mtic_unmute(const char* name_iop);


int mtic_muteAll();


int mtic_unmuteAll();



#endif // MTIC_MODEL_H
