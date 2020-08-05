//
//  agent.c
//  ingescape
//
//  Created by Stephane Vales on 19/11/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "ingescape_private.h"
#include "ingescape_agent.h"

void agent_propagateAgentEvent(igs_agent_event_t event, const char *uuid, const char *name){
    //propagate event on all local agents
    igs_agent_t *agent, *tmp;
    HASH_ITER(hh, coreContext->agents, agent, tmp){
        igs_agent_event_callback_t *cb;
        DL_FOREACH(agent->agentEventCallbacks, cb){
            cb->callback_ptr(agent, event, uuid, name, cb->myData);
        }
    }
}

igs_agent_t *igsAgent_new(const char *name, bool activateImmediately){
    core_initContext();
    assert(name);
    igs_agent_t *agent = calloc(1, sizeof(igs_agent_t));
    zuuid_t *uuid = zuuid_new();
    agent->uuid = strdup(zuuid_str(uuid));
    zuuid_destroy(&uuid);
    agent->name = strndup((name == NULL)?IGS_DEFAULT_AGENT_NAME:name, IGS_MAX_AGENT_NAME_LENGTH);
    zhash_insert(coreContext->createdAgents, agent->uuid, agent);
    if (activateImmediately)
        igsAgent_activate(agent);
    return agent;
}

void igsAgent_destroy(igs_agent_t **agent){
    assert(agent);
    assert(*agent);
    if (igsAgent_isActivated(*agent))
        igsAgent_deactivate(*agent);
    
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
    
    igs_activate_calback_t *activateCb, *activatetmp;
    DL_FOREACH_SAFE((*agent)->activateCallbacks, activateCb, activatetmp){
        DL_DELETE((*agent)->activateCallbacks, activateCb);
        free(activateCb);
    }
    igs_mute_callback_t *muteCb, *mutetmp;
    DL_FOREACH_SAFE((*agent)->muteCallbacks, muteCb, mutetmp){
        DL_DELETE((*agent)->muteCallbacks, muteCb);
        free(muteCb);
    }
    igs_agent_event_callback_t *eventCb, *eventtmp;
    DL_FOREACH_SAFE((*agent)->agentEventCallbacks, eventCb, eventtmp){
        DL_DELETE((*agent)->agentEventCallbacks, eventCb);
        free(eventCb);
    }
    if ((*agent)->mapping)
        mapping_freeMapping(&(*agent)->mapping);
    if ((*agent)->definition)
        definition_freeDefinition(&(*agent)->definition);
    zhash_delete(coreContext->createdAgents, (*agent)->uuid);
    free(*agent);
    *agent = NULL;
}

igs_result_t igsAgent_activate(igs_agent_t *agent){
    assert(agent);
    igs_agent_t *a = NULL;
    HASH_FIND_STR(coreContext->agents, agent->uuid, a);
    if (a != NULL){
        igs_error("agent %s (%s) is already activated", agent->name, agent->uuid);
        return IGS_FAILURE;
    }else{
        agent->context = coreContext;
        HASH_ADD_STR(coreContext->agents, uuid, agent);
        igs_activate_calback_t *cb;
        DL_FOREACH(agent->activateCallbacks, cb){
            cb->callback_ptr(agent, true, cb->myData);
        }
        agent->network_needToSendDefinitionUpdate = true; //will also trigger mapping update
    }
    agent_propagateAgentEvent(IGS_AGENT_ENTERED, agent->uuid, agent->name);
    return IGS_SUCCESS;
}

igs_result_t igsAgent_deactivate(igs_agent_t *agent){
    assert(agent);
    igs_agent_t *a = NULL;
    HASH_FIND_STR(coreContext->agents, agent->uuid, a);
    if (a != NULL){
        HASH_DEL(coreContext->agents, agent);
        agent->context = NULL;
        igs_activate_calback_t *cb;
        DL_FOREACH(agent->activateCallbacks, cb){
            cb->callback_ptr(agent, false, cb->myData);
        }
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
    agent_propagateAgentEvent(IGS_AGENT_EXITED, agent->uuid, agent->name);
    return IGS_SUCCESS;
}

bool igsAgent_isActivated(igs_agent_t *agent){
    assert(agent);
    igs_agent_t *a = NULL;
    HASH_FIND_STR(coreContext->agents, agent->uuid, a);
    if (a != NULL){
        return true;
    }else{
        return false;
    }
}

void igsAgent_observeActivate(igs_agent_t *agent, igsAgent_activateCallback cb, void *myData){
    assert(agent);
    assert(cb);
    igs_activate_calback_t *newCb = calloc(1, sizeof(igs_activate_calback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(agent->activateCallbacks, newCb);
}

void igsAgent_observeAgentEvents(igs_agent_t *agent, igsAgent_agentEventCallback cb, void *myData){
    assert(agent);
    assert(cb);
    core_initContext();
    igs_agent_event_callback_t *newCb = calloc(1, sizeof(igs_agent_event_callback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(agent->agentEventCallbacks, newCb);
}

void igsAgent_log(igs_logLevel_t level, const char *function, igs_agent_t *agent, const char *format, ...){
    assert(function);
    assert(agent);
    assert(format);
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(agent, level, function, "%s", content);
}
