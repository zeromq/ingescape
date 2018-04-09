//
//  mastic_advanced.h
//  mastic
//
//  Created by Stephane Vales on 07/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#ifndef mastic_advanced_h
#define mastic_advanced_h

#if defined WINDOWS
#if defined MASTIC
#define PUBLIC __declspec(dllexport)
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

#include <zyre.h>

#define MAX_STRING_MSG_LENGTH 4096

//network configuration
//void mtic_setBusEndpoint(const char *endpoint); //usefull only with gossip discovery - TODO
//void mtic_connectAgentOnEndpoint(const char *endpoint); //not officially supported in Zyre 2.0.x yet
PUBLIC void mtic_setPublishingPort(unsigned int port); //warning if called after start
PUBLIC void mtic_setDiscoveryInterval(unsigned int interval); //in milliseconds
PUBLIC void mtic_setAgentTimeout(unsigned int duration); //in milliseconds


//data serialization using ZMQ
//give code examples here or link to documentation for zmsg and zframe
PUBLIC int mtic_writeOutputAsZMQMsg(const char *name, zmsg_t *msg);
PUBLIC int mtic_readInputAsZMQMsg(const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy


//ZMQ internal bus
typedef void (*mtic_BusMessageIncoming) (const char *event, const char *peer, const char *name,
                                         const char *address, const char *channel,
                                         zhash_t *headers, zmsg_t *msg, void *myData);
PUBLIC int mtic_observeBus(mtic_BusMessageIncoming cb, void *myData);

PUBLIC void mtic_busJoinChannel(const char *channel);
PUBLIC void mtic_busLeaveChannel(const char *channel);

PUBLIC int mtic_busSendStringToChannel(const char *channel, const char *msg, ...);
PUBLIC int mtic_busSendDataToChannel(const char *channel, void *data, long size);
PUBLIC int mtic_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p); //destroys message after sending it

// the functions below support mutiple agents with same name
PUBLIC int mtic_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...);
PUBLIC int mtic_busSendDataToAgent(const char *agentNameOrPeerID, void *data, unsigned long size);
PUBLIC int mtic_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p); //destroys message after sending it

PUBLIC void mtic_busAddServiceDescription(const char *key, const char *value);

//security
//TODO when officially supported in Zyre 2.0.x

#endif /* mastic_advanced_h */
