//
//  mtic_network.c
//
//  Created by Stephane Vales on 10/06/2016.
//  Modified by Vincent Deliencourt
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#include <zyre.h>
#include <czmq.h>
#include "uthash/uthash.h"
#include "uthash/utlist.h"
#include "mastic_private.h"
#include "mastic.h"

#ifdef _WIN32
    #define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
    #define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
    #define MAX_TRIES 3

    size_t  count;
    char *frindly_name = NULL;
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
bool verboseMode = false;
bool isPaused = false;

//global parameters
//prefix for sending definition through zyre
static const char *exportDefinitionPrefix = "DEFINITION#";
#define CHANNEL "MASTIC_PRIVATE"
#define NETWORK_DEVICE_LENGTH 16
#define AGENT_NAME_LENGTH 256
#define IP_ADDRESS_LENGTH 256
char agentName[AGENT_NAME_LENGTH] = "";
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

typedef struct subcriber{
    const char *agentName;
    const char *agentPeerId;
    zsock_t *subscriber;
    zmq_pollitem_t *pollItem;
    UT_hash_handle hh;
} subscriber_t;

typedef struct pauseCallback {      //Need to be unique : the table hash key
    mtic_pauseCallback callback_ptr;   //pointer on the callback
    void *myData;
    struct pauseCallback *prev;
    struct pauseCallback *next;
} pauseCallback_t;


//we manage agent data as a global variables inside the network module for now
zyreloopElements_t *agentElements = NULL;
subscriber_t *subscribers = NULL;
pauseCallback_t *pauseCallbacks = NULL;


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
void sendDefinition()
{
    // Send my own definition
    if(mtic_definition_loaded != NULL)
    {
        char * definitionStr = NULL;
        definitionStr = export_definition(mtic_definition_loaded);
        // Send definition to the network
        if(definitionStr)
        {
            // Send definition
            zstr_sendx(agentElements->agentActor,"definition",CHANNEL,definitionStr,NULL);
            
            // Free memory
            free ((char*) definitionStr);
            definitionStr = NULL;
        } else {
            mtic_debug("Error : could not send definition of %s.\n",mtic_definition_loaded->name);
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
int subscribeToPublisher(const char *agentName, const char *outputName)
{
    // If a filter has to be mtic_set
    if(strlen(outputName) > 0)
    {
        //we found a possible publisher to subscribe to
        subscriber_t *subscriber, *tmp;
        HASH_ITER(hh, subscribers, subscriber, tmp) {
            
            if(strcmp(subscriber->agentName,agentName) == 0)
            {
                // Set subscriber to the output filter
                mtic_debug("subcribe agent %s to %s.\n",agentName,outputName);
                zsock_set_subscribe(subscriber->subscriber, outputName);
                
                // we quit
                return 1;
            }
        }
    }
    
    // Subsriber not found, it is not in the network yet
    return 0;
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
int unsubscribeToPublisher(const char *agentName, const char *outputName)
{
    // If a filter has to be mtic_set
    if(strlen(outputName) > 0)
    {
        //we found a possible publisher to subscribe to
        subscriber_t *subscriber, *tmp;
        HASH_ITER(hh, subscribers, subscriber, tmp) {
            
            if(strcmp(subscriber->agentName,agentName) == 0)
            {
                // Unsubscribe to the output filter
                mtic_debug("unsubcribe agent %s to %s.\n",agentName,outputName);
                zsock_set_unsubscribe(subscriber->subscriber, outputName);
                
                // we quit
                return 0;
            }
        }
    }
    
    // Subsriber not found, it is not in the network yet
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
        } else if (streq (command, "definition")) {
            char *group = zmsg_popstr (msg);
            char *string = zmsg_popstr (msg);
            zyre_shouts (agentElements->node, group, "%s%s",exportDefinitionPrefix, string);
            free(group);
            free(string);
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
        subscriber_t * subscriberFound = NULL;

        // Try to find our original subscriber
        HASH_FIND_STR(subscribers, subscriberPeerId, subscriberFound);
        if(subscriberFound != NULL)
        {
            zmsg_t *msg = zmsg_recv(subscriberFound->subscriber);
            // Message ust contain 3 elements=
            // 1 : filter "agentName.outputName
            // 2 : output name only
            // 3 : value of the output
            if(zmsg_size(msg) == 3 && isPaused == false)
            {
                char * filter = zmsg_popstr(msg);
                char * output = zmsg_popstr(msg);
                char * value = zmsg_popstr(msg);

                // Find the subcriber definition
                // Look for the new agent definition
                definition * externalDefinition = NULL;
                HASH_FIND_STR(mtic_agents_defs_on_network, subscriberFound->agentName, externalDefinition);

                // We had it if we don't have it already
                if(externalDefinition != NULL)
                {
                    // convert the string value in void* corresponding to the type of iop
                    model_state code;
                    agent_iop * found_iop = mtic_find_iop_by_name_on_definition(output,externalDefinition, &code);

                    if(found_iop != NULL)
                    {
                        const void* converted_value = mtic_iop_value_string_to_real_type(found_iop, value);

                        // Map reception send to modify the internal model
                        code = mtic_map_received(subscriberFound->agentName,
                                            output,
                                            (void*)converted_value);

                    }
                }

                free(filter);
                free(output);
                free(value);
            } else {
                // Ignore the message

                // Print message if the agent has been paused
                if(isPaused == true)
                {
                    mtic_debug("Message received from publisher but all traffic in the agent has been paused\n");
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
                char endpointAddress[128];
                strcpy(endpointAddress, address);
                char *insert = endpointAddress + strlen(endpointAddress);
                while (*insert != ':'){
                    insert--;
                    if (insert == endpointAddress){
                        mtic_debug("Error: could not extract port from address %s", address);
                    }
                }
                *(insert + 1) = '\0';
                strcat(endpointAddress, v);

                if (strcmp(k, "publisher")==0){
                    //we found a possible publisher to subscribe to
                    subscriber_t *subscriber;
                    HASH_FIND_STR(subscribers, peer, subscriber);
                    if (subscriber != NULL){
                        //Agent is already know and not cleaned: this is a case of reconnection
                        mtic_debug("Destroy subscriber %s.\n",subscriber->agentName);
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
                    subscriber = calloc(1, sizeof(subscriber_t));
                    subscriber->agentName = strdup(name);
                    subscriber->agentPeerId = strdup (peer);
                    subscriber->subscriber = zsock_new_sub(endpointAddress, "DEFAULT");
                    assert(subscriber->subscriber);
                    HASH_ADD_STR(subscribers, agentPeerId, subscriber);
                    subscriber->pollItem = calloc(1, sizeof(zmq_pollitem_t));
                    subscriber->pollItem->socket = zsock_resolve(subscriber->subscriber);
                    subscriber->pollItem->fd = 0;
                    subscriber->pollItem->events = ZMQ_POLLIN;
                    subscriber->pollItem->revents = 0;
                    zloop_poller (agentElements->loop, subscriber->pollItem, manageSubscription, (void*)subscriber->agentPeerId);
                    zloop_poller_set_tolerant(loop, subscriber->pollItem);
                    mtic_debug("Subscriber created for %s at %s.\n",subscriber->agentName,endpointAddress);


                    // Send my definition to the new agent
                    mtic_debug("Send our definition ...\n");
                    sendDefinition();
                }


                free(k);
            }
            zlist_destroy(&keys);
        } else if (streq (event, "JOIN")){
            mtic_debug("+%s has joined %s\n", name, group);
        } else if (streq (event, "LEAVE")){
            mtic_debug("-%s has left %s\n", name, group);
        } else if (streq (event, "SHOUT")){
            char *message = zmsg_popstr (msg);
            if(strlen(message) > strlen(exportDefinitionPrefix))
            {
                if (strncmp (message, exportDefinitionPrefix, strlen(exportDefinitionPrefix)) == 0)
                {
                    char* strDefinition = calloc(strlen(message)- strlen(exportDefinitionPrefix)+1, sizeof(char));

                    // Extract definition from message
                    memcpy(strDefinition, &message[strlen(exportDefinitionPrefix)], strlen(message)- strlen(exportDefinitionPrefix));
                    strDefinition[strlen(message)- strlen(exportDefinitionPrefix)] = '\0';

                    // Look for the new agent definition
                    definition * receivedDefinition = NULL;
                    HASH_FIND_STR(mtic_agents_defs_on_network, name, receivedDefinition);

                    // We had it if we don't have it already
                    if(receivedDefinition == NULL){
                        // Load definition from string content
                        receivedDefinition = load_definition(strDefinition);

                        // Add the new definition if not found
                        if(receivedDefinition != NULL && receivedDefinition->name != NULL)
                        {
                            mtic_debug("Add new definition from %s\n", name);

                            // FIXME - needed ? Send back our definition if it's a new one for us
                            //printf ("Send our definition...\n");
                            //mtic_sendDefinition();

                            // Add definition to the map
                            HASH_ADD_STR(mtic_agents_defs_on_network, name, receivedDefinition);

                            // check and subscribe to the new added outputs if eixts and the concerning agent is present.
                            // Check if we have a mapping with it
                            // Check and add mapping if needed
                            network_checkAndSubscribeTo(name);
                        }
                    }

                    free(strDefinition);
                }
            }
            free(message);
        } else if (streq (event, "WHISPER")){
            char *message;
            while ((message = zmsg_popstr(msg))) {
                mtic_debug("#whisper[%s] : %s\n", name, message);
                free (message);
            }
        } else if (streq (event, "EXIT")){
            mtic_debug("<-%s exited\n", name);
            // Try to find the subscriber to destory
            subscriber_t *subscriber = NULL;
            HASH_FIND_STR(subscribers, peer, subscriber);
            if (subscriber != NULL)
            {
                // Remove the agent definition from network
                definition * receivedDefinition = NULL;
                HASH_FIND_STR(mtic_agents_defs_on_network, name, receivedDefinition);
                // We had it if we don't have it already
                if(receivedDefinition != NULL)
                {
                    // Deactivate mapping of the leaving agent
                    agent_iop* iop_unmappped = mtic_unmap(receivedDefinition);
                    struct agent_iop *iop, *tmp;
                    HASH_ITER(hh,iop_unmappped, iop, tmp)
                    {
                        HASH_DEL(iop_unmappped, iop);
                        free(iop);
                    }

                    HASH_DEL(mtic_agents_defs_on_network, receivedDefinition);
                    free_definition(receivedDefinition);
                    receivedDefinition = NULL;
                }

                // NB - subscriber destruction :
                // The subscriber is destoryed at the reconnection
                // or at the end of the execution. It's made safer for czmq
                // sub socets to terminate that way.

            }

        }

        zyre_event_destroy(&zyre_event);
    }
    return 0;
}

//FOR TESTING - SEND PERIODIC MESSAGES ON PUBLISHER
int sendTestMessagesOnPublisher(zloop_t *loop, int timer_id, void *arg){

    //zyreloopElements_t *zEl = (zyreloopElements_t *)arg;

    // FIXME - For test, only send an update if we are the moduletest2,
    // will be received by moduletest
    // Message ust contain 3 elements=
    // 1 : filter "agentName.outputName
    // 2 : output name only
    // 3 : value of the output
    network_publishOutput ("output1");

    return 0;
}


static void
initActor (zsock_t *pipe, void *args)
{
    if (strlen(agentName) == 0){
        strncpy(agentName, "undefined", AGENT_NAME_LENGTH);
    }
    //start zyre
    agentElements->node = zyre_new (agentName);
    zyre_set_port(agentElements->node, agentElements->zyrePort);
    if (agentElements->node == NULL){
        printf("Error : could not create zyre node... exiting.\n");
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
    zyre_set_header(agentElements->node, "publisher", "%s", insert + 1);

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
    void *zsock = zsock_resolve(zyre_socket (agentElements->node));
    if (zsock == NULL){
        printf("Error : could not get the zyre socket for polling... exiting.\n");
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
        fprintf(stderr, "           ... in %s (%s, line %d).\n", __func__, __FILE__, __LINE__);
#endif // DEBUG
        
        va_end(ar);
    }
}

int network_publishOutput (const char* output_name)
{
    int result = 0;
    
    model_state code;
    agent_iop * found_iop = mtic_find_iop_by_name(output_name, &code);
    
    if(agentElements->publisher != NULL && found_iop != NULL)
    {
        if(found_iop->is_muted == false && found_iop->name != NULL && isPaused == false)
        {
            char* str_value = mtic_iop_value_to_string(found_iop);
            if(strlen(str_value) > 0)
            {
                // Build the map description used as filter for other agents
                char mapDescription[100];
                strcpy(mapDescription, agentName);
                strcat(mapDescription, ".");
                strcat(mapDescription, found_iop->name);
                
                mtic_debug("publish %s -> %s.\n",found_iop->name,str_value);
                // Send message
                zstr_sendx(agentElements->publisher, mapDescription,found_iop->name,str_value,NULL);
                
                result = 1;
            }
            free(str_value);
        } else {
            // Print message if output has been muted
            if(found_iop->is_muted == true)
            {
                mtic_debug("Shoudl publish output but the output %s has been muted.\n",found_iop->name);
            }
            
            // Print message if the agent has been paused
            if(isPaused == true)
            {
                mtic_debug("Shoudl publish output but all traffic in the agent has been paused\n");
            }
        }
    }
    
    return result;
}

/*
 * Function: network_checkAndSubscribeTo
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
int network_checkAndSubscribeTo(const char* agentName)
{
    int result = 0;
    // Look for the new agent definition
    definition * externalDefinition = NULL;
    HASH_FIND_STR(mtic_agents_defs_on_network, agentName, externalDefinition);
    
    // We had it if we don't have it already
    if(externalDefinition != NULL)
    {
        // Porcess mapping
        // Check if we have a mapping with it
        // Check and add mapping if needed
        agent_iop* outputsToSubscribe = mtic_check_map(externalDefinition);
        
        if(outputsToSubscribe != NULL)
        {
            char map_description[100];
            
            struct agent_iop *iop, *tmp;
            HASH_ITER(hh,outputsToSubscribe, iop, tmp)
            {
                strcpy(map_description, externalDefinition->name);
                strcat(map_description, ".");
                strcat(map_description, iop->name);
                
                // Make subscribtion
                int cr = subscribeToPublisher(externalDefinition->name,map_description);
                
                // Subscription has been done
                if(cr > 0)
                {
                    mtic_debug("Subscription found and done to output: %s from agent: %s.\n",iop->name,externalDefinition->name);
                    result = 1;
                } else {
                    mtic_debug("Subscription has been found but not done to output: %s from agent: %s.\n",iop->name,externalDefinition->name);
                }
                
                HASH_DEL(outputsToSubscribe, iop);
                free(iop);
            }
        } else {
            //printf("[Warning] : No outputs to subscribe to for agent: %s.\n",agentName);
        }
    } else {
        //printf("[Warning] : No definiton found for the agent: %s. It must been arrived yet, wait for it !\n",agentName);
    }
    
    return result;
}


////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////

//start, stop & kill the agent
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
            frindly_name = (char *)malloc( BUFSIZ );
            count = wcstombs(frindly_name, pCurrAddresses->FriendlyName, BUFSIZ );
            
            //If the friendly_name is the same of the networkDevice
            if (strcmp(zEl->networkDevice, frindly_name) == 0)
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
                                zEl->ipAddress = strdup(inet_ntop(AF_INET,&(sa_in->sin_addr),buff,bufflen));
                                free(frindly_name);
                                mtic_debug("Connection on ip address %s on device %s\n", zEl->ipAddress, zEl->networkDevice);
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
}//TODO: warning si agent name pas défini


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
}//TODO: warning si agent name pas défini

int mtic_stop(){
    if (agentElements != NULL){
        //interrupting and destroying mastic thread
        zstr_sendx (agentElements->agentActor, "$TERM", NULL);
        zactor_destroy (&agentElements->agentActor);
        //cleaning agent
        free (agentElements);
        agentElements = NULL;
        //cleaning pause callbacks
        pauseCallback_t *elt, *tmp;
        DL_FOREACH_SAFE(pauseCallbacks,elt,tmp) {
            DL_DELETE(pauseCallbacks,elt);
            free(elt);
        }
    }else{
        mtic_debug("agent already stopped\n");
    }
    
    return 1;
}

void mtic_die(){
    mtic_stop();
    exit(1);
}

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


//pause and resume the agent
int mtic_pause(){
    if(isPaused == false)
    {
        mtic_debug("Agent paused\n");
        isPaused = true;
        pauseCallback_t *elt;
        DL_FOREACH(pauseCallbacks,elt){
            elt->callback_ptr(isPaused, elt->myData);
        }
    }
    return 1;
}

bool mtic_isPaused(){
    return isPaused;
}

int mtic_resume(){
    if(isPaused == true)
    {
        mtic_debug("Agent resumed\n");
        isPaused = false;
        pauseCallback_t *elt;
        DL_FOREACH(pauseCallbacks,elt){
            elt->callback_ptr(isPaused, elt->myData);
        }
    }
    return 1;
}

int mtic_observePause(mtic_pauseCallback cb, void *myData){
    if (cb != NULL){
        pauseCallback_t *newCb = calloc(1, sizeof(pauseCallback_t));
        newCb->callback_ptr = cb;
        newCb->myData = myData;
        DL_APPEND(pauseCallbacks, newCb);
    }else{
        mtic_debug("callback is null\n");
        return 0;
    }
    return 1;
}

//control agent state
int mtic_setAgentState(const char *state){
    if (strcmp(state, agentState) != 0){
        strncpy(agentState, state, AGENT_NAME_LENGTH);
    }
    return 1;
}

char *mtic_getAgentState(){
    return strdup(agentState);
}

//set library parameters
void mtic_setVerbose (bool verbose){
    verboseMode = verbose;
};




