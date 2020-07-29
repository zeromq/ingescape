//
//  ingescape.h
//  ingescape - https://ingescape.com
//
//  Created by Stephane Vales on 22/05/2017.
//  Copyright © 2017 Ingenuity i/o. All rights reserved.
//

#ifndef ingescape_public_h
#define ingescape_public_h

#include <stdbool.h>
#include <stddef.h>
#include <czmq.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (defined WIN32 || defined _WIN32)
#if defined INGESCAPE
#define PUBLIC __declspec(dllexport)
#elif defined INGESCAPE_FROM_PRI
#define PUBLIC
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

typedef enum{
    IGS_SUCCESS = 0,
    IGS_FAILURE
} igs_result_t;

// GCC and clang can validate format strings for functions that act like printf
// this is used to check the logging functions
#if defined (__GNUC__) && (__GNUC__ >= 2)
#   define CHECK_PRINTF(a)   __attribute__((format (printf, a, a + 1)))
#else
#   define CHECK_PRINTF(a)
#endif

//Macro to avoid "unused parameter" warnings
#define IGS_UNUSED(x) (void)x;

//////////////////////////////////////////////////
// Initialization and control

//start & stop ingescape
PUBLIC igs_result_t igs_startWithDevice(const char *networkDevice, unsigned int port);
PUBLIC igs_result_t igs_startWithIP(const char *ipAddress, unsigned int port);
PUBLIC void igs_stop(void);
PUBLIC bool igs_isStarted(void);

/*
 Ingescape can be stopped either from the applications itself or from the network.
 When ingescape is stopped from the network, the application can be notified and
 take actions such as also stopping, enter a specific mode, etc.
 
 To stop ingescape from its hosting application, just call igs_stop().
 
 To be notified that Ingescape has been stopped, one can by order of
 preference:
 - read the pipe socket to ingescape and expect a "LOOP_STOPPED" message
 - register a callabck with igs_observeExternalStop - WARNING: this callback
 will be executed from the ingescape thread with potential thread-safety issues
 depending on your application structure.
 - periodically check igs_isStarted()
 In any case, igs_stop MUST NEVER BE CALLED directly from any Ingescape callback,
 as it would cause a deadlock.
 */
PUBLIC zsock_t* igs_getPipeToIngescape(void); //socket to get stop event from ingescape in a thread-safe manner
typedef void (*igs_externalStopCallback)(void *myData);
PUBLIC void igs_observeExternalStop(igs_externalStopCallback cb, void *myData);

//agent name set and get
PUBLIC void igs_setAgentName(const char *name);
PUBLIC char *igs_getAgentName(void); //char* must be freed by caller

//control agent state
PUBLIC void igs_setAgentState(const char *state);
PUBLIC char* igs_getAgentState(void); //must be freed by caller

//mute the agent ouputs
PUBLIC void igs_mute(void);
PUBLIC void igs_unmute(void);
PUBLIC bool igs_isMuted(void);
typedef void (*igs_muteCallback)(bool isMuted, void *myData);
PUBLIC void igs_observeMute(igs_muteCallback cb, void *myData);

//freeze and unfreeze the agent
//When freezed, agent will not send anything on its outputs and
//its inputs are not reactive to external data.
//NB: the internal semantics of freeze and unfreeze for a given agent
//are up to the developer and can be controlled using callbacks and igs_observeFreeze
PUBLIC igs_result_t igs_freeze(void);
PUBLIC bool igs_isFrozen(void);
PUBLIC void igs_unfreeze(void);
typedef void (*igs_freezeCallback)(bool isPaused, void *myData);
PUBLIC void igs_observeFreeze(igs_freezeCallback cb, void *myData);


//////////////////////////////////////////////////
//IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

typedef enum {
    IGS_INPUT_T = 1,
    IGS_OUTPUT_T,
    IGS_PARAMETER_T
} iop_t;

typedef enum {
    IGS_INTEGER_T = 1,
    IGS_DOUBLE_T,
    IGS_STRING_T,
    IGS_BOOL_T,
    IGS_IMPULSION_T,
    IGS_DATA_T,
    IGS_UNKNOWN_T
} iopType_t;

//read per type
PUBLIC bool igs_readInputAsBool(const char *name);
PUBLIC int igs_readInputAsInt(const char *name);
PUBLIC double igs_readInputAsDouble(const char *name);
PUBLIC char* igs_readInputAsString(const char *name); //returned char* must be freed by caller
PUBLIC igs_result_t igs_readInputAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

PUBLIC bool igs_readOutputAsBool(const char *name);
PUBLIC int igs_readOutputAsInt(const char *name);
PUBLIC double igs_readOutputAsDouble(const char *name);
PUBLIC char* igs_readOutputAsString(const char *name); //returned char* must be freed by caller
PUBLIC igs_result_t igs_readOutputAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

PUBLIC bool igs_readParameterAsBool(const char *name);
PUBLIC int igs_readParameterAsInt(const char *name);
PUBLIC double igs_readParameterAsDouble(const char *name);
PUBLIC char* igs_readParameterAsString(const char *name); //returned char* must be freed by caller
PUBLIC igs_result_t igs_readParameterAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

//write per type
PUBLIC igs_result_t igs_writeInputAsBool(const char *name, bool value);
PUBLIC igs_result_t igs_writeInputAsInt(const char *name, int value);
PUBLIC igs_result_t igs_writeInputAsDouble(const char *name, double value);
PUBLIC igs_result_t igs_writeInputAsString(const char *name, const char *value);
PUBLIC igs_result_t igs_writeInputAsImpulsion(const char *name);
PUBLIC igs_result_t igs_writeInputAsData(const char *name, void *value, size_t size);

PUBLIC igs_result_t igs_writeOutputAsBool(const char *name, bool value);
PUBLIC igs_result_t igs_writeOutputAsInt(const char *name, int value);
PUBLIC igs_result_t igs_writeOutputAsDouble(const char *name, double value);
PUBLIC igs_result_t igs_writeOutputAsString(const char *name, const char *value);
PUBLIC igs_result_t igs_writeOutputAsImpulsion(const char *name);
PUBLIC igs_result_t igs_writeOutputAsData(const char *name, void *value, size_t size);

PUBLIC igs_result_t igs_writeParameterAsBool(const char *name, bool value);
PUBLIC igs_result_t igs_writeParameterAsInt(const char *name, int value);
PUBLIC igs_result_t igs_writeParameterAsDouble(const char *name, double value);
PUBLIC igs_result_t igs_writeParameterAsString(const char *name, const char *value);
PUBLIC igs_result_t igs_writeParameterAsData(const char *name, void *value, size_t size);

//read IOP using memory space - use only when per-type read functions cannot.
// value : pointer to actual value, initialized by function, to be freed by user
// size : size of returned value
PUBLIC igs_result_t igs_readInput(const char *name, void **value, size_t *size);
PUBLIC igs_result_t igs_readOutput(const char *name, void **value, size_t *size);
PUBLIC igs_result_t igs_readParameter(const char *name, void **value, size_t *size);


//clear IOP data in memory without having to write the IOP
//(relevant for IOPs with IGS_DATA_T type only)
PUBLIC void igs_clearDataForInput(const char *name);
PUBLIC void igs_clearDataForOutput(const char *name);
PUBLIC void igs_clearDataForParameter(const char *name);

//observe writing to an IOP
typedef void (*igs_observeCallback)(iop_t iopType, const char *name, iopType_t valueType, void *value, size_t valueSize, void *myData);
PUBLIC void igs_observeInput(const char *name, igs_observeCallback cb, void *myData);
PUBLIC void igs_observeOutput(const char *name, igs_observeCallback cb, void * myData);
PUBLIC void igs_observeParameter(const char *name, igs_observeCallback cb, void * myData);

//mute or unmute an IOP
PUBLIC void igs_muteOutput(const char *name);
PUBLIC void igs_unmuteOutput(const char *name);
PUBLIC bool igs_isOutputMuted(const char *name);

//check IOP type, lists and existence
PUBLIC iopType_t igs_getTypeForInput(const char *name);
PUBLIC iopType_t igs_getTypeForOutput(const char *name);
PUBLIC iopType_t igs_getTypeForParameter(const char *name);

PUBLIC size_t igs_getInputsNumber(void);
PUBLIC size_t igs_getOutputsNumber(void);
PUBLIC size_t igs_getParametersNumber(void);

PUBLIC char** igs_getInputsList(size_t *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igs_getOutputsList(size_t *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igs_getParametersList(size_t *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC void igs_freeIOPList(char ***list, size_t nbOfElements);

PUBLIC bool igs_checkInputExistence(const char *name);
PUBLIC bool igs_checkOutputExistence(const char *name);
PUBLIC bool igs_checkParameterExistence(const char *name);


//////////////////////////////////////////////////
//Definitions

//load / set / get definition
PUBLIC igs_result_t igs_loadDefinition (const char* json_str);
PUBLIC igs_result_t igs_loadDefinitionFromPath (const char* file_path);
PUBLIC void igs_clearDefinition(void); //clears definition data for the agent
PUBLIC char* igs_getDefinition(void); //returns json string, must be freed by caller
PUBLIC char *igs_getDefinitionName(void); //returned char* must be freed by caller
PUBLIC char *igs_getDefinitionDescription(void); //returned char* must be freed by caller
PUBLIC char *igs_getDefinitionVersion(void); //returned char* must be freed by caller
PUBLIC void igs_setDefinitionName(const char *name);
PUBLIC void igs_setDefinitionDescription(const char *description);
PUBLIC void igs_setDefinitionVersion(const char *version);

//edit the definition using the API
PUBLIC igs_result_t igs_createInput(const char *name, iopType_t value_type, void *value, size_t size);
PUBLIC igs_result_t igs_createOutput(const char *name, iopType_t type, void *value, size_t size);
PUBLIC igs_result_t igs_createParameter(const char *name, iopType_t type, void *value, size_t size);

PUBLIC igs_result_t igs_removeInput(const char *name);
PUBLIC igs_result_t igs_removeOutput(const char *name);
PUBLIC igs_result_t igs_removeParameter(const char *name);


//////////////////////////////////////////////////
//Mappings

//load / set / get mapping
PUBLIC igs_result_t igs_loadMapping (const char* json_str);
PUBLIC igs_result_t igs_loadMappingFromPath (const char* file_path);
PUBLIC void igs_clearMapping(void); //clears all mapping for the agent
PUBLIC char* igs_getMapping(void); //returns json string, must be freed by caller
PUBLIC char *igs_getMappingName(void); //returned char* must be freed by caller
PUBLIC char *igs_getMappingDescription(void); //returned char* must be freed by caller
PUBLIC char *igs_getMappingVersion(void); //returned char* must be freed by caller
PUBLIC void igs_setMappingName(const char *name);
PUBLIC void igs_setMappingDescription(const char *description);
PUBLIC void igs_setMappingVersion(const char *version);

//edit mapping using the API
PUBLIC size_t igs_getMappingEntriesNumber(void); //number of entries in the mapping output type
PUBLIC unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput); //returns mapping id or zero if creation failed
PUBLIC igs_result_t igs_removeMappingEntryWithId(unsigned long theId);
PUBLIC igs_result_t igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput);


//////////////////////////////////////////////////
//Administration, configuration & utilities

//IngeScape library version
//returns MAJOR*10000 + MINOR*100 + MICRO
PUBLIC int igs_version(void);

//IngeScape protocol version
PUBLIC int igs_protocol(void);

//Utility functions to find network adapters with broadcast capabilities
//to be used in igs_startWithDevice and igs_startWithIP
PUBLIC void igs_getNetdevicesList(char ***devices, int *nb);
PUBLIC void igs_freeNetdevicesList(char **devices, int nb);
PUBLIC void igs_getNetaddressesList(char ***addresses, int *nb);
PUBLIC void igs_freeNetaddressesList(char **addresses, int nb);


//Agent command line can be passed here to be used by ingescapeLauncher. If not set,
//command line is initialized with exec path without any parameter.
PUBLIC void igs_setCommandLine(const char *line);
PUBLIC void igs_setCommandLineFromArgs(int argc, const char * argv[]); //first element is replaced by absolute exec path on UNIX
PUBLIC char* igs_getCommandLine(void); //must be freed by caller


//When mapping other agents, it is possible to request the
//mapped agents to send us their current output values
//through a private communication for our proper initialization.
//By default, this behavior is disabled.
PUBLIC void igs_setRequestOutputsFromMappedAgents(bool notify);
PUBLIC bool igs_getRequestOutputsFromMappedAgents(void);


/* Logs policy
 - fatal : Events that force application termination.
 - error : Events that are fatal to the current operation but not the whole application.
 - warning : Events that can potentially cause application anomalies but that can be recovered automatically (by circumventing or retrying).
 - info : Generally useful information to log (service start/stop, configuration assumptions, etc.).
 - debug : Information that is diagnostically helpful to people more than just developers but useless for system monitoring.
 - trace : Information about parts of functions, for detailed diagnostic only.
 */
typedef enum {
    IGS_LOG_TRACE = 0,
    IGS_LOG_DEBUG,
    IGS_LOG_INFO,
    IGS_LOG_WARN,
    IGS_LOG_ERROR,
    IGS_LOG_FATAL
} igs_logLevel_t;

//logs management
PUBLIC void igs_setVerbose(bool); //enable logs in console (ERROR and FATAL are always displayed)
PUBLIC bool igs_isVerbose(void);
PUBLIC void igs_setUseColorVerbose(bool); //use colors in console
PUBLIC bool igs_getUseColorVerbose(void);
PUBLIC void igs_setLogStream(bool); //enable logs in socket stream
PUBLIC bool igs_getLogStream(void);
PUBLIC void igs_setLogInFile(bool); //enable logs in file
PUBLIC bool igs_getLogInFile(void);
PUBLIC void igs_setLogPath(const char *path); //default directory is ~/ on UNIX systems and current PATH on Windows
PUBLIC char* igs_getLogPath(void); // must be freed by caller
PUBLIC void igs_setLogLevel (igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
PUBLIC igs_logLevel_t igs_getLogLevel(void);

//do not use these functions, use aliases just below
PUBLIC void igs_log(igs_logLevel_t level, const char *function, const char *format, ...) CHECK_PRINTF (3);
#define igs_trace(...) igs_log(IGS_LOG_TRACE, __func__, __VA_ARGS__)
#define igs_debug(...) igs_log(IGS_LOG_DEBUG, __func__, __VA_ARGS__)
#define igs_info(...)  igs_log(IGS_LOG_INFO, __func__, __VA_ARGS__)
#define igs_warn(...)  igs_log(IGS_LOG_WARN, __func__, __VA_ARGS__)
#define igs_error(...) igs_log(IGS_LOG_ERROR, __func__, __VA_ARGS__)
#define igs_fatal(...) igs_log(IGS_LOG_FATAL, __func__, __VA_ARGS__)


//resources file management
PUBLIC void igs_setDefinitionPath(const char *path);
PUBLIC void igs_setMappingPath(const char *path);
PUBLIC void igs_writeDefinitionToPath(void);
PUBLIC void igs_writeMappingToPath(void);

//Ingescape automatically detects agents on the same computer and same process (PID)
//Then, it uses optimized communication for input/output data exchange chosen
//between TCP, IPC/loopback and inproc.

//Same IP address but differet PIDs : use IPC or loopback
//IPC is supported on UNIX systems only. On windows, we use the loopback as an alternative.
//IPC is activated by default be can be deactivated here.
PUBLIC void igs_setAllowIpc(bool allow);
PUBLIC bool igs_getAllowIpc(void);
#if defined __unix__ || defined __APPLE__ || defined __linux__
//set IPC folder path for the agent on UNIX systems (default is /tmp/)
PUBLIC void igs_setIpcFolderPath(char *path);
PUBLIC const char* igs_getIpcFolderPath(void);
#endif

//Same IP address and same PID : use inproc
//Inproc is activated by default be can be deactivated here.
PUBLIC void igs_setAllowInproc(bool allow);
PUBLIC bool igs_getAllowInproc(void);


//////////////////////////////////////////////////
//licenses
#define igs_license(...) igs_log(IGS_LOG_FATAL + 1, __func__, __VA_ARGS__)
typedef enum {
    IGS_LICENSE_TIMEOUT = 0,
    IGS_LICENSE_TOO_MANY_AGENTS,
    IGS_LICENSE_TOO_MANY_IOPS
} igs_license_limit_t;
typedef void (igs_licenseCallback)(igs_license_limit_t limit, void *myData);

#if !defined(TARGET_OS_IOS) || !TARGET_OS_IOS
// Default licenses path is empty and, if so, is automatically set at runtime to agent's executable path.
// All licenses in path will be examined and used if valid
// When path is set manually, it takes priority over agent's executable path.
PUBLIC void igs_setLicensePath(const char *path);
PUBLIC char* igs_getLicensePath(void); //must be freed by caller
    
//Any agent developer can use this function to check the license against her/his agent's unique id.
//IDs are provided by the ingescape team.
//Returns true if check is OK.
PUBLIC bool igs_checkLicenseForAgent(const char *agentId);

//use this callback mechanism to be notified when the timer
//or number of agents or number of IOPs has been exceeded in demo mode
PUBLIC void igs_observeLicense(igs_licenseCallback cb, void *myData);
    
//This function loads a license in memory and overrides all provided
//license files.
PUBLIC void igs_loadLicenseData(const void *data, size_t size);

#endif
    
#ifdef __cplusplus
}
#endif

#endif /* ingescape_public_h */
