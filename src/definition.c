//
//  mtic_definition.c
//
//  Created by Patxi Berard
//  Modified by Mathieu Poirier
//  Modified by Vincent Deliencourt
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "mastic_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#ifdef _WIN32
#include "unixfunctions.h"
#endif

definition * mtic_internal_definition = NULL;

typedef struct agent_port_t {
    const char * name;          //Needs to be unique : the table hash key
    int port;
    UT_hash_handle hh;
} agent_port;

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void definition_freeIOP (agent_iop* agent_iop){
    if (agent_iop == NULL){
        return;
    }
    if ((agent_iop)->name != NULL){
        free((char*)(agent_iop)->name);
    }
    if ((agent_iop)->value.s != NULL){
        free((char*)(agent_iop)->value.s);
    }
    if ((agent_iop)->value.data != NULL){
        free((agent_iop)->value.data);
    }
    if ((agent_iop)->callbacks != NULL){
        mtic_observe_callback_t *cb, *tmp;
        DL_FOREACH_SAFE((agent_iop)->callbacks, cb, tmp){
            DL_DELETE((agent_iop)->callbacks, cb);
            free(cb);
        }
    }
    free(agent_iop);
}

int definition_addIopToDefinition(agent_iop *iop, iop_t iop_type, definition *def){
    if(def == NULL){
        mtic_error("Cannot add IOP %s to NULL definition", iop->name);
        return 0;
    }
    agent_iop *previousIOP = NULL;
    switch (iop_type) {
        case INPUT_T:
            HASH_FIND_STR(def->inputs_table, iop->name , previousIOP);
            break;
        case OUTPUT_T:
            HASH_FIND_STR(def->outputs_table, iop->name , previousIOP);
            break;
        case PARAMETER_T:
            HASH_FIND_STR(def->params_table, iop->name , previousIOP);
            break;
        default:
            break;
    }
    if(previousIOP != NULL){
        mtic_error("%s already exists and cannot be overwritten", iop->name);
        return 0;
    }
    switch (iop_type) {
        case INPUT_T:
            HASH_ADD_STR(def->inputs_table, name, iop);
            break;
        case OUTPUT_T:
            HASH_ADD_STR(def->outputs_table, name, iop);
            break;
        case PARAMETER_T:
            HASH_ADD_STR(def->params_table, name, iop);
            break;
        default:
            break;
    }
    return 1;
}

agent_iop* definition_createIop(const char *name, iop_t type, iopType_t value_type, void *value, long size){
    if (mtic_internal_definition == NULL){
        mtic_error("Cannot add IOP %s to NULL definition", name);
        return NULL;
    }
    agent_iop *iop = NULL;
    iop = calloc (1, sizeof(agent_iop));
    char *n = strndup(name, MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t lengthOfN = strlen(n);
    for (size_t i = 0; i < lengthOfN; i++){
        if (n[i] == ' '){
            n[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        mtic_warn("Spaces are not allowed in IOP: %s has been renamed to %s", name, n);
    }
    iop->name = n;
    iop->type = type;
    iop->value_type = value_type;
    switch (type) {
        case INPUT_T:
            if (definition_addIopToDefinition(iop, INPUT_T, mtic_internal_definition) < 1){
                definition_freeIOP(iop);
                return NULL;
            }
            break;
        case OUTPUT_T:
            if (definition_addIopToDefinition(iop, OUTPUT_T, mtic_internal_definition) < 1){
                definition_freeIOP(iop);
                return NULL;
            }
            break;
        case PARAMETER_T:
            if (definition_addIopToDefinition(iop, PARAMETER_T, mtic_internal_definition) < 1){
                definition_freeIOP(iop);
                return NULL;
            }
            break;
            
        default:
            break;
    }
    model_writeIOP(n, type, value_type, value, size);
    return iop;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void definition_freeDefinition (definition* def) {
    struct agent_iop *current_iop, *tmp_iop;
    if (def->name != NULL){
        free((char*)def->name);
        def->name = NULL;
    }
    if (def->description != NULL){
        free((char*)def->description);
        def->description = NULL;
    }
    if (def->version != NULL){
        free((char*)def->version);
        def->version = NULL;
    }
    HASH_ITER(hh, def->params_table, current_iop, tmp_iop) {
        HASH_DEL(def->params_table,current_iop);
        definition_freeIOP(current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->inputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->inputs_table,current_iop);
        definition_freeIOP(current_iop);
        current_iop = NULL;
    }
    HASH_ITER(hh, def->outputs_table, current_iop, tmp_iop) {
        HASH_DEL(def->outputs_table,current_iop);
        definition_freeIOP(current_iop);
        current_iop = NULL;
    }
    free(def);
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
 *        Free all members of the structure mtic_definition_loaded & mtic_internal_definition.
 *        But the pointer of these structure is not free and stay allocated.
 * \return 1 if ok else 0
 */
int mtic_clearDefinition(){

    //Free the structure definition loaded
    mtic_info("Clear our definition and initiate an empty one");
    if(mtic_internal_definition != NULL){
        definition_freeDefinition(mtic_internal_definition);
    }
    mtic_internal_definition = calloc(1, sizeof(struct definition));
    mtic_internal_definition->name = mtic_getAgentName();
    mtic_internal_definition->description = NULL;
    mtic_internal_definition->version = NULL;
    mtic_internal_definition->params_table = NULL;
    mtic_internal_definition->inputs_table = NULL;
    mtic_internal_definition->outputs_table = NULL;
    network_needToSendDefinitionUpdate = true;
    return 1;
}

char* mtic_getDefinition(){
    char * def = NULL;
    if(mtic_internal_definition == NULL)
        return NULL;
    def = parser_export_definition(mtic_internal_definition);
    return def;
}

char *mtic_getDefinitionName(void){
    if (mtic_internal_definition != NULL && mtic_internal_definition->name != NULL){
        return strdup(mtic_internal_definition->name);
    }else{
        return NULL;
    }
}

char *mtic_getDefinitionDescription(void){
    if (mtic_internal_definition != NULL && mtic_internal_definition->description != NULL){
        return strdup(mtic_internal_definition->description);
    }else{
        return NULL;
    }
}

char *mtic_getDefinitionVersion(void){
    if (mtic_internal_definition != NULL && mtic_internal_definition->version != NULL){
        return strdup(mtic_internal_definition->version);
    }else{
        return NULL;
    }
}

int mtic_setDefinitionName(const char *name){
    if (name == NULL){
        mtic_error("Definition name cannot be NULL");
        return 0;
    }
    if (strlen(name) == 0){
        mtic_error("Definition name cannot be empty");
        return -1;
    }
    
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(definition));
    }
    if(mtic_internal_definition->name != NULL){
        free((char*)mtic_internal_definition->name);
    }
    mtic_internal_definition->name = strdup(name);
    network_needToSendDefinitionUpdate = true;
    return 1;
}

int mtic_setDefinitionDescription(const char *description){
    if(description == NULL){
        mtic_error("Definition description cannot be NULL");
        return 0;
    }
    if (strlen(description) == 0){
        mtic_error("Definition description cannot be empty\n");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(definition));
    }
    if(mtic_internal_definition->description != NULL){
        free((char*)mtic_internal_definition->description);
    }
    mtic_internal_definition->description = strdup(description);
    network_needToSendDefinitionUpdate = true;
    return 1;
}

int mtic_setDefinitionVersion(const char *version){
    if(version == NULL){
        mtic_error("Definition version cannot be NULL");
        return 0;
    }
    if (strlen(version) == 0){
        mtic_error("Definition version cannot be empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(struct definition));
    }
    if(mtic_internal_definition->version != NULL){
        free((char*)mtic_internal_definition->version);
    }
    mtic_internal_definition->version = strdup(version);
    network_needToSendDefinitionUpdate = true;
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
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int mtic_createInput(const char *name, iopType_t value_type, void *value, long size){
    if (name == NULL || strlen (name) == 0){
        mtic_error("Input name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop *iop = definition_createIop(name, INPUT_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn mtic_createOutput(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a output for the agent
 *
 * \param name The name of the Iop
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int mtic_createOutput(const char *name, iopType_t value_type, void *value, long size){
    if (name == NULL || strlen (name) == 0){
        mtic_error("Output name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop* iop = definition_createIop(name, OUTPUT_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn mtic_createParameter(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a parameter for the agent
 *
 * \param name The name of the Iop
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int mtic_createParameter(const char *name, iopType_t value_type, void *value, long size){
    if (name == NULL || strlen (name) == 0){
        mtic_error("Parameter name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop* iop = definition_createIop(name, PARAMETER_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    network_needToSendDefinitionUpdate = true;
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
    if (name == NULL){
        mtic_error("Input name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_error("No definition available yet");
        return -1;
    }
    agent_iop * iop = model_findIopByName(name,INPUT_T);
    if(iop == NULL){
        mtic_warn("The input %s could not be found", name);
        return -2;
    }
    HASH_DEL(mtic_internal_definition->inputs_table, iop);
    definition_freeIOP(iop);
    network_needToSendDefinitionUpdate = true;
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
    if (name == NULL){
        mtic_error("Output name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_error("No definition available yet");
        return -1;
    }
    agent_iop * iop = model_findIopByName(name,OUTPUT_T);
    if(iop == NULL){
        mtic_warn("The output %s could not be found", name);
        return -2;
    }
    HASH_DEL(mtic_internal_definition->outputs_table, iop);
    definition_freeIOP(iop);
    network_needToSendDefinitionUpdate = true;
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
    if (name == NULL){
        mtic_error("Parameter name cannot be NULL or empty");
        return -1;
    }
    if(mtic_internal_definition == NULL){
        mtic_error("No definition available yet");
        return -1;
    }
    agent_iop * iop = model_findIopByName(name,PARAMETER_T);
    if(iop == NULL){
        mtic_warn("The parameter %s could not be found", name);
        return -2;
    }
    HASH_DEL(mtic_internal_definition->params_table, iop);
    definition_freeIOP(iop);
    network_needToSendDefinitionUpdate = true;
    return 1;
}

void mtic_setDefinitionPath(const char *path){
    strncpy(definitionPath, path, MAX_PATH - 1);
}

void mtic_writeDefinitionToPath(void){
    FILE *fp = NULL;
    fp = fopen (definitionPath,"w+");
    if (fp == NULL){
        mtic_error("Could not open %s for writing", definitionPath);
    }else{
        char *def = parser_export_definition(mtic_internal_definition);
        fprintf(fp, "%s", def);
        fflush(fp);
        fclose(fp);
        free(def);
    }
}
