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
void igsAgent_busJoinChannel(igs_agent_t *agent, const char *channel){
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igsAgent_error(agent, "channel name %s is reserved and cannot be joined", channel);
        return;
    }
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_join(coreContext->node, channel);
        bus_zyreUnlock();
    }else{
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before joining a channel");
    }
}
void igsAgent_busLeaveChannel(igs_agent_t *agent, const char *channel){
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_leave(coreContext->node, channel);
        bus_zyreUnlock();
    }else{
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before leaving a channel");
    }
}

int igsAgent_busSendStringToChannel(igs_agent_t *agent, const char *channel, const char *msg, ...){
    if (channel == NULL){
        igsAgent_debug(agent, "channel is NULL");
        return 0;
    }
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igsAgent_error(agent, "channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    char content[MAX_STRING_MSG_LENGTH] = "";
    va_list list;
    va_start(list, msg);
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    bus_zyreLock();
    if (zyre_shouts(coreContext->node, channel, "%s", content) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igsAgent_busSendDataToChannel(igs_agent_t *agent, const char *channel, void *data, size_t size){
    if (channel == NULL){
        igsAgent_debug(agent, "channel is NULL");
        return 0;
    }
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igsAgent_error(agent, "channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    zframe_t *frame = zframe_new(data, size);
    zmsg_t *msg = zmsg_new();
    zmsg_append(msg, &frame);
    bus_zyreLock();
    if(zyre_shout(coreContext->node, channel, &msg) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igsAgent_busSendZMQMsgToChannel(igs_agent_t *agent, const char *channel, zmsg_t **msg_p){
    if (channel == NULL){
        igsAgent_debug(agent, "channel is NULL");
        return 0;
    }
    if (strcmp(IGS_PRIVATE_CHANNEL, channel) == 0){
        igsAgent_error(agent, "channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    bus_zyreLock();
    if (zyre_shout(coreContext->node, channel, msg_p) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igsAgent_busSendStringToAgent(igs_agent_t *agent, const char *agentNameOrPeerID, const char *msg, ...){
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igsAgent_debug(agent, "agentNameOrPeerID is NULL");
        return 0;
    }
    igs_zyre_peer_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            char content[MAX_STRING_MSG_LENGTH] = "";
            va_list list;
            va_start(list, msg);
            vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
            va_end(list);
            bus_zyreLock();
            if (zyre_whispers(coreContext->node, el->peerId, "%s", content) != 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    return res;
}

int igsAgent_busSendDataToAgent(igs_agent_t *agent, const char *agentNameOrPeerID, void *data, size_t size){
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igsAgent_debug(agent, "agentNameOrPeerID is NULL");
        return 0;
    }
    igs_zyre_peer_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zframe_t *frame = zframe_new(data, size);
            zmsg_t *msg = zmsg_new();
            zmsg_append(msg, &frame);
            bus_zyreLock();
            if (zyre_whisper(coreContext->node, el->peerId, &msg) != 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    return res;
}

int igsAgent_busSendZMQMsgToAgent(igs_agent_t *agent, const char *agentNameOrPeerID, zmsg_t **msg_p){
    if (coreContext == NULL || coreContext->node == NULL){
        igsAgent_error(agent, "igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igsAgent_debug(agent, "agentNameOrPeerID is NULL");
        return 0;
    }
    igs_zyre_peer_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, coreContext->zyrePeers, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zmsg_t *msg = zmsg_dup(*msg_p);
            bus_zyreLock();
            if (zyre_whisper(coreContext->node, el->peerId, &msg) != 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    zmsg_destroy(msg_p);
    return res;
}

void igsAgent_busAddServiceDescription(igs_agent_t *agent, const char *key, const char *value){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        igs_service_header_t *header;
        HASH_FIND_STR(coreContext->serviceHeaders, key, header);
        if (header != NULL){
            igsAgent_error(agent, "service key '%s' already defined : new value will be ignored", key);
            return;
        }
        header = calloc(1, sizeof(igs_service_header_t));
        header->key = strndup(key, MAX_STRING_MSG_LENGTH);
        header->value = strndup(value, MAX_STRING_MSG_LENGTH * 16);
        HASH_ADD_STR(coreContext->serviceHeaders, key, header);
    }else{
        igsAgent_warn(agent, "service key '%s' is reserved and cannot be edited", key);
    }
}

void igsAgent_busRemoveServiceDescription(igs_agent_t *agent, const char *key){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        igs_service_header_t *header;
        HASH_FIND_STR(coreContext->serviceHeaders, key, header);
        if (header == NULL){
            igsAgent_warn(agent, "service key '%s' does not exist", key);
            return;
        }
        HASH_DEL(coreContext->serviceHeaders, header);
        free(header->key);
        free(header->value);
        free(header);
        if (coreContext != NULL && coreContext->node != NULL){
            igsAgent_warn(agent, "agent is started : restart the agent to actually remove the service");
        }
    }else{
        igsAgent_warn(agent, "service key '%s' is reserved and cannot be removed", key);
    }
}
