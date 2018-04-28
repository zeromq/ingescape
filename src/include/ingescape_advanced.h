//
//  ingescape_advanced.h
//  ingescape
//
//  Created by Stephane Vales on 07/04/2018.
//  Copyright © 2018 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_advanced_h
#define ingescape_advanced_h

#if defined WINDOWS
#if defined INGESCAPE
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
//void igs_setBusEndpoint(const char *endpoint); //usefull only with gossip discovery - TODO
//void igs_connectAgentOnEndpoint(const char *endpoint); //not officially supported in Zyre 2.0.x yet
PUBLIC void igs_setPublishingPort(unsigned int port);
PUBLIC void igs_setDiscoveryInterval(unsigned int interval); //in milliseconds
PUBLIC void igs_setAgentTimeout(unsigned int duration); //in milliseconds


//data serialization using ZMQ
//give code examples here or link to documentation for zmsg and zframe
PUBLIC int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg);
PUBLIC int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy


//ZMQ internal bus
typedef void (*igs_BusMessageIncoming) (const char *event, const char *peer, const char *name,
                                         const char *address, const char *channel,
                                         zhash_t *headers, zmsg_t *msg, void *myData);
PUBLIC int igs_observeBus(igs_BusMessageIncoming cb, void *myData);

PUBLIC void igs_busJoinChannel(const char *channel);
PUBLIC void igs_busLeaveChannel(const char *channel);

PUBLIC int igs_busSendStringToChannel(const char *channel, const char *msg, ...);
PUBLIC int igs_busSendDataToChannel(const char *channel, void *data, long size);
PUBLIC int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p); //destroys message after sending it

// the functions below support mutiple agents with same name
PUBLIC int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...);
PUBLIC int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, unsigned long size);
PUBLIC int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p); //destroys message after sending it

PUBLIC void igs_busAddServiceDescription(const char *key, const char *value);
PUBLIC void igs_busRemoveServiceDescription(const char *key);

//security
//TODO when officially supported in Zyre 2.0.x

#endif /* ingescape_advanced_h */
