//
//  bus.c
//  mastic
//
//  Created by Stephane Vales on 08/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include "mastic_private.h"
#include "mastic_advanced.h"

serviceHeader_t *serviceHeaders = NULL;

void mtic_busJoinChannel(const char *channel){
    if (strcmp(CHANNEL, channel) == 0){
        mtic_error("channel name %s is reserved and cannot be joined", channel);
        return;
    }
    zyre_join(agentElements->node, channel);
}
void mtic_busLeaveChannel(const char *channel){
    zyre_leave(agentElements->node, channel);
}

int mtic_busSendStringToChannel(const char *channel, const char *msg, ...){
    if (strcmp(CHANNEL, channel) == 0){
        mtic_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    int res = 1;
    char content[MAX_STRING_MSG_LENGTH] = "";
    va_list list;
    va_start(list, msg);
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    if (zyre_shouts(agentElements->node, channel, "%s", content) != 0)
        res = -1;
    return res;
}

int mtic_busSendDataToChannel(const char *channel, void *data, long size){
    if (strcmp(CHANNEL, channel) == 0){
        mtic_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    int res = 1;
    zframe_t *frame = zframe_new(data, size);
    zmsg_t *msg = zmsg_new();
    zmsg_append(msg, &frame);
    if(zyre_shout(agentElements->node, channel, &msg) != 0)
        res = -1;
    return res;
}

int mtic_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    if (strcmp(CHANNEL, channel) == 0){
        mtic_error("channel name %s is reserved and cannot be used", channel);
        return -1;
    }
    int res = 1;
    if (zyre_shout(agentElements->node, channel, msg_p) != 0)
        res = -1;
    return res;
}

int mtic_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            char content[MAX_STRING_MSG_LENGTH] = "";
            va_list list;
            va_start(list, msg);
            vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
            va_end(list);
            if (zyre_whispers(agentElements->node, el->peerId, "%s", content) != 0)
                res = -1;
        }
    }
    return res;
}

int mtic_busSendDataToAgent(const char *agentNameOrPeerID, void *data, unsigned long size){
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zframe_t *frame = zframe_new(data, size);
            zmsg_t *msg = zmsg_new();
            zmsg_append(msg, &frame);
            if (zyre_whisper(agentElements->node, el->peerId, &msg) <= 0)
                res = -1;
        }
    }
    return res;
}

int mtic_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    zyreAgent_t *el, *tmp;
    int res = 1;
    HASH_ITER(hh, zyreAgents, el, tmp){
        if (strcmp(el->name, agentNameOrPeerID) == 0 || strcmp(el->peerId, agentNameOrPeerID) == 0){
            zmsg_t *msg = zmsg_dup(*msg_p);
            if (zyre_whisper(agentElements->node, el->peerId, &msg) <= 0)
                res = -1;
        }
    }
    zmsg_destroy(msg_p);
    return res;
}

void mtic_busAddServiceDescription(const char *key, const char *value){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "canBeFrozen") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0
        && strcmp(key, "isEditor") != 0)
    {
        serviceHeader_t *header;
        HASH_FIND_STR(serviceHeaders, key, header);
        if (header != NULL){
            mtic_error("service key '%s' already defined : new value will be ignored", key);
            return;
        }
        header = calloc(1, sizeof(serviceHeader_t));
        header->key = strndup(key, MAX_STRING_MSG_LENGTH);
        header->value = strndup(value, MAX_STRING_MSG_LENGTH);
        HASH_ADD_STR(serviceHeaders, key, header);
    }else{
        mtic_warn("service key '%s' is reserved and cannot be edited", key);
    }
}

void mtic_busRemoveServiceDescription(const char *key){
    if (strcmp(key, "publisher") != 0
        && strcmp(key, "logger") != 0
        && strcmp(key, "canBeFrozen") != 0
        && strcmp(key, "pid") != 0
        && strcmp(key, "commandline") != 0
        && strcmp(key, "hostname") != 0
        && strcmp(key, "isEditor") != 0)
    {
        serviceHeader_t *header;
        HASH_FIND_STR(serviceHeaders, key, header);
        if (header == NULL){
            mtic_warn("service key '%s' does not exist");
            return;
        }
        HASH_DEL(serviceHeaders, header);
        free(header->key);
        free(header->value);
        free(header);
        if (agentElements != NULL && agentElements->node != NULL){
            mtic_warn("agent is started : restart the agent to actually remove the service");
        }
    }else{
        mtic_warn("service key '%s' is reserved and cannot be removed", key);
    }
}
