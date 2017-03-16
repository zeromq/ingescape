#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include "definition.h"
#include "network.h"

//Define a state to return in functions
typedef enum
{OK,
NOK,
NAME_EMPTY,
NAME_NOT_EXIST,
TYPE_INPUT,
TYPE_OUTPUT,
TYPE_PARAM,
NO_CALLBACK} model_state;

//Define the pointer on the callback function
typedef void (*calback_ptr_t)( agent_iop* );

//Hashable structure which associate the name of one 'iop' and the pointer of one callback
typedef struct callbacks_t {
    const char * iop_name;          //Need to be unique : the table hash key
    calback_ptr_t callback_ptr;   //pointer on the callback
    UT_hash_handle hh;
} callbacks;

/*
 * Function: find_iop_by_name
 * ----------------------------
 *
 */
MASTICAPI_COMMON_DLLSPEC agent_iop * find_iop_by_name(const char* name, model_state *code);
MASTICAPI_COMMON_DLLSPEC agent_iop * find_iop_by_name_on_definition(const char *name, definition* definition, model_state *code);

/*
 * Function: observe :
 * ----------------------------
 *  Observe the iop and associate a callback to it.
 *  When the iop value will change by calling the 'set' function, the associated callback will be called.
 *
 *  name_iop        : Name of the 'input/output/parameter'
 *
 *  callback_fct    : The pointer to the associated callback
 *
 *  return : model_state (cf. enum) return error is occur
 */

MASTICAPI_COMMON_DLLSPEC model_state observe(const char *iop_name,
                    void (*callback_fct)(agent_iop *input_iop) );

/*
 * Function: set
 * ----------------------------
 *  Set the new value to the associated iop.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  new_value   : The new value
 *
 *  return : model_state (cf. enum) return error is occur
 */
MASTICAPI_COMMON_DLLSPEC model_state set(const char* iop_name, void* new_value);

/*
 * Function: get
 * ----------------------------
 *  Find the 'input/output/parameter' value.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  state       : The pointer on the state (cf. enum)
 *
 *  return : pointer on the value OR null
 */
MASTICAPI_COMMON_DLLSPEC void * get(const char* name_iop, model_state* state);

/*
 * Function: mute
 * ----------------------------
 *  Mute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly muted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int mute(const char* name_iop);

/*
 * Function: unmute
 * ----------------------------
 *  Unmute an 'input/output/parameter'.
 *
 *  name_iop    : Name of the 'input/output/parameter'
 *
 *  return : 0 is the output has been properly unmuted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int unmute(const char* name_iop);

/*
 * Function: muteAll
 * ----------------------------
 *  Mute all outputs.
 *
 *  return : 0 is the outputs have been properly muted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int muteAll();

/*
 * Function: unmuteAll
 * ----------------------------
 *  Unmute all outputs.
 *
 *  return : 0 is the outputs have been properly unmuted, -1 otherwise.
 */
MASTICAPI_COMMON_DLLSPEC int unmuteAll();



#endif // MODEL_H_INCLUDED
