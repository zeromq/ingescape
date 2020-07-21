//
//  agent.c
//  ingescape
//
//  Created by Stephane Vales on 19/11/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "ingescape_private.h"
#include "ingescape_agent.h"

igs_agent_t *igsAgent_new(const char *name, bool activateImmediately){
    igs_agent_t *agent = calloc(1, sizeof(igs_agent_t));
    zuuid_t *uuid = zuuid_new();
    agent->uuid = strdup(zuuid_str(uuid));
    zuuid_destroy(&uuid);
    agent->name = strndup((name == NULL)?IGS_DEFAULT_AGENT_NAME:name, IGS_MAX_AGENT_NAME_LENGTH);
    if (activateImmediately)
        igsAgent_activate(agent);
    return agent;
}

void igsAgent_destroy(igs_agent_t **agent){
    igsAgent_activate(*agent);
    if ((*agent)->uuid != NULL)
        free((*agent)->uuid);
    if ((*agent)->name != NULL)
        free((*agent)->name);
    if ((*agent)->state != NULL)
        free((*agent)->state);
    if ((*agent)->definitionPath != NULL)
        free((*agent)->definitionPath);
    if ((*agent)->mappingPath != NULL)
        free((*agent)->mappingPath);
    
    igs_forced_stop_calback_t *forceCb, *tmp;
    DL_FOREACH_SAFE((*agent)->forcedStopCalbacks, forceCb, tmp){
        DL_DELETE((*agent)->forcedStopCalbacks, forceCb);
        free(forceCb);
    }
    igs_mute_callback_t *muteCb, *mutetmp;
    DL_FOREACH_SAFE((*agent)->muteCallbacks, muteCb, mutetmp){
        DL_DELETE((*agent)->muteCallbacks, muteCb);
        free(muteCb);
    }
    if ((*agent)->mapping)
        mapping_freeMapping(&(*agent)->mapping);
    if ((*agent)->definition)
        definition_freeDefinition(&(*agent)->definition);
    
    free(*agent);
    *agent = NULL;
}

int igsAgent_activate(igs_agent_t *agent){
    assert(agent);
    core_initContext();
    igs_agent_t *a = NULL;
    HASH_FIND_STR(coreContext->agents, agent->uuid, a);
    if (a != NULL){
        igs_error("agent %s (%s) is already activated", agent->name, agent->uuid);
        return IGS_FAILURE;
    }else{
        agent->context = coreContext;
        HASH_ADD_STR(coreContext->agents, uuid, agent);
        agent->network_needToSendDefinitionUpdate = true; //will also trigger mapping update
    }
    return IGS_SUCCESS;
}

int igsAgent_deactivate(igs_agent_t *agent){
    assert(agent);
    core_initContext();
    igs_agent_t *a = NULL;
    HASH_FIND_STR(coreContext->agents, agent->uuid, a);
    if (a != NULL){
        HASH_DEL(coreContext->agents, agent);
        agent->context = NULL;
        if (coreContext->networkActor && coreContext->node){
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "REMOTE_AGENT_EXIT");
            zmsg_addstr(msg, agent->uuid);
            zmsg_addstr(msg, agent->name);
            zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
        }
    }else{
        igs_error("agent %s (%s) is not activated", agent->name, agent->uuid);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void igsAgent_log(igs_logLevel_t level, const char *function, igs_agent_t *agent, const char *format, ...){
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(agent, level, function, "%s", content);
}
