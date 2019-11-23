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

igsAgent_t *processAgent = NULL;
bool igs_Interrupted = false;

void globalforcedStopCB(igsAgent_t *agent, void *myData){
    IGS_UNUSED(myData)
    igs_Interrupted = true;
}

void initInternalAgentIfNeeded(){
    if (processAgent == NULL){
        processAgent = igsAgent_new();
        igsAgent_observeForcedStop(processAgent, globalforcedStopCB, NULL);
    }
}

int igs_startWithDevice(const char *networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithDevice(processAgent, networkDevice, port);
}

int igs_startWithIP(const char *ipAddress, unsigned int port){
    initInternalAgentIfNeeded();
    igs_Interrupted = false;
    return igsAgent_startWithIP(processAgent, ipAddress, port);
}

int igs_stop(void){
    initInternalAgentIfNeeded();
    return igsAgent_stop(processAgent);
}


typedef struct {
    igs_forcedStopCallback cb;
    void *myData;
} observeForcedStopCbWrapper_t;

void global_observeForcedStopCallback(igsAgent_t *agent, void *myData){
    observeForcedStopCbWrapper_t *wrap = (observeForcedStopCbWrapper_t *)myData;
    wrap->cb(wrap->myData);
}

void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeForcedStopCbWrapper_t *wrap = calloc(1, sizeof(observeForcedStopCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_observeForcedStop(processAgent, global_observeForcedStopCallback, wrap);
}

void igs_die(void){
    initInternalAgentIfNeeded();
    igsAgent_die(processAgent);
}

int igs_setAgentName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentName(processAgent, name);
}

char *igs_getAgentName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentName(processAgent);
}

int igs_setAgentState(const char *state){
    initInternalAgentIfNeeded();
    return igsAgent_setAgentState(processAgent, state);
}

char *igs_getAgentState(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAgentState(processAgent);
}

int igs_mute(void){
    initInternalAgentIfNeeded();
    return igsAgent_mute(processAgent);
}

int igs_unmute(void){
    initInternalAgentIfNeeded();
    return igsAgent_unmute(processAgent);
}

bool igs_isMuted(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMuted(processAgent);
}

typedef struct {
    igs_muteCallback cb;
    void *myData;
} observeMuteCbWrapper_t;

void global_observeMuteCallback(igsAgent_t *agent, bool isMuted, void *myData){
    observeMuteCbWrapper_t *wrap = (observeMuteCbWrapper_t *)myData;
    wrap->cb(isMuted, wrap->myData);
}

int igs_observeMute(igs_muteCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeMuteCbWrapper_t *wrap = calloc(1, sizeof(observeMuteCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeMute(processAgent, global_observeMuteCallback, wrap);
}

int igs_freeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_freeze(processAgent);
}

bool igs_isFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_isFrozen(processAgent);
}

int igs_unfreeze(void){
    initInternalAgentIfNeeded();
    return igsAgent_unfreeze(processAgent);
}

typedef struct {
    igs_freezeCallback cb;
    void *myData;
} observeFreezeCbWrapper_t;

void global_observeFreezeCallback(igsAgent_t *agent, bool isPaused, void *myData){
    observeFreezeCbWrapper_t *wrap = (observeFreezeCbWrapper_t *)myData;
    wrap->cb(isPaused, wrap->myData);
}

int igs_observeFreeze(igs_freezeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeFreezeCbWrapper_t *wrap = calloc(1, sizeof(observeFreezeCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeFreeze(processAgent, cb, wrap);
}

void igs_setCanBeFrozen(bool canBeFrozen){
    initInternalAgentIfNeeded();
    igsAgent_setCanBeFrozen(processAgent, canBeFrozen);
}

bool igs_canBeFrozen(void){
    initInternalAgentIfNeeded();
    return igsAgent_canBeFrozen(processAgent);
}


//IOP
int igs_readInput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInput(processAgent, name, value, size);
}

int igs_readOutput(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutput(processAgent, name, value, size);
}

int igs_readParameter(const char *name, void **value, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameter(processAgent, name, value, size);
}

bool igs_readInputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsBool(processAgent, name);
}

int igs_readInputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsInt(processAgent, name);
}

double igs_readInputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsDouble(processAgent, name);
}

char* igs_readInputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsString(processAgent, name);
}

int igs_readInputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsData(processAgent, name, data, size);
}

bool igs_readOutputAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsBool(processAgent, name);
}

int igs_readOutputAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsInt(processAgent, name);
}

double igs_readOutputAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsDouble(processAgent, name);
}

char* igs_readOutputAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsString(processAgent, name);
}

int igs_readOutputAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readOutputAsData(processAgent, name, data, size);
}

bool igs_readParameterAsBool(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsBool(processAgent, name);
}

int igs_readParameterAsInt(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsInt(processAgent, name);
}

double igs_readParameterAsDouble(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsDouble(processAgent, name);
}

char* igs_readParameterAsString(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsString(processAgent, name);
}

int igs_readParameterAsData(const char *name, void **data, size_t *size){
    initInternalAgentIfNeeded();
    return igsAgent_readParameterAsData(processAgent, name, data, size);
}

int igs_writeInputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsBool(processAgent, name, value);
}

int igs_writeInputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsInt(processAgent, name, value);
}

int igs_writeInputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsDouble(processAgent, name, value);
}

int igs_writeInputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsString(processAgent, name, value);
}

int igs_writeInputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsImpulsion(processAgent, name);
}

int igs_writeInputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeInputAsData(processAgent, name, value, size);
}

int igs_writeOutputAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsBool(processAgent, name, value);
}

int igs_writeOutputAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsInt(processAgent, name, value);
}

int igs_writeOutputAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsDouble(processAgent, name, value);
}

int igs_writeOutputAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsString(processAgent, name, value);
}

int igs_writeOutputAsImpulsion(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsImpulsion(processAgent, name);
}

int igs_writeOutputAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsData(processAgent, name, value, size);
}

int igs_writeParameterAsBool(const char *name, bool value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsBool(processAgent, name, value);
}

int igs_writeParameterAsInt(const char *name, int value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsInt(processAgent, name, value);
}

int igs_writeParameterAsDouble(const char *name, double value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsDouble(processAgent, name, value);
}

int igs_writeParameterAsString(const char *name, const char *value){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsString(processAgent, name, value);
}

int igs_writeParameterAsData(const char *name, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_writeParameterAsData(processAgent, name, value, size);
}

void igs_clearDataForInput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForInput(processAgent, name);
}

void igs_clearDataForOutput(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForOutput(processAgent, name);
}

void igs_clearDataForParameter(const char *name){
    initInternalAgentIfNeeded();
    igsAgent_clearDataForParameter(processAgent, name);
}

typedef struct {
    igs_observeCallback cb;
    void *myData;
} observeIOPCbWrapper_t;

void global_observeIOPCallback(igsAgent_t *agent, iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData){
    observeIOPCbWrapper_t *wrap = (observeIOPCbWrapper_t *)myData;
    wrap->cb(iopType, name, valueType, value, valueSize, wrap->myData);
}

int igs_observeInput(const char *name, igs_observeCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeInput(processAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeOutput(processAgent, name, global_observeIOPCallback, wrap);
}

int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData){
    initInternalAgentIfNeeded();
    observeIOPCbWrapper_t *wrap = calloc(1, sizeof(observeIOPCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeParameter(processAgent, name, global_observeIOPCallback, wrap);
}

int igs_muteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_muteOutput(processAgent, name);
}

int igs_unmuteOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_unmuteOutput(processAgent, name);
}

bool igs_isOutputMuted(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_isOutputMuted(processAgent, name);
}

iopType_t igs_getTypeForInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForInput(processAgent, name);
}

iopType_t igs_getTypeForOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForOutput(processAgent, name);
}

iopType_t igs_getTypeForParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_getTypeForParameter(processAgent, name);
}

int igs_getInputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsNumber(processAgent);
}

int igs_getOutputsNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsNumber(processAgent);
}

int igs_getParametersNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersNumber(processAgent);
}

char** igs_getInputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getInputsList(processAgent, nbOfElements);
}

char** igs_getOutputsList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getOutputsList(processAgent, nbOfElements);
}

char** igs_getParametersList(long *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getParametersList(processAgent, nbOfElements);
}

bool igs_checkInputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkInputExistence(processAgent, name);
}

bool igs_checkOutputExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkOutputExistence(processAgent, name);
}

bool igs_checkParameterExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkParameterExistence(processAgent, name);
}


//definition
int igs_loadDefinition (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinition(processAgent, json_str);
}

int igs_loadDefinitionFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadDefinitionFromPath(processAgent, file_path);
}

int igs_clearDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearDefinition(processAgent);
}

char* igs_getDefinition(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinition(processAgent);
}

char *igs_getDefinitionName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionName(processAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionDescription(processAgent);
}
 //returned char* must be freed by caller
char *igs_getDefinitionVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getDefinitionVersion(processAgent);
}
 //returned char* must be freed by caller
int igs_setDefinitionName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionName(processAgent, name);
}

int igs_setDefinitionDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionDescription(processAgent, description);
}

int igs_setDefinitionVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setDefinitionVersion(processAgent, version);
}

int igs_createInput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createInput(processAgent, name, type, value, size);
}

int igs_createOutput(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createOutput(processAgent, name, type, value, size);
}

int igs_createParameter(const char *name, iopType_t type, void *value, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_createParameter(processAgent, name, type, value, size);
}

int igs_removeInput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeInput(processAgent, name);
}

int igs_removeOutput(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeOutput(processAgent, name);
}

int igs_removeParameter(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeParameter(processAgent, name);
}


//mapping
int igs_loadMapping (const char* json_str){
    initInternalAgentIfNeeded();
    return igsAgent_loadMapping(processAgent, json_str);
}

int igs_loadMappingFromPath (const char* file_path){
    initInternalAgentIfNeeded();
    return igsAgent_loadMappingFromPath(processAgent, file_path);
}

int igs_clearMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_clearMapping(processAgent);
}

char* igs_getMapping(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(processAgent);
}

char *igs_getMappingName(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMapping(processAgent);
}

char *igs_getMappingDescription(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingDescription(processAgent);
}

char *igs_getMappingVersion(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingVersion(processAgent);
}

int igs_setMappingName(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingName(processAgent, name);
}

int igs_setMappingDescription(const char *description){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingDescription(processAgent, description);
}

int igs_setMappingVersion(const char *version){
    initInternalAgentIfNeeded();
    return igsAgent_setMappingVersion(processAgent, version);
}

int igs_getMappingEntriesNumber(void){
    initInternalAgentIfNeeded();
    return igsAgent_getMappingEntriesNumber(processAgent);
}

unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_addMappingEntry(processAgent, fromOurInput, toAgent, withOutput);
}
 //returns mapping id or zero or below if creation failed
int igs_removeMappingEntryWithId(unsigned long theId){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithId(processAgent, theId);
}

int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput){
    initInternalAgentIfNeeded();
    return igsAgent_removeMappingEntryWithName(processAgent, fromOurInput, toAgent, withOutput);
}


//admin
void igs_setCommandLine(const char *line){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLine(processAgent, line);
}

void igs_setCommandLineFromArgs(int argc, const char * argv[]){
    initInternalAgentIfNeeded();
    igsAgent_setCommandLineFromArgs(processAgent, argc, argv);
}

void igs_setRequestOutputsFromMappedAgents(bool notify){
    initInternalAgentIfNeeded();
    igsAgent_setRequestOutputsFromMappedAgents(processAgent, notify);
}

bool igs_getRequestOutputsFromMappedAgents(void){
    initInternalAgentIfNeeded();
    return igsAgent_getRequestOutputsFromMappedAgents(processAgent);
}


void igs_setDefinitionPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setDefinitionPath(processAgent, path);
}

void igs_setMappingPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setMappingPath(processAgent, path);
}

void igs_writeDefinitionToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeDefinitionToPath(processAgent);
}

void igs_writeMappingToPath(void){
    initInternalAgentIfNeeded();
    igsAgent_writeMappingToPath(processAgent);
}

void igs_setAllowIpc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowIpc(processAgent, allow);
}

bool igs_getAllowIpc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowIpc(processAgent);
}

void igs_setAllowInproc(bool allow){
    initInternalAgentIfNeeded();
    igsAgent_setAllowInproc(processAgent, allow);
}

bool igs_getAllowInproc(void){
    initInternalAgentIfNeeded();
    return igsAgent_getAllowInproc(processAgent);
}

#if defined __unix__ || defined __APPLE__ || defined __linux__
void igs_setIpcFolderPath(char *path){
    initInternalAgentIfNeeded();
    igsAgent_setIpcFolderPath(processAgent, path);
}

const char* igs_getIpcFolderPath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getIpcFolderPath(processAgent);
}
#endif

void igs_setVerbose(bool verbose){
    initInternalAgentIfNeeded();
    igsAgent_setVerbose(processAgent, verbose);
}

bool igs_isVerbose(void){
    initInternalAgentIfNeeded();
    return igsAgent_isVerbose(processAgent);
}

void igs_setUseColorVerbose(bool useColor){
    initInternalAgentIfNeeded();
    igsAgent_setUseColorVerbose(processAgent, useColor);
}

bool igs_getUseColorVerbose(void){
    initInternalAgentIfNeeded();
    return igsAgent_getUseColorVerbose(processAgent);
}

void igs_setLogStream(bool useLogStream){
    initInternalAgentIfNeeded();
    igsAgent_setLogStream(processAgent, useLogStream);
}

bool igs_getLogStream(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogStream(processAgent);
}

void igs_setLogInFile(bool useLogFile){
    initInternalAgentIfNeeded();
    igsAgent_setLogInFile(processAgent, useLogFile);
}

bool igs_getLogInFile(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogInFile(processAgent);
}

void igs_setLogPath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setLogPath(processAgent, path);
}

char* igs_getLogPath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLogPath(processAgent);
}


void igs_setLogLevel (igs_logLevel_t level){
    initInternalAgentIfNeeded();
    igsAgent_setLogLevel(processAgent, level);
}


void igs_log(igs_logLevel_t level, const char *function, const char *format, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, format);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end(list);
    admin_log(processAgent, level, function, "%s", content);
}

//licenses
#if !defined(TARGET_OS_IOS) || !TARGET_OS_IOS
void igs_setLicensePath(const char *path){
    initInternalAgentIfNeeded();
    igsAgent_setLicensePath(processAgent, path);
}

char *igs_getLicensePath(void){
    initInternalAgentIfNeeded();
    return igsAgent_getLicensePath(processAgent);
}

bool igs_checkLicenseForAgent(const char *agentId){
    initInternalAgentIfNeeded();
    return igsAgent_checkLicense(processAgent, agentId);
}

typedef struct {
    igs_licenseCallback cb;
    void *myData;
} observeLicenseCbWrapper_t;

void global_observeLicenseCallback(igsAgent_t *agent, igs_license_limit_t limit, void *myData){
    observeLicenseCbWrapper_t *wrap = (observeLicenseCbWrapper_t *)myData;
    wrap->cb(limit, wrap->myData);
}

int igs_observeLicense(igs_licenseCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeLicenseCbWrapper_t *wrap = calloc(1, sizeof(observeLicenseCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeLicense(processAgent, global_observeLicenseCallback, wrap);
}
#endif


//ADVANCED
void igs_setPublishingPort(unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_setPublishingPort(processAgent, port);
}

void igs_setDiscoveryInterval(unsigned int interval){
    initInternalAgentIfNeeded();
    igsAgent_setDiscoveryInterval(processAgent, interval);
}

void igs_setAgentTimeout(unsigned int duration){
    initInternalAgentIfNeeded();
    igsAgent_setAgentTimeout(processAgent, duration);
}
 //in milliseconds
void igs_performanceCheck(const char *peerId, size_t msgSize, size_t nbOfMsg){
    initInternalAgentIfNeeded();
    igsAgent_performanceCheck(processAgent, peerId, msgSize, nbOfMsg);
}

void igs_setHighWaterMarks(int hwmValue){
    initInternalAgentIfNeeded();
    igsAgent_setHighWaterMarks(processAgent, hwmValue);
}


void igs_monitoringEnable(unsigned int period){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnable(processAgent, period);
}

void igs_monitoringEnableWithExpectedDevice(unsigned int period, const char* networkDevice, unsigned int port){
    initInternalAgentIfNeeded();
    igsAgent_monitoringEnableWithExpectedDevice(processAgent, period, networkDevice, port);
}

void igs_monitoringDisable(void){
    initInternalAgentIfNeeded();
    igsAgent_monitoringDisable(processAgent);
}

bool igs_isMonitoringEnabled(void){
    initInternalAgentIfNeeded();
    return igsAgent_isMonitoringEnabled(processAgent);
}

typedef struct {
    igs_monitorCallback cb;
    void *myData;
} observeMonitorCbWrapper_t;

void global_observeMonitorCallback(igsAgent_t *agent, igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData){
    observeMonitorCbWrapper_t *wrap = (observeMonitorCbWrapper_t *)myData;
    wrap->cb(event, device, ipAddress, wrap->myData);
}

void igs_monitor(igs_monitorCallback cb, void *myData){
    initInternalAgentIfNeeded();
    observeMonitorCbWrapper_t *wrap = calloc(1, sizeof(observeMonitorCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    igsAgent_monitor(processAgent, global_observeMonitorCallback, wrap);
}

void igs_monitoringShallStartStopAgent(bool flag){
    initInternalAgentIfNeeded();
    igsAgent_monitoringShallStartStopAgent(processAgent, flag);
}


int igs_writeOutputAsZMQMsg(const char *name, zmsg_t *msg){
    initInternalAgentIfNeeded();
    return igsAgent_writeOutputAsZMQMsg(processAgent, name, msg);
}

int igs_readInputAsZMQMsg(const char *name, zmsg_t **msg){
    initInternalAgentIfNeeded();
    return igsAgent_readInputAsZMQMsg(processAgent, name, msg);
}

typedef struct {
    igs_BusMessageIncoming cb;
    void *myData;
} observeBusCbWrapper_t;

void global_observeBusCallback(igsAgent_t *agent, const char *event, const char *peerID, const char *name,
                               const char *address, const char *channel,
                               zhash_t *headers, zmsg_t *msg, void *myData){
    observeBusCbWrapper_t *wrap = (observeBusCbWrapper_t *)myData;
    wrap->cb(event, peerID, name, address, channel, headers, msg, wrap->myData);
}

int igs_observeBus(igs_BusMessageIncoming cb, void *myData){
    initInternalAgentIfNeeded();
    observeBusCbWrapper_t *wrap = calloc(1, sizeof(observeBusCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_observeBus(processAgent, global_observeBusCallback, wrap);
}

void igs_busJoinChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busJoinChannel(processAgent, channel);
}

void igs_busLeaveChannel(const char *channel){
    initInternalAgentIfNeeded();
    igsAgent_busLeaveChannel(processAgent, channel);
}

int igs_busSendStringToChannel(const char *channel, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    int res = igsAgent_busSendStringToChannel(processAgent, channel, msg, list);
    va_end(list);
    return res;
}

int igs_busSendDataToChannel(const char *channel, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToChannel(processAgent, channel, data, size);
}

int igs_busSendZMQMsgToChannel(const char *channel, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToChannel(processAgent, channel, msg_p);
}

int igs_busSendStringToAgent(const char *agentNameOrPeerID, const char *msg, ...){
    initInternalAgentIfNeeded();
    va_list list;
    va_start(list, msg);
    char content[MAX_STRING_MSG_LENGTH] = "";
    vsnprintf(content, MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end(list);
    int res = igsAgent_busSendStringToAgent(processAgent, agentNameOrPeerID, "%s", content);
    return res;
}

int igs_busSendDataToAgent(const char *agentNameOrPeerID, void *data, size_t size){
    initInternalAgentIfNeeded();
    return igsAgent_busSendDataToAgent(processAgent, agentNameOrPeerID, data, size);
}

int igs_busSendZMQMsgToAgent(const char *agentNameOrPeerID, zmsg_t **msg_p){
    initInternalAgentIfNeeded();
    return igsAgent_busSendZMQMsgToAgent(processAgent, agentNameOrPeerID, msg_p);
}

void igs_busAddServiceDescription(const char *key, const char *value){
    initInternalAgentIfNeeded();
    igsAgent_busAddServiceDescription(processAgent, key, value);
}

void igs_busRemoveServiceDescription(const char *key){
    initInternalAgentIfNeeded();
    igsAgent_busRemoveServiceDescription(processAgent, key);
}


int igs_sendCall(const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list){
    initInternalAgentIfNeeded();
    return igsAgent_sendCall(processAgent, agentNameOrUUID, callName, list);
}

typedef struct {
    igs_callFunction cb;
    void *myData;
} callCbWrapper_t;

void global_callCallback(igsAgent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                         const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                         void* myData){
    callCbWrapper_t *wrap = (callCbWrapper_t *)myData;
    wrap->cb(senderAgentName, senderAgentUUID, callName, firstArgument, nbArgs, wrap->myData);
}

int igs_initCall(const char *name, igs_callFunction cb, void *myData){
    initInternalAgentIfNeeded();
    callCbWrapper_t *wrap = calloc(1, sizeof(callCbWrapper_t));
    wrap->cb = cb;
    wrap->myData = myData;
    return igsAgent_initCall(processAgent, name, global_callCallback, wrap);
}

int igs_removeCall(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_removeCall(processAgent, name);
}

int igs_addArgumentToCall(const char *callName, const char *argName, iopType_t type){
    initInternalAgentIfNeeded();
    return igsAgent_addArgumentToCall(processAgent, callName, argName, type);
}

int igs_removeArgumentFromCall(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_removeArgumentFromCall(processAgent, callName, argName);
}
 //removes first occurence with this name
size_t igs_getNumberOfCalls(void){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfCalls(processAgent);
}

bool igs_checkCallExistence(const char *name){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallExistence(processAgent, name);
}

char** igs_getCallsList(size_t *nbOfElements){
    initInternalAgentIfNeeded();
    return igsAgent_getCallsList(processAgent, nbOfElements);
}

igs_callArgument_t* igs_getFirstArgumentForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getFirstArgumentForCall(processAgent, callName);
}

size_t igs_getNumberOfArgumentsForCall(const char *callName){
    initInternalAgentIfNeeded();
    return igsAgent_getNumberOfArgumentsForCall(processAgent, callName);
}

bool igs_checkCallArgumentExistence(const char *callName, const char *argName){
    initInternalAgentIfNeeded();
    return igsAgent_checkCallArgumentExistence(processAgent, callName, argName);
}
