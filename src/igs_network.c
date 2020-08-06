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
    assert(network_Mutex);
    pthread_mutex_unlock(network_Mutex);
}

void cleanAndFreeZyrePeer(igs_zyre_peer_t **zyrePeer){
    assert(zyrePeer);
    assert(*zyrePeer);
    igs_debug("cleaning peer %s (%s)", (*zyrePeer)->name, (*zyrePeer)->peerId);
    if ((*zyrePeer)->peerId != NULL)
        free((*zyrePeer)->peerId);
    if ((*zyrePeer)->name != NULL)
        free((*zyrePeer)->name);
    if ((*zyrePeer)->subscriber != NULL)
        zsock_destroy(&((*zyrePeer)->subscriber));
}

//Adds proper filter to 'subscribe' socket for a spectific output of a given remote agent
void subscribeToRemoteAgentOutput(igs_remote_agent_t *remoteAgent, const char *outputName){
    assert(remoteAgent);
    assert(outputName);
    if(strlen(outputName) > 0){
        char filterValue[IGS_MAX_IOP_NAME_LENGTH + 33] = ""; //33 is UUID length + separator
        snprintf(filterValue, IGS_MAX_IOP_NAME_LENGTH + 33, "%s-%s", remoteAgent->uuid, outputName);
        bool filterAlreadyExists = false;
        igs_mappings_filter_t *filter = NULL;
        DL_FOREACH(remoteAgent->mappingsFilters, filter){
            if (strcmp(filter->filter, filterValue) == 0){
                filterAlreadyExists = true;
                break;
            }
        }
        if (!filterAlreadyExists){
            // Set subscriber to the output filter
            assert(remoteAgent->peer->subscriber);
            igs_debug("subscribe to agent %s output %s (%s)",remoteAgent->name,outputName, filterValue);
            zsock_set_subscribe(remoteAgent->peer->subscriber, filterValue);
            igs_mappings_filter_t *f = calloc(1, sizeof(igs_mappings_filter_t));
            f->filter = strdup(filterValue);
            DL_APPEND(remoteAgent->mappingsFilters, f);
        }else{
            //printf("\n****************\nFILTER BIS %s - %s\n***************\n", subscriber->agent->name, outputName);
        }
    }
}

//Removes filter to 'subscribe' socket for a spectific output of a given remote agent
void unsubscribeToRemoteAgentOutput(igs_remote_agent_t *remoteAgent, const char *outputName){
    assert(remoteAgent);
    assert(outputName);
    if(strlen(outputName) > 0){
        igs_mappings_filter_t *filter = NULL;
        DL_FOREACH(remoteAgent->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                assert(remoteAgent->peer->subscriber);
                igs_debug("unsubscribe to agent %s output %s",remoteAgent->name,outputName);
                zsock_set_unsubscribe(remoteAgent->peer->subscriber, outputName);
                free(filter->filter);
                DL_DELETE(remoteAgent->mappingsFilters, filter);
                free(filter);
                break;
            }
        }
    }
}

//Timer callback to send GET_CURRENT_OUTPUTS notification for an agent we subscribed to
int triggerOutputsRequestToNewcomer(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_remote_agent_t *remoteAgent = (igs_remote_agent_t *)arg;
    assert(remoteAgent);
    assert(remoteAgent->context);
    assert(remoteAgent->context->node);
    
    if (remoteAgent->shallSendOutputsRequest){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "GET_CURRENT_OUTPUTS");
        zmsg_addstr(msg, remoteAgent->uuid);
        zyre_whisper(remoteAgent->context->node, remoteAgent->peer->peerId, &msg);
        bus_zyreUnlock();
        remoteAgent->shallSendOutputsRequest = false;
    }
    return 0;
}

int network_configureMappingsToRemoteAgent(igs_agent_t *agent, igs_remote_agent_t *remoteAgent){
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
                    if (!remoteAgent->shallSendOutputsRequest && agent->network_requestOutputsFromMappedAgents){
                        remoteAgent->shallSendOutputsRequest = true;
                        remoteAgent->timerId = zloop_timer(coreContext->loop, 500, 1, triggerOutputsRequestToNewcomer, remoteAgent);
                    }
                }
                //NOTE: we do not clean subscriptions here because we cannot check if
                //an output is not used in another mapping element
            }
        }
    }
    return 0;
}

void sendDefinitionToZyrePeer(igs_agent_t *agent, const char *peer, const char *def){
    assert(agent);
    assert(agent->context);
    assert(agent->context->node);
    assert(peer);
    assert(def);
    bus_zyreLock();
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, definitionPrefix);
    zmsg_addstr(msg, def);
    zmsg_addstr(msg, agent->uuid);
    zmsg_addstr(msg, agent->name);
    zyre_whisper(coreContext->node, peer, &msg);
    bus_zyreUnlock();
}

void sendMappingToZyrePeer(igs_agent_t *agent, const char *peer, const char *mapping){
    assert(agent);
    assert(agent->context);
    assert(agent->context->node);
    assert(peer);
    assert(mapping);
    bus_zyreLock();
    zmsg_t *msg = zmsg_new();
    zmsg_addstr(msg, mappingPrefix);
    zmsg_addstr(msg, mapping);
    zmsg_addstr(msg, agent->uuid);
    zyre_whisper(coreContext->node, peer, &msg);
    bus_zyreUnlock();
}

void sendStateTo(igs_agent_t *agent, const char *peerOrChannel, bool isForPeer){
    assert(agent);
    assert(agent->context);
    assert(agent->context->node);
    assert(peerOrChannel);
    igs_core_context_t *context = agent->context;
    if (agent->definition != NULL && agent->definition->outputs_table != NULL){
        igs_iop_t *current_iop, *tmp_iop;
        HASH_ITER(hh, agent->definition->outputs_table, current_iop, tmp_iop) {
            if (current_iop->is_muted && current_iop->name != NULL){
                bus_zyreLock();
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "OUTPUT_MUTED");
                zmsg_addstr(msg, current_iop->name);
                zmsg_addstr(msg, agent->uuid);
                if (isForPeer){
                    zyre_whisper(context->node, peerOrChannel, &msg);
                }else{
                    zyre_shout(context->node, peerOrChannel, &msg);
                }
                bus_zyreUnlock();
            }
        }
    }
    //we also send our frozen and muted states, and other usefull information
    if (agent->isWholeAgentMuted){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "MUTED");
        zmsg_addstr(msg, "1");
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (agent->state != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "STATE");
        zmsg_addstr(msg, agent->state);
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (agent->definitionPath != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "DEFINITION_FILE_PATH");
        zmsg_addstr(msg, agent->definitionPath);
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (agent->mappingPath != NULL){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "MAPPING_FILE_PATH");
        zmsg_addstr(msg, agent->mappingPath);
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (context->isFrozen){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "FROZEN");
        zmsg_addstr(msg, "1");
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (context->logInStream){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "LOG_IN_STREAM");
        zmsg_addstr(msg, "1");
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
    if (context->logInFile){
        bus_zyreLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstr(msg, "LOG_IN_FILE");
        zmsg_addstr(msg, "1");
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        msg = zmsg_new();
        zmsg_addstr(msg, "LOG_FILE_PATH");
        zmsg_addstr(msg, context->logFilePath);
        zmsg_addstr(msg, agent->uuid);
        if (isForPeer){
            zyre_whisper(context->node, peerOrChannel, &msg);
        }else{
            zyre_shout(context->node, peerOrChannel, &msg);
        }
        bus_zyreUnlock();
    }
}

void cleanAndFreeRemoteAgent(igs_remote_agent_t **remoteAgent){
    assert(remoteAgent);
    assert(*remoteAgent);
    assert((*remoteAgent)->context);
    igs_debug("cleaning remote agent %s (%s)", (*remoteAgent)->name, (*remoteAgent)->uuid);
    #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    (*remoteAgent)->context->licenseEnforcement->currentAgentsNb--;
    #endif
    // clean the agent definition
    if((*remoteAgent)->definition != NULL){
        #if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
        (*remoteAgent)->context->licenseEnforcement->currentIOPNb -= (HASH_COUNT((*remoteAgent)->definition->inputs_table) +
                                                                      HASH_COUNT((*remoteAgent)->definition->outputs_table) +
                                                                      HASH_COUNT((*remoteAgent)->definition->params_table));
        //igs_license("license: %ld agents and %ld iops (%s)", agent->licenseEnforcement->currentAgentsNb, agent->licenseEnforcement->currentIOPNb, subscriber->name);
        #endif
        definition_freeDefinition(&(*remoteAgent)->definition);
    }
    //clean the agent mapping
    if((*remoteAgent)->mapping != NULL){
        mapping_freeMapping(&(*remoteAgent)->mapping);
    }
    //clean the remoteAgent itself
    igs_mappings_filter_t *elt, *tmp;
    DL_FOREACH_SAFE((*remoteAgent)->mappingsFilters,elt,tmp) {
        zsock_set_unsubscribe((*remoteAgent)->peer->subscriber, elt->filter);
        DL_DELETE((*remoteAgent)->mappingsFilters,elt);
        free(elt);
    }
    if ((*remoteAgent)->context->loop != NULL && (*remoteAgent)->peer->subscriber != NULL){
        zloop_reader_end((*remoteAgent)->context->loop, (*remoteAgent)->peer->subscriber);
    }
    if ((*remoteAgent)->uuid)
        free((*remoteAgent)->uuid);
    if ((*remoteAgent)->name)
        free((*remoteAgent)->name);
    if ((*remoteAgent)->context->loop != NULL && (*remoteAgent)->timerId > 0){
        zloop_timer_end((*remoteAgent)->context->loop, (*remoteAgent)->timerId);
        (*remoteAgent)->timerId = -2;
    }
    free(*remoteAgent);
    *remoteAgent = NULL;
}

////////////////////////////////////////////////////////////////////////
// ZMQ callbacks
////////////////////////////////////////////////////////////////////////

//manage messages from the parent thread
int manageParent (zloop_t *loop, zsock_t *pipe, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(arg)

    zmsg_t *msg = zmsg_recv(pipe);
    assert(msg);
    char *command = zmsg_popstr (msg);
    if (command == NULL){
        igs_error("command is NULL for parent message: aborting");
        zmsg_destroy(&msg);
        return 0;
    }
    if (streq (command, "STOP_LOOP")){
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
void handlePublicationFromRemoteAgent(zmsg_t *msg, igs_remote_agent_t *remoteAgent){
    assert(msg);
    assert(remoteAgent);
    assert(remoteAgent->context);
    
    if(remoteAgent->context->isFrozen == true){
        igs_debug("Message received from %s but all traffic in our process is currently frozen",
                  remoteAgent->name);
        return;
    }
    
    //Publication does not provide information about the targeted agents.
    //At this stage, we only know that one or more of our agents are targeted.
    //We need to iterate through our agents and their mapping to check which
    //inputs need to be updated on which agent.
    igs_agent_t *agent, *tmpAgent;
    HASH_ITER(hh, remoteAgent->context->agents, agent, tmpAgent){
        zmsg_t *dup = zmsg_dup(msg);
        size_t msgSize = zmsg_size(dup);
        char *output = NULL;
        char *vType = NULL;
        iopType_t valueType = 0;
        unsigned long i = 0;
        for (i = 0; i < msgSize; i += 3){
            // Each message part must contain 3 elements
            // 1 : output name
            // 2 : output ioptType
            // 3 : value of the output as a string or zframe
            output = zmsg_popstr(dup);
            if (output == NULL){
                igs_error("output name is NULL in received publication : aborting");
                break;
            }
            vType = zmsg_popstr(dup);
            if (vType == NULL){
                igs_error("output type is NULL in received publication : aborting");
                free(output);
                break;
            }
            valueType = atoi(vType);
            if (valueType < IGS_INTEGER_T || valueType > IGS_DATA_T){
                igs_error("output type is not valid (%d) in received publication : aborting", valueType);
                free(output);
                free(vType);
                break;
            }
            free(vType);
            vType = NULL;
            
            zframe_t *frame = NULL;
            void *data = NULL;
            size_t size = 0;
            char * value = NULL;
            //get data before iterating to all the mapping elements using it
            if (valueType == IGS_STRING_T){
                value = zmsg_popstr(dup);
                if (value == NULL){
                    igs_error("value is NULL in received publication : aborting");
                    break;
                }
            }else{
                frame = zmsg_pop(dup);
                if (frame == NULL){
                    igs_error("value is NULL in received publication : aborting");
                    break;
                }
                data = zframe_data(frame);
                size = zframe_size(frame);
            }
            if (agent->mapping){
                //try to find mapping elements matching with this subscriber's output
                //and update mapped input(s) value accordingly
                //TODO : optimize mapping storage to avoid iterating
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
                            igsAgent_warn(agent, "Input %s is missing in our definition but expected in our mapping with %s.%s",
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
    }
}

//manage incoming messages from one of the remote agents agents we subscribed to
int manageRemotePublication (zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(loop)
    igs_core_context_t *context = (igs_core_context_t *)arg;
    assert(socket);
    assert(context);
    
    zmsg_t *msg = zmsg_recv(socket);
    char *outputName = zmsg_popstr(msg);
    if (outputName == NULL){
        igs_error("output name is NULL in received publication : aborting");
        return 0;
    }
    char uuid[33] = ""; //33 is UUID length + terminal 0
    if (strlen(outputName) < 33){
        igs_error("output name '%s' is missing information : aborting", outputName);
        free(outputName);
        return 0;
    }
    snprintf(uuid, 33, "%s", outputName);
    char *realOutputName = outputName + 33;
    
    //ATTENTION: We push the output name again at the beginning of
    //the message for proper use by handlePublicationFromRemoteAgent
    zmsg_pushstr(msg, realOutputName);
    free(outputName);
    
    igs_remote_agent_t *remoteAgent = NULL;
    HASH_FIND_STR(context->remoteAgents, uuid, remoteAgent);
    if (remoteAgent == NULL){
        igs_error("no remote agent with uuid '%s' : aborting", uuid);
        return 0;
    }
    handlePublicationFromRemoteAgent(msg, remoteAgent);
    zmsg_destroy(&msg);
    return 0;
}

//manage messages received on the bus
int manageBusIncoming (zloop_t *loop, zsock_t *socket, void *arg){
    IGS_UNUSED(socket);
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
                                //FIXME: certainly useless with new architecture
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
                        if (context->network_allowInproc && useInproc){
                            zyrePeer->subscriber = zsock_new_sub(inprocAddress, NULL);
                            zsock_set_rcvhwm(zyrePeer->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (inproc)",zyrePeer->name, inprocAddress);
                        }else if (context->network_allowIpc && useIPC){
                            zyrePeer->subscriber = zsock_new_sub(ipcAddress, NULL);
                            zsock_set_rcvhwm(zyrePeer->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (ipc)",zyrePeer->name, ipcAddress);
                        }else{
                            zyrePeer->subscriber = zsock_new_sub(endpointAddress, NULL);
                            zsock_set_rcvhwm(zyrePeer->subscriber, context->network_hwmValue);
                            igs_debug("Subscription created for %s at %s (tcp)",zyrePeer->name, endpointAddress);
                        }
                        assert(zyrePeer->subscriber);
                        zloop_reader(loop, zyrePeer->subscriber, manageRemotePublication, context);
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
            agent_propagateAgentEvent(IGS_PEER_ENTERED, peer, name);
        }else{
            //Agent already exists, we set its reconnected flag
            //(this is used below to avoid agent destruction on EXIT received after timeout)
            zyrePeer->reconnected++;
        }
    } else if (streq (event, "JOIN")){
        igs_debug("+%s has joined %s", name, group);
        if (streq(group, IGS_PRIVATE_CHANNEL)){
            //send information for all our agents to the newcomer
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, context->agents, agent, tmp){
                //definition is sent to every newcomer on the channel (whether it is a ingescape agent or not)
                char * definitionStr = NULL;
                if (agent->definition != NULL)
                    definitionStr = parser_export_definition(agent->definition);
                if (definitionStr != NULL){
                    sendDefinitionToZyrePeer(agent, peer, definitionStr);
                    free(definitionStr);
                }else{
                    sendDefinitionToZyrePeer(agent, peer, "");
                }
                //and so is our mapping
                char *mappingStr = NULL;
                if (agent->mapping != NULL)
                    mappingStr = parser_export_mapping(agent->mapping);
                if (mappingStr != NULL){
                    sendMappingToZyrePeer(agent, peer, mappingStr);
                    free(mappingStr);
                }else{
                    sendMappingToZyrePeer(agent, peer, "");
                }
                //and so is the state of our internal variables
                sendStateTo(agent, peer, true);
            }
            
            igs_zyre_peer_t *zyrePeer = NULL;
            HASH_FIND_STR(context->zyrePeers, peer, zyrePeer);
            assert(zyrePeer);
            zyrePeer->hasJoinedPrivateChannel = true;
        }
    } else if (streq (event, "LEAVE")){
        igs_debug("-%s has left %s", name, group);
    } else if (streq (event, "SHOUT")){
        if (streq(group, context->replayChannel)){
            //this is a replay message for one of our inputs
            char *agentName = zmsg_popstr (msgDuplicate);
            char *input = zmsg_popstr (msgDuplicate);
            if (agentName == NULL){
                igs_error("agent name is NULL for replay message from %s(%s): aborting", name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            if (input == NULL){
                igs_error("input is NULL for replay message from %s(%s): aborting", name, peer);
                free(agentName);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }

            char *value = NULL;
            zframe_t *frame = NULL;
            void *data = NULL;
            size_t size = 0;
            igs_agent_t *targetAgent, *targettmp;
            HASH_ITER(hh, context->agents, targetAgent, targettmp){
                if (streq(agentName, targetAgent->name)){
                    iopType_t inputType = igsAgent_getTypeForInput(targetAgent, input);
                    if (zmsg_size(msgDuplicate) > 0){
                        igs_debug("replaying %s.%s", agentName, input);
                        if (inputType == IGS_STRING_T){
                            value = zmsg_popstr(msgDuplicate);
                            if (value == NULL){
                                igs_error("value is NULL for replay message from %s(%s): aborting", name, peer);
                                free(agentName);
                                free(input);
                                zmsg_destroy(&msgDuplicate);
                                zyre_event_destroy(&zyre_event);
                                return 0;
                            }
                            igsAgent_writeInputAsString(targetAgent, input, value);
                            free(value);
                        }else{
                            frame = zmsg_pop(msgDuplicate);
                            if (frame == NULL){
                                igs_error("value is NULL for replay message from %s(%s): aborting", name, peer);
                                free(agentName);
                                free(input);
                                zmsg_destroy(&msgDuplicate);
                                zyre_event_destroy(&zyre_event);
                                return 0;
                            }
                            data = zframe_data(frame);
                            size = zframe_size(frame);
                            model_writeIOP(targetAgent, input, IGS_INPUT_T, inputType, data, size);
                            zframe_destroy(&frame);
                        }
                    }else{
                        igsAgent_error(targetAgent, "replay message for input %s is not correct and was ignored", input);
                    }
                }
            }
            free(agentName);
            free(input);
            
        }else if (streq(group, IGS_PRIVATE_CHANNEL)){
            char *title = zmsg_popstr (msgDuplicate);
            if (streq(title, "REMOTE_AGENT_EXIT")){
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_remote_agent_t *remote = NULL;
                HASH_FIND_STR(context->remoteAgents, uuid, remote);
                if (remote){
                    igs_debug("<-%s (%s) exited", remote->name, uuid);
                    HASH_DEL(context->remoteAgents, remote);
                    agent_propagateAgentEvent(IGS_AGENT_EXITED, uuid, remote->name);
                    cleanAndFreeRemoteAgent(&remote);
                }else{
                    igs_error("%s is not a known remote agent", uuid);
                }
                if (uuid)
                    free(uuid);
            }
            free(title);
        }
    } else if(streq (event, "WHISPER")){
        char *title = zmsg_popstr (msgDuplicate);
        if (title == NULL){
            igs_error("no header in message received from %s(%s): aborting", name, peer);
            zmsg_destroy(&msgDuplicate);
            zyre_event_destroy(&zyre_event);
            return 0;
        }
        
        if(streq(title, "PEER_KNOWS_YOU")){
            //peer has received one of our agents definition
            //=> all agents in this peer know us
            char *uuid = zmsg_popstr(msgDuplicate);
            if (uuid == NULL){
                igs_error("no valid uuid in %s message received from %s(%s): aborting", title, name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            igs_agent_t *agent = NULL;
            HASH_FIND_STR(context->agents, uuid, agent);
            assert(agent);
            igs_agent_event_callback_t *cb;
            DL_FOREACH(agent->agentEventCallbacks, cb){
                //iterate on all remote agents for this peer : all its agents know us
                igs_remote_agent_t *r, *rtmp;
                HASH_ITER(hh, coreContext->remoteAgents, r, rtmp){
                    cb->callback_ptr(agent, IGS_AGENT_KNOWS_US, r->uuid, r->name, cb->myData);
                }
            }
        }
        //check if title is an EXTERNAL definition
        else if(streq(title, definitionPrefix)){
            //identify remote agent or create it if unknown.
            //NB: we suppose that remote agent creation is achieved when
            //the agent sends its definition for the first time.
            //Agents without definition are considered impossible.
            char* strDefinition = zmsg_popstr(msgDuplicate);
            if (strDefinition == NULL){
                igs_error("no valid definition in %s message received from %s(%s): aborting", title, name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr(msgDuplicate);
            if (uuid == NULL){
                igs_error("no valid uuid in %s message received from %s(%s): aborting", title, name, peer);
                free(strDefinition);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            char *remoteAgentName = zmsg_popstr(msgDuplicate);
            if (remoteAgentName == NULL){
                igs_error("no valid agent name in %s message received from %s(%s): aborting", title, name, peer);
                free(strDefinition);
                free(uuid);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            
            // Load definition from string content
            igs_definition_t *newDefinition = parser_loadDefinition(strDefinition);
            if (newDefinition != NULL){
                bool isAgentNew = false;
                igs_remote_agent_t *remoteAgent = NULL;
                HASH_FIND_STR(context->remoteAgents, uuid, remoteAgent);
                if (remoteAgent == NULL){
                    remoteAgent = calloc(1, sizeof(igs_remote_agent_t));
                    remoteAgent->context = context;
                    remoteAgent->uuid = strdup(uuid);
                    remoteAgent->name = strndup(remoteAgentName, IGS_MAX_AGENT_NAME_LENGTH);
                    igs_zyre_peer_t *zyrePeer = NULL;
                    HASH_FIND_STR(context->zyrePeers, peer, zyrePeer);
                    assert(zyrePeer);
                    remoteAgent->peer = zyrePeer;
                    HASH_ADD_STR(context->remoteAgents, uuid, remoteAgent);
                    igs_info("registering agent %s(%s)", uuid, remoteAgentName);
                    isAgentNew = true;
                }else{
                    //we already know this agent
                    if (remoteAgent->name != NULL)
                        free(remoteAgent->name);
                    remoteAgent->name = strndup(remoteAgentName, IGS_MAX_AGENT_NAME_LENGTH);
                }
                assert(remoteAgent);
                
                // Look if this agent already has a definition
                if(remoteAgent->definition != NULL) {
                    igs_debug("Definition already exists for agent %s : new definition will overwrite the previous one...", name);
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                    //we remove IOP count from previous definition
                    context->licenseEnforcement->currentIOPNb -= (HASH_COUNT(remoteAgent->definition->inputs_table) +
                                                                  HASH_COUNT(remoteAgent->definition->outputs_table) +
                                                                  HASH_COUNT(remoteAgent->definition->params_table));
                    //igs_license("%ld iops (cleaning %s)", agent->licenseEnforcement->currentIOPNb, name);
#endif
                    definition_freeDefinition(&remoteAgent->definition);
                }
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
                context->licenseEnforcement->currentIOPNb += (HASH_COUNT(newDefinition->inputs_table) +
                                                                  HASH_COUNT(newDefinition->outputs_table) +
                                                                  HASH_COUNT(newDefinition->params_table));
                //igs_license("%ld iops (adding %s)", agent->licenseEnforcement->currentIOPNb, name);
                if (context->licenseEnforcement->currentIOPNb > context->license->platformNbIOPs){
                    igs_license("maximum number of allowed IOPs (%d) is exceeded : agent will stop", context->license->platformNbIOPs);
                    igs_license_callback_t *el = NULL;
                    DL_FOREACH(context->licenseCallbacks, el){
                        el->callback_ptr(IGS_LICENSE_TOO_MANY_IOPS, el->data);
                    }
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return -1;
                }
#endif
                igs_debug("store definition for remote agent %s(%s)", remoteAgent->name, remoteAgent->uuid);
                remoteAgent->definition = newDefinition;
                //Check the involvement of this new remote agent and its definition in our agent mappings
                //and update subscriptions.
                //We check here because remote agent definition is required to handle received data.
                igs_agent_t *agent, *tmp;
                HASH_ITER(hh, context->agents, agent, tmp){
                    network_configureMappingsToRemoteAgent(agent, remoteAgent);
                }
                
                if (isAgentNew){
                    agent_propagateAgentEvent(IGS_AGENT_ENTERED, uuid, remoteAgentName);
                    bus_zyreLock();
                    zmsg_t *msg = zmsg_new();
                    zmsg_addstr(msg, "PEER_KNOWS_YOU");
                    zmsg_addstr(msg, uuid);
                    zyre_whisper(node, peer, &msg);
                    bus_zyreUnlock();
                }else{
                    agent_propagateAgentEvent(IGS_AGENT_UPDATED_DEFINITION, uuid, remoteAgentName);
                }
            }else{
                igs_error("received definition from remote agent %s(%s) is NULL : agent will not be registered", remoteAgentName, uuid);
            }
            free(strDefinition);
            free(uuid);
            free(remoteAgentName);
        }
        //check if title is an EXTERNAL mapping
        else if(streq(title, mappingPrefix)){
            //identify remote agent
            char* strMapping = zmsg_popstr (msgDuplicate);
            if (strMapping == NULL){
                igs_error("no valid mapping in %s message received from %s(%s): aborting", title, name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msgDuplicate);
            if (uuid == NULL){
                igs_error("uuid is NULL in %s message received from %s(%s): aborting", title, name, peer);
                free(strMapping);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            igs_remote_agent_t *remoteAgent = NULL;
            HASH_FIND_STR(context->remoteAgents, uuid, remoteAgent);
            if (remoteAgent == NULL){
                igs_error("no known remote agent with uuid '%s': aborting", uuid);
                free(strMapping);
                free(uuid);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            
            igs_mapping_t *newMapping = NULL;
            if (strlen(strMapping) > 0){
                //load mapping from string content
                newMapping = parser_loadMapping(strMapping);
                if (newMapping == NULL){
                    igs_error("received mapping for agent %s(%s) could not be parsed properly", remoteAgent->name, remoteAgent->uuid);
                }
            }else{
                igs_debug("received mapping from agent %s(%s) is empty", remoteAgent->name, remoteAgent->uuid);
                if(remoteAgent != NULL && remoteAgent->mapping != NULL) {
                    mapping_freeMapping(&remoteAgent->mapping);
                    remoteAgent->mapping = NULL;
                }
            }
            
            if (newMapping != NULL && remoteAgent != NULL){
                //look if this agent already has a mapping
                if(remoteAgent->mapping != NULL){
                    igs_debug("mapping already exists for agent %s(%s) : new mapping will overwrite the previous one...", remoteAgent->name, remoteAgent->uuid);
                    mapping_freeMapping(&remoteAgent->mapping);
                    remoteAgent->mapping = NULL;
                }
                
                igs_debug("store mapping for agent %s(%s)", remoteAgent->name, remoteAgent->uuid);
                remoteAgent->mapping = newMapping;
            }else{
                if(newMapping != NULL) {
                    mapping_freeMapping(&newMapping);
                    newMapping = NULL;
                }
            }
            free(strMapping);
            free(uuid);
        }
        //check if title is DEFINITION TO BE LOADED
        else if (streq(title, loadDefinitionPrefix)){
            //identify agent
            char* strDefinition = zmsg_popstr (msgDuplicate);
            if (strDefinition == NULL){
                igs_error("no valid definition in %s message received from %s(%s): aborting", title, name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msgDuplicate);
            igs_agent_t *agent = NULL;
            HASH_FIND_STR(context->agents, uuid, agent);
            if (agent == NULL){
                igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                free(strDefinition);
                if (uuid)
                    free(uuid);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            
            //load definition
            igsAgent_loadDefinition(agent, strDefinition);
            //recheck mapping towards our new definition
            igs_remote_agent_t *remote, *tmp;
            HASH_ITER(hh, context->remoteAgents, remote, tmp){
                network_configureMappingsToRemoteAgent(agent, remote);
            }
            free(strDefinition);
            free(uuid);
        }
        //check if title is MAPPING TO BE LOADED
        else if (streq(title, loadMappingPrefix)){
            //identify agent
            char* strMapping = zmsg_popstr (msgDuplicate);
            if (strMapping == NULL){
                igs_error("no valid mapping in %s message received from %s(%s): aborting", title, name, peer);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msgDuplicate);
            igs_agent_t *agent = NULL;
            HASH_FIND_STR(context->agents, uuid, agent);
            if (agent == NULL){
                igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                free(strMapping);
                if (uuid)
                    free(uuid);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                return 0;
            }
            
            // Load mapping from string content
            igs_mapping_t *m = parser_loadMapping(strMapping);
            if (m != NULL){
                if (agent->mapping != NULL){
                    mapping_freeMapping(&agent->mapping);
                }
                agent->mapping = m;
                //check and activate mapping
                igs_remote_agent_t *remote, *tmp;
                HASH_ITER(hh, context->remoteAgents, remote, tmp){
                    network_configureMappingsToRemoteAgent(agent, remote);
                }
                agent->network_needToUpdateMapping = true;
            }
            free(strMapping);
            free(uuid);
            
        }else{
            //
            //OTHER SUPPORTED MESSAGES
            //
            if (streq(title, "GET_CURRENT_OUTPUTS")){
                //identify agent
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                model_readWriteLock();
                zmsg_t *msgToSend = zmsg_new();
                zmsg_addstr(msgToSend, "CURRENT_OUTPUTS");
                zmsg_addstr(msgToSend, agent->uuid);
                igs_iop_t *outputs = agent->definition->outputs_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL; current = current->hh.next){
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.i), sizeof(int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.d), sizeof(double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addstr(msgToSend, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.b), sizeof(bool));
                            break;
                        case IGS_IMPULSION_T:
                            //FIXME: we had to disable outputs sending for data and impulsions but
                            //this is not consistent with inputs and parameters
                            //disabled
                            //                                    zmsg_addstr(msgToSend, found_iop->name);
                            //                                    zmsg_addstrf(msgToSend, "%d", found_iop->value_type);
                            //                                    zmsg_addmem(msgToSend, NULL, 0);
                            break;
                        case IGS_DATA_T:
                            //disabled
                            //                                    zmsg_addstr(msgToSend, found_iop->name);
                            //                                    zmsg_addstrf(msgToSend, "%d", found_iop->value_type);
                            //                                    zmsg_addmem(msgToSend, (found_iop->value.data), found_iop->valueSize);
                            break;
                            
                        default:
                            break;
                    }
                }
                model_readWriteUnlock();
                bus_zyreLock();
                igs_debug("send output values to %s", peer);
                zyre_whisper(node, peer, &msgToSend);
                bus_zyreUnlock();
                free(uuid);
                
            }else if (streq(title, "CURRENT_OUTPUTS")){
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_remote_agent_t *remoteAgent = NULL;
                HASH_FIND_STR(context->remoteAgents, uuid, remoteAgent);
                if (remoteAgent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                igs_debug("privately received output values from %s (%s)", remoteAgent->name, remoteAgent->uuid);
                handlePublicationFromRemoteAgent(msgDuplicate, remoteAgent);
                zmsg_destroy(&msgDuplicate);
                free(uuid);
                
            }else if (streq(title, "GET_CURRENT_INPUTS")){
                //identify agent
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                model_readWriteLock();
                zmsg_t *msgToSend = zmsg_new();
                zmsg_addstr(msgToSend, "CURRENT_INPUTS");
                zmsg_addstr(msgToSend, agent->uuid);
                igs_iop_t *outputs = agent->definition->inputs_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL; current = current->hh.next){
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.i), sizeof(int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.d), sizeof(double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addstr(msgToSend, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.b), sizeof(bool));
                            break;
                        case IGS_IMPULSION_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, NULL, 0);
                            break;
                        case IGS_DATA_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, (current->value.data), current->valueSize);
                            break;
                            
                        default:
                            break;
                    }
                }
                model_readWriteUnlock();
                bus_zyreLock();
                igs_debug("send input values to %s", peer);
                zyre_whisper(node, peer, &msgToSend);
                bus_zyreUnlock();
                free(uuid);
                
            }else if (streq(title, "GET_CURRENT_PARAMETERS")){
                //identify agent
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                model_readWriteLock();
                zmsg_t *msgToSend = zmsg_new();
                zmsg_addstr(msgToSend, "CURRENT_PARAMETERS");
                zmsg_addstr(msgToSend, agent->uuid);
                igs_iop_t *outputs = agent->definition->params_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL; current = current->hh.next){
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.i), sizeof(int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.d), sizeof(double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addstr(msgToSend, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, &(current->value.b), sizeof(bool));
                            break;
                        case IGS_IMPULSION_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, NULL, 0);
                            break;
                        case IGS_DATA_T:
                            zmsg_addstr(msgToSend, current->name);
                            zmsg_addstrf(msgToSend, "%d", current->value_type);
                            zmsg_addmem(msgToSend, (current->value.data), current->valueSize);
                            break;
                            
                        default:
                            break;
                    }
                }
                model_readWriteUnlock();
                bus_zyreLock();
                igs_debug("send parameters values to %s", peer);
                zyre_whisper(node, peer, &msgToSend);
                bus_zyreUnlock();
                free(uuid);
                
            }else if (streq(title, "GET_LICENSE_INFO")){
#if !TARGET_OS_IOS
                zmsg_t *resp = zmsg_new();
                zmsg_addstr(resp, "LICENSE_INFO");
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
                        zmsg_addstr(resp, licForAgent->agentName);
                        licForAgent = zhash_next(context->license->agents);
                    }
                }
                bus_zyreLock();
                zyre_whisper(node, peer, &resp);
                bus_zyreUnlock();
                igs_debug("send license information to %s", peer);
#endif
                
            }
            else if (streq(title, "START_AGENT")){
                char *agentName = zmsg_popstr (msgDuplicate);
                if (agentName == NULL){
                    igs_error("no agent name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }

                igs_debug("received 'START_AGENT %s' command from %s (%s)", agentName, name, peer);
                igs_agent_t *a = zhash_first(coreContext->createdAgents);
                while (a) {
                    if (streq(a->name, agentName)){
                        igs_info("activating agent %s (%s)", a->name, a->uuid);
                        igsAgent_activate(a);
                    }
                    a = zhash_next(coreContext->createdAgents);
                }

            }
            else if (streq(title, "STOP_AGENT")){
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                igs_debug("received 'STOP_AGENT %s' command from %s (%s)", uuid, name, peer);
                igs_info("deactivating agent %s (%s)", agent->name, agent->uuid);
                igsAgent_deactivate(agent);
                
            }else if (streq(title, "STOP_PEER")){
                context->externalStop = true;
                igs_debug("received STOP_PEER command from %s (%s)", name, peer);
                free(title);
                zmsg_destroy(&msgDuplicate);
                zyre_event_destroy(&zyre_event);
                //stop our zyre loop by returning -1 : this will start the cleaning process
                return -1;
                
            }else if (streq(title, "CLEAR_MAPPING")){
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received CLEAR_MAPPING command from %s (%s)", name, peer);
                igsAgent_clearMapping(agent);
                free(uuid);
                
            }else if (streq(title, "FREEZE")){
                igs_debug("received FREEZE command from %s (%s)", name, peer);
                igs_freeze();
                
            }else if (streq(title, "UNFREEZE")){
                igs_debug("received UNFREEZE command from %s (%s)", name, peer);
                igs_unfreeze();
                
            }else if (streq(title, "MUTE_ALL")){
                igs_debug("received MUTE_ALL command from %s (%s)", name, peer);
                igs_agent_t *agent, *tmp;
                HASH_ITER(hh, context->agents, agent, tmp){
                    igsAgent_mute(agent);
                }
                
            }else if (streq(title, "UNMUTE_ALL")){
                igs_debug("received UNMUTE_ALL command from %s (%s)", name, peer);
                igs_agent_t *agent, *tmp;
                HASH_ITER(hh, context->agents, agent, tmp){
                    igsAgent_unmute(agent);
                }
                
            }else if (streq(title, "MUTE")){
                char *iopName = zmsg_popstr (msgDuplicate);
                if (iopName == NULL){
                    igs_error("no valid iop name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(iopName);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received MUTE command from %s (%s)", name, peer);
                igsAgent_muteOutput(agent, iopName);
                free(iopName);
                free(uuid);
                
            }else if (streq(title, "UNMUTE")){
                char *iopName = zmsg_popstr (msgDuplicate);
                if (iopName == NULL){
                    igs_error("no valid iop name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(iopName);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received UNMUTE command from %s (%s)", name, peer);
                igsAgent_unmuteOutput(agent,iopName);
                free(iopName);
                free(uuid);
                
            }else if (streq(title, "SET_INPUT")){
                char *iopName = zmsg_popstr (msgDuplicate);
                if (iopName == NULL){
                    igs_error("no valid iop name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msgDuplicate);
                if (value == NULL){
                    igs_error("no valid value in %s message received from %s(%s): aborting", title, name, peer);
                    free(iopName);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(iopName);
                    free(value);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SET_INPUT command from %s (%s)", name, peer);
                if (iopName != NULL && value != NULL){
                    igsAgent_writeInputAsString(agent, iopName, value);
                }
                free(iopName);
                free(value);
                free(uuid);
                
            }else if (streq(title, "SET_OUTPUT")){
                char *iopName = zmsg_popstr (msgDuplicate);
                if (iopName == NULL){
                    igs_error("no valid iop name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msgDuplicate);
                if (value == NULL){
                    igs_error("no valid value in %s message received from %s(%s): aborting", title, name, peer);
                    free(iopName);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(iopName);
                    free(value);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SET_OUTPUT command from %s (%s)", name, peer);
                if (iopName != NULL && value != NULL){
                    igsAgent_writeOutputAsString(agent, iopName, value);
                }
                free(iopName);
                free(value);
                free(uuid);
                
            }else if (streq(title, "SET_PARAMETER")){
                char *iopName = zmsg_popstr (msgDuplicate);
                if (iopName == NULL){
                    igs_error("no valid iop name in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msgDuplicate);
                if (value == NULL){
                    igs_error("no valid value in %s message received from %s(%s): aborting", title, name, peer);
                    free(iopName);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(iopName);
                    free(value);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SET_PARAMETER command from %s (%s)", name, peer);
                if (iopName != NULL && value != NULL){
                    igsAgent_writeParameterAsString(agent, iopName, value);
                }
                free(iopName);
                free(value);
                free(uuid);
                
            }else if (streq(title, "MAP")){
                char *input = zmsg_popstr (msgDuplicate);
                if (input == NULL){
                    igs_error("no valid input in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *remoteAgent = zmsg_popstr (msgDuplicate);
                if (remoteAgent == NULL){
                    igs_error("no valid agent name in %s message received from %s(%s): aborting", title, name, peer);
                    free(input);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msgDuplicate);
                if (output == NULL){
                    igs_error("no valid output in %s message received from %s(%s): aborting", title, name, peer);
                    free(input);
                    free(remoteAgent);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(input);
                    free(remoteAgent);
                    free(output);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received MAP command from %s (%s)", name, peer);
                if (input != NULL && remoteAgent != NULL && output != NULL){
                    igsAgent_addMappingEntry(agent, input, remoteAgent, output);
                }
                free(input);
                free(remoteAgent);
                free(output);
                free(uuid);
                
            }else if (streq(title, "UNMAP")){
                char *input = zmsg_popstr (msgDuplicate);
                if (input == NULL){
                    igs_error("no valid input in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *remoteAgent = zmsg_popstr (msgDuplicate);
                if (remoteAgent == NULL){
                    igs_error("no valid agent name in %s message received from %s(%s): aborting", title, name, peer);
                    free(input);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msgDuplicate);
                if (output == NULL){
                    igs_error("no valid output in %s message received from %s(%s): aborting", title, name, peer);
                    free(input);
                    free(remoteAgent);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(input);
                    free(remoteAgent);
                    free(output);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received UNMAP command from %s (%s)", name, peer);
                if (input != NULL && remoteAgent != NULL && output != NULL){
                    igsAgent_removeMappingEntryWithName(agent, input, remoteAgent, output);
                }
                free(input);
                free(remoteAgent);
                free(output);
                free(uuid);
            }
            //admin API
            else if (streq(title, "ENABLE_LOG_STREAM")){
                igs_debug("received ENABLE_LOG_STREAM command from %s (%s)", name, peer);
                igs_setLogStream(true);
            }
            else if (streq(title, "DISABLE_LOG_STREAM")){
                igs_debug("received DISABLE_LOG_STREAM command from %s (%s)", name, peer);
                igs_setLogStream(false);
            }
            else if (streq(title, "ENABLE_LOG_FILE")){
                igs_debug("received ENABLE_LOG_FILE command from %s (%s)", name, peer);
                igs_setLogInFile(true);
            }
            else if (streq(title, "DISABLE_LOG_FILE")){
                igs_debug("received DISABLE_LOG_FILE command from %s (%s)", name, peer);
                igs_setLogInFile(false);
            }
            else if (streq(title, "SET_LOG_PATH")){
                char *logPath = zmsg_popstr (msgDuplicate);
                igs_debug("received SET_LOG_PATH command from %s (%s)", name, peer);
                igs_setLogPath(logPath);
            }
            else if (streq(title, "SET_DEFINITION_PATH")){
                char *definitionPath = zmsg_popstr (msgDuplicate);
                if (definitionPath == NULL){
                    igs_error("no valid definition path in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(definitionPath);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SET_DEFINITION_PATH command from %s (%s)", name, peer);
                igsAgent_setDefinitionPath(agent, definitionPath);
                free(definitionPath);
                free(uuid);
            }
            else if (streq(title, "SET_MAPPING_PATH")){
                char *mappingPath = zmsg_popstr (msgDuplicate);
                if (mappingPath == NULL){
                    igs_error("no valid mapping path in %s message received from %s(%s): aborting", title, name, peer);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    free(mappingPath);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                         
                igs_debug("received SET_MAPPING_PATH command from %s (%s)", name, peer);
                igsAgent_setMappingPath(agent, mappingPath);
                free(mappingPath);
                free(uuid);
            }
            else if (streq(title, "SAVE_DEFINITION_TO_PATH")){
                //identify agent
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SAVE_DEFINITION_TO_PATH command from %s (%s)", name, peer);
                igsAgent_writeDefinitionToPath(agent);
                free(uuid);
            }
            else if (streq(title, "SAVE_MAPPING_TO_PATH")){
                //identify agent
                char *uuid = zmsg_popstr (msgDuplicate);
                igs_agent_t *agent = NULL;
                HASH_FIND_STR(context->agents, uuid, agent);
                if (agent == NULL){
                    igs_error("no agent with uuid '%s' in %s message received from %s(%s): aborting", uuid, title, name, peer);
                    if (uuid)
                        free(uuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                igs_debug("received SAVE_MAPPING_TO_PATH command from %s (%s)", name, peer);
                igsAgent_writeMappingToPath(agent);
                free(uuid);
            }
            else if (streq (title, "CALL")){
                //identify agent
                char *callerUuid = zmsg_popstr (msgDuplicate);
                char *calleeUuid = zmsg_popstr (msgDuplicate);
                
                igs_remote_agent_t *callerAgent = NULL;
                HASH_FIND_STR(context->remoteAgents, callerUuid, callerAgent);
                if (callerAgent == NULL){
                    igs_error("no caller agent with uuid '%s' in %s message received from %s(%s): aborting", callerUuid, title, name, peer);
                    if (calleeUuid)
                        free(calleeUuid);
                    if (callerUuid)
                        free(callerUuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }

                igs_agent_t *calleeAgent = NULL;
                HASH_FIND_STR(context->agents, calleeUuid, calleeAgent);
                if (calleeAgent == NULL){
                    igs_error("no callee agent with uuid '%s' in %s message received from %s(%s): aborting", calleeUuid, title, name, peer);
                    if (calleeUuid)
                        free(calleeUuid);
                    if (callerUuid)
                        free(callerUuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                
                char *callName = zmsg_popstr(msgDuplicate);
                if (callName == NULL){
                    igs_error("no call name in %s message received from %s(%s): aborting", title, name, peer);
                    free(callerUuid);
                    free(calleeUuid);
                    zmsg_destroy(&msgDuplicate);
                    zyre_event_destroy(&zyre_event);
                    return 0;
                }
                if (calleeAgent->definition != NULL && calleeAgent->definition->calls_table != NULL){
                    igs_call_t *call = NULL;
                    HASH_FIND_STR(calleeAgent->definition->calls_table, callName, call);
                    if (call != NULL ){
                        if (call->cb != NULL){
                            bus_zyreLock();
                            zyre_shouts(context->node, context->callsChannel, "%s from %s (%s)", callName, callerAgent->name, callerUuid);
                            bus_zyreUnlock();
                            size_t nbArgs = 0;
                            igs_callArgument_t *_arg = NULL;
                            LL_COUNT(call->arguments, _arg, nbArgs);
                            if (call_addValuesToArgumentsFromMessage(callName, call->arguments, msgDuplicate) == IGS_SUCCESS){
                                (call->cb)(calleeAgent, callerAgent->name, callerUuid, callName, call->arguments, nbArgs, call->cbData);
                                call_freeValuesInArguments(call->arguments);
                            }
                        }else{
                            igsAgent_warn(calleeAgent, "no defined callback to handle received call %s", callName);
                        }
                    }else{
                        igsAgent_warn(calleeAgent, "agent %s(%s) has no call named %s", calleeAgent->name, calleeUuid, callName);
                    }
                }
                free(callerUuid);
                free(calleeUuid);
                free(callName);
            }
            //Performance
            else if (strcmp (title, "PING") == 0){
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
            else if (strcmp (title, "PONG") == 0){
                //continue performance measurement
                zframe_t *countF = zmsg_pop(msgDuplicate);
                size_t count = 0;
                memcpy(&count, zframe_data(countF), sizeof(size_t));
                zframe_t *payload = zmsg_pop(msgDuplicate);
                //igsAgent_info(agent, "pong %zu from %s", count, peer);
                if (count != context->performanceMsgCounter){
                    igs_error("pong message lost at index %zu from %s", count, peer);
                } else if (count == context->performanceMsgCountTarget){
                    //last message received
                    context->performanceStop = zclock_usecs();
                    igs_info("message size: %zu bytes", context->performanceMsgSize);
                    igs_info("roundtrip count: %zu", context->performanceMsgCountTarget);
                    igs_info("average latency: %.3f µs", ((double) context->performanceStop - (double) context->performanceStart) / context->performanceMsgCountTarget);
                    size_t throughput = (size_t) ((double) context->performanceMsgCountTarget / ((double) context->performanceStop - (double) context->performanceStart) * 1000000);
                    double megabytes = (double) throughput * context->performanceMsgSize / (1024*1024);
                    igs_info("average roundtrip throughput: %zu msg/s", (size_t)throughput);
                    igs_info("average roundtrip throughput: %.3f MB/s", megabytes);
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
        free(title);
    } else if (streq (event, "EXIT")){
        igs_debug("<-%s (%s) exited", name, peer);
        igs_zyre_peer_t *zyrePeer = NULL;
        HASH_FIND_STR(context->zyrePeers, peer, zyrePeer);
        if (zyrePeer != NULL){
            if (zyrePeer->reconnected > 0){
                //do not clean: we are getting a timemout now whereas
                //the agent is reconnected
                zyrePeer->reconnected--;
            }else{
                igs_remote_agent_t *remote, *tmpremote;
                HASH_ITER(hh, context->remoteAgents, remote, tmpremote){
                    //destroy all remote agents attached to this peer
                    if (streq(remote->peer->peerId, zyrePeer->peerId)){
                        HASH_DEL(context->remoteAgents, remote);
                        agent_propagateAgentEvent(IGS_AGENT_EXITED, remote->uuid, remote->name);
                        cleanAndFreeRemoteAgent(&remote);
                    }
                }
                HASH_DEL(context->zyrePeers, zyrePeer);
                agent_propagateAgentEvent(IGS_PEER_EXITED, peer, name);
                cleanAndFreeZyrePeer(&zyrePeer);
            }
        }
    }
    
    //handle callbacks
    //NB: as explained earlier, agent may be NULL
    //depending on the event type.
    igs_zyre_callback_t *elt;
    DL_FOREACH(context->zyreCallbacks,elt){
        if (zyre_event != NULL){
            zmsg_t *dup = zmsg_dup(msg);
            elt->callback_ptr(event, peer, name, address, group, headers, dup, elt->myData);
            zmsg_destroy(&dup);
        }else{
            igs_error("previous callback certainly destroyed the bus event : next callbacks will not be executed");
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
    assert(context);

    igs_agent_t *agent, *tmp;
    HASH_ITER(hh, context->agents, agent, tmp){
        if (agent->network_needToSendDefinitionUpdate){
            model_readWriteLock();
            char * definitionStr = NULL;
            definitionStr = parser_export_definition(agent->definition);
            if (definitionStr != NULL){
                igs_zyre_peer_t *p, *ptmp;
                HASH_ITER(hh, context->zyrePeers, p, ptmp){
                    if (p->hasJoinedPrivateChannel){
                        sendDefinitionToZyrePeer(agent, p->peerId, definitionStr);
                    }
                }
                free(definitionStr);
            }
            //NB: this is not optimal to resend state details on definition change but it
            //is the cleanest way to send state on after-start agent activation.
            //State details are still sent individually when they change.
            sendStateTo(agent, IGS_PRIVATE_CHANNEL, false);
            agent->network_needToSendDefinitionUpdate = false;
            //when definition changes, mapping may need to be updated as well
            agent->network_needToUpdateMapping = true;
            model_readWriteUnlock();
        }
    }
    return 0;
}

//Timer callback to update and (re)send our mapping to agents on the private channel
int triggerMappingUpdate(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_core_context_t *context = (igs_core_context_t *)arg;
    assert(context);

    igs_agent_t *agent, *tmp;
    HASH_ITER(hh, context->agents, agent, tmp){
        if (agent->network_needToUpdateMapping){
            char *mappingStr = NULL;
            mappingStr = parser_export_mapping(agent->mapping);
            if (mappingStr != NULL){
                igs_zyre_peer_t *p, *ptmp;
                HASH_ITER(hh, context->zyrePeers, p, ptmp){
                    if (p->hasJoinedPrivateChannel){
                        sendMappingToZyrePeer(agent, p->peerId, mappingStr);
                    }
                }
                free(mappingStr);
            }
            igs_remote_agent_t *remote, *rtmp;
            HASH_ITER(hh, context->remoteAgents, remote, rtmp){
                network_configureMappingsToRemoteAgent(agent, remote);
            }
            agent->network_needToUpdateMapping = false;
        }
    }
    return 0;
}

//Timer callback to stop loop if license is expired or demo mode is on
int triggerLicenseStop(zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop);
    IGS_UNUSED(timer_id);
    IGS_UNUSED(arg);
    igs_license("Runtime duration limit has been reached : stopping Ingescape now");
    igs_license_callback_t *el = NULL;
    DL_FOREACH(coreContext->licenseCallbacks, el){
        el->callback_ptr(IGS_LICENSE_TIMEOUT, el->data);
    }
    return -1;
}

static void runLoop (zsock_t *mypipe, void *args){
    network_Lock();
    igs_core_context_t *context = (igs_core_context_t *)args;
    assert(context);
    assert(context->node);
    assert(context->publisher);
    assert(context->replayChannel);
    assert(context->callsChannel);
#if ENABLE_LICENSE_ENFORCEMENT && !TARGET_OS_IOS
    assert(context->licenseEnforcement);
#endif
#if defined __unix__ || (defined __APPLE__ && !TARGET_OS_IOS) || defined __linux__
    assert(context->network_ipcFullPath);
    assert(context->network_ipcEndpoint);
    assert(context->ipcPublisher);
    assert(context->inprocPublisher);
#endif
    
    //iterate on agents to avoid sending definition and mapping update at startup
    //to all peers (they will receive def & map when joining INGESCAPE_PRIVATE)
    igs_agent_t *agent, *tmp;
    HASH_ITER(hh, context->agents, agent, tmp){
        agent->network_needToUpdateMapping = false;
        agent->network_needToSendDefinitionUpdate = false;
    }
    
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
    assert(context->license);
    if (!context->license->isLicenseValid){
        igs_license("License is not valid : starting timer for demonstration mode (%d seconds)...", IGS_MAX_EXEC_DURATION_DURING_EVAL);
        zloop_timer(context->loop, IGS_MAX_EXEC_DURATION_DURING_EVAL * 1000, 0, triggerLicenseStop, NULL);
    }
#endif
    zsock_signal (mypipe, 0);
    network_Unlock();
    
    
    /////////////////////
    igs_debug("loop starting");
    zloop_start (context->loop); //returns when one of the pollers returns -1
    /////////////////////
    
    
    network_Lock();
    igs_debug("loop stopping..."); //clean dynamic part of the context
    zloop_destroy (&context->loop);
    
    igs_remote_agent_t *remote, *tmpremote;
    HASH_ITER(hh, context->remoteAgents, remote, tmpremote) {
        HASH_DEL(context->remoteAgents, remote);
        cleanAndFreeRemoteAgent(&remote);
    }
    
    igs_zyre_peer_t *zyrePeer, *tmpPeer;
    HASH_ITER(hh, context->zyrePeers, zyrePeer, tmpPeer){
        HASH_DEL(context->zyrePeers, zyrePeer);
        cleanAndFreeZyrePeer(&zyrePeer);
    }
    
    igs_timer_t *current_timer, *tmp_timer;
    HASH_ITER(hh, context->timers, current_timer, tmp_timer){
        HASH_DEL(context->timers, current_timer);
        free(current_timer);
    }
    
    //zmq stack cleaning
    zyre_stop (context->node);
    zyre_destroy (&context->node);
    zsock_destroy(&context->publisher);
    zsock_destroy(&context->ipcPublisher);
#if defined __unix__ || (defined __APPLE__ && !TARGET_OS_IOS) || defined __linux__
    zsys_file_delete(context->network_ipcFullPath); //destroy ipcPath in file system
    //NB: ipcPath is based on peer id which is unique. It will never be used again.
    free(context->network_ipcFullPath);
    context->network_ipcFullPath = NULL;
#endif
#if (!TARGET_OS_IOS)
    if (context->inprocPublisher != NULL){
        zsock_destroy(&context->inprocPublisher);
    }
#endif
    if (context->logger != NULL){
        zsock_destroy(&context->logger);
    }
    
    //handle external stop if needed
    if (context->externalStop){
        igs_external_stop_calback_t *cb = NULL;
        igs_agent_t *a, *atmp;
        HASH_ITER(hh, context->agents, a, atmp){
            DL_FOREACH(context->externalStopCalbacks, cb){
                cb->callback_ptr(cb->myData);
            }
        }
    }
    
    //clean remaining dynamic data
    if (context->licenseEnforcement){
        free(context->licenseEnforcement);
        context->licenseEnforcement = NULL;
    }
    free(context->replayChannel);
    context->replayChannel = NULL;
    free(context->callsChannel);
    context->callsChannel = NULL;
    if (context->network_ipcFullPath){
        free(context->network_ipcFullPath);
        context->network_ipcFullPath = NULL;
    }
    if (context->network_ipcEndpoint){
        free(context->network_ipcEndpoint);
        context->network_ipcEndpoint = NULL;
    }
    
    igs_debug("loop stopped");
    zstr_send(mypipe, "LOOP_STOPPED");
    network_Unlock();
}

void initLoop (igs_core_context_t *context){
    core_initAgent(); //to be sure to have a default agent name
    
    igs_debug("loop init");
    network_Lock();
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
    
    context->externalStop = false;
    bool canContinue = true;
    //prepare zyre
    bus_zyreLock();
    context->node = zyre_new(coreAgent->name);
    //zyre_set_verbose(context->node);
    bus_zyreUnlock();
    if (context->brokerEndPoint != NULL){
        bus_zyreLock();
        zyre_set_verbose(context->node);
        zyre_gossip_connect(context->node,"%s", context->brokerEndPoint);
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
    zyre_join(context->node, IGS_PRIVATE_CHANNEL);
    bus_zyreUnlock();
    
    //create channel for replay
    assert(context->replayChannel == NULL);
    context->replayChannel = calloc(1, strlen(coreAgent->name) + strlen("-IGS-REPLAY") + 1);
    snprintf(context->replayChannel, IGS_MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-REPLAY", coreAgent->name);
    bus_zyreLock();
    zyre_join(context->node, context->replayChannel);
    bus_zyreUnlock();
    
    //create channel for calls feedback
    assert(context->callsChannel == NULL);
    context->callsChannel = calloc(1, strlen(coreAgent->name) + strlen("-IGS-CALLS") + 1);
    snprintf(context->callsChannel, IGS_MAX_AGENT_NAME_LENGTH + 15, "%s-IGS-CALLS", coreAgent->name);
    bus_zyreLock();
    zyre_join(context->node, context->callsChannel);
    bus_zyreUnlock();
    
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
#if defined __unix__ || (defined __APPLE__ && ! TARGET_OS_IOS) || defined __linux__
    if (context->network_ipcFolderPath == NULL){
        context->network_ipcFolderPath = strdup(IGS_DEFAULT_IPC_FOLDER_PATH);
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
    context->ipcPublisher = zsock_new_pub(context->network_ipcEndpoint);
    if (context->ipcPublisher == NULL){
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
        zsock_set_sndhwm(context->ipcPublisher, context->network_hwmValue);
        bus_zyreLock();
        zyre_set_header(context->node, "loopback", "%s", zsock_endpoint(ipcPublisher));
        bus_zyreUnlock();
    }
#endif
    
    //start inproc publisher
#if (!TARGET_OS_IOS)
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
    free(inprocEndpoint);
#endif
    
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

#if defined __unix__ || defined __APPLE__ || defined __linux__
    ssize_t ret;
    context->processId = getpid();
    bus_zyreLock();
    zyre_set_header(context->node, "pid", "%i", context->processId);
    bus_zyreUnlock();
    
    if (context->commandLine == NULL){
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
            igs_debug("PID %d: proc_pidpath () - %s", context->processId, strerror(errno));
        } else {
            igs_debug("proc %d: %s", context->processId, pathbuf);
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
    if ((context->commandLine == NULL) || (strlen(context->commandLine) == 0)) {
        //command line was not set manually : we try to get exec path instead

        // Use GetModuleFileName() to get exec path
        char exeFilePath[IGS_MAX_PATH_LENGTH];
#ifdef UNICODE
        WCHAR temp[IGS_MAX_PATH];
        GetModuleFileName(NULL,temp,IGS_MAX_PATH_LENGTH);
        //Conversion in char *
        wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));
#else
        GetModuleFileName(NULL,exeFilePath,IGS_MAX_PATH_LENGTH);
#endif
        
        bus_zyreLock();
        zyre_set_header(context->node, "commandline", "%s", exeFilePath);
        bus_zyreUnlock();
    }else{
        bus_zyreLock();
        zyre_set_header(context->node, "commandline", "%s", context->commandLine);
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
    network_Unlock();
    
    if (canContinue){
        context->networkActor = zactor_new(runLoop, context);
    }
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

igs_result_t network_publishOutput (igs_agent_t *agent, const igs_iop_t *iop){
    assert(agent);
    assert(agent->context);
    assert(agent->uuid);
    assert(iop);
    assert(iop->name);
    int result = IGS_SUCCESS;
    
    if(!agent->isWholeAgentMuted && !iop->is_muted && !agent->context->isFrozen){
        model_readWriteLock();
        zmsg_t *msg = zmsg_new();
        zmsg_addstrf(msg, "%s-%s", agent->uuid, iop->name);
        zmsg_addstrf(msg, "%d", iop->value_type);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                zmsg_addmem(msg, &(iop->value.i), sizeof(int));
                igsAgent_debug(agent, "%s(%s) publishes %s -> %d",
                               agent->name, agent->uuid, iop->name, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                zmsg_addmem(msg, &(iop->value.d), sizeof(double));
                igsAgent_debug(agent, "%s(%s) publishes %s -> %f",
                               agent->name, agent->uuid, iop->name, iop->value.d);
                break;
            case IGS_BOOL_T:
                zmsg_addmem(msg, &(iop->value.b), sizeof(bool));
                igsAgent_debug(agent, "%s(%s) publishes %s -> %d",
                               agent->name, agent->uuid, iop->name, iop->value.b);
                break;
            case IGS_STRING_T:
                zmsg_addstr(msg, iop->value.s);
                igsAgent_debug(agent, "%s(%s) publishes %s -> '%s'",
                               agent->name, agent->uuid, iop->name, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                zmsg_addmem(msg, NULL, 0);
                igsAgent_debug(agent, "%s(%s) publishes impulsion %s",
                               agent->name, agent->uuid, iop->name);
                break;
            case IGS_DATA_T:{
                zframe_t *frame = zframe_new(iop->value.data, iop->valueSize);
                zmsg_append(msg, &frame);
                igsAgent_debug(agent, "%s(%s) publishes data %s (%zu bytes)",
                               agent->name, agent->uuid, iop->name, iop->valueSize);
            }
                break;
            default:
                break;
        }
        
        //1- publish to TCP
        zmsg_t *msgQuater = zmsg_dup(msg);
        if(agent->context->networkActor && agent->context->publisher){
            zmsg_t *msgBis = zmsg_dup(msg);
            zmsg_t *msgTer = zmsg_dup(msg);
            if (zmsg_send(&msg, coreContext->publisher) != 0){
                igsAgent_error(agent, "Could not publish output %s on the network\n",iop->name);
                zmsg_destroy(&msg);
                result = IGS_FAILURE;
            }
            //2- publish to IPC
            if (coreContext->ipcPublisher != NULL){
                //publisher can be NULL on IOS or for read/write problems with assigned IPC path
                //in both cases, an error message has been issued at start
                if (zmsg_send(&msgBis, coreContext->ipcPublisher) != 0){
                    igsAgent_error(agent, "Could not publish output %s using IPC\n",iop->name);
                    zmsg_destroy(&msgBis);
                    result = IGS_FAILURE;
                }
            }
            //3- publish to inproc
            if (coreContext->inprocPublisher != NULL){
                if (zmsg_send(&msgTer, coreContext->inprocPublisher) != 0){
                    igsAgent_error(agent, "Could not publish output %s using inproc\n",iop->name);
                    zmsg_destroy(&msgTer);
                    result = IGS_FAILURE;
                }
            }
        }else{
            zmsg_destroy(&msg);
            igsAgent_warn(agent, "agent not started : could not publish output %s to the network (published to agents in same process only)", iop->name);
        }
        //4- distribute publication message to other agents inside our context without using network
        free(zmsg_popstr(msgQuater)); //remove composite uuid/iop name
        zmsg_pushstr(msgQuater, iop->name); //replace it by simple iop name
        //Generate a temporary fake remote agent, containing only
        //necessary information for handlePublicationFromRemoteAgent.
        igs_remote_agent_t *fakeRemote = calloc(1, sizeof(igs_remote_agent_t));
        fakeRemote->context = coreContext;
        fakeRemote->name = agent->name;
        model_readWriteUnlock(); //to avoid deadlock inside handlePublicationFromRemoteAgent
        handlePublicationFromRemoteAgent(msgQuater, fakeRemote);
        zmsg_destroy(&msgQuater);
        free(fakeRemote);
        
    }else{
        if (agent->isWholeAgentMuted){
            igsAgent_debug(agent, "Should publish output %s but the agent has been muted",iop->name);
        }
        if(iop->is_muted){
            igsAgent_debug(agent, "Should publish output %s but it has been muted",iop->name);
        }
        if(agent->context->isFrozen == true){
            igsAgent_debug(agent, "Should publish output %s but the agent has been frozen",iop->name);
        }
    }
    return result;
}

int network_timerCallback (zloop_t *loop, int timer_id, void *arg){
    IGS_UNUSED(loop)
    IGS_UNUSED(timer_id)
    igs_timer_t *timer = (igs_timer_t *)arg;
    timer->cb(timer->timerId, timer->myData);
    return 1;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_observeBus(igs_BusMessageIncoming cb, void *myData){
    core_initContext();
    assert(cb);
    igs_zyre_callback_t *newCb = calloc(1, sizeof(igs_zyre_callback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(coreContext->zyreCallbacks, newCb);
}

igs_result_t igs_startWithDevice(const char *networkDevice, unsigned int port){
    core_initAgent();
    assert(networkDevice);
    assert(port > 0);
    
    if (coreContext->networkActor != NULL){
        //Agent is active : need to stop it first
        igs_stop();
    }
    
    coreContext->networkDevice = strndup(networkDevice, IGS_NETWORK_DEVICE_LENGTH);
    
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
            coreContext->ipAddress = strndup(ziflist_address (iflist), IGS_IP_ADDRESS_LENGTH);
            igs_info("Starting with ip address %s and port %d on device %s", coreContext->ipAddress, port, networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (coreContext->ipAddress == NULL){
        igs_error("IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        igs_stop();
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

igs_result_t igs_startWithIP(const char *ipAddress, unsigned int port){
    assert(ipAddress);
    assert(port > 0);
    core_initAgent();
    
    if (coreContext->networkActor != NULL){
        //Agent is already active : need to stop it first
        igs_stop();
    }
    
    coreContext->ipAddress = strndup(ipAddress, IGS_IP_ADDRESS_LENGTH);
    
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
            coreContext->networkDevice = strndup(name, IGS_NETWORK_DEVICE_LENGTH);
            igs_info("Starting with ip address %s and port %d on device %s", ipAddress, port, coreContext->networkDevice);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (coreContext->networkDevice == NULL){
        igs_error("Device name could not be determined for IP address %s : our agent will NOT start", ipAddress);
        igs_stop();
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

igs_result_t igs_startWithDeviceOnBroker(const char *networkDevice, const char *brokerEndpoint){
    core_initAgent();
    assert(networkDevice);
    assert(brokerEndpoint);
    //TODO: manage a list of brokers instead of just one
    
    if (coreContext->networkActor != NULL){
        //Agent is already active : need to stop it first
        igs_stop();
    }
    
    coreContext->brokerEndPoint = strndup(brokerEndpoint, IGS_IP_ADDRESS_LENGTH);
    coreContext->networkDevice = strndup(networkDevice, IGS_NETWORK_DEVICE_LENGTH);
    
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
            coreContext->ipAddress = strndup(ziflist_address (iflist), IGS_IP_ADDRESS_LENGTH);
            igs_info("Starting with ip address %s on device %s with broker %s",
                      coreContext->ipAddress,
                      networkDevice,
                      coreContext->brokerEndPoint);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (coreContext->ipAddress){
        igs_error("IP address could not be determined on device %s : our agent will NOT start", networkDevice);
        igs_stop();
        return IGS_FAILURE;
    }
#if !TARGET_OS_IOS
    license_readLicense(coreContext);
#endif
    coreContext->network_zyrePort = 0;
    initLoop(coreContext);
    assert (coreContext->networkActor);
    return 1;
}


void igs_stop(){
    core_initContext();
    if (coreContext->networkActor != NULL){
        //interrupting and destroying ingescape thread and zyre layer
        //this will also clean all agent->subscribers
        if (!coreContext->externalStop){
            //NB: if agent has been forcibly stopped, actor is already stopping
            //and this command would deadlock.
            zstr_send(coreContext->networkActor, "STOP_LOOP");
        }
        zactor_destroy(&coreContext->networkActor);
#if (defined WIN32 || defined _WIN32)
        // On Windows, if we don't call zsys_shutdown, the application will crash on exit
        // (WSASTARTUP assertion failure)
        // NB: Monitoring also uses a zactor, we cannot call zsys_shutdown() when it is running
        if (coreContext->monitor == NULL){
            igs_debug("calling zsys_shutdown");
            zsys_shutdown();
        }
#endif
        igs_info("ingescape stopped properly");
    }else{
        igs_debug("ingescape already stopped");
    }
    
    if (coreContext->networkDevice != NULL){
        free(coreContext->networkDevice);
        coreContext->networkDevice = NULL;
    }
    if (coreContext->ipAddress != NULL){
        free(coreContext->ipAddress);
        coreContext->ipAddress = NULL;
    }
    if (coreContext->brokerEndPoint != NULL){
        free(coreContext->brokerEndPoint);
        coreContext->brokerEndPoint = NULL;
    }
    
#if !TARGET_OS_IOS
    license_cleanLicense(coreContext);
#endif
}

bool igs_isStarted(){
    core_initContext();
    network_Lock();
    if (coreContext->loop != NULL){
        network_Unlock();
        return true;
    }else{
        network_Unlock();
        return false;
    }
}

void igsAgent_setAgentName(igs_agent_t *agent, const char *name){
    assert(agent);
    assert(name && strlen(name) > 0);
    if (streq(agent->name, name)){
        //nothing to do
        return;
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
        igsAgent_warn(agent, "Spaces are not allowed in agent name: '%s' has been changed to '%s'", name, n);
    }
    char *previous = agent->name;
    agent->name = n;
    agent->network_needToSendDefinitionUpdate = true;
    igsAgent_debug(agent, "Agent (%s) name changed from %s to %s", agent->uuid, previous, agent->name);
    free(previous);
}


char *igsAgent_getAgentName(igs_agent_t *agent){
    assert(agent);
    if (agent->name == NULL){
        return NULL;
    }else{
        return strdup(agent->name);
    }
}


igs_result_t igs_freeze(void){
    core_initContext();
    if(coreContext->isFrozen == false){
        if ((coreContext != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "FROZEN");
                zmsg_addstr(msg, "1");
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            bus_zyreUnlock();
        }
        coreContext->isFrozen = true;
        igs_freeze_callback_t *elt;
        DL_FOREACH(coreContext->freezeCallbacks,elt){
            elt->callback_ptr(coreContext->isFrozen, elt->myData);
        }
    }
    return IGS_SUCCESS;
}


bool igs_isFrozen(void){
    core_initContext();
    return coreContext->isFrozen;
}


void igs_unfreeze(void){
    core_initContext();
    if(coreContext->isFrozen == true){
        if ((coreContext->networkActor != NULL) && (coreContext->node != NULL)){
            bus_zyreLock();
            igs_agent_t *agent, *tmp;
            HASH_ITER(hh, coreContext->agents, agent, tmp){
                zmsg_t *msg = zmsg_new();
                zmsg_addstr(msg, "FROZEN");
                zmsg_addstr(msg, "0");
                zmsg_addstr(msg, agent->uuid);
                zyre_shout(coreContext->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            bus_zyreUnlock();
        }
        coreContext->isFrozen = false;
        igs_freeze_callback_t *elt;
        DL_FOREACH(coreContext->freezeCallbacks,elt){
            elt->callback_ptr(coreContext->isFrozen, elt->myData);
        }
    }
}


void igs_observeFreeze(igs_freezeCallback cb, void *myData){
    core_initContext();
    if (cb != NULL){
        igs_freeze_callback_t *newCb = calloc(1, sizeof(igs_freeze_callback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(coreContext->freezeCallbacks, newCb);
    }else{
        igs_warn("callback is null");
    }
}


void igsAgent_setAgentState(igs_agent_t *agent, const char *state){
    assert(agent);
    assert(state);
    if (agent->state == NULL || !streq(state, agent->state)){
        if (agent->state != NULL)
            free(agent->state);
        agent->state = strndup(state, IGS_MAX_AGENT_NAME_LENGTH);
        if (agent->context->node != NULL){
            bus_zyreLock();
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "STATE");
            zmsg_addstr(msg, agent->state);
            zmsg_addstr(msg, agent->uuid);
            zyre_shout(agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            bus_zyreUnlock();
        }
    }
}


char *igsAgent_getAgentState(igs_agent_t *agent){
    assert(agent);
    if (agent->state == NULL){
        return NULL;
    }else{
        return strdup(agent->state);
    }
}


void igsAgent_mute(igs_agent_t *agent){
    assert(agent);
    if (!agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = true;
        if ((agent->context->networkActor != NULL) && (agent->context->node != NULL)){
            bus_zyreLock();
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "MUTED");
            zmsg_addstrf(msg, "%i", agent->isWholeAgentMuted);
            zmsg_addstr(msg, agent->uuid);
            zyre_shout(agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            bus_zyreUnlock();
        }
        igs_mute_callback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
}


void igsAgent_unmute(igs_agent_t *agent){
    assert(agent);
    if (agent->isWholeAgentMuted)
    {
        agent->isWholeAgentMuted = false;
        if ((agent->context->networkActor != NULL) && (agent->context->node != NULL)){
            bus_zyreLock();
            zmsg_t *msg = zmsg_new();
            zmsg_addstr(msg, "MUTED");
            zmsg_addstrf(msg, "%i", agent->isWholeAgentMuted);
            zmsg_addstr(msg, agent->uuid);
            zyre_shout(agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            bus_zyreUnlock();
        }
        igs_mute_callback_t *elt;
        DL_FOREACH(agent->muteCallbacks,elt){
            elt->callback_ptr(agent, agent->isWholeAgentMuted, elt->myData);
        }
    }
}


bool igsAgent_isMuted(igs_agent_t *agent){
    assert(agent);
    return agent->isWholeAgentMuted;
}


void igsAgent_observeMute(igs_agent_t *agent, igsAgent_muteCallback cb, void *myData){
    assert(agent);
    assert(cb);
    igs_mute_callback_t *newCb = calloc(1, sizeof(igs_mute_callback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(agent->muteCallbacks, newCb);
}

char* igs_getCommandLine(void){
    core_initContext();
    if (coreContext->commandLine == NULL){
        return NULL;
    }else{
        return strndup(coreContext->commandLine, IGS_COMMAND_LINE_LENGTH);
    }
}

void igs_setCommandLine(const char *line){
    core_initContext();
    assert(line);
    if (coreContext->commandLine != NULL)
        free(coreContext->commandLine);
    coreContext->commandLine = strndup(line, IGS_COMMAND_LINE_LENGTH);
    igs_debug("Command line set to %s", coreContext->commandLine);
}

void igs_setCommandLineFromArgs(int argc, const char * argv[]){
    core_initContext();
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
    char exeFilePath[IGS_MAX_PATH_LENGTH];
#ifdef UNICODE
    WCHAR temp[IGS_MAX_PATH_LENGTH];
    GetModuleFileName(NULL,temp,IGS_MAX_PATH_LENGTH);
    //Conversion in char *
    wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));
#else
    GetModuleFileName(NULL,exeFilePath,IGS_MAX_PATH_LENGTH);
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
    if (coreContext->commandLine != NULL)
        free(coreContext->commandLine);
    coreContext->commandLine = strndup(cmd, IGS_COMMAND_LINE_LENGTH);
}

void igsAgent_setRequestOutputsFromMappedAgents(igs_agent_t *agent, bool notify){
    agent->network_requestOutputsFromMappedAgents = notify;
}

bool igsAgent_getRequestOutputsFromMappedAgents(igs_agent_t *agent){
    return agent->network_requestOutputsFromMappedAgents;
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

void igs_observeExternalStop(igs_externalStopCallback cb, void *myData){
    core_initAgent();
    assert(cb);
    igs_external_stop_calback_t *newCb = calloc(1, sizeof(igs_external_stop_calback_t));
    newCb->callback_ptr = cb;
    newCb->myData = myData;
    DL_APPEND(coreContext->externalStopCalbacks, newCb);
}

void igs_setDiscoveryInterval(unsigned int interval){
    core_initContext();
    if (coreContext->networkActor != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_set_interval(coreContext->node, interval);
        bus_zyreUnlock();
    }
    coreContext->network_discoveryInterval = interval;
}

void igs_setTimeout(unsigned int duration){
    core_initContext();
    if (coreContext->networkActor != NULL && coreContext->node != NULL){
        bus_zyreLock();
        zyre_set_expired_timeout(coreContext->node, duration);
        bus_zyreUnlock();
    }
    coreContext->network_agentTimeout = duration;
}

void igs_setPublishingPort(unsigned int port){
    core_initContext();
    if (coreContext->networkActor != NULL && coreContext->publisher != NULL){
        igs_error("agent is already started : stop it first to change its publishing port");
        return;
    }
    coreContext->network_publishingPort = port;
}

void igs_setLogStreamPort(unsigned int port){
    core_initContext();
    if (coreContext->networkActor != NULL && coreContext->logger != NULL){
        igs_error("agent is already started : stop it first to change its logger port");
        return;
    }
    coreContext->network_logStreamPort = port;
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igs_setIpcFolderPath(char *path){
    core_initContext();
    assert(path);
    if (coreContext->network_ipcFolderPath == NULL || !streq(path, coreContext->network_ipcFolderPath)){
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

const char* igs_getIpcFolderPath(){
    core_initContext();
    return strdup(coreContext->network_ipcFolderPath);
}
#endif

void igs_setAllowInproc(bool allow){
    core_initContext();
    coreContext->network_allowInproc = allow;
}

bool igs_getAllowInproc(){
    core_initContext();
    return coreContext->network_allowInproc;
}

void igs_setAllowIpc(bool allow){
    core_initContext();
    coreContext->network_allowIpc = allow;
}

bool igs_getAllowIpc(){
    core_initContext();
    return coreContext->network_allowIpc;
}

void igs_setHighWaterMarks(int hwmValue){
    core_initContext();
    if (hwmValue < 0){
        igs_error("HWM value must be zero or higher");
        return;
    }
    if (coreContext->networkActor != NULL && coreContext->publisher != NULL){
        zsock_set_sndhwm(coreContext->publisher, hwmValue);
        zsock_set_sndhwm(coreContext->logger, hwmValue);
        igs_zyre_peer_t *tmp = NULL, *peer = NULL;
        HASH_ITER(hh, coreContext->zyrePeers, peer, tmp){
            zsock_set_rcvhwm(peer->subscriber, hwmValue);
        }
    }
    coreContext->network_hwmValue = hwmValue;
}

void igs_raiseSocketsLimit(){
    core_initContext();
#if defined __unix__ || defined __APPLE__ || defined __linux__
    if (coreContext->network_shallRaiseFileDescriptorsLimit){
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
                    coreContext->network_shallRaiseFileDescriptorsLimit = false;
                }
            }
        }
    }
#else
    igs_info("this function has no effect on non-UNIX systems");
#endif
}

zsock_t* igs_getPipeToIngescape(void){
    core_initContext();
    if (coreContext->networkActor != NULL){
        return zactor_sock(coreContext->networkActor);
    }else{
        igs_warn("ingescape is not started yet");
        return NULL;
    }
}

int igs_timerStart(size_t delay, size_t times, igs_timerCallback cb, void *myData){
    core_initContext();
    if (coreContext->loop == NULL){
        igs_error("Ingescape must be started to create a timer");
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
    core_initContext();
    if (coreContext->loop == NULL){
        igs_error("Ingescape must be started to destroy a timer");
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
