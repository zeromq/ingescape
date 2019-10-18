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

serviceHeader_t *serviceHeaders = NULL;

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
            igs_error("mutex init failed");
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (bus_zyreMutex == NULL){
        if (pthread_mutex_init(&bus_zyreMutex) != 0){
            igs_error("mutex init failed");
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
        igs_error("mutex was NULL");
    }
}

////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////
void igs_busJoinChannel(const char *channel){
    if (strcmp(CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be joined", channel);
        return;
    }
    if (agentElements != NULL && agentElements->node != NULL){
        bus_zyreLock();
        zyre_join(agentElements->node, channel);
        bus_zyreUnlock();
    }else{
        igs_error("igs_startWithDevice or igs_startWithIP must be called before joining a channel");
    }
}
void igs_busLeaveChannel(const char *channel){
    if (agentElements != NULL && agentElements->node != NULL){
        bus_zyreLock();
        zyre_leave(agentElements->node, channel);
        bus_zyreUnlock();
    }else{
        igs_error("igs_startWithDevice or igs_startWithIP must be called before leaving a channel");
    }
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    if (channel == NULL){
        igs_debug("channel is NULL");
        return 0;
    }
    if (strcmp(CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    char content[MAX_STRING_MSG_LENGTH] = "";
    va_list list;
    va_start(list, msg);
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    bus_zyreLock();
    if (zyre_shouts(agentElements->node, channel, "%s", content) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    if (channel == NULL){
        igs_debug("channel is NULL");
        return 0;
    }
    if (strcmp(CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    zframe_t *frame = zframe_new(data, size);
    zmsg_t *msg = zmsg_new();
    zmsg_append(msg, &frame);
    bus_zyreLock();
    if(zyre_shout(agentElements->node, channel, &msg) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    if (channel == NULL){
        igs_debug("channel is NULL");
        return 0;
    }
    if (strcmp(CHANNEL, channel) == 0){
        igs_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    int res = 1;
    bus_zyreLock();
    if (zyre_shout(agentElements->node, channel, msg_p) != 0)
        res = -1;
    bus_zyreUnlock();
    return res;
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igs_debug("agentNameOrPeerID is NULL");
        return 0;
    }
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            char content[MAX_STRING_MSG_LENGTH] = "";
            va_list list;
            va_start(list, msg);
            vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
            va_end(list);
            bus_zyreLock();
            if (zyre_whispers(agentElements->node, el->peerId, "%s", content) != 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igs_debug("agentNameOrPeerID is NULL");
        return 0;
    }
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zframe_t *frame = zframe_new(data, size);
            zmsg_t *msg = zmsg_new();
            zmsg_append(msg, &frame);
            bus_zyreLock();
            if (zyre_whisper(agentElements->node, el->peerId, &msg) <= 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    return res;
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    if (agentElements == NULL || agentElements->node == NULL){
        igs_error("igs_startWithDevice or igs_startWithIP must be called before trying to send a message");
        return -1;
    }
    if (agentNameOrPeerID == NULL){
        igs_debug("agentNameOrPeerID is NULL");
        return 0;
    }
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zmsg_t *msg = zmsg_dup(*msg_p);
            bus_zyreLock();
            if (zyre_whisper(agentElements->node, el->peerId, &msg) <= 0)
                res = -1;
            bus_zyreUnlock();
        }
    }
    zmsg_destroy(msg_p);
    return res;
}

void igs_busAddServiceDescription(const char *key, const char *value){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "canBeFrozen") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        serviceHeader_t *header;
        HASH_FIND_STR(serviceHeaders, key, header);
        if (header != NULL){
            igs_error("service key '%s' already defined : new value will be ignored", key);
            return;
        }
        header = calloc(1, sizeof(serviceHeader_t));
        header->key = strndup(key, MAX_STRING_MSG_LENGTH);
        header->value = strndup(value, MAX_STRING_MSG_LENGTH * 16);
        HASH_ADD_STR(serviceHeaders, key, header);
    }else{
        igs_warn("service key '%s' is reserved and cannot be edited", key);
    }
}

void igs_busRemoveServiceDescription(const char *key){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "canBeFrozen") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0)
    {
        serviceHeader_t *header;
        HASH_FIND_STR(serviceHeaders, key, header);
        if (header == NULL){
            igs_warn("service key '%s' does not exist", key);
            return;
        }
        HASH_DEL(serviceHeaders, header);
        free(header->key);
        free(header->value);
        free(header);
        if (agentElements != NULL && agentElements->node != NULL){
            igs_warn("agent is started : restart the agent to actually remove the service");
        }
    }else{
        igs_warn("service key '%s' is reserved and cannot be removed", key);
    }
}
