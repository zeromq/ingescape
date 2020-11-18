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

void agent_propagateAgentEvent(igs_agent_event_t event, const char *uuid, const char *name, void *eventData){
    //propagate event on all local agents
    igs_agent_t *agent, *tmp;
    HASH_ITER(hh, coreContext->agents, agent, tmp){
        if (!streq(uuid, agent->uuid)){
            igs_agent_event_callback_t *cb;
            DL_FOREACH(agent->agentEventCallbacks, cb){
                cb->callback_ptr(agent, event, uuid, name, eventData, cb->myData);
            }
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
    agent->name = strndup(name, IGS_MAX_AGENT_NAME_LENGTH);
    zhash_insert(coreContext->createdAgents, agent->uuid, agent);
    igsAgent_clearDefinition(agent); //set valid but empty definition, only with name equal to agent name
    igsAgent_clearMapping(agent); //set valid but empty mapping
    if (activateImmediately)
        igsAgent_activate(agent);
    return agent;
}

void igsAgent_destroy(igs_agent_t **agent){
    assert(agent);
    assert(*agent);
    model_readWriteLock();
    if (igsAgent_isActivated(*agent))
        igsAgent_deactivate(*agent);
    
    zhash_delete(coreContext->createdAgents, (*agent)->uuid);
    if ((*agent)->uuid != NULL) {
        free((*agent)->uuid);
        (*agent)->uuid = NULL;
    }
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
    
    model_readWriteUnlock();
    if ((*agent)->definition)
        definition_freeDefinition(&(*agent)->definition);
    model_readWriteLock();
    free(*agent);
    *agent = NULL;
    model_readWriteUnlock();
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
        agent->network_activationDuringRuntime = true;
    }
    
    //notify all other agents inside this context that we have arrived
    agent_propagateAgentEvent(IGS_AGENT_ENTERED, agent->uuid, agent->name, NULL);
    agent_propagateAgentEvent(IGS_AGENT_KNOWS_US, agent->uuid, agent->name, NULL);
    
    //notify this agent with all the other agents already present in our context locally and remotely
    igs_agent_t *tmp;
    HASH_ITER(hh, coreContext->agents, a, tmp){
        if (!streq(a->uuid, agent->uuid)){
            igs_agent_event_callback_t *cb;
            DL_FOREACH(agent->agentEventCallbacks, cb){
                cb->callback_ptr(agent, IGS_AGENT_ENTERED, a->uuid, a->name, NULL, cb->myData);
                //in our local context, other agents already know us
                cb->callback_ptr(agent, IGS_AGENT_KNOWS_US, a->uuid, a->name, NULL, cb->myData);
            }
        }
    }
    igs_remote_agent_t *r, *rtmp;
    HASH_ITER(hh, coreContext->remoteAgents, r, rtmp){
        igs_agent_event_callback_t *cb;
        DL_FOREACH(agent->agentEventCallbacks, cb){
            cb->callback_ptr(agent, IGS_AGENT_ENTERED, r->uuid, r->name, NULL, cb->myData);
        }
    }
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
            bus_zyreLock();
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "REMOTE_AGENT_EXIT");
            zmsg_addstr(msg, agent->uuid);
            zmsg_addstr(msg, agent->name);
            zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            bus_zyreUnlock();
        }
    }else{
        igs_error("agent %s (%s) is not activated", agent->name, agent->uuid);
        return IGS_FAILURE;
    }
    agent_propagateAgentEvent(IGS_AGENT_EXITED, agent->uuid, agent->name, NULL);
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
