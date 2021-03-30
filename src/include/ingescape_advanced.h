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


//////////////////////////////////////////////////
/*BROKERS VS. SELF-DISCOVERY
 
 igs_startWithDevice and igs_startWithIP enable the agents to self-discover
 using UDP broadcast messages on the passed port. UDP broadcast messages can
 be blocked on some networks and can make things complex on networks with
 sub-networks.
 That is why ingescape also supports the use of brokers to relay discovery
 using TCP connections. Any agent can be a broker and agents using brokers
 simply have to use a list of broker endpoints. One broker is enough but
 several brokers can be set for robustness.
 
 For clarity, it is better if brokers are well identified on your platform,
 started before any agent, and serve only as brokers. But any other architecture
 is permitted and brokers can be restarted at any time.
 
 Endpoints have the form tcp://ip_address:port
 • A broker endpoint in igs_brokerAdd is used to connect to a given broker. Add
 as many brokers as you want. At least one declared broker is necessary to
 use igs_startWithBrokers.
 • The endpoint in igs_enableAsBroker is the broker address we should be reached
 at as a broker if we want to be one. Using igs_setAsBroker makes us a broker
 when starting.
 • The endpoint in igs_brokerAdvertiseEndpoint replaces the one declared in
 igs_startWithBrokers for the registration to the brokers. This function enables
 passing through NAT and using a public address. Attention: this public address
 shall make sense to all the agents that will connect to us, independently from
 their local network.
 • Our agent endpoint in igs_startWithBrokers gives the address and port our
 agent can be reached at. This endpoint must be valid in the actual network
 configuration.
 */
PUBLIC igs_result_t igs_brokerAdd(const char *brokerEndpoint);
PUBLIC void igs_enableAsBroker(const char *ourBrokerEndpoint);
PUBLIC void igs_brokerAdvertiseEndpoint(const char *advertisedEndpoint); //parameter can be NULL
PUBLIC igs_result_t igs_startWithBrokers(const char *agentEndpoint);


//////////////////////////////////////////////////
/* SECURITY
 Security is about authentification of other agents and encrypted communications.
 Both are offered by Ingescape with a public/private certificates mechanism relying
 on ZeroMQ. Security is activated optionally.
 When security is enabled :
 • When private certificates are generated on the fly by Ingescape, it provides the
 same protection as TLS for HTTPS communications. Thirdparties cannot steal identities
 and  communications are encrypted end-to-end. But any Ingescape agent with security
 enabled can join a platform.
 • When private certificates are file-based and secretly owned by each agent, no third
 party can join a platform without providing an identity that is well-known by the other
 agents using public certificates. This is safer but requires securing private certificates
 individually and sharing public certificates between all agents.
 
 Security is enabled by calling igs_enableSecurity.
 • If privateCertificateFile is NULL, our private certificate is generated on the fly, and
 any agent with security enabled will be able to connect to us. Any value provided for
 publicCertificatesDirectory will be ignored. This is the equivalent of HTTPS/TLS end-to-end
 encryption without access restriction.
 • If privateCertificateFile is NOT NULL, the private certificate at privateCertificateFile
 path will be used and only agents whose public certificates are in publicCertificatesDirectory
 will be able to connect to us: this is end-ti-end encryption + authentication.
 NB: if privateCertificateFile is NOT NULL and publicCertificatesDirectory is NULL or does not
 exist, security will not be enabled and our agent will not start.
*/
PUBLIC igs_result_t igs_enableSecurity(const char *privateCertificateFile, const char *publicCertificatesDirectory);
PUBLIC igs_result_t igs_brokerAddSecure(const char *brokerEndpoint, const char *publicCertificatePath);
PUBLIC zactor_t* igs_getZeroMQAuthenticator(void);


//////////////////////////////////////////////////
/* ELECTIONS between agents
 Create named contests between agents and designate a leader, as soon as they
 are two or more.
 • IGS_AGENT_WON_ELECTION means that the election is over and this agent has WON
 • IGS_AGENT_LOST_ELECTION means that the election is over and this agent has LOST
 • When only one agent participates in an election, the election does not happen.
 • When only one agent remains for an election, the election does not happen.
 At startup, this means that developers must either start their agents as
 leaders or wait a reasonable amount of time for an election to happen.
 During runtime, this means that developers shall rely on IGS_AGENT_EXITED
 events to check if they suddenly are alone in an election and thus shall
 become leaders.
 */
PUBLIC igs_result_t igs_competeInElection(const char *electionName);
PUBLIC igs_result_t igs_leaveElection(const char *electionName);


//////////////////////////////////////////////////
// ADVANCED ADMIN

//NETWORK CONFIGURATION
PUBLIC void igs_setPublishingPort(unsigned int port);
PUBLIC void igs_setLogStreamPort(unsigned int port);
PUBLIC void igs_setDiscoveryInterval(unsigned int interval); //in milliseconds
PUBLIC void igs_setAgentTimeout(unsigned int duration); //in milliseconds
PUBLIC void igs_raiseSocketsLimit(void); //UNIX only, to be called before any ingescape or ZeroMQ activity
//Set high water marks (HWM) for the publish/subscribe sockets.
//Setting HWM to 0 means that they are disabled.
PUBLIC void igs_setHighWaterMarks(int hwmValue);


/* PERFORMANCE CHECK
 sends number of messages with defined size and displays performance
 information when finished (information displayed as INFO-level log)*/
PUBLIC void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg);

/* AGENT FAMILY - optional
 32 characters canonical UUID format is commonly expected,
 default is an empty string, max length is 64 characters*/
PUBLIC void igs_setFamily(const char *family);
PUBLIC char* igs_getFamily(void); //char* must be freed by caller


/* TIMERS
 Timers can be created to call code a certain number of times,
 each time after a certain delay. 0 times means repeating forever.
 Timers must be created after starting an agent.*/
typedef void (igs_timerCallback) (int timerId, void *myData);
PUBLIC int igs_timerStart(size_t delay, size_t times, igs_timerCallback cb, void *myData); //returns timer id or -1 if error
PUBLIC void igs_timerStop(int timerId);


/* NETWORK MONITORING
 Ingescape provides an integrated monitor to detect events relative to the network.
 NB: once igs_monitoringEnable has been called, igs_monitoringDisable must be
 called to actually stop the monitor. If not stopped, it may cause an error when
 an agent terminates.*/
PUBLIC void igs_monitoringEnable(unsigned int period); //in milliseconds
PUBLIC void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port);
PUBLIC void igs_monitoringDisable(void);
PUBLIC bool igs_isMonitoringEnabled(void);
/* When the monitor is started and igs_monitoringShallStartStopAgent is set to true :
 - IP change will cause the agent to restart on the new IP (same device, same port)
 - Network device disappearance will cause the agent to stop. Agent will restart when device is back.*/
PUBLIC void igs_monitoringShallStartStopAgent(bool flag);
typedef enum {
    IGS_NETWORK_OK = 1, //when the monitor starts
    IGS_NETWORK_DEVICE_NOT_AVAILABLE, //when our network device is not available
    IGS_NETWORK_ADDRESS_CHANGED, //when the IP address of our network device has changed
    IGS_NETWORK_OK_AFTER_MANUAL_RESTART //when our agent has been manually restarted and is now OK
} igs_monitorEvent_t;
typedef void (*igs_monitorCallback)(igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData);
PUBLIC void igs_monitor(igs_monitorCallback cb, void *myData);


/* LOGS REPLAY
 Ingescape logs contain all the necessary information for an agent to replay
 its changes for inputs, outputs, parameters and calls.
 
 Replay happens in a dedicated thread created after calling igs_replayInit:
 • logFilePath : path to the log file to be read
 • speed : replay speed. Default is zero, meaning as fast as possible.
 • startTime : with format hh:mm::s, specifies the time when speed shall be used.
               Replay as fast as possible before that.
 • waitForStart : waits for a call to igs_replayStart before starting the replay. Default is false.
 • replayMode : a boolean composition of igs_replay_mode_t value to decide what shall be replayed.
                If mode is zero, all IOP and calls are replayed.
 • agent : an OPTIONAL agent name serving as filter when the logs contain activity for multiple agents.
 
 igs_replayTerminate cleans the thread and requires calling igs_replayInit again.
 Replay thread is cleaned automatically also when the log file has been read completely.
 */
typedef enum {
    IGS_REPLAY_INPUT = 1,
    IGS_REPLAY_OUTPUT = 2,
    IGS_REPLAY_PARAMETER = 4,
    IGS_REPLAY_RECEIVED_CALL = 8,
    IGS_REPLAY_SENT_CALL = 16

} igs_replay_mode_t;
PUBLIC void igs_replayInit(const char *logFilePath, size_t speed, const char *startTime,
                           bool waitForStart, uint replayMode, const char *agent);
PUBLIC void igs_replayStart(void);
PUBLIC void igs_replayPause(bool pause);
PUBLIC void igs_replayTerminate(void);

/*
 CLEAN CONTEXT
 Use this function when you absolutely need to clean the whole Ingescape context
 and you cannot stop your application to do so. This function SHALL NOT be used
 in production environments.
 */
PUBLIC void igs_cleanContext(void);

//////////////////////////////////////////////////
// DATA SERIALIZATION using ZeroMQ
/*
 These two functions enable sending and receiving on DATA
 inputs/outputs by using zmsg_t structures. zmsg_t structures
 offer advanced functionalities for data serialization.
 More can be found here: http://czmq.zeromq.org/manual:zmsg
 */
PUBLIC igs_result_t igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg);
PUBLIC igs_result_t igs_readInputAsZMQMsg(const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy


//////////////////////////////////////////////////
// BUS CHANNELS
typedef void (*igs_BusMessageIncoming) (const char *event, const char *peerID, const char *peerName,
                                        const char *address, const char *channel,
                                        zhash_t *headers, zmsg_t *msg, void *myData);
PUBLIC void igs_observeBus(igs_BusMessageIncoming cb, void *myData);

PUBLIC igs_result_t igs_busJoinChannel(const char *channel);
PUBLIC void igs_busLeaveChannel(const char *channel);

PUBLIC igs_result_t igs_busSendStringToChannel(const char *channel, const char *msg, ...);
PUBLIC igs_result_t igs_busSendDataToChannel(const char *channel, void *data, size_t size);
PUBLIC igs_result_t igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p); //destroys message after sending it

// Sending message to an agent by name or by uuid
//NB: peer ids and names are also supported by these functions but are used only if no agent is found first
//NB: if several agents share the same name, all will receive the message if addressed by name
PUBLIC igs_result_t igs_busSendStringToAgent(const char *agentNameOrAgentIdOrPeerID, const char *msg, ...);
PUBLIC igs_result_t igs_busSendDataToAgent(const char *agentNameOrAgentIdOrPeerID, void *data, size_t size);
PUBLIC igs_result_t igs_busSendZMQMsgToAgent(const char *agentNameOrAgentIdOrPeerID, zmsg_t **msg_p); //destroys message after sending it

PUBLIC igs_result_t igs_busAddServiceDescription(const char *key, const char *value);
PUBLIC igs_result_t igs_busRemoveServiceDescription(const char *key);


//////////////////////////////////////////////////
//CALLS : create, remove, call, react
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
//Requires to pass agent a name or UUID, a call name and a list of arguments specific to the call.
//Token is an optional information to specifically identify a call and help routing replies.
//Passed arguments list will be deallocated and destroyed by the function.
PUBLIC igs_result_t igs_sendCall(const char *agentNameOrUUID, const char *callName,
                                 igs_callArgument_t **list, const char *token);


//CREATE CALLS for our agent
//callback model to handle calls received by our agent
typedef void (*igs_callFunction)(const char *senderAgentName, const char *senderAgentUUID,
                                 const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                                 const char *token, void* myData);


//Manage calls supported by our agent
//Calls can be created either by code or by loading a definition. The function below will
//create a call if it does not exist or will attach callback and data if they are
//stil undefined. Warning: only one callback can be attached to a call (further attempts
//will be ignored and signaled by an error log).
PUBLIC igs_result_t igs_initCall(const char *name, igs_callFunction cb, void *myData);
PUBLIC igs_result_t igs_removeCall(const char *name);
PUBLIC igs_result_t igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type);
PUBLIC igs_result_t igs_removeArgumentFromCall(const char *callName, const char *argName); //removes first occurence with this name


//Manage optional reply
//NB: a reply can be seen as a subcall used to answer to sender upon call reception.
//PUBLIC int igs_addReplyToCall(const char *callName, const char *replyName);
//PUBLIC int igs_addArgumentToReplyForCall(const char *callName, const char *argName, iopType_t type);
//PUBLIC int igs_removeArgumentFromReplyForCall(const char *callName, const char *argName);
//PUBLIC int igs_removeReplyFromCall(const char *callName); //reply elements will be destroyed as well

//introspection for calls, arguments and replies
PUBLIC size_t igs_getNumberOfCalls(void);
PUBLIC bool igs_checkCallExistence(const char *name);
PUBLIC char** igs_getCallsList(size_t *nbOfElements); //returned char** shall be freed by caller
PUBLIC void igs_freeCallsList(char ***list, size_t nbOfCalls);

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
PUBLIC char* igs_JSONcompactDump(igsJSON_t json); //returned value must be freed by caller

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


#ifdef __cplusplus
}
#endif

#endif /* ingescape_advanced_h */
