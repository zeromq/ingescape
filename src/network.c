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
static const char *definitionPrefix = "DEFINITION#";
static const char *mappingPrefix = "MAPPING#";
#define CHANNEL "MASTIC_PRIVATE"
#define NETWORK_DEVICE_LENGTH 16
#define AGENT_NAME_LENGTH 256
#define IP_ADDRESS_LENGTH 256
char agentName[AGENT_NAME_LENGTH] = AGENT_NAME_DEFAULT;
char agentState[AGENT_NAME_LENGTH] = "";
#define NO_DEVICE "unknown"


//network structures
typedef struct zyreloopElements{
    char networkDevice[NETWORK_DEVICE_LENGTH];
    char ipAddress[IP_ADDRESS_LENGTH];
    int zyrePort;
    zactor_t *agentActor;
    zyre_t *node;
    zsock_t *publisher;
    zloop_t *loop;
} zyreloopElements_t;

typedef struct FreezeCallback {      //Need to be unique : the table hash key
    mtic_freezeCallback callback_ptr;   //pointer on the callback
    void *myData;
    struct FreezeCallback *prev;
    struct FreezeCallback *next;
} FreezeCallback_t;


//we manage agent data as a global variables inside the network module for now
zyreloopElements_t *agentElements = NULL;
subscriber_t *subscribers = NULL;
FreezeCallback_t *FreezeCallbacks = NULL;

////////////////////////////////////////////////////////////////////////
// Network internal functions
////////////////////////////////////////////////////////////////////////

/*
 * Function: sendDefinition
 * ----------------------------
 *   Send my definition through our publisher
 *   The agent definition must be defined : mtic_definition_loaded
 *
 *   usage : mtic_sendDefinition()
 *
 */
void sendDefinitionToAgent(const char *peerId)
{
    // Send my own definition
    if(mtic_internal_definition != NULL)
    {
        char * definitionStr = NULL;
        definitionStr = parser_export_definition(mtic_internal_definition);
        // Send definition to the network
        if(definitionStr)
        {
            // Send definition
            zyre_whispers(agentElements->node, peerId, "%s%s", definitionPrefix, definitionStr);
            //zyre_shouts (agentElements->node, CHANNEL, "%s%s", exportDefinitionPrefix, definitionStr);
            free (definitionStr);
            definitionStr = NULL;
        } else {
            mtic_debug("Error : could not send definition of %s.\n",mtic_internal_definition->name);
        }
    }
}

/*
 * Function: subscribe_to
 * ----------------------------
 *   Add a subscription to an agent
 *
 *   usage : subscribeTo(const char *agentName, const char *filterDescription)
 *
 *   agentName          : agent name
 *   filterDescription  : filter description (ex: "moduletest.output1")
 *
 *   returns : the state of the subscribtion
 */
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

/*
 * Function: unsubscribe_to
 * ----------------------------
 *   Remove a subscription to an agent
 *
 *   usage : unsubscribeTo(const char *agentName, const char *filterDescription)
 *
 *   agentName          : agent name
 *   filterDescription  : filter description (ex: "moduletest.output1")
 *
 *   returns : the state of the subscribtion removal
 */
int unsubscribeToPublisherOutput(subscriber_t *subscriber, const char *outputName)
{
    if(outputName != NULL && strlen(outputName) > 0)
    {
        // Set subscriber to the output filter
        mtic_debug("subscribe to agent %s output %s.\n",agentName,outputName);
        zsock_set_unsubscribe(subscriber->subscriber, outputName);
        return 1;
    }
    
    // Subscriber not found, it is not in the network yet
    return -1;
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

                // Find the subscriber definition if received
                definition * externalDefinition = foundSubscriber->definition;

                // We add it if we don't have it already
                if(externalDefinition != NULL)
                {
                    // convert the string value in void* corresponding to the type of iop
                    agent_iop * found_iop = model_find_iop_by_name_in_definition(output,externalDefinition);

                    if(found_iop != NULL)
                    {
                        if (found_iop->value_type == DATA_T){
                            zframe_t *frame = zmsg_pop(msg);
                            void *data = zframe_data(frame);
                            long size = zframe_size(frame);
                            mapping_map_received(foundSubscriber->agentName,
                                              output,
                                              data,
                                              size);
                        }else if (found_iop->value_type == IMPULSION_T){
                            char * value = zmsg_popstr(msg);
                            free(value);
                            mapping_map_received(foundSubscriber->agentName,
                                              output,
                                              0,
                                              0);
                        }else{
                            char * value = zmsg_popstr(msg);

                            // Map reception send to update the internal model
                            mapping_map_received(foundSubscriber->agentName,
                                              output,
                                              value,
                                              0);
                            free(value);
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

        if (streq (event, "ENTER")){
            mtic_debug("->%s has entered the network with peer id %s and address %s\n", name, peer, address);
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
                            //we have a reconnection with same peerId : normally impossible
                            mtic_debug("Error: Peer id %s was connected before with agent name %s (normally impossible)\n", peer, subscriber->agentName);
                            HASH_DEL(subscribers, subscriber);
                            zloop_poller_end(agentElements->loop , subscriber->pollItem);
                            zsock_destroy(&subscriber->subscriber);
                            free((char*)subscriber->agentName);
                            free((char*)subscriber->agentPeerId);
                            free(subscriber->pollItem);
                            free(subscriber->subscriber);
                            if (subscriber->definition != NULL){
                                definition_free_definition(subscriber->definition);
                                subscriber->definition = NULL;
                            }
                            subscriber->subscriber = NULL;
                            free(subscriber);
                            subscriber = NULL;
                        }
                        subscriber = calloc(1, sizeof(subscriber_t));
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
                subscriber_t *subscriber;
                HASH_FIND_STR(subscribers, peer, subscriber);
                if (subscriber != NULL){
                    //A new agent joined the MASTIC channel and has subscriber header
                    //Send my definition to this new agent
                    mtic_debug("Send our definition to %s (%s)\n", subscriber->agentName, subscriber->agentPeerId);
                    sendDefinitionToAgent(subscriber->agentPeerId);
                }
            }
        } else if (streq (event, "LEAVE")){
            mtic_debug("-%s has left %s\n", name, group);
        } else if (streq (event, "SHOUT")){
            //nothing to do so far
        } else if(streq (event, "WHISPER")){
            char *message = zmsg_popstr (msg);
            
            //check if message is a definition
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
                        definition_free_definition(subscriber->definition);
                        subscriber->definition = NULL;
                    }
                    
                    mtic_debug("Store definition for agent %s\n", name);
                    subscriber->definition = newDefinition;
                    // Check the involvement of this new definition in our mapping and update subscriptions
                    network_checkAndSubscribeToPublisher(newDefinition->name);
                }else{
                    mtic_debug("ERROR: definition is NULL or has no name for agent %s\n", name);
                    definition_free_definition(newDefinition);
                    newDefinition = NULL;
                }
                free(strDefinition);
            }
            //check if message is mapping
            else if (strlen(message) > strlen(mappingPrefix) && strncmp (message, mappingPrefix, strlen(mappingPrefix)) == 0)
            {
                // Extract mapping from message
                char* strMapping = calloc(strlen(message)- strlen(definitionPrefix)+1, sizeof(char));
                memcpy(strMapping, &message[strlen(mappingPrefix)], strlen(message)- strlen(mappingPrefix));
                strMapping[strlen(message)- strlen(mappingPrefix)] = '\0';
                
                // Load definition from string content
                mtic_my_agent_mapping = parser_LoadMap(strMapping);
                
                //TODO: activate mapping dynamically
                
            }else{
                //other supported messages
                if (strncmp (message, "MAPPED", strlen("MAPPED")) == 0){
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
                }
            }
            free(message);
        } else if (streq (event, "EXIT")){
            mtic_debug("<-%s exited\n", name);
            // Try to find the subscriber to destory
            subscriber_t *subscriber = NULL;
            HASH_FIND_STR(subscribers, peer, subscriber);
            if (subscriber != NULL)
            {
                mtic_debug("cleaning subscribtions to %s\n", name);
                // Remove the agent definition from network
                definition * myDefinition = subscriber->definition;
                if(myDefinition != NULL)
                {
                    // Deactivate mapping of the leaving agent
                    agent_iop* iop_unmappped = mapping_unmap(myDefinition);
                    struct agent_iop *iop, *tmp;
                    HASH_ITER(hh,iop_unmappped, iop, tmp)
                    {
                        HASH_DEL(iop_unmappped, iop);
                        free(iop);
                    }
                    definition_free_definition(myDefinition);
                    subscriber->definition = myDefinition = NULL;
                }
                
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
        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

static void
initActor (zsock_t *pipe, void *args)
{
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

    //FOR TESTING - SEND PERIODIC MESSAGES ON PUBLISHER
    //zloop_timer(loop, 1000, 0, sendTestMessagesOnPublisher, zEl);

    // FIXME - needed ? Export my definition for once if exists
    // mtic_sendDefinition();

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

//Timer callback to send MAPPED notification for agents we subscribed to
int triggerMappingNotificationToNewcomer(zloop_t *loop, int timer_id, void *arg){
    char *peerId = (char *) arg;
    //zyre_shouts(agentElements->node, CHANNEL, "MAPPED");
    zyre_whispers(agentElements->node, peerId, "MAPPED");
    return 0;
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

/*
 * Function: debug
 * ----------------------------
 *   trace debug messages only on verbose mode
 *   Print file name and line number in debug mode
 *
 *   usage : mtic_debug(format,msg)
 *
 */
// Definition of a trace function depending of the verbose mode and debug compilation
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
                char* str_value = definition_get_iop_value_as_string(found_iop);
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

/*
 * Function: network_checkAndSubscribeToPublisher
 * ----------------------------
 *   Check mappings made on agent name
 *   Connect to these outputs if compatibility is ok
 *
 *   usage : checkAndSubscribeTo(char* agentName)
 *
 *   agentName          : agent name
 *
 *   returns : -1 if an error occured, 1 otherwise.
 *
 */
int network_checkAndSubscribeToPublisher(const char* agtName)
{
    int result = 0;
    bool sendMapNotif = false;
    // Look for the definition for this agent
    definition * publisherDefinition = NULL;
    subscriber_t *iter = NULL;
    for(iter = subscribers; iter != NULL; iter=iter->hh.next) {
        if (strcmp(agtName, iter->agentName)==0 || strcmp("*", agtName) == 0){
            //we found an agent with the name to map or we map on *
            publisherDefinition = iter->definition;
            if(publisherDefinition != NULL)
            {
                // Process mapping
                agent_iop* outputsToSubscribe = mapping_check_map(publisherDefinition);
                if(outputsToSubscribe != NULL)
                {
                    struct agent_iop *iop, *tmp;
                    HASH_ITER(hh,outputsToSubscribe, iop, tmp)
                    {
                        // Make subscribtion
                        int cr = subscribeToPublisherOutput(iter, iop->name);
                        if(cr > 0)
                        {
                            mtic_debug("Subscription found and done to output %s from agent %s.\n",iop->name,publisherDefinition->name);
                            result = 1;
                            sendMapNotif = true;
                        } else {
                            mtic_debug("Subscription has been found but not done to output %s from agent %s.\n",iop->name,publisherDefinition->name);
                        }
                        HASH_DEL(outputsToSubscribe, iop);
                        free(iop);
                    }
                } else {
                    mtic_debug("No outputs to subscribe to for agent: %s.\n",agtName);
                }
            } else {
                mtic_debug("No definiton found for the agent: %s\n",agtName);
            }
            if (sendMapNotif){
                //mapping was successful : we inform target agent via zyre
                //we set a timer instead of send message to zyre immediately to
                //let the time to all agents pub/sub sockets to synchronize
                //their actual subscribtions
                zloop_timer(agentElements->loop, 100, 1, triggerMappingNotificationToNewcomer, (void *)iter->agentPeerId);
                sendMapNotif = false;
            }
        }
    }
    return result;
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
        FreezeCallback_t *elt, *tmp;
        DL_FOREACH_SAFE(FreezeCallbacks,elt,tmp) {
            DL_DELETE(FreezeCallbacks,elt);
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
            zyre_shouts(agentElements->node, CHANNEL, "Freeze=ON");
        }
        isFrozen = true;
        FreezeCallback_t *elt;
        DL_FOREACH(FreezeCallbacks,elt){
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
            zyre_shouts(agentElements->node, CHANNEL, "Freeze=OFF");
        }
        isFrozen = false;
        FreezeCallback_t *elt;
        DL_FOREACH(FreezeCallbacks,elt){
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
        FreezeCallback_t *newCb = calloc(1, sizeof(FreezeCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(FreezeCallbacks, newCb);
    }else{
        mtic_debug("callback is null\n");
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



