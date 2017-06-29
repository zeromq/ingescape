//
//  mastic_public.h
//
//  Created by Stephane Vales on 22/05/2017.
//  Modified by Mathieu Poirier
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

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

//start, stop & kill the agent
PUBLIC int mtic_startWithDevice(const char *networkDevice, int port);
PUBLIC int mtic_startWithIP(const char *ipAddress, int port);
PUBLIC int mtic_stop();
PUBLIC void mtic_die();
PUBLIC int mtic_setAgentName(const char *name);
PUBLIC char *mtic_getAgentName();

//pause and resume the agent
typedef void (*mtic_freezeCallback)(bool isPaused, void *myData);
PUBLIC int mtic_freeze();
PUBLIC bool mtic_isFrozen();
PUBLIC int mtic_unfreeze();
PUBLIC int mtic_observeFreeze(mtic_freezeCallback cb, void *myData);

//control agent state
PUBLIC int mtic_setAgentState(const char *state);
PUBLIC char *mtic_getAgentState(); //returned string shall be freed by caller

//mute the agent
PUBLIC int mtic_mute();
PUBLIC int mtic_unmute();
PUBLIC bool mtic_isMuted();

//set/get library parameters
PUBLIC void mtic_setVerbose (bool verbose);
PUBLIC bool mtic_getVerbose();
PUBLIC void mtic_setCanBeFrozen (bool canBeFrozen);

//////////////////////////////////////////////////
//IOP Model : Inputs, Outputs and Parameters read/write/check/observe/mute

/**
 * \var iop_t
 * \brief type of agent's inputs / outputs / parameters
 */
typedef enum {
    INPUT_T = 1,
    OUTPUT_T,
    PARAMETER_T
} iop_t;

/**
 * \var iopType_t
 * \brief type of the inputs / outputs / parameters
 */
typedef enum {
    INTEGER_T = 1,
    DOUBLE_T,
    STRING_T,
    BOOL_T,
    IMPULSION_T,
    DATA_T
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
PUBLIC int mtic_readInputAsData(const char *name, void *data, long *size); //allocs data structure to be disposed by caller

PUBLIC bool mtic_readOutputAsBool(const char *name);
PUBLIC int mtic_readOutputAsInt(const char *name);
PUBLIC double mtic_readOutputAsDouble(const char *name);
PUBLIC char* mtic_readOutputAsString(const char *name);
PUBLIC int mtic_readOutputAsData(const char *name, void *data, long *size); //allocs data structure to be disposed by caller

PUBLIC bool mtic_readParameterAsBool(const char *name);
PUBLIC int mtic_readParameterAsInt(const char *name);
PUBLIC double mtic_readParameterAsDouble(const char *name);
PUBLIC char* mtic_readParameterAsString(const char *name);
PUBLIC int mtic_readParameterAsData(const char *name, void *data, long *size); //allocs data structure to be disposed by caller

//write using void*
PUBLIC int mtic_writeInput(const char *name, void *value, long size);
PUBLIC int mtic_writeOutput(const char *name, void *value, long size);
PUBLIC int mtic_writeParameter(const char *name, void *value, long size);

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

PUBLIC int mtic_getInputsNumber();
PUBLIC int mtic_getOutputsNumber();
PUBLIC int mtic_getParametersNumber();

PUBLIC char** mtic_getInputsList(long *nbOfElements);
PUBLIC char** mtic_getOutputsList(long *nbOfElements);
PUBLIC char** mtic_getParametersList(long *nbOfElements);

PUBLIC bool mtic_checkInputExistence(const char *name);
PUBLIC bool mtic_checkOutputExistence(const char *name);
PUBLIC bool mtic_checkParameterExistence(const char *name);

//observe IOP
typedef void (*mtic_observeCallback)(iop_t iopType, const char *name, iopType_t valueType, void *value, void *myData);
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
int mtic_clearDefinition(); //clears definition data for the agent
PUBLIC char* mtic_getDefinition(); //returns json string
PUBLIC int mtic_setDefinitionDescription(char *description);
PUBLIC int mtic_setDefinitionVersion(char *version);

//edit the definition using the API
PUBLIC int mtic_createInput(const char *name, iopType_t type, void *value, long size); //value must be copied in function
PUBLIC int mtic_createOutput(const char *name, iopType_t type, void *value, long size); //value must be copied in function
PUBLIC int mtic_createParameter(const char *name, iopType_t type, void *value, long size); //value must be copied in function

PUBLIC int mtic_removeInput(const char *name);
PUBLIC int mtic_removeOutput(const char *name);
PUBLIC int mtic_removeParameter(const char *name);

//////////////////////////////////////////////////
//categories
//TODO later

//////////////////////////////////////////////////
//mapping

//load / set / get definition
PUBLIC int mtic_loadMapping (const char* json_str);
PUBLIC int mtic_loadMappingFromPath (const char* file_path);
PUBLIC int mtic_clearMapping(); //clears mapping data for the agent
PUBLIC char* mtic_getMapping(); //returns json string

//edit mapping using the API
PUBLIC int mtic_setMappingName(char *name);
PUBLIC int mtic_setMappingDescription(char *description);
PUBLIC int mtic_setMappingVersion(char *version);
PUBLIC int mtic_getMappingEntriesNumber(); //number of entries in the mapping output type
PUBLIC int mtic_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput); //returns mapping id or 0 if creation failed
PUBLIC int mtic_removeMappingEntryWithId(int theId);
PUBLIC int mtic_removeMappingEntryWithName(char *fromOurInput, char *toAgent, char *withOutput);

#endif /* mastic_public_h */
