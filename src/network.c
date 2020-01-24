//
//  igs_network.c
//
//  Created by Stephane Vales on 10/06/2016.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

/**
  * \file ../../src/include/ingescape.h
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef __APPLE__ 
#include <TargetConditionals.h>
#if TARGET_OS_OSX
#include <libproc.h>
#endif
#endif

#include <zyre.h>
#include <czmq.h>
#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#include "ingescape_private.h"
#include "ingescape_advanced.h"

#ifdef _WIN32
    #define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
    #define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
    #define MAX_TRIES 3

    size_t  count;
    char *friendly_name = NULL;
    char buff[100];
    DWORD bufflen=100;

    /* Declare and initialize variables */
    DWORD dwRetVal = 0;

    unsigned int i = 0;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

    // default to unspecified address family (both)
    ULONG family = AF_UNSPEC;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;
    PIP_ADAPTER_UNICAST_ADDRESS pUnicast = NULL;
#endif

//global parameters
//prefixes for sending definitions and mappings through zyre
static const char *definitionPrefix = "EXTERNAL_DEFINITION#";
static const char *mappingPrefix = "EXTERNAL_MAPPING#";
static const char *loadMappingPrefix = "LOAD_THIS_MAPPING#";
static const char *loadDefinitionPrefix = "LOAD_THIS_DEFINITION#";

#define DEFAULT_IPC_PATH "/tmp/"

#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
pthread_mutex_t *network_Mutex = NULL;
#else
#define W_OK 02
pthread_mutex_t network_Mutex = NULL;
#endif

////////////////////////////////////////////////////////////////////////
// INTERNAL API
////////////////////////////////////////////////////////////////////////
/*
 Network mutex is used to avoid collisions between starting and stopping
 an agent, and between the manageBusIncoming, initLoop and start/stop functions.
 IT SHOULD NO BE USED FOR ANYTHING ELSE
 */
void network_Lock(void)   {
#if defined(__unix__) || defined(__linux__) || \
(defined(__APPLE__) && defined(__MACH__))
    if (network_Mutex == NULL){
        network_Mutex = calloc(1, sizeof(pthread_mutex_t));
        if (pthread_mutex_init(network_Mutex, NULL) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#elif (defined WIN32 || defined _WIN32)
    if (network_Mutex == NULL){
        if (pthread_mutex_init(&network_Mutex) != 0){
            igs_fatal("mutex init failed");
            assert(false);
            return;
        }
    }
#endif
    pthread_mutex_lock(network_Mutex);
}

void network_Unlock(void) {
    if (network_Mutex != NULL){
        pthread_mutex_unlock(network_Mutex);
    }else{
        igs_fatal("mutex was NULL\n");
        assert(false);
    }
}

int subscribeToPublisherOutput(igsAgent_t *agent, subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        bool filterAlreadyExists = false;
        mappingFilter_t *filter = NULL;
        DL_FOREACH(subscriber->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                filterAlreadyExists = true;
                break;
            }
        }
        if (!filterAlreadyExists){
            // Set subscriber to the output filter
            igsAgent_debug(agent, "Subscribe to agent %s output %s",subscriber->agentName,outputName);
            zsock_set_subscribe(subscriber->subscriber, outputName);
            mappingFilter_t *f = calloc(1, sizeof(mappingFilter_t));
            strncpy(f->filter, outputName, MAX_FILTER_SIZE-1);
            DL_APPEND(subscriber->mappingsFilters, f);
        }else{
            //printf("\n****************\nFILTER BIS %s - %s\n***************\n", subscriber->agent->agentName, outputName);
        }
        return 1;
    }
    return -1;
}

int unsubscribeToPublisherOutput(igsAgent_t *agent, subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        mappingFilter_t *filter = NULL;
        DL_FOREACH(subscriber->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                igsAgent_debug(agent, "Unsubscribe to agent %s output %s",agent->agentName,outputName);
                zsock_set_unsubscribe(subscriber->subscriber, outputName);
                DL_DELETE(subscriber->mappingsFilters, filter);
                break;
            }
        }
        return 1;
    }
    return -1;
}

//Timer callback to send REQUEST_OUPUTS notification for agents we subscribed to
int triggerMappingNotificationToNewcomer(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    
    subscriber_t *subscriber = (subscriber_t *)arg;
    igsAgent_t *agent = subscriber->agent;
    
    if (subscriber->mappedNotificationToSend){
        bus_zyreLock();
        zyre_whispers(agent->loopElements->node, subscriber->agentPeerId, "REQUEST_OUPUTS");
        bus_zyreUnlock();
        subscriber->mappedNotificationToSend = false;
    }
    return 0;
}

int network_manageSubscriberMapping(igsAgent_t *agent, subscriber_t *subscriber){
    //get mapping elements for this subscriber
    mapping_element_t *el, *tmp;
    if (agent->mapping != NULL){
        HASH_ITER(hh, agent->mapping->map_elements, el, tmp){
            if (strcmp(subscriber->agentName, el->agent_name)==0 || strcmp(el->agent_name, "*") == 0){
                //mapping element is compatible with subscriber name
                //check if we find a compatible output in subscriber definition
                agent_iop_t *foundOutput = NULL;
                if (subscriber->definition != NULL){
                    HASH_FIND_STR(subscriber->definition->outputs_table, el->output_name, foundOutput);
                }
                //check if we find a valid input in our own definition
                agent_iop_t *foundInput = NULL;
                if (agent->definition != NULL){
                    HASH_FIND_STR(agent->definition->inputs_table, el->input_name, foundInput);
                }
                //check type compatibility between input and output value types
                //including implicit conversions
                if (foundOutput != NULL && foundInput != NULL
                    && mapping_checkCompatibilityInputOutput(agent, foundInput, foundOutput)){
                    //we have validated input, agent and output names : we can map
                    //NOTE: the call below may happen several times if our agent uses
                    //the external agent ouput on several of its inputs. This should not have any consequence.
                    subscribeToPublisherOutput(agent, subscriber, el->output_name);
                    //mapping was successful : we set timer to notify remote agent if not already done
                    if (!subscriber->mappedNotificationToSend && agent->network_RequestOutputsFromMappedAgents){
                        subscriber->mappedNotificationToSend = true;
                        subscriber->agent = agent;
                        subscriber->timerId = zloop_timer(agent->loopElements->loop, 500, 1, triggerMappingNotificationToNewcomer, subscriber);
                    }
                }
                //NOTE: we do not clean subscriptions here because we cannot check if
                //an output is not used in another mapping element
            }
        }
    }
    return 0;
}

void sendDefinitionToAgent(igsAgent_t *agent, const char *peerId, const char *def)
{
    if(peerId != NULL &&  def != NULL)
    {
        if(agent->loopElements->node != NULL)
        {
            bus_zyreLock();
            zyre_whispers(agent->loopElements->node, peerId, "%s%s", definitionPrefix, def);
            bus_zyreUnlock();
        } else {
            igsAgent_warn(agent, "Could not send our definition to %s : our agent is not connected",peerId);
        }
    }
}

void sendMappingToAgent(igsAgent_t *agent, const char *peerId, const char *mapping)
{
    if(peerId != NULL &&  mapping != NULL){
        if(agent->loopElements->node != NULL){
            bus_zyreLock();
            zyre_whispers(agent->loopElements->node, peerId, "%s%s", mappingPrefix, mapping);
            bus_zyreUnlock();
        } else {
            igsAgent_warn(agent, "Could not send our mapping to %s : our agent is not connected",peerId);
        }
    }
}

void network_cleanAndFreeSubscriber(igsAgent_t *agent, subscriber_t *subscriber){
    igsAgent_debug(agent, "cleaning subscription to %s\n", subscriber->agentName);
    #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    agent->licenseEnforcement->currentAgentsNb--;
    #endif
    // clean the agent definition
    if(subscriber->definition != NULL){
        #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
        agent->licenseEnforcement->currentIOPNb -= (HASH_COUNT(subscriber->definition->inputs_table) +
                                         HASH_COUNT(subscriber->definition->outputs_table) +
                                         HASH_COUNT(subscriber->definition->params_table));
        //igs_license("license: %ld agents and %ld iops (%s)", agent->licenseEnforcement->currentAgentsNb, agent->licenseEnforcement->currentIOPNb, subscriber->agentName);
        #endif
        definition_freeDefinition(subscriber->definition);
    }
    //clean the agent mapping
    if(subscriber->mapping != NULL){
        mapping_freeMapping(subscriber->mapping);
    }
    //clean the subscriber itself
    mappingFilter_t *elt, *tmp;
    DL_FOREACH_SAFE(subscriber->mappingsFilters,elt,tmp) {
        zsock_set_unsubscribe(subscriber->subscriber, elt->filter);
        DL_DELETE(subscriber->mappingsFilters,elt);
        free(elt);
    }
    zloop_poller_end(agent->loopElements->loop , subscriber->pollItem);
    zsock_destroy(&subscriber->subscriber);
    free((char*)subscriber->agentName);
    free((char*)subscriber->agentPeerId);
    free(subscriber->pollItem);
    free(subscriber->subscriber);
    subscriber->subscriber = NULL;
    if (subscriber->timerId != -1){
        zloop_timer_end(agent->loopElements->loop, subscriber->timerId);
        subscriber->timerId = -2;
    }
    HASH_DEL(agent->subscribers, subscriber);
    free(subscriber);
    subscriber = NULL;
//    int n = HASH_COUNT(agent->subscribers);
//    igsAgent_debug(agent, "%d agent->subscribers in the list\n", n);
//    subscriber_t *s, *tmps;
//    HASH_ITER(hh, agent->subscribers, s, tmps){
//        igsAgent_debug(agent, "\tsubscriber : %s - %s\n", s->agentName, s->agentPeerId);
//    }
}

////////////////////////////////////////////////////////////////////////
// ZMQ callbacks
////////////////////////////////////////////////////////////////////////

//manage messages from the parent thread
int manageParent (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    IGS_UNUSED(loop)
    igsAgent_t *agent = (igsAgent_t *)arg;

    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            igsAgent_error(agent, "Could not read message from main thread : Agent will interrupt immediately.");
            return -1;
        }
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM")){
            free (command);
            zmsg_destroy (&msg);
            return -1;
        } else {
            //nothing to do so far
        }

        free (command);
        zmsg_destroy (&msg);
    }
    return 0;
}

//function actually handling messages from one of the publisher agents we subscribed to
int handleSubscriptionMessage(igsAgent_t *agent, zmsg_t *msg, subscriber_t *subscriber){
    
    if(subscriber == NULL){
        igsAgent_error(agent, "subscriber is NULL");
        return -1;
    }
    if(agent->isFrozen == true){
        igsAgent_debug(agent, "Message received from agent %s but all traffic in our agent has been frozen", subscriber->agentName);
        return 0;
    }
    
    size_t msgSize = zmsg_size(msg);
    char *output = NULL;
    char *vType = NULL;
    iopType_t valueType = 0;
    unsigned long i = 0;
    for (i = 0; i < msgSize; i += 3){
        // Each message part must contain 3 elements
        // 1 : output name
        // 2 : output ioptType
        // 3 : value of the output as a string or zframe
        output = zmsg_popstr(msg);
        vType = zmsg_popstr(msg);
        valueType = atoi(vType);
        free(vType);
        vType = NULL;
        
        zframe_t *frame = NULL;
        void *data = NULL;
        size_t size = 0;
        char * value = NULL;
        //get data before iterating to all the mapping elements using it
        if (valueType == IGS_STRING_T){
            value = zmsg_popstr(msg);
        }else{
            frame = zmsg_pop(msg);
            data = zframe_data(frame);
            size = zframe_size(frame);
        }
        //try to find mapping elements matching with this subscriber's output
        //and update mapped input(s) value accordingly
        //TODO : some day, optimize mapping storage to avoid iterating
        mapping_element_t *elmt, *tmp;
        HASH_ITER(hh, agent->mapping->map_elements, elmt, tmp) {
            if (strcmp(elmt->agent_name, subscriber->agentName) == 0
                && strcmp(elmt->output_name, output) == 0){
                //we have a match on emitting agent name and its ouput name :
                //still need to check the targeted input existence in our definition
                agent_iop_t *foundInput = NULL;
                if (agent->definition->inputs_table != NULL){
                    HASH_FIND_STR(agent->definition->inputs_table, elmt->input_name, foundInput);
                }
                if (foundInput == NULL){
                    igsAgent_error(agent, "Input %s is missing in our definition but expected in our mapping with %s.%s",
                               elmt->input_name,
                               elmt->agent_name,
                               elmt->output_name);
                }else{
                    //we have a fully matching mapping element : write from received output to our input
                    if (valueType == IGS_STRING_T){
                        model_writeIOP(agent, elmt->input_name, IGS_INPUT_T, valueType, value, strlen(value)+1);
                    }else{
                        model_writeIOP(agent, elmt->input_name, IGS_INPUT_T, valueType, data, size);
                    }
                }
            }
        }
        if (frame != NULL){
            zframe_destroy(&frame);
        }
        if (value != NULL){
            free(value);
        }
        free(output);
        output = NULL;
    }
    
    return 0;
}

//manage incoming messages from one of the publisher agents we subscribed to
int manageSubscription (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    IGS_UNUSED(loop)
    subscriber_t *subscriber = (subscriber_t *)arg;
    igsAgent_t *agent = subscriber->agent;
    
    if (item->revents & ZMQ_POLLIN && strlen(subscriber->agentPeerId) > 0){
        zmsg_t *msg = zmsg_recv(item->socket);
        handleSubscriptionMessage(agent, msg, subscriber);
        zmsg_destroy(&msg);
    }
    return 0;
}

//manage messages received on the bus
int manageBusIncoming (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    network_Lock();
    igsAgent_t *agent = (igsAgent_t *)arg;
    zyre_t *node = agent->loopElements->node;
    
    if (item->revents & ZMQ_POLLIN){
        zyre_event_t *zyre_event = zyre_event_new (node);
        const char *event = zyre_event_type (zyre_event);
        const char *peer = zyre_event_peer_uuid (zyre_event);
        const char *name = zyre_event_peer_name (zyre_event);
        const char *address = zyre_event_peer_addr (zyre_event);
        zhash_t *headers = zyre_event_headers (zyre_event);
        const char *group = zyre_event_group (zyre_event);
        zmsg_t *msg = zyre_event_msg (zyre_event);
        zmsg_t *msgDuplicate = zmsg_dup(msg);
        
        //parse event
        if (streq (event, "ENTER")){
            igsAgent_debug(agent, "->%s has entered the network with peer id %s and endpoint %s", name, peer, address);
            zyreAgent_t *zagent = NULL;
            HASH_FIND_STR(agent->zyreAgents, peer, zagent);
            if (zagent == NULL){
                zagent = calloc(1, sizeof(zyreAgent_t));
                zagent->reconnected = 0;
                zagent->subscriber = NULL;
                zagent->hasJoinedPrivateChannel = false;
                strncpy(zagent->peerId, peer, NAME_BUFFER_SIZE-1);
                HASH_ADD_STR(agent->zyreAgents, peerId, zagent);
                strncpy(zagent->name, name, NAME_BUFFER_SIZE-1);
                assert(headers);
                char *k;
                const char *v;
                zlist_t *keys = zhash_keys(headers);
                size_t s = zlist_size(keys);
                if (s > 0){
                    igsAgent_debug(agent, "Handling headers for agent %s", name);
                }
                while ((k = (char *)zlist_pop(keys))) {
                    v = zyre_event_header (zyre_event,k);
                    igsAgent_debug(agent, "\t%s -> %s", k, v);
                    
                    // we extract the publisher adress to subscribe to from the zyre message header
                    if(strncmp(k,"publisher", strlen("publisher")) == 0)
                    {
                        char endpointAddress[128];
                        strncpy(endpointAddress, address, 127);
                        
                        // IP adress extraction
                        char *insert = endpointAddress + strlen(endpointAddress);
                        bool extractOK = true;
                        while (*insert != ':'){
                            insert--;
                            if (insert == endpointAddress){
                                igsAgent_error(agent, "Could not extract port from address %s", address);
                                extractOK = false;
                                break;
                            }
                        }
                        
                        if (extractOK){
                            //we found a possible publisher to subscribe to
                            *(insert + 1) = '\0'; //close endpointAddress string after ':' location
                            
                            //check towards our own ip address (without port)
                            char *incomingIpAddress = endpointAddress + 6; //ignore tcp://
                            *insert = '\0';
                            bool useIPC = false;
                            bool useInproc = false;
                            const char *ipcAddress = NULL;
                            const char *inprocAddress = NULL;
                            if (strcmp(agent->loopElements->ipAddress, incomingIpAddress) == 0){
                                //same IP address : we can try to use ipc (or loopback on windows) instead of TCP
                                //or we can use inproc if both agents are in the same process
                                int pid = atoi(zyre_event_header(zyre_event, "pid"));
                                if (agent->loopElements->processId == pid){
                                    //same ip address and same process : we can use inproc
                                    inprocAddress = zyre_event_header(zyre_event, "inproc");
                                    if (inprocAddress != NULL){
                                        useInproc = true;
                                        igsAgent_debug(agent, "Use address %s to subscribe to %s", inprocAddress, name);
                                    }
                                }else{
                                    //try to recover agent ipc/loopback address
                                    #if defined __unix__ || defined __APPLE__ || defined __linux__
                                    ipcAddress = zyre_event_header(zyre_event, "ipc");
                                    #elif (defined WIN32 || defined _WIN32)
                                    ipcAddress = zyre_event_header(zyre_event, "loopback");
                                    #endif
                                    if (ipcAddress != NULL){
                                        useIPC = true;
                                        igsAgent_debug(agent, "Use address %s to subscribe to %s", ipcAddress, name);
                                    }
                                }
                            }
                            *insert = ':';
                            //add port to the endpoint to compose it fully
                            strcat(endpointAddress, v);
                            subscriber_t *subscriber;
                            HASH_FIND_STR(agent->subscribers, peer, subscriber);
                            if (subscriber != NULL){
                                //we have a reconnection with same peerId
                                igsAgent_debug(agent, "Peer id %s was connected before with agent name %s : reset and recreate subscription", peer, subscriber->agentName);
                                HASH_DEL(agent->subscribers, subscriber);
                                zloop_poller_end(agent->loopElements->loop , subscriber->pollItem);
                                zsock_destroy(&subscriber->subscriber);
                                free((char*)subscriber->agentName);
                                free((char*)subscriber->agentPeerId);
                                free(subscriber->pollItem);
                                free(subscriber->subscriber);
                                if (subscriber->definition != NULL){
                                    definition_freeDefinition(subscriber->definition);
                                    subscriber->definition = NULL;
                                }
                                subscriber->subscriber = NULL;
                                if (subscriber->timerId != -1){
                                    zloop_timer_end(agent->loopElements->loop, subscriber->timerId);
                                    subscriber->timerId = -1;
                                }
                                free(subscriber);
                                subscriber = NULL;
                            }
                            subscriber = calloc(1, sizeof(subscriber_t));
                            zagent->subscriber = subscriber;
                            subscriber->agent = agent;
                            subscriber->agentName = strdup(name);
                            subscriber->agentPeerId = strdup (peer);
                            if (agent->allowInproc && useInproc){
                                subscriber->subscriber = zsock_new_sub(inprocAddress, NULL);
                                zsock_set_rcvhwm(subscriber->subscriber, agent->network_hwmValue);
                                igsAgent_debug(agent, "Subscription created for %s at %s (inproc)",subscriber->agentName, inprocAddress);
                            }else if (agent->allowIpc && useIPC){
                                subscriber->subscriber = zsock_new_sub(ipcAddress, NULL);
                                zsock_set_rcvhwm(subscriber->subscriber, agent->network_hwmValue);
                                igsAgent_debug(agent, "Subscription created for %s at %s (ipc)",subscriber->agentName, ipcAddress);
                            }else{
                                subscriber->subscriber = zsock_new_sub(endpointAddress, NULL);
                                zsock_set_rcvhwm(subscriber->subscriber, agent->network_hwmValue);
                                igsAgent_debug(agent, "Subscription created for %s at %s (tcp)",subscriber->agentName, endpointAddress);
                            }
                            assert(subscriber->subscriber);
                            subscriber->definition = NULL;
                            subscriber->mappingsFilters = NULL;
                            subscriber->timerId = -1;
                            HASH_ADD_STR(agent->subscribers, agentPeerId, subscriber);
                            subscriber->pollItem = calloc(1, sizeof(zmq_pollitem_t));
                            subscriber->pollItem->socket = zsock_resolve(subscriber->subscriber);
                            subscriber->pollItem->fd = 0;
                            subscriber->pollItem->events = ZMQ_POLLIN;
                            subscriber->pollItem->revents = 0;
                            zloop_poller (agent->loopElements->loop, subscriber->pollItem, manageSubscription, (void*)subscriber);
                            zloop_poller_set_tolerant(loop, subscriber->pollItem);
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                            agent->licenseEnforcement->currentAgentsNb++;
                            //igs_license("%ld agents (adding %s)", agent->licenseEnforcement->currentAgentsNb, name);
                            if (agent->licenseEnforcement->currentAgentsNb > agent->license->platformNbAgents){
                                igs_license("Maximum number of allowed agents (%d) is exceeded : agent will stop", agent->license->platformNbAgents);
                                license_callback_t *el = NULL;
                                DL_FOREACH(agent->licenseCallbacks, el){
                                    el->callback_ptr(agent, IGS_LICENSE_TOO_MANY_AGENTS, el->data);
                                }
                                return -1;
                            }
#endif
                        }
                    }
                    free(k);
                }
                zlist_destroy(&keys);
            }else{
                //Agent already exists, we set its reconnected flag
                //(this is used below to avoid agent destruction on EXIT received after timeout)
                zagent->reconnected++;
            }
        } else if (streq (event, "JOIN")){
            igsAgent_debug(agent, "+%s has joined %s", name, group);
            if (streq(group, CHANNEL)){
                //definition is sent to every newcomer on the channel (whether it is a ingescape agent or not)
                char * definitionStr = NULL;
                definitionStr = parser_export_definition(agent->definition);
                if (definitionStr != NULL){
                    sendDefinitionToAgent(agent, peer, definitionStr);
                    free(definitionStr);
                }
                //and so is our mapping
                char *mappingStr = NULL;
                mappingStr = parser_export_mapping(agent->mapping);
                if (mappingStr != NULL){
                    sendMappingToAgent(agent, peer, mappingStr);
                    free(mappingStr);
                }else{
                    sendMappingToAgent(agent, peer, "");
                }
                //we also send our frozen and muted states, and other usefull information
                if (agent->isWholeAgentMuted){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "MUTED=1");
                    bus_zyreUnlock();
                }
                if (agent->isFrozen){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "FROZEN=1");
                    bus_zyreUnlock();
                }
                if (strlen(agent->agentState) > 0){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "STATE=%s", agent->agentState);
                    bus_zyreUnlock();
                }
                if (agent->definition != NULL){
                    agent_iop_t *current_iop, *tmp_iop;
                    HASH_ITER(hh, agent->definition->outputs_table, current_iop, tmp_iop) {
                        if (current_iop->is_muted && current_iop->name != NULL){
                            bus_zyreLock();
                            zyre_whispers(agent->loopElements->node, peer, "OUTPUT_MUTED %s", current_iop->name);
                            bus_zyreUnlock();
                        }
                    }
                }
                if (agent->logInStream){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "LOG_IN_STREAM=1");
                    bus_zyreUnlock();
                }
                if (agent->logInFile){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "LOG_IN_FILE=1");
                    zyre_whispers(agent->loopElements->node, peer, "LOG_FILE_PATH=%s", agent->logFilePath);
                    bus_zyreUnlock();
                }
                if (strlen(agent->definitionPath) > 0){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "DEFINITION_FILE_PATH=%s", agent->definitionPath);
                    bus_zyreUnlock();
                }
                if (strlen(agent->mappingPath) > 0){
                    bus_zyreLock();
                    zyre_whispers(agent->loopElements->node, peer, "MAPPING_FILE_PATH=%s", agent->mappingPath);
                    bus_zyreUnlock();
                }
                
                zyreAgent_t *zagent = NULL;
                HASH_FIND_STR(agent->zyreAgents, peer, zagent);
                if (zagent != NULL){
                    zagent->hasJoinedPrivateChannel = true;
                }
            }
        } else if (streq (event, "LEAVE")){
            igsAgent_debug(agent, "-%s has left %s", name, group);
        } else if (streq (event, "SHOUT")){
            if (strcmp(group, agent->replayChannel) == 0){
                //this is a replay message for one of our inputs
                char *input = zmsg_popstr (msgDuplicate);
                iopType_t inputType = igsAgent_getTypeForInput(agent, input);
                
                if (inputType > 0 && zmsg_size(msgDuplicate) > 0){
                    zframe_t *frame = NULL;
                    void *data = NULL;
                    size_t size = 0;
                    if (inputType == IGS_STRING_T){
                        char * value = NULL;
                        value = zmsg_popstr(msgDuplicate);
                        igsAgent_debug(agent, "replaying %s (%s)", input, value);
                        igsAgent_writeInputAsString(agent, input, value);
                        if (value != NULL){
                            free(value);
                        }
                    }else{
                        igsAgent_debug(agent, "replaying %s", input);
                        frame = zmsg_pop(msgDuplicate);
                        data = zframe_data(frame);
                        size = zframe_size(frame);
                        model_writeIOP(agent, input, IGS_INPUT_T, inputType, data, size);
                        if (frame != NULL){
                            zframe_destroy(&frame);
                        }
                    }
                }else{
                    igsAgent_warn(agent, "replay message for input %s is not correct and was ignored", input);
                }
                if (input != NULL){
                    free(input);
                }
            }
        } else if(streq (event, "WHISPER")){
            char *message = zmsg_popstr (msgDuplicate);
            
            //check if message is an EXTERNAL definition
            if(strlen(message) > strlen(definitionPrefix) && strncmp (message, definitionPrefix, strlen(definitionPrefix)) == 0)
            {
                // Extract definition from message
                char* strDefinition = calloc(strlen(message)- strlen(definitionPrefix)+1, sizeof(char));
                memcpy(strDefinition, &message[strlen(definitionPrefix)], strlen(message)- strlen(definitionPrefix));
                strDefinition[strlen(message)- strlen(definitionPrefix)] = '\0';
                
                // Load definition from string content
                igs_definition_t *newDefinition = parser_loadDefinition(strDefinition);
                subscriber_t *subscriber;
                HASH_FIND_STR(agent->subscribers, peer, subscriber);
                
                if (newDefinition != NULL && newDefinition->name != NULL && subscriber != NULL){
                    // Look if this agent already has a definition
                    if(subscriber->definition != NULL) {
                        igsAgent_debug(agent, "Definition already exists for agent %s : new definition will overwrite the previous one...", name);
                        #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                        //we remove IOP count from previous definition
                        agent->licenseEnforcement->currentIOPNb -= (HASH_COUNT(subscriber->definition->inputs_table) +
                                                         HASH_COUNT(subscriber->definition->outputs_table) +
                                                         HASH_COUNT(subscriber->definition->params_table));
                        //igs_license("%ld iops (cleaning %s)", agent->licenseEnforcement->currentIOPNb, name);
                        #endif
                        definition_freeDefinition(subscriber->definition);
                        subscriber->definition = NULL;
                    }
                    #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                    agent->licenseEnforcement->currentIOPNb += (HASH_COUNT(newDefinition->inputs_table) +
                                                     HASH_COUNT(newDefinition->outputs_table) +
                                                     HASH_COUNT(newDefinition->params_table));
                    //igs_license("%ld iops (adding %s)", agent->licenseEnforcement->currentIOPNb, name);
                    if (agent->licenseEnforcement->currentIOPNb > agent->license->platformNbIOPs){
                        igs_license("Maximum number of allowed IOPs (%d) is exceeded : agent will stop", agent->license->platformNbIOPs);
                        license_callback_t *el = NULL;
                        DL_FOREACH(agent->licenseCallbacks, el){
                            el->callback_ptr(agent, IGS_LICENSE_TOO_MANY_IOPS, el->data);
                        }
                        return -1;
                    }
                    #endif
                    igsAgent_debug(agent, "Store definition for agent %s", name);
                    subscriber->definition = newDefinition;
                    //Check the involvement of this new agent and its definition in our mapping and update subscriptions
                    //we check here because subscriber definition is required to handle received data
                    network_manageSubscriberMapping(agent, subscriber);
                }else{
                    igsAgent_error(agent, "Received definition from agent %s is NULL or has no name", name);
                    if(newDefinition != NULL) {
                        definition_freeDefinition(newDefinition);
                        newDefinition = NULL;
                    }
                }
                free(strDefinition);
            }
            //check if message is an EXTERNAL mapping
            else if(strlen(message) >= strlen(mappingPrefix) && strncmp (message, mappingPrefix, strlen(mappingPrefix)) == 0){
                subscriber_t *subscriber = NULL;
                HASH_FIND_STR(agent->subscribers, peer, subscriber);
                if (subscriber == NULL){
                    igsAgent_error(agent, "Could not find internal structure for agent %s", name);
                }
                
                char* strMapping = NULL;
                igs_mapping_t *newMapping = NULL;
                if (strlen(message) != strlen(mappingPrefix)){
                    //extract mapping from message
                    strMapping = calloc(strlen(message)- strlen(mappingPrefix)+1, sizeof(char));
                    memcpy(strMapping, &message[strlen(mappingPrefix)], strlen(message)- strlen(mappingPrefix));
                    strMapping[strlen(message)- strlen(mappingPrefix)] = '\0';
                    
                    //load mapping from string content
                    newMapping = parser_LoadMap(strMapping);
                    if (newMapping == NULL){
                        igsAgent_error(agent, "Received mapping for agent %s could not be parsed properly", name);
                    }
                }else{
                    igsAgent_debug(agent, "Received mapping from agent %s is empty", name);
                    if(subscriber != NULL && subscriber->mapping != NULL) {
                        mapping_freeMapping(subscriber->mapping);
                        subscriber->mapping = NULL;
                    }
                }
                
                if (newMapping != NULL && subscriber != NULL){
                    //look if this agent already has a mapping
                    if(subscriber->mapping != NULL){
                        igsAgent_debug(agent, "Mapping already exists for agent %s : new mapping will overwrite the previous one...", name);
                        mapping_freeMapping(subscriber->mapping);
                        subscriber->mapping = NULL;
                    }
                    
                    igsAgent_debug(agent, "Store mapping for agent %s", name);
                    subscriber->mapping = newMapping;
                }else{
                    if(newMapping != NULL) {
                        mapping_freeMapping(newMapping);
                        newMapping = NULL;
                    }
                }
                free(strMapping);
            }
            //check if message is DEFINITION TO BE LOADED
            else if (strlen(message) > strlen(loadDefinitionPrefix)
                     && strncmp (message, loadDefinitionPrefix, strlen(loadDefinitionPrefix)) == 0){
                // Extract definition from message
                char* strDefinition = calloc(strlen(message)- strlen(loadDefinitionPrefix)+1, sizeof(char));
                memcpy(strDefinition, &message[strlen(loadDefinitionPrefix)], strlen(message)- strlen(loadDefinitionPrefix));
                strDefinition[strlen(message)- strlen(loadDefinitionPrefix)] = '\0';
                
                //load definition
                igsAgent_loadDefinition(agent, strDefinition);
                //recheck mapping towards our new definition
                subscriber_t *s, *tmp;
                HASH_ITER(hh, agent->subscribers, s, tmp){
                    network_manageSubscriberMapping(agent, s);
                }
                free(strDefinition);
            }
            //check if message is MAPPING TO BE LOADED
            else if (strlen(message) > strlen(loadMappingPrefix)
                     && strncmp (message, loadMappingPrefix, strlen(loadMappingPrefix)) == 0){
                // Extract mapping from message
                char* strMapping = calloc(strlen(message)- strlen(loadMappingPrefix)+1, sizeof(char));
                memcpy(strMapping, &message[strlen(loadMappingPrefix)], strlen(message)- strlen(loadMappingPrefix));
                strMapping[strlen(message)- strlen(loadMappingPrefix)] = '\0';
                
                // Load mapping from string content
                igs_mapping_t *m = parser_LoadMap(strMapping);
                if (m != NULL){
                    if (agent->mapping != NULL){
                        mapping_freeMapping(agent->mapping);
                    }
                    agent->mapping = m;
                    //check and activate mapping
                    subscriber_t *s, *tmp;
                    HASH_ITER(hh, agent->subscribers, s, tmp){
                        network_manageSubscriberMapping(agent, s);
                    }
                    agent->network_needToUpdateMapping = true;
                }
                free(strMapping);
            }else{
                //other supported messages
                if (strlen("REQUEST_OUPUTS") == strlen(message) && strncmp (message, "REQUEST_OUPUTS", strlen("REQUEST_OUPUTS")) == 0){
                    igsAgent_debug(agent, "Responding to outputs request received from %s", name);
                    //send all outputs via whisper to agent that mapped us
                    long nbOutputs = 0;
                    char **outputsList = NULL;
                    outputsList = igsAgent_getOutputsList(agent, &nbOutputs);
                    int i = 0;
                    zmsg_t *omsg = zmsg_new();
                    zmsg_addstr(omsg, "OUTPUTS");
                    for (i = 0; i < nbOutputs; i++){
                        agent_iop_t * found_iop = model_findIopByName(agent, outputsList[i],IGS_OUTPUT_T);
                        if (found_iop != NULL){
                            switch (found_iop->value_type) {
                                case IGS_INTEGER_T:
                                    zmsg_addstr(omsg, found_iop->name);
                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
                                    zmsg_addmem(omsg, &(found_iop->value.i), sizeof(int));
                                    break;
                                case IGS_DOUBLE_T:
                                    zmsg_addstr(omsg, found_iop->name);
                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
                                    zmsg_addmem(omsg, &(found_iop->value.d), sizeof(double));
                                    break;
                                case IGS_STRING_T:
                                    zmsg_addstr(omsg, found_iop->name);
                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
                                    zmsg_addstr(omsg, found_iop->value.s);
                                    break;
                                case IGS_BOOL_T:
                                    zmsg_addstr(omsg, found_iop->name);
                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
                                    zmsg_addmem(omsg, &(found_iop->value.b), sizeof(bool));
                                    break;
                                case IGS_IMPULSION_T:
                                    //disabled
//                                    zmsg_addstr(omsg, found_iop->name);
//                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
//                                    zmsg_addmem(omsg, NULL, 0);
                                    break;
                                case IGS_DATA_T:
                                    //disabled
//                                    zmsg_addstr(omsg, found_iop->name);
//                                    zmsg_addstrf(omsg, "%d", found_iop->value_type);
//                                    zmsg_addmem(omsg, (found_iop->value.data), found_iop->valueSize);
                                    break;
                                    
                                default:
                                    break;
                            }
                            
                            
                            free(outputsList[i]);
                        }
                    }
                    bus_zyreLock();
                    zyre_whisper(node, peer, &omsg);
                    bus_zyreUnlock();
                    free(outputsList);
                }else if (strlen("OUTPUTS") == strlen(message) && strncmp (message, "OUTPUTS", strlen("OUTPUTS")) == 0){
                    subscriber_t *foundSubscriber = NULL;
                    HASH_FIND_STR(agent->subscribers, peer, foundSubscriber);
                    if (foundSubscriber){
                        handleSubscriptionMessage(agent, msgDuplicate, foundSubscriber);
                        igsAgent_debug(agent, "privately received output values from %s (%s)", name, peer);
                    }else{
                        igsAgent_error(agent, "could not find subscriber for peer %s (%s)", name, peer);
                    }
                    
                }else if (strlen("GET_CURRENT_INPUTS") == strlen(message) && strncmp (message, "GET_CURRENT_INPUTS", strlen("GET_CURRENT_INPUTS")) == 0){
                    zmsg_t *resp = zmsg_new();
                    agent_iop_t *inputs = agent->definition->inputs_table;
                    agent_iop_t *current = NULL;
                    zmsg_addstr(resp, "CURRENT_INPUTS");
                    model_readWriteLock();
                    for (current = inputs; current != NULL; current = current->hh.next){
                        zmsg_addstr(resp, current->name);
                        zmsg_addmem(resp, &current->type, sizeof(iop_t));
                        zmsg_addmem(resp, &current->value, current->valueSize);
                    }
                    model_readWriteUnlock();
                    bus_zyreLock();
                    zyre_whisper(node, peer, &resp);
                    bus_zyreUnlock();
                    igsAgent_debug(agent, "send input values to %s", peer);
                }else if (strlen("GET_CURRENT_PARAMETERS") == strlen(message) && strncmp (message, "GET_CURRENT_PARAMETERS", strlen("GET_CURRENT_PARAMETERS")) == 0){
                    zmsg_t *resp = zmsg_new();
                    agent_iop_t *inputs = agent->definition->params_table;
                    agent_iop_t *current = NULL;
                    zmsg_addstr(resp, "CURRENT_PARAMETERS");
                    model_readWriteLock();
                    for (current = inputs; current != NULL; current = current->hh.next){
                        zmsg_addstr(resp, current->name);
                        zmsg_addmem(resp, &current->type, sizeof(iop_t));
                        zmsg_addmem(resp, &current->value, current->valueSize);
                    }
                    model_readWriteUnlock();
                    bus_zyreLock();
                    zyre_whisper(node, peer, &resp);
                    bus_zyreUnlock();
                    igsAgent_debug(agent, "send parameters values to %s", peer);
                }else if (strlen("LICENSE_INFO") == strlen(message) && strncmp (message, "LICENSE_INFO", strlen("LICENSE_INFO")) == 0){
#if !TARGET_OS_IOS
                    zmsg_t *resp = zmsg_new();
                    if (agent->license == NULL){
                        zmsg_addstr(resp, "no license available");
                    }else{
                        char buf[128] = "";
                        struct tm ts;
                        zmsg_addstr(resp, "customer");
                        zmsg_addstr(resp, agent->license->customer);
                        zmsg_addstr(resp, "order");
                        zmsg_addstr(resp, agent->license->order);
                        zmsg_addstr(resp, "platformNbAgents");
                        zmsg_addstrf(resp, "%d", agent->license->platformNbAgents);
                        zmsg_addstr(resp, "platformNbIOPs");
                        zmsg_addstrf(resp, "%d", agent->license->platformNbIOPs);
                        zmsg_addstr(resp, "licenseExpirationDate");
                        ts = *localtime(&agent->license->licenseExpirationDate);
                        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
                        zmsg_addstr(resp, buf);
                        zmsg_addstr(resp, "editorOwner");
                        zmsg_addstr(resp, agent->license->editorOwner);
                        zmsg_addstr(resp, "editorExpirationDate");
                        ts = *localtime(&agent->license->editorExpirationDate);
                        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
                        zmsg_addstr(resp, buf);
                        char *feature = zhash_first(agent->license->features);
                        while (feature != NULL) {
                            zmsg_addstr(resp, "feature");
                            zmsg_addstr(resp, feature);
                            feature = zhash_next(agent->license->features);
                        }
                        licenseForAgent_t *licForAgent = zhash_first(agent->license->agents);
                        while (licForAgent != NULL) {
                            zmsg_addstr(resp, "agent");
                            zmsg_addstr(resp, licForAgent->agentId);
                            zmsg_addstr(resp, licForAgent->agentName);
                            licForAgent = zhash_next(agent->license->agents);
                        }
                    }
                    bus_zyreLock();
                    zyre_whisper(node, peer, &resp);
                    bus_zyreUnlock();
                    igsAgent_debug(agent, "send license information to %s", peer);
#endif
                }else if (strlen("STOP") == strlen(message) && strncmp (message, "STOP", strlen("STOP")) == 0){
                    free(message);
                    agent->forcedStop = true;
                    igsAgent_debug(agent, "received STOP command from %s (%s)", name, peer);
                    //stop our zyre loop by returning -1 : this will start the cleaning process
                    return -1;
                }else if (strlen("CLEAR_MAPPING") == strlen(message) && strncmp (message, "CLEAR_MAPPING", strlen("CLEAR_MAPPING")) == 0){
                    igsAgent_debug(agent, "received CLEAR_MAPPING command from %s (%s)", name, peer);
                    igsAgent_clearMapping(agent);
                }else if (strlen("FREEZE") == strlen(message) && strncmp (message, "FREEZE", strlen("FREEZE")) == 0){
                    igsAgent_debug(agent, "received FREEZE command from %s (%s)", name, peer);
                    igsAgent_freeze(agent);
                }else if (strlen("UNFREEZE") == strlen(message) && strncmp (message, "UNFREEZE", strlen("UNFREEZE")) == 0){
                    igsAgent_debug(agent, "received UNFREEZE command from %s (%s)", name, peer);
                    igsAgent_unfreeze(agent);
                }else if (strlen("MUTE_ALL") == strlen(message) && strncmp (message, "MUTE_ALL", strlen("MUTE_ALL")) == 0){
                    igsAgent_debug(agent, "received MUTE_ALL command from %s (%s)", name, peer);
                    igsAgent_mute(agent);
                }else if (strlen("UNMUTE_ALL") == strlen(message) && strncmp (message, "UNMUTE_ALL", strlen("UNMUTE_ALL")) == 0){
                    igsAgent_debug(agent, "received UNMUTE_ALL command from %s (%s)", name, peer);
                    igsAgent_unmute(agent);
                }else if ((strncmp (message, "MUTE ", strlen("MUTE ")) == 0) && (strlen(message) > strlen("MUTE ")+1)){
                    igsAgent_debug(agent, "received MUTE command from %s (%s)", name, peer);
                    char *subStr = message + strlen("MUTE") + 1;
                    igsAgent_muteOutput(agent, subStr);
                }else if ((strncmp (message, "UNMUTE ", strlen("UNMUTE ")) == 0) && (strlen(message) > strlen("UNMUTE ")+1)){
                    igsAgent_debug(agent, "received UNMUTE command from %s (%s)", name, peer);
                    char *subStr = message + strlen("UNMUTE") + 1;
                    igsAgent_unmuteOutput(agent,subStr);
                }else if ((strncmp (message, "SET_INPUT ", strlen("SET_INPUT ")) == 0) && (strlen(message) > strlen("SET_INPUT ")+1)){
                    igsAgent_debug(agent, "received SET_INPUT command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_INPUT") + 1;
                    char *_name, *value;
                    _name = strtok (subStr," ");
                    value = strtok (NULL,"\0");
                    if (_name != NULL && value != NULL){
                        igsAgent_writeInputAsString(agent, _name, value);//last parameter is used for DATA only
                    }
                }else if ((strncmp (message, "SET_OUTPUT ", strlen("SET_OUTPUT ")) == 0) && (strlen(message) > strlen("SET_OUTPUT ")+1)){
                    igsAgent_debug(agent, "received SET_OUTPUT command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_OUTPUT") + 1;
                    char *_name, *value;
                    _name = strtok (subStr," ");
                    value = strtok (NULL,"\0");
                    if (_name != NULL && value != NULL){
                        igsAgent_writeOutputAsString(agent, _name, value);//last paramter is used for DATA only
                    }
                }else if ((strncmp (message, "SET_PARAMETER ", strlen("SET_PARAMETER ")) == 0) && (strlen(message) > strlen("SET_PARAMETER ")+1)){
                    igsAgent_debug(agent, "received SET_PARAMETER command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_PARAMETER") + 1;
                    char *_name, *value;
                    _name = strtok (subStr," ");
                    value = strtok (NULL,"\0");
                    if (_name != NULL && value != NULL){
                        igsAgent_writeParameterAsString(agent, _name, value);//last paramter is used for DATA only
                    }
                }else if ((strncmp (message, "MAP ", strlen("MAP ")) == 0) && (strlen(message) > strlen("MAP ")+1)){
                    igsAgent_debug(agent, "received MAP command from %s (%s)", name, peer);
                    char *subStr = message + strlen("MAP") + 1;
                    char *input, *agt, *output;
                    input = strtok (subStr," ");
                    agt = strtok (NULL," ");
                    output = strtok (NULL," ");
                    if (input != NULL && agent != NULL && output != NULL){
                        igsAgent_addMappingEntry(agent, input, agt, output);
                    }
                }else if ((strncmp (message, "UNMAP ", strlen("UNMAP ")) == 0) && (strlen(message) > strlen("UNMAP ")+1)){
                    igsAgent_debug(agent, "received UNMAP command from %s (%s)", name, peer);
                    char *subStr = message + strlen("UNMAP") + 1;
                    char *input, *agt, *output;
                    input = strtok (subStr," ");
                    agt = strtok (NULL," ");
                    output = strtok (NULL," ");
                    if (input != NULL && agent != NULL && output != NULL){
                        igsAgent_removeMappingEntryWithName(agent, input, agt, output);
                    }
                }
                //admin API
                else if (strlen("ENABLE_LOG_STREAM") == strlen(message) && strncmp (message, "ENABLE_LOG_STREAM", strlen("ENABLE_LOG_STREAM")) == 0){
                    igsAgent_debug(agent, "received ENABLE_LOG_STREAM command from %s (%s)", name, peer);
                    igs_setLogStream(true);
                }
                else if (strlen("DISABLE_LOG_STREAM") == strlen(message) && strncmp (message, "DISABLE_LOG_STREAM", strlen("DISABLE_LOG_STREAM")) == 0){
                    igsAgent_debug(agent, "received DISABLE_LOG_STREAM command from %s (%s)", name, peer);
                    igs_setLogStream(false);
                }
                else if (strlen("ENABLE_LOG_FILE") == strlen(message) && strncmp (message, "ENABLE_LOG_FILE", strlen("ENABLE_LOG_FILE")) == 0){
                    igsAgent_debug(agent, "received ENABLE_LOG_FILE command from %s (%s)", name, peer);
                    igs_setLogInFile(true);
                }
                else if (strlen("DISABLE_LOG_FILE") == strlen(message) && strncmp (message, "DISABLE_LOG_FILE", strlen("DISABLE_LOG_FILE")) == 0){
                    igsAgent_debug(agent, "received DISABLE_LOG_FILE command from %s (%s)", name, peer);
                    igs_setLogInFile(false);
                }
                else if ((strncmp (message, "SET_LOG_PATH ", strlen("SET_LOG_PATH ")) == 0) && (strlen(message) > strlen("SET_LOG_PATH ")+1)){
                    igsAgent_debug(agent, "received SET_LOG_PATH command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_LOG_PATH") + 1;
                    igs_setLogPath(subStr);
                }
                else if ((strncmp (message, "SET_DEFINITION_PATH ", strlen("SET_DEFINITION_PATH ")) == 0)
                         && (strlen(message) > strlen("SET_DEFINITION_PATH ")+1)){
                    igsAgent_debug(agent, "received SET_DEFINITION_PATH command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_DEFINITION_PATH") + 1;
                    igsAgent_setDefinitionPath(agent, subStr);
                }
                else if ((strncmp (message, "SET_MAPPING_PATH ", strlen("SET_MAPPING_PATH ")) == 0)
                         && (strlen(message) > strlen("SET_MAPPING_PATH ")+1)){
                    igsAgent_debug(agent, "received SET_MAPPING_PATH command from %s (%s)", name, peer);
                    char *subStr = message + strlen("SET_MAPPING_PATH") + 1;
                    igsAgent_setMappingPath(agent, subStr);
                }
                else if (strlen("SAVE_DEFINITION_TO_PATH") == strlen(message) && strncmp (message, "SAVE_DEFINITION_TO_PATH", strlen("SAVE_DEFINITION_TO_PATH")) == 0){
                    igsAgent_debug(agent, "received SAVE_DEFINITION_TO_PATH command from %s (%s)", name, peer);
                    igsAgent_writeDefinitionToPath(agent);
                }
                else if (strlen("SAVE_MAPPING_TO_PATH") == strlen(message) && strncmp (message, "SAVE_MAPPING_TO_PATH", strlen("SAVE_MAPPING_TO_PATH")) == 0){
                    igsAgent_debug(agent, "received SAVE_MAPPING_TO_PATH command from %s (%s)", name, peer);
                    igsAgent_writeMappingToPath(agent);
                }
                //CALLS
                else if (strcmp (message, "CALL") == 0){
                    char *callName = zmsg_popstr(msgDuplicate);
                    igs_call_t *call = NULL;
                    if (agent->definition != NULL && agent->definition->calls_table != NULL){
                        HASH_FIND_STR(agent->definition->calls_table, callName, call);
                        if (call != NULL ){
                            if (call->cb != NULL){
                                bus_zyreLock();
                                zyre_shouts(agent->loopElements->node, agent->callsChannel, "%s from %s (%s)", callName, name, peer);
                                bus_zyreUnlock();
                                size_t nbArgs = 0;
                                igs_callArgument_t *_arg = NULL;
                                LL_COUNT(call->arguments, _arg, nbArgs);
                                if (call_addValuesToArgumentsFromMessage(callName, call->arguments, msgDuplicate)){
                                    (call->cb)(agent, name, peer, callName, call->arguments, nbArgs, call->cbData);
                                    call_freeValuesInArguments(call->arguments);
                                }
                            }else{
                                igsAgent_warn(agent, "no defined callback to handle received call %s", callName);
                            }
                        }else{
                            igsAgent_warn(agent, "agent %s has no call named %s", name, callName);
                        }
                    }
                }
                //Performance
                else if (strcmp (message, "PING") == 0){
                    //we are pinged by another agent
                    zframe_t *countF = zmsg_pop(msgDuplicate);
                    size_t count = 0;
                    memcpy(&count, zframe_data(countF), sizeof(size_t));
                    zframe_t *payload = zmsg_pop(msgDuplicate);
                    //igsAgent_info(agent, "ping %zu from %s", count, peer);
                    zmsg_t *back = zmsg_new();
                    zmsg_addstr(back, "PONG");
                    zmsg_addmem(back, &count, sizeof(size_t));
                    zmsg_append(back, &payload);
                    bus_zyreLock();
                    zyre_whisper(node, peer, &back);
                    bus_zyreUnlock();
                }
                else if (strcmp (message, "PONG") == 0){
                    //continue performance measurement
                    zframe_t *countF = zmsg_pop(msgDuplicate);
                    size_t count = 0;
                    memcpy(&count, zframe_data(countF), sizeof(size_t));
                    zframe_t *payload = zmsg_pop(msgDuplicate);
                    //igsAgent_info(agent, "pong %zu from %s", count, peer);
                    if (count != agent->performanceMsgCounter){
                        igsAgent_error(agent, "pong message lost at index %zu from %s", count, peer);
                    } else if (count == agent->performanceMsgCountTarget){
                        //last message received
                        agent->performanceStop = zclock_usecs();
                        igsAgent_info(agent, "message size: %zu bytes", agent->performanceMsgSize);
                        igsAgent_info(agent, "roundtrip count: %zu", agent->performanceMsgCountTarget);
                        igsAgent_info(agent, "average latency: %.3f µs", ((double) agent->performanceStop - (double) agent->performanceStart) / agent->performanceMsgCountTarget);
                        double throughput = (size_t) ((double) agent->performanceMsgCountTarget / ((double) agent->performanceStop - (double) agent->performanceStart) * 1000000);
                        double megabytes = (double) throughput * agent->performanceMsgSize / (1024*1024);
                        igsAgent_info(agent, "average roundtrip throughput: %d msg/s", (int)throughput);
                        igsAgent_info(agent, "average roundtrip throughput: %.3f MB/s", megabytes);
                        agent->performanceMsgCountTarget = 0;
                    } else {
                        agent->performanceMsgCounter++;
                        zmsg_t *back = zmsg_new();
                        zmsg_addstr(back, "PING");
                        zmsg_addmem(back, &agent->performanceMsgCounter, sizeof(size_t));
                        zmsg_append(back, &payload);
                        bus_zyreLock();
                        zyre_whisper(node, peer, &back);
                        bus_zyreUnlock();
                    }
                    
                }
            }
            free(message);
        } else if (streq (event, "EXIT")){
            igsAgent_debug(agent, "<-%s (%s) exited", name, peer);
            zyreAgent_t *a = NULL;
            HASH_FIND_STR(agent->zyreAgents, peer, a);
            if (a != NULL){
                if (a->reconnected > 0){
                    //do not clean: we are getting a timemout now whereas
                    //the agent is reconnected
                    a->reconnected--;
                }else{
                    HASH_DEL(agent->zyreAgents, a);
                    free(a);
                    // Try to find the subscriber to destory
                    subscriber_t *subscriber = NULL;
                    HASH_FIND_STR(agent->subscribers, peer, subscriber);
                    if (subscriber != NULL){
                        network_cleanAndFreeSubscriber(agent, subscriber);
                    }
                }
            }
        }
        
        //handle callbacks
        zyreCallback_t *elt;
        DL_FOREACH(agent->zyreCallbacks,elt){
            if (zyre_event != NULL){
                zmsg_t *dup = zmsg_dup(msg);
                elt->callback_ptr(agent, event, peer, name, address, group, headers, dup, elt->myData);
                zmsg_destroy(&dup);
            }else{
                igsAgent_error(agent, "previous callback certainly destroyed the bus event : next callbacks will not be executed");
                break;
            }
        }
        zmsg_destroy(&msgDuplicate);
        zyre_event_destroy(&zyre_event);
    }
    network_Unlock();
    return 0;
}

//Timer callback to (re)send our definition to agents present on the private channel
int triggerDefinitionUpdate(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igsAgent_t *agent = (igsAgent_t *)arg;

    if (agent->network_needToSendDefinitionUpdate){
        model_readWriteLock();
        char * definitionStr = NULL;
        definitionStr = parser_export_definition(agent->definition);
        if (definitionStr != NULL){
            zyreAgent_t *a, *tmp;
            HASH_ITER(hh, agent->zyreAgents, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendDefinitionToAgent(agent, a->peerId, definitionStr);
                }
            }
            free(definitionStr);
        }
        agent->network_needToSendDefinitionUpdate = false;
        //when definition changes, mapping may need to be updated as well
        agent->network_needToUpdateMapping = true;
        model_readWriteUnlock();
    }
    return 0;
}

//Timer callback to stop loop if license is expired or demo mode is on
int triggerLicenseStop(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop);
    IGS_UNUSED(timer_id);
    igsAgent_t *agent = (igsAgent_t *)arg;
    igs_license("Runtime duration limit has been reached : stopping ingescape now");
    license_callback_t *el = NULL;
    DL_FOREACH(agent->licenseCallbacks, el){
        el->callback_ptr(agent, IGS_LICENSE_TIMEOUT, el->data);
    }
    return -1;
}

//Timer callback to update and (re)send our mapping to agents on the private channel
int triggerMappingUpdate(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igsAgent_t *agent = (igsAgent_t *)arg;

    if (agent->network_needToUpdateMapping){
        char *mappingStr = NULL;
        mappingStr = parser_export_mapping(agent->mapping);
        if (mappingStr != NULL){
            zyreAgent_t *a, *tmp;
            HASH_ITER(hh, agent->zyreAgents, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendMappingToAgent(agent, a->peerId, mappingStr);
                }
            }
            free(mappingStr);
        }
        subscriber_t *s, *tmp;
        HASH_ITER(hh, agent->subscribers, s, tmp){
            network_manageSubscriberMapping(agent, s);
        }
        agent->network_needToUpdateMapping = false;
    }
    return 0;
}

static void
initLoop (zsock_t *pipe, void *args){
    network_Lock();
    igsAgent_t *agent = (igsAgent_t *)args;
    igsAgent_debug(agent, "loop init");
    
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    if (agent->licenseEnforcement != NULL){
        free(agent->licenseEnforcement);
    }
    agent->licenseEnforcement = calloc(1, sizeof(licenseEnforcement_t));
    //count our own presence and IOPs
    agent->licenseEnforcement->currentAgentsNb = 1;
    if (agent->definition != NULL){
        agent->licenseEnforcement->currentIOPNb = (HASH_COUNT(agent->definition->inputs_table) +
                                        HASH_COUNT(agent->definition->outputs_table) +
                                        HASH_COUNT(agent->definition->params_table));
    }else{
        agent->licenseEnforcement->currentIOPNb = 0;
    }
    
    //NB: counting the number of IOPs here does not include future
    //definition modifications when the agent is running.
#endif

    agent->network_needToSendDefinitionUpdate = false;
    agent->network_needToUpdateMapping = false;

    bool canContinue = true;
    //prepare zyre
    bus_zyreLock();
    agent->loopElements->node = zyre_new (agent->agentName);
    //zyre_set_verbose(agent->loopElements->node);
    bus_zyreUnlock();
    if (strlen(agent->loopElements->brokerEndPoint) > 0){
        bus_zyreLock();
        zyre_set_verbose(agent->loopElements->node);
        zyre_gossip_connect(agent->loopElements->node,
                            "%s", agent->loopElements->brokerEndPoint);
        bus_zyreUnlock();
    }else{
        if (agent->loopElements->node == NULL){
            igsAgent_fatal(agent, "Could not create bus node : Agent will interrupt immediately.");
            return;
        }else{
            bus_zyreLock();
            zyre_set_interface(agent->loopElements->node, agent->loopElements->networkDevice);
            zyre_set_port(agent->loopElements->node, agent->loopElements->zyrePort);
            bus_zyreUnlock();
        }
    }
    bus_zyreLock();
    zyre_set_interval(agent->loopElements->node, agent->network_discoveryInterval);
    zyre_set_expired_timeout(agent->loopElements->node, agent->network_agentTimeout);
    bus_zyreUnlock();
    
    //create channel for replay
    snprintf(agent->replayChannel, MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-REPLAY", agent->agentName);
    bus_zyreLock();
    zyre_join(agent->loopElements->node, agent->replayChannel);
    bus_zyreUnlock();
    
    //create channel for calls feedback
    snprintf(agent->callsChannel, MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-CALLS", agent->agentName);
    bus_zyreLock();
    zyre_join(agent->loopElements->node, agent->callsChannel);
    bus_zyreUnlock();
    
    //Add version and protocol to headers
    bus_zyreLock();
    zyre_set_header(agent->loopElements->node, "ingescape", "v%d.%d.%d", (int)igs_version()/10000, (int)(igs_version() %10000)/100, (int)(igs_version() %100));
    zyre_set_header(agent->loopElements->node, "protocol", "v%d", igs_protocol());
    bus_zyreUnlock();
    
    //Add stored headers to zyre
    serviceHeader_t *el, *tmp;
    bus_zyreLock();
    HASH_ITER(hh, agent->serviceHeaders, el, tmp){
        zyre_set_header(agent->loopElements->node, el->key, "%s", el->value);
    }
    bus_zyreUnlock();
    
    //start TCP publisher
    char endpoint[512];
    if (agent->network_publishingPort == 0){
        snprintf(endpoint, 511, "tcp://%s:*", agent->loopElements->ipAddress);
    }else{
        snprintf(endpoint, 511, "tcp://%s:%d", agent->loopElements->ipAddress, agent->network_publishingPort);
    }
    agent->loopElements->publisher = zsock_new_pub(endpoint);
    if (agent->loopElements->publisher == NULL){
        igsAgent_error(agent, "Could not create publishing socket (%s): Agent will interrupt immediately.", endpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(agent->loopElements->publisher, agent->network_hwmValue);
        strncpy(endpoint, zsock_endpoint(agent->loopElements->publisher), 256);
        char *insert = endpoint + strlen(endpoint) - 1;
        while (*insert != ':' && insert > endpoint) {
            insert--;
        }
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "publisher", "%s", insert + 1);
        bus_zyreUnlock();
    }

    //start ipc publisher
    char *ipcEndpoint = NULL;
#if defined __unix__ || defined __APPLE__ || defined __linux__
    char *ipcFullPath = NULL;
    if (agent->ipcFolderPath == NULL){
        agent->ipcFolderPath = strdup(DEFAULT_IPC_PATH);
    }
    struct stat st;
    if(stat(agent->ipcFolderPath,&st) != 0){
        igsAgent_warn(agent, "Expected IPC directory %s does not exist : create it to remove this warning", agent->ipcFolderPath);
    }
    bus_zyreLock();
    ipcFullPath = calloc(1, strlen(agent->ipcFolderPath)+strlen(zyre_uuid(agent->loopElements->node))+2);
    sprintf(ipcFullPath, "%s/%s", agent->ipcFolderPath, zyre_uuid(agent->loopElements->node));
    ipcEndpoint = calloc(1, strlen(agent->ipcFolderPath)+strlen(zyre_uuid(agent->loopElements->node))+8);
    sprintf(ipcEndpoint, "ipc://%s/%s", agent->ipcFolderPath, zyre_uuid(agent->loopElements->node));
    bus_zyreUnlock();
    zsock_t *ipcPublisher = agent->loopElements->ipcPublisher = zsock_new_pub(ipcEndpoint);
    if (ipcPublisher == NULL){
        igsAgent_warn(agent, "Could not create IPC publishing socket (%s)", ipcEndpoint);
    }else{
        zsock_set_sndhwm(agent->loopElements->ipcPublisher, agent->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "ipc", "%s", ipcEndpoint);
        bus_zyreUnlock();
    }
#elif (defined WIN32 || defined _WIN32)
    ipcEndpoint = strdup("tcp://127.0.0.1:*");
    zsock_t *ipcPublisher = agent->loopElements->ipcPublisher = zsock_new_pub(ipcEndpoint);
    zsock_set_sndhwm(agent->loopElements->ipcPublisher, agent->network_hwmValue);
    if (ipcPublisher == NULL){
        igsAgent_warn(agent, "Could not create loopback publishing socket (%s)", ipcEndpoint);
    }else{
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "loopback", "%s", zsock_endpoint(ipcPublisher));
        bus_zyreUnlock();
    }
#endif
    
    //start inproc publisher
    bus_zyreLock();
    char *inprocEndpoint = calloc(1, sizeof(char) * (12 + strlen(zyre_uuid(agent->loopElements->node))));
    sprintf(inprocEndpoint, "inproc://%s", zyre_uuid(agent->loopElements->node));
    bus_zyreUnlock();
    agent->loopElements->inprocPublisher = zsock_new_pub(inprocEndpoint);
    if (agent->loopElements->inprocPublisher == NULL){
        igsAgent_warn(agent, "Could not create inproc publishing socket (%s)", inprocEndpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(agent->loopElements->inprocPublisher, agent->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "inproc", "%s", inprocEndpoint);
        bus_zyreUnlock();
    }
    
    //start logger stream if needed
    if (agent->logInStream){
        if (agent->network_logStreamPort == 0){
            sprintf(endpoint, "tcp://%s:*", agent->loopElements->ipAddress);
        } else {
            sprintf(endpoint, "tcp://%s:%d", agent->loopElements->ipAddress, agent->network_logStreamPort);
        }
        agent->loopElements->logger = zsock_new_pub(endpoint);
        if (agent->loopElements->logger == NULL){
            igsAgent_error(agent, "Could not create log stream socket (%s): Agent will interrupt immediately.", endpoint);
            canContinue = false;
        } else {
            zsock_set_sndhwm(agent->loopElements->logger, agent->network_hwmValue);
            strncpy(endpoint, zsock_endpoint(agent->loopElements->logger), 256);
            char *insert = endpoint + strlen(endpoint) - 1;
            while (*insert != ':' && insert > endpoint) {
                insert--;
            }
            bus_zyreLock();
            zyre_set_header(agent->loopElements->node, "logger", "%s", insert + 1);
            bus_zyreUnlock();
        }
    }
    
    //set other headers for agent
    bus_zyreLock();
    zyre_set_header(agent->loopElements->node, "canBeFrozen", "%i", agent->canBeFrozen);
    bus_zyreUnlock();

#if defined __unix__ || defined __APPLE__ || defined __linux__
    ssize_t ret;
    pid_t pid;
    pid = getpid();
    bus_zyreLock();
    zyre_set_header(agent->loopElements->node, "pid", "%i", pid);
    bus_zyreUnlock();
    
    if (strlen(agent->commandLine) == 0){
        //command line was not set manually : we try to get exec path instead
#ifdef __APPLE__
#if TARGET_OS_IOS
        char pathbuf[64] = "no_path";
        ret = 1;
#elif TARGET_OS_OSX
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
#endif
#else
        char pathbuf[4*1024];
        memset(pathbuf, 0, 4*1024);
        ret = readlink("/proc/self/exe", pathbuf, sizeof(pathbuf));
#endif
        if ( ret <= 0 ) {
            igsAgent_error(agent, "PID %d: proc_pidpath () - %s", pid, strerror(errno));
        } else {
            igsAgent_debug(agent, "proc %d: %s", pid, pathbuf);
        }
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "commandline", "%s", pathbuf);
        bus_zyreUnlock();
    }else{
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "commandline", "%s", agent->commandLine);
        bus_zyreUnlock();
    }
#endif
    
#if (defined WIN32 || defined _WIN32)
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    if (strlen(agent->commandLine) == 0){
        //command line was not set manually : we try to get exec path instead

        // Use GetModuleFileName() to get exec path
        char exeFilePath[IGS_MAX_PATH];
#ifdef UNICODE
        WCHAR temp[IGS_MAX_PATH];
        GetModuleFileName(NULL,temp,IGS_MAX_PATH);
        //Conversion in char *
        wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));
#else
        GetModuleFileName(NULL,exeFilePath,IGS_MAX_PATH);
#endif
        
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "commandline", "%s", exeFilePath);
        bus_zyreUnlock();
    }else{
        bus_zyreLock();
        zyre_set_header(agent->loopElements->node, "commandline", "%s", agent->commandLine);
        bus_zyreUnlock();
    }
    DWORD pid = GetCurrentProcessId();
    bus_zyreLock();
    zyre_set_header(agent->loopElements->node, "pid", "%i", (int)pid);
    bus_zyreUnlock();
#endif
    agent->loopElements->processId = (int)pid;


    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    #if (defined WIN32 || defined _WIN32)
        WSACleanup();
    #endif
    bus_zyreLock();
    zyre_set_header(agent->loopElements->node, "hostname", "%s", hostname);
    bus_zyreUnlock();
    //code for Fully Qualified Domain Name if needed someday
//    struct addrinfo hints, *info, *p;
//    int gai_result;
//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
//    hints.ai_socktype = SOCK_STREAM;
//    hints.ai_flags = AI_CANONNAME;
//    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
//        igsAgent_debug(agent, "getaddrinfo error: %s\n", gai_strerror(gai_result));
//        exit(1);
//    }
//    for(p = info; p != NULL; p = p->ai_next) {
//        igsAgent_debug(agent, "hostname: %s\n", p->ai_canonname);
//    }
//    freeaddrinfo(info);
    
    //finally start zyre now that everything is set
    bus_zyreLock();
    int zyreStartRes = zyre_start (agent->loopElements->node);
    bus_zyreUnlock();
    if (zyreStartRes == -1){
        igsAgent_error(agent, "Could not start bus node : Agent will interrupt immediately.");
        canContinue = false;
    }
    bus_zyreLock();
    zyre_join(agent->loopElements->node, CHANNEL);
    bus_zyreUnlock();
    zsock_signal (pipe, 0); //notify main thread that we are ready
    
    zmq_pollitem_t zpipePollItem;
    zmq_pollitem_t zyrePollItem;

    //main zmq socket (i.e. main thread)
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        igsAgent_error(agent, "Could not get the pipe descriptor to the main thread for polling : Agent will interrupt immediately.");
        canContinue = false;
    }
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;

    //zyre socket
    void *zsock = zsock_resolve(zyre_socket (agent->loopElements->node));
    if (zsock == NULL){
        igsAgent_error(agent, "Could not get the bus socket for polling : Agent will interrupt immediately.");
        canContinue = false;
    }
    zyrePollItem.socket = zsock;
    zyrePollItem.fd = 0;
    zyrePollItem.events = ZMQ_POLLIN;
    zyrePollItem.revents = 0;

    agent->loopElements->loop = zloop_new ();
    assert (agent->loopElements->loop);
    zloop_set_verbose (agent->loopElements->loop, false);

    zloop_poller (agent->loopElements->loop, &zpipePollItem, manageParent, agent);
    zloop_poller_set_tolerant(agent->loopElements->loop, &zpipePollItem);
    zloop_poller (agent->loopElements->loop, &zyrePollItem, manageBusIncoming, agent);
    zloop_poller_set_tolerant(agent->loopElements->loop, &zyrePollItem);
    
    zloop_timer(agent->loopElements->loop, 1000, 0, triggerDefinitionUpdate, agent);
    zloop_timer(agent->loopElements->loop, 1000, 0, triggerMappingUpdate, agent);
    
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    if (agent->license != NULL && !agent->license->isLicenseValid){
        igs_license("License is not valid : starting timer for demonstration mode (%d seconds)...", MAX_EXEC_DURATION_DURING_EVAL);
        zloop_timer(agent->loopElements->loop, MAX_EXEC_DURATION_DURING_EVAL * 1000, 0, triggerLicenseStop, agent);
    }
#endif
    igsAgent_debug(agent, "loop starting");
    network_Unlock();

    if (canContinue){
        zloop_start (agent->loopElements->loop); //start returns when one of the pollers returns -1
    }
    
    network_Lock();
    igsAgent_debug(agent, "loop stopping...");

    //clean
    zyreAgent_t *zagent, *tmpa;
    HASH_ITER(hh, agent->zyreAgents, zagent, tmpa){
        HASH_DEL(agent->zyreAgents, zagent);
        free(zagent);
    }
    subscriber_t *s, *tmps;
    HASH_ITER(hh, agent->subscribers, s, tmps) {
        network_cleanAndFreeSubscriber(agent, s);
    }
    bus_zyreLock();
    zyre_stop (agent->loopElements->node);
    bus_zyreUnlock();
    zclock_sleep (100);
    zyre_destroy (&agent->loopElements->node);
    zsock_destroy(&agent->loopElements->publisher);
    if (agent->loopElements->ipcPublisher != NULL){
        zsock_destroy(&agent->loopElements->ipcPublisher);
#if defined __unix__ || defined __APPLE__ || defined __linux__
        if (ipcFullPath != NULL){
            zsys_file_delete(ipcFullPath); //destroy ipcPath in file system
            //NB: ipcPath is based on peer id which is unique. It will never be used again.
            free(ipcFullPath);
        }
#endif
    }
    if (agent->loopElements->inprocPublisher != NULL){
        zsock_destroy(&agent->loopElements->inprocPublisher);
    }
    free(ipcEndpoint);
    
    if (agent->loopElements->logger != NULL){
        zsock_destroy(&agent->loopElements->logger);
    }
    
    zloop_destroy (&agent->loopElements->loop);
    assert (agent->loopElements->loop == NULL);
    //call registered interruption callbacks
    forcedStopCalback_t *cb = NULL;
    if (agent->forcedStop){
        DL_FOREACH(agent->forcedStopCalbacks, cb){
            cb->callback_ptr(agent, cb->myData);
        }
    }
    if (agent->forcedStop){
        agent->isInterrupted = true;
        //in case of forced stop, we send SIGINT to our process so
        //that it can be trapped by main thread for a proper stop
        #if defined __unix__ || defined __APPLE__ || defined __linux__
        igsAgent_debug(agent, "triggering SIGINT");
        kill(pid, SIGINT);
        #endif
        //TODO : do that for windows also
    }
    igsAgent_debug(agent, "loop stopped");
    network_Unlock();
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

int network_publishOutput (igsAgent_t *agent, const agent_iop_t *iop){
    int result = 0;
    if (iop == NULL){
        igsAgent_warn(agent, "passed IOP is NULL");
        return 0;
    }
    
    model_readWriteLock();
    if(agent->loopElements != NULL && agent->loopElements->publisher != NULL && iop != NULL)
    {
        if(!agent->isWholeAgentMuted && !iop->is_muted && iop->name != NULL && !agent->isFrozen)
        {
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, iop->name);
            zmsg_addstrf(msg, "%d", iop->value_type);
            switch (iop->value_type) {
                case IGS_INTEGER_T:
                    zmsg_addmem(msg, &(iop->value.i), sizeof(int));
                    igsAgent_debug(agent, "publish %s -> %d",iop->name,iop->value.i);
                    break;
                case IGS_DOUBLE_T:
                    zmsg_addmem(msg, &(iop->value.d), sizeof(double));
                    igsAgent_debug(agent, "publish %s -> %f",iop->name,iop->value.d);
                    break;
                case IGS_BOOL_T:
                    zmsg_addmem(msg, &(iop->value.b), sizeof(bool));
                    igsAgent_debug(agent, "publish %s -> %d",iop->name,iop->value.b);
                    break;
                case IGS_STRING_T:
                    zmsg_addstr(msg, iop->value.s);
                    igsAgent_debug(agent, "publish %s -> %s",iop->name,iop->value.s);
                    break;
                case IGS_IMPULSION_T:
                    zmsg_addmem(msg, NULL, 0);
                    igsAgent_debug(agent, "publish impulsion %s",iop->name);
                    break;
                case IGS_DATA_T:{
                    zframe_t *frame = zframe_new(iop->value.data, iop->valueSize);
                    zmsg_append(msg, &frame);
                    igsAgent_debug(agent, "publish data %s",iop->name);
                }
                    break;
                default:
                    break;
            }
            zmsg_t *msgBis = zmsg_dup(msg);
            //successively publish to TCP, IPC and inproc
            if (zmsg_send(&msg, agent->loopElements->publisher) != 0){
                igsAgent_error(agent, "Could not publish output %s on the network\n",iop->name);
                zmsg_destroy(&msgBis);
            }else{
                result = 1;
                zmsg_t *msgTer = zmsg_dup(msgBis);
                if (agent->loopElements->ipcPublisher != NULL){
                    //publisher can be NULL on IOS or for read/write problems with assigned IPC path
                    //in both cases, an error message has been issued at start
                    if (zmsg_send(&msgBis, agent->loopElements->ipcPublisher) != 0){
                        igsAgent_error(agent, "Could not publish output %s using IPC\n",iop->name);
                        zmsg_destroy(&msgBis);
                    }
                }
                if (agent->loopElements->inprocPublisher != NULL){
                    if (zmsg_send(&msgTer, agent->loopElements->inprocPublisher) != 0){
                        igsAgent_error(agent, "Could not publish output %s using inproc\n",iop->name);
                        zmsg_destroy(&msgTer);
                    }
                }
            }
        }else{
            if(iop == NULL){
                igsAgent_error(agent, "Output %s is unknown", iop->name);
            }
            if (agent->isWholeAgentMuted){
                igsAgent_debug(agent, "Should publish output %s but the agent has been muted",iop->name);
            }
            if(iop->is_muted){
                igsAgent_debug(agent, "Should publish output %s but it has been muted",iop->name);
            }
            if(agent->isFrozen == true){
                igsAgent_debug(agent, "Should publish output %s but the agent has been frozen",iop->name);
            }
        }
    }
    model_readWriteUnlock();
    return result;
}

int igsAgent_observeBus(igsAgent_t *agent, igsAgent_BusMessageIncoming cb, void *myData){
    if (cb != NULL){
        zyreCallback_t *newCb = calloc(1, sizeof(zyreCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->zyreCallbacks, newCb);
    }else{
        igsAgent_error(agent, "callback is null");
        return 0;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

/**
 *  \defgroup startStopKillFct Agent: Start / Stop / Kill functions
 *
 */

/**
 * \fn int igs_startWithDevice(const char *networkDevice, int port)
 * \ingroup startStopKillFct
 * \brief Start an agent on a specific network device and network port.
 * \param networkDevice is the name of the network device (ex: eth0, ens2 ...)
 * \param port is the network port number used
 * \return 1 if ok, else 0.
 */
int igsAgent_startWithDevice(igsAgent_t *agent, const char *networkDevice, unsigned int port){
    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igsAgent_error(agent, "networkDevice cannot be NULL or empty");
        return 0;
    }
    
    if (agent->loopElements != NULL){
        //Agent is already active : need to stop it first
        igsAgent_stop(agent);
    }
    agent->isInterrupted = false;
    agent->forcedStop = false;
    
    agent->loopElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agent->loopElements->networkDevice, networkDevice, NETWORK_DEVICE_LENGTH-1);
    agent->loopElements->brokerEndPoint[0] = '\0';
    agent->loopElements->ipAddress[0] = '\0';
    
#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
        HIBYTE (wsa_data.wVersion) == 2);
#endif

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
//        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
//                name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
        if (strcmp(name, networkDevice) == 0){
            strncpy(agent->loopElements->ipAddress, ziflist_address (iflist), IP_ADDRESS_LENGTH-1);
            igsAgent_info(agent, "Starting with ip address %s and port %d on device %s", agent->loopElements->ipAddress, port, networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(agent->loopElements->ipAddress) == 0){
        igsAgent_error(agent, "IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        free(agent->loopElements);
        agent->loopElements = NULL;
        return 0;
    }
#if !TARGET_OS_IOS
    license_readLicense(agent);
#endif
    agent->loopElements->zyrePort = port;
    agent->loopElements->agentActor = zactor_new (initLoop, agent);
    assert (agent->loopElements->agentActor);
    igs_nbOfInternalAgents++;
    return 1;
}

/**
 * \fn int igs_startWithIP(const char *ipAddress, int port)
 * \ingroup startStopKillFct
 * \brief Start an agent on a specific network IP and network port.
 * \param ipAddress is the ip address on network
 * \param port s the network port number used
 * \return 1 if ok, else 0.
 */
int igsAgent_startWithIP(igsAgent_t *agent, const char *ipAddress, unsigned int port){
    if ((ipAddress == NULL) || (strlen(ipAddress) == 0)){
        igsAgent_error(agent, "IP address cannot be NULL or empty");
        return 0;
    }
    
    if (agent->loopElements != NULL){
        //Agent is already active : need to stop it first
        igsAgent_stop(agent);
    }
    agent->isInterrupted = false;
    agent->forcedStop = false;
    agent->loopElements = calloc(1, sizeof(zyreloopElements_t));
    agent->loopElements->brokerEndPoint[0] = '\0';
    strncpy(agent->loopElements->ipAddress, ipAddress, IP_ADDRESS_LENGTH-1);
    
#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
        HIBYTE (wsa_data.wVersion) == 2);
#endif

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
//        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
//                name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
        if (strcmp(ziflist_address (iflist), ipAddress) == 0){
            strncpy(agent->loopElements->networkDevice, name, 15);
            igsAgent_info(agent, "Starting with ip address %s and port %d on device %s", ipAddress, port, agent->loopElements->networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(agent->loopElements->networkDevice) == 0){
        igsAgent_error(agent, "Device name could not be determined for IP address %s : our agent will NOT start", ipAddress);
        agent->loopElements = NULL;
        free(agent->loopElements);
        return 0;
    }
#if !TARGET_OS_IOS
    license_readLicense(agent);
#endif
    agent->loopElements->zyrePort = port;
    agent->loopElements->agentActor = zactor_new (initLoop, agent);
    assert (agent->loopElements->agentActor);
    igs_nbOfInternalAgents++;
    return 1;
}

int igsAgent_startWithDeviceOnBroker(igsAgent_t *agent, const char *networkDevice, const char *brokerEndpoint){
    //TODO: manage a list of brokers instead of just one
    if ((brokerEndpoint == NULL) || (strlen(brokerEndpoint) == 0)){
        igsAgent_error(agent, "brokerIpAddress cannot be NULL or empty");
        return 0;
    }
    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igsAgent_error(agent, "networkDevice cannot be NULL or empty");
        return 0;
    }
    
    if (agent->loopElements != NULL){
        igsAgent_stop(agent);
    }
    agent->isInterrupted = false;
    agent->forcedStop = false;
    
    agent->loopElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agent->loopElements->brokerEndPoint, brokerEndpoint, IP_ADDRESS_LENGTH-1);
    strncpy(agent->loopElements->networkDevice, networkDevice, NETWORK_DEVICE_LENGTH-1);
    agent->loopElements->ipAddress[0] = '\0';
    
#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
            HIBYTE (wsa_data.wVersion) == 2);
#endif
    
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        //        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
        //                name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
        if (strcmp(name, networkDevice) == 0){
            strncpy(agent->loopElements->ipAddress, ziflist_address (iflist), IP_ADDRESS_LENGTH-1);
            igsAgent_info(agent, "Starting with ip address %s on device %s with broker %s",
                      agent->loopElements->ipAddress,
                      networkDevice,
                      agent->loopElements->brokerEndPoint);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(agent->loopElements->ipAddress) == 0){
        igsAgent_error(agent, "IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        free(agent->loopElements);
        agent->loopElements = NULL;
        return 0;
    }
#if !TARGET_OS_IOS
    license_readLicense(agent);
#endif
    agent->loopElements->zyrePort = 0;
    agent->loopElements->agentActor = zactor_new (initLoop, agent);
    assert (agent->loopElements->agentActor);
    igs_nbOfInternalAgents++;
    return 1;
}


int igsAgent_stop(igsAgent_t *agent){
    if (agent->loopElements != NULL){
        //interrupting and destroying ingescape thread and zyre layer
        //this will also clean all agent->subscribers
        if (agent->loopElements->node != NULL){
            //we send message only if zactor is still active, i.e.
            //if its node still exists
            zstr_sendx (agent->loopElements->agentActor, "$TERM", NULL);
        }
        if (agent->loopElements->agentActor != NULL){
            zactor_destroy (&agent->loopElements->agentActor);
        }
        //cleaning agent
        free (agent->loopElements);
        agent->loopElements = NULL;
        igs_nbOfInternalAgents--;
        //igsAgent_debug(agent, "still %d internal agents running", igs_nbOfInternalAgents);
        #if (defined WIN32 || defined _WIN32)
        // On Windows, we need to use a sledgehammer to avoid assertion errors
        // NB: If we don't call zsys_shutdown, the application will crash on exit
        // (WSASTARTUP assertion failure)
        // NB: Monitoring also uses a zactor, we can not call zsys_shutdown() when it is running
        if (igs_nbOfInternalAgents == 0) {
            igsAgent_debug(agent, "calling zsys_shutdown after last agent in process has stopped");
            zsys_shutdown();
        }
        #endif

        igsAgent_info(agent, "%s stopped", agent->agentName);
    }else{
        igsAgent_debug(agent, "%s already stopped", agent->agentName);
    }
#if !TARGET_OS_IOS
    license_cleanLicense(agent);
#endif
    return 1;
}


int igsAgent_setAgentName(igsAgent_t *agent, const char *name){
    if ((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Agent name cannot be NULL or empty");
        return 0;
    }
    if (strcmp(agent->agentName, name) == 0){
        //nothing to do
        return 1;
    }
    char networkDevice[NETWORK_DEVICE_LENGTH] = "";
    char ipAddress[IP_ADDRESS_LENGTH] = "";
    int zyrePort = 0;
    bool needRestart = false;
    if (agent->loopElements != NULL){
        //Agent is already started, zyre actor needs to be recreated
        strncpy(networkDevice, agent->loopElements->networkDevice, NETWORK_DEVICE_LENGTH);
        strncpy(ipAddress, agent->loopElements->ipAddress, IP_ADDRESS_LENGTH);
        zyrePort = agent->loopElements->zyrePort;
        igsAgent_stop(agent);
        needRestart = true;
    }
    char *n = strndup(name, MAX_AGENT_NAME_LENGTH);
    if (strlen(name) > MAX_AGENT_NAME_LENGTH){
        igsAgent_warn(agent, "Agent name '%s' exceeds maximum size and will be truncated to '%s'", name, n);
    }
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
        igsAgent_warn(agent, "Spaces are not allowed in agent name: %s has been renamed to %s", name, n);
    }
    strncpy(agent->agentName, n, MAX_AGENT_NAME_LENGTH-1);
    free(n);
    
    if (needRestart){
        igsAgent_startWithIP(agent, ipAddress, zyrePort);
    }
    igsAgent_debug(agent, "Agent name is %s", agent->agentName);
    return 1;
}


char *igsAgent_getAgentName(igsAgent_t *agent){
    return strdup(agent->agentName);
}


int igsAgent_freeze(igsAgent_t *agent){
    if (!agent->canBeFrozen){
        igsAgent_error(agent, "agent is requested to be frozen but is still set to 'can't be Frozen' : call igs_setCanBeFrozen to change this");
        return 0;
    }
    if(agent->isFrozen == false)
    {
        igsAgent_debug(agent, "Agent frozen");
        if ((agent->loopElements != NULL) && (agent->loopElements->node != NULL)){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "FROZEN=1");
            bus_zyreUnlock();
        }
        agent->isFrozen = true;
        freezeCallback_t *elt;
        DL_FOREACH(agent->freezeCallbacks,elt){
            elt->callback_ptr(agent->isFrozen, elt->myData);
        }
    }
    return 1;
}


bool igsAgent_isFrozen(igsAgent_t *agent){
    return agent->isFrozen;
}


int igsAgent_unfreeze(igsAgent_t *agent){
    if(agent->isFrozen == true)
    {
        igsAgent_debug(agent, "Agent resumed (unfrozen)");
        if ((agent->loopElements != NULL) && (agent->loopElements->node != NULL)){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "FROZEN=0");
            bus_zyreUnlock();
        }
        agent->isFrozen = false;
        freezeCallback_t *elt;
        DL_FOREACH(agent->freezeCallbacks,elt){
            elt->callback_ptr(agent->isFrozen, elt->myData);
        }
    }
    return 1;
}


int igsAgent_observeFreeze(igsAgent_t *agent, igsAgent_freezeCallback cb, void *myData){
    if (cb != NULL){
        freezeCallback_t *newCb = calloc(1, sizeof(freezeCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->freezeCallbacks, newCb);
    }else{
        igsAgent_warn(agent, "callback is null");
        return 0;
    }
    return 1;
}


int igsAgent_setAgentState(igsAgent_t *agent, const char *state){
    if (state == NULL){
        igsAgent_error(agent, "state can not be NULL");
        return 0;
    }
    
    if (strcmp(state, agent->agentState) != 0){
        strncpy(agent->agentState, state, MAX_AGENT_NAME_LENGTH-1);
        igsAgent_debug(agent, "changed to %s", agent->agentState);
        if (agent->loopElements != NULL && agent->loopElements->node != NULL){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "STATE=%s", agent->agentState);
            bus_zyreUnlock();
        }
    }
    return 1;
}


char *igsAgent_getAgentState(igsAgent_t *agent){
    return strdup(agent->agentState);
}


void igsAgent_setCanBeFrozen(igsAgent_t *agent, bool canBeFrozen){
    agent->canBeFrozen = canBeFrozen;
    if (agent->loopElements != NULL && agent->loopElements->node != NULL){
        bus_zyreLock();
        //update header for information to agents not arrived yet
        zyre_set_header(agent->loopElements->node, "canBeFrozen", "%i", agent->canBeFrozen);
        //send real time notification for agents already there
        zyre_shouts(agent->loopElements->node, CHANNEL, "CANBEFROZEN=%i", canBeFrozen);
        bus_zyreUnlock();
        igsAgent_debug(agent, "changed to %d", canBeFrozen);
    }
}


bool igsAgent_canBeFrozen (igsAgent_t *agent){
    return agent->canBeFrozen;
}


int igsAgent_mute(igsAgent_t *agent){
    if (!agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = true;
        if ((agent->loopElements != NULL) && (agent->loopElements->node != NULL)){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "MUTED=%i", agent->isWholeAgentMuted);
            bus_zyreUnlock();
        }
        muteCallback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
    return 1;
}


int igsAgent_unmute(igsAgent_t *agent){
    if (agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = false;
        if ((agent->loopElements != NULL) && (agent->loopElements->node != NULL)){
            bus_zyreLock();
            zyre_shouts(agent->loopElements->node, CHANNEL, "MUTED=%i", agent->isWholeAgentMuted);
            bus_zyreUnlock();
        }
        muteCallback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
    return 1;
}


bool igsAgent_isMuted(igsAgent_t *agent){
    return agent->isWholeAgentMuted;
}


int igsAgent_observeMute(igsAgent_t *agent, igsAgent_muteCallback cb, void *myData){
    if (cb != NULL){
        muteCallback_t *newCb = calloc(1, sizeof(muteCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->muteCallbacks, newCb);
    }
    else{
        igsAgent_warn(agent, "callback is null");
        return 0;
    }
    return 1;
}


void igsAgent_die(igsAgent_t *agent){
    agent->forcedStop = true;
    igsAgent_stop(agent);
}

void igsAgent_setCommandLine(igsAgent_t *agent, const char *line){
    strncpy(agent->commandLine, line, COMMAND_LINE_LENGTH-1);
    igsAgent_debug(agent, "Command line set to %s", agent->commandLine);
}

void igsAgent_setCommandLineFromArgs(igsAgent_t *agent, int argc, const char * argv[]){
    if (argc < 1 || argv == NULL || argv[0] == NULL){
        igsAgent_error(agent, "passed args must at least contain one element");
        return;
    }
    char cmd[COMMAND_LINE_LENGTH] = "";
    
#if defined __linux__ || defined __APPLE__ || defined __unix__
    size_t ret;
    pid_t pid;
    pid = getpid();
#ifdef __APPLE__
#if TARGET_OS_IOS
    char pathbuf[64] = "no_path";
    ret = 1;
#elif TARGET_OS_OSX
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
#endif
#else
    char pathbuf[4*1024];
    memset(pathbuf, 0, 4*1024);
    ret = readlink("/proc/self/exe", pathbuf, 4*1024);
#endif
    if ( ret <= 0 ) {
        igsAgent_error(agent, "PID %d: proc_pidpath () - %s", pid, strerror(errno));
        return;
    } else {
        igsAgent_debug(agent, "proc %d: %s", pid, pathbuf);
    }
    if (strlen(pathbuf) < COMMAND_LINE_LENGTH){
        strcat(cmd, pathbuf);
    }else{
        igsAgent_error(agent, "path is too long: %s", pathbuf);
        return;
    }
    
#elif (defined WIN32 || defined _WIN32)
    // Use GetModuleFileName() to get exec path, argv[0] do not contain full path
    char exeFilePath[IGS_MAX_PATH];
#ifdef UNICODE
    WCHAR temp[IGS_MAX_PATH];
    GetModuleFileName(NULL,temp,IGS_MAX_PATH);
    //Conversion in char *
    wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));
#else
    GetModuleFileName(NULL,exeFilePath,IGS_MAX_PATH);
#endif
    strcat(cmd, exeFilePath);
#endif
    
    int i = 1;
    for (; i<argc; i++){
        if (strlen(cmd) + strlen(argv[i]) + 2 > COMMAND_LINE_LENGTH){ // 2 is for space and EOL
            igsAgent_error(agent, "passed arguments exceed buffer size: concatenation will stop here with '%s'", cmd);
            break;
        }else{
            strcat(cmd, " ");
        }
        strcat(cmd, argv[i]);
    }
    strncpy(agent->commandLine, cmd, COMMAND_LINE_LENGTH);
    igsAgent_debug(agent, "Command line set to %s", agent->commandLine);
}

void igsAgent_setRequestOutputsFromMappedAgents(igsAgent_t *agent, bool notify){
    agent->network_RequestOutputsFromMappedAgents = notify;
    igsAgent_debug(agent, "changed to %d", notify);
}

bool igsAgent_getRequestOutputsFromMappedAgents(igsAgent_t *agent){
    return agent->network_RequestOutputsFromMappedAgents;
}

#define MAX_NUMBER_OF_NETDEVICES 16

void igs_getNetdevicesList(char ***devices, int *nb){
#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
            HIBYTE (wsa_data.wVersion) == 2);
#endif
    if (devices != NULL)
        *devices = calloc(MAX_NUMBER_OF_NETDEVICES, sizeof(char*));
    int currentDeviceNb = 0;
    
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        //        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
        //                name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
        if (devices != NULL){
            (*devices)[currentDeviceNb] = calloc(NETWORK_DEVICE_LENGTH+1, sizeof(char));
            strncpy((*devices)[currentDeviceNb], name, NETWORK_DEVICE_LENGTH);
        }
        currentDeviceNb++;
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    *nb = currentDeviceNb;
#if (defined WIN32 || defined _WIN32)
    WSACleanup();
#endif
}

void igs_getNetaddressesList(char ***addresses, int *nb){
#if (defined WIN32 || defined _WIN32)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
            HIBYTE (wsa_data.wVersion) == 2);
#endif
    if (addresses != NULL)
        *addresses = calloc(MAX_NUMBER_OF_NETDEVICES, sizeof(char*));
    int currentDeviceNb = 0;
    
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        if (addresses != NULL){
            (*addresses)[currentDeviceNb] = strdup(ziflist_address(iflist));
        }
        currentDeviceNb++;
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    *nb = currentDeviceNb;
#if (defined WIN32 || defined _WIN32)
    WSACleanup();
#endif
}

void igs_freeNetdevicesList(char **devices, int nb){
    int i = 0;
    for (i = 0; i < nb; i++){
        if (devices != NULL && devices[i] != NULL){
            free(devices[i]);
        }
    }
    if (devices != NULL)
        free (devices);
}

void igs_freeNetaddressesList(char **addresses, int nb){
    igs_freeNetdevicesList(addresses, nb);
}

void igsAgent_observeForcedStop(igsAgent_t *agent, igsAgent_forcedStopCallback cb, void *myData){
    if (cb != NULL){
        forcedStopCalback_t *newCb = calloc(1, sizeof(forcedStopCalback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->forcedStopCalbacks, newCb);
    }else{
        igsAgent_warn(agent, "callback is null");
    }
}

void igsAgent_setDiscoveryInterval(igsAgent_t *agent, unsigned int interval){
    if (agent->loopElements != NULL && agent->loopElements->node != NULL){
        bus_zyreLock();
        zyre_set_interval(agent->loopElements->node, interval);
        bus_zyreUnlock();
    }
    agent->network_discoveryInterval = interval;
}

void igsAgent_setAgentTimeout(igsAgent_t *agent, unsigned int duration){
    if (agent->loopElements != NULL && agent->loopElements->node != NULL){
        bus_zyreLock();
        zyre_set_expired_timeout(agent->loopElements->node, duration);
        bus_zyreUnlock();
    }
    agent->network_agentTimeout = duration;
}

void igsAgent_setPublishingPort(igsAgent_t *agent, unsigned int port){
    if (agent->loopElements != NULL && agent->loopElements->publisher != NULL){
        igsAgent_error(agent, "agent is already started : stop it first to change its publishing port");
        return;
    }
    agent->network_publishingPort = port;
}

void igsAgent_setLogStreamPort(igsAgent_t *agent, unsigned int port){
    if (agent->loopElements != NULL && agent->loopElements->logger != NULL){
        igsAgent_error(agent, "agent is already started : stop it first to change its logger port");
        return;
    }
    agent->network_logStreamPort = port;
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igsAgent_setIpcFolderPath(igsAgent_t *agent, char *path){
    if (strcmp(path, DEFAULT_IPC_PATH) == 0
        || (agent->ipcFolderPath != NULL && strcmp(path, agent->ipcFolderPath) == 0)){
        igsAgent_debug(agent, "IPC folder path already is '%s'", path);
    }else{
        if (*path == '/'){
            if (agent->ipcFolderPath != NULL){
                free(agent->ipcFolderPath);
            }
            if (!zsys_file_exists(path)){
                igsAgent_info(agent, "folder %s was created automatically", path);
                zsys_dir_create("%s", path);
            }
            agent->ipcFolderPath = strdup(path);
        }else{
            igsAgent_error(agent, "IPC folder path must be absolute");
        }
    }
}

const char* igsAgent_getIpcFolderPath(igsAgent_t *agent){
    return agent->ipcFolderPath;
}
#endif

void igsAgent_setAllowInproc(igsAgent_t *agent, bool allow){
    agent->allowInproc = allow;
}

bool igsAgent_getAllowInproc(igsAgent_t *agent){
    return agent->allowInproc;
}

void igsAgent_setAllowIpc(igsAgent_t *agent, bool allow){
    agent->allowIpc = allow;
}

bool igsAgent_getAllowIpc(igsAgent_t *agent){
    return agent->allowIpc;
}

void igsAgent_setHighWaterMarks(igsAgent_t *agent, int hwmValue){
    if (hwmValue < 0){
        igsAgent_error(agent, "HWM value must be zero or higher");
        return;
    }
    if (agent->loopElements != NULL && agent->loopElements->publisher != NULL){
        zsock_set_sndhwm(agent->loopElements->publisher, hwmValue);
        zsock_set_sndhwm(agent->loopElements->logger, hwmValue);
        subscriber_t *tmp = NULL, *subs = NULL;
        HASH_ITER(hh, agent->subscribers, subs, tmp){
            zsock_set_rcvhwm(subs->subscriber, hwmValue);
        }
    }
    agent->network_hwmValue = hwmValue;
}
