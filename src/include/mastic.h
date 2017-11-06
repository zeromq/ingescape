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
//initialization and configuration

//This flag can be used to check if the Mastic internal thread
//has been interrupted or not. The flag is updated when using the
//mtic_start* (set to true) and mtic_stop (set to false) functions,
//and also when a DIE command is received by the agent (set to false).
//There three non-exclusive ways to check & control the execution of the mastic
//instance and its hosting application:
//1- using mtic_start* and mtic_stop from the hosting app
//2- catching SIGINT in the hosting app that is triggered by a DIE command in the agent
//3- monitoring the status of mtic_Interrupted in the hosting app
PUBLIC extern bool mtic_Interrupted;

//start, stop & kill the agent
PUBLIC int mtic_startWithDevice(const char *networkDevice, int port);
PUBLIC int mtic_startWithIP(const char *ipAddress, int port);
PUBLIC int mtic_stop(void);
PUBLIC void mtic_die(void);
PUBLIC int mtic_setAgentName(const char *name);
PUBLIC char *mtic_getAgentName(void);

//pause and resume the agent
/**
 * \var typedef void (*mtic_freezeCallback)(bool isPaused, void *myData)
 * \ingroup pauseResumeFct
 * \brief typedef for the callback used in freezed functions
 */
typedef void (*mtic_freezeCallback)(bool isPaused, void *myData);
PUBLIC int mtic_freeze(void);
PUBLIC bool mtic_isFrozen(void);
PUBLIC int mtic_unfreeze(void);
PUBLIC int mtic_observeFreeze(mtic_freezeCallback cb, void *myData);

//control agent state
PUBLIC int mtic_setAgentState(const char *state);
PUBLIC char *mtic_getAgentState(void);

//mute the agent
PUBLIC int mtic_mute(void);
PUBLIC int mtic_unmute(void);
PUBLIC bool mtic_isMuted(void);

//set/get library parameters
PUBLIC void mtic_setVerbose (bool verbose);
PUBLIC bool mtic_getVerbose(void);
PUBLIC void mtic_setCanBeFrozen (bool canBeFrozen);

//////////////////////////////////////////////////
//IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

/**
 * \enum iop_t
 * \brief type of agent's inputs / outputs / parameters
 */
typedef enum {
    INPUT_T = 1, ///< input of an agent.
    OUTPUT_T,    ///< output of an agent.
    PARAMETER_T  ///< parameter of an agent.
} iop_t;

/**
 * \enum iopType_t
 * \brief type of the value of the inputs / outputs / parameters
 */
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
/**
 * \var typedef void (*mtic_observeCallback)(iop_t iopType, const char *name, iopType_t valueType, void *value, void * myData)
 * \ingroup observefct
 * \brief typedef for the callback used in observe functions
 */
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
//categories
//TODO: later

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

#endif /* mastic_public_h */
