//
//  igs_definition.c
//
//  Created by Patxi Berard & Stephane Vales
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "ingescape_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////
void definition_freeIOP (agent_iop_t* iop){
    if (iop == NULL){
        return;
    }
    if ((iop)->name != NULL){
        free((char*)(iop)->name);
    }
    switch (iop->value_type) {
        case IGS_STRING_T:
            if (iop->value.s != NULL)
                free((char*)(iop)->value.s);
            break;
        case IGS_DATA_T:
            if (iop->value.data != NULL)
                free((iop)->value.data);
            break;
        default:
            break;
    }
    if ((iop)->callbacks != NULL){
        igs_observe_callback_t *cb, *tmp;
        DL_FOREACH_SAFE((iop)->callbacks, cb, tmp){
            DL_DELETE((iop)->callbacks, cb);
            free(cb);
        }
    }
    free(iop);
}

int definition_addIopToDefinition(igsAgent_t *agent, agent_iop_t *iop, iop_t iop_type, definition *def){
    if(def == NULL){
        igsAgent_error(agent, "Cannot add IOP %s to NULL definition", iop->name);
        return 0;
    }
    model_readWriteLock();
    agent_iop_t *previousIOP = NULL;
    switch (iop_type) {
        case IGS_INPUT_T:
            HASH_FIND_STR(def->inputs_table, iop->name , previousIOP);
            break;
        case IGS_OUTPUT_T:
            HASH_FIND_STR(def->outputs_table, iop->name , previousIOP);
            break;
        case IGS_PARAMETER_T:
            HASH_FIND_STR(def->params_table, iop->name , previousIOP);
            break;
        default:
            break;
    }
    if(previousIOP != NULL){
        igsAgent_error(agent, "%s already exists and cannot be overwritten", iop->name);
        model_readWriteUnlock();
        return 0;
    }
    switch (iop_type) {
        case IGS_INPUT_T:
            HASH_ADD_STR(def->inputs_table, name, iop);
            break;
        case IGS_OUTPUT_T:
            HASH_ADD_STR(def->outputs_table, name, iop);
            break;
        case IGS_PARAMETER_T:
            HASH_ADD_STR(def->params_table, name, iop);
            break;
        default:
            break;
    }
    model_readWriteUnlock();
    return 1;
}

agent_iop_t* definition_createIop(igsAgent_t *agent, const char *name, iop_t type, iopType_t value_type, void *value, size_t size){
    if (agent->internal_definition == NULL){
        igsAgent_error(agent, "Cannot add IOP %s to NULL definition", name);
        return NULL;
    }
    agent_iop_t *iop = NULL;
    iop = calloc (1, sizeof(agent_iop_t));
    char *n = strndup(name, MAX_IOP_NAME_LENGTH);
    bool spaceInName = false;
    size_t lengthOfN = strlen(n);
    size_t i = 0;
    for (i = 0; i < lengthOfN; i++){
        if (n[i] == ' '){
            n[i] = '_';
            spaceInName = true;
        }
    }
    if (spaceInName){
        igsAgent_error(agent, "Spaces are not allowed in IOP name: %s has been renamed to %s", name, n);
    }
    iop->name = n;
    iop->type = type;
    iop->value_type = value_type;
    switch (type) {
        case IGS_INPUT_T:
            if (definition_addIopToDefinition(agent, iop, IGS_INPUT_T, agent->internal_definition) < 1){
                model_readWriteLock();
                definition_freeIOP(iop);
                model_readWriteUnlock();
                return NULL;
            }
            break;
        case IGS_OUTPUT_T:
            if (definition_addIopToDefinition(agent, iop, IGS_OUTPUT_T, agent->internal_definition) < 1){
                model_readWriteLock();
                definition_freeIOP(iop);
                model_readWriteUnlock();
                return NULL;
            }
            break;
        case IGS_PARAMETER_T:
            if (definition_addIopToDefinition(agent, iop, IGS_PARAMETER_T, agent->internal_definition) < 1){
                model_readWriteLock();
                definition_freeIOP(iop);
                model_readWriteUnlock();
                return NULL;
            }
            break;
            
        default:
            break;
    }
    model_writeIOP(agent, n, type, value_type, value, size);
    return iop;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////
void definition_freeDefinition (definition* def) {
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
    model_readWriteLock();
    agent_iop_t *current_iop, *tmp_iop;
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
    model_readWriteUnlock();
    igs_call_t *call, *tmpCall;
    HASH_ITER(hh, def->calls_table, call, tmpCall) {
        HASH_DEL(def->calls_table,call);
        call_freeCall(call);
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
 * \fn int igs_clearDefinition()
 * \ingroup loadSetGetDefFct
 * \brief Clear the internal definition of the agent.
 *        Free all members of the structure igs_definition_loaded & agent->internal_definition.
 *        But the pointer of these structure is not free and stay allocated.
 * \return 1 if ok else 0
 */
int igsAgent_clearDefinition(igsAgent_t *agent){

    //Free the structure definition loaded
    igsAgent_debug(agent, "Clear our definition and initiate an empty one");
    if(agent->internal_definition != NULL){
        definition_freeDefinition(agent->internal_definition);
    }
    agent->internal_definition = calloc(1, sizeof(definition));
    agent->internal_definition->name = igsAgent_getAgentName(agent);
    agent->internal_definition->description = NULL;
    agent->internal_definition->version = NULL;
    agent->internal_definition->params_table = NULL;
    agent->internal_definition->inputs_table = NULL;
    agent->internal_definition->outputs_table = NULL;
    agent->internal_definition->calls_table = NULL;
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

char* igsAgent_getDefinition(igsAgent_t *agent){
    char * def = NULL;
    if(agent->internal_definition == NULL)
        return NULL;
    def = parser_export_definition(agent->internal_definition);
    return def;
}

char *igsAgent_getDefinitionName(igsAgent_t *agent){
    if (agent->internal_definition != NULL && agent->internal_definition->name != NULL){
        return strdup(agent->internal_definition->name);
    }else{
        return NULL;
    }
}

char *igsAgent_getDefinitionDescription(igsAgent_t *agent){
    if (agent->internal_definition != NULL && agent->internal_definition->description != NULL){
        return strdup(agent->internal_definition->description);
    }else{
        return NULL;
    }
}

char *igsAgent_getDefinitionVersion(igsAgent_t *agent){
    if (agent->internal_definition != NULL && agent->internal_definition->version != NULL){
        return strdup(agent->internal_definition->version);
    }else{
        return NULL;
    }
}

int igsAgent_setDefinitionName(igsAgent_t *agent, const char *name){
    if (name == NULL){
        igsAgent_error(agent, "Definition name cannot be NULL");
        return 0;
    }
    if (strlen(name) == 0){
        igsAgent_error(agent, "Definition name cannot be empty");
        return -1;
    }
    
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    if(agent->internal_definition->name != NULL){
        free((char*)agent->internal_definition->name);
    }
    agent->internal_definition->name = strndup(name, MAX_DEFINITION_NAME_LENGTH);
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

int igsAgent_setDefinitionDescription(igsAgent_t *agent, const char *description){
    if(description == NULL){
        igsAgent_error(agent, "Definition description cannot be NULL");
        return 0;
    }
    if (strlen(description) == 0){
        igsAgent_error(agent, "Definition description cannot be empty\n");
        return -1;
    }
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    if(agent->internal_definition->description != NULL){
        free((char*)agent->internal_definition->description);
    }
    agent->internal_definition->description = strndup(description, MAX_DESCRIPTION_LENGTH);
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

int igsAgent_setDefinitionVersion(igsAgent_t *agent, const char *version){
    if(version == NULL){
        igsAgent_error(agent, "Definition version cannot be NULL");
        return 0;
    }
    if (strlen(version) == 0){
        igsAgent_error(agent, "Definition version cannot be empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    if(agent->internal_definition->version != NULL){
        free((char*)agent->internal_definition->version);
    }
    agent->internal_definition->version = strndup(version, 64);
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 *  \defgroup EditDefFct  Agent's definition: Edit functions
 *
 */

/**
 * \fn igs_createInput(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add an input for the agent
 *
 * \param name The name of the Iop
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int igsAgent_createInput(igsAgent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    if (name == NULL || strlen (name) == 0){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop_t *iop = definition_createIop(agent, name, IGS_INPUT_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn igs_createOutput(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a output for the agent
 *
 * \param name The name of the Iop
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */

int igsAgent_createOutput(igsAgent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    if (name == NULL || strlen (name) == 0){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop_t* iop = definition_createIop(agent, name, IGS_OUTPUT_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn igs_createParameter(const char *name, iopType_t type, void *value)
 * \ingroup EditDefFct
 * \brief Create and add a parameter for the agent
 *
 * \param name The name of the Iop
 * \param value_type The Iop type : input, output or parameter
 * \param value The pointer on the value (the value will be copied)
 * \return The error. 1 is OK, 0 not able to add in definition loaded, -1 not able to add in definition live
 */
int igsAgent_createParameter(igsAgent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    if (name == NULL || strlen (name) == 0){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        agent->internal_definition = calloc(1, sizeof(definition));
    }
    agent_iop_t* iop = definition_createIop(agent, name, IGS_PARAMETER_T, value_type, value, size);
    if (iop == NULL){
        return -1;
    }
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn igs_removeInput(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free an input for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int igsAgent_removeInput(igsAgent_t *agent, const char *name){
    if (name == NULL){
        igsAgent_error(agent, "Input name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    agent_iop_t * iop = model_findIopByName(agent, name,IGS_INPUT_T);
    if(iop == NULL){
        igsAgent_warn(agent, "The input %s could not be found", name);
        return -2;
    }
    model_readWriteLock();
    HASH_DEL(agent->internal_definition->inputs_table, iop);
    definition_freeIOP(iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn igs_removeOutput(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free a output for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int igsAgent_removeOutput(igsAgent_t *agent, const char *name){
    if (name == NULL){
        igsAgent_error(agent, "Output name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    agent_iop_t * iop = model_findIopByName(agent, name,IGS_OUTPUT_T);
    if(iop == NULL){
        igsAgent_warn(agent, "The output %s could not be found", name);
        return -2;
    }
    model_readWriteLock();
    HASH_DEL(agent->internal_definition->outputs_table, iop);
    definition_freeIOP(iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

/**
 * \fn igs_removeParameter(const char *name)
 * \ingroup EditDefFct
 * \brief Remove and free a output for the agent
 *
 * \param name The name of the Iop
 * \return The error. 1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name
 */
int igsAgent_removeParameter(igsAgent_t *agent, const char *name){
    if (name == NULL){
        igsAgent_error(agent, "Parameter name cannot be NULL or empty");
        return -1;
    }
    if(agent->internal_definition == NULL){
        igsAgent_error(agent, "No definition available yet");
        return -1;
    }
    agent_iop_t * iop = model_findIopByName(agent, name,IGS_PARAMETER_T);
    if(iop == NULL){
        igsAgent_warn(agent, "The parameter %s could not be found", name);
        return -2;
    }
    model_readWriteLock();
    HASH_DEL(agent->internal_definition->params_table, iop);
    definition_freeIOP(iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return 1;
}

void igsAgent_setDefinitionPath(igsAgent_t *agent, const char *path){
    strncpy(agent->definition_path, path, IGS_MAX_PATH - 1);
    if (agent->agentElements != NULL && agent->agentElements->node != NULL){
        bus_zyreLock();
        zyre_shouts(agent->agentElements->node, CHANNEL, "DEFINITION_FILE_PATH=%s", agent->definition_path);
        bus_zyreUnlock();
    }
}

void igsAgent_writeDefinitionToPath(igsAgent_t *agent){
    FILE *fp = NULL;
    fp = fopen (agent->definition_path,"w+");
    if (fp == NULL){
        igsAgent_error(agent, "Could not open %s for writing", agent->definition_path);
    }else{
        char *def = parser_export_definition(agent->internal_definition);
        fprintf(fp, "%s", def);
        fflush(fp);
        fclose(fp);
        free(def);
    }
}
