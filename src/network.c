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


//global parameters
//prefixes for sending definitions and mappings through zyre
static const char *definitionPrefix = "EXTERNAL_DEFINITION#";
static const char *loadMappingPrefix = "LOAD_THIS_MAPPING#";
static const char *loadDefinitionPrefix = "LOAD_THIS_DEFINITION#";
#define CHANNEL "MASTIC_PRIVATE"
#define AGENT_NAME_LENGTH 256
char agentName[AGENT_NAME_LENGTH] = AGENT_NAME_DEFAULT;
char agentState[AGENT_NAME_LENGTH] = "";
#define NO_DEVICE "unknown"

typedef struct freezeCallback {      //Need to be unique : the table hash key
    mtic_freezeCallback callback_ptr;   //pointer on the callback
    void *myData;
    struct freezeCallback *prev;
    struct freezeCallback *next;
} freezeCallback_t;

typedef struct zyreCallback {      //Need to be unique : the table hash key
    network_zyreIncoming callback_ptr;   //pointer on the callback
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
zyreAgent_t *zyreAgents = NULL;
bool network_needToSendDefinitionUpdate = false;

//we manage agent data as a global variables inside the network module for now
zyreloopElements_t *agentElements = NULL;
subscriber_t *subscribers = NULL;
freezeCallback_t *freezeCallbacks = NULL;
zyreCallback_t *zyreCallbacks = NULL;

////////////////////////////////////////////////////////////////////////
// INTERNAL API
////////////////////////////////////////////////////////////////////////
int subscribeToPublisherOutput(subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        // Set subscriber to the output filter
        mtic_debug("subscribe to agent %s output %s.\n",agentName,outputName);
        zsock_set_subscribe(subscriber->subscriber, outputName);
        return 1;
    }
    return -1;
}

int unsubscribeToPublisherOutput(subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        // Set subscriber to the output filter
        mtic_debug("unsubscribe to agent %s output %s.\n",agentName,outputName);
        zsock_set_unsubscribe(subscriber->subscriber, outputName);
        return 1;
    }
    
    // Subscriber not found, it is not in the network yet
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
    HASH_ITER(hh, mtic_internal_mapping->map_elements, el, tmp){
        if (strcmp(subscriber->agentName, el->agent_name)==0 || strcmp(el->agent_name, "*") == 0){
            //mapping element is compatible with subscriber name
            //check if we find a compatible output in subscriber definition
            agent_iop *foundOutput = NULL;
            if (subscriber->definition != NULL){
                HASH_FIND_STR(subscriber->definition->outputs_table, el->output_name, foundOutput);
            }
            //TODO : manage '*' as output by iterating to all outputs of subscriber definition
            if (foundOutput != NULL){
                //we have validated agent and output name : we can map
                if (el->state == MAPPING_INACTIVE){
                    el->state = MAPPING_ACTIVE;
                    subscribeToPublisherOutput(subscriber, el->output_name);
                    //mapping was successful : we set timer to notify remote agent if not already done
                    if (!subscriber->mappedNotificationToSend){
                        subscriber->mappedNotificationToSend = true;
                        zloop_timer(agentElements->loop, 100, 1, triggerMappingNotificationToNewcomer, (void *)subscriber);
                    }
                }
            }else{
                //subscriber does not have this output
                if (el->state == MAPPING_ACTIVE){
                    //and this mapping element was previously activated:
                    //we need to clean
                    unsubscribeToPublisherOutput(subscriber, el->output_name);
                    el->state = MAPPING_INACTIVE;
                }
            }
        }
    }
    return 0;
}

int network_disableSubscriberMapping(subscriber_t *subscriber){
    //get mapping elements for this subscriber
    mapping_element_t *el, *tmp;
    HASH_ITER(hh, mtic_internal_mapping->map_elements, el, tmp){
        if (strcmp(subscriber->agentName, el->agent_name)==0 || strcmp(el->agent_name, "*") == 0){
            //mapping element is compatible with subscriber name
            if (el->state == MAPPING_ACTIVE){
                //and this mapping element was previously activated:
                //we need to clean
                unsubscribeToPublisherOutput(subscriber, el->output_name);
                el->state = MAPPING_INACTIVE;
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

        // Try to find our original subscriber
        HASH_FIND_STR(subscribers, subscriberPeerId, foundSubscriber);
        if(foundSubscriber != NULL)
        {
            zmsg_t *msg = zmsg_recv(foundSubscriber->subscriber);
            // Message must contain 2 elements
            // 1 : output name
            // 2 : value of the output
            if(zmsg_size(msg) == 2 && isFrozen == false)
            {
                char * output = zmsg_popstr(msg);
                
                //NOTE: we rely on the external agent definition exclusively to get the output
                //type. This type is then used to convert the received data.
                definition * externalDefinition = foundSubscriber->definition;

                // We add it if we don't have it already
                if(externalDefinition != NULL)
                {
                    // convert the string value in void* corresponding to the type of iop
                    agent_iop * found_iop = model_find_iop_by_name_in_definition(output,externalDefinition);

                    if(found_iop != NULL && found_iop->type == OUTPUT_T)
                    {
                        //try to find mapping elements matching with this subscriber's output
                        //and update mapped input(s) accordingly
                        mapping_element_t *elmt, *tmp;
                        HASH_ITER(hh, mtic_internal_mapping->map_elements, elmt, tmp) {
                            if (strcmp(elmt->agent_name, foundSubscriber->agentName) == 0 && strcmp(elmt->output_name, output) == 0){
                                //we have a matching mapping element
                                if (found_iop->value_type == DATA_T){
                                    zframe_t *frame = zmsg_pop(msg);
                                    void *data = zframe_data(frame);
                                    long size = zframe_size(frame);
                                    mtic_writeInputAsData(elmt->input_name, data, size);
                                }else if (found_iop->value_type == IMPULSION_T){
                                    char * value = zmsg_popstr(msg);
                                    free(value);
                                    mtic_writeInputAsImpulsion(elmt->input_name);
                                }else{
                                    char * value = zmsg_popstr(msg);
                                    mtic_writeInput(elmt->input_name, value, 0); //size is not used for these types
                                    free(value);
                                }
                            }
                        }
                    }
                }
                free(output);
            } else {
                // Ignore the message

                // Print message if the agent has been Frozen
                if(isFrozen == true)
                {
                    mtic_debug("Message received from publisher but all traffic in the agent has been frozen\n");
                }
            }
            zmsg_destroy(&msg);
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
        
        //handle callbacks
        zyreCallback_t *elt;
        DL_FOREACH(zyreCallbacks,elt){
            elt->callback_ptr(zyre_event, elt->myData);
        }

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
                        subscriber->definition = NULL;
                        assert(subscriber->subscriber);
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
            char *message = zmsg_popstr (msg);
            
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
                    if(subscriber->definition != NULL){
                        mtic_debug("definition already exists for agent %s : new definition will overwrite the previous one...\n", name);
                        definition_freeDefinition(subscriber->definition);
                        subscriber->definition = NULL;
                    }
                    
                    mtic_debug("Store definition for agent %s\n", name);
                    subscriber->definition = newDefinition;
                    // Check the involvement of this new definition in our mapping and update subscriptions
                    network_manageSubscriberMapping(subscriber);
                }else{
                    mtic_debug("ERROR: definition is NULL or has no name for agent %s\n", name);
                    definition_freeDefinition(newDefinition);
                    newDefinition = NULL;
                }
                free(strDefinition);
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
                mtic_internal_mapping = parser_LoadMap(strMapping);
                free(strMapping);
                
                //TODO: activate mapping dynamically
                
            }else{
                //other supported messages
                if (strlen("MAPPED") == strlen(message) && strncmp (message, "MAPPED", strlen("MAPPED")) == 0){
                    mtic_debug("Mapping notification received from %s\n", name);
                    //TODO: optimize to rewrite only outputs actually involved in the mapping
                    long nbOutputs = 0;
                    char **outputsList = NULL;
                    outputsList = mtic_getOutputsList(&nbOutputs);
                    for (int i = 0; i < nbOutputs; i++){
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
                    mtic_die();
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
                }else if ((strncmp (message, "MUTE", strlen("MUTE")) == 0) && (strlen(message) > strlen("MUTE")+1)){
                    char *subStr = message + strlen("MUTE") + 1;
                    mtic_muteOutput(subStr);
                }else if ((strncmp (message, "UNMUTE", strlen("UNMUTE")) == 0) && (strlen(message) > strlen("UNMUTE")+1)){
                    char *subStr = message + strlen("UNMUTE") + 1;
                    mtic_unmuteOutput(subStr);
                }else if ((strncmp (message, "MAP", strlen("MAP")) == 0) && (strlen(message) > strlen("MAP")+1)){
                    char *subStr = message + strlen("MAP") + 1;
                    char *input, *agent, *output;
                    input = strtok (subStr," ");
                    agent = strtok (NULL," ");
                    output = strtok (NULL," ");
                    if (input != NULL && agent != NULL && output != NULL){
                        mtic_addMappingEntry(input, agent, output);
                    }
                }else if ((strncmp (message, "UNMAP", strlen("UNMAP")) == 0) && (strlen(message) > strlen("UNMAP")+1)){
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
                    if (subscriber != NULL)
                    {
                        mtic_debug("cleaning subscribtions and mappings to %s\n", name);
                        // disable mapping of the leaving agent
                        network_disableSubscriberMapping(subscriber);
                        // clean the agent definition
                        definition * myDefinition = subscriber->definition;
                        if(myDefinition != NULL){
                            definition_freeDefinition(myDefinition);
                        }
                        //clean the rest
                        HASH_DEL(subscribers, subscriber);
                        zloop_poller_end(agentElements->loop , subscriber->pollItem);
                        zsock_destroy(&subscriber->subscriber);
                        free((char*)subscriber->agentName);
                        free((char*)subscriber->agentPeerId);
                        free(subscriber->pollItem);
                        free(subscriber->subscriber);
                        subscriber->subscriber = NULL;
                        free(subscriber);
                        subscriber = NULL;
                    }
                }
            }
        }
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

//Timer callback to (re)send our definition to agents on the private channel
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
    }
    return 0;
}

static void
initActor (zsock_t *pipe, void *args)
{
    //we are (re)starting : we rest the sendDefinitionFlag because
    //all network connections are going to be (re)started and the
    //"join MASTIC_PRIVATE" trigger will do its job.
    network_needToSendDefinitionUpdate = false;
    
    //start zyre
    agentElements->node = zyre_new (agentName);
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
#ifdef __APPLE__
#if TARGET_OS_IOS
    char pathbuf[64] = "not available";
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
    zyre_set_header(agentElements->node, "execpath", "%s", pathbuf);
    zyre_set_header(agentElements->node, "pid", "%i", pid);
#endif
    
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    //Use GetModuleFileName() to get exec path
    WCHAR temp[MAX_PATH];
    GetModuleFileName(NULL,temp,MAX_PATH);

    //Conversion in char *
    char exeFilePath[MAX_PATH];
    wcstombs_s(NULL,exeFilePath,sizeof(exeFilePath),temp,sizeof(temp));

    //Get PID as well
    DWORD pid = GetCurrentProcessId();

    mtic_debug("proc %d: %s\n", (int)pid, exeFilePath);

    //Add to header
    zyre_set_header(agentElements->node, "execpath", "%s", exeFilePath);
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

    zloop_start (loop); //start returns when one of the pollers returns -1

    zloop_destroy (&loop);
    assert (loop == NULL);

    //clean
    zloop_destroy (&loop);
    assert (loop == NULL);
    zyre_stop (agentElements->node);
    zclock_sleep (100);
    zyre_destroy (&agentElements->node);
    zsock_destroy(&agentElements->publisher);
    //clean subscribers
    subscriber_t *s, *tmp;
    HASH_ITER(hh, subscribers, s, tmp) {
        HASH_DEL(subscribers, s);
        zsock_destroy(&s->subscriber);
        free((char*)s->agentName);
        free((char*)s->agentPeerId);
        free(s->pollItem);
        free(s->subscriber);
        s->subscriber = NULL;
        free(s);
        s = NULL;
    }
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
            if (found_iop->value_type == DATA_T){
                void *data = NULL;
                long size = 0;
                mtic_readOutputAsData(output_name, &data, &size);
                //TODO: decide if we should delete the data after use or keep it in memory
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
        agentElements = NULL;
    }
    
    agentElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agentElements->networkDevice, networkDevice, NETWORK_DEVICE_LENGTH);
    agentElements->ipAddress[0] = '\0';
    
#ifdef _WIN32
    do {
        pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct... exiting.\n");
            exit(1);
        }
        
        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
        
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }
        
        Iterations++;
        
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));
    
    if (dwRetVal == NO_ERROR) {
        // If successful, output some information from the data we received
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            //Convert the wchar_t to char *
            friendly_name = (char *)malloc( BUFSIZ );
            count = wcstombs(friendly_name, pCurrAddresses->FriendlyName, BUFSIZ );
            
            //If the friendly_name is the same of the networkDevice
            if (strcmp(agentElements->networkDevice, friendly_name) == 0)
            {
                pUnicast = pCurrAddresses->FirstUnicastAddress;
                if (pUnicast != NULL)
                {
                    for (i = 0; pUnicast != NULL; i++)
                    {
                        if(pUnicast)
                        {
                            if (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
                            {
                                struct sockaddr_in *sa_in = (struct sockaddr_in *)pUnicast->Address.lpSockaddr;
                                strncpy(agentElements->ipAddress, inet_ntoa(sa_in->sin_addr), IP_ADDRESS_LENGTH);
                                free(friendly_name);
                                mtic_debug("Connection on ip address %s on device %s\n", agentElements->ipAddress, agentElements->networkDevice);
                            }
                        }
                        pUnicast = pUnicast->Next;
                    }
                }
            }
            
            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        mtic_debug("Call to GetAdaptersAddresses failed with error: %d\n",
               dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            mtic_debug("\tNo addresses were found for the requested parameters\n");
        else {
            
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                              FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              // Default language
                              (LPTSTR) & lpMsgBuf, 0, NULL)) {
                mtic_debug("\tError: %s", lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                    FREE(pAddresses);
                exit(1);
            }
        }
    }
    
    if (pAddresses) {
        FREE(pAddresses);
    }
    
#else
    struct ifaddrs *addrs, *tmpaddr;
    getifaddrs(&addrs);
    tmpaddr = addrs;
    while (tmpaddr)
    {
        if (tmpaddr->ifa_addr && tmpaddr->ifa_addr->sa_family == AF_INET)
        {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmpaddr->ifa_addr;
            if (strcmp(networkDevice, tmpaddr->ifa_name) == 0)
            {
                strncpy(agentElements->ipAddress, inet_ntoa(pAddr->sin_addr), IP_ADDRESS_LENGTH);
                mtic_debug("Connection with ip address %s on device %s\n", agentElements->ipAddress, networkDevice);
            }
        }
        tmpaddr = tmpaddr->ifa_next;
    }
    freeifaddrs(addrs);
#endif
    
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
        agentElements = NULL;
    }
    
    agentElements = calloc(1, sizeof(zyreloopElements_t));
    strncpy(agentElements->networkDevice, NO_DEVICE, NETWORK_DEVICE_LENGTH);
    strncpy(agentElements->ipAddress, ipAddress, IP_ADDRESS_LENGTH);
    mtic_debug("Connection with ip address %s\n", agentElements->ipAddress);
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
        //interrupting and destroying mastic thread
        zstr_sendx (agentElements->agentActor, "$TERM", NULL);
        zactor_destroy (&agentElements->agentActor);
        //cleaning agent
        free (agentElements);
        agentElements = NULL;
        //cleaning Freeze callbacks
        freezeCallback_t *elt, *tmp;
        DL_FOREACH_SAFE(freezeCallbacks,elt,tmp) {
            DL_DELETE(freezeCallbacks,elt);
            free(elt);
        }

    #ifdef _WIN32
    zsys_shutdown();
    #endif
    }else{
        mtic_debug("agent already stopped\n");
    }
    
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
        if (strcmp(networkDevice, NO_DEVICE) == 0){
            mtic_startWithIP(ipAddress, zyrePort);
        }else{
            mtic_startWithDevice(networkDevice, zyrePort);
        }
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
            zyre_shouts(agentElements->node, CHANNEL, "FREEZE=ON");
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
            zyre_shouts(agentElements->node, CHANNEL, "FREEZE=OFF");
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



