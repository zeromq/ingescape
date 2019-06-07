//
//  ingescape_advanced.h
//  ingescape - https://ingescape.com
//
//  Created by Stephane Vales on 07/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_advanced_h
#define ingescape_advanced_h

#if (defined WIN32 || defined _WIN32)
#if defined INGESCAPE
#define PUBLIC __declspec(dllexport)
#elif defined INGESCAPE_FROM_PRI
#define PUBLIC
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

#include <czmq.h>
#include "ingescape.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_STRING_MSG_LENGTH 4096

//Start the agent using a broker instead of selfdiscovery
//NB: an ingescape broker must be running and available at the provided endpoint.
//Endpoints have the form tcp://ip_address:port
//Selected network device must be able to reach the endpoint address.
//PUBLIC int igs_startWithDeviceOnBroker(const char *networkDevice, const char *brokerEndpoint);
    
//network configuration and monitoring
//void igs_setBusEndpoint(const char *endpoint); //usefull only with gossip discovery - TODO
//void igs_connectAgentOnEndpoint(const char *endpoint); //not officially supported in Zyre 2.0.x yet
PUBLIC void igs_setPublishingPort(unsigned int port);
PUBLIC void igs_setDiscoveryInterval(unsigned int interval); //in milliseconds
PUBLIC void igs_setAgentTimeout(unsigned int duration); //in milliseconds

//IngeScape provides an integrated monitor to detect events relative to the network
//Warning: once igs_monitoringEnable has been called, igs_monitoringDisable must be
//called to actually stop the monitor. If not stopped, it may cause an error when
//an agent terminates.
PUBLIC void igs_monitoringEnable(unsigned int period); //in milliseconds
PUBLIC void igs_monitoringDisable(void);
//When the monitor is started and igs_monitoringShallStartStopAgent is set to true :
// - IP change will cause the agent to restart on the new IP (same device, same port)
// - Network device disappearance will cause the agent to stop. Agent will restart when device is back.
PUBLIC void igs_monitoringShallStartStopAgent(bool flag);
typedef enum {
    IGS_NETWORK_OK = 1, //Default status when the monitor starts
    IGS_NETWORK_DEVICE_NOT_AVAILABLE,
    IGS_NETWORK_ADDRESS_CHANGED
} igs_monitorEvent_t;
typedef void (*igs_monitorCallback)(igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData);
PUBLIC void igs_monitor(igs_monitorCallback cb, void *myData);
    
//////////////////////////////////////////////////
//data serialization using ZeroMQ
//TODO: give code examples here or link to documentation for zmsg and zframe
PUBLIC int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg);
PUBLIC int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy


//////////////////////////////////////////////////
//internal bus
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
//Tokens Model : create, remove, call, react
/*NOTES:
 - one and only one mandatory callback per token, set using igs_handleToken : generates warning if cb missing when loading definition or receiving token
 - one optional reply per token
 - reply shall be sent in callabck, using igs_sendToken with sender's UUID or name
 - token names shall be unique for a given agent
 */

//SEND TOKENS to other agents
//token arguments are provided as a chained list
typedef struct igs_tokenArgument{
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
    struct igs_tokenArgument *next;
} igs_tokenArgument_t;

//Arguments management
//arguments list shall be initialized to NULL and filled by calling igs_add*ToArgumentsList
//Example:
// igs_tokenArgument_t *list = NULL;
// igs_addIntToArgumentsList(&list, 10);
PUBLIC void igs_addIntToArgumentsList(igs_tokenArgument_t **list, int value);
PUBLIC void igs_addBoolToArgumentsList(igs_tokenArgument_t **list, bool value);
PUBLIC void igs_addDoubleToArgumentsList(igs_tokenArgument_t **list, double value);
PUBLIC void igs_addStringToArgumentsList(igs_tokenArgument_t **list, const char *value);
PUBLIC void igs_addDataToArgumentsList(igs_tokenArgument_t **list, void *value, size_t size);
PUBLIC void igs_destroyArgumentsList(igs_tokenArgument_t **list);
PUBLIC igs_tokenArgument_t *igs_cloneArgumentsList(igs_tokenArgument_t *list);

//send a token to another agent
//requires to pass agent name or UUID, token name and a list of arguments
//passed arguments list will be deallocated and destroyed
PUBLIC int igs_sendToken(const char *agentNameOrUUID, const char *tokenName, igs_tokenArgument_t **list);


//CREATE TOKENS for our agent
//callback model to handle tokens received by our agent
typedef void (*igs_tokenCallback)(const char *senderAgentName, const char *senderAgentUUID,
                                  const char *tokenName, igs_tokenArgument_t *firstArgument, size_t nbArgs,
                                  void* myData);


//manage tokens supported by our agent
//Tokens can be created either by code or by loading a definition. The function below will
//create a token if it does not exist or will attach callback and data if they are
//stil undefined. Warning: only one callback can be attached to a token (further attempts
//will be ignored and signaled by an error log).
PUBLIC int igs_initToken(const char *name, igs_tokenCallback cb, void *myData);
PUBLIC int igs_removeToken(const char *name);
PUBLIC int igs_addArgumentToToken(const char *tokenName, const char *argName, iopType_t type);
PUBLIC int igs_removeArgumentFromToken(const char *tokenName, const char *argName); //removes first occurence with this name


//manage optional reply
//NB: a reply can be seen as a subtoken used to answer to sender upon token reception.
//PUBLIC int igs_addReplyToToken(const char *tokenName, const char *replyName);
//PUBLIC int igs_addArgumentToReplyForToken(const char *tokeName, const char *argName, iopType_t type);
//PUBLIC int igs_removeArgumentFromReplyForToken(const char *tokeName, const char *argName);
//PUBLIC int igs_removeReplyFromToken(const char *tokenName); //reply elements will be destroyed as well

//introspection for tokens, arguments and replies
PUBLIC size_t igs_getNumberOfTokens(void);
PUBLIC bool igs_checkTokenExistence(const char *name);
PUBLIC char** igs_getTokensList(size_t *nbOfElements); //returned char** shall be freed by caller
PUBLIC void igs_freeTokensList(char **list, size_t nbOfTokens);

PUBLIC igs_tokenArgument_t* igs_getFirstArgumentForToken(const char *tokenName);
PUBLIC size_t igs_getNumberOfArgumentsForToken(const char *tokenName);
PUBLIC bool igs_checkTokenArgumentExistence(const char *tokenName, const char *argName);
//PUBLIC igs_tokenArgument_t* igs_getFirstArgumentForReplyInToken(const char *tokenName);
//PUBLIC size_t igs_getNumberOfArgumentsForReplyInToken(const char *tokenName);
//PUBLIC char* igs_getReplyNameInToken(const char *tokenName); //returned char* must be freed by caller, NULL if no reply
//PUBLIC bool igs_isReplyAddedForToken(const char *name);
//PUBLIC bool igs_checkTokenReplyArgumentExistence(const char *tokenName, const char *argName);

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
    IGS_JSON_NULL = 1,
    IGS_JSON_STRING,
    IGS_JSON_KEY,
    IGS_JSON_NUMBER, //FIXME int vs double
    IGS_JSON_BOOL,
    IGS_JSON_MAP_START,
    IGS_JSON_MAP_END,
    IGS_JSON_ARRAY_START,
    IGS_JSON_ARRAY_END
} igs_JSONValueType_t;
typedef void (*igs_JSONCallback)(igs_JSONValueType_t type, void *value, size_t size, void *myData);
PUBLIC void igs_JSONparseFromFile(const char *path, igs_JSONCallback cb, void *myData);
PUBLIC void igs_JSONparseFromString(const char *content, igs_JSONCallback cb, void *myData);

//////////////////////////////////////////////////
//security
//TODO when officially supported in Zyre 2.x.x

#ifdef __cplusplus
}
#endif

#endif /* ingescape_advanced_h */
