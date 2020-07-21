/*
 *    INGEPROBE
 *
 *  Copyright (c) 2016-2020 Ingenuity i/o. All rights reserved.
 *
 *    See license terms for the rights and conditions
 *    defined by copyright holders.
 *
 *
 *    Contributors:
 *      Stephane Vales <vales@ingenuity.io>
 *
 *
 */


#include <zyre.h>
#include <zyre_event.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "uthash/uthash.h"
#include <getopt.h>
#include "call.h"

#define UNUSED(x) (void)x;
#define BUFFER_SIZE 1024

//global application options
int port = 5670;
const char *name = "ingeprobe";
char *netdevice = NULL;
bool verbose = false;
bool keepRunning = false;
bool proxy = false;
const char *gossipbind = NULL;//"tcp://10.0.0.7:12345";
const char *gossipconnect = NULL;
const char *endpoint = NULL;

char *outputTypes[] = {"INT", "DOUBLE", "STRING", "BOOL", "IMPULSION", "DATA"};

//for message passed as run parameter
char *paramMessage = NULL;

//data storage
typedef struct peer peer_t;
typedef struct agent agent_t;

typedef struct context{
    char *name;
    zyre_t *node;
    peer_t *peers;
    agent_t *agents;
    bool useGossip;
} context_t;

#define NAME_BUFFER_SIZE 256
typedef struct peer {
    char *uuid;
    char *name;
    char *endpoint;
    int reconnected;
    char *publisherPort;
    char *logPort;
    char *protocol;
    zsock_t *subscriber;
    zmq_pollitem_t *subscriberPoller;
    zsock_t *logger;
    zmq_pollitem_t *loggerPoller;
    context_t *context;
    UT_hash_handle hh;
} peer_t;

typedef struct agent{
    char *uuid;
    char *name;
    peer_t *peer;
    call_t *calls;
    UT_hash_handle hh;
} agent_t;

///////////////////////////////////////////////////////////////////////////////
// ZYRE AGENT MANAGEMENT
//


//manage incoming messages from one of the publisher agent we subscribed to
int manageSubscription (zloop_t *loop, zsock_t *socket, void *arg){
    UNUSED(loop);
    agent_t *agent = (agent_t *)arg;
    zmsg_t *msg = zmsg_recv(socket);
    size_t s = zmsg_size(msg);
    char *outputName = NULL;
    char *string = NULL;
    zframe_t *frame = NULL;
    void *data = NULL;
    char uuid[33] = ""; //33 is UUID length + terminal 0
    size_t size = 0;
    iopType_t type = 0;
    printf("%s published : ", agent->name);
    
    for (size_t i = 0; i < s; i += 3){
        outputName = zmsg_popstr(msg);
        if (streq(agent->peer->protocol, "v2")){
            //in v2 protocol, publication message starts with agent identification
            snprintf(uuid, 33, "%s", outputName);
            agent_t *a = NULL;
            HASH_FIND_STR(agent->peer->context->agents, uuid, a);
            printf("%s.%s", a->name, outputName + 33);
        }else{
            printf("%s", outputName);
        }
        free(outputName);
        string = zmsg_popstr(msg); //output type as string
        type = atoi(string);
        free(string);
        printf(" %s", outputTypes[type-1]);
        switch (type) {
            case IGS_INTEGER_T:
                frame = zmsg_pop(msg);
                data = zframe_data(frame);
                size = zframe_size(frame);
                printf(" %d\n", *((int *)data));
                break;
            case IGS_DOUBLE_T:
                frame = zmsg_pop(msg);
                data = zframe_data(frame);
                size = zframe_size(frame);
                printf(" %f\n", *((double *)data));
                break;
            case IGS_BOOL_T:
                frame = zmsg_pop(msg);
                data = zframe_data(frame);
                size = zframe_size(frame);
                printf(" %d\n", *((bool *)data));
                break;
            case IGS_STRING_T:{
                char *str = zmsg_popstr(msg);
                printf(" %s\n", str);
                free(str);
                break;
            }
            case IGS_IMPULSION_T:
                frame = zmsg_pop(msg);
                data = zframe_data(frame);
                size = zframe_size(frame);
                printf("\n");
                break;
            case IGS_DATA_T:
                frame = zmsg_pop(msg);
                data = zframe_data(frame);
                size = zframe_size(frame);
                string = zframe_strhex(frame);
                printf(" (%lu bytes) %.64s\n", size, string);
                free(string);
                break;
                
            default:
                break;
        }
        if (frame != NULL)
            zframe_destroy(&frame);
    }
    zmsg_destroy(&msg);
    return 0;
}

//manage incoming messages from one of the logger agent we subscribed to
int manageLog (zloop_t *loop, zsock_t *socket, void *arg){
    UNUSED(loop);
    peer_t *a = (peer_t *)arg;
    zmsg_t *msg = zmsg_recv(a->logger);
    size_t s = zmsg_size(msg);
    printf("%s logged : ", a->name);
    for (unsigned long i = 0; i < s; i++){
        char *part = zmsg_popstr(msg);
        if (part == NULL){
            part = strdup("NULL");
        }
        printf(" %s", part);
        free(part);
    }
    zmsg_destroy(&msg);
    return 0;
}

// cross-platform sleep method
void crossSleep(unsigned int duration)
{
#ifdef _WIN32
    Sleep(duration*1000);
#else
    sleep(duration);
#endif
}

//manage commands entered on the command line from the parent
int manageParent (zloop_t *loop, zmq_pollitem_t *item, void *args){
    context_t *context = (context_t *)args;
    zyre_t *node = context->node;
    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            printf("Error while reading message from main thread");
            exit(EXIT_FAILURE); //Interrupted
        }
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM")){
            return -1;
        }
        else
        {
            if (streq (command, "SHOUT")) {
                char *channel = zmsg_popstr (msg);
                char *string = zmsg_popstr (msg);
                zyre_shouts (node, channel, "%s", string);
                free(channel);
                free(string);
            }
            else if (streq (command, "WHISPER")) {
                char *peer = zmsg_popstr (msg);
                char *string = zmsg_popstr (msg);
                peer_t *p, *tmp;
                HASH_ITER(hh, context->peers, p, tmp){
                    if (strcmp(p->name, peer) == 0 || strcmp(p->uuid, peer) == 0){
                        //NB: no break here beacause we allow whispering several peers
                        //having the same name
                        zyre_whispers (node, p->uuid, "%s", string);
                    }
                }
                free(peer);
                free(string);
            }
            else if (streq (command, "WHISPERALL")) {
                char *string = zmsg_popstr (msg);
                peer_t *p, *tmp;
                HASH_ITER(hh, context->peers, p, tmp){
                    zyre_whispers (node, p->uuid, "%s", string);
                }
                free(string);
            }
            else if(streq (command, "LEAVE")){
                char *channel = zmsg_popstr (msg);
                zyre_leave (node, channel);
                free(channel);
            }
            else if(streq (command, "LEAVEALL")){
                char *p;
                zlist_t *all_channels = zyre_peer_groups(node);
                while ((p = (char *)zlist_pop(all_channels))) {
                    zyre_leave (node, p);
                    free(p);
                }
                zlist_destroy(&all_channels);
            }
            else if(streq (command, "JOIN")){
                char *channel = zmsg_popstr (msg);
                zyre_join (node, channel);
                free(channel);
            }
            else if(streq (command, "JOINALL")){
                char *p;
                zlist_t *all_channels = zyre_peer_groups(node);
                while ((p = (char *)zlist_pop(all_channels))) {
                    zyre_join (node, p);
                    free(p);
                }
                zlist_destroy(&all_channels);
            }
            else if(streq (command, "CALL")){
                char *callTarget = zmsg_popstr (msg);
                char *callArgs = zmsg_popstr (msg);
                peer_t *p = NULL;
                agent_t *a = NULL, *atmp;
                //try to find agent with this name or uuid
                HASH_ITER(hh, context->agents, a, atmp){
                    if (streq(a->name, callTarget) || streq(a->uuid, callTarget)){
                        p = a->peer;
                        break; //sets variable a as well
                    }
                }
                if (a != NULL && p != NULL){
                    //Extract call name
                    size_t offset = 0;
                    while (callArgs[offset] != ' ' && callArgs[offset] != '\0') {
                        offset++;
                    }
                    char callName[4096] = "";
                    strncpy(callName, callArgs, offset);
                    call_t *call = NULL;
                    HASH_FIND_STR(a->calls, callName, call);
                    if (call != NULL){
                        zmsg_t *callMsg = zmsg_new();
                        zmsg_addstr(callMsg, "CALL");
                        if (streq(p->protocol, "v2")){
                            //v2 protocol : add agent uuid
                            zmsg_addstr(callMsg, a->uuid);
                        }else{
                            //v1 protocol
                            //nothing to do
                        }
                        if (addArgumentsToCallMessage(callMsg, call, callArgs)){
                            zyre_whisper(node, p->uuid, &callMsg);
                        }
                    }
                    free(callArgs);
                    free(callTarget);
                }else{
                    printf("unknown agent %s\n", callTarget);
                }
            }
            else if(streq (command, "PEERS")){
                zlist_t *peers = zyre_peers(node);
                char *p;
                printf("@peers:\n");
                while ((p = (char *)zlist_pop(peers))) {
                    peer_t *a = NULL;
                    HASH_FIND_STR(context->peers, p, a);
                    if (a != NULL){
                        printf("\t%s -> %s\n", a->name, p);
                    }else{
                        printf("\t? -> %s\n", p);
                    }
                    free(p);
                }
                zlist_destroy(&peers);
            }
            else if(streq (command, "CHANNELS")){
                zlist_t *my_channels = zyre_own_groups(node);
                zlist_t *all_channels = zyre_peer_groups(node);
                char *p;
                printf("@my channels:\n");
                while ((p = (char *)zlist_pop(my_channels))) {
                    printf("\t%s\n", p);
                    free(p);
                }
                printf("@all channels:\n");
                while ((p = (char *)zlist_pop(all_channels))) {
                    printf("\t%s\n", p);
                    free(p);
                }
                zlist_destroy(&my_channels);
                zlist_destroy(&all_channels);
            }
            else if(streq (command, "VERBOSE")){
                zyre_set_verbose(node);
            }
            else if (streq (command, "SUBSCRIBE")) {
                char *agentId = zmsg_popstr (msg);
                agent_t *agent, *tmp;
                size_t nbAgents = HASH_COUNT(context->agents);
                if (nbAgents == 0){
                    printf("no agent : join INGESCAPE_PRIVATE to receive agents definitions\n");
                }
                HASH_ITER(hh, context->agents, agent, tmp){
                    //NB: no break here because we allow subscribing to several peers
                    //having the same name
                    if (strcmp(agent->name, agentId) == 0 || strcmp(agent->uuid, agentId) == 0){
                        if (agent->peer->publisherPort == NULL){
                            printf("Found agent %s but its peer publisher port is NULL : command has been ignored\n", agentId);
                            continue;
                        }
                        if (agent->peer->subscriber != NULL){
                            //subscriber to this agent is already active:
                            //we just have to add internal pub/sub command
                            zsock_set_subscribe(agent->peer->subscriber, "");
                            continue;
                        }
                        char endpointAddress[128];
                        strncpy(endpointAddress, agent->peer->endpoint, 128);
                        
                        // IP adress extraction
                        char *insert = endpointAddress + strlen(endpointAddress);
                        bool extractOK = true;
                        while (*insert != ':'){
                            insert--;
                            if (insert == endpointAddress){
                                printf("Error: could not extract port from address %s", agent->peer->endpoint);
                                extractOK = false;
                                break;
                            }
                        }
                        if (extractOK){
                            *(insert + 1) = '\0';
                            strcat(endpointAddress, agent->peer->publisherPort);
                            agent->peer->subscriber = zsock_new_sub(endpointAddress, NULL);
                            zsock_set_subscribe(agent->peer->subscriber, "");
                            zloop_reader(loop, agent->peer->subscriber, manageSubscription, agent);
                            printf("Subscriber created for %s\n", agent->name);
                        }
                    }
                }
                free(agentId);
            }
            else if (streq (command, "SUBSCRIBE2")) {
                char *agentId = zmsg_popstr (msg);
                char *output = zmsg_popstr (msg);
                size_t nbAgents = HASH_COUNT(context->agents);
                if (nbAgents == 0){
                    printf("no agent : join INGESCAPE_PRIVATE to receive agents definitions\n");
                }
                agent_t *agent, *tmp;
                HASH_ITER(hh, context->agents, agent, tmp){
                    //NB: no break here because we allow subscribing to several peers
                    //having the same name
                    if (strcmp(agent->name, agentId) == 0 || strcmp(agent->uuid, agentId) == 0){
                        if (agent->peer->publisherPort == NULL){
                            printf("Found agent %s but its peer publisher port is NULL : command has been ignored\n", agentId);
                            continue;
                        }
                        if (agent->peer->subscriber != NULL){
                            //subscriber to this agent is already active:
                            //we just have to add internal pub/sub command
                            zsock_set_subscribe(agent->peer->subscriber, output);
                            continue;
                        }
                        
                        char endpointAddress[128];
                        strncpy(endpointAddress, agent->peer->endpoint, 128);
                        
                        // IP adress extraction
                        char *insert = endpointAddress + strlen(endpointAddress);
                        bool extractOK = true;
                        while (*insert != ':'){
                            insert--;
                            if (insert == endpointAddress){
                                printf("Error: could not extract port from address %s", agent->peer->endpoint);
                                extractOK = false;
                                break;
                            }
                        }
                        if (extractOK){
                            *(insert + 1) = '\0';
                            strcat(endpointAddress, agent->peer->publisherPort);
                            agent->peer->subscriber = zsock_new_sub(endpointAddress, NULL);
                            if (streq(agent->peer->protocol, "v1")){
                                zsock_set_subscribe(agent->peer->subscriber, output);
                            }else if (streq(agent->peer->protocol, "v2")){
                                char filterValue[4096 + 33] = ""; //33 is UUID length + separator
                                snprintf(filterValue, 4096 + 33, "%s-%s", agent->uuid, output);
                                zsock_set_subscribe(agent->peer->subscriber, filterValue);
                            }
                            
                            zloop_reader(loop, agent->peer->subscriber, manageSubscription, agent);
                            printf("Subscriber created for %s on output %s\n", agent->name, output);
                        }
                    }
                }
                free(agentId);
                free(output);
            }
            else if (streq (command, "LOG")) {
                char *peerId = zmsg_popstr (msg);
                peer_t *p, *tmp;
                HASH_ITER(hh, context->peers, p, tmp){
                    if (strcmp(p->name, peerId) == 0 || strcmp(p->uuid, peerId) == 0){
                        if (p->logPort == NULL){
                            printf("Found peer %s but its log port is NULL : command has been ignored\n", peerId);
                            continue;
                        }
                        //NB: no break here beacause we allow subscribing to several peers
                        //having the same name
                        char endpointAddress[128];
                        strncpy(endpointAddress, p->endpoint, 128);
                        
                        // IP adress extraction
                        char *insert = endpointAddress + strlen(endpointAddress);
                        bool extractOK = true;
                        while (*insert != ':'){
                            insert--;
                            if (insert == endpointAddress){
                                printf("Error: could not extract port from address %s", p->endpoint);
                                extractOK = false;
                                break;
                            }
                        }
                        if (extractOK){
                            *(insert + 1) = '\0';
                            strcat(endpointAddress, p->logPort);
                            p->logger = zsock_new_sub(endpointAddress, NULL);
                            zsock_set_subscribe(p->logger, "");
                            zloop_reader(loop, p->logger, manageLog, p);
                            printf("Log watcher created for %s\n", p->name);
                        }
                    }
                }
                free(peerId);
            }
            else if (streq (command, "UNSUBSCRIBE")) {
                char *agentId = zmsg_popstr (msg);
                agent_t *agent, *tmp;
                HASH_ITER(hh, context->agents, agent, tmp){
                    //NB: no break here beacause we allow unsubscribing to several peers
                    //having the same name
                    if (strcmp(agent->name, agentId) == 0 || strcmp(agent->uuid, agentId) == 0){
                        if (agent->peer->publisherPort == NULL){
                            printf("Found peer %s but its publisher port is NULL : command has been ignored\n", agentId);
                            continue;
                        }
                        if (agent->peer->subscriber == NULL){
                            printf("Found peer %s but its subscriber socket is NULL : command has been ignored\n", agentId);
                            continue;
                        }else{
                            zloop_reader_end(loop, agent->peer->subscriber);
                            zsock_destroy(&(agent->peer->subscriber));
                        }
                    }
                }
                free(agentId);
            }
            else if (streq (command, "UNLOG")) {
                char *peerId = zmsg_popstr (msg);
                peer_t *p, *tmp;
                HASH_ITER(hh, context->peers, p, tmp){
                    //NB: no break here beacause we allow subscribing to several peers
                    //having the same name
                    if (strcmp(p->name, peerId) == 0 || strcmp(p->uuid, peerId) == 0){
                        if (p->logPort == NULL){
                            printf("Found peer %s but its logger port is NULL : command has been ignored\n", peerId);
                            continue;
                        }
                        if (p->logger == NULL){
                            printf("Found peer %s but its logger socket is NULL : command has been ignored\n", peerId);
                            continue;
                        }else{
                            zloop_reader_end(loop, p->logger);
                            zsock_destroy(&(p->logger));
                        }
                    }
                }
                free(peerId);
            }
            else {
                printf("Error: invalid command to actor\n%s\n", command);
                assert (false);
            }
        }
        
        free (command);
        zmsg_destroy (&msg);
    }
    return 0;
}

//manage messages received on the bus
int manageIncoming (zloop_t *loop, zmq_pollitem_t *item, void *args){
    context_t *context = (context_t *)args;
    zyre_t *node = context->node;
    if (item->revents & ZMQ_POLLIN){
        zyre_event_t *zyre_event = zyre_event_new (node);
        const char *event = zyre_event_type(zyre_event);
        const char *peerId = zyre_event_peer_uuid(zyre_event);
        const char *name = zyre_event_peer_name (zyre_event);
        const char *address = zyre_event_peer_addr (zyre_event);
        const char *channel = zyre_event_group (zyre_event);
        zmsg_t *msg = zyre_event_msg (zyre_event);
        //size_t msg_size = zmsg_content_size(msg);

        peer_t *peer = NULL;
        HASH_FIND_STR(context->peers, peerId, peer);
        
        if (streq (event, "ENTER")){
            if (!context->useGossip){
                printf ("B->%s has entered the network with peer id %s and endpoint %s\n", name, peerId, address);
            }else{
                printf ("G->%s has entered the network with peer id %s and endpoint %s\n", name, peerId, address);
            }
            //printf ("->%s has entered the network with peer id %s and endpoint %s\n", name, peer, address);
            if (peer == NULL){
                peer = calloc(1, sizeof(peer_t));
                peer->reconnected = 0;
                peer->uuid = strndup(peerId, NAME_BUFFER_SIZE);
                peer->name = strndup(name, NAME_BUFFER_SIZE);
                peer->endpoint = strndup(address, NAME_BUFFER_SIZE);
                peer->context = context;
                HASH_ADD_STR(context->peers, uuid, peer);
            }else{
                //Agent already exists, we set its reconnected flag
                //(this is used below to avoid agent destruction on EXIT received after timeout)
                peer->reconnected++;
            }
            
            
            zhash_t *headers = zyre_event_headers (zyre_event);
            assert(headers);
            char *k;
            const char *v;
            zlist_t *keys = zhash_keys(headers);
            size_t s = zlist_size(keys);
            if (s > 0){
                printf("@%s's headers are:\n", name);
            }
            while ((k = (char *)zlist_pop(keys))) {
                v = zyre_event_header (zyre_event,k);
                if(strncmp(k,"publisher", strlen("publisher")) == 0){
                    //this is a ingescape agent, we store its publishing port
                    if (peer->publisherPort != NULL) {
                        free(peer->publisherPort);
                    }
                    peer->publisherPort = strndup(v,6); //port is 5 digits max
                }else if(strncmp(k,"logger", strlen("logger")) == 0){
                    //this is a ingescape agent, we store its publishing port
                    if (peer->logPort != NULL) {
                        free(peer->logPort);
                    }
                    peer->logPort = strndup(v,6); //port is 5 digits max
                }else if(strncmp(k,"protocol", strlen("protocol")) == 0){
                    //this is a ingescape agent, we store its publishing port
                    if (peer->protocol != NULL) {
                        free(peer->protocol);
                    }
                    peer->protocol = strndup(v, 6); //max protocol length
                }
                printf("\t%s -> %s\n", k, v);
                free(k);
            }
            zlist_destroy(&keys);
        } else if (streq (event, "JOIN")){
            printf ("+%s has joined %s\n", name, channel);
        } else if (streq (event, "LEAVE")){
            printf ("-%s has left %s\n", name, channel);
        } else if (streq (event, "SHOUT")){
            char *message = zmsg_popstr(msg);
            if (streq(message, "REMOTE_AGENT_EXIT")){
                char *uuid = zmsg_popstr(msg);
                agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent){
                    HASH_DEL(context->agents,agent);
                    printf("[IGS] %s (%s) exited\n", agent->name, agent->uuid);
                    free(agent->name);
                    free(agent->uuid);
                    if (agent->calls != NULL){
                        freeCalls(&(agent->calls));
                    }
                    free(agent);
                }
                zmsg_pushstr(msg, uuid); //put uuid back into message
                free(uuid);
            }
            printf("#%s:%s(%s) - %s |", channel, name, peerId, message);
            free(message);
            while ((message = zmsg_popstr(msg))){
                printf("%s |", message);
                free(message);
            }
            printf("\n");
            
        } else if (streq (event, "WHISPER")){
            zmsg_t *dup = zmsg_dup(msg);
            char *message = zmsg_popstr(dup);
            if (streq(peer->protocol, "v2") && streq(message, "EXTERNAL_DEFINITION#")){
                char* strDefinition = zmsg_popstr(dup);
                char *uuid = zmsg_popstr(dup);
                char *remoteAgentName = zmsg_popstr(dup);
                agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    agent = calloc(1, sizeof(agent_t));
                    peer_t *p = NULL;
                    HASH_FIND_STR(context->peers, peerId, p);
                    assert(p);
                    agent->peer = p;
                }
                if (agent->name != NULL)
                    free(agent->name);
                if (agent->uuid != NULL)
                    free(agent->uuid);
                agent->name = remoteAgentName;
                agent->uuid = uuid;
                HASH_ADD_STR(context->agents, uuid, agent);
                parseCallsFromDefinition(strDefinition, &(agent->calls));
                free(strDefinition);
            } else if (strncmp(message, "EXTERNAL_DEFINITION#", strlen("EXTERNAL_DEFINITION#")) == 0){
                //v1 or v0 protocol
                char *definition = message + strlen("EXTERNAL_DEFINITION#");
                agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, peerId, agent);
                if (agent == NULL){
                    agent = calloc(1, sizeof(agent_t));
                    peer_t *p = NULL;
                    HASH_FIND_STR(context->peers, peerId, p);
                    assert(p);
                    agent->peer = p;
                }
                if (agent->name != NULL)
                    free(agent->name);
                if (agent->uuid != NULL)
                    free(agent->uuid);
                agent->name = strdup(name);
                agent->uuid = strdup(peerId);
                HASH_ADD_STR(context->agents, uuid, agent);
                parseCallsFromDefinition(definition, &(agent->calls));
            }
            if (message)
                free(message);
            zmsg_destroy(&dup);
            
            //print message details
            while ((message = zmsg_popstr(msg))) {
                printf("#%s(%s) - %s |", name, peerId, message);
                free(message);
                while ((message = zmsg_popstr(msg))){
                    printf("%s |", message);
                    free(message);
                }
                printf("\n");
                free (message);
            }
            
        } else if (streq (event, "EXIT")){
            if (!context->useGossip){
                printf ("B<-%s exited\n", name);
            }else{
                printf ("G<-%s exited\n", name);
            }
            if (peer != NULL){
                if (peer->reconnected > 0){
                    //do not destroy: we are getting a timemout now whereas
                    //the agent is reconnected
                    peer->reconnected--;
                }else{
                    agent_t *a, *atmp;
                    HASH_ITER(hh, context->agents, a, atmp){
                        if (a->peer == peer){
                            //this agent lost its peer : remove
                            printf("cleaning agent %s(%s)\n", a->name, a->uuid);
                            HASH_DEL(context->agents,a);
                            free(a->name);
                            free(a->uuid);
                            if (a->calls != NULL){
                                freeCalls(&(a->calls));
                            }
                            free(a);
                        }
                    }
                    printf("cleaning peer %s(%s)\n", peer->name, peer->uuid);
                    HASH_DEL(context->peers, peer);
                    free(peer->name);
                    free(peer->uuid);
                    free(peer->endpoint);
                    if (peer->publisherPort != NULL){
                        free(peer->publisherPort);
                    }
                    if (peer->subscriber != NULL){
                        zloop_poller_end(loop, peer->subscriberPoller);
                        zsock_destroy(&(peer->subscriber));
                    }
                    if (peer->subscriberPoller != NULL){
                        free(peer->subscriberPoller);
                    }
                    if (peer->logPort != NULL){
                        free(peer->logPort);
                    }
                    if (peer->logger != NULL){
                        zloop_poller_end(loop, peer->loggerPoller);
                        zsock_destroy(&(peer->logger));
                    }
                    if (peer->loggerPoller != NULL){
                        free(peer->loggerPoller);
                    }
                    if (peer->protocol != NULL){
                        free(peer->protocol);
                    }
                    free(peer);
                }
            }
        } else if (streq (event, "SILENT")){
            printf ("%s (%s) is being silent\n", name, peerId);
        }
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

//Function used to stop main thread when ingeprobe is used just
//to send a message and stop
int stopMainThreadLoop(zloop_t *loop, int timer_id, void *args){
    UNUSED(loop);
    UNUSED(timer_id);
    UNUSED(args);
    return -1;
}

//manage message passed as run parameter
int triggerMessageSend(zloop_t *loop, int timer_id, void *args){
    UNUSED(loop);
    UNUSED(timer_id);
    context_t *context = (context_t *)args;
    zyre_t *node = context->node;
    char target[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    int matches = sscanf(paramMessage, "%s", target);
    if (matches == 1){
        strncpy(message, paramMessage + strlen(target) + 1, BUFFER_SIZE);
        if (strlen(target) > 0 && target[0] == '#'){
            zyre_shouts(node, target+1, "%s", message);
        }else if (strlen(target) > 0){
            zyre_whispers(node, target, "%s", message);
        }
    }
    //this return will stop the loop and terminate the app
    return -1;
}

static void
zyre_actor (zsock_t *pipe, void *args){
    context_t *context = (context_t *)args;
    zyre_t *node = zyre_new (context->name);
    //zyre_join(node, "INGESCAPE_PRIVATE");
    context->node = node;
    
    if (!context->useGossip){
        //beacon
        zyre_set_port(node, port);
        printf("using broadcast discovery with port %i", port);
        if (netdevice != NULL){
            zyre_set_interface(node, netdevice);
            printf(" on device %s", netdevice);
        }
        printf("\n");
    }else{
        //gossip
        if (endpoint != NULL){
            int res = zyre_set_endpoint(node, "%s", endpoint);
            if (res != 0){
                printf("impossible to create our endpoint %s ...exiting.", endpoint);
            }
            printf("using endpoint %s\n", endpoint);
            if (gossipconnect == NULL && gossipbind == NULL){
                printf("warning : endpoint specified but no attached gossip information, %s won't reach any other agent", name);
            }
        }
        if (gossipconnect != NULL){
            zyre_gossip_connect(node, "%s", gossipconnect);
            printf("connecting to P2P node at %s\n", gossipconnect);
        }
        if (gossipbind != NULL){
            zyre_gossip_bind(node, "%s", gossipbind);
            printf("creating P2P node %s\n", gossipbind);
        }
    }
    
    if (verbose){
        zyre_set_verbose(node);
    }
    if (!node)
        return;
    zyre_start (node);
    zsock_signal (pipe, 0); //notify main thread that we are ready
    zyre_print(node);
    
    //preparing and running zyre mainloop
    zmq_pollitem_t zpipePollItem;
    zmq_pollitem_t zyrePollItem;
    
    //main zmq socket (i.e. main thread)
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        printf("Error : could not get the pipe descriptor for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;
    
    //zyre socket
    void *zsock = zsock_resolve(zyre_socket (node));
    if (zsock == NULL){
        printf("Error : could not get the zyre socket for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zyrePollItem.socket = zsock;
    zyrePollItem.fd = 0;
    zyrePollItem.events = ZMQ_POLLIN;
    zyrePollItem.revents = 0;
    
    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);
    
    zloop_poller (loop, &zpipePollItem, manageParent, context);
    zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageIncoming, context);
    zloop_poller_set_tolerant(loop, &zyrePollItem);
    
    if (paramMessage != NULL && strlen(paramMessage) > 0){
        zloop_timer(loop, 10, 1, triggerMessageSend, context);
    }
    
    zloop_start (loop); //start returns when one of the pollers returns -1
    
    printf("shutting down...\n");
    //clean
    zloop_destroy (&loop);
    assert (loop == NULL);
    peer_t *current, *tmp;
    HASH_ITER(hh, context->peers, current, tmp) {
        HASH_DEL(context->peers,current);
        free(current->name);
        free(current->uuid);
        free(current->endpoint);
        if (current->publisherPort != NULL){
            free(current->publisherPort);
        }
        if (current->subscriber != NULL){
            zsock_destroy(&(current->subscriber));
        }
        if (current->subscriberPoller != NULL){
            free(current->subscriberPoller);
        }
        if (current->logPort != NULL){
            free(current->logPort);
        }
        if (current->logger != NULL){
            zsock_destroy(&(current->logger));
        }
        if (current->loggerPoller != NULL){
            free(current->loggerPoller);
        }
        if (current->protocol != NULL){
            free(current->protocol);
        }
        free(current);
    }
    agent_t *agent, *atmp;
    HASH_ITER(hh, context->agents, agent, atmp) {
        HASH_DEL(context->agents,agent);
        free(agent->name);
        free(agent->uuid);
        if (agent->calls != NULL){
            freeCalls(&(agent->calls));
        }
        free(agent);
    }
    zyre_stop (node);
    zclock_sleep (100);
    zyre_destroy (&node);
    keepRunning = false;
    free(context);
}

///////////////////////////////////////////////////////////////////////////////
// COMMAND LINE AND INTERPRETER OPTIONS
//
void print_usage(){
    printf("Usage example: ingeprobe --verbose --port 5670 --name ingeprobe\n");
    printf("(all parameters are optional)\n");
    printf("--verbose : enable verbose mode in the application\n");
    printf("--name peer_name : published name of this peer (default : ingeprobe)\n");
    printf("--noninteractiveloop : non-interactive loop for use as a background application\n");
    printf("--message \"[#channel|peer] message\" : message to send to a channel (indicated with #) or a peer (peer id) at startup and then stop\n");
    //printf("\n--proxy : run both beacon and gossip instances (default : false)\n");
    
    printf("\nUDP discovery configuration :\n");
    printf("--device : name of the network device to be used (shall be set if several devices available)\n");
    printf("--port port_number : port used for autodiscovery between peers (default : 5670)\n");
    
    printf("\nOR");
    printf("\nP2P discovery and fixed endpoint configuration :\n");
    printf("an endpoint looks like : tcp://10.0.0.7:49155 or ipc:///tmp/feeds/0 or inproc://my-endpoint\n");
    printf("(ipc is for UNIX systems only)\n");
    printf("(inproc is for inter-thread communication only)\n");
    printf("--bind endpoint : our address as a P2P known node\n");
    printf("--connect endpoint : address of a P2P node to use\n");
    printf("\tNB: if P2P endpoint restarts, others depending on it may not see it coming back\n");
    printf("\tNB: in P2P mode, leaving peers are not detected by others\n");
    printf("--endpoint endpoint : optional custom zyre endpoint address (overrides --netdevice and --port)\n");
    printf("\tNB: forcing the endpoint disables UDP discovery and P2P must be used\n");
    printf("\tNB: zyre endpoint can be tcp, ipc or inproc\n");
}

void print_commands(){
    printf("---------------------------------\n");
    printf("Supported commands:\n");
    printf("/quit : cleanly stop the application\n");
    printf("/verbose : triggers verbose mode for detailed activity information\n");
    printf("/peers : list all connected peers at the time (name -> uuid)\n");
    printf("/channels : list all existing channels at the time\n");
    printf("/join channel_name : joins a specific channel\n");
    printf("/joinall : join all existing channels at the time\n");
    printf("/leave channel_name : leave a specific channel\n");
    printf("/leaveall : leave all existing channels at the time\n");
    printf("/whisper peer message : sends a message to a specific peer\n\t(peer can be name or uuid)\n");
    printf("/shout channel_name message : sends a message to a specific channel\n");
    printf("/whisperall message : sends a message to all peers individually\n");
    printf("/subscribe agent : subscribes to all ingescape outputs for this agent\n\t(agent can be  name or uuid)\n");
    printf("/subscribe agent output : subscribes to ingescape agent specific output\n\t(agent can be  name or uuid)\n");
    printf("/unsubscribe peer : cancel all subscriptions to ingescape peer outputs\n\t(peer can be  name or uuid)\n");
    printf("/log peer : subscribes to ingescape agent log stream\n\t(peer can be name or uuid)\n");
    printf("/unlog peer : cancel subscription to ingescape agent log stream\n\t(peer can be name or uuid)\n");
    printf("/call agent call_name ... : sends a call to agent for specified call_name wth parameters\n");
    printf("\n");
}

///////////////////////////////////////////////////////////////////////////////
// MAIN & OPTIONS & COMMAND INTERPRETER
//
//
int main (int argc, char *argv [])
{
    //manage options
    int opt= 0;
    bool noninteractiveloop = false;
    
    //Specifying the expected options
    //The two options l and b expect numbers as argument
    static struct option long_options[] = {
        {"verbose",   no_argument, 0,  'v' },
        {"proxy",   no_argument, 0,  'r' },
        {"device",      required_argument, 0,  'd' },
        {"port",      required_argument, 0,  'p' },
        {"name",      required_argument, 0,  'n' },
        {"message",      required_argument, 0,  'm' },
        {"noninteractiveloop",      no_argument, 0,  'i' },
        {"help",      no_argument, 0,  'h' },
        {"bind",      required_argument, 0,  's' },
        {"connect",      required_argument, 0,  'g' },
        {"endpoint",      required_argument, 0,  'e' },
    };
    
    int long_index =0;
    while ((opt = getopt_long(argc, argv,"p",long_options, &long_index )) != -1) {
        switch (opt) {
            case 'p' :
                port = atoi(optarg);
                //printf("port: %i\n", port);
                break;
            case 'd' :
                netdevice = optarg;
                //printf("device: %s\n", netdevice);
                break;
            case 'n' :
                name = optarg;
                //printf("name: %s\n", name);
                break;
            case 's' :
                gossipbind = optarg;
                //printf("name: %s\n", name);
                break;
            case 'g' :
                gossipconnect = optarg;
                //printf("name: %s\n", name);
                break;
            case 'e' :
                endpoint = optarg;
                //printf("name: %s\n", name);
                break;
            case 'v' :
                verbose = true;
                //printf("verbose\n");
                break;
            case 'm' :
                paramMessage = strdup(optarg);
                break;
            case 'i' :
                noninteractiveloop = true;
                break;
            case 'r' :
                proxy = true;
                break;
            case 'h' :
                print_usage();
                exit (0);
                break;
            default:
                print_usage();
                exit(EXIT_FAILURE);
        }
    }
    
    if (paramMessage != NULL){
        noninteractiveloop = true;
    }
    
    //init zyre
    zactor_t *beaconActor = NULL;
    zactor_t *gossipActor = NULL;
    context_t *context = calloc(1, sizeof(context_t));
    if ((gossipconnect == NULL && gossipbind == NULL && endpoint == NULL)){
        assert(context);
        context->name = strdup(name);
        context->useGossip = false;
        context->peers = NULL;
        beaconActor = zactor_new (zyre_actor, context);
        assert (beaconActor);
    }else{
        if (endpoint != NULL && gossipconnect == NULL && gossipbind == NULL){
            printf("warning : endpoint specified but no attached P2P parameters, %s won't reach any other agent", name);
        }else{
            assert(context);
            context->name = strdup(name);
            context->useGossip = true;
            context->peers = NULL;
            gossipActor = zactor_new (zyre_actor, context);
            assert (gossipActor);
        }
    }
    
    if (noninteractiveloop){
        zloop_t *loop = zloop_new();
        if (paramMessage != NULL){
            zloop_timer(loop, 10, 1, stopMainThreadLoop, NULL);
        }
        zloop_start(loop);
        zloop_destroy(&loop);
    }else{
        //mainloop
        printf("%s is running...\nType /help for available commands\n", name);
        while (!zsys_interrupted) {
            char message [BUFFER_SIZE];
            char command[BUFFER_SIZE];
            char param1[BUFFER_SIZE];
            char param2[BUFFER_SIZE];
            int usedChar = 0;
            if (!fgets (message, BUFFER_SIZE, stdin))
                break;
            if ((message[0] == '/')&&(strlen (message) > 1)) {
                int matches = sscanf(message + 1, "%s %s%n%s", command, param1, &usedChar, param2);
                if (matches > 2) {
                    // copy the remaining of the message in param 2
                    strncpy(param2, message + usedChar + 2, BUFFER_SIZE);
                    // remove '\n' at the end
                    param2[strnlen(param2, BUFFER_SIZE) - 1] = '\0';
                }
                // Process command
                if (matches == -1) {
                    //printf("Error: could not interpret message %s\n", message + 1);
                }else if (matches == 1) {
                    if (strcmp(command, "verbose") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "VERBOSE", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "VERBOSE", NULL);
                        }
                    }else if (strcmp(command, "peers") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "PEERS", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "PEERS", NULL);
                        }
                    }else if (strcmp(command, "channels") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "CHANNELS", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "CHANNELS", NULL);
                        }
                    }else if (strcmp(command, "joinall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "JOINALL", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "JOINALL", NULL);
                        }
                    }else if (strcmp(command, "leaveall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "LEAVEALL", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "LEAVEALL", NULL);
                        }
                    }else if (strcmp(command, "help") == 0){
                        print_commands();
                    }else if (strcmp(command, "quit") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "$TERM", NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "$TERM", NULL);
                        }
                        break;
                    }
                }else if (matches == 2) {
                    //printf("Received command: %s + %s\n", command, param1);
                    if (strcmp(command, "join") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "JOIN", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "JOIN", param1, NULL);
                        }
                    } else if (strcmp(command, "leave") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "LEAVE", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "LEAVE", param1, NULL);
                        }
                    }else if (strcmp(command, "whisperall") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "WHISPERALL", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "WHISPERALL", param1, NULL);
                        }
                    }else if (strcmp(command, "subscribe") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "SUBSCRIBE", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "SUBSCRIBE", param1, NULL);
                        }
                    }else if (strcmp(command, "log") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "LOG", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "LOG", param1, NULL);
                        }
                    }else if (strcmp(command, "unsubscribe") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "UNSUBSCRIBE", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "UNSUBSCRIBE", param1, NULL);
                        }
                    }else if (strcmp(command, "unlog") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "UNLOG", param1, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "UNLOG", param1, NULL);
                        }
                    }
                }else if (matches == 3) {
                    //printf("Received command: %s + %s + %s\n", command, param1, param2);
                    if (strcmp(command, "whisper") == 0){
                        //FIXME: check to which actor UUID belongs
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "WHISPER", param1, param2, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "WHISPER", param1, param2, NULL);
                        }
                    } else if (strcmp(command, "shout") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "SHOUT", param1, param2, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "SHOUT", param1, param2, NULL);
                        }
                    } else if (strcmp(command, "subscribe") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "SUBSCRIBE2", param1, param2, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "SUBSCRIBE2", param1, param2, NULL);
                        }
                    } else if (strcmp(command, "call") == 0){
                        if (beaconActor != NULL){
                            zstr_sendx (beaconActor, "CALL", param1, param2, NULL);
                        }
                        if (gossipActor != NULL){
                            zstr_sendx (gossipActor, "CALL", param1, param2, NULL);
                        }
                    }
                }else{
                    printf("Error: message returned %d matches (%s)\n", matches, message);
                }
            }
        }
    }
    zactor_destroy (&beaconActor);
    zactor_destroy (&gossipActor);

    #ifdef _WIN32
    zsys_shutdown();
    #endif

    return 0;
}
