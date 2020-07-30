//
//  mapping.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef mapping_h
#define mapping_h

#include <Python.h>



//igs_loadMapping
 PyObject * loadMapping_wrapper(PyObject * self, PyObject * args);

//igs_loadMappingFromPath
 PyObject * loadMappingFromPath_wrapper(PyObject * self, PyObject * args);

//igs_clearMapping
 PyObject * clearMapping_wrapper(PyObject * self, PyObject * args);

//igs_getMapping
 PyObject * getMapping_wrapper(PyObject * self, PyObject * args);

//igs_getMappingName
 PyObject * getMappingName_wrapper(PyObject * self, PyObject * args);

//igs_getMappingDescription
 PyObject * getMappingDescription_wrapper(PyObject * self, PyObject * args);

//igs_getMappingVersion
 PyObject * getMappingVersion_wrapper(PyObject * self, PyObject * args);


//setMappingName
 PyObject * setMappingName_wrapper(PyObject * self, PyObject * args);

//setMappingDescription
 PyObject * setMappingDescription_wrapper(PyObject * self, PyObject * args);

//setMappingVersion
 PyObject * setMappingVersion_wrapper(PyObject * self, PyObject * args);

//getMappingEntriesNumber
 PyObject * getMappingEntriesNumber_wrapper(PyObject * self, PyObject * args);

//addMappingEntry
 PyObject * addMappingEntry_wrapper(PyObject * self, PyObject * args);

//igs_removeMappingEntryWithId
 PyObject * removeMappingEntryWithId_wrapper(PyObject * self, PyObject * args);

//igs_removeMappingEntryWithName
 PyObject * removeMappingEntryWithName_wrapper(PyObject * self, PyObject * args);

// igs_setRequestOutputsFromMappedAgents
 PyObject * setRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args);

// igs_getRequestOutputsFromMappedAgents
 PyObject * getRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args);


PyDoc_STRVAR(
             setRequestOutputsFromMappedAgentsDoc,
             "igs_setRequestOutputsFromMappedAgents(notify)\n"
             "--\n"
             "boolean notify\n"
             "\n");

PyDoc_STRVAR(
             getRequestOutputsFromMappedAgentsDoc,
             "igs_getRequestOutputsFromMappedAgents()\n"
             "--\n"
             "\n"
             "\n");


PyDoc_STRVAR(
             removeMappingEntryWithNameDoc,
             "igs_removeMappingEntryWithName(fromInput, toAgent, toOutput)\n"
             "--\n"
             "\n"
             "this function allows the user to remove a mapping in table by the input name, the extern agent's name, the extern agent's output.\n"
             "param fromOurInput The string which contains the name of the input mapped. Can't be NULL.\n"
             "param toAgent The string which contains the name of the extern agent. Can't be NULL.\n"
             "param withOutput The string which contains the name of the output mapped of the extern agent. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             removeMappingEntryWithIdDoc,
             "igs_removeMappingEntryWithId(idInput)\n"
             "--\n"
             "\n"
             "this function allows the user to remove a mapping in table by its id.\n"
             "param theId The id of the mapping.Cannot be negative.\n"
             "Return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             addMappingEntriesDoc,
             "igs_addMappingEntry(fromInput, toAgent, toOutput)\n"
             "--\n"
             "\n"
             "this function allows the user to add a new mapping entry dynamically.\n \n"
             "param fromOurInput The string which contains the name of the input to be mapped. Can't be NULL.\n"
             "Param toAgent The string which contains the name of the extern agent. Can't be NULL. \n"
             "param withOutput The string which contains the name of the output of the extern agent to be mapped. Can't be NULL.\n"
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             getMappingEntriesNumberDoc,
             "igs_getMappingEntriesNumber()\n"
             "--\n"
             "\n"
             "the agent mapping entries number getter.\n return The number of mapping type output entries.\n"
             "If -1 The structure igs_internal_mapping is NULL.\n");

PyDoc_STRVAR(
             loadMappingDoc,
             "igs_loadMapping(jsonMapping)\n"
             "--\n"
             "\n"
             "load Mapping in variable 'igs_Mapping_loaded' & copy in 'igs_internal_Mapping' from a json string\n"
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             loadMappingFromPathDoc,
             "igs_loadMappingFromPath(path)\n"
             "--\n"
             "\n"
             "load Mapping in variable 'igs_Mapping_loaded' & copy in 'igs_internal_Mapping' from a file path\n"
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             clearMappingDoc,
             "igs_clearMapping()\n"
             "--\n"
             "\n"
             "Clear the internal Mapping of the agent.\n"
             "Free all members of the structure igs_Mapping_loaded & igs_internal_Mapping.\n"
             "But the pointer of these structure is not free and stay allocated.\n"
             "Return 0 if ok\n");

PyDoc_STRVAR(
             getMappingDoc,
             "igs_getMapping()\n"
             "--\n"
             "\n"
             "the agent Mapping getter."
             "\n return The loaded Mapping string in json format (allocated).\n"
             "if igs_Mapping_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             getMappingNameDoc,
             "igs_getMappingName()\n"
             "--\n"
             "\n"
             "the agent Mapping name getter."
             "\n return The loaded Mapping name string .\n"
             " if igs_Mapping_loaded was not initialized raise SystemError .\n");

PyDoc_STRVAR(
             getMappingDescriptionDoc,
             "igs_getMappingDescription()\n"
             "--\n"
             "\n"
             "the agent Mapping description getter."
             "\n return The loaded Mapping description string .\n"
             "If igs_Mapping_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             getMappingVersionDoc,
             "igs_getMappingVersion()\n"
             "--\n"
             "\n"
             "the agent Mapping version getter."
             "\n return The loaded Mapping version string .\n"
             "If igs_Mapping_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             setMappingNameDoc,
             "igs_setMappingName(MappingName)\n"
             "--\n"
             "\n"
             "the agent Mapping name setter."
             "\n return The loaded Mapping name string .\n"
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             setMappingDescriptionDoc,
             "igs_setMappingDescription(MappingDescription)\n"
             "--\n"
             "\n"
             "the agent Mapping description setter."
             "\n return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             setMappingVersionDoc,
             "igs_setMappingVersion(MappingVersion)\n"
             "--\n"
             "\n"
             "the agent Mapping version setter."
             "\n return The error.\n 0 is ok\n");


#endif /* mapping_h */
