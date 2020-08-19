//
//  common.h
//  testing
//
//  Created by Stephane Vales on 17/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#ifndef common_h
#define common_h

#include <stdio.h>
#include <ingescape/ingescape_agent.h>

extern unsigned int port;
extern char *agentName;
extern char *networkDevice;
extern bool verbose;

extern bool myBool;
extern int myInt;
extern double myDouble;
extern char *myString;
extern void *myData;
extern void *myOtherData;

#define BUFFER_SIZE 4096

void myIOPCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData);
void myCallCallback(const char *senderAgentName, const char *senderAgentUUID,
                    const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                    void* myData);
void myBusCallback(const char *event, const char *peerID, const char *name,
                   const char *address, const char *channel,
                   zhash_t *headers, zmsg_t *msg, void *myData);

void agentIOPCallback(igs_agent_t *agent, iop_t iopType, const char* name, iopType_t valueType,
                      void* value, size_t valueSize, void* myData);
void agentCallCallback(igs_agent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                       const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                       void* myData);


void print_usage(void);
void makeFilePath(char *from, char *to, size_t size_of_to);
void print_cli_usage(void);

void publishCommand(void);
void callCommand(void);
void busCommand(void);
void publishCommandSparing(void);
void callCommandSparing(void);
void busCommandSparing(void);
void gossipCommand(void);
void gossipCommandSparing(void);
void securityCommand(void);
void securityCommandSparing(void);

void editorCommand(const char *agentUUID, const char *input);

#endif /* common_h */
