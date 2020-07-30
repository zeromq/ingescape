//
//  output.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef output_h
#define output_h

#include <Python.h>

PyDoc_STRVAR(
             readOutputAsBoolDoc,
             "igs_readOutputAsBool(nameOfOutput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfOutput as a Bool");

PyDoc_STRVAR(
             readOutputAsIntDoc,
             "igs_readOutputAsInt(nameOfOutput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfOutput as an Int");

PyDoc_STRVAR(
             readOutputAsDoubleDoc,
             "igs_readOutputAsDouble(nameOfOutput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfOutput as a Double");

PyDoc_STRVAR(
             readOutputAsStringDoc,
             "igs_readOutputAsString(nameOfOutput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfOutput as a String");

PyDoc_STRVAR(
             writeOutputAsBoolDoc,
             "igs_writeOutputAsBool(nameOfOutput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfOutput' as 'value'");

PyDoc_STRVAR(
             writeOutputAsIntDoc,
             "igs_writeOutputAsInt(nameOfOutput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfOutput' as 'value'");

PyDoc_STRVAR(
             writeOutputAsDoubleDoc,
             "igs_writeOutputAsDouble(nameOfOutput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfOutput' as value");

PyDoc_STRVAR(
             writeOutputAsStringDoc,
             "igs_writeOutputAsString(nameOfOutput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfOutput' as value");

PyDoc_STRVAR(
             writeOutputAsImpulsionDoc,
             "igs_writeOutputAsImpulsion(nameOfOutput)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfOutput' as an impulsion");

PyDoc_STRVAR(
             getTypeForOutputDoc,
             "igs_getTypeForOutput(nameOfOutput)\n"
             "--\n"
             "\n"
             "return the type of the Output 'nameOfOutput'\n-1 the definition live is NULL.\n If an error occurs a igs_debug will be set.");

PyDoc_STRVAR(
             getOutputsNumberDoc,
             "igs_getOutputsNumber()\n"
             "--\n"
             "\n"
             "return the number of Outputs for the agent");

PyDoc_STRVAR(
             getOutputsListDoc,
             "igs_getOutputsList(numberOfOutputs)\n"
             "--\n"
             "\n"
             "return a list containing the name of Outputs");

PyDoc_STRVAR(
             checkOutputExistenceDoc,
             "igs_checkOutputExistence(nameOfOutput)\n"
             "--\n"
             "\n"
             "return True if the Output exist");

PyDoc_STRVAR(
             createOutputDoc,
             "igs_createOutput(nameOfOutput, value_type, value, size)\n"
             "--\n"
             "\n"
             "Create and add an Output for the agent.\n"
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             muteOutputDoc,
             "igs_muteOutput(nameOfOutput)\n"
             "--\n"
             "\n"
             "Mute an Output of the agent.\n"
             "return The error.\n  0 is ok\n");

PyDoc_STRVAR(
             unmuteOutputDoc,
             "igs_unmuteOutput(nameOfOutput)\n"
             "--\n"
             "\n"
             "Unmute an Output of the agent.\n"
             "return The error.\n  0 is ok\n");

PyDoc_STRVAR(
             isOutputMutedDoc,
             "igs_isOutputMuted(nameOfOutput)\n"
             "--\n"
             "\n"
             "Give the state of an agent output (mute/unmute).\n"
             "return true if muted else false");

//igs_readOutputAsBool
 PyObject * readOutputAsBool_wrapper(PyObject * self, PyObject * args);

//igs_readOutputAsInt
 PyObject * readOutputAsInt_wrapper(PyObject * self, PyObject * args);

//igs_readOutputAsDouble
 PyObject * readOutputAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_readOutputAsStirng
 PyObject * readOutputAsString_wrapper(PyObject * self, PyObject * args);

//igs_writeOutputAsBool
 PyObject * writeOutputAsBool_wrapper(PyObject * self, PyObject * args);

//igs_writeOutputAsInt
 PyObject * writeOutputAsInt_wrapper(PyObject * self, PyObject * args);

//igs_writeOutputAsDouble
 PyObject * writeOutputAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_writeOutputAsString
 PyObject * writeOutputAsString_wrapper(PyObject * self, PyObject * args);

//igs_writeOutputAsImpulsion
 PyObject * writeOutputAsImpulsion_wrapper(PyObject * self, PyObject * args);

//igs_getTypeForOutput
 PyObject * getTypeForOutput_wrapper(PyObject * self, PyObject * args);

//igs_getOutputsNumber
 PyObject * getOutputsNumber_wrapper(PyObject * self, PyObject * args);

//igs_getOutputsList

 PyObject * igs_getOutputsList_wrapper(PyObject * self, PyObject * args);

//igs_checkOutputExistence
 PyObject * checkOutputExistence_wrapper(PyObject * self, PyObject * args);

//igs_muteOutput
 PyObject * muteOutput_wrapper(PyObject * self, PyObject * args);

//igs_unmuteOutput
 PyObject * unmuteOutput_wrapper(PyObject * self, PyObject * args);

//igs_isOutputMuted
 PyObject * isOutputMuted_wrapper(PyObject * self, PyObject * args);

//igs_createOutput
 PyObject * createOutput_wrapper(PyObject * self, PyObject * args);

#endif /* output_h */
