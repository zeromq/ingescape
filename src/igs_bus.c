//
//  bus.c
//  ingescape
//
//  Created by Stephane Vales on 08/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include "ingescape_private.h"
#include "ingescape_advanced.h"
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *bus_zyreMutex = NULL;
#else
#define W_OK 02
pthread_mutex_t bus_zyreMutex = NULL;
#endif

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void bus_zyreLock(void)   {
#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
    if (bus_zyreMutex == NULL){
        bus_zyreMutex = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(bus_zyreMutex, NULL) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (bus_zyreMutex == NULL){
        if (pthread_mutex_init(&bus_zyreMutex) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#endif
    pthread_mutex_lock(bus_zyreMutex);
}

void bus_zyreUnlock(void) {
    if (bus_zyreMutex != NULL){
        pthread_mutex_unlock(bus_zyreMutex);
    }else{
        igs_fatal("mutex was NULL\n");
        assert(false);
    }
}

////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////
igs_result_t igs_busJoinChannel(const char *channel){
    core_initContext();
    assert(channel && strlen(channel) > 0);
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be joined", channel);
        return IGS_FAILURE;
    }
    if (coreContext->networkActor != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_join(coreContext->node, channel);
        bus_zyreUnlock();
    }else{
        igs_error("Ingescape must be started before joining a channel");
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void igs_busLeaveChannel(const char *channel){
    core_initContext();
    assert(channel && strlen(channel) > 0);
    if (coreContext->networkActor != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_leave(coreContext->node, channel);
        bus_zyreUnlock();
    }else{
        igs_warn("Ingescape is not started, this command will be useless");
    }
}

igs_result_t igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    core_initContext();
    assert(channel);
    assert(msg);
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    int res = IGS_SUCCESS;
    char content[MAX_STRING_MSG_LENGTH] = "";
    va_list list;
    va_start(list, msg);
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    bus_zyreLock();
    if (zyre_shouts(coreContext->node, channel, "%s", content) != 0)
        res = IGS_FAILURE;
    bus_zyreUnlock();
    return res;
}

igs_result_t igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    core_initContext();
    assert(channel);
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    int res = IGS_SUCCESS;
    zframe_t *frame = zframe_new(data, size);
    zmsg_t *msg = zmsg_new();
    zmsg_append(msg, &frame);
    bus_zyreLock();
    if(zyre_shout(coreContext->node, channel, &msg) != 0)
        res = IGS_FAILURE;
    bus_zyreUnlock();
    return res;
}

igs_result_t igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    core_initContext();
    assert(channel);
    assert(msg_p);
    assert(*msg_p);
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    int res = IGS_SUCCESS;
    bus_zyreLock();
    if (zyre_shout(coreContext->node, channel, msg_p) != 0)
        res = IGS_FAILURE;
    bus_zyreUnlock();
    return res;
}

igs_result_t igs_busSendStringToAgent(const char *agentNameOrAgentIdOrPeerID, const char *msg, ...){
    core_initContext();
    assert(agentNameOrAgentIdOrPeerID);
    assert(msg);
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool hasSent = false;
    int res = IGS_SUCCESS;
    //we iterate first on agents
    igs_remote_agent_t *agent, *tmpAgent;
    HASH_ITER(hh, coreContext->remoteAgents, agent, tmpAgent){
        if (streq(agent->definition->name, agentNameOrAgentIdOrPeerID) || streq(agent->uuid, agentNameOrAgentIdOrPeerID)){
            char content[MAX_STRING_MSG_LENGTH] = "";
            va_list list;
            va_start(list, msg);
            vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
            va_end(list);
            bus_zyreLock();
            zmsg_t *msgToSend = zmsg_new();
            zmsg_addstr(msgToSend, content);
            zmsg_addstr(msgToSend, agent->uuid);
            if (zyre_whisper(coreContext->node, agent->peer->peerId, &msgToSend) != 0)
                res = IGS_FAILURE;
            bus_zyreUnlock();
            hasSent = true;
            //NB: no break to support multiple agents with same name
        }
    }
    
    //if no agent found, we iterate on peers
    if (!hasSent){
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
            if (strcmp(el->name, agentNameOrAgentIdOrPeerID) == 0 || strcmp(el->peerId, agentNameOrAgentIdOrPeerID) == 0){
                char content[MAX_STRING_MSG_LENGTH] = "";
                va_list list;
                va_start(list, msg);
                vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
                va_end(list);
                bus_zyreLock();
                if (zyre_whispers(coreContext->node, el->peerId, "%s", content) != 0)
                    res = IGS_FAILURE;
                bus_zyreUnlock();
            }
        }
    }
    return res;
}

igs_result_t igs_busSendDataToAgent(const char *agentNameOrAgentIdOrPeerID, void *data, size_t size){
    core_initContext();
    assert(agentNameOrAgentIdOrPeerID);
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool hasSent = false;
    int res = IGS_SUCCESS;
    //we iterate first on agents
    igs_remote_agent_t *agent, *tmpAgent;
    HASH_ITER(hh, coreContext->remoteAgents, agent, tmpAgent){
        if (streq(agent->definition->name, agentNameOrAgentIdOrPeerID) || streq(agent->uuid, agentNameOrAgentIdOrPeerID)){
            zframe_t *frame = zframe_new(data, size);
            zmsg_t *msg = zmsg_new();
            zmsg_append(msg, &frame);
            zmsg_addstr(msg, agent->uuid);
            bus_zyreLock();
            if (zyre_whisper(coreContext->node, agent->peer->peerId, &msg) != 0)
                res = IGS_FAILURE;
            bus_zyreUnlock();
            hasSent = true;
            //NB: no break to support multiple agents with same name
        }
    }
    
    //if no agent found, we iterate on peers
    if (!hasSent){
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
            if (strcmp(el->name, agentNameOrAgentIdOrPeerID) == 0 || strcmp(el->peerId, agentNameOrAgentIdOrPeerID) == 0){
                zframe_t *frame = zframe_new(data, size);
                zmsg_t *msg = zmsg_new();
                zmsg_append(msg, &frame);
                bus_zyreLock();
                if (zyre_whisper(coreContext->node, el->peerId, &msg) != 0)
                    res = IGS_FAILURE;
                bus_zyreUnlock();
            }
        }
    }
    return res;
}

igs_result_t igs_busSendZMQMsgToAgent(const char *agentNameOrAgentIdOrPeerID, zmsg_t **msg_p){
    core_initContext();
    assert(agentNameOrAgentIdOrPeerID);
    assert(msg_p);
    assert(*msg_p);
    if (coreContext->node == NULL){
        igs_error("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool hasSent = false;
    int res = IGS_SUCCESS;
    //we iterate first on agents
    igs_remote_agent_t *agent, *tmpAgent;
    HASH_ITER(hh, coreContext->remoteAgents, agent, tmpAgent){
        if (streq(agent->definition->name, agentNameOrAgentIdOrPeerID) || streq(agent->uuid, agentNameOrAgentIdOrPeerID)){
            zmsg_t *dup = zmsg_dup(*msg_p);
            zmsg_addstr(dup, agent->uuid); //add agent uuid at the end of the message
            bus_zyreLock();
            if (zyre_whisper(coreContext->node, agent->peer->peerId, &dup) != 0)
                res = IGS_FAILURE;
            bus_zyreUnlock();
            hasSent = true;
            //NB: no break to support multiple agents with same name
        }
    }
    
    //if no agent found, we iterate on peers
    if (!hasSent){
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
            if (strcmp(el->name, agentNameOrAgentIdOrPeerID) == 0 || strcmp(el->peerId, agentNameOrAgentIdOrPeerID) == 0){
                zmsg_t *dup = zmsg_dup(*msg_p);
                bus_zyreLock();
                if (zyre_whisper(coreContext->node, el->peerId, &dup) != 0)
                    res = IGS_FAILURE;
                bus_zyreUnlock();
                hasSent = true;
            }
        }
    }
    
    if (hasSent)
        zmsg_destroy(msg_p);
    
    return res;
}

igs_result_t igs_busAddServiceDescription(const char *key, const char *value){
    core_initContext();
    assert(key);
    assert(value);
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        igs_service_header_t *header;
        HASH_FIND_STR(coreContext->serviceHeaders, key, header);
        if (header != NULL){
            igs_error("service key '%s' already defined : new value will be ignored", key);
            return IGS_FAILURE;
        }
        header = calloc(1, sizeof(igs_service_header_t));
        header->key = strndup(key, MAX_STRING_MSG_LENGTH);
        header->value = strndup(value, MAX_STRING_MSG_LENGTH * 16);
        HASH_ADD_STR(coreContext->serviceHeaders, key, header);
    }else{
        igs_error("service key '%s' is reserved and cannot be edited", key);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

igs_result_t igs_busRemoveServiceDescription(const char *key){
    core_initContext();
    assert(key);
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        igs_service_header_t *header;
        HASH_FIND_STR(coreContext->serviceHeaders, key, header);
        if (header == NULL){
            igs_error("service key '%s' does not exist", key);
            return IGS_FAILURE;
        }
        HASH_DEL(coreContext->serviceHeaders, header);
        free(header->key);
        free(header->value);
        free(header);
        if (coreContext != NULL && coreContext->node != NULL){
            igs_error("agent is started : restart the agent to actually remove the service description");
            return IGS_FAILURE;
        }
    }else{
        igs_error("service key '%s' is reserved and cannot be removed", key);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}
