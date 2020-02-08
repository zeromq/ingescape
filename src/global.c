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

igsAgent_t *globalAgent = NULL;
int igs_nbOfAgentsInProcess = 0;
bool igs_Interrupted = false;
bool igs_shallRaiseFileDescriptorsLimit = true;

void globalforcedStopCB(igsAgent_t *agent, void *myData){
    IGS_UNUSED(myData)
    IGS_UNUSED(agent)
    igs_Interrupted = true;
}

void initInternalAgentIfNeeded(){
    if (globalAgent == NULL){
        globalAgent = igsAgent_new();
        igsAgent_observeForcedStop(globalAgent, globalforcedStopCB, NULL);
    }
}

int igs_startWithDevice(const char *networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithDevice(globalAgent, networkDevice, port);
}

int igs_startWithIP(const char *ipAddress, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithIP(globalAgent, ipAddress, port);
}

int igs_stop(void){
    initInternalAgentIfNeeded();
    return igsAgent_stop(globalAgent);
}


typedef struct {
    igs_forcedStopCallback cb;
    void *myData;
} observeForcedStopCbWrapper_t;

void global_observeForcedStopCallback(igsAgent_t *agent, void *myData){
    IGS_UNUSED(agent)
    observeForcedStopCbWrapper_t *wrap = (observeForcedStopCbWrapper_t *)myData;
    wrap->cb(wrap->myData);
}

void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeForcedStopCbWrapper_t *wrap = calloc(1, sizeof(observeForcedStopCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeForcedStop(globalAgent, global_observeForcedStopCallback, wrap);
}

void igs_die(void){
    initInternalAgentIfNeeded();
    igsAgent_die(globalAgent);
}

int igs_setAgentName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentName(globalAgent, name);
}

char *igs_getAgentName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentName(globalAgent);
}

int igs_setAgentState(const char *state){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentState(globalAgent, state);
}

char *igs_getAgentState(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentState(globalAgent);
}

int igs_mute(void){
    initInternalAgentIfNeeded();
    return igsAgent_mute(globalAgent);
}

int igs_unmute(void){
    initInternalAgentIfNeeded();
    return igsAgent_unmute(globalAgent);
}

bool igs_isMuted(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMuted(globalAgent);
}

typedef struct {
    igs_muteCallback cb;
    void *myData;
} observeMuteCbWrapper_t;

void global_observeMuteCallback(igsAgent_t *agent, bool isMuted, void *myData){
    IGS_UNUSED(agent)
    observeMuteCbWrapper_t *wrap = (observeMuteCbWrapper_t *)myData;
    wrap->cb(isMuted, wrap->myData);
}

int igs_observeMute(igs_muteCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeMuteCbWrapper_t *wrap = calloc(1, sizeof(observeMuteCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeMute(globalAgent, global_observeMuteCallback, wrap);
}

int igs_freeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_freeze(globalAgent);
}

bool igs_isFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_isFrozen(globalAgent);
}

int igs_unfreeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_unfreeze(globalAgent);
}

typedef struct {
    igs_freezeCallback cb;
    void *myData;
} observeFreezeCbWrapper_t;

void global_observeFreezeCallback(igsAgent_t *agent, bool isPaused, void *myData){
    IGS_UNUSED(agent)
    observeFreezeCbWrapper_t *wrap = (observeFreezeCbWrapper_t *)myData;
    wrap->cb(isPaused, wrap->myData);
}

int igs_observeFreeze(igs_freezeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeFreezeCbWrapper_t *wrap = calloc(1, sizeof(observeFreezeCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeFreeze(globalAgent, cb, wrap);
}

void igs_setCanBeFrozen(bool canBeFrozen){
    initInternalAgentIfNeeded();
    igsAgent_setCanBeFrozen(globalAgent, canBeFrozen);
}

bool igs_canBeFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_canBeFrozen(globalAgent);
}


//IOP
int igs_readInput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInput(globalAgent, name, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutput(globalAgent, name, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameter(globalAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsBool(globalAgent, name);
}

int igs_readInputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsInt(globalAgent, name);
}

double igs_readInputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsDouble(globalAgent, name);
}

char* igs_readInputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsString(globalAgent, name);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsData(globalAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsBool(globalAgent, name);
}

int igs_readOutputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsInt(globalAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsDouble(globalAgent, name);
}

char* igs_readOutputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsString(globalAgent, name);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsData(globalAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsBool(globalAgent, name);
}

int igs_readParameterAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsInt(globalAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsDouble(globalAgent, name);
}

char* igs_readParameterAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsString(globalAgent, name);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsData(globalAgent, name, data, size);
}

int igs_writeInputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsBool(globalAgent, name, value);
}

int igs_writeInputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsInt(globalAgent, name, value);
}

int igs_writeInputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsDouble(globalAgent, name, value);
}

int igs_writeInputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsString(globalAgent, name, value);
}

int igs_writeInputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsImpulsion(globalAgent, name);
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsData(globalAgent, name, value, size);
}

int igs_writeOutputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsBool(globalAgent, name, value);
}

int igs_writeOutputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsInt(globalAgent, name, value);
}

int igs_writeOutputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsDouble(globalAgent, name, value);
}

int igs_writeOutputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsString(globalAgent, name, value);
}

int igs_writeOutputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsImpulsion(globalAgent, name);
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsData(globalAgent, name, value, size);
}

int igs_writeParameterAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsBool(globalAgent, name, value);
}

int igs_writeParameterAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsInt(globalAgent, name, value);
}

int igs_writeParameterAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsDouble(globalAgent, name, value);
}

int igs_writeParameterAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsString(globalAgent, name, value);
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsData(globalAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForInput(globalAgent, name);
}

void igs_clearDataForOutput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForOutput(globalAgent, name);
}

void igs_clearDataForParameter(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForParameter(globalAgent, name);
}

typedef struct {
    igs_observeCallback cb;
    void *myData;
} observeIOPCbWrapper_t;

void global_observeIOPCallback(igsAgent_t *agent, iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData){
    IGS_UNUSED(agent)
    observeIOPCbWrapper_t *wrap = (observeIOPCbWrapper_t *)myData;
    wrap->cb(iopType, name, valueType, value, valueSize, wrap->myData);
}

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeInput(globalAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeOutput(globalAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeParameter(globalAgent, name, global_observeIOPCallback, wrap);
}

int igs_muteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_muteOutput(globalAgent, name);
}

int igs_unmuteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_unmuteOutput(globalAgent, name);
}

bool igs_isOutputMuted(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_isOutputMuted(globalAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForInput(globalAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForOutput(globalAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForParameter(globalAgent, name);
}

int igs_getInputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsNumber(globalAgent);
}

int igs_getOutputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsNumber(globalAgent);
}

int igs_getParametersNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersNumber(globalAgent);
}

char** igs_getInputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsList(globalAgent, nbOfElements);
}

char** igs_getOutputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsList(globalAgent, nbOfElements);
}

char** igs_getParametersList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersList(globalAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkInputExistence(globalAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkOutputExistence(globalAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkParameterExistence(globalAgent, name);
}


//definition
int igs_loadDefinition (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinition(globalAgent, json_str);
}

int igs_loadDefinitionFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinitionFromPath(globalAgent, file_path);
}

int igs_clearDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearDefinition(globalAgent);
}

char* igs_getDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinition(globalAgent);
}

char *igs_getDefinitionName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionName(globalAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionDescription(globalAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionVersion(globalAgent);
}
 //returned char* must be freed by caller
int igs_setDefinitionName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionName(globalAgent, name);
}

int igs_setDefinitionDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionDescription(globalAgent, description);
}

int igs_setDefinitionVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionVersion(globalAgent, version);
}

int igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createInput(globalAgent, name, type, value, size);
}

int igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createOutput(globalAgent, name, type, value, size);
}

int igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createParameter(globalAgent, name, type, value, size);
}

int igs_removeInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeInput(globalAgent, name);
}

int igs_removeOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeOutput(globalAgent, name);
}

int igs_removeParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeParameter(globalAgent, name);
}


//mapping
int igs_loadMapping (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadMapping(globalAgent, json_str);
}

int igs_loadMappingFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadMappingFromPath(globalAgent, file_path);
}

int igs_clearMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearMapping(globalAgent);
}

char* igs_getMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(globalAgent);
}

char *igs_getMappingName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(globalAgent);
}

char *igs_getMappingDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingDescription(globalAgent);
}

char *igs_getMappingVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingVersion(globalAgent);
}

int igs_setMappingName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingName(globalAgent, name);
}

int igs_setMappingDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingDescription(globalAgent, description);
}

int igs_setMappingVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingVersion(globalAgent, version);
}

int igs_getMappingEntriesNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingEntriesNumber(globalAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_addMappingEntry(globalAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
int igs_removeMappingEntryWithId(unsigned long theId){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithId(globalAgent, theId);
}

int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithName(globalAgent, fromOurInput, toAgent, withOutput);
}


//admin
void igs_setCommandLine(const char *line){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLine(globalAgent, line);
}

void igs_setCommandLineFromArgs(int argc, const char * argv[]){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLineFromArgs(globalAgent, argc, argv);
}

void igs_setRequestOutputsFromMappedAgents(bool notify){
    initInternalAgentIfNeeded();
    igsAgent_setRequestOutputsFromMappedAgents(globalAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    initInternalAgentIfNeeded();
    return igsAgent_getRequestOutputsFromMappedAgents(globalAgent);
}


void igs_setDefinitionPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setDefinitionPath(globalAgent, path);
}

void igs_setMappingPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setMappingPath(globalAgent, path);
}

void igs_writeDefinitionToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeDefinitionToPath(globalAgent);
}

void igs_writeMappingToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeMappingToPath(globalAgent);
}

void igs_setAllowIpc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowIpc(globalAgent, allow);
}

bool igs_getAllowIpc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowIpc(globalAgent);
}

void igs_setAllowInproc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowInproc(globalAgent, allow);
}

bool igs_getAllowInproc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowInproc(globalAgent);
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igs_setIpcFolderPath(char *path){
    initInternalAgentIfNeeded();
    igsAgent_setIpcFolderPath(globalAgent, path);
}

const char* igs_getIpcFolderPath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getIpcFolderPath(globalAgent);
}
#endif

void igs_setVerbose(bool verbose){
    initInternalAgentIfNeeded();
    igsAgent_setVerbose(globalAgent, verbose);
}

bool igs_isVerbose(void){
    initInternalAgentIfNeeded();
    return igsAgent_isVerbose(globalAgent);
}

void igs_setUseColorVerbose(bool useColor){
    initInternalAgentIfNeeded();
    igsAgent_setUseColorVerbose(globalAgent, useColor);
}

bool igs_getUseColorVerbose(void){
    initInternalAgentIfNeeded();
    return igsAgent_getUseColorVerbose(globalAgent);
}

void igs_setLogStream(bool useLogStream){
    initInternalAgentIfNeeded();
    igsAgent_setLogStream(globalAgent, useLogStream);
}

bool igs_getLogStream(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogStream(globalAgent);
}

void igs_setLogInFile(bool useLogFile){
    initInternalAgentIfNeeded();
    igsAgent_setLogInFile(globalAgent, useLogFile);
}

bool igs_getLogInFile(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogInFile(globalAgent);
}

void igs_setLogPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setLogPath(globalAgent, path);
}

char* igs_getLogPath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogPath(globalAgent);
}


void igs_setLogLevel (igs_logLevel_t level){
    initInternalAgentIfNeeded();
    igsAgent_setLogLevel(globalAgent, level);
}

igs_logLevel_t igs_getLogLevel(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogLevel(globalAgent);
}

void igs_log(igs_logLevel_t level, const char *function, const char *format, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(globalAgent, level, function, "%s", content);
}

//licenses
#if !defined(TARGET_OS_IOS) || !TARGET_OS_IOS
void igs_setLicensePath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setLicensePath(globalAgent, path);
}

char *igs_getLicensePath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLicensePath(globalAgent);
}

bool igs_checkLicenseForAgent(const char *agentId){
    initInternalAgentIfNeeded();
    return igsAgent_checkLicense(globalAgent, agentId);
}

typedef struct {
    igs_licenseCallback cb;
    void *myData;
} observeLicenseCbWrapper_t;

void global_observeLicenseCallback(igsAgent_t *agent, igs_license_limit_t limit, void *myData){
    IGS_UNUSED(agent)
    observeLicenseCbWrapper_t *wrap = (observeLicenseCbWrapper_t *)myData;
    wrap->cb(limit, wrap->myData);
}

int igs_observeLicense(igs_licenseCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeLicenseCbWrapper_t *wrap = calloc(1, sizeof(observeLicenseCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeLicense(globalAgent, global_observeLicenseCallback, wrap);
}

void igs_loadLicenseData(const void *data, size_t size){
    initInternalAgentIfNeeded();
    igsAgent_loadLicenseData(globalAgent, data, size);
}
#endif


//ADVANCED
void igs_setPublishingPort(unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_setPublishingPort(globalAgent, port);
}

void igs_setLogStreamPort(unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_setLogStreamPort(globalAgent, port);
}

void igs_setDiscoveryInterval(unsigned int interval){
    initInternalAgentIfNeeded();
    igsAgent_setDiscoveryInterval(globalAgent, interval);
}

void igs_setAgentTimeout(unsigned int duration){
    initInternalAgentIfNeeded();
    igsAgent_setAgentTimeout(globalAgent, duration);
}

void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg){
    initInternalAgentIfNeeded();
    igsAgent_performanceCheck(globalAgent, peerId, msgSize, nbOfMsg);
}

int igs_timerStart(size_t delay, size_t times, igs_timerCallback cb, void *myData){
    initInternalAgentIfNeeded();
    return igsAgent_timerStart(globalAgent, delay, times, cb, myData);
}

void igs_timerStop(int timerId){
    initInternalAgentIfNeeded();
    igsAgent_timerStop(globalAgent, timerId);
}

void igs_setHighWaterMarks(int hwmValue){
    initInternalAgentIfNeeded();
    igsAgent_setHighWaterMarks(globalAgent, hwmValue);
}


void igs_monitoringEnable(unsigned int period){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnable(globalAgent, period);
}

void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnableWithExpectedDevice(globalAgent, period, networkDevice, port);
}

void igs_monitoringDisable(void){
    initInternalAgentIfNeeded();
    igsAgent_monitoringDisable(globalAgent);
}

bool igs_isMonitoringEnabled(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMonitoringEnabled(globalAgent);
}

typedef struct {
    igs_monitorCallback cb;
    void *myData;
} observeMonitorCbWrapper_t;

void global_observeMonitorCallback(igsAgent_t *agent, igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData){
    IGS_UNUSED(agent)
    observeMonitorCbWrapper_t *wrap = (observeMonitorCbWrapper_t *)myData;
    wrap->cb(event, device, ipAddress, wrap->myData);
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeMonitorCbWrapper_t *wrap = calloc(1, sizeof(observeMonitorCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_monitor(globalAgent, global_observeMonitorCallback, wrap);
}

void igs_monitoringShallStartStopAgent(bool flag){
    initInternalAgentIfNeeded();
    igsAgent_monitoringShallStartStopAgent(globalAgent, flag);
}


int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsZMQMsg(globalAgent, name, msg);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsZMQMsg(globalAgent, name, msg);
}

typedef struct {
    igs_BusMessageIncoming cb;
    void *myData;
} observeBusCbWrapper_t;

void global_observeBusCallback(igsAgent_t *agent, const char *event, const char *peerID, const char *name,
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
    return igsAgent_observeBus(globalAgent, global_observeBusCallback, wrap);
}

void igs_busJoinChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busJoinChannel(globalAgent, channel);
}

void igs_busLeaveChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busLeaveChannel(globalAgent, channel);
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    int res = igsAgent_busSendStringToChannel(globalAgent, channel, msg, list);
    va_end(list);
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToChannel(globalAgent, channel, data, size);
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToChannel(globalAgent, channel, msg_p);
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    int res = igsAgent_busSendStringToAgent(globalAgent, agentNameOrPeerID, "%s", content);
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToAgent(globalAgent, agentNameOrPeerID, data, size);
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToAgent(globalAgent, agentNameOrPeerID, msg_p);
}

void igs_busAddServiceDescription(const char *key, const char *value){
    initInternalAgentIfNeeded();
    igsAgent_busAddServiceDescription(globalAgent, key, value);
}

void igs_busRemoveServiceDescription(const char *key){
    initInternalAgentIfNeeded();
    igsAgent_busRemoveServiceDescription(globalAgent, key);
}


int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    initInternalAgentIfNeeded();
    return igsAgent_sendCall(globalAgent, agentNameOrUUID, callName, list);
}

typedef struct {
    igs_callFunction cb;
    void *myData;
} callCbWrapper_t;

void global_callCallback(igsAgent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
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
    return igsAgent_initCall(globalAgent, name, global_callCallback, wrap);
}

int igs_removeCall(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeCall(globalAgent, name);
}

int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    initInternalAgentIfNeeded();
    return igsAgent_addArgumentToCall(globalAgent, callName, argName, type);
}

int igs_removeArgumentFromCall(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_removeArgumentFromCall(globalAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfCalls(globalAgent);
}

bool igs_checkCallExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallExistence(globalAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getCallsList(globalAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getFirstArgumentForCall(globalAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfArgumentsForCall(globalAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallArgumentExistence(globalAgent, callName, argName);
}
