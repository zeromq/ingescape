//
//  global.c
//  ingescape
//
//  Created by Stephane Vales on 18/11/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include "ingescape.h"
#include "ingescape_advanced.h"
#include "ingescape_agent.h"
#include "ingescape_private.h"

igs_agent_t *coreAgent = NULL;
//int igs_nbOfAgentsInProcess = 0;
//bool igs_shallRaiseFileDescriptorsLimit = true;

void globalforcedStopCB(igs_agent_t *agent, void *myData){
    IGS_UNUSED(myData)
    IGS_UNUSED(agent)
    igs_Interrupted = true;
}

void initInternalAgentIfNeeded(){
    if (coreAgent == NULL){
        coreAgent = igsAgent_new();
        igsAgent_observeForcedStop(coreAgent, globalforcedStopCB, NULL);
    }
}

typedef struct {
    igs_forcedStopCallback cb;
    void *myData;
} observeForcedStopCbWrapper_t;

void global_observeForcedStopCallback(igs_agent_t *agent, void *myData){
    IGS_UNUSED(agent)
    observeForcedStopCbWrapper_t *wrap = (observeForcedStopCbWrapper_t *)myData;
    wrap->cb(wrap->myData);
}

void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeForcedStopCbWrapper_t *wrap = calloc(1, sizeof(observeForcedStopCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeForcedStop(coreAgent, global_observeForcedStopCallback, wrap);
}

int igs_setAgentName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentName(coreAgent, name);
}

char *igs_getAgentName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentName(coreAgent);
}

int igs_setAgentState(const char *state){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentState(coreAgent, state);
}

char *igs_getAgentState(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentState(coreAgent);
}

int igs_mute(void){
    initInternalAgentIfNeeded();
    return igsAgent_mute(coreAgent);
}

int igs_unmute(void){
    initInternalAgentIfNeeded();
    return igsAgent_unmute(coreAgent);
}

bool igs_isMuted(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMuted(coreAgent);
}

typedef struct {
    igs_muteCallback cb;
    void *myData;
} observeMuteCbWrapper_t;

void global_observeMuteCallback(igs_agent_t *agent, bool isMuted, void *myData){
    IGS_UNUSED(agent)
    observeMuteCbWrapper_t *wrap = (observeMuteCbWrapper_t *)myData;
    wrap->cb(isMuted, wrap->myData);
}

int igs_observeMute(igs_muteCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeMuteCbWrapper_t *wrap = calloc(1, sizeof(observeMuteCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeMute(coreAgent, global_observeMuteCallback, wrap);
}

int igs_freeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_freeze(coreAgent);
}

bool igs_isFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_isFrozen(coreAgent);
}

int igs_unfreeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_unfreeze(coreAgent);
}

typedef struct {
    igs_freezeCallback cb;
    void *myData;
} observeFreezeCbWrapper_t;

void global_observeFreezeCallback(igs_agent_t *agent, bool isPaused, void *myData){
    IGS_UNUSED(agent)
    observeFreezeCbWrapper_t *wrap = (observeFreezeCbWrapper_t *)myData;
    wrap->cb(isPaused, wrap->myData);
}

int igs_observeFreeze(igs_freezeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeFreezeCbWrapper_t *wrap = calloc(1, sizeof(observeFreezeCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeFreeze(coreAgent, cb, wrap);
}

void igs_setCanBeFrozen(bool canBeFrozen){
    initInternalAgentIfNeeded();
    igsAgent_setCanBeFrozen(coreAgent, canBeFrozen);
}

bool igs_canBeFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_canBeFrozen(coreAgent);
}


//IOP
int igs_readInput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInput(coreAgent, name, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutput(coreAgent, name, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameter(coreAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsBool(coreAgent, name);
}

int igs_readInputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsInt(coreAgent, name);
}

double igs_readInputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsDouble(coreAgent, name);
}

char* igs_readInputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsString(coreAgent, name);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsData(coreAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsBool(coreAgent, name);
}

int igs_readOutputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsInt(coreAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsDouble(coreAgent, name);
}

char* igs_readOutputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsString(coreAgent, name);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsData(coreAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsBool(coreAgent, name);
}

int igs_readParameterAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsInt(coreAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsDouble(coreAgent, name);
}

char* igs_readParameterAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsString(coreAgent, name);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsData(coreAgent, name, data, size);
}

int igs_writeInputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsBool(coreAgent, name, value);
}

int igs_writeInputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsInt(coreAgent, name, value);
}

int igs_writeInputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsDouble(coreAgent, name, value);
}

int igs_writeInputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsString(coreAgent, name, value);
}

int igs_writeInputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsImpulsion(coreAgent, name);
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsData(coreAgent, name, value, size);
}

int igs_writeOutputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsBool(coreAgent, name, value);
}

int igs_writeOutputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsInt(coreAgent, name, value);
}

int igs_writeOutputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsDouble(coreAgent, name, value);
}

int igs_writeOutputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsString(coreAgent, name, value);
}

int igs_writeOutputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsImpulsion(coreAgent, name);
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsData(coreAgent, name, value, size);
}

int igs_writeParameterAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsBool(coreAgent, name, value);
}

int igs_writeParameterAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsInt(coreAgent, name, value);
}

int igs_writeParameterAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsDouble(coreAgent, name, value);
}

int igs_writeParameterAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsString(coreAgent, name, value);
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsData(coreAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForInput(coreAgent, name);
}

void igs_clearDataForOutput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForOutput(coreAgent, name);
}

void igs_clearDataForParameter(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForParameter(coreAgent, name);
}

typedef struct {
    igs_observeCallback cb;
    void *myData;
} observeIOPCbWrapper_t;

void global_observeIOPCallback(igs_agent_t *agent, iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData){
    IGS_UNUSED(agent)
    observeIOPCbWrapper_t *wrap = (observeIOPCbWrapper_t *)myData;
    wrap->cb(iopType, name, valueType, value, valueSize, wrap->myData);
}

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeInput(coreAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeOutput(coreAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeParameter(coreAgent, name, global_observeIOPCallback, wrap);
}

int igs_muteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_muteOutput(coreAgent, name);
}

int igs_unmuteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_unmuteOutput(coreAgent, name);
}

bool igs_isOutputMuted(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_isOutputMuted(coreAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForInput(coreAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForOutput(coreAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForParameter(coreAgent, name);
}

int igs_getInputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsNumber(coreAgent);
}

int igs_getOutputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsNumber(coreAgent);
}

int igs_getParametersNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersNumber(coreAgent);
}

char** igs_getInputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsList(coreAgent, nbOfElements);
}

char** igs_getOutputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsList(coreAgent, nbOfElements);
}

char** igs_getParametersList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersList(coreAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkInputExistence(coreAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkOutputExistence(coreAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkParameterExistence(coreAgent, name);
}


//definition
int igs_loadDefinition (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinition(coreAgent, json_str);
}

int igs_loadDefinitionFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinitionFromPath(coreAgent, file_path);
}

int igs_clearDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearDefinition(coreAgent);
}

char* igs_getDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinition(coreAgent);
}

char *igs_getDefinitionName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionName(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionDescription(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionVersion(coreAgent);
}
 //returned char* must be freed by caller
int igs_setDefinitionName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionName(coreAgent, name);
}

int igs_setDefinitionDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionDescription(coreAgent, description);
}

int igs_setDefinitionVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionVersion(coreAgent, version);
}

int igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createInput(coreAgent, name, type, value, size);
}

int igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createOutput(coreAgent, name, type, value, size);
}

int igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createParameter(coreAgent, name, type, value, size);
}

int igs_removeInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeInput(coreAgent, name);
}

int igs_removeOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeOutput(coreAgent, name);
}

int igs_removeParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeParameter(coreAgent, name);
}


//mapping
int igs_loadMapping (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadMapping(coreAgent, json_str);
}

int igs_loadMappingFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadMappingFromPath(coreAgent, file_path);
}

int igs_clearMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearMapping(coreAgent);
}

char* igs_getMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(coreAgent);
}

char *igs_getMappingName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(coreAgent);
}

char *igs_getMappingDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingDescription(coreAgent);
}

char *igs_getMappingVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingVersion(coreAgent);
}

int igs_setMappingName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingName(coreAgent, name);
}

int igs_setMappingDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingDescription(coreAgent, description);
}

int igs_setMappingVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingVersion(coreAgent, version);
}

int igs_getMappingEntriesNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingEntriesNumber(coreAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_addMappingEntry(coreAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
int igs_removeMappingEntryWithId(unsigned long theId){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithId(coreAgent, theId);
}

int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithName(coreAgent, fromOurInput, toAgent, withOutput);
}


//admin

void igs_setRequestOutputsFromMappedAgents(bool notify){
    initInternalAgentIfNeeded();
    igsAgent_setRequestOutputsFromMappedAgents(coreAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    initInternalAgentIfNeeded();
    return igsAgent_getRequestOutputsFromMappedAgents(coreAgent);
}


void igs_setDefinitionPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setDefinitionPath(coreAgent, path);
}

void igs_setMappingPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setMappingPath(coreAgent, path);
}

void igs_writeDefinitionToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeDefinitionToPath(coreAgent);
}

void igs_writeMappingToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeMappingToPath(coreAgent);
}

void igs_log(igs_logLevel_t level, const char *function, const char *format, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(coreAgent, level, function, "%s", content);
}

//ADVANCED
int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsZMQMsg(coreAgent, name, msg);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsZMQMsg(coreAgent, name, msg);
}

typedef struct {
    igs_BusMessageIncoming cb;
    void *myData;
} observeBusCbWrapper_t;

void global_observeBusCallback(igs_agent_t *agent, const char *event, const char *peerID, const char *name,
                               const char *address, const char *channel,
                               zhash_t *headers, zmsg_t *msg, void *myData){
    IGS_UNUSED(agent)
    observeBusCbWrapper_t *wrap = (observeBusCbWrapper_t *)myData;
    wrap->cb(event, peerID, name, address, channel, headers, msg, wrap->myData);
}

int igs_observeBus(igs_BusMessageIncoming cb, void *myData){
    initInternalAgentIfNeeded();
    observeBusCbWrapper_t *wrap = calloc(1, sizeof(observeBusCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeBus(coreAgent, global_observeBusCallback, wrap);
}

void igs_busJoinChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busJoinChannel(coreAgent, channel);
}

void igs_busLeaveChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busLeaveChannel(coreAgent, channel);
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    int res = igsAgent_busSendStringToChannel(coreAgent, channel, msg, list);
    va_end(list);
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToChannel(coreAgent, channel, data, size);
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToChannel(coreAgent, channel, msg_p);
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    int res = igsAgent_busSendStringToAgent(coreAgent, agentNameOrPeerID, "%s", content);
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToAgent(coreAgent, agentNameOrPeerID, data, size);
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToAgent(coreAgent, agentNameOrPeerID, msg_p);
}

void igs_busAddServiceDescription(const char *key, const char *value){
    initInternalAgentIfNeeded();
    igsAgent_busAddServiceDescription(coreAgent, key, value);
}

void igs_busRemoveServiceDescription(const char *key){
    initInternalAgentIfNeeded();
    igsAgent_busRemoveServiceDescription(coreAgent, key);
}


int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    initInternalAgentIfNeeded();
    return igsAgent_sendCall(coreAgent, agentNameOrUUID, callName, list);
}

typedef struct {
    igs_callFunction cb;
    void *myData;
} callCbWrapper_t;

void global_callCallback(igs_agent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                         const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                         void* myData){
    IGS_UNUSED(agent)
    callCbWrapper_t *wrap = (callCbWrapper_t *)myData;
    wrap->cb(senderAgentName, senderAgentUUID, callName, firstArgument, nbArgs, wrap->myData);
}

int igs_initCall(const char *name, igs_callFunction cb, void *myData){
    initInternalAgentIfNeeded();
    callCbWrapper_t *wrap = calloc(1, sizeof(callCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_initCall(coreAgent, name, global_callCallback, wrap);
}

int igs_removeCall(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeCall(coreAgent, name);
}

int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    initInternalAgentIfNeeded();
    return igsAgent_addArgumentToCall(coreAgent, callName, argName, type);
}

int igs_removeArgumentFromCall(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_removeArgumentFromCall(coreAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfCalls(coreAgent);
}

bool igs_checkCallExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallExistence(coreAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getCallsList(coreAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getFirstArgumentForCall(coreAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfArgumentsForCall(coreAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallArgumentExistence(coreAgent, callName, argName);
}
