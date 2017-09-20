//
//  mtic_definition.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

/**
  * \file ../../src/include/mastic.h
  */

#include <stdio.h>
#include <stdlib.h>
#include "mastic_private.h"
#include "uthash/uthash.h"

definition * mtic_definition_loaded = NULL;
definition * mtic_definition_live = NULL;
definition * mtic_agents_defs_on_network = NULL;

typedef struct agent_port_t {
    const char * name;          //Need to be unique : the table hash key
    int port;
    UT_hash_handle hh;         /* makes this structure hashable */
} agent_port;

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void free_agent_iop (agent_iop** agent_iop){
    
    if ((*agent_iop)->name != NULL){
        free((char*)(*agent_iop)->name);
    }
    
    if ((*agent_iop)->value.s != NULL){
        free((char*)(*agent_iop)->value.s);
    }
    
    if ((*agent_iop)->value.data != NULL){
        free((*agent_iop)->value.data);
    }
    
    free((*agent_iop));
}

bool check_category_agent_iop(agent_iop *ref_iop,
                              agent_iop *iop_to_check) {
    
    bool state = true;
    
    
    struct agent_iop *iop, *iop_found;
    
    for(iop = ref_iop; iop != NULL; iop = iop->hh.next) {
        //Init to null for the next
        iop_found = NULL;
        
        //Find the iop corresponding to name (key)
        HASH_FIND_STR(iop_to_check, iop->name, iop_found);
        
        //Check the type of the iop correspond
        if(iop_found == NULL || (iop_found->value_type != iop->value_type)) {
            state = false;
        }
    }
    
    return state;
}

bool check_category(definition* def,
                       category *category,
                       category_check_type check_type)
{
    bool state = true;

    switch(check_type)
    {
    case INPUT_CAT:
        if(check_category_agent_iop(def->inputs_table,
                                    category->inputs_table) != true)
        {
            state = false;
        }
        break;

    case OUTPUT_CAT:
        if(check_category_agent_iop(def->outputs_table,
                                    category->outputs_table) != true)
        {
            state = false;
        }
        break;

    case GLOBAL_CAT:
        if(check_category_agent_iop(def->inputs_table,
                                    category->inputs_table) != true)
        {
            state = false;
        }


        if(check_category_agent_iop(def->outputs_table,
                                    category->outputs_table) != true)
        {
            state = false;
        }
        break;

    default:
        break;
    }

    return state;
}

void free_category (category* cat){

    struct agent_iop *current, *tmp;

    free((char*)cat->name);
    cat->name = NULL ;
    free((char*)cat->version);
    cat->version = NULL;


    HASH_ITER(hh, cat->params_table, current, tmp) {
        HASH_DEL(cat->params_table,current);
        free_agent_iop(&current);
        //current = NULL;
    }
    HASH_ITER(hh, cat->inputs_table, current, tmp) {
        HASH_DEL(cat->inputs_table,current);
        free_agent_iop(&current);
        //current = NULL;
    }
    HASH_ITER(hh, cat->outputs_table, current, tmp) {
        HASH_DEL(cat->outputs_table,current);
        free_agent_iop(&current);
        //current = NULL;
    }

    free (cat);
}

int definition_setIopValue(agent_iop *iop, void * value, long size)
{
    if(iop == NULL)
        return 0;

    switch (iop->value_type) {
    case INTEGER_T:
        if(value != NULL)
            iop->value.i = *(int*)(value);
        else
            iop->value.i = 0;
        break;
    case DOUBLE_T:
        if(value != NULL)
            iop->value.d = *(double*)(value);
        else
            iop->value.d = 0.0;
        break;
    case BOOL_T:
        if(value != NULL)
            iop->value.b = *(bool*)(value);
        else
            iop->value.b = false;
        break;
    case STRING_T:
        if (iop->value.s != NULL)
            free(iop->value.s);
        if(value != NULL)
            iop->value.s = strdup(value);
        else
            iop->value.s = strdup("");
        break;
    case IMPULSION_T:
        break;
    case DATA_T:
    {
        if (iop->value.data != NULL){
            free(iop->value.data);
        }
        iop->value.data = NULL;
        iop->valueSize = size;
        iop->value.data = calloc(1,size);
        memcpy(iop->value.data,value, size);
    }
        break;
    default:
        break;
    }

    return 1;
}

agent_iop* definition_createIop(const char *name, iop_t type, iopType_t value_type, void *value, long size)
{
    //Create the iop
    agent_iop *iop = NULL;
    iop = calloc (1, sizeof (struct agent_iop));
    iop->name = strdup(name);
    iop->type = type;
    iop->value_type = value_type;

    //Set value
//    agent_iop *ret = NULL;
//    ret = calloc (1, sizeof (struct agent_iop));
    definition_setIopValue(iop, value, size);

    return iop;
}

int definition_addIop(agent_iop *iop, iop_t iop_type, definition **def)
{
    agent_iop *iop_to_add = NULL;

    //Check if already initialized, and do it if not
    if((*def) == NULL){
        (*def) = calloc(1, sizeof(struct definition));
    }

    //Check if the key already exist
    switch (iop_type) {
    case INPUT_T:
        HASH_FIND_STR((*def)->inputs_table, iop->name , iop_to_add);
        break;
    case OUTPUT_T:
        HASH_FIND_STR((*def)->outputs_table, iop->name , iop_to_add);
        break;
    case PARAMETER_T:
        HASH_FIND_STR((*def)->params_table, iop->name , iop_to_add);
        break;
    default:
        break;
    }

    if(iop_to_add != NULL)
        return 0;

    //Copy the iop
    iop_to_add = calloc(1, sizeof(struct agent_iop));
    memcpy(iop_to_add, iop, sizeof(*iop));

    //Add the iop
    switch (iop_type) {
    case INPUT_T:
        HASH_ADD_STR((*def)->inputs_table, name, iop_to_add);
        break;
    case OUTPUT_T:
        HASH_ADD_STR((*def)->outputs_table, name, iop_to_add);
        break;
    case PARAMETER_T:
        HASH_ADD_STR((*def)->params_table, name, iop_to_add);
        break;
    default:
        break;
    }

    return 1;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void definition_initDefinitionToDefault()
{
    //Check if already allocated
    if(mtic_definition_loaded == NULL){
        //Dynamically allocate the memory
        mtic_definition_loaded = calloc(1, sizeof(struct definition));
    }
    
    if(mtic_definition_live== NULL){
        //Dynamically allocate the memory
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }
}

int definition_get_iop_value_as_int(agent_iop *iop, iop_t type){
    int val = *(int *)(model_get(iop->name,type));
    return val;
}

double definition_get_iop_value_as_double(agent_iop *iop,iop_t type){
    double val = *(double *)(model_get(iop->name,type));
    return val;
}

char* definition_get_iop_value_as_string (agent_iop* iop)
{
    char str_value[BUFSIZ];
    if(iop != NULL)
    {
        switch (iop->value_type) {
            case INTEGER_T:
                sprintf(str_value,"%i",iop->value.i);
                break;
            case DOUBLE_T:
                sprintf(str_value,"%lf",iop->value.d);
                break;
            case BOOL_T:
                if(iop->value.b == true)
                {
                    sprintf(str_value,"%s","true");
                    
                } else {
                    sprintf(str_value,"%s","false");
                    
                }
                break;
            case STRING_T:
                sprintf(str_value,"%s",iop->value.s);
                break;
            case IMPULSION_T:
                break;
            case DATA_T:
                sprintf(str_value,"%s", (char*) iop->value.data);
                break;
            default:
                break;
        }
    }
    
    return strdup(str_value);
}

void definition_free_definition (definition* def) {
    
    struct agent_iop *current_iop, *tmp_iop;
    struct category *current_cat, *tmp_cat;
    
    free((char*)def->name);
    def->name = NULL;
    free((char*)def->version);
    def->version = NULL;
    free((char*)def->description);
    def->description = NULL;
    
    HASH_ITER(hh, def->params_table, current_iop, tmp_iop) {
        HASH_DEL(def->params_table,current_iop);
        free_agent_iop(&current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->inputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->inputs_table,current_iop);
        free_agent_iop(&current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->outputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->outputs_table,current_iop);
        free_agent_iop(&current_iop);
        current_iop = NULL;
    }
    
    HASH_ITER(hh, def->categories, current_cat, tmp_cat) {
        HASH_DEL(def->categories,current_cat);
        free_category(current_cat);
        current_cat = NULL;
    }
}



////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

/**
 *  \defgroup loadSetGetDefFct Agent's definition: Load / Clear / Set / Get functions
 *
 */

/**
 * \fn int mtic_clearDefinition()
 * \ingroup loadSetGetDefFct
 * \brief Clear the internal definition of the agent.
 *        Free all members of the structure mtic_definition_loaded & mtic_definition_live.
 *        But the pointer of these structure is not free and stay allocated.
 * \return 1 if ok else 0
 */
int mtic_clearDefinition(){

    mtic_debug("mtic_clearDefinition ... \n");

    //Free the structure definition loaded
    if(mtic_definition_loaded != NULL){
        mtic_debug("Clear the definition loaded ... \n");
        definition_free_definition(mtic_definition_loaded);
    }

    //Free the structure definition loaded
    if(mtic_definition_live != NULL){
        mtic_debug("Clear the definition live ... \n");
        definition_free_definition(mtic_definition_live);
    }

    return 1;
}

/**
 * \fn char* mtic_getDefinition()
 * \ingroup loadSetGetDefFct
 * \brief the agent definition getter
 *
 * \return The loaded definition string in json format (allocated). NULL if mtic_definition_loaded was not initialized.
 * \warning Allocate memory than should be freed by the user.
 */
char* mtic_getDefinition(){
    char * def = NULL;

    if(mtic_definition_loaded == NULL)
        return NULL;

    def = parser_export_definition(mtic_definition_loaded);

    return def;
}

/**
 * \fn int mtic_setDefinitionDescription(char *description)
 * \ingroup loadSetGetDefFct
 * \brief the agent definition description setter
 *
 * \param description The string which contains the description of the agent. Can't be NULL.
 * \return The error. 1 is OK, 0 Agent description is NULL, -1 Agent description is empty
 */
int mtic_setDefinitionDescription(char *description){

    if(description == NULL){
        mtic_debug("mtic_setDefinitionDescription : Agent description cannot be NULL \n");
        return 0;
    }

    if (strlen(description) == 0){
        mtic_debug("mtic_setDefinitionDescription : Agent description cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_definition_loaded == NULL){
        mtic_definition_loaded = calloc(1, sizeof(struct definition));
    }

    //Copy the description in the structure in loaded definition
    if(mtic_definition_loaded->description != NULL)//Free the field if needed
        free((char*)mtic_definition_loaded->description);
    mtic_definition_loaded->description = strdup(description);

    //Check if already initialized, and do it if not
    if(mtic_definition_live == NULL){
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }

    // Live data corresponds to a copy of the initial definition
    if(mtic_definition_live->description != NULL)//Free the field if needed
        free((char*)mtic_definition_live->description);
     mtic_definition_live->description = strdup(mtic_definition_loaded->description);

    return 1;
}

/**
 * \fn mtic_setDefinitionVersion(char *version)
 * \ingroup loadSetGetDefFct
 * \brief the agent definition version setter
 *
 * \param version The string which contains the version of the agent. Can't be NULL.
 * \return The error. 1 is OK, 0 Agent version is NULL, -1 Agent version is empty
 */
int mtic_setDefinitionVersion(char *version){

    if(version == NULL){
        mtic_debug("mtic_setDefinitionVersion : Agent version cannot be NULL \n");
        return 0;
    }

    if (strlen(version) == 0){
        mtic_debug("mtic_setDefinitionVersion : Agent version cannot be empty\n");
        return -1;
    }

    //Check if already initialized, and do it if not
    if(mtic_definition_loaded == NULL){
        mtic_definition_loaded = calloc(1, sizeof(struct definition));
    }

    //Copy the description in the structure in loaded definition
    if(mtic_definition_loaded->version != NULL)//Free the field if needed
        free((char*)mtic_definition_loaded->version);
    mtic_definition_loaded->version = strdup(version);

    //Check if already initialized, and do it if not
    if(mtic_definition_live == NULL){
        mtic_definition_live = calloc(1, sizeof(struct definition));
    }

    // Live data corresponds to a copy of the initial definition
    if(mtic_definition_live->version != NULL)//Free the field if needed
        free((char*)mtic_definition_live->version);
    mtic_definition_live->version = strdup(mtic_definition_loaded->version);

    return 1;
}

/**
 *  \defgroup EditDefFct  Agent's definition: Edit functions
 *
 */

/**
 * \fn mtic_createInput(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add an input for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int mtic_createInput(const char *name, iopType_t value_type, void *value, long size){

    //Create the iop for loaded
    agent_iop* iopLoaded = definition_createIop(name, INPUT_T, value_type, value, size);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iopLoaded, INPUT_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Create the iop for loaded
    agent_iop* iopLive = definition_createIop(name, INPUT_T, value_type, value, size);

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iopLive, INPUT_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iopLoaded);
    free(iopLive);

    return 1;
}

/**
 * \fn mtic_createOutput(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a output for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int mtic_createOutput(const char *name, iopType_t value_type, void *value, long size){
    //Create the iop for loaded
    agent_iop* iopLoaded = definition_createIop(name, OUTPUT_T, value_type, value, size);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iopLoaded, OUTPUT_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Create the iop for loaded
    agent_iop* iopLive = definition_createIop(name, OUTPUT_T, value_type, value, size);

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iopLive, OUTPUT_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iopLoaded);
    free(iopLive);

    return 1;
}

/**
 * \fn mtic_createParameter(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a parameter for the agent
 *
 * \param name The name of the Iop
 * \param type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int mtic_createParameter(const char *name, iopType_t value_type, void *value, long size){
    //Create the iop loaded
    agent_iop* iopLoaded = definition_createIop(name, PARAMETER_T, value_type, value, size);

    //Add iop in structure def loaded, need to be copied
    if (definition_addIop(iopLoaded, PARAMETER_T, &mtic_definition_loaded) < 1){
        return 0;
    }

    //Create the iop
    agent_iop* iopLive = definition_createIop(name, PARAMETER_T, value_type, value, size);

    //Add iop in structure def live, need to be copied
    if (definition_addIop(iopLive, PARAMETER_T, &mtic_definition_live) < 1){
        return -1;
    }

    //free iop
    free(iopLoaded);
    free(iopLive);

    return 1;
}

/**
 * \fn mtic_removeInput(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free an input for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int mtic_removeInput(const char *name){

    //check if def loaded exist
    if(mtic_definition_loaded == NULL){
        mtic_debug("Definition loaded is NULL.");
        return 0;
    }

    //check if def live iexist
    if(mtic_definition_live == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find Iop
    agent_iop * iop = model_findIopByName(name,INPUT_T);

    //Check if iop exist
    if(iop == NULL){
        mtic_debug("An error occurs while finding the iop by name : the iop is NULL.");
        return -2;
    }
    //remove in definition loaded
    HASH_DEL(mtic_definition_loaded->inputs_table, iop);

    //remove in definition live
    HASH_DEL(mtic_definition_live->inputs_table, iop);

    //free Iop
    free_agent_iop(&iop);

    return 1;
}

/**
 * \fn mtic_removeOutput(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free a output for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int mtic_removeOutput(const char *name){
    //check if def loaded exist
    if(mtic_definition_loaded == NULL){
        mtic_debug("Definition loaded is NULL.");
        return 0;
    }

    //check if def live iexist
    if(mtic_definition_live == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find Iop
    agent_iop * iop = model_findIopByName(name,OUTPUT_T);

    //Check if iop exist
    if(iop == NULL){
        mtic_debug("An error occurs while finding the iop by name : the iop is NULL.");
        return -2;
    }
    //remove in definition loaded
    HASH_DEL(mtic_definition_loaded->outputs_table, iop);

    //remove in definition live
    HASH_DEL(mtic_definition_live->outputs_table, iop);

    //free Iop
    free_agent_iop(&iop);

    return 1;
}

/**
 * \fn mtic_removeParameter(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free a output for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int mtic_removeParameter(const char *name){

    //check if def loaded exist
    if(mtic_definition_loaded == NULL){
        mtic_debug("Definition loaded is NULL.");
        return 0;
    }

    //check if def live iexist
    if(mtic_definition_live == NULL){
        mtic_debug("Definition live is NULL.");
        return -1;
    }

    //Find Iop
    agent_iop * iop = model_findIopByName(name,PARAMETER_T);

    //Check if iop exist
    if(iop == NULL){
        mtic_debug("An error occurs while finding the iop by name : the iop is NULL.");
        return -2;
    }
    //remove in definition loaded
    HASH_DEL(mtic_definition_loaded->params_table, iop);

    //remove in definition live
    HASH_DEL(mtic_definition_live->params_table, iop);

    //free Iop
    free_agent_iop(&iop);

    return 1;
}
