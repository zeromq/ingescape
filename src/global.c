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

igsAgent_t *internalAgent = NULL;
bool igs_Interrupted = false;

void globalforcedStopCB(void *myData){
    IGS_UNUSED(myData)
    igs_Interrupted = true;
}

void initInternalAgentIfNeeded(){
    if (internalAgent == NULL){
        internalAgent = igsAgent_new();
        igsAgent_observeForcedStop(internalAgent, globalforcedStopCB, NULL);
    }
}

int igs_startWithDevice(const char *networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithDevice(internalAgent, networkDevice, port);
}

int igs_startWithIP(const char *ipAddress, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithIP(internalAgent, ipAddress, port);
}

int igs_stop(void){
    initInternalAgentIfNeeded();
    return igsAgent_stop(internalAgent);
}

void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData){
    initInternalAgentIfNeeded();
    igsAgent_observeForcedStop(internalAgent, cb, myData);
}

void igs_die(void){
    initInternalAgentIfNeeded();
    igsAgent_die(internalAgent);
}

int igs_setAgentName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentName(internalAgent, name);
}

char *igs_getAgentName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentName(internalAgent);
}

int igs_setAgentState(const char *state){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentState(internalAgent, state);
}

char *igs_getAgentState(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentState(internalAgent);
}

int igs_mute(void){
    initInternalAgentIfNeeded();
    return igsAgent_mute(internalAgent);
}

int igs_unmute(void){
    initInternalAgentIfNeeded();
    return igsAgent_unmute(internalAgent);
}

bool igs_isMuted(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMuted(internalAgent);
}

int igs_observeMute(igs_muteCallback cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeMute(internalAgent, cb, myData);
}

int igs_freeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_freeze(internalAgent);
}

bool igs_isFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_isFrozen(internalAgent);
}

int igs_unfreeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_unfreeze(internalAgent);
}

int igs_observeFreeze(igs_freezeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeFreeze(internalAgent, cb, myData);
}

void igs_setCanBeFrozen(bool canBeFrozen){
    initInternalAgentIfNeeded();
    igsAgent_setCanBeFrozen(internalAgent, canBeFrozen);
}

bool igs_canBeFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_canBeFrozen(internalAgent);
}


//IOP
int igs_readInput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInput(internalAgent, name, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutput(internalAgent, name, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameter(internalAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsBool(internalAgent, name);
}

int igs_readInputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsInt(internalAgent, name);
}

double igs_readInputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsDouble(internalAgent, name);
}

char* igs_readInputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsString(internalAgent, name);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsData(internalAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsBool(internalAgent, name);
}

int igs_readOutputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsInt(internalAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsDouble(internalAgent, name);
}

char* igs_readOutputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsString(internalAgent, name);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsData(internalAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsBool(internalAgent, name);
}

int igs_readParameterAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsInt(internalAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsDouble(internalAgent, name);
}

char* igs_readParameterAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsString(internalAgent, name);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsData(internalAgent, name, data, size);
}

int igs_writeInputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsBool(internalAgent, name, value);
}

int igs_writeInputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsInt(internalAgent, name, value);
}

int igs_writeInputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsDouble(internalAgent, name, value);
}

int igs_writeInputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsString(internalAgent, name, value);
}

int igs_writeInputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsImpulsion(internalAgent, name);
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsData(internalAgent, name, value, size);
}

int igs_writeOutputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsBool(internalAgent, name, value);
}

int igs_writeOutputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsInt(internalAgent, name, value);
}

int igs_writeOutputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsDouble(internalAgent, name, value);
}

int igs_writeOutputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsString(internalAgent, name, value);
}

int igs_writeOutputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsImpulsion(internalAgent, name);
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsData(internalAgent, name, value, size);
}

int igs_writeParameterAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsBool(internalAgent, name, value);
}

int igs_writeParameterAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsInt(internalAgent, name, value);
}

int igs_writeParameterAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsDouble(internalAgent, name, value);
}

int igs_writeParameterAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsString(internalAgent, name, value);
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsData(internalAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForInput(internalAgent, name);
}

void igs_clearDataForOutput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForOutput(internalAgent, name);
}

void igs_clearDataForParameter(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForParameter(internalAgent, name);
}

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeInput(internalAgent, name, cb, myData);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeOutput(internalAgent, name, cb, myData);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeParameter(internalAgent, name, cb, myData);
}

int igs_muteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_muteOutput(internalAgent, name);
}

int igs_unmuteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_unmuteOutput(internalAgent, name);
}

bool igs_isOutputMuted(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_isOutputMuted(internalAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForInput(internalAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForOutput(internalAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForParameter(internalAgent, name);
}

int igs_getInputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsNumber(internalAgent);
}

int igs_getOutputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsNumber(internalAgent);
}

int igs_getParametersNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersNumber(internalAgent);
}

char** igs_getInputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsList(internalAgent, nbOfElements);
}

char** igs_getOutputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsList(internalAgent, nbOfElements);
}

char** igs_getParametersList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersList(internalAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkInputExistence(internalAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkOutputExistence(internalAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkParameterExistence(internalAgent, name);
}


//definition
int igs_loadDefinition (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinition(internalAgent, json_str);
}

int igs_loadDefinitionFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinitionFromPath(internalAgent, file_path);
}

int igs_clearDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearDefinition(internalAgent);
}

char* igs_getDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinition(internalAgent);
}

char *igs_getDefinitionName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionName(internalAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionDescription(internalAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionVersion(internalAgent);
}
 //returned char* must be freed by caller
int igs_setDefinitionName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionName(internalAgent, name);
}

int igs_setDefinitionDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionDescription(internalAgent, description);
}

int igs_setDefinitionVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionVersion(internalAgent, version);
}

int igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createInput(internalAgent, name, type, value, size);
}

int igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createOutput(internalAgent, name, type, value, size);
}

int igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createParameter(internalAgent, name, type, value, size);
}

int igs_removeInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeInput(internalAgent, name);
}

int igs_removeOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeOutput(internalAgent, name);
}

int igs_removeParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeParameter(internalAgent, name);
}


//mapping
int igs_loadMapping (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadMapping(internalAgent, json_str);
}

int igs_loadMappingFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadMappingFromPath(internalAgent, file_path);
}

int igs_clearMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearMapping(internalAgent);
}

char* igs_getMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(internalAgent);
}

char *igs_getMappingName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(internalAgent);
}

char *igs_getMappingDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingDescription(internalAgent);
}

char *igs_getMappingVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingVersion(internalAgent);
}

int igs_setMappingName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingName(internalAgent, name);
}

int igs_setMappingDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingDescription(internalAgent, description);
}

int igs_setMappingVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingVersion(internalAgent, version);
}

int igs_getMappingEntriesNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingEntriesNumber(internalAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_addMappingEntry(internalAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
int igs_removeMappingEntryWithId(unsigned long theId){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithId(internalAgent, theId);
}

int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithName(internalAgent, fromOurInput, toAgent, withOutput);
}


//admin
void igs_setCommandLine(const char *line){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLine(internalAgent, line);
}

void igs_setCommandLineFromArgs(int argc, const char * argv[]){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLineFromArgs(internalAgent, argc, argv);
}

void igs_setRequestOutputsFromMappedAgents(bool notify){
    initInternalAgentIfNeeded();
    igsAgent_setRequestOutputsFromMappedAgents(internalAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    initInternalAgentIfNeeded();
    return igsAgent_getRequestOutputsFromMappedAgents(internalAgent);
}


void igs_setDefinitionPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setDefinitionPath(internalAgent, path);
}

void igs_setMappingPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setMappingPath(internalAgent, path);
}

void igs_writeDefinitionToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeDefinitionToPath(internalAgent);
}

void igs_writeMappingToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeMappingToPath(internalAgent);
}

void igs_setAllowIpc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowIpc(internalAgent, allow);
}

bool igs_getAllowIpc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowIpc(internalAgent);
}

void igs_setAllowInproc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowInproc(internalAgent, allow);
}

bool igs_getAllowInproc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowInproc(internalAgent);
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igs_setIpcFolderPath(char *path){
    initInternalAgentIfNeeded();
    igsAgent_setIpcFolderPath(internalAgent, path);
}

const char* igs_getIpcFolderPath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getIpcFolderPath(internalAgent);
}

#endif

//licenses
#if !defined(TARGET_OS_IOS) || !TARGET_OS_IOS
void igs_setLicensePath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setLicensePath(internalAgent, path);
}

char *igs_getLicensePath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLicensePath(internalAgent);
}

bool igs_checkLicenseForAgent(const char *agentId){
    initInternalAgentIfNeeded();
    return igsAgent_checkLicense(internalAgent, agentId);
}

int igs_observeLicense(igs_licenseCallback cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeLicense(internalAgent, cb, myData);
}
#endif


//ADVANCED
void igs_setPublishingPort(unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_setPublishingPort(internalAgent, port);
}

void igs_setDiscoveryInterval(unsigned int interval){
    initInternalAgentIfNeeded();
    igsAgent_setDiscoveryInterval(internalAgent, interval);
}

void igs_setAgentTimeout(unsigned int duration){
    initInternalAgentIfNeeded();
    igsAgent_setAgentTimeout(internalAgent, duration);
}
 //in milliseconds
void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg){
    initInternalAgentIfNeeded();
    igsAgent_performanceCheck(internalAgent, peerId, msgSize, nbOfMsg);
}

void igs_setHighWaterMarks(int hwmValue){
    initInternalAgentIfNeeded();
    igsAgent_setHighWaterMarks(internalAgent, hwmValue);
}


void igs_monitoringEnable(unsigned int period){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnable(internalAgent, period);
}

void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnableWithExpectedDevice(internalAgent, period, networkDevice, port);
}

void igs_monitoringDisable(void){
    initInternalAgentIfNeeded();
    igsAgent_monitoringDisable(internalAgent);
}

bool igs_isMonitoringEnabled(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMonitoringEnabled(internalAgent);
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    initInternalAgentIfNeeded();
    igsAgent_monitor(internalAgent, cb, myData);
}

void igs_monitoringShallStartStopAgent(bool flag){
    initInternalAgentIfNeeded();
    igsAgent_monitoringShallStartStopAgent(internalAgent, flag);
}


int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsZMQMsg(internalAgent, name, msg);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsZMQMsg(internalAgent, name, msg);
}

int igs_observeBus(igs_BusMessageIncoming cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_observeBus(internalAgent, cb, myData);
}

void igs_busJoinChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busJoinChannel(internalAgent, channel);
}

void igs_busLeaveChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busLeaveChannel(internalAgent, channel);
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    int res = igsAgent_busSendStringToChannel(internalAgent, channel, msg, list);
    va_end(list);
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToChannel(internalAgent, channel, data, size);
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToChannel(internalAgent, channel, msg_p);
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    int res = igsAgent_busSendStringToAgent(internalAgent, agentNameOrPeerID, "%s", content);
    va_end(list);
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToAgent(internalAgent, agentNameOrPeerID, data, size);
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToAgent(internalAgent, agentNameOrPeerID, msg_p);
}

void igs_busAddServiceDescription(const char *key, const char *value){
    initInternalAgentIfNeeded();
    igsAgent_busAddServiceDescription(internalAgent, key, value);
}

void igs_busRemoveServiceDescription(const char *key){
    initInternalAgentIfNeeded();
    igsAgent_busRemoveServiceDescription(internalAgent, key);
}


int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    initInternalAgentIfNeeded();
    return igsAgent_sendCall(internalAgent, agentNameOrUUID, callName, list);
}

int igs_initCall(const char *name, igs_callFunction cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_initCall(internalAgent, name, cb, myData);
}

int igs_removeCall(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeCall(internalAgent, name);
}

int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    initInternalAgentIfNeeded();
    return igsAgent_addArgumentToCall(internalAgent, callName, argName, type);
}

int igs_removeArgumentFromCall(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_removeArgumentFromCall(internalAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfCalls(internalAgent);
}

bool igs_checkCallExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallExistence(internalAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getCallsList(internalAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getFirstArgumentForCall(internalAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfArgumentsForCall(internalAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallArgumentExistence(internalAgent, callName, argName);
}
