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
bool igs_Interrupted = false;


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
        coreContext->network_ipcFolderPath = strdup(IGS_DEFAULT_IPC_FOLDER_PATH);
    }
}

//////////////////  CORE AGENT //////////////////
void core_forcedStopCB(void *myData){
    IGS_UNUSED(myData)
    igs_Interrupted = true;
}

void core_initAgent(){
    core_initContext();
    if (coreAgent == NULL){
        coreAgent = igsAgent_new(IGS_DEFAULT_AGENT_NAME, false);
        igs_observeForcedStop(core_forcedStopCB, NULL);
        coreAgent->context = coreContext;
        HASH_ADD_STR(coreContext->agents, uuid, coreAgent);
    }
}

void igs_setAgentName(const char *name){
    core_initAgent();
    igsAgent_setAgentName(coreAgent, name);
}

char *igs_getAgentName(void){
    core_initAgent();
    return igsAgent_getAgentName(coreAgent);
}

void igs_setAgentState(const char *state){
    core_initAgent();
    igsAgent_setAgentState(coreAgent, state);
}

char *igs_getAgentState(void){
    core_initAgent();
    return igsAgent_getAgentState(coreAgent);
}

void igs_mute(void){
    core_initAgent();
    igsAgent_mute(coreAgent);
}

void igs_unmute(void){
    core_initAgent();
    igsAgent_unmute(coreAgent);
}

bool igs_isMuted(void){
    core_initAgent();
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

void igs_observeMute(igs_muteCallback cb, void *myData){
    core_initAgent();
    observeMuteCbWrapper_t *wrap = calloc(1, sizeof(observeMuteCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeMute(coreAgent, core_observeMuteCallback, wrap);
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

//IOP
igs_result_t igs_readInput(const char *name, void **value, size_t *size){
    core_initAgent();
    return igsAgent_readInput(coreAgent, name, value, size);
}

igs_result_t igs_readOutput(const char *name, void **value, size_t *size){
    core_initAgent();
    return igsAgent_readOutput(coreAgent, name, value, size);
}

igs_result_t igs_readParameter(const char *name, void **value, size_t *size){
    core_initAgent();
    return igsAgent_readParameter(coreAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    core_initAgent();
    return igsAgent_readInputAsBool(coreAgent, name);
}

int igs_readInputAsInt(const char *name){
    core_initAgent();
    return igsAgent_readInputAsInt(coreAgent, name);
}

double igs_readInputAsDouble(const char *name){
    core_initAgent();
    return igsAgent_readInputAsDouble(coreAgent, name);
}

char* igs_readInputAsString(const char *name){
    core_initAgent();
    return igsAgent_readInputAsString(coreAgent, name);
}

igs_result_t igs_readInputAsData(const char *name, void **data, size_t *size){
    core_initAgent();
    return igsAgent_readInputAsData(coreAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    core_initAgent();
    return igsAgent_readOutputAsBool(coreAgent, name);
}

int igs_readOutputAsInt(const char *name){
    core_initAgent();
    return igsAgent_readOutputAsInt(coreAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    core_initAgent();
    return igsAgent_readOutputAsDouble(coreAgent, name);
}

char* igs_readOutputAsString(const char *name){
    core_initAgent();
    return igsAgent_readOutputAsString(coreAgent, name);
}

igs_result_t igs_readOutputAsData(const char *name, void **data, size_t *size){
    core_initAgent();
    return igsAgent_readOutputAsData(coreAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    core_initAgent();
    return igsAgent_readParameterAsBool(coreAgent, name);
}

int igs_readParameterAsInt(const char *name){
    core_initAgent();
    return igsAgent_readParameterAsInt(coreAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    core_initAgent();
    return igsAgent_readParameterAsDouble(coreAgent, name);
}

char* igs_readParameterAsString(const char *name){
    core_initAgent();
    return igsAgent_readParameterAsString(coreAgent, name);
}

igs_result_t igs_readParameterAsData(const char *name, void **data, size_t *size){
    core_initAgent();
    return igsAgent_readParameterAsData(coreAgent, name, data, size);
}

igs_result_t igs_writeInputAsBool(const char *name, bool value){
    core_initAgent();
    return igsAgent_writeInputAsBool(coreAgent, name, value);
}

igs_result_t igs_writeInputAsInt(const char *name, int value){
    core_initAgent();
    return igsAgent_writeInputAsInt(coreAgent, name, value);
}

igs_result_t igs_writeInputAsDouble(const char *name, double value){
    core_initAgent();
    return igsAgent_writeInputAsDouble(coreAgent, name, value);
}

igs_result_t igs_writeInputAsString(const char *name, const char *value){
    core_initAgent();
    return igsAgent_writeInputAsString(coreAgent, name, value);
}

igs_result_t igs_writeInputAsImpulsion(const char *name){
    core_initAgent();
    return igsAgent_writeInputAsImpulsion(coreAgent, name);
}

igs_result_t igs_writeInputAsData(const char *name, void *value, size_t size){
    core_initAgent();
    return igsAgent_writeInputAsData(coreAgent, name, value, size);
}

igs_result_t igs_writeOutputAsBool(const char *name, bool value){
    core_initAgent();
    return igsAgent_writeOutputAsBool(coreAgent, name, value);
}

igs_result_t igs_writeOutputAsInt(const char *name, int value){
    core_initAgent();
    return igsAgent_writeOutputAsInt(coreAgent, name, value);
}

igs_result_t igs_writeOutputAsDouble(const char *name, double value){
    core_initAgent();
    return igsAgent_writeOutputAsDouble(coreAgent, name, value);
}

igs_result_t igs_writeOutputAsString(const char *name, const char *value){
    core_initAgent();
    return igsAgent_writeOutputAsString(coreAgent, name, value);
}

igs_result_t igs_writeOutputAsImpulsion(const char *name){
    core_initAgent();
    return igsAgent_writeOutputAsImpulsion(coreAgent, name);
}

igs_result_t igs_writeOutputAsData(const char *name, void *value, size_t size){
    core_initAgent();
    return igsAgent_writeOutputAsData(coreAgent, name, value, size);
}

igs_result_t igs_writeParameterAsBool(const char *name, bool value){
    core_initAgent();
    return igsAgent_writeParameterAsBool(coreAgent, name, value);
}

igs_result_t igs_writeParameterAsInt(const char *name, int value){
    core_initAgent();
    return igsAgent_writeParameterAsInt(coreAgent, name, value);
}

igs_result_t igs_writeParameterAsDouble(const char *name, double value){
    core_initAgent();
    return igsAgent_writeParameterAsDouble(coreAgent, name, value);
}

igs_result_t igs_writeParameterAsString(const char *name, const char *value){
    core_initAgent();
    return igsAgent_writeParameterAsString(coreAgent, name, value);
}

igs_result_t igs_writeParameterAsData(const char *name, void *value, size_t size){
    core_initAgent();
    return igsAgent_writeParameterAsData(coreAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    core_initAgent();
    igsAgent_clearDataForInput(coreAgent, name);
}

void igs_clearDataForOutput(const char *name){
    core_initAgent();
    igsAgent_clearDataForOutput(coreAgent, name);
}

void igs_clearDataForParameter(const char *name){
    core_initAgent();
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

void igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    core_initAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeInput(coreAgent, name, core_observeIOPCallback, wrap);
}

void igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    core_initAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeOutput(coreAgent, name, core_observeIOPCallback, wrap);
}

void igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    core_initAgent();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeParameter(coreAgent, name, core_observeIOPCallback, wrap);
}

void igs_muteOutput(const char *name){
    core_initAgent();
    igsAgent_muteOutput(coreAgent, name);
}

void igs_unmuteOutput(const char *name){
    core_initAgent();
    igsAgent_unmuteOutput(coreAgent, name);
}

bool igs_isOutputMuted(const char *name){
    core_initAgent();
    return igsAgent_isOutputMuted(coreAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    core_initAgent();
    return igsAgent_getTypeForInput(coreAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    core_initAgent();
    return igsAgent_getTypeForOutput(coreAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    core_initAgent();
    return igsAgent_getTypeForParameter(coreAgent, name);
}

size_t igs_getInputsNumber(void){
    core_initAgent();
    return igsAgent_getInputsNumber(coreAgent);
}

size_t igs_getOutputsNumber(void){
    core_initAgent();
    return igsAgent_getOutputsNumber(coreAgent);
}

size_t igs_getParametersNumber(void){
    core_initAgent();
    return igsAgent_getParametersNumber(coreAgent);
}

char** igs_getInputsList(size_t *nbOfElements){
    core_initAgent();
    return igsAgent_getInputsList(coreAgent, nbOfElements);
}

char** igs_getOutputsList(size_t *nbOfElements){
    core_initAgent();
    return igsAgent_getOutputsList(coreAgent, nbOfElements);
}

char** igs_getParametersList(size_t *nbOfElements){
    core_initAgent();
    return igsAgent_getParametersList(coreAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    core_initAgent();
    return igsAgent_checkInputExistence(coreAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    core_initAgent();
    return igsAgent_checkOutputExistence(coreAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    core_initAgent();
    return igsAgent_checkParameterExistence(coreAgent, name);
}


//definition
igs_result_t igs_loadDefinition (const char* json_str){
    core_initAgent();
    return igsAgent_loadDefinition(coreAgent, json_str);
}

igs_result_t igs_loadDefinitionFromPath (const char* file_path){
    core_initAgent();
    return igsAgent_loadDefinitionFromPath(coreAgent, file_path);
}

void igs_clearDefinition(void){
    core_initAgent();
    igsAgent_clearDefinition(coreAgent);
}

char* igs_getDefinition(void){
    core_initAgent();
    return igsAgent_getDefinition(coreAgent);
}

char *igs_getDefinitionName(void){
    core_initAgent();
    return igsAgent_getDefinitionName(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    core_initAgent();
    return igsAgent_getDefinitionDescription(coreAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    core_initAgent();
    return igsAgent_getDefinitionVersion(coreAgent);
}
 //returned char* must be freed by caller
void igs_setDefinitionName(const char *name){
    core_initAgent();
    igsAgent_setDefinitionName(coreAgent, name);
}

void igs_setDefinitionDescription(const char *description){
    core_initAgent();
    igsAgent_setDefinitionDescription(coreAgent, description);
}

void igs_setDefinitionVersion(const char *version){
    core_initAgent();
    igsAgent_setDefinitionVersion(coreAgent, version);
}

igs_result_t igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    core_initAgent();
    return igsAgent_createInput(coreAgent, name, type, value, size);
}

igs_result_t igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    core_initAgent();
    return igsAgent_createOutput(coreAgent, name, type, value, size);
}

igs_result_t igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    core_initAgent();
    return igsAgent_createParameter(coreAgent, name, type, value, size);
}

igs_result_t igs_removeInput(const char *name){
    core_initAgent();
    return igsAgent_removeInput(coreAgent, name);
}

igs_result_t igs_removeOutput(const char *name){
    core_initAgent();
    return igsAgent_removeOutput(coreAgent, name);
}

igs_result_t igs_removeParameter(const char *name){
    core_initAgent();
    return igsAgent_removeParameter(coreAgent, name);
}


//mapping
igs_result_t igs_loadMapping (const char* json_str){
    core_initAgent();
    return igsAgent_loadMapping(coreAgent, json_str);
}

igs_result_t igs_loadMappingFromPath (const char* file_path){
    core_initAgent();
    return igsAgent_loadMappingFromPath(coreAgent, file_path);
}

void igs_clearMapping(void){
    core_initAgent();
    igsAgent_clearMapping(coreAgent);
}

char* igs_getMapping(void){
    core_initAgent();
    return igsAgent_getMapping(coreAgent);
}

char *igs_getMappingName(void){
    core_initAgent();
    return igsAgent_getMappingName(coreAgent);
}

char *igs_getMappingDescription(void){
    core_initAgent();
    return igsAgent_getMappingDescription(coreAgent);
}

char *igs_getMappingVersion(void){
    core_initAgent();
    return igsAgent_getMappingVersion(coreAgent);
}

void igs_setMappingName(const char *name){
    core_initAgent();
    igsAgent_setMappingName(coreAgent, name);
}

void igs_setMappingDescription(const char *description){
    core_initAgent();
    igsAgent_setMappingDescription(coreAgent, description);
}

void igs_setMappingVersion(const char *version){
    core_initAgent();
    igsAgent_setMappingVersion(coreAgent, version);
}

size_t igs_getMappingEntriesNumber(void){
    core_initAgent();
    return igsAgent_getMappingEntriesNumber(coreAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    core_initAgent();
    return igsAgent_addMappingEntry(coreAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
igs_result_t igs_removeMappingEntryWithId(unsigned long theId){
    core_initAgent();
    return igsAgent_removeMappingEntryWithId(coreAgent, theId);
}

igs_result_t igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    core_initAgent();
    return igsAgent_removeMappingEntryWithName(coreAgent, fromOurInput, toAgent, withOutput);
}


//admin

void igs_setRequestOutputsFromMappedAgents(bool notify){
    core_initAgent();
    igsAgent_setRequestOutputsFromMappedAgents(coreAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    core_initAgent();
    return igsAgent_getRequestOutputsFromMappedAgents(coreAgent);
}


void igs_setDefinitionPath(const char *path){
    core_initAgent();
    igsAgent_setDefinitionPath(coreAgent, path);
}

void igs_setMappingPath(const char *path){
    core_initAgent();
    igsAgent_setMappingPath(coreAgent, path);
}

void igs_writeDefinitionToPath(void){
    core_initAgent();
    igsAgent_writeDefinitionToPath(coreAgent);
}

void igs_writeMappingToPath(void){
    core_initAgent();
    igsAgent_writeMappingToPath(coreAgent);
}

void igs_log(igs_logLevel_t level, const char *function, const char *format, ...){
    core_initAgent();
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(coreAgent, level, function, "%s", content);
}

//ADVANCED
igs_result_t igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    core_initAgent();
    return igsAgent_writeOutputAsZMQMsg(coreAgent, name, msg);
}

igs_result_t igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    core_initAgent();
    return igsAgent_readInputAsZMQMsg(coreAgent, name, msg);
}


igs_result_t igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    core_initAgent();
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

igs_result_t igs_initCall(const char *name, igs_callFunction cb, void *myData){
    core_initAgent();
    callCbWrapper_t *wrap = calloc(1, sizeof(callCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_initCall(coreAgent, name, core_callCallback, wrap);
}

igs_result_t igs_removeCall(const char *name){
    core_initAgent();
    return igsAgent_removeCall(coreAgent, name);
}

igs_result_t igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    core_initAgent();
    return igsAgent_addArgumentToCall(coreAgent, callName, argName, type);
}

igs_result_t igs_removeArgumentFromCall(const char *callName, const char *argName){
    core_initAgent();
    return igsAgent_removeArgumentFromCall(coreAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    core_initAgent();
    return igsAgent_getNumberOfCalls(coreAgent);
}

bool igs_checkCallExistence(const char *name){
    core_initAgent();
    return igsAgent_checkCallExistence(coreAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    core_initAgent();
    return igsAgent_getCallsList(coreAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    core_initAgent();
    return igsAgent_getFirstArgumentForCall(coreAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    core_initAgent();
    return igsAgent_getNumberOfArgumentsForCall(coreAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    core_initAgent();
    return igsAgent_checkCallArgumentExistence(coreAgent, callName, argName);
}
