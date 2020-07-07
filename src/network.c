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
#else
#include <sys/resource.h>
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

//Adds proper filter to 'subscribe' socket for a spectific output of a given remote agent
int subscribeToRemoteAgentOutput(igs_remote_agent_t *remoteAgent, const char *outputName){
    assert(remoteAgent);
    assert(outputName);
    if(strlen(outputName) > 0){
        bool filterAlreadyExists = false;
        igs_mappings_filter_t *filter = NULL;
        DL_FOREACH(remoteAgent->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                filterAlreadyExists = true;
                break;
            }
        }
        if (!filterAlreadyExists){
            // Set subscriber to the output filter
            assert(remoteAgent->subscriber);
            igs_debug("Subscribe to agent %s output %s",remoteAgent->name,outputName);
            zsock_set_subscribe(remoteAgent->subscriber, outputName);
            igs_mappings_filter_t *f = calloc(1, sizeof(igs_mappings_filter_t));
            f->filter = strndup(outputName, IGS_MAX_IOP_NAME_LENGTH);
            DL_APPEND(remoteAgent->mappingsFilters, f);
        }else{
            //printf("\n****************\nFILTER BIS %s - %s\n***************\n", subscriber->agent->name, outputName);
        }
        return 1;
    }
    return -1;
}

//Removes filter to 'subscribe' socket for a spectific output of a given remote agent
int unsubscribeToRemoteAgentOutput(igs_remote_agent_t *remoteAgent, const char *outputName){
    assert(remoteAgent);
    assert(outputName);
    if(strlen(outputName) > 0){
        igs_mappings_filter_t *filter = NULL;
        DL_FOREACH(remoteAgent->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                assert(remoteAgent->subscriber);
                igs_debug("Unsubscribe to agent %s output %s",remoteAgent->name,outputName);
                zsock_set_unsubscribe(remoteAgent->subscriber, outputName);
                free(filter->filter);
                DL_DELETE(remoteAgent->mappingsFilters, filter);
                free(filter);
                break;
            }
        }
        return 1;
    }
    return -1;
}

//Timer callback to send REQUEST_OUPUTS notification for an agent we subscribed to
int triggerMappingNotificationToNewcomer(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_remote_agent_t *remoteAgent = (igs_remote_agent_t *)arg;
    assert(remoteAgent);
    assert(remoteAgent->context);
    assert(remoteAgent->context->node);
    
    if (remoteAgent->isMappingNotificationSet){
        bus_zyreLock();
        zyre_whispers(remoteAgent->context->node, remoteAgent->peerId, "REQUEST_OUPUTS");
        bus_zyreUnlock();
        remoteAgent->isMappingNotificationSet = false;
    }
    return 0;
}

int network_manageMappingsToRemoteAgent(igs_core_context_t *context, igs_agent_t *agent, igs_remote_agent_t *remoteAgent){
    assert(context);
    assert(agent);
    assert(remoteAgent);
    igs_mapping_element_t *el, *tmp;
    if (agent->mapping != NULL){
        HASH_ITER(hh, agent->mapping->map_elements, el, tmp){
            if (strcmp(remoteAgent->name, el->agent_name)==0 || strcmp(el->agent_name, "*") == 0){
                //mapping element is compatible with subscriber name
                //check if we find a compatible output in subscriber definition
                igs_iop_t *foundOutput = NULL;
                if (remoteAgent->definition != NULL){
                    HASH_FIND_STR(remoteAgent->definition->outputs_table, el->output_name, foundOutput);
                }
                //check if we find a valid input in our own definition
                igs_iop_t *foundInput = NULL;
                if (agent->definition != NULL){
                    HASH_FIND_STR(agent->definition->inputs_table, el->input_name, foundInput);
                }
                //check type compatibility between input and output value types
                //including implicit conversions
                if (foundOutput != NULL && foundInput != NULL
                    && mapping_checkInputOutputCompatibility(agent, foundInput, foundOutput)){
                    //we have validated input, agent and output names : we can map
                    //NOTE: the call below may happen several times if our agent uses
                    //the remote agent ouput on several of its inputs. This should not have any consequence.
                    subscribeToRemoteAgentOutput(remoteAgent, el->output_name);
                    
                    //mapping was successful : we set timer to notify remote agent if not already done
                    if (!remoteAgent->isMappingNotificationSet && agent->network_RequestOutputsFromMappedAgents){
                        remoteAgent->isMappingNotificationSet = true;
                        remoteAgent->timerId = zloop_timer(coreContext->loop, 500, 1, triggerMappingNotificationToNewcomer, remoteAgent);
                    }
                }
                //NOTE: we do not clean subscriptions here because we cannot check if
                //an output is not used in another mapping element
            }
        }
    }
    return 0;
}

void sendDefinitionToRemoteAgent(igs_core_context_t *context, igs_remote_agent_t *remoteAgent, const char *def){
    assert(context);
    assert(context->node);
    assert(remoteAgent);
    assert(def);
    bus_zyreLock();
    zyre_whispers(coreContext->node, remoteAgent->peerId, "%s%s", definitionPrefix, def);
    bus_zyreUnlock();
}

void sendMappingToRemoteAgent(igs_core_context_t *context, igs_remote_agent_t *remoteAgent, const char *mapping){
    assert(context);
    assert(context->node);
    assert(remoteAgent);
    assert(mapping);
    bus_zyreLock();
    zyre_whispers(coreContext->node, remoteAgent->peerId, "%s%s", mappingPrefix, mapping);
    bus_zyreUnlock();
}

void network_cleanAndFreeRemoteAgent(igs_core_context_t *context, igs_agent_t *agent, igs_remote_agent_t *remoteAgent){
    assert(context);
    assert(agent);
    assert(remoteAgent);
    igs_debug("cleaning subscription to %s\n", remoteAgent->name);
    #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    context->licenseEnforcement->currentAgentsNb--;
    #endif
    // clean the agent definition
    if(remoteAgent->definition != NULL){
        #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
        context->licenseEnforcement->currentIOPNb -= (HASH_COUNT(remoteAgent->definition->inputs_table) +
                                                      HASH_COUNT(remoteAgent->definition->outputs_table) +
                                                      HASH_COUNT(remoteAgent->definition->params_table));
        //igs_license("license: %ld agents and %ld iops (%s)", agent->licenseEnforcement->currentAgentsNb, agent->licenseEnforcement->currentIOPNb, subscriber->name);
        #endif
        definition_freeDefinition(remoteAgent->definition);
    }
    //clean the agent mapping
    if(remoteAgent->mapping != NULL){
        mapping_freeMapping(remoteAgent->mapping);
    }
    //clean the remoteAgent itself
    igs_mappings_filter_t *elt, *tmp;
    DL_FOREACH_SAFE(remoteAgent->mappingsFilters,elt,tmp) {
        zsock_set_unsubscribe(remoteAgent->subscriber, elt->filter);
        DL_DELETE(remoteAgent->mappingsFilters,elt);
        free(elt);
    }
    if (context->loop != NULL){
        zloop_reader_end(context->loop, remoteAgent->subscriber);
    }
    zsock_destroy(&remoteAgent->subscriber);
    if (remoteAgent->uuid)
        free(remoteAgent->uuid);
    if (remoteAgent->name)
        free(remoteAgent->name);
    if (remoteAgent->peerId)
        free(remoteAgent->peerId);
    if (context->loop != NULL && remoteAgent->timerId >= 0){
        zloop_timer_end(context->loop, remoteAgent->timerId);
        remoteAgent->timerId = -2;
    }
    HASH_DEL(context->remoteAgents, remoteAgent);
    free(remoteAgent);
}

////////////////////////////////////////////////////////////////////////
// ZMQ callbacks
////////////////////////////////////////////////////////////////////////

//manage messages from the parent thread
int manageParent (zloop_t *loop, zsock_t *pipe, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)

    zmsg_t *msg = zmsg_recv(pipe);
    if (!msg){
        igs_error("Could not read message from main thread : Ingescape will interrupt immediately.");
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
    
    return 0;
}

//function actually handling messages from one of the remote agents we subscribed to
int handleMessageFromRemoteAgent(zmsg_t *msg, igs_remote_agent_t *remoteAgent, igs_agent_t *agent){
    assert(msg);
    assert(remoteAgent);
    assert(remoteAgent->context);
    assert(agent);
    
    if(remoteAgent->context->isFrozen == true){
        igsAgent_debug(agent, "Message received from %s for %s but all traffic in our process is currently frozen",
                       remoteAgent->name,
                       agent->name);
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
        igs_mapping_element_t *elmt, *tmp;
        HASH_ITER(hh, agent->mapping->map_elements, elmt, tmp) {
            if (strcmp(elmt->agent_name, remoteAgent->name) == 0
                && strcmp(elmt->output_name, output) == 0){
                //we have a match on emitting agent name and its ouput name :
                //still need to check the targeted input existence in our definition
                igs_iop_t *foundInput = NULL;
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

//manage incoming messages from one of the remote agents agents we subscribed to
int manageRemoteAgent (zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop)
    igs_remote_agent_t *remoteAgent = (igs_remote_agent_t *)arg;
    assert(socket);
    assert(remoteAgent);
    
    zmsg_t *msg = zmsg_recv(socket);
    //FIXME: parse agent uuid from message
    igs_agent_t *agent = NULL;
    handleMessageFromRemoteAgent(msg, remoteAgent, agent);
    zmsg_destroy(&msg);
    return 0;
}

//manage messages received on the bus
int manageBusIncoming (zloop_t *loop, zsock_t *socket, void *arg){
    igs_core_context_t *context = (igs_core_context_t *)arg;
    assert(context);
    zyre_t *node = context->node;
    assert(node);
    
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
        igs_debug("->%s has entered the network with peer id %s and endpoint %s", name, peer, address);
        igs_zyre_peer_t *zyrePeer = NULL;
        HASH_FIND_STR(context->zyrePeers, peer, zyrePeer);
        if (zyrePeer == NULL){
            zyrePeer = calloc(1, sizeof(igs_zyre_peer_t));
            zyrePeer->peerId = strndup(peer, IGS_MAX_PEER_ID_LENGTH);
            HASH_ADD_STR(context->zyrePeers, peerId, zyrePeer);
            zyrePeer->name = strndup(name, IGS_MAX_AGENT_NAME_LENGTH);
            char *k;
            const char *v;
            zlist_t *keys = zhash_keys(headers);
            size_t s = zlist_size(keys);
            if (s > 0){
                igs_debug("Handling headers for agent %s", name);
            }
            while ((k = (char *)zlist_pop(keys))) {
                v = zyre_event_header (zyre_event,k);
                igs_debug("\t%s -> %s", k, v);
                
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
                            igs_error("Could not extract port from address %s", address);
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
                        if (strcmp(context->ipAddress, incomingIpAddress) == 0){
                            //same IP address : we can try to use ipc (or loopback on windows) instead of TCP
                            //or we can use inproc if both agents are in the same process
                            int pid = atoi(zyre_event_header(zyre_event, "pid"));
                            if (context->processId == pid){
                                //same ip address and same process : we can use inproc
                                inprocAddress = zyre_event_header(zyre_event, "inproc");
                                if (inprocAddress != NULL){
                                    useInproc = true;
                                    igs_debug("Use address %s to subscribe to %s", inprocAddress, name);
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
                                    igs_debug("Use address %s to subscribe to %s", ipcAddress, name);
                                }
                            }
                        }
                        *insert = ':';
                        //add port to the endpoint to compose it fully
                        strcat(endpointAddress, v);
                        igs_remote_agent_t *subscriber;
                        HASH_FIND_STR(agent->subscribers, peer, subscriber);
                        if (subscriber != NULL){
                            //we have a reconnection with same peerId
                            igs_debug("Peer id %s was connected before with agent name %s : reset and recreate subscription", peer, subscriber->name);
                            HASH_DEL(agent->subscribers, subscriber);
                            zloop_poller_end(context->loop , subscriber->pollItem);
                            zsock_destroy(&subscriber->subscriber);
                            free((char*)subscriber->name);
                            free((char*)subscriber->agentPeerId);
                            free(subscriber->pollItem);
                            free(subscriber->subscriber);
                            if (subscriber->definition != NULL){
                                definition_freeDefinition(subscriber->definition);
                                subscriber->definition = NULL;
                            }
                            subscriber->subscriber = NULL;
                            if (subscriber->timerId != -1){
                                zloop_timer_end(context->loop, subscriber->timerId);
                                subscriber->timerId = -1;
                            }
                            free(subscriber);
                            subscriber = NULL;
                        }
                        subscriber = calloc(1, sizeof(igs_remote_agent_t));
                        zagent->subscriber = subscriber;
                        subscriber->agent = agent;
                        subscriber->name = strdup(name);
                        subscriber->agentPeerId = strdup (peer);
                        if (context->allowInproc && useInproc){
                            subscriber->subscriber = zsock_new_sub(inprocAddress, NULL);
                            zsock_set_rcvhwm(subscriber->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (inproc)",subscriber->name, inprocAddress);
                        }else if (context->allowIpc && useIPC){
                            subscriber->subscriber = zsock_new_sub(ipcAddress, NULL);
                            zsock_set_rcvhwm(subscriber->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (ipc)",subscriber->name, ipcAddress);
                        }else{
                            subscriber->subscriber = zsock_new_sub(endpointAddress, NULL);
                            zsock_set_rcvhwm(subscriber->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (tcp)",subscriber->name, endpointAddress);
                        }
                        assert(subscriber->subscriber);
                        subscriber->definition = NULL;
                        subscriber->mappingsFilters = NULL;
                        subscriber->timerId = -1;
                        HASH_ADD_STR(agent->subscribers, agentPeerId, subscriber);
                        zloop_reader(loop, subscriber->subscriber, manageRemoteAgent, subscriber);
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                        context->licenseEnforcement->currentAgentsNb++;
                        //igs_license("%ld agents (adding %s)", agent->licenseEnforcement->currentAgentsNb, name);
                        if (context->licenseEnforcement->currentAgentsNb > context->license->platformNbAgents){
                            igs_license("Maximum number of allowed agents (%d) is exceeded : agent will stop", context->license->platformNbAgents);
                            igs_license_callback_t *el = NULL;
                            DL_FOREACH(context->licenseCallbacks, el){
                                el->callback_ptr(IGS_LICENSE_TOO_MANY_AGENTS, el->data);
                            }
                            free(k);
                            zlist_destroy(&keys);
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
        igs_debug("+%s has joined %s", name, group);
        if (streq(group, IGS_PRIVATE_CHANNEL)){
            //definition is sent to every newcomer on the channel (whether it is a ingescape agent or not)
            char * definitionStr = NULL;
            definitionStr = parser_export_definition(agent->definition);
            if (definitionStr != NULL){
                sendDefinitionToRemoteAgent(agent, peer, definitionStr);
                free(definitionStr);
            }
            //and so is our mapping
            char *mappingStr = NULL;
            mappingStr = parser_export_mapping(agent->mapping);
            if (mappingStr != NULL){
                sendMappingToRemoteAgent(agent, peer, mappingStr);
                free(mappingStr);
            }else{
                sendMappingToRemoteAgent(agent, peer, "");
            }
            //we also send our frozen and muted states, and other usefull information
            if (agent->isWholeAgentMuted){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "MUTED=1");
                bus_zyreUnlock();
            }
            if (agent->isFrozen){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "FROZEN=1");
                bus_zyreUnlock();
            }
            if (strlen(agent->agentState) > 0){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "STATE=%s", agent->agentState);
                bus_zyreUnlock();
            }
            if (agent->definition != NULL){
                igs_iop_t *current_iop, *tmp_iop;
                HASH_ITER(hh, agent->definition->outputs_table, current_iop, tmp_iop) {
                    if (current_iop->is_muted && current_iop->name != NULL){
                        bus_zyreLock();
                        zyre_whispers(context->node, peer, "OUTPUT_MUTED %s", current_iop->name);
                        bus_zyreUnlock();
                    }
                }
            }
            if (context->logInStream){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "LOG_IN_STREAM=1");
                bus_zyreUnlock();
            }
            if (context->logInFile){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "LOG_IN_FILE=1");
                zyre_whispers(context->node, peer, "LOG_FILE_PATH=%s", context->logFilePath);
                bus_zyreUnlock();
            }
            if (strlen(agent->definitionPath) > 0){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "DEFINITION_FILE_PATH=%s", agent->definitionPath);
                bus_zyreUnlock();
            }
            if (strlen(agent->mappingPath) > 0){
                bus_zyreLock();
                zyre_whispers(context->node, peer, "MAPPING_FILE_PATH=%s", agent->mappingPath);
                bus_zyreUnlock();
            }
            
            igs_zyre_peer_t *zagent = NULL;
            HASH_FIND_STR(context->zyrePeers, peer, zagent);
            if (zagent != NULL){
                zagent->hasJoinedPrivateChannel = true;
            }
        }
    } else if (streq (event, "LEAVE")){
        igs_debug("-%s has left %s", name, group);
    } else if (streq (event, "SHOUT")){
        if (strcmp(group, context->replayChannel) == 0){
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
                    igs_debug("replaying %s (%s)", input, value);
                    igsAgent_writeInputAsString(agent, input, value);
                    if (value != NULL){
                        free(value);
                    }
                }else{
                    igs_debug("replaying %s", input);
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
            igs_remote_agent_t *subscriber;
            HASH_FIND_STR(agent->subscribers, peer, subscriber);
            
            if (newDefinition != NULL && newDefinition->name != NULL && subscriber != NULL){
                // Look if this agent already has a definition
                if(subscriber->definition != NULL) {
                    igs_debug("Definition already exists for agent %s : new definition will overwrite the previous one...", name);
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                    //we remove IOP count from previous definition
                    context->licenseEnforcement->currentIOPNb -= (HASH_COUNT(subscriber->definition->inputs_table) +
                                                                      HASH_COUNT(subscriber->definition->outputs_table) +
                                                                      HASH_COUNT(subscriber->definition->params_table));
                    //igs_license("%ld iops (cleaning %s)", agent->licenseEnforcement->currentIOPNb, name);
#endif
                    definition_freeDefinition(subscriber->definition);
                    subscriber->definition = NULL;
                }
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                context->licenseEnforcement->currentIOPNb += (HASH_COUNT(newDefinition->inputs_table) +
                                                                  HASH_COUNT(newDefinition->outputs_table) +
                                                                  HASH_COUNT(newDefinition->params_table));
                //igs_license("%ld iops (adding %s)", agent->licenseEnforcement->currentIOPNb, name);
                if (context->licenseEnforcement->currentIOPNb > context->license->platformNbIOPs){
                    igs_license("Maximum number of allowed IOPs (%d) is exceeded : agent will stop", context->license->platformNbIOPs);
                    igs_license_callback_t *el = NULL;
                    DL_FOREACH(context->licenseCallbacks, el){
                        el->callback_ptr(IGS_LICENSE_TOO_MANY_IOPS, el->data);
                    }
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return -1;
                }
#endif
                igs_debug("Store definition for agent %s", name);
                subscriber->definition = newDefinition;
                //Check the involvement of this new agent and its definition in our mapping and update subscriptions
                //we check here because subscriber definition is required to handle received data
                network_manageMappingsToRemoteAgent(agent, subscriber);
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
            igs_remote_agent_t *subscriber = NULL;
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
                igs_debug("Received mapping from agent %s is empty", name);
                if(subscriber != NULL && subscriber->mapping != NULL) {
                    mapping_freeMapping(subscriber->mapping);
                    subscriber->mapping = NULL;
                }
            }
            
            if (newMapping != NULL && subscriber != NULL){
                //look if this agent already has a mapping
                if(subscriber->mapping != NULL){
                    igs_debug("Mapping already exists for agent %s : new mapping will overwrite the previous one...", name);
                    mapping_freeMapping(subscriber->mapping);
                    subscriber->mapping = NULL;
                }
                
                igs_debug("Store mapping for agent %s", name);
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
            igs_remote_agent_t *s, *tmp;
            HASH_ITER(hh, agent->subscribers, s, tmp){
                network_manageMappingsToRemoteAgent(agent, s);
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
                igs_remote_agent_t *s, *tmp;
                HASH_ITER(hh, agent->subscribers, s, tmp){
                    network_manageMappingsToRemoteAgent(agent, s);
                }
                agent->network_needToUpdateMapping = true;
            }
            free(strMapping);
        }else{
            //other supported messages
            if (strlen("REQUEST_OUPUTS") == strlen(message) && strncmp (message, "REQUEST_OUPUTS", strlen("REQUEST_OUPUTS")) == 0){
                igs_debug("Responding to outputs request received from %s", name);
                //send all outputs via whisper to agent that mapped us
                long nbOutputs = 0;
                char **outputsList = NULL;
                outputsList = igsAgent_getOutputsList(agent, &nbOutputs);
                int i = 0;
                zmsg_t *omsg = zmsg_new();
                zmsg_addstr(omsg, "OUTPUTS");
                for (i = 0; i < nbOutputs; i++){
                    igs_iop_t * found_iop = model_findIopByName(agent, outputsList[i],IGS_OUTPUT_T);
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
                igs_remote_agent_t *foundSubscriber = NULL;
                HASH_FIND_STR(agent->subscribers, peer, foundSubscriber);
                if (foundSubscriber){
                    handleMessageFromRemoteAgent(agent, msgDuplicate, foundSubscriber);
                    igs_debug("privately received output values from %s (%s)", name, peer);
                }else{
                    igsAgent_error(agent, "could not find subscriber for peer %s (%s)", name, peer);
                }
                
            }else if (strlen("GET_CURRENT_INPUTS") == strlen(message) && strncmp (message, "GET_CURRENT_INPUTS", strlen("GET_CURRENT_INPUTS")) == 0){
                zmsg_t *resp = zmsg_new();
                igs_iop_t *inputs = agent->definition->inputs_table;
                igs_iop_t *current = NULL;
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
                igs_debug("send input values to %s", peer);
            }else if (strlen("GET_CURRENT_PARAMETERS") == strlen(message) && strncmp (message, "GET_CURRENT_PARAMETERS", strlen("GET_CURRENT_PARAMETERS")) == 0){
                zmsg_t *resp = zmsg_new();
                igs_iop_t *inputs = agent->definition->params_table;
                igs_iop_t *current = NULL;
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
                igs_debug("send parameters values to %s", peer);
            }else if (strlen("LICENSE_INFO") == strlen(message) && strncmp (message, "LICENSE_INFO", strlen("LICENSE_INFO")) == 0){
#if !TARGET_OS_IOS
                zmsg_t *resp = zmsg_new();
                if (context->license == NULL){
                    zmsg_addstr(resp, "no license available");
                }else{
                    char buf[128] = "";
                    struct tm ts;
                    zmsg_addstr(resp, "customer");
                    zmsg_addstr(resp, context->license->customer);
                    zmsg_addstr(resp, "order");
                    zmsg_addstr(resp, context->license->order);
                    zmsg_addstr(resp, "platformNbAgents");
                    zmsg_addstrf(resp, "%d", context->license->platformNbAgents);
                    zmsg_addstr(resp, "platformNbIOPs");
                    zmsg_addstrf(resp, "%d", context->license->platformNbIOPs);
                    zmsg_addstr(resp, "licenseExpirationDate");
                    ts = *localtime(&context->license->licenseExpirationDate);
                    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
                    zmsg_addstr(resp, buf);
                    zmsg_addstr(resp, "editorOwner");
                    zmsg_addstr(resp, context->license->editorOwner);
                    zmsg_addstr(resp, "editorExpirationDate");
                    ts = *localtime(&context->license->editorExpirationDate);
                    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
                    zmsg_addstr(resp, buf);
                    char *feature = zhash_first(context->license->features);
                    while (feature != NULL) {
                        zmsg_addstr(resp, "feature");
                        zmsg_addstr(resp, feature);
                        feature = zhash_next(context->license->features);
                    }
                    igs_license_for_agent_t *licForAgent = zhash_first(context->license->agents);
                    while (licForAgent != NULL) {
                        zmsg_addstr(resp, "agent");
                        zmsg_addstr(resp, licForAgent->agentId);
                        zmsg_addstr(resp, licForAgent->name);
                        licForAgent = zhash_next(context->license->agents);
                    }
                }
                bus_zyreLock();
                zyre_whisper(node, peer, &resp);
                bus_zyreUnlock();
                igs_debug("send license information to %s", peer);
#endif
            }else if (strlen("STOP") == strlen(message) && strncmp (message, "STOP", strlen("STOP")) == 0){
                context->forcedStop = true;
                igs_debug("received STOP command from %s (%s)", name, peer);
                free(message);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                //stop our zyre loop by returning -1 : this will start the cleaning process
                return -1;
            }else if (strlen("CLEAR_MAPPING") == strlen(message) && strncmp (message, "CLEAR_MAPPING", strlen("CLEAR_MAPPING")) == 0){
                igs_debug("received CLEAR_MAPPING command from %s (%s)", name, peer);
                igsAgent_clearMapping(agent);
            }else if (strlen("FREEZE") == strlen(message) && strncmp (message, "FREEZE", strlen("FREEZE")) == 0){
                igs_debug("received FREEZE command from %s (%s)", name, peer);
                igsAgent_freeze(agent);
            }else if (strlen("UNFREEZE") == strlen(message) && strncmp (message, "UNFREEZE", strlen("UNFREEZE")) == 0){
                igs_debug("received UNFREEZE command from %s (%s)", name, peer);
                igsAgent_unfreeze(agent);
            }else if (strlen("MUTE_ALL") == strlen(message) && strncmp (message, "MUTE_ALL", strlen("MUTE_ALL")) == 0){
                igs_debug("received MUTE_ALL command from %s (%s)", name, peer);
                igsAgent_mute(agent);
            }else if (strlen("UNMUTE_ALL") == strlen(message) && strncmp (message, "UNMUTE_ALL", strlen("UNMUTE_ALL")) == 0){
                igs_debug("received UNMUTE_ALL command from %s (%s)", name, peer);
                igsAgent_unmute(agent);
            }else if ((strncmp (message, "MUTE ", strlen("MUTE ")) == 0) && (strlen(message) > strlen("MUTE ")+1)){
                igs_debug("received MUTE command from %s (%s)", name, peer);
                char *subStr = message + strlen("MUTE") + 1;
                igsAgent_muteOutput(agent, subStr);
            }else if ((strncmp (message, "UNMUTE ", strlen("UNMUTE ")) == 0) && (strlen(message) > strlen("UNMUTE ")+1)){
                igs_debug("received UNMUTE command from %s (%s)", name, peer);
                char *subStr = message + strlen("UNMUTE") + 1;
                igsAgent_unmuteOutput(agent,subStr);
            }else if ((strncmp (message, "SET_INPUT ", strlen("SET_INPUT ")) == 0) && (strlen(message) > strlen("SET_INPUT ")+1)){
                igs_debug("received SET_INPUT command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_INPUT") + 1;
                char *_name, *value;
                _name = strtok (subStr," ");
                value = strtok (NULL,"\0");
                if (_name != NULL && value != NULL){
                    igsAgent_writeInputAsString(agent, _name, value);//last parameter is used for DATA only
                }
            }else if ((strncmp (message, "SET_OUTPUT ", strlen("SET_OUTPUT ")) == 0) && (strlen(message) > strlen("SET_OUTPUT ")+1)){
                igs_debug("received SET_OUTPUT command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_OUTPUT") + 1;
                char *_name, *value;
                _name = strtok (subStr," ");
                value = strtok (NULL,"\0");
                if (_name != NULL && value != NULL){
                    igsAgent_writeOutputAsString(agent, _name, value);//last paramter is used for DATA only
                }
            }else if ((strncmp (message, "SET_PARAMETER ", strlen("SET_PARAMETER ")) == 0) && (strlen(message) > strlen("SET_PARAMETER ")+1)){
                igs_debug("received SET_PARAMETER command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_PARAMETER") + 1;
                char *_name, *value;
                _name = strtok (subStr," ");
                value = strtok (NULL,"\0");
                if (_name != NULL && value != NULL){
                    igsAgent_writeParameterAsString(agent, _name, value);//last paramter is used for DATA only
                }
            }else if ((strncmp (message, "MAP ", strlen("MAP ")) == 0) && (strlen(message) > strlen("MAP ")+1)){
                igs_debug("received MAP command from %s (%s)", name, peer);
                char *subStr = message + strlen("MAP") + 1;
                char *input, *agt, *output;
                input = strtok (subStr," ");
                agt = strtok (NULL," ");
                output = strtok (NULL," ");
                if (input != NULL && agent != NULL && output != NULL){
                    igsAgent_addMappingEntry(agent, input, agt, output);
                }
            }else if ((strncmp (message, "UNMAP ", strlen("UNMAP ")) == 0) && (strlen(message) > strlen("UNMAP ")+1)){
                igs_debug("received UNMAP command from %s (%s)", name, peer);
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
                igs_debug("received ENABLE_LOG_STREAM command from %s (%s)", name, peer);
                igs_setLogStream(true);
            }
            else if (strlen("DISABLE_LOG_STREAM") == strlen(message) && strncmp (message, "DISABLE_LOG_STREAM", strlen("DISABLE_LOG_STREAM")) == 0){
                igs_debug("received DISABLE_LOG_STREAM command from %s (%s)", name, peer);
                igs_setLogStream(false);
            }
            else if (strlen("ENABLE_LOG_FILE") == strlen(message) && strncmp (message, "ENABLE_LOG_FILE", strlen("ENABLE_LOG_FILE")) == 0){
                igs_debug("received ENABLE_LOG_FILE command from %s (%s)", name, peer);
                igs_setLogInFile(true);
            }
            else if (strlen("DISABLE_LOG_FILE") == strlen(message) && strncmp (message, "DISABLE_LOG_FILE", strlen("DISABLE_LOG_FILE")) == 0){
                igs_debug("received DISABLE_LOG_FILE command from %s (%s)", name, peer);
                igs_setLogInFile(false);
            }
            else if ((strncmp (message, "SET_LOG_PATH ", strlen("SET_LOG_PATH ")) == 0) && (strlen(message) > strlen("SET_LOG_PATH ")+1)){
                igs_debug("received SET_LOG_PATH command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_LOG_PATH") + 1;
                igs_setLogPath(subStr);
            }
            else if ((strncmp (message, "SET_DEFINITION_PATH ", strlen("SET_DEFINITION_PATH ")) == 0)
                     && (strlen(message) > strlen("SET_DEFINITION_PATH ")+1)){
                igs_debug("received SET_DEFINITION_PATH command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_DEFINITION_PATH") + 1;
                igsAgent_setDefinitionPath(agent, subStr);
            }
            else if ((strncmp (message, "SET_MAPPING_PATH ", strlen("SET_MAPPING_PATH ")) == 0)
                     && (strlen(message) > strlen("SET_MAPPING_PATH ")+1)){
                igs_debug("received SET_MAPPING_PATH command from %s (%s)", name, peer);
                char *subStr = message + strlen("SET_MAPPING_PATH") + 1;
                igsAgent_setMappingPath(agent, subStr);
            }
            else if (strlen("SAVE_DEFINITION_TO_PATH") == strlen(message) && strncmp (message, "SAVE_DEFINITION_TO_PATH", strlen("SAVE_DEFINITION_TO_PATH")) == 0){
                igs_debug("received SAVE_DEFINITION_TO_PATH command from %s (%s)", name, peer);
                igsAgent_writeDefinitionToPath(agent);
            }
            else if (strlen("SAVE_MAPPING_TO_PATH") == strlen(message) && strncmp (message, "SAVE_MAPPING_TO_PATH", strlen("SAVE_MAPPING_TO_PATH")) == 0){
                igs_debug("received SAVE_MAPPING_TO_PATH command from %s (%s)", name, peer);
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
                            zyre_shouts(context->node, context->callsChannel, "%s from %s (%s)", callName, name, peer);
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
                if (count != context->performanceMsgCounter){
                    igsAgent_error(agent, "pong message lost at index %zu from %s", count, peer);
                } else if (count == context->performanceMsgCountTarget){
                    //last message received
                    context->performanceStop = zclock_usecs();
                    igsAgent_info(agent, "message size: %zu bytes", context->performanceMsgSize);
                    igsAgent_info(agent, "roundtrip count: %zu", context->performanceMsgCountTarget);
                    igsAgent_info(agent, "average latency: %.3f µs", ((double) context->performanceStop - (double) context->performanceStart) / context->performanceMsgCountTarget);
                    double throughput = (size_t) ((double) context->performanceMsgCountTarget / ((double) context->performanceStop - (double) context->performanceStart) * 1000000);
                    double megabytes = (double) throughput * context->performanceMsgSize / (1024*1024);
                    igsAgent_info(agent, "average roundtrip throughput: %d msg/s", (int)throughput);
                    igsAgent_info(agent, "average roundtrip throughput: %.3f MB/s", megabytes);
                    context->performanceMsgCountTarget = 0;
                } else {
                    context->performanceMsgCounter++;
                    zmsg_t *back = zmsg_new();
                    zmsg_addstr(back, "PING");
                    zmsg_addmem(back, &context->performanceMsgCounter, sizeof(size_t));
                    zmsg_append(back, &payload);
                    bus_zyreLock();
                    zyre_whisper(node, peer, &back);
                    bus_zyreUnlock();
                }
                
            }
        }
        free(message);
    } else if (streq (event, "EXIT")){
        igs_debug("<-%s (%s) exited", name, peer);
        igs_zyre_peer_t *a = NULL;
        HASH_FIND_STR(context->zyrePeers, peer, a);
        if (a != NULL){
            if (a->reconnected > 0){
                //do not clean: we are getting a timemout now whereas
                //the agent is reconnected
                a->reconnected--;
            }else{
                HASH_DEL(context->zyrePeers, a);
                free(a);
                // Try to find the subscriber to destory
                igs_remote_agent_t *subscriber = NULL;
                HASH_FIND_STR(agent->subscribers, peer, subscriber);
                if (subscriber != NULL){
                    network_cleanAndFreeRemoteAgent(agent, subscriber);
                }
            }
        }
    }
    
    //handle callbacks
    igs_zyre_callback_t *elt;
    DL_FOREACH(context->zyreCallbacks,elt){
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
        
    return 0;
}

//Timer callback to (re)send our definition to agents present on the private channel
int triggerDefinitionUpdate(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_core_context_t *context = (igs_core_context_t *)arg;

    if (agent->network_needToSendDefinitionUpdate){
        model_readWriteLock();
        char * definitionStr = NULL;
        definitionStr = parser_export_definition(agent->definition);
        if (definitionStr != NULL){
            igs_zyre_peer_t *a, *tmp;
            HASH_ITER(hh, coreContext->zyrePeers, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendDefinitionToRemoteAgent(agent, a->peerId, definitionStr);
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
    igs_agent_t *agent = (igs_agent_t *)arg;
    igs_license("Runtime duration limit has been reached : stopping Ingescape now");
    igs_license_callback_t *el = NULL;
    DL_FOREACH(coreContext->licenseCallbacks, el){
        el->callback_ptr(IGS_LICENSE_TIMEOUT, el->data);
    }
    return -1;
}

//Timer callback to update and (re)send our mapping to agents on the private channel
int triggerMappingUpdate(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_core_context_t *context = (igs_core_context_t *)arg;

    if (agent->network_needToUpdateMapping){
        char *mappingStr = NULL;
        mappingStr = parser_export_mapping(agent->mapping);
        if (mappingStr != NULL){
            igs_zyre_peer_t *a, *tmp;
            HASH_ITER(hh, coreContext->zyrePeers, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendMappingToRemoteAgent(agent, a->peerId, mappingStr);
                }
            }
            free(mappingStr);
        }
        igs_remote_agent_t *s, *tmp;
        HASH_ITER(hh, agent->subscribers, s, tmp){
            network_manageMappingsToRemoteAgent(agent, s);
        }
        agent->network_needToUpdateMapping = false;
    }
    return 0;
}

static void runLoop (zsock_t *mypipe, void *args){
    network_Lock();
    igs_core_context_t *context = (igs_core_context_t *)args;
    //start zyre now that everything is set
    bus_zyreLock();
    int zyreStartRes = zyre_start (coreContext->node);
    bus_zyreUnlock();
    if (zyreStartRes != IGS_SUCCESS){
        igs_error("Could not start bus node : Ingescape will interrupt immediately.");
        return;
    }
    
    context->loop = zloop_new ();
    assert (context->loop);
    zloop_set_verbose (context->loop, false);
    zloop_reader(context->loop, mypipe, manageParent, context);
    zloop_reader(context->loop, zyre_socket(context->node), manageBusIncoming, context);
    zloop_timer(context->loop, 1000, 0, triggerDefinitionUpdate, context);
    zloop_timer(context->loop, 1000, 0, triggerMappingUpdate, context);
    
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    if (context->license != NULL && !context->license->isLicenseValid){
        igs_license("License is not valid : starting timer for demonstration mode (%d seconds)...", MAX_EXEC_DURATION_DURING_EVAL);
        zloop_timer(context->loop, MAX_EXEC_DURATION_DURING_EVAL * 1000, 0, triggerLicenseStop, agent);
    }
#endif
    zsock_signal (mypipe, 0);
    network_Unlock();
    
    igsAgent_debug(agent, "loop starting");
    zloop_start (context->loop); //returns when one of the pollers returns -1
    
    network_Lock();
    igsAgent_debug(agent, "loop stopping...");
    //clean
    igs_zyre_peer_t *zagent, *tmpa;
    HASH_ITER(hh, context->zyrePeers, zagent, tmpa){
        HASH_DEL(context->zyrePeers, zagent);
        free(zagent);
    }
    igs_remote_agent_t *s, *tmps;
    HASH_ITER(hh, agent->subscribers, s, tmps) {
        network_cleanAndFreeRemoteAgent(agent, s);
    }
    bus_zyreLock();
    if (context != NULL)
        zyre_stop (context->node);
    bus_zyreUnlock();
    //zclock_sleep (100);
    if (context != NULL){
        zyre_destroy (&context->node);
        zsock_destroy(&context->publisher);
        if (context->ipcPublisher != NULL){
            zsock_destroy(&context->ipcPublisher);
#if defined __unix__ || defined __APPLE__ || defined __linux__
            if (context->network_ipcFullPath != NULL){
                zsys_file_delete(context->network_ipcFullPath); //destroy ipcPath in file system
                //NB: ipcPath is based on peer id which is unique. It will never be used again.
                free(context->network_ipcFullPath);
                context->network_ipcFullPath = NULL;
            }
#endif
        }
        if (context->inprocPublisher != NULL){
            zsock_destroy(&context->inprocPublisher);
        }
        if (context->logger != NULL){
            zsock_destroy(&context->logger);
        }
        
        zloop_destroy (&context->loop);
        
        igs_timer_t *current_timer, *tmp_timer;
        HASH_ITER(hh, context->timers, current_timer, tmp_timer){
            HASH_DEL(context->timers, current_timer);
            free(current_timer);
        }
        
        if (context->forcedStop){
            igs_forced_stop_calback_t *cb = NULL;
            DL_FOREACH(context->forcedStopCalbacks, cb){
                cb->callback_ptr(agent, cb->myData);
            }
            context->isInterrupted = true;
            //in case of forced stop, we send SIGINT to our process so
            //that it can be trapped by main thread for a proper stop
            #if defined __unix__ || defined __APPLE__ || defined __linux__
            igsAgent_debug(agent, "triggering SIGINT");
            kill(context->processId, SIGINT);
            #endif
            //FIXME : find a way to do that for windows as well
        }
    }
    if (context->network_ipcEndpoint)
        free(context->network_ipcEndpoint);
    context->network_ipcEndpoint = NULL;
    
    igsAgent_debug(agent, "loop stopped");
    network_Unlock();
}

void initLoop (igs_core_context_t *context){
    igs_debug("loop init");
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    if (context->licenseEnforcement != NULL){
        free(context->licenseEnforcement);
    }
    context->licenseEnforcement = calloc(1, sizeof(igs_license_enforcement_t));
    //iterate on all agents in this process under this context
    if (context->agents != NULL){
        igs_agent_t *el, *tmp;
        HASH_ITER(hh, context->agents, el, tmp){
            if (el->definition != NULL){
                context->licenseEnforcement->currentIOPNb += (HASH_COUNT(el->definition->inputs_table) +
                                                              HASH_COUNT(el->definition->outputs_table) +
                                                              HASH_COUNT(el->definition->params_table));
                //count our own presence and IOPs
                context->licenseEnforcement->currentAgentsNb++;
            }
        }
    }
    //NB: counting the number of IOPs here does not include future
    //definition modifications when the agent is running.
#endif
    
#if defined __unix__ || defined __APPLE__ || defined __linux__
    if (context->network_shallRaiseFileDescriptorsLimit){
        struct rlimit limit;
        if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
            igs_error("getrlimit() failed with errno=%d", errno);
        }else{
            rlim_t prevCur = limit.rlim_cur;
#ifdef __APPLE__
            limit.rlim_cur = MIN(OPEN_MAX, limit.rlim_max); //OPEN_MAX is the actual per process limit in macOS
#else
            limit.rlim_cur = limit.rlim_max;
#endif
            if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
              igs_error("setrlimit() failed with errno=%d", errno);
            }else{
                if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
                  igs_error("getrlimit() failed with errno=%d", errno);
                }else{
                    //adjust allowed number of sockets per process in ZeroMQ
                    zsys_set_max_sockets(0); //0 = use maximum value allowed by the OS
                    igs_debug("raised file descriptors limit from %llu to %llu", prevCur, limit.rlim_cur);
                    context->network_shallRaiseFileDescriptorsLimit = false;
                }
            }
        }
    }
#endif

    //FIXME: !
//    agent->network_needToSendDefinitionUpdate = false;
//    agent->network_needToUpdateMapping = false;

    bool canContinue = true;
    //prepare zyre
    bus_zyreLock();
    context->node = zyre_new(coreAgent->name);
    //zyre_set_verbose(context->node);
    bus_zyreUnlock();
    if (strlen(context->brokerEndPoint) > 0){
        bus_zyreLock();
        zyre_set_verbose(context->node);
        zyre_gossip_connect(context->node,
                            "%s", context->brokerEndPoint);
        bus_zyreUnlock();
    }else{
        if (context->node == NULL){
            igs_fatal("Could not create bus node : Ingescape will interrupt immediately.");
            return;
        }else{
            bus_zyreLock();
            zyre_set_interface(context->node, context->networkDevice);
            zyre_set_port(context->node, context->network_zyrePort);
            bus_zyreUnlock();
        }
    }
    bus_zyreLock();
    zyre_set_interval(context->node, context->network_discoveryInterval);
    zyre_set_expired_timeout(context->node, context->network_agentTimeout);
    bus_zyreUnlock();
    
    //join ingescape private channel
    bus_zyreLock();
    zyre_join(context->node, IGS_PRIVATE_CHANNEL);
    bus_zyreUnlock();
    
    //FIXME: !
//    //create channel for replay
//    snprintf(context->replayChannel, IGS_MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-REPLAY", agent->name);
//    bus_zyreLock();
//    zyre_join(context->node, context->replayChannel);
//    bus_zyreUnlock();
    
    //FIXME: !
//    //create channel for calls feedback
//    snprintf(context->callsChannel, IGS_MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-CALLS", agent->name);
//    bus_zyreLock();
//    zyre_join(context->node, context->callsChannel);
//    bus_zyreUnlock();
    
    //Add version and protocol to headers
    bus_zyreLock();
    zyre_set_header(context->node, "ingescape", "v%d.%d.%d", (int)igs_version()/10000, (int)(igs_version() %10000)/100, (int)(igs_version() %100));
    zyre_set_header(context->node, "protocol", "v%d", igs_protocol());
    bus_zyreUnlock();
    
    //Add stored headers to zyre
    igs_service_header_t *el, *tmp;
    bus_zyreLock();
    HASH_ITER(hh, context->serviceHeaders, el, tmp){
        zyre_set_header(context->node, el->key, "%s", el->value);
    }
    bus_zyreUnlock();
    
    //start TCP publisher
    char endpoint[512];
    if (context->network_publishingPort == 0){
        snprintf(endpoint, 511, "tcp://%s:*", context->ipAddress);
    }else{
        snprintf(endpoint, 511, "tcp://%s:%d", context->ipAddress, context->network_publishingPort);
    }
    context->publisher = zsock_new_pub(endpoint);
    if (context->publisher == NULL){
        igs_error("Could not create publishing socket (%s): Ingescape will interrupt immediately.", endpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(context->publisher, context->network_hwmValue);
        strncpy(endpoint, zsock_endpoint(context->publisher), 256);
        char *insert = endpoint + strlen(endpoint) - 1;
        while (*insert != ':' && insert > endpoint) {
            insert--;
        }
        bus_zyreLock();
        zyre_set_header(context->node, "publisher", "%s", insert + 1);
        bus_zyreUnlock();
    }

    //start ipc publisher
#if defined __unix__ || defined __APPLE__ || defined __linux__
    if (context->network_ipcFolderPath == NULL){
        context->network_ipcFolderPath = strdup(DEFAULT_IPC_PATH);
    }
    if (!zsys_file_exists(context->network_ipcFolderPath)){
        zsys_dir_create("%s", context->network_ipcFolderPath);
        if (!zsys_file_exists(context->network_ipcFolderPath)){
            igs_error("could not create ipc folder path '%s'", context->network_ipcFolderPath);
            canContinue = false;
        }
    }
    bus_zyreLock();
    context->network_ipcFullPath = calloc(1, strlen(context->network_ipcFolderPath)+strlen(zyre_uuid(context->node))+2);
    sprintf(context->network_ipcFullPath, "%s/%s", context->network_ipcFolderPath, zyre_uuid(context->node));
    context->network_ipcEndpoint = calloc(1, strlen(context->network_ipcFolderPath)+strlen(zyre_uuid(context->node))+8);
    sprintf(context->network_ipcEndpoint, "ipc://%s/%s", context->network_ipcFolderPath, zyre_uuid(context->node));
    bus_zyreUnlock();
    zsock_t *ipcPublisher = context->ipcPublisher = zsock_new_pub(context->network_ipcEndpoint);
    if (ipcPublisher == NULL){
        igs_error("Could not create IPC publishing socket '%s'", context->network_ipcEndpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(context->ipcPublisher, context->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(context->node, "ipc", "%s", context->network_ipcEndpoint);
        bus_zyreUnlock();
    }
#elif (defined WIN32 || defined _WIN32)
    context->network_ipcEndpoint = strdup("tcp://127.0.0.1:*");
    zsock_t *ipcPublisher = context->ipcPublisher = zsock_new_pub(context->network_ipcEndpoint);
    if (ipcPublisher == NULL){
        igs_error("Could not create loopback publishing socket (%s)", context->network_ipcEndpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(context->ipcPublisher, agent->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(context->node, "loopback", "%s", zsock_endpoint(ipcPublisher));
        bus_zyreUnlock();
    }
#endif
    
    //start inproc publisher
    bus_zyreLock();
    char *inprocEndpoint = calloc(1, sizeof(char) * (12 + strlen(zyre_uuid(context->node))));
    sprintf(inprocEndpoint, "inproc://%s", zyre_uuid(context->node));
    bus_zyreUnlock();
    context->inprocPublisher = zsock_new_pub(inprocEndpoint);
    if (context->inprocPublisher == NULL){
        igs_error("Could not create inproc publishing socket (%s)", inprocEndpoint);
        canContinue = false;
    }else{
        zsock_set_sndhwm(context->inprocPublisher, context->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(context->node, "inproc", "%s", inprocEndpoint);
        bus_zyreUnlock();
    }
    
    //start logger stream if needed
    if (context->logInStream){
        if (context->network_logStreamPort == 0){
            sprintf(endpoint, "tcp://%s:*", context->ipAddress);
        } else {
            sprintf(endpoint, "tcp://%s:%d", context->ipAddress, context->network_logStreamPort);
        }
        context->logger = zsock_new_pub(endpoint);
        if (context->logger == NULL){
            igs_error("Could not create log stream socket (%s): Ingescape will interrupt immediately.", endpoint);
            canContinue = false;
        } else {
            zsock_set_sndhwm(context->logger, context->network_hwmValue);
            strncpy(endpoint, zsock_endpoint(context->logger), 256);
            char *insert = endpoint + strlen(endpoint) - 1;
            while (*insert != ':' && insert > endpoint) {
                insert--;
            }
            bus_zyreLock();
            zyre_set_header(context->node, "logger", "%s", insert + 1);
            bus_zyreUnlock();
        }
    }
    
    //set other headers for agent
    bus_zyreLock();
    zyre_set_header(context->node, "canBeFrozen", "%i", context->canBeFrozen);
    bus_zyreUnlock();

#if defined __unix__ || defined __APPLE__ || defined __linux__
    ssize_t ret;
    context->processId = getpid();
    bus_zyreLock();
    zyre_set_header(context->node, "pid", "%i", context->processId);
    bus_zyreUnlock();
    
    if (strlen(context->commandLine) == 0){
        //command line was not set manually : we try to get exec path instead
#ifdef __APPLE__
#if TARGET_OS_IOS
        char pathbuf[64] = "no_path";
        ret = 1;
#elif TARGET_OS_OSX
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        ret = proc_pidpath (context->processId, pathbuf, sizeof(pathbuf));
#endif
#else
        char pathbuf[4*1024];
        memset(pathbuf, 0, 4*1024);
        ret = readlink("/proc/self/exe", pathbuf, sizeof(pathbuf));
#endif
        if ( ret <= 0 ) {
            igs_error("PID %d: proc_pidpath () - %s", context->processId, strerror(errno));
        } else {
            igs_error("proc %d: %s", context->processId, pathbuf);
        }
        bus_zyreLock();
        zyre_set_header(context->node, "commandline", "%s", pathbuf);
        bus_zyreUnlock();
    }else{
        bus_zyreLock();
        zyre_set_header(context->node, "commandline", "%s", context->commandLine);
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
        zyre_set_header(context->node, "commandline", "%s", exeFilePath);
        bus_zyreUnlock();
    }else{
        bus_zyreLock();
        zyre_set_header(context->node, "commandline", "%s", agent->commandLine);
        bus_zyreUnlock();
    }
    DWORD pid = GetCurrentProcessId();
    context->processId = (int)pid;
    bus_zyreLock();
    zyre_set_header(context->node, "pid", "%i", (int)pid);
    bus_zyreUnlock();
#endif


    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    #if (defined WIN32 || defined _WIN32)
        WSACleanup();
    #endif
    bus_zyreLock();
    zyre_set_header(context->node, "hostname", "%s", hostname);
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
    
    if (canContinue){
        context->networkActor = zactor_new(runLoop, context);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

int network_publishOutput (igs_agent_t *agent, const igs_iop_t *iop){
    int result = 0;
    if (iop == NULL){
        igsAgent_warn(agent, "passed IOP is NULL");
        return 0;
    }
    
    model_readWriteLock();
    if(coreContext != NULL && coreContext->publisher != NULL && iop != NULL)
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
            if (zmsg_send(&msg, coreContext->publisher) != 0){
                igsAgent_error(agent, "Could not publish output %s on the network\n",iop->name);
                zmsg_destroy(&msgBis);
            }else{
                result = 1;
                zmsg_t *msgTer = zmsg_dup(msgBis);
                if (coreContext->ipcPublisher != NULL){
                    //publisher can be NULL on IOS or for read/write problems with assigned IPC path
                    //in both cases, an error message has been issued at start
                    if (zmsg_send(&msgBis, coreContext->ipcPublisher) != 0){
                        igsAgent_error(agent, "Could not publish output %s using IPC\n",iop->name);
                        zmsg_destroy(&msgBis);
                    }
                }
                if (coreContext->inprocPublisher != NULL){
                    if (zmsg_send(&msgTer, coreContext->inprocPublisher) != 0){
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
    }else{
        igsAgent_warn(agent, "agent not started : could not publish output %s", iop->name);
    }
    model_readWriteUnlock();
    return result;
}

int network_timerCallback (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_timer_t *timer = (igs_timer_t *)arg;
    timer->cb(timer->timerId, timer->myData);
    return 1;
}

int igsAgent_observeBus(igs_agent_t *agent, igsAgent_BusMessageIncoming cb, void *myData){
    if (cb != NULL){
        igs_zyre_callback_t *newCb = calloc(1, sizeof(igs_zyre_callback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(coreContext->zyreCallbacks, newCb);
    }else{
        igsAgent_error(agent, "callback is null");
        return 0;
    }
    return 1;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

int igs_startWithDevice(const char *networkDevice, unsigned int port){
    core_initContext();
    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igs_error("networkDevice cannot be NULL or empty");
        return IGS_FAILURE;
    }
    
    if (coreContext->networkActor != NULL){
        //Agent is active : need to stop it first
        igs_stop();
    }
    
    strncpy(coreContext->networkDevice, networkDevice, IGS_NETWORK_DEVICE_LENGTH-1);
    coreContext->brokerEndPoint[0] = '\0';
    coreContext->ipAddress[0] = '\0';
    
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
            strncpy(coreContext->ipAddress, ziflist_address (iflist), IGS_IP_ADDRESS_LENGTH-1);
            igs_info("Starting with ip address %s and port %d on device %s", coreContext->ipAddress, port, networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(coreContext->ipAddress) == 0){
        igs_error("IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        return IGS_FAILURE;
    }
#if !TARGET_OS_IOS
    license_readLicense(coreContext);
#endif
    coreContext->network_zyrePort = port;
    initLoop(coreContext);
    assert (coreContext->networkActor);
    return IGS_SUCCESS;
}

int igs_startWithIP(const char *ipAddress, unsigned int port){
    if ((ipAddress == NULL) || (strlen(ipAddress) == 0)){
        igs_error("IP address cannot be NULL or empty");
        return 0;
    }
    
    if (coreContext != NULL){
        //Agent is already active : need to stop it first
        igs_stop();
    }
    coreContext->isInterrupted = false;
    coreContext->forcedStop = false;
    coreContext = calloc(1, sizeof(struct igs_core_context));
    coreContext->brokerEndPoint[0] = '\0';
    strncpy(coreContext->ipAddress, ipAddress, IGS_IP_ADDRESS_LENGTH-1);
    
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
            strncpy(coreContext->networkDevice, name, 15);
            igs_info("Starting with ip address %s and port %d on device %s", ipAddress, port, coreContext->networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(coreContext->networkDevice) == 0){
        igs_error("Device name could not be determined for IP address %s : our agent will NOT start", ipAddress);
        coreContext = NULL;
        free(coreContext);
        return 0;
    }
#if !TARGET_OS_IOS
    license_readLicense();
#endif
    coreContext->network_zyrePort = port;
    initLoop(coreContext);
    assert (coreContext->networkActor);
    return 1;
}

int igsAgent_startWithDeviceOnBroker(igs_agent_t *agent, const char *networkDevice, const char *brokerEndpoint){
    //TODO: manage a list of brokers instead of just one
    if ((brokerEndpoint == NULL) || (strlen(brokerEndpoint) == 0)){
        igsAgent_error(agent, "brokerIpAddress cannot be NULL or empty");
        return 0;
    }
    if ((networkDevice == NULL) || (strlen(networkDevice) == 0)){
        igsAgent_error(agent, "networkDevice cannot be NULL or empty");
        return 0;
    }
    
    if (coreContext != NULL){
        igs_stop();
    }
    coreContext->isInterrupted = false;
    coreContext->forcedStop = false;
    
    coreContext = calloc(1, sizeof(struct igs_core_context));
    strncpy(coreContext->brokerEndPoint, brokerEndpoint, IGS_IP_ADDRESS_LENGTH-1);
    strncpy(coreContext->networkDevice, networkDevice, IGS_NETWORK_DEVICE_LENGTH-1);
    coreContext->ipAddress[0] = '\0';
    
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
            strncpy(coreContext->ipAddress, ziflist_address (iflist), IGS_IP_ADDRESS_LENGTH-1);
            igsAgent_info(agent, "Starting with ip address %s on device %s with broker %s",
                      coreContext->ipAddress,
                      networkDevice,
                      coreContext->brokerEndPoint);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(coreContext->ipAddress) == 0){
        igsAgent_error(agent, "IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        free(coreContext);
        coreContext = NULL;
        return 0;
    }
#if !TARGET_OS_IOS
    license_readLicense();
#endif
    coreContext->network_zyrePort = 0;
    initLoop(agent);
    assert (coreContext->networkActor);
    return 1;
}


int igs_stop(){
    if (coreContext != NULL){
        //interrupting and destroying ingescape thread and zyre layer
        //this will also clean all agent->subscribers
        if (coreContext->networkActor != NULL){
            if (!coreContext->forcedStop){
                //NB: if agent has been forcibly stopped, actor is already stopping
                //and this command would deadlock.
                zstr_sendx (coreContext->networkActor, "$TERM", NULL);
            }
            //This function will block until runLoop has terminated
            zactor_destroy(&coreContext->networkActor);
        }
        free(coreContext);
        coreContext = NULL;
        //igsAgent_debug(agent, "still %d agents running in process", igs_nbOfAgentsInProcess);
#if (defined WIN32 || defined _WIN32)
        // On Windows, if we don't call zsys_shutdown, the application will crash on exit
        // (WSASTARTUP assertion failure)
        // NB: Monitoring also uses a zactor, we can not call zsys_shutdown() when it is running
        if (igs_nbOfAgentsInProcess == 0) {
            igsAgent_debug(agent, "calling zsys_shutdown after last agent in process has stopped");
            zsys_shutdown();
        }
#endif
        igs_info("agent stopped");
    }else{
        igs_debug("agent already stopped");
    }
#if !TARGET_OS_IOS
    license_cleanLicense();
#endif
    return 1;
}

bool igs_isStarted(){
    if (coreContext != NULL
        && coreContext->loop != NULL){
        return true;
    }else{
        return false;
    }
}

int igsAgent_setAgentName(igs_agent_t *agent, const char *name){
    if ((name == NULL) || (strlen(name) == 0)){
        igsAgent_error(agent, "Agent name cannot be NULL or empty");
        return 0;
    }
    if (strcmp(agent->name, name) == 0){
        //nothing to do
        return 1;
    }
    char networkDevice[IGS_NETWORK_DEVICE_LENGTH] = "";
    char ipAddress[IGS_IP_ADDRESS_LENGTH] = "";
    int zyrePort = 0;
    bool needRestart = false;
    if (coreContext != NULL){
        //Agent is already started, zyre actor needs to be recreated
        strncpy(networkDevice, coreContext->networkDevice, IGS_NETWORK_DEVICE_LENGTH);
        strncpy(ipAddress, coreContext->ipAddress, IGS_IP_ADDRESS_LENGTH);
        zyrePort = coreContext->network_zyrePort;
        igs_stop();
        needRestart = true;
    }
    char *n = strndup(name, IGS_MAX_AGENT_NAME_LENGTH);
    if (strlen(name) > IGS_MAX_AGENT_NAME_LENGTH){
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
    strncpy(agent->name, n, IGS_MAX_AGENT_NAME_LENGTH-1);
    free(n);
    
    if (needRestart){
        igs_startWithIP(ipAddress, zyrePort);
    }
    igsAgent_debug(agent, "Agent name is %s", agent->name);
    return 1;
}


char *igsAgent_getAgentName(igs_agent_t *agent){
    return strdup(agent->name);
}


int igsAgent_freeze(igs_agent_t *agent){
    if (!agent->canBeFrozen){
        igsAgent_error(agent, "agent is requested to be frozen but is still set to 'can't be Frozen' : call igs_setCanBeFrozen to change this");
        return 0;
    }
    if(agent->isFrozen == false)
    {
        igsAgent_debug(agent, "Agent frozen");
        if ((coreContext != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "FROZEN=1");
            bus_zyreUnlock();
        }
        agent->isFrozen = true;
        igs_freeze_callback_t *elt;
        DL_FOREACH(agent->freezeCallbacks,elt){
            elt->callback_ptr(agent->isFrozen, elt->myData);
        }
    }
    return 1;
}


bool igsAgent_isFrozen(igs_agent_t *agent){
    return agent->isFrozen;
}


int igsAgent_unfreeze(igs_agent_t *agent){
    if(agent->isFrozen == true)
    {
        igsAgent_debug(agent, "Agent resumed (unfrozen)");
        if ((coreContext != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "FROZEN=0");
            bus_zyreUnlock();
        }
        agent->isFrozen = false;
        igs_freeze_callback_t *elt;
        DL_FOREACH(agent->freezeCallbacks,elt){
            elt->callback_ptr(agent->isFrozen, elt->myData);
        }
    }
    return 1;
}


int igsAgent_observeFreeze(igs_agent_t *agent, igsAgent_freezeCallback cb, void *myData){
    if (cb != NULL){
        igs_freeze_callback_t *newCb = calloc(1, sizeof(igs_freeze_callback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(agent->freezeCallbacks, newCb);
    }else{
        igsAgent_warn(agent, "callback is null");
        return 0;
    }
    return 1;
}


int igsAgent_setAgentState(igs_agent_t *agent, const char *state){
    if (state == NULL){
        igsAgent_error(agent, "state can not be NULL");
        return 0;
    }
    
    if (strcmp(state, agent->agentState) != 0){
        strncpy(agent->agentState, state, IGS_MAX_AGENT_NAME_LENGTH-1);
        igsAgent_debug(agent, "changed to %s", agent->agentState);
        if (coreContext != NULL && coreContext->node != NULL){
            bus_zyreLock();
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "STATE=%s", agent->agentState);
            bus_zyreUnlock();
        }
    }
    return 1;
}


char *igsAgent_getAgentState(igs_agent_t *agent){
    return strdup(agent->agentState);
}


void igsAgent_setCanBeFrozen(igs_agent_t *agent, bool canBeFrozen){
    agent->canBeFrozen = canBeFrozen;
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        //update header for information to agents not arrived yet
        zyre_set_header(coreContext->node, "canBeFrozen", "%i", agent->canBeFrozen);
        //send real time notification for agents already there
        zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "CANBEFROZEN=%i", canBeFrozen);
        bus_zyreUnlock();
        igsAgent_debug(agent, "changed to %d", canBeFrozen);
    }
}


bool igsAgent_canBeFrozen (igs_agent_t *agent){
    return agent->canBeFrozen;
}


int igsAgent_mute(igs_agent_t *agent){
    if (!agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = true;
        if ((coreContext != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "MUTED=%i", agent->isWholeAgentMuted);
            bus_zyreUnlock();
        }
        igs_mute_callback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
    return 1;
}


int igsAgent_unmute(igs_agent_t *agent){
    if (agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = false;
        if ((coreContext != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            zyre_shouts(coreContext->node, IGS_PRIVATE_CHANNEL, "MUTED=%i", agent->isWholeAgentMuted);
            bus_zyreUnlock();
        }
        igs_mute_callback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
    return 1;
}


bool igsAgent_isMuted(igs_agent_t *agent){
    return agent->isWholeAgentMuted;
}


int igsAgent_observeMute(igs_agent_t *agent, igsAgent_muteCallback cb, void *myData){
    if (cb != NULL){
        igs_mute_callback_t *newCb = calloc(1, sizeof(igs_mute_callback_t));
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


void igs_die(){
    coreContext->forcedStop = true;
    igs_stop();
}

void igs_setCommandLine(const char *line){
    strncpy(coreContext->commandLine, line, IGS_COMMAND_LINE_LENGTH-1);
    igs_debug("Command line set to %s", coreContext->commandLine);
}

void igs_setCommandLineFromArgs(int argc, const char * argv[]){
    if (argc < 1 || argv == NULL || argv[0] == NULL){
        igs_error("passed args must at least contain one element");
        return;
    }
    char cmd[IGS_COMMAND_LINE_LENGTH] = "";
    
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
        igs_error("PID %d: proc_pidpath () - %s", pid, strerror(errno));
        return;
    } else {
        igs_debug("proc %d: %s", pid, pathbuf);
    }
    if (strlen(pathbuf) < IGS_COMMAND_LINE_LENGTH){
        strcat(cmd, pathbuf);
    }else{
        igs_error("path is too long: %s", pathbuf);
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
        if (strlen(cmd) + strlen(argv[i]) + 2 > IGS_COMMAND_LINE_LENGTH){ // 2 is for space and EOL
            igs_error("passed arguments exceed buffer size: concatenation will stop here with '%s'", cmd);
            break;
        }else{
            strcat(cmd, " ");
        }
        strcat(cmd, argv[i]);
    }
    strncpy(coreContext->commandLine, cmd, IGS_COMMAND_LINE_LENGTH);
    igs_debug("Command line set to %s", coreContext->commandLine);
}

void igsAgent_setRequestOutputsFromMappedAgents(igs_agent_t *agent, bool notify){
    agent->network_RequestOutputsFromMappedAgents = notify;
    igsAgent_debug(agent, "changed to %d", notify);
}

bool igsAgent_getRequestOutputsFromMappedAgents(igs_agent_t *agent){
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
            (*devices)[currentDeviceNb] = calloc(IGS_NETWORK_DEVICE_LENGTH+1, sizeof(char));
            strncpy((*devices)[currentDeviceNb], name, IGS_NETWORK_DEVICE_LENGTH);
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

void igsAgent_observeForcedStop(igs_agent_t *agent, igsAgent_forcedStopCallback cb, void *myData){
    if (cb != NULL){
        igs_forced_stop_calback_t *newCb = calloc(1, sizeof(igs_forced_stop_calback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(coreContext->forcedStopCalbacks, newCb);
    }else{
        igsAgent_warn(agent, "callback is null");
    }
}

void igs_setDiscoveryInterval(unsigned int interval){
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_set_interval(coreContext->node, interval);
        bus_zyreUnlock();
    }
    coreContext->network_discoveryInterval = interval;
}

void igs_setAgentTimeout(unsigned int duration){
    if (coreContext != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_set_expired_timeout(coreContext->node, duration);
        bus_zyreUnlock();
    }
    coreContext->network_agentTimeout = duration;
}

void igs_setPublishingPort(unsigned int port){
    if (coreContext != NULL && coreContext->publisher != NULL){
        igs_error("agent is already started : stop it first to change its publishing port");
        return;
    }
    coreContext->network_publishingPort = port;
}

void igs_setLogStreamPort(unsigned int port){
    if (coreContext != NULL && coreContext->logger != NULL){
        igs_error("agent is already started : stop it first to change its logger port");
        return;
    }
    coreContext->network_logStreamPort = port;
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igs_setIpcFolderPath(char *path){
    if (strcmp(path, DEFAULT_IPC_PATH) == 0
        || (coreContext->network_ipcFolderPath != NULL && strcmp(path, coreContext->network_ipcFolderPath) == 0)){
        igs_debug("IPC folder path already is '%s'", path);
    }else{
        if (*path == '/'){
            if (coreContext->network_ipcFolderPath != NULL){
                free(coreContext->network_ipcFolderPath);
            }
            if (!zsys_file_exists(path)){
                igs_info("folder %s was created automatically", path);
                zsys_dir_create("%s", path);
            }
            coreContext->network_ipcFolderPath = strdup(path);
        }else{
            igs_error("IPC folder path must be absolute");
        }
    }
}

const char* igsAgent_getIpcFolderPath(){
    return coreContext->network_ipcFolderPath;
}
#endif

void igs_setAllowInproc(bool allow){
    coreContext->allowInproc = allow;
}

bool igs_getAllowInproc(){
    return coreContext->allowInproc;
}

void igs_setAllowIpc(bool allow){
    coreContext->allowIpc = allow;
}

bool igs_getAllowIpc(){
    return coreContext->allowIpc;
}

void igs_setHighWaterMarks(int hwmValue){
    if (hwmValue < 0){
        igs_error("HWM value must be zero or higher");
        return;
    }
    if (coreContext != NULL && coreContext->publisher != NULL){
        zsock_set_sndhwm(coreContext->publisher, hwmValue);
        zsock_set_sndhwm(coreContext->logger, hwmValue);
        igs_remote_agent_t *tmp = NULL, *subs = NULL;
        //FIXME: iterate on subscribers per agent
//        HASH_ITER(hh, coreContext->->subscribers, subs, tmp){
//            zsock_set_rcvhwm(subs->subscriber, hwmValue);
//        }
    }
    coreContext->network_hwmValue = hwmValue;
}

int igs_timerStart(size_t delay, size_t times, igs_timerCallback cb, void *myData){
    if (coreContext == NULL || coreContext->loop == NULL){
        igs_error("agent must be started before creating a timer");
        return -1;
    }
    if (cb == NULL){
        igs_error("callback function cannot be NULL");
        return -1;
    }
    network_Lock();
    igs_timer_t *timer = calloc(1, sizeof(igs_timer_t));
    timer->cb = cb;
    timer->myData = myData;
    timer->timerId = zloop_timer(coreContext->loop, delay, times, network_timerCallback, timer);
    HASH_ADD_INT(coreContext->timers, timerId, timer);
    network_Unlock();
    return timer->timerId;
}

void igs_timerStop(int timerId){
    if (coreContext->loop == NULL){
        igs_error("agent must be started to destroy a timer");
        return;
    }
    network_Lock();
    igs_timer_t *timer = NULL;
    HASH_FIND_INT(coreContext->timers, &timerId, timer);
    if (timer != NULL){
        zloop_timer_end(coreContext->loop, timerId);
        HASH_DEL(coreContext->timers, timer);
        free(timer);
    }else{
        igs_error("could not find timer with id %d", timerId);
    }
    network_Unlock();
}
