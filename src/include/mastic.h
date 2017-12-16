//
//  mastic_public.h
//
//  Created by Stephane Vales on 22/05/2017.
//  Modified by Mathieu Poirier
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

/**
  * \file ../../src/include/mastic.h
  */

#ifndef mastic_public_h
#define mastic_public_h

#include <stdbool.h>

#if defined WINDOWS
#if defined MASTIC
#define PUBLIC __declspec(dllexport)
#else
#define PUBLIC __declspec(dllimport)
#endif
#else
#define PUBLIC
#endif

//////////////////////////////////////////////////
//initialization and control

//start & stop the agent
PUBLIC int mtic_startWithDevice(const char *networkDevice, int port);
PUBLIC int mtic_startWithIP(const char *ipAddress, int port);
PUBLIC int mtic_stop(void);

//agent name set and get
PUBLIC int mtic_setAgentName(const char *name);
PUBLIC char *mtic_getAgentName(void);

//control agent state
PUBLIC int mtic_setAgentState(const char *state);
PUBLIC char *mtic_getAgentState(void);

//mute the agent ouputs
PUBLIC int mtic_mute(void);
PUBLIC int mtic_unmute(void);
PUBLIC bool mtic_isMuted(void);

//freeze and unfreeze the agent
//When freezed, agent will not send anything on its outputs and
//its inputs are not reactive to external data.
//NB: the internal semantics of freeze and unfreeze for a given agenent
//are up to the developer and can be controlled using callbacks and mtic_observeFreeze
PUBLIC int mtic_freeze(void);
PUBLIC bool mtic_isFrozen(void);
PUBLIC int mtic_unfreeze(void);
typedef void (*mtic_freezeCallback)(bool isPaused, void *myData);
PUBLIC int mtic_observeFreeze(mtic_freezeCallback cb, void *myData);
PUBLIC void mtic_setCanBeFrozen(bool canBeFrozen);

//There are four non-exclusive ways to check & control the execution of the mastic
//instance and its hosting application:
//1- using mtic_start* and mtic_stop from the hosting app
//2- monitoring the status of mtic_Interrupted in the hosting app
//3- using mtic_observeForcedStop below and providing a callback using parent thread
//4- setting mtic_Interrupted from Mastic callbacks and arranging to call mtic_stop from main thread when mtic_Interrupted is set to true

PUBLIC extern bool mtic_Interrupted;
//register a callback when the agent is asked to stop on the network
//NB: callbacks should execute their code in the main hosting application thread
typedef void (*mtic_forcedStopCallback)(void *myData);
PUBLIC void mtic_observeForcedStop(mtic_forcedStopCallback cb, void *myData);

//terminate the agent and trigger the forcedStopCallbacks
PUBLIC void mtic_die(void);

//////////////////////////////////////////////////
//IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

typedef enum {
    INPUT_T = 1, ///< input of an agent.
    OUTPUT_T,    ///< output of an agent.
    PARAMETER_T  ///< parameter of an agent.
} iop_t;

typedef enum {
    INTEGER_T = 1,  ///< integer value type
    DOUBLE_T,       ///< double value type
    STRING_T,       ///< string value type
    BOOL_T,         ///< bool value type
    IMPULSION_T,    ///< impulsion value type
    DATA_T          ///< data value type
} iopType_t;

//read/write IOP using void*
PUBLIC int mtic_readInput(const char *name, void **value, long *size);
PUBLIC int mtic_readOutput(const char *name, void **value, long *size);
PUBLIC int mtic_readParameter(const char *name, void **value, long *size);

//read per type
PUBLIC bool mtic_readInputAsBool(const char *name);
PUBLIC int mtic_readInputAsInt(const char *name);
PUBLIC double mtic_readInputAsDouble(const char *name);
PUBLIC char* mtic_readInputAsString(const char *name);
PUBLIC int mtic_readInputAsData(const char *name, void **data, long *size);

PUBLIC bool mtic_readOutputAsBool(const char *name);
PUBLIC int mtic_readOutputAsInt(const char *name);
PUBLIC double mtic_readOutputAsDouble(const char *name);
PUBLIC char* mtic_readOutputAsString(const char *name);
PUBLIC int mtic_readOutputAsData(const char *name, void **data, long *size);

PUBLIC bool mtic_readParameterAsBool(const char *name);
PUBLIC int mtic_readParameterAsInt(const char *name);
PUBLIC double mtic_readParameterAsDouble(const char *name);
PUBLIC char* mtic_readParameterAsString(const char *name);
PUBLIC int mtic_readParameterAsData(const char *name, void **data, long *size);

//write using values in a string format
PUBLIC int mtic_writeInput(const char *name, char *value, long size);
PUBLIC int mtic_writeOutput(const char *name, char *value, long size);
PUBLIC int mtic_writeParameter(const char *name, char *value, long size);

//write per type
PUBLIC int mtic_writeInputAsBool(const char *name, bool value);
PUBLIC int mtic_writeInputAsInt(const char *name, int value);
PUBLIC int mtic_writeInputAsDouble(const char *name, double value);
PUBLIC int mtic_writeInputAsString(const char *name, char *value);
PUBLIC int mtic_writeInputAsImpulsion(const char *name);
PUBLIC int mtic_writeInputAsData(const char *name, void *value, long size);

PUBLIC int mtic_writeOutputAsBool(const char *name, bool value);
PUBLIC int mtic_writeOutputAsInt(const char *name, int value);
PUBLIC int mtic_writeOutputAsDouble(const char *name, double value);
PUBLIC int mtic_writeOutputAsString(const char *name, char *value);
PUBLIC int mtic_writeOutputAsImpulsion(const char *name);
PUBLIC int mtic_writeOutputAsData(const char *name, void *value, long size);

PUBLIC int mtic_writeParameterAsBool(const char *name, bool value);
PUBLIC int mtic_writeParameterAsInt(const char *name, int value);
PUBLIC int mtic_writeParameterAsDouble(const char *name, double value);
PUBLIC int mtic_writeParameterAsString(const char *name, char *value);
PUBLIC int mtic_writeParameterAsData(const char *name, void *value, long size);

//check IOP type, lists and existence
PUBLIC iopType_t mtic_getTypeForInput(const char *name);
PUBLIC iopType_t mtic_getTypeForOutput(const char *name);
PUBLIC iopType_t mtic_getTypeForParameter(const char *name);

PUBLIC int mtic_getInputsNumber(void);
PUBLIC int mtic_getOutputsNumber(void);
PUBLIC int mtic_getParametersNumber(void);

PUBLIC char** mtic_getInputsList(long *nbOfElements);
PUBLIC char** mtic_getOutputsList(long *nbOfElements);
PUBLIC char** mtic_getParametersList(long *nbOfElements);

PUBLIC bool mtic_checkInputExistence(const char *name);
PUBLIC bool mtic_checkOutputExistence(const char *name);
PUBLIC bool mtic_checkParameterExistence(const char *name);

//observe IOP
typedef void (*mtic_observeCallback)(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData);
PUBLIC int mtic_observeInput(const char *name, mtic_observeCallback cb, void *myData);
PUBLIC int mtic_observeOutput(const char *name, mtic_observeCallback cb, void * myData);
PUBLIC int mtic_observeParameter(const char *name, mtic_observeCallback cb, void * myData);

//mute or unmute an IOP
PUBLIC int mtic_muteOutput(const char *name);
PUBLIC int mtic_unmuteOutput(const char *name);
PUBLIC bool mtic_isOutputMuted(const char *name);

//////////////////////////////////////////////////
//Definitions

//load / set / get definition
PUBLIC int mtic_loadDefinition (const char* json_str);
PUBLIC int mtic_loadDefinitionFromPath (const char* file_path);
PUBLIC int mtic_clearDefinition(void); //clears definition data for the agent
PUBLIC char* mtic_getDefinition(void); //returns json string
PUBLIC char *mtic_getDefinitionName(void);
PUBLIC char *mtic_getDefinitionDescription(void);
PUBLIC char *mtic_getDefinitionVersion(void);
PUBLIC int mtic_setDefinitionName(char *name);
PUBLIC int mtic_setDefinitionDescription(char *description);
PUBLIC int mtic_setDefinitionVersion(char *version);

//edit the definition using the API
PUBLIC int mtic_createInput(const char *name, iopType_t value_type, void *value, long size); //value must be copied in function
PUBLIC int mtic_createOutput(const char *name, iopType_t type, void *value, long size); //value must be copied in function
PUBLIC int mtic_createParameter(const char *name, iopType_t type, void *value, long size); //value must be copied in function

PUBLIC int mtic_removeInput(const char *name);
PUBLIC int mtic_removeOutput(const char *name);
PUBLIC int mtic_removeParameter(const char *name);

//////////////////////////////////////////////////
//mapping

//load / set / get mapping
PUBLIC int mtic_loadMapping (const char* json_str);
PUBLIC int mtic_loadMappingFromPath (const char* file_path);
PUBLIC int mtic_clearMapping(void); //clears mapping data for the agent
PUBLIC char* mtic_getMapping(void); //returns json string
PUBLIC char *mtic_getMappingName(void);
PUBLIC char *mtic_getMappingDescription(void);
PUBLIC char *mtic_getMappingVersion(void);

//edit mapping using the API
PUBLIC int mtic_setMappingName(char *name);
PUBLIC int mtic_setMappingDescription(char *description);
PUBLIC int mtic_setMappingVersion(char *version);
PUBLIC int mtic_getMappingEntriesNumber(void); //number of entries in the mapping output type
PUBLIC unsigned long mtic_addMappingEntry(const char *fromOurInput, const char *toAgent, const char *withOutput); //returns mapping id or zero or below if creation failed
PUBLIC int mtic_removeMappingEntryWithId(unsigned long theId);
PUBLIC int mtic_removeMappingEntryWithName(const char *fromOurInput, const char *toAgent, const char *withOutput);


//////////////////////////////////////////////////
//administration, configuration & utilities

//utility function to find network adapters with broadcast capabilities
//to be used in mtic_startWithDevice
void mtic_getNetdevicesList(char ***devices, int *nb);
void mtic_freeNetdevicesList(char **devices, int nb);

//Command line for the agent can be passed here for inclusion in the
//agent's headers. If not set, header is initialized with exec path.
PUBLIC void mtic_setCommandLine(const char *line);

//By default, an agent notifies all the agent it maps. Each notification
//makes the mapped agents publish their outputs (except for data & impulsions).
//We allow to disable this notification to avoid side effects by agents frequently
//changing their mapping.
PUBLIC void mtic_setNotifyMappedAgents(bool notify);

//logs and debug messages
PUBLIC void mtic_setVerbose(bool verbose); //log in console
PUBLIC void mtic_setLogInFile(bool useLogFile); //log in file
PUBLIC void mtic_setUseColorVerbose (bool useColor); //use color in console
void mtic_setLogPath(const char *path);
typedef enum {
    MTIC_LOG_TRACE = 0,
    MTIC_LOG_DEBUG,
    MTIC_LOG_INFO,
    MTIC_LOG_WARN,
    MTIC_LOG_ERROR,
    MTIC_LOG_FATAL
} mtic_logLevel_t;
void mtic_setLogLevel (mtic_logLevel_t level);

//void mtic_debug(const char*fmt, ...);
void mtic_log(mtic_logLevel_t, const char*fmt, ...);
#define mtic_trace(...) mtic_log(MTIC_LOG_TRACE, __VA_ARGS__)
#define mtic_debug(...) mtic_log(MTIC_LOG_DEBUG, __VA_ARGS__)
#define mtic_info(...)  mtic_log(MTIC_LOG_INFO,  __VA_ARGS__)
#define mtic_warn(...)  mtic_log(MTIC_LOG_WARN,  __VA_ARGS__)
#define mtic_error(...) mtic_log(MTIC_LOG_ERROR, __VA_ARGS__)
#define mtic_fatal(...) mtic_log(MTIC_LOG_FATAL, __VA_ARGS__)

//resources file management
void mtic_setDefinitionPath(const char *path);
void mtic_setMappingPath(const char *path);
void mtic_writeDefinitionToPath(void);
void mtic_writeMappingToPath(void);


#endif /* mastic_public_h */
