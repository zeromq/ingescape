//
//  ingescape_public.h
//
//  Multi Agent Supervision Transport Integration and Control
//
//  Created by Stephane Vales on 22/05/2017.
//  Modified by Mathieu Poirier
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef ingescape_public_h
#define ingescape_public_h

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (defined WIN32 || defined _WIN32)
#if defined INGESCAPE
#define PUBLIC __declspec(dllexport)
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

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

//start & stop the agent
PUBLIC int igs_startWithDevice(const char *networkDevice, unsigned int port);
PUBLIC int igs_startWithIP(const char *ipAddress, unsigned int port);
PUBLIC int igs_stop(void);

//There are four non-exclusive ways to stop the execution of an ingescape agent:
//1- calling igs_stop from the hosting app's threads reacting on user actions or external events
//2- handling SIGINT in the hosting app to call igs_stop and stop the rest of the app properly
//3- monitoring the status of igs_Interrupted in the hosting app
//4- using an igs_forcedStopCallback (see below) and calling code ON THE MAIN APP THREAD from it
//In any case, igs_stop MUST NEVER BE CALLED directly from any ingeScape callback, as it would cause a thread deadlock.

PUBLIC extern bool igs_Interrupted; //true when the ingescape library triggered SIGINT when forced to stop
typedef void (*igs_forcedStopCallback)(void *myData);
//register a callback when the agent is forced to stop by the ingescape platform
PUBLIC void igs_observeForcedStop(igs_forcedStopCallback cb, void *myData);

//terminate the agent with trigger of SIGINT and call to the registered igs_forcedStopCallbacks
PUBLIC void igs_die(void);

//agent name set and get
PUBLIC int igs_setAgentName(const char *name);
PUBLIC char *igs_getAgentName(void); //char* must be freed by caller

//control agent state
PUBLIC int igs_setAgentState(const char *state);
PUBLIC char *igs_getAgentState(void); //char* must be freed by caller

//mute the agent ouputs
PUBLIC int igs_mute(void);
PUBLIC int igs_unmute(void);
PUBLIC bool igs_isMuted(void);
typedef void (*igs_muteCallback)(bool isMuted, void *myData);
PUBLIC int igs_observeMute(igs_muteCallback cb, void *myData);

//freeze and unfreeze the agent
//When freezed, agent will not send anything on its outputs and
//its inputs are not reactive to external data.
//NB: the internal semantics of freeze and unfreeze for a given agent
//are up to the developer and can be controlled using callbacks and igs_observeFreeze
PUBLIC int igs_freeze(void);
PUBLIC bool igs_isFrozen(void);
PUBLIC int igs_unfreeze(void);
typedef void (*igs_freezeCallback)(bool isPaused, void *myData);
PUBLIC int igs_observeFreeze(igs_freezeCallback cb, void *myData);
PUBLIC void igs_setCanBeFrozen(bool canBeFrozen);
PUBLIC bool igs_canBeFrozen(void);


//////////////////////////////////////////////////
//IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

typedef enum {
    IGS_INPUT_T = 1, ///< input of an agent.
    IGS_OUTPUT_T,    ///< output of an agent.
    IGS_PARAMETER_T  ///< parameter of an agent.
} iop_t;

typedef enum {
    IGS_INTEGER_T = 1,  ///< integer value type
    IGS_DOUBLE_T,       ///< double value type
    IGS_STRING_T,       ///< string value type
    IGS_BOOL_T,         ///< bool value type
    IGS_IMPULSION_T,    ///< impulsion value type
    IGS_DATA_T,         ///< data value type
    IGS_UNKNOWN_T       ///< for unknown parsed types
} iopType_t;

//read IOP using void*
PUBLIC int igs_readInput(const char *name, void **value, size_t *size);
PUBLIC int igs_readOutput(const char *name, void **value, size_t *size);
PUBLIC int igs_readParameter(const char *name, void **value, size_t *size);

//read per type
PUBLIC bool igs_readInputAsBool(const char *name);
PUBLIC int igs_readInputAsInt(const char *name);
PUBLIC double igs_readInputAsDouble(const char *name);
PUBLIC char* igs_readInputAsString(const char *name); //returned char* must be freed by caller
PUBLIC int igs_readInputAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

PUBLIC bool igs_readOutputAsBool(const char *name);
PUBLIC int igs_readOutputAsInt(const char *name);
PUBLIC double igs_readOutputAsDouble(const char *name);
PUBLIC char* igs_readOutputAsString(const char *name); //returned char* must be freed by caller
PUBLIC int igs_readOutputAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

PUBLIC bool igs_readParameterAsBool(const char *name);
PUBLIC int igs_readParameterAsInt(const char *name);
PUBLIC double igs_readParameterAsDouble(const char *name);
PUBLIC char* igs_readParameterAsString(const char *name); //returned char* must be freed by caller
PUBLIC int igs_readParameterAsData(const char *name, void **data, size_t *size); //returned data must be freed by caller

//write per type
PUBLIC int igs_writeInputAsBool(const char *name, bool value);
PUBLIC int igs_writeInputAsInt(const char *name, int value);
PUBLIC int igs_writeInputAsDouble(const char *name, double value);
PUBLIC int igs_writeInputAsString(const char *name, const char *value);
PUBLIC int igs_writeInputAsImpulsion(const char *name);
PUBLIC int igs_writeInputAsData(const char *name, void *value, size_t size);

PUBLIC int igs_writeOutputAsBool(const char *name, bool value);
PUBLIC int igs_writeOutputAsInt(const char *name, int value);
PUBLIC int igs_writeOutputAsDouble(const char *name, double value);
PUBLIC int igs_writeOutputAsString(const char *name, const char *value);
PUBLIC int igs_writeOutputAsImpulsion(const char *name);
PUBLIC int igs_writeOutputAsData(const char *name, void *value, size_t size);

PUBLIC int igs_writeParameterAsBool(const char *name, bool value);
PUBLIC int igs_writeParameterAsInt(const char *name, int value);
PUBLIC int igs_writeParameterAsDouble(const char *name, double value);
PUBLIC int igs_writeParameterAsString(const char *name, const char *value);
PUBLIC int igs_writeParameterAsData(const char *name, void *value, size_t size);

//clear IOP data in memory without having to write the IOP
//(relevant for IOPs with IGS_DATA_T type only)
PUBLIC void igs_clearDataForInput(const char *name);
PUBLIC void igs_clearDataForOutput(const char *name);
PUBLIC void igs_clearDataForParameter(const char *name);

//observe writing to an IOP
typedef void (*igs_observeCallback)(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData);
PUBLIC int igs_observeInput(const char *name, igs_observeCallback cb, void *myData);
PUBLIC int igs_observeOutput(const char *name, igs_observeCallback cb, void * myData);
PUBLIC int igs_observeParameter(const char *name, igs_observeCallback cb, void * myData);

//mute or unmute an IOP
PUBLIC int igs_muteOutput(const char *name);
PUBLIC int igs_unmuteOutput(const char *name);
PUBLIC bool igs_isOutputMuted(const char *name);

//check IOP type, lists and existence
PUBLIC iopType_t igs_getTypeForInput(const char *name);
PUBLIC iopType_t igs_getTypeForOutput(const char *name);
PUBLIC iopType_t igs_getTypeForParameter(const char *name);

PUBLIC int igs_getInputsNumber(void);
PUBLIC int igs_getOutputsNumber(void);
PUBLIC int igs_getParametersNumber(void);

PUBLIC char** igs_getInputsList(long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igs_getOutputsList(long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC char** igs_getParametersList(long *nbOfElements); //returned char** must be freed using igs_freeIOPList
PUBLIC void igs_freeIOPList(char ***list, long nbOfElements);

PUBLIC bool igs_checkInputExistence(const char *name);
PUBLIC bool igs_checkOutputExistence(const char *name);
PUBLIC bool igs_checkParameterExistence(const char *name);


//////////////////////////////////////////////////
//Definitions

//load / set / get definition
PUBLIC int igs_loadDefinition (const char* json_str);
PUBLIC int igs_loadDefinitionFromPath (const char* file_path);
PUBLIC int igs_clearDefinition(void); //clears definition data for the agent
PUBLIC char* igs_getDefinition(void); //returns json string, must be freed by caller
PUBLIC char *igs_getDefinitionName(void); //returned char* must be freed by caller
PUBLIC char *igs_getDefinitionDescription(void); //returned char* must be freed by caller
PUBLIC char *igs_getDefinitionVersion(void); //returned char* must be freed by caller
PUBLIC int igs_setDefinitionName(const char *name);
PUBLIC int igs_setDefinitionDescription(const char *description);
PUBLIC int igs_setDefinitionVersion(const char *version);

//edit the definition using the API
PUBLIC int igs_createInput(const char *name, iopType_t value_type, void *value, size_t size);
PUBLIC int igs_createOutput(const char *name, iopType_t type, void *value, size_t size);
PUBLIC int igs_createParameter(const char *name, iopType_t type, void *value, size_t size);

PUBLIC int igs_removeInput(const char *name);
PUBLIC int igs_removeOutput(const char *name);
PUBLIC int igs_removeParameter(const char *name);


//////////////////////////////////////////////////
//mapping

//load / set / get mapping
PUBLIC int igs_loadMapping (const char* json_str);
PUBLIC int igs_loadMappingFromPath (const char* file_path);
PUBLIC int igs_clearMapping(void); //clears all mapping for the agent
PUBLIC char* igs_getMapping(void); //returns json string, must be freed by caller
PUBLIC char *igs_getMappingName(void); //returned char* must be freed by caller
PUBLIC char *igs_getMappingDescription(void); //returned char* must be freed by caller
PUBLIC char *igs_getMappingVersion(void); //returned char* must be freed by caller

//edit mapping using the API
PUBLIC int igs_setMappingName(const char *name);
PUBLIC int igs_setMappingDescription(const char *description);
PUBLIC int igs_setMappingVersion(const char *version);
PUBLIC int igs_getMappingEntriesNumber(void); //number of entries in the mapping output type
PUBLIC unsigned long igs_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput); //returns mapping id or zero or below if creation failed
PUBLIC int igs_removeMappingEntryWithId(unsigned long theId);
PUBLIC int igs_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput);


//////////////////////////////////////////////////
//administration, configuration & utilities

//IngeScape library version
//returns MAJOR*10000 + MINOR*100 + MICRO
//displays MAJOR.MINOR.MICRO in console
PUBLIC int igs_version(void);


//Utility functions to find network adapters with broadcast capabilities
//to be used in igs_startWithDevice
PUBLIC void igs_getNetdevicesList(char ***devices, int *nb);
PUBLIC void igs_freeNetdevicesList(char **devices, int nb);


//Agent command line can be passed here for inclusion in the
//agent's headers and to be used by ingescapeLauncher. If not set,
//command line is initialized with exec path without any parameter.
PUBLIC void igs_setCommandLine(const char *line);
PUBLIC void igs_setCommandLineFromArgs(int argc, const char * argv[]); //first element is replaced by absolute exec path on UNIX


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
PUBLIC void igs_setVerbose(bool verbose); //enable logs in console (ERROR and FATAL are always displayed)
PUBLIC bool igs_isVerbose(void);
PUBLIC void igs_setUseColorVerbose(bool useColor); //use colors in console
PUBLIC bool igs_getUseColorVerbose(void);
PUBLIC void igs_setLogStream(bool useLogStream); //enable logs in socket stream
PUBLIC bool igs_getLogStream(void);
PUBLIC void igs_setLogInFile(bool useLogFile); //enable logs in file
PUBLIC bool igs_getLogInFile(void);
PUBLIC void igs_setLogPath(const char *path); //default directory is ~/ on UNIX systems and current PATH on Windows
PUBLIC char* igs_getLogPath(void); // must be freed by caller

PUBLIC void igs_setLogLevel (igs_logLevel_t level); //set log level in console, default is IGS_LOG_INFO
PUBLIC igs_logLevel_t igs_getLogLevel(void);
PUBLIC void igs_log(igs_logLevel_t, const char *function, const char *format, ...)  CHECK_PRINTF (3);
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

#if defined __unix__ || defined __APPLE__ || defined __linux__
//IPC is supported on UNIX systems only
//set/get IPC folder path for the agent (default is /tmp/ingesscape/)
PUBLIC void igs_setIpcFolderPath(char *path);
PUBLIC const char* igs_getIpcFolderPath(void);

//IPC is activated by default be can be deactivated here
PUBLIC void igs_setAllowIpc(bool allow);
PUBLIC bool igs_getAllowIpc(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* ingescape_public_h */
