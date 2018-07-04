//
//  definition.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef definition_h
#define definition_h

#include <Python.h>

//{"igs_getDefinition", getDefinition_wrapper, METH_NOARGS, "the agent definition getter.\n return The loaded definition string in json format (allocated).\n NULL if igs_definition_loaded was not initialized.\n"},
//{"igs_getDefinitionName", getDefinitionName_wrapper, METH_NOARGS, "IngeScape Definition Name Getter"},
//{"igs_getDefinitionDescription", getDefinitionDescription_wrapper, METH_NOARGS, "IngeScape Definition Description Getter"},
//{"igs_getDefinitionVersion", getDefinitionVersion_wrapper, METH_NOARGS, "IngeScape Definition Version Getter"},
//
////set definition
//{"igs_setDefinitionName", setDefinitionName_wrapper, METH_VARARGS, "Definition Name setter"},
//{"igs_setDefinitionDescription", setDefinitionDescription_wrapper, METH_VARARGS, "the agent definition description setter.\n param description The string which contains the description of the agent.\n Can't be NULL.\n return The error.\n 1 is OK, 0 Agent description is NULL, -1 Agent description is empty"},
//{"igs_setDefinitionVersion", setDefinitionVersion_wrapper, METH_VARARGS, "the agent definition version setter.\n param version The string which contains the version of the agent.\n Can't be NULL.\n return The error.\n 1 is OK, 0 Agent version is NULL, -1 Agent version is empty"},

PyDoc_STRVAR(
             loadDefinitionDoc,
             "igs_loadDefinition(jsonDefinition)\n"
             "--\n"
             "\n"
             "load definition in variable 'igs_definition_loaded' & copy in 'igs_internal_definition' from a json string\n"
             "return The error.\n 1 is OK, 0 json string is NULL, -1 Definition file has not been loaded");

PyDoc_STRVAR(
             loadDefinitionFromPathDoc,
             "igs_loadDefinitionFromPath(path)\n"
             "--\n"
             "\n"
             "load definition in variable 'igs_definition_loaded' & copy in 'igs_internal_definition' from a file path\n"
             "return The error.\n 1 is OK, 0 json string is NULL, -1 Definition file has not been loaded");

PyDoc_STRVAR(
             clearDefinitionDoc,
             "igs_clearDefinition()\n"
             "--\n"
             "\n"
             "Clear the internal definition of the agent.\n"
             "Free all members of the structure igs_definition_loaded & igs_internal_definition.\n"
             "But the pointer of these structure is not free and stay allocated.\n return 1 if ok else 0");

PyDoc_STRVAR(
             getDefinitionDoc,
             "igs_getDefinition()\n"
             "--\n"
             "\n"
             "the agent definition getter."
             "\n return The loaded definition string in json format (allocated).\n"
             "if igs_definition_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             getDefinitionNameDoc,
             "igs_getDefinitionName()\n"
             "--\n"
             "\n"
             "the agent definition name getter."
             "\n return The loaded definition name string .\n"
             " if igs_definition_loaded was not initialized raise SystemError .\n");

PyDoc_STRVAR(
             getDefinitionDescriptionDoc,
             "igs_getDefinitionDescription()\n"
             "--\n"
             "\n"
             "the agent definition description getter."
             "\n return The loaded definition description string .\n"
             "If igs_definition_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             getDefinitionVersionDoc,
             "igs_getDefinitionVersion()\n"
             "--\n"
             "\n"
             "the agent definition version getter."
             "\n return The loaded definition version string .\n"
             "If igs_definition_loaded was not initialized raise SystemError.\n");

PyDoc_STRVAR(
             setDefinitionNameDoc,
             "igs_setDefinitionName(definitionName)\n"
             "--\n"
             "\n"
             "the agent definition name setter."
             "\n return The loaded definition name string .\n"
             "return The error.\n 1 is OK, 0 Agent description is empty");

PyDoc_STRVAR(
             setDefinitionDescriptionDoc,
             "igs_setDefinitionDescription(definitionDescription)\n"
             "--\n"
             "\n"
             "the agent definition description setter."
             "\n return The error.\n 1 is OK, 0 Agent description is NULL, -1 Agent description is empty\n");

PyDoc_STRVAR(
             setDefinitionVersionDoc,
             "igs_setDefinitionVersion(definitionVersion)\n"
             "--\n"
             "\n"
             "the agent definition version setter."
             "\n return The error.\n 1 is OK, 0 Agent description is NULL, -1 Agent version is empty");

PyDoc_STRVAR(
             removeInputDoc,
             "igs_removeInput(inputName)\n"
             "--\n"
             "\n"
             "Remove and free an input for the agent.\n"
             "return The error.\n "
             "1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name.\n");

PyDoc_STRVAR(
             removeOutputDoc,
             "igs_removeOutput(OutputName)\n"
             "--\n"
             "\n"
             "Remove and free an Output for the agent.\n"
             "return The error.\n "
             "1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name.\n");
             
PyDoc_STRVAR(
             removeParameterDoc,
             "igs_removeParameter(ParameterName)\n"
             "--\n"
             "\n"
             "Remove and free an Parameter for the agent.\n"
             "return The error.\n "
             "1 is OK, 0 Definition loaded is NULL, -1 Definition live is NULL, -2 An error occurs while finding the iop by name.\n");


//igs_loadDefinition
 PyObject * loadDefinition_wrapper(PyObject * self, PyObject * args);

//igs_loadDefinitionFromPath
 PyObject * loadDefinitionFromPath_wrapper(PyObject * self, PyObject * args);

//igs_clearDefinition
 PyObject * clearDefinition_wrapper(PyObject * self, PyObject * args);

//igs_getDefinition
 PyObject * getDefinition_wrapper(PyObject * self, PyObject * args);

//igs_getDefinitionName
 PyObject * getDefinitionName_wrapper(PyObject * self, PyObject * args);

//igs_getDefinitionDescription
 PyObject * getDefinitionDescription_wrapper(PyObject * self, PyObject * args);

//igs_getDefinitionVersion
 PyObject * getDefinitionVersion_wrapper(PyObject * self, PyObject * args);

//setDefinitionName
 PyObject * setDefinitionName_wrapper(PyObject * self, PyObject * args);

//setDefinitionDescription
 PyObject * setDefinitionDescription_wrapper(PyObject * self, PyObject * args);

//setDefinitionVersion
 PyObject * setDefinitionVersion_wrapper(PyObject * self, PyObject * args);

//igs_removeInput
 PyObject * removeInput_wrapper(PyObject * self, PyObject * args);

//igs_removeOutput
 PyObject * removeOutput_wrapper(PyObject * self, PyObject * args);

//igs_removeParameter
 PyObject * removeParameter_wrapper(PyObject * self, PyObject * args);

#endif /* definition_h */
