//
//  mastic_public.h
//
//  Created by Stephane Vales on 22/05/2017.
//  Copyright © 2016 IKKY WP4.8. All rights reserved.
//

#ifndef mastic_public_h
#define mastic_public_h

//return codes policy for functions (using int type)
// 1 : everything OK
// 0 and below : an error occured

typedef enum {
    INPUT,
    OUTPUT,
    PARAMETER
} iop_t;

typedef enum {
    INTEGER_T,
    DOUBLE_T,
    STRING_T,
    BOOL_T,
    IMPULSION_T,
    STRUCTURE_T
} iopType_t;


//////////////////////////////////////////////////
//initialization and configuration

//start, stop & kill the agent
int mtic_start(const char *agentName, const char *networkDevice, int port); //channel removed to keep it private
int mtic_stop();
int mtic_die();

//pause and resume the agent
int mtic_pause();
bool mtic_isPaused();
void mtic_resume();

//control agent state
int mtic_setAgentState(const char *state);
void mtic_getAgentState(char *);

//mute the agent
int mtic_mute();
int mtic_unmute();

//set library parameters
void mtic_setVerbose (bool verbose);




//////////////////////////////////////////////////
//IOP : inputs, outputs and parameters read/write/check/observe

//read/write IOP using void*
//generic typeless functions (require to check IOP type for type casting)
//for IMPULSION_T value is always 0
void *mtic_readInput(const char *name);
void *mtic_readOutput(const char *name);
void *mtic_readParameter(const char *name);

//read per type
//implicit conversions are possible. Some need to raise warnings.
//we need to make things clear on structures
//for IMPULSION_T value is always 0
bool mtic_readInputAsBool(const char *name);
int mtic_readInputAsInt(const char *name);
double mtic_readInputAsDouble(const char *name);
char* mtic_readInputAsString(const char *name);

bool mtic_readOutputAsBool(const char *name);
int mtic_readOutputAsInt(const char *name);
double mtic_readOutputAsDouble(const char *name);
char* mtic_readOutputAsString(const char *name);

bool mtic_readParameterAsBool(const char *name);
int mtic_readParameterAsInt(const char *name);
double mtic_readParameterAsDouble(const char *name);
char* mtic_readParameterAsString(const char *name);

//write using void*
//for IMPULSION_T value is just ignored
int mtic_writeInput(const char *name, void *value);
int mtic_writeOutput(const char *name, void *value);
int mtic_writeParameter(const char *name, void *value);

//write using internal conversions (Mastic does the conversion job)
//we need to make things clear on structures
//for IMPULSION_T value is just ignored
int mtic_writeInputAsBool(const char *name, bool value);
int mtic_writeInputAsInt(const char *name, int value);
int mtic_writeInputAsDouble(const char *name, double value);
int mtic_writeInputAsString(const char *name, char *value);

int mtic_writeOutputAsBool(const char *name, bool value);
int mtic_writeOutputAsInt(const char *name, int value);
int mtic_writeOutputAsDouble(const char *name, double value);
int mtic_writeOutputAsString(const char *name, char *value);

int mtic_writeParameterAsBool(const char *name, bool value);
int mtic_writeParameterAsInt(const char *name, int value);
int mtic_writeParameterAsDouble(const char *name, double value);
int mtic_writeParameterAsString(const char *name, char *value);


//check IOP type and existence
iopType_t mtic_getTypeForInput(const char *name);
iopType_t mtic_getTypeForOutput(const char *name);
iopType_t mtic_getTypeForParameter(const char *name);

bool mtic_checkInputExistence(const char *name);
bool mtic_checkOutputExistence(const char *name);
bool mtic_checkParameterExistence(const char *name);

//observe IOP
//calback format for IOP observation
typedef void* mtic_observeCallback(iop_t iop, const char *name, iopType_t valueType, void *value, void *data);
int mtic_observeInput(const char *name, mtic_observeCallback);
int mtic_observeOutput(const char *name, mtic_observeCallback);
int mtic_observeParameter(const char *name, mtic_observeCallback);


//mute or unmute an IOP
mtic_muteOutput(const char *name);
mtic_unmuteOutput(const char *name);




//////////////////////////////////////////////////
//definition and categories

int mtic_loadDefinition (const char* json_str);
int mtic_loadDefinitionFromPath (const char* file_path);
int mtic_clearDefinition(); //clears definition data for the agent
char* mtic_getDefinition(); //returns json string
int mtic_setDefinitionDescription(char *description);
int mtic_setDefinitionVersion(char *description);

//edit the definition using the API
int mtic_createInput(const char *name, iopType_t type, void *value); //value must be copied in function
int mtic_createOutput(const char *name, iopType_t type, void *value); //value must be copied in function
int mtic_createParameter(const char *name, iopType_t type, void *value); //value must be copied in function

int mtic_removeInput(const char *name);
int mtic_removeOutput(const char *name);
int mtic_removeParameter(const char *name);

//categories
//TODO



//////////////////////////////////////////////////
//mapping

int mtic_loadMapping (const char* json_str);
int mtic_loadMappingFromPath (const char* file_path);
int mtic_clearMapping(); //clears mapping data for the agent
char* mtic_getMapping(); //returns json string
int mtic_setMappingDescription(char *description);
int mtic_setMappingVersion(char *description);
int mtic_getMappingEntriesNumber(); //number of entries in the mapping

//edit mapping using the API
int mtic_addMappingEntry(char *fromOurInput, char *toAgent, char *withOutput); //returns mapping id or 0 if creation failed
int mtic_removeMappingEntry(int withId);
int mtic_removeMappingEntry(char *fromOurInput, char *toAgent, char *withOutput);




#endif /* mastic_public_h */
