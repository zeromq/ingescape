//
//  common.h
//  testing
//
//  Created by Stephane Vales on 17/07/2020.
//  Copyright © 2020 Ingenuity i/o. All rights reserved.
//

#ifndef common_h
#define common_h

#if defined (__WINDOWS__)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    #include <winsock2.h>
#endif

#include <stdio.h>
#include "igsagent.h"

extern unsigned int port;
const extern char *agentName;
const extern char *networkDevice;
extern bool verbose;

extern bool myBool;
extern int myInt;
extern double myDouble;
extern char *myString;
extern void *myData;
extern void *myOtherData;

#define BUFFER_SIZE 4096

void myIOPCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType, void* value, size_t valueSize, void* myData);
void myServiceCallback(const char *senderAgentName, const char *senderAgentUUID,
                       const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                       const char *token, void* myCbData);
void myChannelsCallback(const char *event, const char *peerID, const char *name,
                        const char *address, const char *channel,
                        zhash_t *headers, zmsg_t *msg, void *myData);

void agentIOPCallback(igsagent_t *agent, igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
                      void* value, size_t valueSize, void* myData);
void agentServiceCallback(igsagent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData);


void print_usage(const char *agentName);
void makeFilePath(char *from, char *to, size_t size_of_to);
void print_cli_usage(void);

void publishCommand(void);
void servicesCommand(void);
void channelsCommand(void);
void publishCommandSparing(void);
void servicesCommandSparing(void);
void channelsCommandSparing(void);
void brokerCommand(char * ip_address);
void brokerCommandSparing(char * ip_address);
void securityCommand(char * ip_address);
void securityCommandSparing(char * ip_address);

void editorCommand(const char *agentUUID, const char *input);

#endif /* common_h */
