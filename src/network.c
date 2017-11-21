//
//  mtic_network.c
//
//  Created by Stephane Vales on 10/06/2016.
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

/**
  * \file ../../src/include/mastic.h
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
#endif
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
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#include "mastic_private.h"

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
#else
    #ifdef ANDROID
    #include "ifaddrs.h"
    #endif
#endif


//global flags
bool verboseMode = true;
bool isFrozen = false;
bool agentCanBeFrozen = false;
bool isWholeAgentMuted = false;
bool mtic_Interrupted = false;
bool network_NotifyMappedAgents = true;


//global parameters
//prefixes for sending definitions and mappings through zyre
static const char *definitionPrefix = "EXTERNAL_DEFINITION#";
static const char *mappingPrefix = "EXTERNAL_MAPPING#";
static const char *loadMappingPrefix = "LOAD_THIS_MAPPING#";
static const char *loadDefinitionPrefix = "LOAD_THIS_DEFINITION#";
#define AGENT_NAME_LENGTH 256
#define COMMAND_LINE_LENGTH 2048
char agentName[AGENT_NAME_LENGTH] = AGENT_NAME_DEFAULT;
char agentState[AGENT_NAME_LENGTH] = "";
char commandLine[COMMAND_LINE_LENGTH] = "";

typedef struct freezeCallback {
    mtic_freezeCallback callback_ptr;
    void *myData;
    struct freezeCallback *prev;
    struct freezeCallback *next;
} freezeCallback_t;

typedef struct zyreCallback {
    network_zyreIncoming callback_ptr;
    void *myData;
    struct zyreCallback *prev;
    struct zyreCallback *next;
} zyreCallback_t;

//zyre agents storage
#define NAME_BUFFER_SIZE 256
typedef struct zyreAgent {
    char peerId[NAME_BUFFER_SIZE];
    char name[NAME_BUFFER_SIZE];
    subscriber_t *subscriber;
    int reconnected;
    bool hasJoinedPrivateChannel;
    UT_hash_handle hh;
} zyreAgent_t;

bool network_needToSendDefinitionUpdate = false;
bool network_needToUpdateMapping = false;

//we manage agent data as a global variables inside the network module for now
zyreloopElements_t *agentElements = NULL;
subscriber_t *subscribers = NULL;
freezeCallback_t *freezeCallbacks = NULL;
zyreCallback_t *zyreCallbacks = NULL;
zyreAgent_t *zyreAgents = NULL;

////////////////////////////////////////////////////////////////////////
// INTERNAL API
////////////////////////////////////////////////////////////////////////
int subscribeToPublisherOutput(subscriber_t *subscriber, const char *outputName)
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
            mtic_debug("subscribe to agent %s output %s\n",agentName,outputName);
            zsock_set_subscribe(subscriber->subscriber, outputName);
            mappingFilter_t *f = calloc(1, sizeof(mappingFilter_t));
            strncpy(f->filter, outputName, MAX_FILTER_SIZE);
            DL_APPEND(subscriber->mappingsFilters, f);
        }else{
            //printf("\n****************\nFILTER BIS %s - %s\n***************\n", subscriber->agentName, outputName);
        }
        return 1;
    }
    return -1;
}

int unsubscribeToPublisherOutput(subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        mappingFilter_t *filter = NULL;
        DL_FOREACH(subscriber->mappingsFilters, filter){
            if (strcmp(filter->filter, outputName) == 0){
                mtic_debug("unsubscribe to agent %s output %s\n",agentName,outputName);
                zsock_set_unsubscribe(subscriber->subscriber, outputName);
                DL_DELETE(subscriber->mappingsFilters, filter);
                break;
            }
        }
        return 1;
    }
    return -1;
}

//Timer callback to send MAPPED notification for agents we subscribed to
int triggerMappingNotificationToNewcomer(zloop_t *loop, int timer_id, void *arg){
    subscriber_t *subscriber = (subscriber_t *) arg;
    if (subscriber->mappedNotificationToSend){
        zyre_whispers(agentElements->node, subscriber->agentPeerId, "MAPPED");
        subscriber->mappedNotificationToSend = false;
    }
    return 0;
}

int network_manageSubscriberMapping(subscriber_t *subscriber){
    //get mapping elements for this subscriber
    mapping_element_t *el, *tmp;
    if (mtic_internal_mapping != NULL){
        HASH_ITER(hh, mtic_internal_mapping->map_elements, el, tmp){
            if (strcmp(subscriber->agentName, el->agent_name)==0 || strcmp(el->agent_name, "*") == 0){
                //mapping element is compatible with subscriber name
                //check if we find a compatible output in subscriber definition
                agent_iop *foundOutput = NULL;
                if (subscriber->definition != NULL){
                    HASH_FIND_STR(subscriber->definition->outputs_table, el->output_name, foundOutput);
                }
                //check if we find a valid input in our own definition
                agent_iop *foundInput = NULL;
                if (mtic_internal_definition != NULL){
                    HASH_FIND_STR(mtic_internal_definition->inputs_table, el->input_name, foundInput);
                }
                //check type compatibility between input and output value types
                //including implicit conversions
                if (foundOutput != NULL && foundInput != NULL && mapping_checkCompatibilityInputOutput(foundInput, foundOutput)){
                    //we have validated input, agent and output names : we can map
                    //NOTE: the call below may happen several times if our agent uses
                    //the external agent ouput on several of its inputs. This should not have any consequence.
                    subscribeToPublisherOutput(subscriber, el->output_name);
                    //mapping was successful : we set timer to notify remote agent if not already done
                    if (!subscriber->mappedNotificationToSend && network_NotifyMappedAgents){
                        subscriber->mappedNotificationToSend = true;
                        zloop_timer(agentElements->loop, 500, 1, triggerMappingNotificationToNewcomer, (void *)subscriber);
                    }
                }
                //NOTE: we do not clean subscriptions here because we cannot check
                //an output is not used in another mapping element
            }
        }
    }
    return 0;
}

void sendDefinitionToAgent(const char *peerId, const char *definition)
{
    if(peerId != NULL &&  definition != NULL)
    {
        if(agentElements->node != NULL)
        {
            zyre_whispers(agentElements->node, peerId, "%s%s", definitionPrefix, definition);
        } else {
            mtic_debug("Error : could not send our definition to %s : our agent is not connected\n",peerId);
        }
    }
}

void sendMappingToAgent(const char *peerId, const char *mapping)
{
    if(peerId != NULL &&  mapping != NULL)
    {
        if(agentElements->node != NULL)
        {
            zyre_whispers(agentElements->node, peerId, "%s%s", mappingPrefix, mapping);
        } else {
            mtic_debug("Error : could not send our mapping to %s : our agent is not connected\n",peerId);
        }
    }
}

void network_cleanAndFreeSubscriber(subscriber_t *subscriber){
    mtic_debug("cleaning subscription to %s\n", subscriber->agentName);
    // clean the agent definition
    if(subscriber->definition != NULL){
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
    zloop_poller_end(agentElements->loop , subscriber->pollItem);
    zsock_destroy(&subscriber->subscriber);
    free((char*)subscriber->agentName);
    free((char*)subscriber->agentPeerId);
    free(subscriber->pollItem);
    free(subscriber->subscriber);
    subscriber->subscriber = NULL;
    HASH_DEL(subscribers, subscriber);
    free(subscriber);
    subscriber = NULL;
//    int n = HASH_COUNT(subscribers);
//    mtic_debug("%d subscribers in the list\n", n);
//    subscriber_t *s, *tmps;
//    HASH_ITER(hh, subscribers, s, tmps){
//        mtic_debug("\tsubscriber : %s - %s\n", s->agentName, s->agentPeerId);
//    }
}

////////////////////////////////////////////////////////////////////////
// ZMQ callbacks
////////////////////////////////////////////////////////////////////////

//manage messages from the parent thread
int manageParent (zloop_t *loop, zmq_pollitem_t *item, void *arg){

    if (item->revents & ZMQ_POLLIN)
    {
        zmsg_t *msg = zmsg_recv ((zsock_t *)item->socket);
        if (!msg){
            printf("Error while reading message from main thread... exiting.");
            exit(EXIT_FAILURE); //Interrupted
        }
        char *command = zmsg_popstr (msg);
        if (streq (command, "$TERM")){
            return -1;
        } else {
            //nothing to do so far
        }

        free (command);
        zmsg_destroy (&msg);
    }
    return 0;
}

//manage incoming messages from one of the publisher agents we subscribed to
int manageSubscription (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    //get subscriber id
    char *subscriberPeerId = (char *)arg;

    if (item->revents & ZMQ_POLLIN && strlen(subscriberPeerId) > 0)
    {
        subscriber_t * foundSubscriber = NULL;

        // Try to find the subscriber object
        HASH_FIND_STR(subscribers, subscriberPeerId, foundSubscriber);
//        int n = HASH_COUNT(subscribers);
//        mtic_debug("%d subscribers in the list\n", n);
//        subscriber_t *s, *tmp;
//        HASH_ITER(hh, subscribers, s, tmp){
//            mtic_debug("\tsubscriber : %s - %s\n", s->agentName, s->agentPeerId);
//        }
        if(foundSubscriber != NULL)
        {
            zmsg_t *msg = zmsg_recv(foundSubscriber->subscriber);
            // Message must contain 2 elements
            // 1 : output name
            // 2 : value of the output
            if(zmsg_size(msg) == 2 && isFrozen == false)
            {
                char * output = zmsg_popstr(msg);
                
                //NOTE: we rely on the external agent definition exclusively to get
                //the output type. This type is then used to convert the received data.
                definition * externalDefinition = foundSubscriber->definition;

                if(externalDefinition != NULL){
                    agent_iop * foundOutput = NULL;
                    if (externalDefinition->outputs_table != NULL){
                        HASH_FIND_STR(externalDefinition->outputs_table, output, foundOutput);
                    }
                    if(foundOutput != NULL){
                        zframe_t *frame = NULL;
                        void *data = NULL;
                        long size = 0;
                        char * value = NULL;
                        //get data before iterating to all the mapping elements using it
                        if (foundOutput->value_type == DATA_T){
                            frame = zmsg_pop(msg);
                            data = zframe_data(frame);
                            size = zframe_size(frame);
                        }else{
                            value = zmsg_popstr(msg);
                        }
                        //try to find mapping elements matching with this subscriber's output
                        //and update mapped input(s) value accordingly
                        mapping_element_t *elmt, *tmp;
                        HASH_ITER(hh, mtic_internal_mapping->map_elements, elmt, tmp) {
                            if (strcmp(elmt->agent_name, foundSubscriber->agentName) == 0
                                && strcmp(elmt->output_name, output) == 0){
                                //we have a match on emitting agent name and its ouput name :
                                //still need to check the targeted input existence in our definition
                                agent_iop *foundInput = NULL;
                                if (mtic_internal_definition->inputs_table != NULL){
                                    HASH_FIND_STR(mtic_internal_definition->inputs_table, elmt->input_name, foundInput);
                                }
                                if (foundInput == NULL){
                                    mtic_debug("manageSubscription : input %s missing in our definition for use in a mapping with %s.%s\n",
                                               elmt->input_name,
                                               elmt->agent_name,
                                               elmt->output_name);
                                }else{
                                    //we have a fully matching mapping element : write from received output to our input
                                    if (foundOutput->value_type == DATA_T){
                                        //If the remote output is data, we write our input as data : no type conversion
                                        //mtic_writeInputAsData will reject writing if our input is not data
                                        mtic_writeInputAsData(elmt->input_name, data, size);
                                    }else if (foundOutput->value_type == IMPULSION_T){
                                        //If the remote output is impulsion, we write our input as impulsion :
                                        //this is an implicit type conversion and the received value is not used
                                        mtic_writeInputAsImpulsion(elmt->input_name);
                                    }else{
                                        //conversion from string to actual type is achieved in mtic_writeInput
                                        mtic_writeInput(elmt->input_name, value, 0); //size is not used for these types
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
                    }else{
                        mtic_debug("manageSubscription : output %s missing in %s definition for use in a mapping element\n", output, foundSubscriber->agentName);
                    }
                }else{
                    mtic_debug("manageSubscription : no definition received for %s yet\n", foundSubscriber->agentName);
                }
                free(output);
            } else {
                //ignore the message
                if(isFrozen == true){
                    mtic_debug("Message received from agent %s but all traffic in our agent has been frozen\n", foundSubscriber->agentName);
                }
            }
            zmsg_destroy(&msg);
        }else{
            mtic_debug("manageSubscription : could not find subscriber for agent %s\n", subscriberPeerId);
        }
    }
    return 0;
}

//manage messages received on the bus
int manageZyreIncoming (zloop_t *loop, zmq_pollitem_t *item, void *arg){
    zyre_t *node = agentElements->node;
    if (item->revents & ZMQ_POLLIN)
    {
        zyre_event_t *zyre_event = zyre_event_new (node);
        const char *event = zyre_event_type(zyre_event);
        const char *peer = zyre_event_peer_uuid(zyre_event);
        const char *name = zyre_event_peer_name (zyre_event);
        const char *address = zyre_event_peer_addr (zyre_event);
        zhash_t *headers = zyre_event_headers (zyre_event);
        const char *group = zyre_event_group (zyre_event);
        zmsg_t *msg = zyre_event_msg (zyre_event);
        zmsg_t *msgDuplicate = zmsg_dup(msg);
        
        //parse event
        if (streq (event, "ENTER")){
            mtic_debug("->%s has entered the network with peer id %s and address %s\n", name, peer, address);
            zyreAgent_t *zagent = NULL;
            HASH_FIND_STR(zyreAgents, peer, zagent);
            if (zagent == NULL){
                zagent = calloc(1, sizeof(zyreAgent_t));
                zagent->reconnected = 0;
                zagent->subscriber = NULL;
                zagent->hasJoinedPrivateChannel = false;
                strncpy(zagent->peerId, peer, NAME_BUFFER_SIZE);
                HASH_ADD_STR(zyreAgents, peerId, zagent);
            }else{
                //Agent already exists, we set its reconnected flag
                //(this is used below to avoid agent destruction on EXIT received after timeout)
                zagent->reconnected++;
            }
            strncpy(zagent->name, name, NAME_BUFFER_SIZE);
            assert(headers);
            char *k;
            const char *v;
            zlist_t *keys = zhash_keys(headers);
            size_t s = zlist_size(keys);
            if (s > 0){
                mtic_debug("@%s's headers are:\n", name);
            }
            while ((k = (char *)zlist_pop(keys))) {
                v = zyre_event_header (zyre_event,k);
                mtic_debug("\t%s -> %s\n", k, v);

                // we extract the publisher adress to subscribe to from the zyre message header
                if(strncmp(k,"publisher", strlen("publisher")) == 0)
                {
                    char endpointAddress[128];
                    strncpy(endpointAddress, address, 128);

                    // IP adress extraction 
                    char *insert = endpointAddress + strlen(endpointAddress);
                    bool extractOK = true;
                    while (*insert != ':'){
                        insert--;
                        if (insert == endpointAddress){
                            mtic_debug("Error: could not extract port from address %s", address);
                            extractOK = false;
                            break;
                        }
                    }

                    if (extractOK){
                        *(insert + 1) = '\0';
                        strcat(endpointAddress, v);
                        //we found a possible publisher to subscribe to
                        subscriber_t *subscriber;
                        HASH_FIND_STR(subscribers, peer, subscriber);
                        
                        if (subscriber != NULL){
                            //we have a reconnection with same peerId
                            mtic_debug("Peer id %s was connected before with agent name %s : reset and recreate subscription\n", peer, subscriber->agentName);
                            HASH_DEL(subscribers, subscriber);
                            zloop_poller_end(agentElements->loop , subscriber->pollItem);
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
                            free(subscriber);
                            subscriber = NULL;
                        }
                        subscriber = calloc(1, sizeof(subscriber_t));
                        zagent->subscriber = subscriber;
                        subscriber->agentName = strdup(name);
                        subscriber->agentPeerId = strdup (peer);
                        subscriber->subscriber = zsock_new_sub(endpointAddress, NULL);
                        assert(subscriber->subscriber);
                        subscriber->definition = NULL;
                        subscriber->mappingsFilters = NULL;
                        HASH_ADD_STR(subscribers, agentPeerId, subscriber);
                        subscriber->pollItem = calloc(1, sizeof(zmq_pollitem_t));
                        subscriber->pollItem->socket = zsock_resolve(subscriber->subscriber);
                        subscriber->pollItem->fd = 0;
                        subscriber->pollItem->events = ZMQ_POLLIN;
                        subscriber->pollItem->revents = 0;
                        zloop_poller (agentElements->loop, subscriber->pollItem, manageSubscription, (void*)subscriber->agentPeerId);
                        zloop_poller_set_tolerant(loop, subscriber->pollItem);
                        mtic_debug("\t\tSubscriber created for %s at %s.\n",subscriber->agentName,endpointAddress);
                    }
                }

                free(k);
            }
            zlist_destroy(&keys);
        } else if (streq (event, "JOIN")){
            mtic_debug("+%s has joined %s\n", name, group);
            if (streq(group, CHANNEL)){
                //definition is sent to every newcomer on the channel (whether it is a mastic agent or not)
                char * definitionStr = NULL;
                definitionStr = parser_export_definition(mtic_internal_definition);
                if (definitionStr != NULL){
                    sendDefinitionToAgent(peer, definitionStr);
                    free(definitionStr);
                }
                //and so is our mapping
                char *mappingStr = NULL;
                mappingStr = parser_export_mapping(mtic_internal_mapping);
                if (mappingStr != NULL){
                    sendMappingToAgent(peer, mappingStr);
                    free(mappingStr);
                }
                //we also send our frozen and muted states
                if (isWholeAgentMuted){
                    zyre_whispers(agentElements->node, peer, "MUTED=1");
                }else{
                    
                }
                if (isFrozen){
                    zyre_whispers(agentElements->node, peer, "FROZEN=1");
                }
                if (mtic_internal_definition != NULL){
                    struct agent_iop *current_iop, *tmp_iop;
                    HASH_ITER(hh, mtic_internal_definition->outputs_table, current_iop, tmp_iop) {
                        if (current_iop->is_muted && current_iop->name != NULL){
                            zyre_whispers(agentElements->node, peer, "OUTPUT_MUTED %s", current_iop->name);
                        }
                    }
                }
                
                
                zyreAgent_t *zagent = NULL;
                HASH_FIND_STR(zyreAgents, peer, zagent);
                if (zagent != NULL){
                    zagent->hasJoinedPrivateChannel = true;
                }
            }
        } else if (streq (event, "LEAVE")){
            mtic_debug("-%s has left %s\n", name, group);
        } else if (streq (event, "SHOUT")){
            //nothing to do so far
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
                definition *newDefinition = parser_loadDefinition(strDefinition);
                subscriber_t *subscriber;
                HASH_FIND_STR(subscribers, peer, subscriber);
                
                if (newDefinition != NULL && newDefinition->name != NULL && subscriber != NULL){
                    // Look if this agent already has a definition
                    if(subscriber->definition != NULL) {
                        mtic_debug("definition already exists for agent %s : new definition will overwrite the previous one...\n", name);
                        definition_freeDefinition(subscriber->definition);
                        subscriber->definition = NULL;
                    }
                    
                    mtic_debug("Store definition for agent %s\n", name);
                    subscriber->definition = newDefinition;
                    //Check the involvement of this new agent and its definition in our mapping and update subscriptions
                    //we check here because subscriber definition is required to handle received data
                    network_manageSubscriberMapping(subscriber);
                }else{
                    mtic_debug("ERROR: definition is NULL or has no name for agent %s\n", name);
                    if(newDefinition != NULL) {
                        definition_freeDefinition(newDefinition);
                        newDefinition = NULL;
                    }
                }
                free(strDefinition);
            }
            //check if message is an EXTERNAL mapping
            else if(strlen(message) > strlen(mappingPrefix) && strncmp (message, mappingPrefix, strlen(mappingPrefix)) == 0)
            {
                // Extract mapping from message
                char* strMapping = calloc(strlen(message)- strlen(mappingPrefix)+1, sizeof(char));
                memcpy(strMapping, &message[strlen(mappingPrefix)], strlen(message)- strlen(mappingPrefix));
                strMapping[strlen(message)- strlen(mappingPrefix)] = '\0';
                
                // Load mapping from string content
                mapping_t *newMapping = parser_LoadMap(strMapping);
                subscriber_t *subscriber;
                HASH_FIND_STR(subscribers, peer, subscriber);
                
                if (newMapping != NULL && newMapping->name != NULL && subscriber != NULL){
                    // Look if this agent already has a mapping
                    if(subscriber->mapping != NULL){
                        mtic_debug("mapping already exists for agent %s : new mapping will overwrite the previous one...\n", name);
                        mapping_freeMapping(subscriber->mapping);
                        subscriber->mapping = NULL;
                    }
                    
                    mtic_debug("Store mapping for agent %s\n", name);
                    subscriber->mapping = newMapping;
                }else{
                    mtic_debug("ERROR: mapping is NULL or has no name for agent %s\n", name);
                    if(subscriber != NULL && subscriber->mapping != NULL) {
                        mapping_freeMapping(subscriber->mapping);
                        subscriber->mapping = NULL;
                    }
                    if(newMapping != NULL) {
                        mapping_freeMapping(newMapping);
                        newMapping = NULL;
                    }
                }
                free(strMapping);
            }
            //check if message is DEFINITION TO BE LOADED
            else if (strlen(message) > strlen(loadDefinitionPrefix) && strncmp (message, loadDefinitionPrefix, strlen(loadDefinitionPrefix)) == 0)
            {
                // Extract definition from message
                char* strDefinition = calloc(strlen(message)- strlen(definitionPrefix)+1, sizeof(char));
                memcpy(strDefinition, &message[strlen(definitionPrefix)], strlen(message)- strlen(definitionPrefix));
                strDefinition[strlen(message)- strlen(definitionPrefix)] = '\0';
                
                //load definition
                mtic_loadDefinition(strDefinition);
                //recheck mapping towards our new definition
                subscriber_t *s, *tmp;
                HASH_ITER(hh, subscribers, s, tmp){
                    network_manageSubscriberMapping(s);
                }
                free(strDefinition);
            }
            //check if message is MAPPING TO BE LOADED
            else if (strlen(message) > strlen(loadMappingPrefix) && strncmp (message, loadMappingPrefix, strlen(loadMappingPrefix)) == 0)
            {
                // Extract mapping from message
                char* strMapping = calloc(strlen(message)- strlen(definitionPrefix)+1, sizeof(char));
                memcpy(strMapping, &message[strlen(loadMappingPrefix)], strlen(message)- strlen(loadMappingPrefix));
                strMapping[strlen(message)- strlen(loadMappingPrefix)] = '\0';
                
                // Load definition from string content
                mapping_t *m = parser_LoadMap(strMapping);
                if (m != NULL){
                    if (mtic_internal_mapping != NULL){
                        mapping_freeMapping(mtic_internal_mapping);
                    }
                    mtic_internal_mapping = m;
                    //check and activate mapping
                    subscriber_t *s, *tmp;
                    HASH_ITER(hh, subscribers, s, tmp){
                        network_manageSubscriberMapping(s);
                    }
                }
                free(strMapping);
            }else{
                //other supported messages
                if (strlen("MAPPED") == strlen(message) && strncmp (message, "MAPPED", strlen("MAPPED")) == 0){
                    mtic_debug("Mapping notification received from %s\n", name);
                    //TODO: optimize to rewrite only outputs actually involved in the mapping
                    long nbOutputs = 0;
                    char **outputsList = NULL;
                    outputsList = mtic_getOutputsList(&nbOutputs);
                    int i = 0;
                    for (i = 0; i < nbOutputs; i++){
                        agent_iop * found_iop = model_findIopByName(outputsList[i],OUTPUT_T);
                        if (found_iop != NULL
                            && found_iop->value_type != IMPULSION_T
                            && found_iop->value_type != DATA_T){
                            //we exclude impulsions and data from the update flow to avoid
                            //false information (for impulsions) and data storage issues (for data)
                            network_publishOutput(outputsList[i]);
                            free(outputsList[i]);
                        }
                    }
                    free(outputsList);
                }else if (strlen("DIE") == strlen(message) && strncmp (message, "DIE", strlen("DIE")) == 0){
                    //set flag for parent process
                    mtic_Interrupted = true;
                    free(message);
                    //send interruptin signal to parent process
                    #ifdef _WIN32
                    //TODO for windows
                    #else
                    kill(getppid(), SIGINT);
                    #endif
                    //stop our zyre loop by returning -1
                    return -1;
                }else if (strlen("CLEAR_MAPPING") == strlen(message) && strncmp (message, "CLEAR_MAPPING", strlen("CLEAR_MAPPING")) == 0){
                    mtic_clearMapping();
                }else if (strlen("FREEZE") == strlen(message) && strncmp (message, "FREEZE", strlen("FREEZE")) == 0){
                    mtic_freeze();
                }else if (strlen("UNFREEZE") == strlen(message) && strncmp (message, "UNFREEZE", strlen("UNFREEZE")) == 0){
                    mtic_unfreeze();
                }else if (strlen("MUTE_ALL") == strlen(message) && strncmp (message, "MUTE_ALL", strlen("MUTE_ALL")) == 0){
                    mtic_mute();
                }else if (strlen("UNMUTE_ALL") == strlen(message) && strncmp (message, "UNMUTE_ALL", strlen("UNMUTE_ALL")) == 0){
                    mtic_unmute();
                }else if ((strncmp (message, "MUTE ", strlen("MUTE ")) == 0) && (strlen(message) > strlen("MUTE ")+1)){
                    char *subStr = message + strlen("MUTE") + 1;
                    mtic_muteOutput(subStr);
                }else if ((strncmp (message, "UNMUTE ", strlen("UNMUTE ")) == 0) && (strlen(message) > strlen("UNMUTE ")+1)){
                    char *subStr = message + strlen("UNMUTE") + 1;
                    mtic_unmuteOutput(subStr);
                }else if ((strncmp (message, "SET_INPUT ", strlen("SET_INPUT ")) == 0) && (strlen(message) > strlen("SET_INPUT ")+1)){
                    char *subStr = message + strlen("SET_INPUT") + 1;
                    char *name, *value;
                    name = strtok (subStr," ");
                    value = strtok (NULL," ");
                    if (name != NULL && value != NULL){
                        mtic_writeInput(name, value, 0);
                    }
                }else if ((strncmp (message, "SET_OUTPUT ", strlen("SET_OUTPUT ")) == 0) && (strlen(message) > strlen("SET_OUTPUT ")+1)){
                    char *subStr = message + strlen("SET_OUTPUT") + 1;
                    char *name, *value;
                    name = strtok (subStr," ");
                    value = strtok (NULL," ");
                    if (name != NULL && value != NULL){
                        mtic_writeOutput(name, value, 0);
                    }
                }else if ((strncmp (message, "SET_PARAMETER ", strlen("SET_PARAMETER ")) == 0) && (strlen(message) > strlen("SET_PARAMETER ")+1)){
                    char *subStr = message + strlen("SET_PARAMETER") + 1;
                    char *name, *value;
                    name = strtok (subStr," ");
                    value = strtok (NULL," ");
                    if (name != NULL && value != NULL){
                        mtic_writeParameter(name, value, 0);
                    }
                }else if ((strncmp (message, "MAP ", strlen("MAP ")) == 0) && (strlen(message) > strlen("MAP ")+1)){
                    char *subStr = message + strlen("MAP") + 1;
                    char *input, *agent, *output;
                    input = strtok (subStr," ");
                    agent = strtok (NULL," ");
                    output = strtok (NULL," ");
                    if (input != NULL && agent != NULL && output != NULL){
                        mtic_addMappingEntry(input, agent, output);
                    }
                }else if ((strncmp (message, "UNMAP ", strlen("UNMAP ")) == 0) && (strlen(message) > strlen("UNMAP ")+1)){
                    char *subStr = message + strlen("UNMAP") + 1;
                    char *input, *agent, *output;
                    input = strtok (subStr," ");
                    agent = strtok (NULL," ");
                    output = strtok (NULL," ");
                    if (input != NULL && agent != NULL && output != NULL){
                        mtic_removeMappingEntryWithName(input, agent, output);
                    }
                }
            }
            free(message);
        } else if (streq (event, "EXIT")){
            mtic_debug("<-%s exited\n", name);
            zyreAgent_t *a = NULL;
            HASH_FIND_STR(zyreAgents, peer, a);
            if (a != NULL){
                if (a->reconnected > 0){
                    //do not clean: we are getting a timemout now whereas
                    //the agent is reconnected
                    a->reconnected--;
                }else{
                    HASH_DEL(zyreAgents, a);
                    free(a);
                    // Try to find the subscriber to destory
                    subscriber_t *subscriber = NULL;
                    HASH_FIND_STR(subscribers, peer, subscriber);
                    if (subscriber != NULL){
                        network_cleanAndFreeSubscriber(subscriber);
                    }
                }
            }
        }
        
        //handle callbacks
        zyreCallback_t *elt;
        DL_FOREACH(zyreCallbacks,elt){
            elt->callback_ptr(zyre_event, elt->myData);
        }
        zmsg_destroy(&msgDuplicate);
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

//Timer callback to (re)send our definition to agents present on the private channel
int triggerDefinitionUpdate(zloop_t *loop, int timer_id, void *arg){
    if (network_needToSendDefinitionUpdate){
        char * definitionStr = NULL;
        definitionStr = parser_export_definition(mtic_internal_definition);
        if (definitionStr != NULL){
            zyreAgent_t *a, *tmp;
            HASH_ITER(hh, zyreAgents, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendDefinitionToAgent(a->peerId, definitionStr);
                }
            }
            free(definitionStr);
        }
        network_needToSendDefinitionUpdate = false;
        //when definition changes, mapping may need to be updated as well
        network_needToUpdateMapping = true;
    }
    return 0;
}

//Timer callback to update and (re)send our mapping to agents on the private channel
int triggerMappingUpdate(zloop_t *loop, int timer_id, void *arg){
    if (network_needToUpdateMapping){
        char *mappingStr = NULL;
        mappingStr = parser_export_mapping(mtic_internal_mapping);
        if (mappingStr != NULL){
            zyreAgent_t *a, *tmp;
            HASH_ITER(hh, zyreAgents, a, tmp){
                if (a->hasJoinedPrivateChannel){
                    sendMappingToAgent(a->peerId, mappingStr);
                }
            }
            free(mappingStr);
        }
        subscriber_t *s, *tmp;
        HASH_ITER(hh, subscribers, s, tmp){
            network_manageSubscriberMapping(s);
        }
        network_needToUpdateMapping = false;
    }
    return 0;
}

static void
initActor (zsock_t *pipe, void *args)
{
    //Ae are (re)starting : we disable the timer flags because
    //all network connections are going to be (re)started.
    //Each agent will be mapped if needed when receiving its definition.
    network_needToSendDefinitionUpdate = false;
    network_needToUpdateMapping = false;

    //start zyre
    agentElements->node = zyre_new (agentName);
    zyre_set_interface(agentElements->node, agentElements->networkDevice);
    zyre_set_port(agentElements->node, agentElements->zyrePort);
    if (agentElements->node == NULL){
        mtic_debug("Error : could not create zyre node... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zyre_start (agentElements->node);
    zyre_join(agentElements->node, CHANNEL);
    zsock_signal (pipe, 0); //notify main thread that we are ready

    //start publisher
    char endpoint[256];
    sprintf(endpoint, "tcp://%s:*", agentElements->ipAddress);
    agentElements->publisher = zsock_new_pub(endpoint);
    strncpy(endpoint, zsock_endpoint(agentElements->publisher), 256);
    char *insert = endpoint + strlen(endpoint) - 1;
    while (*insert != ':' && insert > endpoint) {
        insert--;
    }
    
    //set headers for agent
    zyre_set_header(agentElements->node, "publisher", "%s", insert + 1);
    zyre_set_header(agentElements->node, "canBeFrozen", "%i", agentCanBeFrozen);

#if defined __unix__ || defined __APPLE__
    int ret;
    pid_t pid;
    pid = getpid();
    zyre_set_header(agentElements->node, "pid", "%i", pid);
    
    if (strlen(commandLine) == 0){
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
        ret = readlink("/proc/self/exe", pathbuf, sizeof(pathbuf));
#endif
        if ( ret <= 0 ) {
            mtic_debug("PID %d: proc_pidpath ();\n", pid);
            mtic_debug("    %s\n", strerror(errno));
        } else {
            mtic_debug("proc %d: %s\n", pid, pathbuf);
        }
        zyre_set_header(agentElements->node, "commandline", "%s", pathbuf);
    }else{
        zyre_set_header(agentElements->node, "commandline", "%s", commandLine);
    }
#endif
    
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
    if (strlen(commandLine) == 0){
        //command line was not set manually : we try to get exec path instead

        //Use GetModuleFileName() to get exec path
        WCHAR temp[MAX_PATH];
        GetModuleFileName(NULL,temp,MAX_PATH);
        
        //Conversion in char *
        char exeFilePath[MAX_PATH];
        wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));
        zyre_set_header(agentElements->node, "commandline", "%s", exeFilePath);
    }else{
        zyre_set_header(agentElements->node, "commandline", "%s", commandLine);
    }
    DWORD pid = GetCurrentProcessId();
    zyre_set_header(agentElements->node, "pid", "%i", (int)pid);
#endif


    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    #ifdef _WIN32
        WSACleanup();
    #endif
    zyre_set_header(agentElements->node, "hostname", "%s", hostname);
    //code for Fully Qualified Domain Name if needed someday
//    struct addrinfo hints, *info, *p;
//    int gai_result;
//    memset(&hints, 0, sizeof hints);
//    hints.ai_family = AF_UNSPEC; /*either IPV4 or IPV6*/
//    hints.ai_socktype = SOCK_STREAM;
//    hints.ai_flags = AI_CANONNAME;
//    if ((gai_result = getaddrinfo(hostname, "http", &hints, &info)) != 0) {
//        mtic_debug("getaddrinfo error: %s\n", gai_strerror(gai_result));
//        exit(1);
//    }
//    for(p = info; p != NULL; p = p->ai_next) {
//        mtic_debug("hostname: %s\n", p->ai_canonname);
//    }
//    freeaddrinfo(info);
    
    zmq_pollitem_t zpipePollItem;
    zmq_pollitem_t zyrePollItem;

    //main zmq socket (i.e. main thread)
    void *zpipe = zsock_resolve(pipe);
    if (zpipe == NULL){
        mtic_debug("Error : could not get the pipe descriptor for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zpipePollItem.socket = zpipe;
    zpipePollItem.fd = 0;
    zpipePollItem.events = ZMQ_POLLIN;
    zpipePollItem.revents = 0;

    //zyre socket
    void *zsock = zsock_resolve(zyre_socket (agentElements->node));
    if (zsock == NULL){
        mtic_debug("Error : could not get the zyre socket for polling... exiting.\n");
        exit(EXIT_FAILURE);
    }
    zyrePollItem.socket = zsock;
    zyrePollItem.fd = 0;
    zyrePollItem.events = ZMQ_POLLIN;
    zyrePollItem.revents = 0;

    zloop_t *loop = agentElements->loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, false);

    zloop_poller (loop, &zpipePollItem, manageParent, agentElements);
    zloop_poller_set_tolerant(loop, &zpipePollItem);
    zloop_poller (loop, &zyrePollItem, manageZyreIncoming, agentElements);
    zloop_poller_set_tolerant(loop, &zyrePollItem);
    
    zloop_timer(agentElements->loop, 1000, 0, triggerDefinitionUpdate, NULL);
    zloop_timer(agentElements->loop, 1000, 0, triggerMappingUpdate, NULL);

    zloop_start (loop); //start returns when one of the pollers returns -1

    mtic_debug("agent stopping...\n");

    //clean
    zyreAgent_t *zagent, *tmpa;
    HASH_ITER(hh, zyreAgents, zagent, tmpa){
        HASH_DEL(zyreAgents, zagent);
    }
    subscriber_t *s, *tmps;
    HASH_ITER(hh, subscribers, s, tmps) {
        network_cleanAndFreeSubscriber(s);
    }
    zyre_stop (agentElements->node);
    zclock_sleep (100);
    zyre_destroy (&agentElements->node);
    zsock_destroy(&agentElements->publisher);
    zloop_destroy (&loop);
    assert (loop == NULL);
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

void mtic_debug(const char *fmt, ...)
{
    if(verboseMode)
    {
        va_list ar;
        va_start(ar, fmt);
        
        // Add prefix
        fprintf(stderr, "[Debug] : ");
        
        // Add message
        vfprintf(stderr, fmt,ar);
        
        // Add more information in debug mode
#ifdef DEBUG
        //fprintf(stderr, "           ... in %s (%s, line %d).\n", __func__, __FILE__, __LINE__);
#endif // DEBUG
        
        va_end(ar);
    }
}

int network_publishOutput (const char* output_name)
{
    int result = 0;
    
    agent_iop * found_iop = model_findIopByName(output_name,OUTPUT_T);
    
    if(agentElements != NULL && agentElements->publisher != NULL && found_iop != NULL)
    {
        if(!isWholeAgentMuted && !found_iop->is_muted && found_iop->name != NULL && !isFrozen)
        {
            //FIXME: test return code for zmsg_send
            if (found_iop->value_type == DATA_T){
                void *data = NULL;
                long size = 0;
                mtic_readOutputAsData(output_name, &data, &size);
                //TODO: decide if we should delete the data after use or keep it in memory
                //suggestion: we might add a clearOutputData function available to the developer
                //for use when publishing large size data to free memory after publishing.
                //TODO: document ZMQ high water marks and how to change them
                zmsg_t *msg = zmsg_new();
                zmsg_pushstr(msg, output_name);
                zframe_t *frame = zframe_new(data, size);
                zmsg_append(msg, &frame);
                mtic_debug("publish data %s\n",found_iop->name);
                if (zmsg_send(&msg, agentElements->publisher) != 0){
                    mtic_debug("Error while publishing output %s\n",output_name);
                }
            }else if (found_iop->value_type == IMPULSION_T){
                mtic_debug("publish impulsion %s\n",found_iop->name);
                if (zstr_sendx(agentElements->publisher, found_iop->name, "0", NULL) != 0){
                    mtic_debug("Error while publishing output %s\n",output_name);
                }
                result = 1;
            }else{
                char* str_value = definition_getIOPValueAsString(found_iop);
                if(strlen(str_value) > 0)
                {
                    mtic_debug("publish %s -> %s\n",found_iop->name,str_value);
                    if (zstr_sendx(agentElements->publisher, found_iop->name, str_value, NULL) != 0){
                        mtic_debug("Error while publishing output %s\n",output_name);
                    }
                    result = 1;
                }
                free(str_value);
            }
            
        } else {
            if(found_iop == NULL)
            {
                mtic_debug("Output %s is unknown\n", output_name);
            }
            if (isWholeAgentMuted){
                mtic_debug("Should publish output %s but the agent has been muted\n",found_iop->name);
            }
            if(found_iop->is_muted)
            {
                mtic_debug("Should publish output %s but it has been muted\n",found_iop->name);
            }
            if(isFrozen == true)
            {
                mtic_debug("Should publish output %s but the agent has been frozen\n",found_iop->name);
            }
        }
    }
    
    return result;
}

int network_observeZyre(network_zyreIncoming cb, void *myData){
    if (cb != NULL){
        zyreCallback_t *newCb = calloc(1, sizeof(zyreCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(zyreCallbacks, newCb);
    }else{
        mtic_debug("network_observeZyre: callback is null\n");
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
 * \fn int mtic_startWithDevice(const char *networkDevice, int port)
 * \ingroup startStopKillFct
 * \brief Start an agent on a specific network device and network port.
 * \param networkDevice is the name of the network device (ex: eth0, ens2 ...)
 * \param port is the network port number used
 * \return 1 if ok, else 0.
 */
int mtic_startWithDevice(const char *networkDevice, int port){
    
    if (agentElements != NULL){
        //Agent is already active : need to stop it first
        mtic_stop();
    }
    mtic_Interrupted = false;
    
    agentElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agentElements->networkDevice, networkDevice, NETWORK_DEVICE_LENGTH);
    agentElements->ipAddress[0] = '\0';
    
#if defined (__WINDOWS__)
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
            strncpy(agentElements->ipAddress, ziflist_address (iflist), IP_ADDRESS_LENGTH-1);
            mtic_debug("Connection with ip address %s on device %s\n", agentElements->ipAddress, networkDevice);
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(agentElements->ipAddress) == 0){
        fprintf(stderr, "IP address could not be determined on device %s... exiting.\n", networkDevice);
        exit(EXIT_FAILURE);
    }
    
    agentElements->zyrePort = port;
    agentElements->agentActor = zactor_new (initActor, agentElements);
    assert (agentElements->agentActor);
    
    return 1;
}

/**
 * \fn int mtic_startWithIP(const char *ipAddress, int port)
 * \ingroup startStopKillFct
 * \brief Start an agent on a specific network IP and network port.
 * \param ipAddress is the ip address on network
 * \param port s the network port number used
 * \return 1 if ok, else 0.
 */
int mtic_startWithIP(const char *ipAddress, int port){
    if (agentElements != NULL){
        //Agent is already active : need to stop it first
        mtic_stop();
    }
    mtic_Interrupted = false;
    agentElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agentElements->ipAddress, ipAddress, IP_ADDRESS_LENGTH);
    
#if defined (__WINDOWS__)
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
            strncpy(agentElements->networkDevice, name, 15);
            printf("Connection with ip address %s on device %s\n", ipAddress, agentElements->networkDevice);
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    
    if (strlen(agentElements->networkDevice) == 0){
        fprintf(stderr, "Device name could not be determined for IP address %s... exiting.\n", ipAddress);
        exit(EXIT_FAILURE);
    }
    
    agentElements->zyrePort = port;
    agentElements->agentActor = zactor_new (initActor, agentElements);
    assert (agentElements->agentActor);
    
//    // Set sleep time for connection
//    // to leave time for the connection, to be able to send the definition by the publisher
//#ifdef _WIN32
//    Sleep(1);
//#else
//    usleep(1000);
//#endif
    return 1;
}

/**
 * \fn int mtic_stop()
 * \ingroup startStopKillFct
 * \brief Stop the network layer of an agent.
 * \return 1 if ok, else 0.
 */
int mtic_stop(){
    if (agentElements != NULL){
        //interrupting and destroying mastic thread and zyre layer
        //this will also clean all subscribers
        zstr_sendx (agentElements->agentActor, "$TERM", NULL);
        zactor_destroy (&agentElements->agentActor);
        //cleaning agent
        free (agentElements);
        agentElements = NULL;
        #ifdef _WIN32
        zsys_shutdown();
        #endif
    }else{
        mtic_debug("agent already stopped\n");
    }
    mtic_Interrupted = true;
    
    return 1;
}

/**
 * \fn void mtic_die()
 * \ingroup startStopKillFct
 * \brief Stop and kill an agent.
 */
void mtic_die(){
    mtic_stop();
    exit(1);
}

/**
 * \fn int mtic_setAgentName(const char *name)
 * \ingroup startStopKillFct
 * \brief Set the agent name on network if different that the one defined in its json definition.
 * \param name is the name of the agent.
 * \return 1 if ok, else 0
 */
int mtic_setAgentName(const char *name){
    if (strlen(name) == 0){
        mtic_debug("mtic_setAgentName : Agent name cannot be empty\n");
        return 0;
    }
    if (strcmp(agentName, name) == 0){
        //nothing to do
        return 1;
    }
    char networkDevice[NETWORK_DEVICE_LENGTH] = "";
    char ipAddress[IP_ADDRESS_LENGTH] = "";
    int zyrePort = 0;
    bool needRestart = false;
    if (agentElements != NULL){
        //Agent is already started, zyre actor needs to be recreated
        strncpy(networkDevice, agentElements->networkDevice, NETWORK_DEVICE_LENGTH);
        strncpy(ipAddress, agentElements->ipAddress, IP_ADDRESS_LENGTH);
        zyrePort = agentElements->zyrePort;
        mtic_stop();
        needRestart = true;
    }
    
    strncpy(agentName, name, AGENT_NAME_LENGTH);
    
    if (needRestart){
        mtic_startWithIP(ipAddress, zyrePort);
    }
    
    return 1;
}

/**
 * \fn char *mtic_getAgentName()
 * \ingroup startStopKillFct
 * \brief Get the agent name on the network
 * \return the name of the agent.
 * \warning Allocate memory that should be freed by the user.
 */
char *mtic_getAgentName(){
    return strdup(agentName);
}

/**
 *  \defgroup pauseResumeFct Agent: Pause / resume functions
 *
 */

/**
 * \fn int mtic_freeze()
 * \ingroup pauseResumeFct
 * \brief Freeze agent. Execute the associated FreezeCallback to the agent.
 * by default no callback is defined.
 * \return 1 if ok, else 0
 */
int mtic_freeze(){
    if (!agentCanBeFrozen){
        mtic_debug("warning: agent has been Frozen but is set to 'can't be Frozen'\n");
    }
    if(isFrozen == false)
    {
        mtic_debug("Agent Frozen\n");
        if (agentElements != NULL && agentElements->node != NULL){
            zyre_shouts(agentElements->node, CHANNEL, "FROZEN=1");
        }
        isFrozen = true;
        freezeCallback_t *elt;
        DL_FOREACH(freezeCallbacks,elt){
            elt->callback_ptr(isFrozen, elt->myData);
        }
    }
    return 1;
}

/**
 * \fn bool mtic_isFrozen()
 * \ingroup pauseResumeFct
 * \brief return the frozon state of a agent.
 * \return true if frozen else false.
 */
bool mtic_isFrozen(){
    return isFrozen;
}

/**
 * \fn int mtic_unfreeze()
 * \ingroup pauseResumeFct
 * \brief  Unfreeze agent. Execute the associated FreezeCallback to the agent.
 * by default no callback is defined.
 * \return 1 if ok, else 0
 */
int mtic_unfreeze(){
    if(isFrozen == true)
    {
        mtic_debug("Agent resumed\n");
        if (agentElements != NULL && agentElements->node != NULL){
            zyre_shouts(agentElements->node, CHANNEL, "FROZEN=0");
        }
        isFrozen = false;
        freezeCallback_t *elt;
        DL_FOREACH(freezeCallbacks,elt){
            elt->callback_ptr(isFrozen, elt->myData);
        }
    }
    return 1;
}

/**
 * \fn int mtic_observeFreeze(mtic_freezeCallback cb, void *myData)
 * \ingroup pauseResumeFct
 * \brief Add a mtic_freezeCallback on an agent.
 * \param cb is a pointer to a mtic_freezeCallback
 * \param myData is pointer to user data is needed.
 * \return 1 if ok, else 0.
 */
int mtic_observeFreeze(mtic_freezeCallback cb, void *myData){
    if (cb != NULL){
        freezeCallback_t *newCb = calloc(1, sizeof(freezeCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(freezeCallbacks, newCb);
    }else{
        mtic_debug("mtic_observeFreeze: callback is null\n");
        return 0;
    }
    return 1;
}

/**
 *  \defgroup controleAgentFct Agent: Controle functions
 *
 */

/**
 * \fn int mtic_setAgentState(const char *state)
 * \ingroup controleAgentFct
 * \brief set the internal state of an agent (as the developper defined it)
 * \return 1 if ok, else 0.
 * \param state is the name of the state you want to send.
 */
int mtic_setAgentState(const char *state){
    if (strcmp(state, agentState) != 0){
        strncpy(agentState, state, AGENT_NAME_LENGTH);
        if (agentElements != NULL && agentElements->node != NULL){
            zyre_shouts(agentElements->node, CHANNEL, "STATE=%s", agentState);
        }
    }
    return 1;
}

/**
 * \fn char *mtic_getAgentState()
 * \ingroup controleAgentFct
 * \brief get the internal state of an agent (as the developper defined it)
 * \return the name of the state you want to get.
 * \warning Allocate memory that should be freed by the user.
 */
char *mtic_getAgentState(){
    return strdup(agentState);
}

/**
 *  \defgroup setGetLibraryFct Agent: Get / Set general parameters
 *
 */

/**
 * \fn void mtic_setVerbose (bool verbose)
 * \ingroup setGetLibraryFct
 * \brief set or unset verbose mode on the agent to get more details informations.
 * \param verbose is a bool to set or unset the verbose mode
 */
void mtic_setVerbose (bool verbose){
    verboseMode = verbose;
}

/**
 * \fn bool mtic_getVerbose ()
 * \ingroup setGetLibraryFct
 * \brief get current verbose mode of agent.
 * \return true is verbose mode is set or false.
 */
bool mtic_getVerbose (){
    return verboseMode;
}

/**
 * \fn void mtic_setCanBeFrozen (bool canBeFrozen)
 * \ingroup setGetLibraryFct
 * \brief set or unset forzen mode on the agent.
 * \param canBeFrozen is a bool to set or unset the verbose mode
 */
void mtic_setCanBeFrozen (bool canBeFrozen){
    agentCanBeFrozen = canBeFrozen;
    if (agentElements != NULL && agentElements->node != NULL){
        //update header for information to agents not arrived yet
        zyre_set_header(agentElements->node, "canBeFrozen", "%i", agentCanBeFrozen);
        //send real time notification for agents already there
        zyre_shouts(agentElements->node, CHANNEL, "CANBEFROZEN=%i", canBeFrozen);
    }
}

/**
 *  \defgroup muteAgentFct Agent: Mute functions
 *
 */

/**
 * \fn int mtic_mute()
 * \ingroup muteAgentFct
 * \brief function to mute the agent
 * \return 1 if ok else 0.
 */
int mtic_mute(){
    isWholeAgentMuted = true;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "MUTED=%i", isWholeAgentMuted);
    }
    return 1;
}

/**
 * \fn int mtic_unmute()
 * \ingroup muteAgentFct
 * \brief function to unmute the agent
 * \return 1 if ok or 0.
 */
int mtic_unmute(){
    isWholeAgentMuted = false;
    if (agentElements != NULL && agentElements->node != NULL){
        zyre_shouts(agentElements->node, CHANNEL, "MUTED=%i", isWholeAgentMuted);
    }
    return 1;
}

/**
 * \fn bool mtic_isMuted()
 * \ingroup muteAgentFct
 * \brief function to know if the agent are muted
 * \return true if it is muted else false.
 */
bool mtic_isMuted(){
    return isWholeAgentMuted;
}

void mtic_setCommandLine(const char *line){
    strncpy(commandLine, line, COMMAND_LINE_LENGTH);
}

void mtic_setNotifyMappedAgents(bool notify){
    network_NotifyMappedAgents = notify;
}

