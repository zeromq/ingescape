//
//  input.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef input_h
#define input_h
#include <Python.h>

PyDoc_STRVAR(
             readInputAsBoolDoc,
             "igs_readInputAsBool(nameOfInput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfInput' as a Bool");

PyDoc_STRVAR(
             readInputAsIntDoc,
             "igs_readInputAsInt(nameOfInput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfInput' as an Int");

PyDoc_STRVAR(
             readInputAsDoubleDoc,
             "igs_readInputAsDouble(nameOfInput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfInput' as a Double");

PyDoc_STRVAR(
             readInputAsStringDoc,
             "igs_readInputAsString(nameOfInput)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfInput' as a String");



PyDoc_STRVAR(
             writeInputAsBoolDoc,
             "igs_writeInputAsBool(nameOfInput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfInput' as 'value'");

PyDoc_STRVAR(
             writeInputAsIntDoc,
             "igs_writeInputAsInt(nameOfInput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfInput' as 'value'");

PyDoc_STRVAR(
             writeInputAsDoubleDoc,
             "igs_writeInputAsDouble(nameOfInput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfInput' as value");

PyDoc_STRVAR(
             writeInputAsStringDoc,
             "igs_writeInputAsString(nameOfInput, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfInput' as value");

PyDoc_STRVAR(
             writeInputAsImpulsionDoc,
             "igs_writeInputAsImpulsion(nameOfInput)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfInput' as an impulsion");

PyDoc_STRVAR(
             getTypeForInputDoc,
             "igs_getTypeForInput(nameOfInput)\n"
             "--\n"
             "\n"
             "return the type of the input 'nameOfInput' \n-1 the definition live is NULL.\n If an error occurs a igs_debug will be set.");

PyDoc_STRVAR(
             getInputsNumberDoc,
             "igs_getInputsNumber()\n"
             "--\n"
             "\n"
             "return the number of inputs for the agent");

PyDoc_STRVAR(
             getInputsListDoc,
             "igs_getInputsList(numberOfInputs)\n"
             "--\n"
             "\n"
             "return a list containing the name of inputs");

PyDoc_STRVAR(
             checkInputExistenceDoc,
             "igs_checkInputExistence(nameOfInput)\n"
             "--\n"
             "\n"
             "return True if the input exist");

PyDoc_STRVAR(
             createInputDoc,
             "igs_createInput(nameOfInput, value_type, value, size)\n"
             "--\n"
             "\n"
             "Create and add an input for the agent.\n"
             "return The error.\n 0 is ok\n");

//igs_readInputAsBool
 PyObject * readInputAsBool_wrapper(PyObject * self, PyObject * args);

//igs_readInputAsInt
 PyObject * readInputAsInt_wrapper(PyObject * self, PyObject * args);

//igs_readInputAsDouble
 PyObject * readInputAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_readInputAsStirng
 PyObject * readInputAsString_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsBool
 PyObject * writeInputAsBool_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsInt
 PyObject * writeInputAsInt_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsDouble
 PyObject * writeInputAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsString
 PyObject * writeInputAsString_wrapper(PyObject * self, PyObject * args);

//igs_writeInputAsImpulsion
 PyObject * writeInputAsImpulsion_wrapper(PyObject * self, PyObject * args);

//igs_getTypeForInput
 PyObject * getTypeForInput_wrapper(PyObject * self, PyObject * args);

//igs_getInputsNumber
 PyObject * getInputsNumber_wrapper(PyObject * self, PyObject * args);

//igs_getInputsList
 PyObject * igs_getInputsList_wrapper(PyObject * self, PyObject * args);

//igs_checkInputExistence
 PyObject * checkInputExistence_wrapper(PyObject * self, PyObject * args);

//igs_createInput
 PyObject * createInput_wrapper(PyObject * self, PyObject * args);


#endif /* input_h */
