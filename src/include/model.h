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
typedef void (*calback_ptr_t)( agent_iop* );


/*
 * Function: mtic_find_iop_by_name
 * ----------------------------
 *
 */
MASTICAPI_COMMON_DLLSPEC agent_iop * mtic_find_iop_by_name(const char* name, model_state *code);
MASTICAPI_COMMON_DLLSPEC agent_iop * mtic_find_iop_by_name_on_definition(const char *name, definition* definition, model_state *code);

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

MASTICAPI_COMMON_DLLSPEC model_state mtic_observe(const char *iop_name,
                    void (*callback_fct)(agent_iop *input_iop) );

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
MASTICAPI_COMMON_DLLSPEC model_state mtic_set(const char* iop_name, void* new_value);

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
MASTICAPI_COMMON_DLLSPEC void * mtic_get(const char* name_iop, model_state* state);

/*
 * Function: mute
 * ----------------------------
 *  Mute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly muted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int mtic_mute(const char* name_iop);

/*
 * Function: unmute
 * ----------------------------
 *  Unmute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly unmuted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int mtic_unmute(const char* name_iop);

/*
 * Function: mtic_muteAll
 * ----------------------------
 *  Mute all outputs.
 *
 *  return : 0 is the outputs have been properly muted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int mtic_muteAll();

/*
 * Function: unmtic_muteAll
 * ----------------------------
 *  Unmute all outputs.
 *
 *  return : 0 is the outputs have been properly unmuted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int unmtic_muteAll();



#endif // MTIC_MODEL_H
