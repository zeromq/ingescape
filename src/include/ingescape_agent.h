//
//  ingescape_class.h
//  ingescape
//
//  Created by Stephane Vales on 18/11/2019.
//  Copyright © 2019 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_class_h
#define ingescape_class_h

#include "ingescape.h"
#include "ingescape_advanced.h"

#ifdef __cplusplus
extern "C" {
#endif

//agent creation and destruction
typedef struct _igsAgent_t igsAgent_t;
PUBLIC igsAgent_t *igsAgent_new(void);
PUBLIC void igsAgent_destroy(igsAgent_t **agent);

/*
 All the functions below behave the same as the functions presented
 in ingescape.h, except that they take an agent instance as first argument.
 */

//initialization and control
PUBLIC int igsAgent_startWithDevice(igsAgent_t *agent, const char *networkDevice, unsigned int port);
PUBLIC int igsAgent_startWithIP(igsAgent_t *agent, const char *ipAddress, unsigned int port);
PUBLIC int igsAgent_stop(igsAgent_t *agent);

typedef void (*igsAgent_forcedStopCallback)(igsAgent_t *agent, void *myData);
PUBLIC void igsAgent_observeForcedStop(igsAgent_t *agent, igsAgent_forcedStopCallback cb, void *myData);

PUBLIC void igsAgent_die(igsAgent_t *agent);
PUBLIC int igsAgent_setAgentName(igsAgent_t *agent, const char *name);
PUBLIC char *igsAgent_getAgentName(igsAgent_t *agent); //char* must be freed by caller
PUBLIC int igsAgent_setAgentState(igsAgent_t *agent, const char *state);
PUBLIC char *igsAgent_getAgentState(igsAgent_t *agent); //char* must be freed by caller

PUBLIC int igsAgent_mute(igsAgent_t *agent);
PUBLIC int igsAgent_unmute(igsAgent_t *agent);
PUBLIC bool igsAgent_isMuted(igsAgent_t *agent);
typedef void (*igsAgent_muteCallback)(igsAgent_t *Agent, bool isMuted, void *myData);
PUBLIC int igsAgent_observeMute(igsAgent_t *agent, igsAgent_muteCallback cb, void *myData);

PUBLIC int igsAgent_freeze(igsAgent_t *agent);
PUBLIC bool igsAgent_isFrozen(igsAgent_t *agent);
PUBLIC int igsAgent_unfreeze(igsAgent_t *agent);
typedef void (*igsAgent_freezeCallback)(bool isPaused, void *myData);
PUBLIC int igsAgent_observeFreeze(igsAgent_t *agent, igsAgent_freezeCallback cb, void *myData);
PUBLIC void igsAgent_setCanBeFrozen(igsAgent_t *agent, bool canBeFrozen);
PUBLIC bool igsAgent_canBeFrozen(igsAgent_t *agent);

//IOP read, write, creation, destruction, introspection
PUBLIC int igsAgent_readInput(igsAgent_t *agent, const char *name, void **value, size_t *size);
PUBLIC int igsAgent_readOutput(igsAgent_t *agent, const char *name, void **value, size_t *size);
PUBLIC int igsAgent_readParameter(igsAgent_t *agent, const char *name, void **value, size_t *size);
PUBLIC bool igsAgent_readInputAsBool(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_readInputAsInt(igsAgent_t *agent, const char *name);
PUBLIC double igsAgent_readInputAsDouble(igsAgent_t *agent, const char *name);
PUBLIC char* igsAgent_readInputAsString(igsAgent_t *agent, const char *name); //returned char* must be freed by caller
PUBLIC int igsAgent_readInputAsData(igsAgent_t *agent, const char *name, void **data, size_t *size); //returned data must be freed by caller
PUBLIC bool igsAgent_readOutputAsBool(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_readOutputAsInt(igsAgent_t *agent, const char *name);
PUBLIC double igsAgent_readOutputAsDouble(igsAgent_t *agent, const char *name);
PUBLIC char* igsAgent_readOutputAsString(igsAgent_t *agent, const char *name); //returned char* must be freed by caller
PUBLIC int igsAgent_readOutputAsData(igsAgent_t *agent, const char *name, void **data, size_t *size); //returned data must be freed by caller
PUBLIC bool igsAgent_readParameterAsBool(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_readParameterAsInt(igsAgent_t *agent, const char *name);
PUBLIC double igsAgent_readParameterAsDouble(igsAgent_t *agent, const char *name);
PUBLIC char* igsAgent_readParameterAsString(igsAgent_t *agent, const char *name); //returned char* must be freed by caller
PUBLIC int igsAgent_readParameterAsData(igsAgent_t *agent, const char *name, void **data, size_t *size); //returned data must be freed by caller
PUBLIC int igsAgent_writeInputAsBool(igsAgent_t *agent, const char *name, bool value);
PUBLIC int igsAgent_writeInputAsInt(igsAgent_t *agent, const char *name, int value);
PUBLIC int igsAgent_writeInputAsDouble(igsAgent_t *agent, const char *name, double value);
PUBLIC int igsAgent_writeInputAsString(igsAgent_t *agent, const char *name, const char *value);
PUBLIC int igsAgent_writeInputAsImpulsion(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_writeInputAsData(igsAgent_t *agent, const char *name, void *value, size_t size);
PUBLIC int igsAgent_writeOutputAsBool(igsAgent_t *agent, const char *name, bool value);
PUBLIC int igsAgent_writeOutputAsInt(igsAgent_t *agent, const char *name, int value);
PUBLIC int igsAgent_writeOutputAsDouble(igsAgent_t *agent, const char *name, double value);
PUBLIC int igsAgent_writeOutputAsString(igsAgent_t *agent, const char *name, const char *value);
PUBLIC int igsAgent_writeOutputAsImpulsion(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_writeOutputAsData(igsAgent_t *agent, const char *name, void *value, size_t size);
PUBLIC int igsAgent_writeParameterAsBool(igsAgent_t *agent, const char *name, bool value);
PUBLIC int igsAgent_writeParameterAsInt(igsAgent_t *agent, const char *name, int value);
PUBLIC int igsAgent_writeParameterAsDouble(igsAgent_t *agent, const char *name, double value);
PUBLIC int igsAgent_writeParameterAsString(igsAgent_t *agent, const char *name, const char *value);
PUBLIC int igsAgent_writeParameterAsData(igsAgent_t *agent, const char *name, void *value, size_t size);
PUBLIC void igsAgent_clearDataForInput(igsAgent_t *agent, const char *name);
PUBLIC void igsAgent_clearDataForOutput(igsAgent_t *agent, const char *name);
PUBLIC void igsAgent_clearDataForParameter(igsAgent_t *agent, const char *name);

typedef void (*igsAgent_observeCallback)(igsAgent_t *agent, iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData);
PUBLIC int igsAgent_observeInput(igsAgent_t *agent, const char *name, igsAgent_observeCallback cb, void *myData);
PUBLIC int igsAgent_observeOutput(igsAgent_t *agent, const char *name, igsAgent_observeCallback cb, void * myData);
PUBLIC int igsAgent_observeParameter(igsAgent_t *agent, const char *name, igsAgent_observeCallback cb, void * myData);

PUBLIC int igsAgent_muteOutput(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_unmuteOutput(igsAgent_t *agent, const char *name);
PUBLIC bool igsAgent_isOutputMuted(igsAgent_t *agent, const char *name);
PUBLIC iopType_t igsAgent_getTypeForInput(igsAgent_t *agent, const char *name);
PUBLIC iopType_t igsAgent_getTypeForOutput(igsAgent_t *agent, const char *name);
PUBLIC iopType_t igsAgent_getTypeForParameter(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_getInputsNumber(igsAgent_t *agent);
PUBLIC int igsAgent_getOutputsNumber(igsAgent_t *agent);
PUBLIC int igsAgent_getParametersNumber(igsAgent_t *agent);
PUBLIC char** igsAgent_getInputsList(igsAgent_t *agent, long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igsAgent_getOutputsList(igsAgent_t *agent, long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igsAgent_getParametersList(igsAgent_t *agent, long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC bool igsAgent_checkInputExistence(igsAgent_t *agent, const char *name);
PUBLIC bool igsAgent_checkOutputExistence(igsAgent_t *agent, const char *name);
PUBLIC bool igsAgent_checkParameterExistence(igsAgent_t *agent, const char *name);

//definition
PUBLIC int igsAgent_loadDefinition (igsAgent_t *agent, const char* json_str);
PUBLIC int igsAgent_loadDefinitionFromPath (igsAgent_t *agent, const char* file_path);
PUBLIC int igsAgent_clearDefinition(igsAgent_t *agent);
PUBLIC char* igsAgent_getDefinition(igsAgent_t *agent);
PUBLIC char *igsAgent_getDefinitionName(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC char *igsAgent_getDefinitionDescription(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC char *igsAgent_getDefinitionVersion(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC int igsAgent_setDefinitionName(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_setDefinitionDescription(igsAgent_t *agent, const char *description);
PUBLIC int igsAgent_setDefinitionVersion(igsAgent_t *agent, const char *version);
PUBLIC int igsAgent_createInput(igsAgent_t *agent, const char *name, iopType_t value_type, void*value, size_t size);
PUBLIC int igsAgent_createOutput(igsAgent_t *agent, const char *name, iopType_t type, void *value, size_t size);
PUBLIC int igsAgent_createParameter(igsAgent_t *agent, const char *name, iopType_t type, void *value, size_t size);
PUBLIC int igsAgent_removeInput(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_removeOutput(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_removeParameter(igsAgent_t *agent, const char *name);

//mapping
PUBLIC int igsAgent_loadMapping (igsAgent_t *agent, const char* json_str);
PUBLIC int igsAgent_loadMappingFromPath (igsAgent_t *agent, const char* file_path);
PUBLIC int igsAgent_clearMapping(igsAgent_t *agent);
PUBLIC char* igsAgent_getMapping(igsAgent_t *agent); //returns json string, must be freed by caller
PUBLIC char *igsAgent_getMappingName(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC char *igsAgent_getMappingDescription(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC char *igsAgent_getMappingVersion(igsAgent_t *agent); //returned char* must be freed by caller
PUBLIC int igsAgent_setMappingName(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_setMappingDescription(igsAgent_t *agent, const char *description);
PUBLIC int igsAgent_setMappingVersion(igsAgent_t *agent, const char *version);
PUBLIC int igsAgent_getMappingEntriesNumber(igsAgent_t *agent);
PUBLIC unsigned long igsAgent_addMappingEntry(igsAgent_t *agent, const char *fromOurInput, const char *toAgent, const char *withOutput);
PUBLIC int igsAgent_removeMappingEntryWithId(igsAgent_t *agent, unsigned long theId);
PUBLIC int igsAgent_removeMappingEntryWithName(igsAgent_t *agent, const char *fromOurInput, const char *toAgent, const char *withOutput);

//admin
PUBLIC void igsAgent_setCommandLine(igsAgent_t *agent, const char *line);
PUBLIC void igsAgent_setCommandLineFromArgs(igsAgent_t *agent, int argc, const char * argv[]);
PUBLIC void igsAgent_setRequestOutputsFromMappedAgents(igsAgent_t *agent, bool notify);
PUBLIC bool igsAgent_getRequestOutputsFromMappedAgents(igsAgent_t *agent);

PUBLIC void igsAgent_setVerbose(igsAgent_t *agent, bool verbose); //enable logs in console (ERROR and FATAL are always displayed)
PUBLIC bool igsAgent_isVerbose(igsAgent_t *agent);
PUBLIC void igsAgent_setUseColorVerbose(igsAgent_t *agent, bool useColor); //use colors in console
PUBLIC bool igsAgent_getUseColorVerbose(igsAgent_t *agent);
PUBLIC void igsAgent_setLogStream(igsAgent_t *agent, bool useLogStream); //enable logs in socket stream
PUBLIC bool igsAgent_getLogStream(igsAgent_t *agent);
PUBLIC void igsAgent_setLogInFile(igsAgent_t *agent, bool useLogFile); //enable logs in file
PUBLIC bool igsAgent_getLogInFile(igsAgent_t *agent);
PUBLIC void igsAgent_setLogPath(igsAgent_t *agent, const char *path); //default directory is ~/ on UNIX systems and current PATH on Windows
PUBLIC char* igsAgent_getLogPath(igsAgent_t *agent); // must be freed by caller

PUBLIC void igsAgent_setLogLevel (igsAgent_t *agent, igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
PUBLIC igs_logLevel_t igsAgent_getLogLevel(igsAgent_t *agent);

PUBLIC void igsAgent_log(igs_logLevel_t level, const char *function, igsAgent_t *agent, const char *format, ...) CHECK_PRINTF (4);
#define igsAgent_trace(...) igsAgent_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
#define igsAgent_debug(...) igsAgent_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
#define igsAgent_info(...) igsAgent_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
#define igsAgent_warn(...) igsAgent_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
#define igsAgent_error(...) igsAgent_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
#define igsAgent_fatal(...) igsAgent_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)

PUBLIC void igsAgent_setDefinitionPath(igsAgent_t *agent, const char *path);
PUBLIC void igsAgent_setMappingPath(igsAgent_t *agent, const char *path);
PUBLIC void igsAgent_writeDefinitionToPath(igsAgent_t *agent);
PUBLIC void igsAgent_writeMappingToPath(igsAgent_t *agent);
PUBLIC void igsAgent_setAllowIpc(igsAgent_t *agent, bool allow);
PUBLIC bool igsAgent_getAllowIpc(igsAgent_t *agent);
#if defined __unix__ || defined __APPLE__ || defined __linux__
PUBLIC void igsAgent_setIpcFolderPath(igsAgent_t *agent, char *path);
PUBLIC const char* igsAgent_getIpcFolderPath(igsAgent_t *agent);
PUBLIC void igsAgent_setAllowInproc(igsAgent_t *agent, bool allow);
PUBLIC bool igsAgent_getAllowInproc(igsAgent_t *agent);
#endif

//licenses
typedef void (*igsAgent_licenseCallback)(igsAgent_t *agent, igs_license_limit_t limit, void *myData);
#if !defined(TARGET_OS_IOS) || !TARGET_OS_IOS
PUBLIC void igsAgent_setLicensePath(igsAgent_t *agent, const char *path);
PUBLIC char *igsAgent_getLicensePath(igsAgent_t *agent);
PUBLIC bool igsAgent_checkLicense(igsAgent_t *agent, const char *agentId);
PUBLIC int igsAgent_observeLicense(igsAgent_t *agent, igsAgent_licenseCallback cb, void *myData);
PUBLIC void igsAgent_loadLicenseData(igsAgent_t *agent, const void *data, size_t size);
#endif

//advanced functions
PUBLIC void igsAgent_setPublishingPort(igsAgent_t *agent, unsigned int port);
PUBLIC void igsAgent_setLogStreamPort(igsAgent_t *agent, unsigned int port);
PUBLIC void igsAgent_setDiscoveryInterval(igsAgent_t *agent, unsigned int interval); //in milliseconds
PUBLIC void igsAgent_setAgentTimeout(igsAgent_t *agent, unsigned int duration); //in milliseconds
PUBLIC void igsAgent_performanceCheck(igsAgent_t *agent, const char *peerId, size_t msgSize, size_t nbOfMsg);
PUBLIC void igsAgent_setHighWaterMarks(igsAgent_t *agent, int hwmValue);

PUBLIC void igsAgent_monitoringEnable(igsAgent_t *agent, unsigned int period); //in milliseconds
PUBLIC void igsAgent_monitoringEnableWithExpectedDevice(igsAgent_t *agent, unsigned int period, const char* networkDevice, unsigned int port);
PUBLIC void igsAgent_monitoringDisable(igsAgent_t *agent);
PUBLIC bool igsAgent_isMonitoringEnabled(igsAgent_t *agent);
typedef void (*igsAgent_monitorCallback)(igsAgent_t *agent, igs_monitorEvent_t event, const char *device, const char *ipAddress, void *myData);
PUBLIC void igsAgent_monitor(igsAgent_t *agent, igsAgent_monitorCallback cb, void *myData);
PUBLIC void igsAgent_monitoringShallStartStopAgent(igsAgent_t *agent, bool flag);

PUBLIC int igsAgent_writeOutputAsZMQMsg(igsAgent_t *agent, const char *name, zmsg_t *msg);
PUBLIC int igsAgent_readInputAsZMQMsg(igsAgent_t *agent, const char *name, zmsg_t **msg); //msg must be freed by caller using zmsg_destroy

typedef void (*igsAgent_BusMessageIncoming) (igsAgent_t *agent, const char *event, const char *peerID, const char *name,
                                             const char *address, const char *channel,
                                             zhash_t *headers, zmsg_t *msg, void *myData);
PUBLIC int igsAgent_observeBus(igsAgent_t *agent, igsAgent_BusMessageIncoming cb, void *myData);
PUBLIC void igsAgent_busJoinChannel(igsAgent_t *agent, const char *channel);
PUBLIC void igsAgent_busLeaveChannel(igsAgent_t *agent, const char *channel);
PUBLIC int igsAgent_busSendStringToChannel(igsAgent_t *agent, const char *channel, const char *msg, ...);
PUBLIC int igsAgent_busSendDataToChannel(igsAgent_t *agent, const char *channel, void *data, size_t size);
PUBLIC int igsAgent_busSendZMQMsgToChannel(igsAgent_t *agent, const char *channel, zmsg_t **msg_p); //destroys message after sending it
PUBLIC int igsAgent_busSendStringToAgent(igsAgent_t *agent, const char *agentNameOrPeerID, const char *msg, ...);
PUBLIC int igsAgent_busSendDataToAgent(igsAgent_t *agent, const char *agentNameOrPeerID, void *data, size_t size);
PUBLIC int igsAgent_busSendZMQMsgToAgent(igsAgent_t *agent, const char *agentNameOrPeerID, zmsg_t **msg_p); //destroys message after sending it
PUBLIC void igsAgent_busAddServiceDescription(igsAgent_t *agent, const char *key, const char *value);
PUBLIC void igsAgent_busRemoveServiceDescription(igsAgent_t *agent, const char *key);

PUBLIC int igsAgent_sendCall(igsAgent_t *agent, const char *agentNameOrUUID, const char *callName, igs_callArgument_t **list);
typedef void (*igsAgent_callFunction)(igsAgent_t *agent, const char *senderAgentName, const char *senderAgentUUID,
                                      const char *callName, igs_callArgument_t *firstArgument, size_t nbArgs,
                                      void* myData);
PUBLIC int igsAgent_initCall(igsAgent_t *agent, const char *name, igsAgent_callFunction cb, void *myData);
PUBLIC int igsAgent_removeCall(igsAgent_t *agent, const char *name);
PUBLIC int igsAgent_addArgumentToCall(igsAgent_t *agent, const char *callName, const char *argName, iopType_t type);
PUBLIC int igsAgent_removeArgumentFromCall(igsAgent_t *agent, const char *callName, const char *argName); //removes first occurence with this name
PUBLIC size_t igsAgent_getNumberOfCalls(igsAgent_t *agent);
PUBLIC bool igsAgent_checkCallExistence(igsAgent_t *agent, const char *name);
PUBLIC char** igsAgent_getCallsList(igsAgent_t *agent, size_t *nbOfElements); //returned char** shall be freed by caller
PUBLIC igs_callArgument_t* igsAgent_getFirstArgumentForCall(igsAgent_t *agent, const char *callName);
PUBLIC size_t igsAgent_getNumberOfArgumentsForCall(igsAgent_t *agent, const char *callName);
PUBLIC bool igsAgent_checkCallArgumentExistence(igsAgent_t *agent, const char *callName, const char *argName);


#ifdef __cplusplus
}
#endif
#endif /* ingescape_class_h */
