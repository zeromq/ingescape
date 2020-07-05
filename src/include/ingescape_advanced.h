//
//  ingescape_advanced.h
//  ingescape - https://ingescape.com
//
//  Created by Stephane Vales on 07/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_advanced_h
#define ingescape_advanced_h

#include <czmq.h>
#include "ingescape.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STRING_MSG_LENGTH 4096

//////////////////////////////////////////////////
// Advanced admin functions

//Start the agent using a broker instead of selfdiscovery
//NB: an ingescape broker must be running and available at the provided endpoint.
//Endpoints have the form tcp://ip_address:port
//Selected network device must be able to reach the endpoint address.
//PUBLIC int igs_startWithDeviceOnBroker(const char *networkDevice, const char *brokerEndpoint);
    
//network configuration
//void igs_setBusEndpoint(const char *endpoint); //usefull only with gossip discovery - TODO
//void igs_connectAgentOnEndpoint(const char *endpoint); //not officially supported in Zyre 2.0.x yet
PUBLIC void igs_setPublishingPort(unsigned int port);
PUBLIC void igs_setLogStreamPort(unsigned int port);
PUBLIC void igs_setDiscoveryInterval(unsigned int interval); //in milliseconds
PUBLIC void igs_setAgentTimeout(unsigned int duration); //in milliseconds

//sends number of messages with defiend size and displays performance
//information when finished (information displayed as INFO-evel log)
PUBLIC void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg);

//Timers can be created to call code a certain number of times,
//each time after a certain delay. 0 times means repeating forever.
//Timers must be created after starting an agent.
typedef void (igs_timerCallback) (int timerId, void *myData);
PUBLIC int igs_timerStart(size_t delay, size_t times, igs_timerCallback cb, void *myData); //returns timer id or -1 if error
PUBLIC void igs_timerStop(int timerId);

//Set high water marks (HWM) for the publish/subscribe sockets.
//Setting HWM to 0 means that they are disabled.
PUBLIC void igs_setHighWaterMarks(int hwmValue);

//IngeScape provides an integrated monitor to detect events relative to the network
//Warning: once igs_monitoringEnable has been called, igs_monitoringDisable must be
//called to actually stop the monitor. If not stopped, it may cause an error when
//an agent terminates.
PUBLIC void igs_monitoringEnable(unsigned int period); //in milliseconds
PUBLIC void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port);
PUBLIC void igs_monitoringDisable(void);
PUBLIC bool igs_isMonitoringEnabled(void);
//When the monitor is started and igs_monitoringShallStartStopAgent is set to true :
// - IP change will cause the agent to restart on the new IP (same device, same port)
// - Network device disappearance will cause the agent to stop. Agent will restart when device is back.
PUBLIC void igs_monitoringShallStartStopAgent(bool flag);
typedef enum {
    IGS_NETWORK_OK = 1, //Default status when the monitor starts
    IGS_NETWORK_DEVICE_NOT_AVAILABLE, //Status when our network device is not available
    IGS_NETWORK_ADDRESS_CHANGED, //Status when the IP address of our network device has changed
    IGS_NETWORK_OK_AFTER_MANUAL_RESTART // Status when our agent has been manually restarted and is now OK
} igs_monitorEvent_t;
typedef void (*igs_monitorCallback)(igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData);
PUBLIC void igs_monitor(igs_monitorCallback cb, void *myData);


//////////////////////////////////////////////////
// Data serialization using ZeroMQ
//TODO: give code examples here or link to documentation for zmsg and zframe
PUBLIC int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg);
PUBLIC int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy


//////////////////////////////////////////////////
// Bus channels
typedef void (*igs_BusMessageIncoming) (const char *event, const char *peerID, const char *name,
                                        const char *address, const char *channel,
                                        zhash_t *headers, zmsg_t *msg, void *myData);
PUBLIC int igs_observeBus(igs_BusMessageIncoming cb, void *myData);

PUBLIC void igs_busJoinChannel(const char *channel);
PUBLIC void igs_busLeaveChannel(const char *channel);

PUBLIC int igs_busSendStringToChannel(const char *channel, const char *msg, ...);
PUBLIC int igs_busSendDataToChannel(const char *channel, void *data, size_t size);
PUBLIC int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p); //destroys message after sending it

// the functions below support mutiple agents with same name
PUBLIC int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...);
PUBLIC int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size);
PUBLIC int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p); //destroys message after sending it

PUBLIC void igs_busAddServiceDescription(const char *key, const char *value);
PUBLIC void igs_busRemoveServiceDescription(const char *key);


//////////////////////////////////////////////////
//Calls : create, remove, call, react
/*NOTES:
 - one and only one mandatory callback per call, set using igs_handleCall : generates warning if cb missing when loading definition or receiving call
 - one optional reply per call
 - reply shall be sent in callabck, using igs_sendCall with sender's UUID or name
 - call names shall be unique for a given agent
 */

//SEND CALLS to other agents
//call arguments are provided as a chained list
typedef struct igs_callArgument{
    char *name;
    iopType_t type;
    union{
        bool b;
        int i;
        double d;
        char *c;
        void *data;
    };
    size_t size;
    struct igs_callArgument *next;
} igs_callArgument_t;

//Arguments management
//arguments list shall be initialized to NULL and filled by calling igs_add*ToArgumentsList
//Example:
// igs_callArgument_t *list = NULL;
// igs_addIntToArgumentsList(&list, 10);
PUBLIC void igs_addIntToArgumentsList(igs_callArgument_t **list, int value);
PUBLIC void igs_addBoolToArgumentsList(igs_callArgument_t **list, bool value);
PUBLIC void igs_addDoubleToArgumentsList(igs_callArgument_t **list, double value);
PUBLIC void igs_addStringToArgumentsList(igs_callArgument_t **list, const char *value);
PUBLIC void igs_addDataToArgumentsList(igs_callArgument_t **list, void *value, size_t size);
PUBLIC void igs_destroyArgumentsList(igs_callArgument_t **list);
PUBLIC igs_callArgument_t *igs_cloneArgumentsList(igs_callArgument_t *list);

//SEND a call to another agent
//requires to pass agent name or UUID, call name and a list of arguments specific to the call
//passed arguments list will be deallocated and destroyed
PUBLIC int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list);


//CREATE CALLS for our agent
//callback model to handle calls received by our agent
typedef void (*igs_callFunction)(const char *senderAgentName, const char *senderAgentUUID,
                                 const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                                 void* myData);


//MANAGE calls supported by our agent
//Calls can be created either by code or by loading a definition. The function below will
//create a call if it does not exist or will attach callback and data if they are
//stil undefined. Warning: only one callback can be attached to a call (further attempts
//will be ignored and signaled by an error log).
PUBLIC int igs_initCall(const char *name, igs_callFunction cb, void *myData);
PUBLIC int igs_removeCall(const char *name);
PUBLIC int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type);
PUBLIC int igs_removeArgumentFromCall(const char *callName, const char *argName); //removes first occurence with this name


//MANAGE optional reply
//NB: a reply can be seen as a subcall used to answer to sender upon call reception.
//PUBLIC int igs_addReplyToCall(const char *callName, const char *replyName);
//PUBLIC int igs_addArgumentToReplyForCall(const char *callName, const char *argName, iopType_t type);
//PUBLIC int igs_removeArgumentFromReplyForCall(const char *callName, const char *argName);
//PUBLIC int igs_removeReplyFromCall(const char *callName); //reply elements will be destroyed as well

//introspection for calls, arguments and replies
PUBLIC size_t igs_getNumberOfCalls(void);
PUBLIC bool igs_checkCallExistence(const char *name);
PUBLIC char** igs_getCallsList(size_t *nbOfElements); //returned char** shall be freed by caller
PUBLIC void igs_freeCallsList(char **list, size_t nbOfCalls);

PUBLIC igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName);
PUBLIC size_t igs_getNumberOfArgumentsForCall(const char *callName);
PUBLIC bool igs_checkCallArgumentExistence(const char *callName, const char *argName);
//PUBLIC igs_callArgument_t* igs_getFirstArgumentForReplyInCall(const char *callName);
//PUBLIC size_t igs_getNumberOfArgumentsForReplyInCall(const char *callName);
//PUBLIC char* igs_getReplyNameInCall(const char *callName); //returned char* must be freed by caller, NULL if no reply
//PUBLIC bool igs_isReplyAddedForCall(const char *name);
//PUBLIC bool igs_checkCallReplyArgumentExistence(const char *callName, const char *argName);


//////////////////////////////////////////////////
//JSON facilities
typedef struct _igsJSON* igsJSON_t;
PUBLIC void igs_JSONfree(igsJSON_t *json);

// generate a JSON string
PUBLIC igsJSON_t igs_JSONinit(void); //must call igs_JSONfree on returned value to free it
PUBLIC void igs_JSONopenMap(igsJSON_t json);
PUBLIC void igs_JSONcloseMap(igsJSON_t json);
PUBLIC void igs_JSONopenArray(igsJSON_t json);
PUBLIC void igs_JSONcloseArray(igsJSON_t json);
PUBLIC void igs_JSONaddNULL(igsJSON_t json);
PUBLIC void igs_JSONaddBool(igsJSON_t json, bool value);
PUBLIC void igs_JSONaddInt(igsJSON_t json, long long value);
PUBLIC void igs_JSONaddDouble(igsJSON_t json, double value);
PUBLIC void igs_JSONaddString(igsJSON_t json, const char *value);
PUBLIC void igs_JSONprint(igsJSON_t json);
PUBLIC char* igs_JSONdump(igsJSON_t json); //returned value must be freed by caller

// parse a JSON string or file based on parsing events and a callback
typedef enum {
    IGS_JSON_STRING = 1,
    IGS_JSON_NUMBER, //int or double
    IGS_JSON_MAP,
    IGS_JSON_ARRAY,
    IGS_JSON_TRUE, //not used in parsing callback
    IGS_JSON_FALSE, //not used in parsing callback
    IGS_JSON_NULL,
    IGS_JSON_KEY = 9, //not used in tree queries
    IGS_JSON_MAP_END, //not used in tree queries
    IGS_JSON_ARRAY_END, //not used in tree queries
    IGS_JSON_BOOL //not used in tree queries
} igs_JSONValueType_t;
typedef void (*igs_JSONCallback)(igs_JSONValueType_t type, void *value, size_t size, void *myData);
PUBLIC void igs_JSONparseFromFile(const char *path, igs_JSONCallback cb, void *myData);
PUBLIC void igs_JSONparseFromString(const char *content, igs_JSONCallback cb, void *myData);

// parse a JSON string or file in a tree supporting queries
typedef struct igsJSONNode {
    igs_JSONValueType_t type;
    union {
        char * string;
        struct {
            long long i; //integer value, if representable
            double  d;   //double value, if representable
            char   *r;   //unparsed number in string form
            unsigned int flags; //flags to manage double and int values
        } number;
        struct {
            const char **keys; //array of keys
            struct igsJSONNode **values; //array of nodes
            size_t len; //number of key-node-pairs
        } object;
        struct {
            struct igsJSONNode **values; //array of nodes
            size_t len; //number of nodes
        } array;
    } u;
} igsJSONTreeNode_t;
PUBLIC void igs_JSONTreeFree(igsJSONTreeNode_t **node);
PUBLIC igsJSONTreeNode_t* igs_JSONTreeParseFromFile(const char *path);
PUBLIC igsJSONTreeNode_t* igs_JSONTreeParseFromString(const char *content);
PUBLIC igsJSONTreeNode_t* igs_JSONTreeClone(igsJSONTreeNode_t *tree); //returned value must be freed by caller
PUBLIC char* igs_JSONTreeDump(igsJSONTreeNode_t *tree); //returned value must be freed by caller

PUBLIC void igs_JSONaddTree(igsJSON_t json, igsJSONTreeNode_t *tree);
PUBLIC igsJSONTreeNode_t* igs_JSONgetTree(igsJSON_t json); //returned value must be freed by caller

//add node at the end of an array
PUBLIC void igs_JSONTreeInsertInArray(igsJSONTreeNode_t *array, igsJSONTreeNode_t *nodeToInsert); //does NOT take ownership of node to insert

//add node (or replace if it already existis) in map based on named key
PUBLIC void igs_JSONTreeInsertInMap(igsJSONTreeNode_t *map, const char *key, igsJSONTreeNode_t *nodeToInsert); //does NOT take ownership of node to insert

/* Tree node can handle queries to retrieve sub-nodes
 Important notes :
 - returned value must NOT be freed manually : it is owned by the node
 - returned structure contains a type that shall be checked to handle actual contained value(s)
 */
PUBLIC igsJSONTreeNode_t* igs_JSONTreeGetNodeAtPath(igsJSONTreeNode_t *node, const char **path);

//JSON parsing creates number values. Use these two additional functions
//to check them as int and double values.
//NB: int values are considered both int and double
PUBLIC bool igs_JSONTreeIsValueAnInteger(igsJSONTreeNode_t *value);
PUBLIC bool igs_JSONTreeIsValueADouble(igsJSONTreeNode_t *value);


//////////////////////////////////////////////////
//security
//TODO when officially supported in Zyre 2.x.x

#ifdef __cplusplus
}
#endif

#endif /* ingescape_advanced_h */
