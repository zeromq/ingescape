
//
//  ingescapePython.c
//  ingescapeWrapp
//
//  Created by vaugien on 09/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include <stdio.h>
#include <Python.h>


#include "advanced.h"
#include "init.h"
#include "start.h"
#include "input.h"
#include "output.h"
#include "parameter.h"
#include "definition.h"
#include "mapping.h"
#include "admin.h"
#include "observecallback.h"
#include "freezecallback.h"
#include "data.h"
#include "stopcallback.h"
#include "call.h"


/*
 *Defining all the methods
 *{"name of the method", function_wrapper called, calling convention, "Docstring in python"}
 */

static PyMethodDef ingescapeMethods[] =
{
    //start & stop the agent
    {"igs_startWithDevice",startWithDevice_wrapper, METH_VARARGS, startWithDeviceDoc },
    {"igs_startWithIP", startWithIP_wrapper, METH_VARARGS, startWithIPDoc},
    {"igs_stop", stop_wrapper, METH_NOARGS, stopDoc},
    
    
    //agent name set and get
    {"igs_setAgentName", setAgentName_wrapper, METH_VARARGS, setAgentNameDoc},
    {"igs_getAgentName", getAgentName_wrapper, METH_NOARGS, getAgentNameDoc},
    
    //control agent state
    {"igs_setAgentState", setAgentState_wrapper, METH_VARARGS,  setAgentStateDoc},
    {"igs_getAgentState", getAgentState_wrapper, METH_NOARGS,  getAgentStateDoc},
    
    //mute the agent ouputs
    {"igs_mute", mute_wrapper, METH_NOARGS, muteDoc},
    {"igs_unmute", unmute_wrapper, METH_NOARGS, unmuteDoc},
    {"igs_isMuted", ismuted_wrapper, METH_NOARGS, ismutedDoc},
    
    //freeze and unfreeze the agent
    {"igs_freeze", freeze_wrapper, METH_NOARGS, freezeDoc},
    {"igs_unfreeze", unfreeze_wrapper, METH_NOARGS, unfreezeDoc},
    {"igs_isFrozen", isFrozen_wrapper, METH_NOARGS, isfrozenDoc},

    //read input per type
    {"igs_readInputAsBool", readInputAsBool_wrapper, METH_VARARGS, readInputAsBoolDoc},
    {"igs_readInputAsInt", readInputAsInt_wrapper, METH_VARARGS, readInputAsIntDoc},
    {"igs_readInputAsDouble", readInputAsDouble_wrapper, METH_VARARGS, readInputAsDoubleDoc},
    {"igs_readInputAsString", readInputAsString_wrapper, METH_VARARGS, readInputAsStringDoc},
    {"igs_readInputAsData", readInputAsData_wrapper, METH_VARARGS,readInputAsDataDoc},
    
    //read output per type
    {"igs_readOutputAsBool", readOutputAsBool_wrapper, METH_VARARGS, readOutputAsBoolDoc},
    {"igs_readOutputAsInt", readOutputAsInt_wrapper, METH_VARARGS, readOutputAsIntDoc},
    {"igs_readOutputAsDouble", readOutputAsDouble_wrapper, METH_VARARGS, readOutputAsDoubleDoc},
    {"igs_readOutputAsString", readOutputAsString_wrapper, METH_VARARGS, readOutputAsStringDoc},
    {"igs_readOutputAsData", readOutputAsData_wrapper, METH_VARARGS, readOutputAsDataDoc},
    
    //read parameter per type
    {"igs_readParameterAsBool", readParameterAsBool_wrapper, METH_VARARGS, readParameterAsBoolDoc},
    {"igs_readParameterAsInt", readParameterAsInt_wrapper, METH_VARARGS, readParameterAsIntDoc},
    {"igs_readParameterAsDouble", readParameterAsDouble_wrapper, METH_VARARGS, readParameterAsDoubleDoc},
    {"igs_readParameterAsString", readParameterAsString_wrapper, METH_VARARGS, readParameterAsStringDoc},
    {"igs_readParameterAsData", readParameterAsData_wrapper, METH_VARARGS, readParameterAsDataDoc},
    
    //write input per type
    {"igs_writeInputAsBool", writeInputAsBool_wrapper, METH_VARARGS, writeInputAsBoolDoc},
    {"igs_writeInputAsInt", writeInputAsInt_wrapper, METH_VARARGS, writeInputAsIntDoc},
    {"igs_writeInputAsDouble", writeInputAsDouble_wrapper, METH_VARARGS, writeInputAsDoubleDoc},
    {"igs_writeInputAsString", writeInputAsString_wrapper, METH_VARARGS, writeInputAsStringDoc},
    {"igs_writeInputAsImpulsion", writeInputAsImpulsion_wrapper, METH_VARARGS, writeInputAsImpulsionDoc},
    {"igs_writeInputAsData", writeInputAsData_wrapper, METH_VARARGS, writeInputAsDataDoc},
    
    //write output per type
    {"igs_writeOutputAsBool", writeOutputAsBool_wrapper, METH_VARARGS, writeOutputAsBoolDoc},
    {"igs_writeOutputAsInt", writeOutputAsInt_wrapper, METH_VARARGS, writeOutputAsIntDoc},
    {"igs_writeOutputAsDouble", writeOutputAsDouble_wrapper, METH_VARARGS, writeOutputAsDoubleDoc},
    {"igs_writeOutputAsString", writeOutputAsString_wrapper, METH_VARARGS, writeOutputAsStringDoc},
    {"igs_writeOutputAsImpulsion", writeOutputAsImpulsion_wrapper, METH_VARARGS, writeOutputAsImpulsionDoc},
    {"igs_writeOutputAsData", writeOutputAsData_wrapper, METH_VARARGS, writeOutputAsDataDoc},
    
    //write Parameter per type
    {"igs_writeParameterAsBool", writeParameterAsBool_wrapper, METH_VARARGS, writeParameterAsBoolDoc},
    {"igs_writeParameterAsInt", writeParameterAsInt_wrapper, METH_VARARGS, writeParameterAsIntDoc},
    {"igs_writeParameterAsDouble", writeParameterAsDouble_wrapper, METH_VARARGS, writeParameterAsDoubleDoc},
    {"igs_writeParameterAsString", writeParameterAsString_wrapper, METH_VARARGS, writeParameterAsStringDoc},
    {"igs_writeParameterAsData", writeParameterAsData_wrapper, METH_VARARGS, writeParameterAsDataDoc},

    //check IOP type
    {"igs_getTypeForInput", getTypeForInput_wrapper, METH_VARARGS, getTypeForInputDoc},
    {"igs_getTypeForOutput", getTypeForOutput_wrapper, METH_VARARGS, getTypeForOutputDoc},
    {"igs_getTypeForParameter", getTypeForParameter_wrapper, METH_VARARGS, getTypeForParameterDoc},
    
    //get number of IOP
    {"igs_getInputsNumber", getInputsNumber_wrapper, METH_NOARGS, getInputsNumberDoc},
    {"igs_getOutputsNumber", getOutputsNumber_wrapper, METH_NOARGS, getOutputsNumberDoc},
    {"igs_getParametersNumber", getParametersNumber_wrapper, METH_NOARGS, getParametersNumberDoc},
    
    //check existence of IOP
    {"igs_checkInputExistence", checkInputExistence_wrapper, METH_VARARGS, checkInputExistenceDoc},
    {"igs_checkOutputExistence", checkOutputExistence_wrapper, METH_VARARGS, checkOutputExistenceDoc},
    {"igs_checkParameterExistence", checkParametersExistence_wrapper, METH_VARARGS, checkParameterExistenceDoc},
    
    // get Iop list
    {"igs_getInputsList", igs_getInputsList_wrapper, METH_VARARGS, getInputsListDoc},
    {"igs_getOutputsList", igs_getOutputsList_wrapper, METH_VARARGS, getOutputsListDoc},
    {"igs_getParametersList", igs_getParametersList_wrapper, METH_VARARGS, getParametersListDoc},
    
    //mute or unmute an IOP
    {"igs_muteOutput", muteOutput_wrapper, METH_VARARGS, muteOutputDoc},
    {"igs_unmuteOutput", unmuteOutput_wrapper, METH_VARARGS, unmuteOutputDoc},
    {"igs_isOutputMuted", isOutputMuted_wrapper, METH_VARARGS,  isOutputMutedDoc},
    
    //load definition
    {"igs_loadDefinition", loadDefinition_wrapper, METH_VARARGS, loadDefinitionDoc},
    {"igs_loadDefinitionFromPath", loadDefinitionFromPath_wrapper, METH_VARARGS, loadDefinitionFromPathDoc},
    {"igs_clearDefinition", clearDefinition_wrapper, METH_NOARGS, clearDefinitionDoc},
    
    //get information about definition
    {"igs_getDefinition", getDefinition_wrapper, METH_NOARGS, getDefinitionDoc},
    {"igs_getDefinitionName", getDefinitionName_wrapper, METH_NOARGS, getDefinitionNameDoc},
    {"igs_getDefinitionDescription", getDefinitionDescription_wrapper, METH_NOARGS, getDefinitionDescriptionDoc},
    {"igs_getDefinitionVersion", getDefinitionVersion_wrapper, METH_NOARGS, getDefinitionVersionDoc},
    
    //set definition
    {"igs_setDefinitionName", setDefinitionName_wrapper, METH_VARARGS, setDefinitionNameDoc},
    {"igs_setDefinitionDescription", setDefinitionDescription_wrapper, METH_VARARGS, setDefinitionDescriptionDoc},
    {"igs_setDefinitionVersion", setDefinitionVersion_wrapper, METH_VARARGS, setDefinitionVersionDoc},
    
    //remove IOP
    {"igs_removeInput", removeInput_wrapper, METH_VARARGS, removeInputDoc},
    {"igs_removeOutput", removeOutput_wrapper, METH_VARARGS, removeOutputDoc},
    {"igs_removeParameter", removeParameter_wrapper, METH_VARARGS, removeParameterDoc},
    
    //createIOP
    {"igs_createInput", createInput_wrapper, METH_VARARGS, createInputDoc},
    {"igs_createOutput", createOutput_wrapper, METH_VARARGS, createOutputDoc},
    {"igs_createParameter", createParameter_wrapper, METH_VARARGS, createParameterDoc},
    
    //load mapping
    {"igs_loadMapping", loadMapping_wrapper, METH_VARARGS,loadMappingDoc},
    {"igs_loadMappingFromPath", loadMappingFromPath_wrapper, METH_VARARGS, loadMappingFromPathDoc},
    {"igs_clearMapping", clearMapping_wrapper, METH_NOARGS, clearMappingDoc},
    
    //get information about mapping
    {"igs_getMapping", getMapping_wrapper, METH_NOARGS, getMappingDoc},
    {"igs_getMappingName", getMappingName_wrapper, METH_NOARGS, getMappingNameDoc},
    {"igs_getMappingDescription", getMappingDescription_wrapper, METH_NOARGS, getMappingDescriptionDoc},
    {"igs_getMappingVersion", getMappingVersion_wrapper, METH_NOARGS, getMappingVersionDoc},
    {"igs_getMappingEntriesNumber", getMappingEntriesNumber_wrapper, METH_NOARGS, getMappingEntriesNumberDoc},

    //set mapping
    {"igs_setMappingName", setMappingName_wrapper, METH_VARARGS, setMappingNameDoc},
    {"igs_setMappingDescription", setMappingDescription_wrapper, METH_VARARGS, setMappingDescriptionDoc},
    {"igs_setMappingVersion", setMappingVersion_wrapper, METH_VARARGS, setMappingVersionDoc},
    {"igs_addMappingEntry", addMappingEntry_wrapper, METH_VARARGS, addMappingEntriesDoc},
    
    //remove mapping
    {"igs_removeMappingEntryWithId", removeMappingEntryWithId_wrapper, METH_VARARGS, removeMappingEntryWithIdDoc},
    {"igs_removeMappingEntryWithName", removeMappingEntryWithName_wrapper, METH_VARARGS, removeMappingEntryWithNameDoc},
    {"igs_setRequestOutputsFromMappedAgents", setRequestOutputsFromMappedAgents_wrapper, METH_VARARGS, ""},
    {"igs_getRequestOutputsFromMappedAgents", getRequestOutputsFromMappedAgents_wrapper, METH_NOARGS, ""},

    //Command line for the agent can be passed here for inclusion in the agent's headers. If not set, header is initialized with exec path.
    {"igs_setCommandLine", setCommandLine_wrapper, METH_VARARGS, setCommandLineDoc},

    //logs and debug messages
    {"igs_setVerbose", setVerbose_wrapper, METH_VARARGS, setVerboseDoc},
    {"igs_isVerbose", getVerbose_wrapper, METH_NOARGS, isVerboseDoc},
    {"igs_setLogStream", setLogStream_wrapper, METH_VARARGS, setLogStreamDoc},
    {"igs_getLogStream", getLogStream_wrapper, METH_NOARGS, getLogStreamDoc},
    {"igs_setLogInFile", setLogInFile_wrapper, METH_VARARGS, setLogInFileDoc},
    {"igs_getLogInFile", getLogInFile_wrapper, METH_NOARGS, getLogInFileDoc},
    {"igs_setUseColorVerbose", setUseColorVerbose_wrapper, METH_VARARGS, setUseColorVerboseDoc},
    {"igs_getUseColorVerbose", getUseColorVerbose_wrapper, METH_NOARGS, getUseColorVerboseDoc},
    {"igs_setLogPath", setLogPath_wrapper, METH_VARARGS, setLogPathDoc},
    {"igs_getLogPath", getLogPath_wrapper, METH_NOARGS, getLogPathDoc},
    {"igs_isStarted", isStarted_wrapper, METH_NOARGS, interruptedDoc},
    
    //observe Iop, freeze and forced stop
    {"igs_observeInput", igs_observeInput_wrapper, METH_VARARGS, observeInputDoc},
    {"igs_observeOutput", igs_observeOutput_wrapper, METH_VARARGS, observeOutputDoc},
    {"igs_observeParameter", igs_observeParameter_wrapper, METH_VARARGS, observeParameterDoc},
    {"igs_observeFreeze", igs_observeFreeze_wrapper, METH_VARARGS, observeFreezeDoc},
    {"igs_observeExternalStop", igs_observeExternalStop_wrapper, METH_VARARGS, observeForcedStopDoc},

    //resources file management
    {"igs_setDefinitionPath", setDefinitionPath_wrapper, METH_VARARGS, setDefinitionPathDoc},
    {"igs_setMappingPath", setMappingPath_wrapper, METH_VARARGS, setMappingPathDoc},
    {"igs_writeDefinitionToPath", writeDefinitionToPath_wrapper, METH_NOARGS, writeDefinitionToPathDoc},
    {"igs_writeMappingToPath", writeMappingToPath_wrapper, METH_NOARGS, writeMappingToPathDoc},
    {"igs_version", version_wrapper, METH_NOARGS, versionDoc},
    
    // advanced
    {"igs_setPublishingPort", igs_setPublishingPort_wrapper, METH_VARARGS, setPublishingPortDoc},
    {"igs_setDiscoveryInterval", igs_setDiscoveryInterval_wrapper, METH_VARARGS, setDiscoveryIntervalDoc},
    {"igs_setAgentTimeOut", igs_setAgentTimeOut_wrapper, METH_VARARGS, setAgentTimeoutDoc},
    {"igs_busJoinChannel", igs_busJoinChannel_wrapper, METH_VARARGS, busJoinChannelDoc},
    {"igs_busLeaveChannel", igs_busLeaveChannel_wrapper, METH_VARARGS, busLeaveChannelDoc},
    {"igs_busSendStringToChannel", igs_busSendStringToChannel_wrapper, METH_VARARGS, busSendStringToChannelDoc},
    {"igs_busSendDataToChannel", igs_busSendDataToChannel_wrapper, METH_VARARGS, busSendDataToChannelDoc},
    {"igs_busSendStringToAgent", igs_busSendStringToAgent_wrapper, METH_VARARGS, busSendStringToAgentDoc},
    {"igs_busSendDataToAgent", igs_busSendDataToAgent_wrapper, METH_VARARGS, busSendDataToAgentDoc},
    {"igs_busAddServiceDescription", igs_busAddServiceDescription_wrapper, METH_VARARGS, busAddServiceDescriptionDoc},
    {"igs_busRemoveServiceDescription", igs_busRemoveServiceDescription_wrapper, METH_VARARGS, busremoveServiceDescriptionDoc},
    
    // calls
    {"igs_sendCall", sendCall_wrapper, METH_VARARGS, sendCallDoc},
    {"igs_removeCall", removeCall_wrapper, METH_VARARGS, removeCallDoc},
    {"igs_addArgumentToCall", addArgumentToCall_wrapper, METH_VARARGS, addArgumentToCallDoc},
    {"igs_removeArgumentFromCall", removeArgumentFromCall_wrapper, METH_VARARGS, removeArgumentFromCallDoc},
    {"igs_getNumberOfCalls", getNumberOfCalls_wrapper, METH_VARARGS, getNumberOfCallsDoc},
    {"igs_checkCallExistence", checkCallExistence_wrapper, METH_VARARGS, checkCallExistenceDoc},
    {"igs_getCallsList", getCallsList_wrapper, METH_VARARGS, getCallsListDoc},
    {"igs_getNumberOfArgumentForCall", getNumberOfArgumentForCall_wrapper, METH_VARARGS, getNumberOfArgumentForCallDoc},
    {"igs_getArgumentListForCall", getArgumentListForCall_wrapper, METH_VARARGS, getArgumentListForCallDoc},
    {"igs_checkCallArgumentExistence", checkCallArgumentExistence_wrapper, METH_VARARGS, checkCallArgumentExistenceDoc},
    {"igs_initCall", initCall_wrapper, METH_VARARGS, initCallDoc},
    {"igs_setLicensePath", setLicensePath_wrapper, METH_VARARGS, ""},
    
    {"igs_getNetdevicesList", igs_getNetdevicesList_wrapper, METH_NOARGS, ""},
    {"igs_getNetaddressesList", igs_getNetadressesList_wrapper, METH_NOARGS, ""},
    
    //logging
    {"igs_setLogLevel", setLogLevel_wrapper, METH_VARARGS, "set log level, \n Trace = 0, ..."},
    {"igs_getLogLevel", getLogLevel_wrapper, METH_VARARGS, "get log level, \n Trace = 0, ..."},
    {"igs_trace", trace_wrapper, METH_VARARGS, ""},
    {"igs_debug", debug_wrapper, METH_VARARGS, ""},
    {"igs_info", info_wrapper, METH_VARARGS, ""},
    {"igs_warn", warn_wrapper, METH_VARARGS, ""},
    {"igs_error", error_wrapper, METH_VARARGS, ""},
    {"igs_fatal", fatal_wrapper, METH_VARARGS, ""},
    
    {NULL, NULL, 0, NULL}       /* Sentinel */
};


static struct PyModuleDef initingescape_wrapper =
{
    PyModuleDef_HEAD_INIT,
    "ingescape", //    Name of the python module
    "IngeScape", //    docstring of the module
    -1,
    ingescapeMethods
};

PyMODINIT_FUNC PyInit_ingescape(void)
{
    Py_Initialize();
    return PyModule_Create(&initingescape_wrapper);
}
