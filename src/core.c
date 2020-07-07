//
//  core.c
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

igs_core_context_t *coreContext = NULL;
igs_agent_t *coreAgent = NULL;


//////////////////  CORE CONTEXT //////////////////
void core_initContext(){
    if (coreContext == NULL){
        coreContext = calloc(1, sizeof(struct igs_core_context));

        //default values for context variables
        //NB: other values stay at zero / NULL until they are changed
        //by other functions.
        coreContext->network_allowIpc = true;
        coreContext->network_allowInproc = true;
        coreContext->network_hwmValue = 1000;
        coreContext->network_discoveryInterval = 1000;
        coreContext->network_agentTimeout = 30000;
        coreContext->logLevel = IGS_LOG_INFO;
        coreContext->network_shallRaiseFileDescriptorsLimit = true;
    }
}

//////////////////  CORE AGENT //////////////////
void core_forcedStopCB(igs_agent_t *agent, void *myData){
    IGS_UNUSED(myData)
    IGS_UNUSED(agent)
    igs_Interrupted = true;
}

void core_initCoreAgent(){
    core_initContext();
    if (coreAgent == NULL){
        coreAgent = igsAgent_new();
        igsAgent_observeForcedStop(coreAgent, core_forcedStopCB, NULL);
    }
}

typedef struct {
    igs_forcedStopCallback cb;
    void *myData;
} observeForcedStopCbWrapper_t;

void core_observeForcedStopCallback(igs_agent_t *agent, void *myData){
    IGS_UNUSED(agent)
    observeForcedStopCbWrapper_t *wrap = (observeForcedStopCbWrapper_t *)myData;
    wrap->cb(wrap->myData);
}

void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData){
    core_initCoreAgent();
    observeForcedStopCbWrapper_t *wrap = calloc(1, sizeof(observeForcedStopCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeForcedStop(coreAgent, core_observeForcedStopCallback, wrap);
}

int igs_setAgentName(const char *name){
    core_initCoreAgent();
    return igsAgent_setAgentName(coreAgent, name);
}

char *igs_getAgentName(void){
    core_initCoreAgent();
    return igsAgent_getAgentName(coreAgent);
}

int igs_setAgentState(const char *state){
    core_initCoreAgent();
    return igsAgent_setAgentState(coreAgent, state);
}

char *igs_getAgentState(void){
    core_initCoreAgent();
    return igsAgent_getAgentState(coreAgent);
}

int igs_mute(void){
    core_initCoreAgent();
    return igsAgent_mute(coreAgent);
}

int igs_unmute(void){
    core_initCoreAgent();
    return igsAgent_unmute(coreAgent);
}

bool igs_isMuted(void){
    core_initCoreAgent();
    return igsAgent_isMuted(coreAgent);
}

typedef struct {
    igs_muteCallback cb;
    void *myData;
} observeMuteCbWrapper_t;

void core_observeMuteCallback(igs_agent_t *agent, bool isMuted, void *myData){
    IGS_UNUSED(agent)
    observeMuteCbWrapper_t *wrap = (observeMuteCbWrapper_t *)myData;
    wrap->cb(isMuted, wrap->myData);
}

int igs_observeMute(igs_muteCallback cb, void *myData){
    core_initCoreAgent();
    observeMuteCbWrapper_t *wrap = calloc(1, sizeof(observeMuteCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeMute(coreAgent, core_observeMuteCallback, wrap);
}

int igs_freeze(void){
    core_initCoreAgent();
    return igsAgent_freeze(coreAgent);
}

bool igs_isFrozen(void){
    core_initCoreAgent();
    return igsAgent_isFrozen(coreAgent);
}

int igs_unfreeze(void){
    core_initCoreAgent();
    return igsAgent_unfreeze(coreAgent);
}

typedef struct {
    igs_freezeCallback cb;
    void *myData;
} observeFreezeCbWrapper_t;

void core_observeFreezeCallback(igs_agent_t *agent, bool isPaused, void *myData){
    IGS_UNUSED(agent)
    observeFreezeCbWrapper_t *wrap = (observeFreezeCbWrapper_t *)myData;
    wrap->cb(isPaused, wrap->myData);
}

int igs_observeFreeze(igs_freezeCallback cb, void *myData){
    core_initCoreAgent();
    observeFreezeCbWrapper_t *wrap = calloc(1, sizeof(observeFreezeCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeFreeze(coreAgent, cb, wrap);
}

void igs_setCanBeFrozen(bool canBeFrozen){
    core_initCoreAgent();
    igsAgent_setCanBeFrozen(coreAgent, canBeFrozen);
}

bool igs_canBeFrozen(void){
    core_initCoreAgent();
    return igsAgent_canBeFrozen(coreAgent);
}


//IOP
int igs_readInput(const char *name, void **value, size_t *size){
    core_initCoreAgent();
    return igsAgent_readInput(coreAgent, name, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    core_initCoreAgent();
    return igsAgent_readOutput(coreAgent, name, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    core_initCoreAgent();
    return igsAgent_readParameter(coreAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    core_initCoreAgent();
    return igsAgent_readInputAsBool(coreAgent, name);
}

int igs_readInputAsInt(const char *name){
    core_initCoreAgent();
    return igsAgent_readInputAsInt(coreAgent, name);
}

double igs_readInputAsDouble(const char *name){
    core_initCoreAgent();
    return igsAgent_readInputAsDouble(coreAgent, name);
}

char* igs_readInputAsString(const char *name){
    core_initCoreAgent();
    return igsAgent_readInputAsString(coreAgent, name);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    core_initCoreAgent();
    return igsAgent_readInputAsData(coreAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    core_initCoreAgent();
    return igsAgent_readOutputAsBool(coreAgent, name);
}

int igs_readOutputAsInt(const char *name){
    core_initCoreAgent();
    return igsAgent_readOutputAsInt(coreAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    core_initCoreAgent();
    return igsAgent_readOutputAsDouble(coreAgent, name);
}

char* igs_readOutputAsString(const char *name){
    core_initCoreAgent();
    return igsAgent_readOutputAsString(coreAgent, name);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    core_initCoreAgent();
    return igsAgent_readOutputAsData(coreAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    core_initCoreAgent();
    return igsAgent_readParameterAsBool(coreAgent, name);
}

int igs_readParameterAsInt(const char *name){
    core_initCoreAgent();
    return igsAgent_readParameterAsInt(coreAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    core_initCoreAgent();
    return igsAgent_readParameterAsDouble(coreAgent, name);
}

char* igs_readParameterAsString(const char *name){
    core_initCoreAgent();
    return igsAgent_readParameterAsString(coreAgent, name);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    core_initCoreAgent();
    return igsAgent_readParameterAsData(coreAgent, name, data, size);
}

int igs_writeInputAsBool(const char *name, bool value){
    core_initCoreAgent();
    return igsAgent_writeInputAsBool(coreAgent, name, value);
}

int igs_writeInputAsInt(const char *name, int value){
    core_initCoreAgent();
    return igsAgent_writeInputAsInt(coreAgent, name, value);
}

int igs_writeInputAsDouble(const char *name, double value){
    core_initCoreAgent();
    return igsAgent_writeInputAsDouble(coreAgent, name, value);
}

int igs_writeInputAsString(const char *name, const char *value){
    core_initCoreAgent();
    return igsAgent_writeInputAsString(coreAgent, name, value);
}

int igs_writeInputAsImpulsion(const char *name){
    core_initCoreAgent();
    return igsAgent_writeInputAsImpulsion(coreAgent, name);
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_writeInputAsData(coreAgent, name, value, size);
}

int igs_writeOutputAsBool(const char *name, bool value){
    core_initCoreAgent();
    return igsAgent_writeOutputAsBool(coreAgent, name, value);
}

int igs_writeOutputAsInt(const char *name, int value){
    core_initCoreAgent();
    return igsAgent_writeOutputAsInt(coreAgent, name, value);
}

int igs_writeOutputAsDouble(const char *name, double value){
    core_initCoreAgent();
    return igsAgent_writeOutputAsDouble(coreAgent, name, value);
}

int igs_writeOutputAsString(const char *name, const char *value){
    core_initCoreAgent();
    return igsAgent_writeOutputAsString(coreAgent, name, value);
}

int igs_writeOutputAsImpulsion(const char *name){
    core_initCoreAgent();
    return igsAgent_writeOutputAsImpulsion(coreAgent, name);
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_writeOutputAsData(coreAgent, name, value, size);
}

int igs_writeParameterAsBool(const char *name, bool value){
    core_initCoreAgent();
    return igsAgent_writeParameterAsBool(coreAgent, name, value);
}

int igs_writeParameterAsInt(const char *name, int value){
    core_initCoreAgent();
    return igsAgent_writeParameterAsInt(coreAgent, name, value);
}

int igs_writeParameterAsDouble(const char *name, double value){
    core_initCoreAgent();
    return igsAgent_writeParameterAsDouble(coreAgent, name, value);
}

int igs_writeParameterAsString(const char *name, const char *value){
    core_initCoreAgent();
    return igsAgent_writeParameterAsString(coreAgent, name, value);
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_writeParameterAsData(coreAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    core_initCoreAgent();
    igsAgent_clearDataForInput(coreAgent, name);
}

void igs_clearDataForOutput(const char *name){
    core_initCoreAgent();
    igsAgent_clearDataForOutput(coreAgent, name);
}

void igs_clearDataForParameter(const char *name){
    core_initCoreAgent();
    igsAgent_clearDataForParameter(coreAgent, name);
}

typedef struct {
    igs_observeCallback cb;
    void *myData;
} observeIOPCbWrapper_t;

void core_observeIOPCallback(igs_agent_t *agent, iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData){
    IGS_UNUSED(agent)
    observeIOPCbWrapper_t *wrap = (observeIOPCbWrapper_t *)myData;
    wrap->cb(iopType, name, valueType, value, valueSize, wrap->myData);
}

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    core_initCoreAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeInput(coreAgent, name, core_observeIOPCallback, wrap);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    core_initCoreAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeOutput(coreAgent, name, core_observeIOPCallback, wrap);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    core_initCoreAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeParameter(coreAgent, name, core_observeIOPCallback, wrap);
}

int igs_muteOutput(const char *name){
    core_initCoreAgent();
    return igsAgent_muteOutput(coreAgent, name);
}

int igs_unmuteOutput(const char *name){
    core_initCoreAgent();
    return igsAgent_unmuteOutput(coreAgent, name);
}

bool igs_isOutputMuted(const char *name){
    core_initCoreAgent();
    return igsAgent_isOutputMuted(coreAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    core_initCoreAgent();
    return igsAgent_getTypeForInput(coreAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    core_initCoreAgent();
    return igsAgent_getTypeForOutput(coreAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    core_initCoreAgent();
    return igsAgent_getTypeForParameter(coreAgent, name);
}

int igs_getInputsNumber(void){
    core_initCoreAgent();
    return igsAgent_getInputsNumber(coreAgent);
}

int igs_getOutputsNumber(void){
    core_initCoreAgent();
    return igsAgent_getOutputsNumber(coreAgent);
}

int igs_getParametersNumber(void){
    core_initCoreAgent();
    return igsAgent_getParametersNumber(coreAgent);
}

char** igs_getInputsList(long *nbOfElements){
    core_initCoreAgent();
    return igsAgent_getInputsList(coreAgent, nbOfElements);
}

char** igs_getOutputsList(long *nbOfElements){
    core_initCoreAgent();
    return igsAgent_getOutputsList(coreAgent, nbOfElements);
}

char** igs_getParametersList(long *nbOfElements){
    core_initCoreAgent();
    return igsAgent_getParametersList(coreAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    core_initCoreAgent();
    return igsAgent_checkInputExistence(coreAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    core_initCoreAgent();
    return igsAgent_checkOutputExistence(coreAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    core_initCoreAgent();
    return igsAgent_checkParameterExistence(coreAgent, name);
}


//definition
int igs_loadDefinition (const char* json_str){
    core_initCoreAgent();
    return igsAgent_loadDefinition(coreAgent, json_str);
}

int igs_loadDefinitionFromPath (const char* file_path){
    core_initCoreAgent();
    return igsAgent_loadDefinitionFromPath(coreAgent, file_path);
}

int igs_clearDefinition(void){
    core_initCoreAgent();
    return igsAgent_clearDefinition(coreAgent);
}

char* igs_getDefinition(void){
    core_initCoreAgent();
    return igsAgent_getDefinition(coreAgent);
}

char *igs_getDefinitionName(void){
    core_initCoreAgent();
    return igsAgent_getDefinitionName(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    core_initCoreAgent();
    return igsAgent_getDefinitionDescription(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    core_initCoreAgent();
    return igsAgent_getDefinitionVersion(coreAgent);
}
 //returned char* must be freed by caller
int igs_setDefinitionName(const char *name){
    core_initCoreAgent();
    return igsAgent_setDefinitionName(coreAgent, name);
}

int igs_setDefinitionDescription(const char *description){
    core_initCoreAgent();
    return igsAgent_setDefinitionDescription(coreAgent, description);
}

int igs_setDefinitionVersion(const char *version){
    core_initCoreAgent();
    return igsAgent_setDefinitionVersion(coreAgent, version);
}

int igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_createInput(coreAgent, name, type, value, size);
}

int igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_createOutput(coreAgent, name, type, value, size);
}

int igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    core_initCoreAgent();
    return igsAgent_createParameter(coreAgent, name, type, value, size);
}

int igs_removeInput(const char *name){
    core_initCoreAgent();
    return igsAgent_removeInput(coreAgent, name);
}

int igs_removeOutput(const char *name){
    core_initCoreAgent();
    return igsAgent_removeOutput(coreAgent, name);
}

int igs_removeParameter(const char *name){
    core_initCoreAgent();
    return igsAgent_removeParameter(coreAgent, name);
}


//mapping
int igs_loadMapping (const char* json_str){
    core_initCoreAgent();
    return igsAgent_loadMapping(coreAgent, json_str);
}

int igs_loadMappingFromPath (const char* file_path){
    core_initCoreAgent();
    return igsAgent_loadMappingFromPath(coreAgent, file_path);
}

int igs_clearMapping(void){
    core_initCoreAgent();
    return igsAgent_clearMapping(coreAgent);
}

char* igs_getMapping(void){
    core_initCoreAgent();
    return igsAgent_getMapping(coreAgent);
}

char *igs_getMappingName(void){
    core_initCoreAgent();
    return igsAgent_getMapping(coreAgent);
}

char *igs_getMappingDescription(void){
    core_initCoreAgent();
    return igsAgent_getMappingDescription(coreAgent);
}

char *igs_getMappingVersion(void){
    core_initCoreAgent();
    return igsAgent_getMappingVersion(coreAgent);
}

int igs_setMappingName(const char *name){
    core_initCoreAgent();
    return igsAgent_setMappingName(coreAgent, name);
}

int igs_setMappingDescription(const char *description){
    core_initCoreAgent();
    return igsAgent_setMappingDescription(coreAgent, description);
}

int igs_setMappingVersion(const char *version){
    core_initCoreAgent();
    return igsAgent_setMappingVersion(coreAgent, version);
}

int igs_getMappingEntriesNumber(void){
    core_initCoreAgent();
    return igsAgent_getMappingEntriesNumber(coreAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    core_initCoreAgent();
    return igsAgent_addMappingEntry(coreAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
int igs_removeMappingEntryWithId(unsigned long theId){
    core_initCoreAgent();
    return igsAgent_removeMappingEntryWithId(coreAgent, theId);
}

int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    core_initCoreAgent();
    return igsAgent_removeMappingEntryWithName(coreAgent, fromOurInput, toAgent, withOutput);
}


//admin

void igs_setRequestOutputsFromMappedAgents(bool notify){
    core_initCoreAgent();
    igsAgent_setRequestOutputsFromMappedAgents(coreAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    core_initCoreAgent();
    return igsAgent_getRequestOutputsFromMappedAgents(coreAgent);
}


void igs_setDefinitionPath(const char *path){
    core_initCoreAgent();
    igsAgent_setDefinitionPath(coreAgent, path);
}

void igs_setMappingPath(const char *path){
    core_initCoreAgent();
    igsAgent_setMappingPath(coreAgent, path);
}

void igs_writeDefinitionToPath(void){
    core_initCoreAgent();
    igsAgent_writeDefinitionToPath(coreAgent);
}

void igs_writeMappingToPath(void){
    core_initCoreAgent();
    igsAgent_writeMappingToPath(coreAgent);
}

void igs_log(igs_logLevel_t level, const char *function, const char *format, ...){
    core_initCoreAgent();
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(coreAgent, level, function, "%s", content);
}

//ADVANCED
int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    core_initCoreAgent();
    return igsAgent_writeOutputAsZMQMsg(coreAgent, name, msg);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    core_initCoreAgent();
    return igsAgent_readInputAsZMQMsg(coreAgent, name, msg);
}

typedef struct {
    igs_BusMessageIncoming cb;
    void *myData;
} observeBusCbWrapper_t;

void core_observeBusCallback(igs_agent_t *agent, const char *event, const char *peerID, const char *name,
                               const char *address, const char *channel,
                               zhash_t *headers, zmsg_t *msg, void *myData){
    IGS_UNUSED(agent)
    observeBusCbWrapper_t *wrap = (observeBusCbWrapper_t *)myData;
    wrap->cb(event, peerID, name, address, channel, headers, msg, wrap->myData);
}

int igs_observeBus(igs_BusMessageIncoming cb, void *myData){
    core_initCoreAgent();
    observeBusCbWrapper_t *wrap = calloc(1, sizeof(observeBusCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeBus(coreAgent, core_observeBusCallback, wrap);
}

void igs_busJoinChannel(const char *channel){
    core_initCoreAgent();
    igsAgent_busJoinChannel(coreAgent, channel);
}

void igs_busLeaveChannel(const char *channel){
    core_initCoreAgent();
    igsAgent_busLeaveChannel(coreAgent, channel);
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    core_initCoreAgent();
    va_list list;
    va_start(list, msg);
    int res = igsAgent_busSendStringToChannel(coreAgent, channel, msg, list);
    va_end(list);
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    core_initCoreAgent();
    return igsAgent_busSendDataToChannel(coreAgent, channel, data, size);
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    core_initCoreAgent();
    return igsAgent_busSendZMQMsgToChannel(coreAgent, channel, msg_p);
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    core_initCoreAgent();
    va_list list;
    va_start(list, msg);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    int res = igsAgent_busSendStringToAgent(coreAgent, agentNameOrPeerID, "%s", content);
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    core_initCoreAgent();
    return igsAgent_busSendDataToAgent(coreAgent, agentNameOrPeerID, data, size);
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    core_initCoreAgent();
    return igsAgent_busSendZMQMsgToAgent(coreAgent, agentNameOrPeerID, msg_p);
}

void igs_busAddServiceDescription(const char *key, const char *value){
    core_initCoreAgent();
    igsAgent_busAddServiceDescription(coreAgent, key, value);
}

void igs_busRemoveServiceDescription(const char *key){
    core_initCoreAgent();
    igsAgent_busRemoveServiceDescription(coreAgent, key);
}


int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    core_initCoreAgent();
    return igsAgent_sendCall(coreAgent, agentNameOrUUID, callName, list);
}

typedef struct {
    igs_callFunction cb;
    void *myData;
} callCbWrapper_t;

void core_callCallback(igs_agent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                         const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                         void* myData){
    IGS_UNUSED(agent)
    callCbWrapper_t *wrap = (callCbWrapper_t *)myData;
    wrap->cb(senderAgentName, senderAgentUUID, callName, firstArgument, nbArgs, wrap->myData);
}

int igs_initCall(const char *name, igs_callFunction cb, void *myData){
    core_initCoreAgent();
    callCbWrapper_t *wrap = calloc(1, sizeof(callCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_initCall(coreAgent, name, core_callCallback, wrap);
}

int igs_removeCall(const char *name){
    core_initCoreAgent();
    return igsAgent_removeCall(coreAgent, name);
}

int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    core_initCoreAgent();
    return igsAgent_addArgumentToCall(coreAgent, callName, argName, type);
}

int igs_removeArgumentFromCall(const char *callName, const char *argName){
    core_initCoreAgent();
    return igsAgent_removeArgumentFromCall(coreAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    core_initCoreAgent();
    return igsAgent_getNumberOfCalls(coreAgent);
}

bool igs_checkCallExistence(const char *name){
    core_initCoreAgent();
    return igsAgent_checkCallExistence(coreAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    core_initCoreAgent();
    return igsAgent_getCallsList(coreAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    core_initCoreAgent();
    return igsAgent_getFirstArgumentForCall(coreAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    core_initCoreAgent();
    return igsAgent_getNumberOfArgumentsForCall(coreAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    core_initCoreAgent();
    return igsAgent_checkCallArgumentExistence(coreAgent, callName, argName);
}
