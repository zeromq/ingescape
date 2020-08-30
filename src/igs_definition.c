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
void definition_freeIOP (igs_iop_t **iop){
    assert(iop);
    assert(*iop);
    if ((*iop)->name)
        free((*iop)->name);
    
    switch ((*iop)->value_type) {
        case IGS_STRING_T:
            if ((*iop)->value.s)
                free((char*)(*iop)->value.s);
            break;
        case IGS_DATA_T:
            if ((*iop)->value.data)
                free((*iop)->value.data);
            break;
        default:
            break;
    }
    if ((*iop)->callbacks){
        igs_observe_callback_t *cb, *tmp;
        DL_FOREACH_SAFE((*iop)->callbacks, cb, tmp){
            DL_DELETE((*iop)->callbacks, cb);
            free(cb);
        }
    }
    free(*iop);
    *iop = NULL;
}

igs_result_t definition_addIOPToDefinition(igs_agent_t *agent, igs_iop_t *iop, iop_t iop_type, igs_definition_t *def){
    assert(agent);
    assert(iop);
    assert(def);
    model_readWriteLock();
    igs_iop_t *previousIOP = NULL;
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
        return IGS_FAILURE;
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
    return IGS_SUCCESS;
}

igs_iop_t* definition_createIOP(igs_agent_t *agent, const char *name, iop_t type, iopType_t value_type, void *value, size_t size){
    assert(agent);
    assert(name);
    assert(agent->definition);
    igs_iop_t *iop = calloc (1, sizeof(igs_iop_t));
    char *n = strndup(name, IGS_MAX_IOP_NAME_LENGTH);
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
        igsAgent_warn(agent, "spaces are not allowed in IOP name: '%s' has been renamed to '%s'", name, n);
    }
    iop->name = n;
    iop->type = type;
    iop->value_type = value_type;
    switch (type) {
        case IGS_INPUT_T:
            if (definition_addIOPToDefinition(agent, iop, IGS_INPUT_T, agent->definition) != IGS_SUCCESS){
                definition_freeIOP(&iop);
                return NULL;
            }
            break;
        case IGS_OUTPUT_T:
            if (definition_addIOPToDefinition(agent, iop, IGS_OUTPUT_T, agent->definition) != IGS_SUCCESS){
                definition_freeIOP(&iop);
                return NULL;
            }
            break;
        case IGS_PARAMETER_T:
            if (definition_addIOPToDefinition(agent, iop, IGS_PARAMETER_T, agent->definition) != IGS_SUCCESS){
                definition_freeIOP(&iop);
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
void definition_freeDefinition (igs_definition_t **def) {
    assert(def);
    assert(*def);
    model_readWriteLock();
    if ((*def)->name != NULL){
        free((char*)(*def)->name);
        (*def)->name = NULL;
    }
    if ((*def)->description != NULL){
        free((char*)(*def)->description);
        (*def)->description = NULL;
    }
    if ((*def)->version != NULL){
        free((char*)(*def)->version);
        (*def)->version = NULL;
    }
    igs_iop_t *current_iop, *tmp_iop;
    HASH_ITER(hh, (*def)->params_table, current_iop, tmp_iop) {
        HASH_DEL((*def)->params_table,current_iop);
        definition_freeIOP(&current_iop);
    }
    HASH_ITER(hh, (*def)->inputs_table, current_iop, tmp_iop) {
        HASH_DEL((*def)->inputs_table,current_iop);
        definition_freeIOP(&current_iop);
    }
    HASH_ITER(hh, (*def)->outputs_table, current_iop, tmp_iop) {
        HASH_DEL((*def)->outputs_table,current_iop);
        definition_freeIOP(&current_iop);
    }
    igs_call_t *call, *tmpCall;
    HASH_ITER(hh, (*def)->calls_table, call, tmpCall) {
        HASH_DEL((*def)->calls_table,call);
        call_freeCall(call);
    }
    model_readWriteUnlock();
    free(*def);
    *def = NULL;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igsAgent_clearDefinition(igs_agent_t *agent){
    assert(agent);
    //Free the structure definition loaded
    igsAgent_debug(agent, "Clear our definition and initiate an empty one");
    if(agent->definition != NULL){
        definition_freeDefinition(&agent->definition);
    }
    model_readWriteLock();
    agent->definition = calloc(1, sizeof(igs_definition_t));
    assert(agent->name);
    agent->definition->name = strdup(agent->name);
    agent->network_needToSendDefinitionUpdate = true;
    model_readWriteUnlock();
}

char* igsAgent_getDefinition(igs_agent_t *agent){
    assert(agent);
    char * def = NULL;
    if(!agent->definition)
        return NULL;
    def = parser_export_definition(agent->definition);
    return def;
}

char *igsAgent_getDefinitionName(igs_agent_t *agent){
    assert(agent);
    assert(agent->definition);
    assert(agent->definition->name);
    return strdup(agent->definition->name);
}

char *igsAgent_getDefinitionDescription(igs_agent_t *agent){
    assert(agent);
    assert(agent->definition);
    if (agent->definition->description){
        return strdup(agent->definition->description);
    }else
        return NULL;
}

char *igsAgent_getDefinitionVersion(igs_agent_t *agent){
    assert(agent);
    assert(agent->definition);
    if ( agent->definition->version){
        return strdup(agent->definition->version);
    }else
        return NULL;
}

void igsAgent_setDefinitionName(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    assert(agent->definition);
    assert(agent->definition->name);
    free(agent->definition->name);
    agent->definition->name = strndup(name, IGS_MAX_DEFINITION_NAME_LENGTH);
    agent->network_needToSendDefinitionUpdate = true;
}

void igsAgent_setDefinitionDescription(igs_agent_t *agent, const char *description){
    assert(agent);
    assert(description);
    assert(agent->definition);
    if(agent->definition->description)
        free(agent->definition->description);
    agent->definition->description = strndup(description, IGS_MAX_DESCRIPTION_LENGTH);
    agent->network_needToSendDefinitionUpdate = true;
}

void igsAgent_setDefinitionVersion(igs_agent_t *agent, const char *version){
    assert(agent);
    assert(version);
    assert(agent->definition);
    if(agent->definition->version != NULL)
        free(agent->definition->version);
    agent->definition->version = strndup(version, 64);
    agent->network_needToSendDefinitionUpdate = true;
}

igs_result_t igsAgent_createInput(igs_agent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    assert(agent);
    assert(name && strlen(name) > 0);
    assert(agent->definition);
    igs_iop_t *iop = definition_createIOP(agent, name, IGS_INPUT_T, value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

igs_result_t igsAgent_createOutput(igs_agent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    assert(agent);
    assert(name && strlen(name) > 0);
    assert(agent->definition);
    igs_iop_t* iop = definition_createIOP(agent, name, IGS_OUTPUT_T, value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

igs_result_t igsAgent_createParameter(igs_agent_t *agent, const char *name, iopType_t value_type, void *value, size_t size){
    assert(agent);
    assert(name && strlen(name) > 0);
    assert(agent->definition);
    igs_iop_t* iop = definition_createIOP(agent, name, IGS_PARAMETER_T, value_type, value, size);
    if (!iop)
        return IGS_FAILURE;
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeInput(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    assert(agent->definition);
    igs_iop_t * iop = model_findIopByName(agent, name,IGS_INPUT_T);
    if(iop == NULL){
        igsAgent_error(agent, "The input %s could not be found", name);
        return IGS_FAILURE;
    }
    model_readWriteLock();
    HASH_DEL(agent->definition->inputs_table, iop);
    definition_freeIOP(&iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeOutput(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    assert(agent->definition);
    igs_iop_t * iop = model_findIopByName(agent, name,IGS_OUTPUT_T);
    if(iop == NULL){
        igsAgent_error(agent, "The output %s could not be found", name);
        return IGS_FAILURE;
    }
    model_readWriteLock();
    HASH_DEL(agent->definition->outputs_table, iop);
    definition_freeIOP(&iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

igs_result_t igsAgent_removeParameter(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name);
    assert(agent->definition);
    igs_iop_t * iop = model_findIopByName(agent, name,IGS_PARAMETER_T);
    if(iop == NULL){
        igsAgent_error(agent, "The parameter %s could not be found", name);
        return IGS_FAILURE;
    }
    model_readWriteLock();
    HASH_DEL(agent->definition->params_table, iop);
    definition_freeIOP(&iop);
    model_readWriteUnlock();
    agent->network_needToSendDefinitionUpdate = true;
    return IGS_SUCCESS;
}

void igsAgent_setDefinitionPath(igs_agent_t *agent, const char *path){
    assert(agent);
    assert(path);
    model_readWriteLock();
    if (agent->definitionPath)
        free(agent->definitionPath);
    agent->definitionPath = strndup(path, IGS_MAX_PATH_LENGTH);
    if (coreContext->networkActor && coreContext->node){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "DEFINITION_FILE_PATH");
        zmsg_addstr(msg, agent->definitionPath);
        zmsg_addstr(msg, agent->uuid);
        zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
        bus_zyreUnlock();
    }
    model_readWriteUnlock();
}

void igsAgent_writeDefinitionToPath(igs_agent_t *agent){
    assert(agent);
    assert(agent->definition);
    if (!agent->definitionPath){
        igsAgent_error(agent, "no path configured to save definition");
        return;
    }
    model_readWriteLock();
    FILE *fp = NULL;
    fp = fopen (agent->definitionPath,"w+");
    igsAgent_info(agent, "save to path %s", agent->definitionPath);
    if (fp == NULL){
        igsAgent_error(agent, "Could not open '%s' for writing", agent->definitionPath);
    }else{
        char *def = parser_export_definition(agent->definition);
        assert(def);
        fprintf(fp, "%s", def);
        fflush(fp);
        fclose(fp);
        free(def);
    }
    model_readWriteUnlock();
}
