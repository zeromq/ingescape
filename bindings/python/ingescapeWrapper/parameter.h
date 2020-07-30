//
//  parameter.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef parameter_h
#define parameter_h

#include <Python.h>

PyDoc_STRVAR(
             readParameterAsBoolDoc,
             "igs_readParameterAsBool(nameOfParameter)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfParameter as a Bool");

PyDoc_STRVAR(
             readParameterAsIntDoc,
             "igs_readParameterAsInt(nameOfParameter)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfParameter as an Int");

PyDoc_STRVAR(
             readParameterAsDoubleDoc,
             "igs_readParameterAsDouble(nameOfParameter)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfParameter as a Double");

PyDoc_STRVAR(
             readParameterAsStringDoc,
             "igs_readParameterAsString(nameOfParameter)\n"
             "--\n"
             "\n"
             "read the value of 'nameOfParameter as a String");

PyDoc_STRVAR(
             writeParameterAsBoolDoc,
             "igs_writeParameterAsBool(nameOfParameter, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfParameter' as 'value'");

PyDoc_STRVAR(
             writeParameterAsIntDoc,
             "igs_writeParameterAsInt(nameOfParameter, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfParameter' as 'value'");

PyDoc_STRVAR(
             writeParameterAsDoubleDoc,
             "igs_writeParameterAsDouble(nameOfParameter, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfParameter' as value");

PyDoc_STRVAR(
             writeParameterAsStringDoc,
             "igs_writeParameterAsString(nameOfParameter, value)\n"
             "--\n"
             "\n"
             "write the value of 'nameOfParameter' as value");

PyDoc_STRVAR(
             getTypeForParameterDoc,
             "igs_getTypeForParameter(nameOfParameter)\n"
             "--\n"
             "\n"
             "return the type of the Parameter 'nameOfParameter'\n0 the iop does not exist.\n-1 the definition live is NULL.\n If an error occurs a igs_debug will be set.");

PyDoc_STRVAR(
             getParametersNumberDoc,
             "igs_getParametersNumber()\n"
             "--\n"
             "\n"
             "return the number of Parameters for the agent");

PyDoc_STRVAR(
             getParametersListDoc,
             "igs_getParametersList(numberOfParameters)\n"
             "--\n"
             "\n"
             "return a list containing the name of Parameters");

PyDoc_STRVAR(
             checkParameterExistenceDoc,
             "igs_checkParameterExistence(nameOfParameter)\n"
             "--\n"
             "\n"
             "return True if the Parameter exist");

PyDoc_STRVAR(
             createParameterDoc,
             "igs_createParameter(nameOfParameter, value_type, value, size)\n"
             "--\n"
             "\n"
             "Create and add an Parameter for the agent.\n"
             "return The error.\n 0 is ok\n");

//igs_readparameterAsBool
 PyObject * readParameterAsBool_wrapper(PyObject * self, PyObject * args);

//igs_readParameterAsInt
 PyObject * readParameterAsInt_wrapper(PyObject * self, PyObject * args);

//igs_readParameterAsDouble
 PyObject * readParameterAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_readParameterAsString
 PyObject * readParameterAsString_wrapper(PyObject * self, PyObject * args);

//igs_writeParameterAsBool
 PyObject * writeParameterAsBool_wrapper(PyObject * self, PyObject * args);

//igs_writeParameterAsInt
 PyObject * writeParameterAsInt_wrapper(PyObject * self, PyObject * args);

//igs_writeParameterAsDouble
 PyObject * writeParameterAsDouble_wrapper(PyObject * self, PyObject * args);

//igs_writeParameterAsString
 PyObject * writeParameterAsString_wrapper(PyObject * self, PyObject * args);

//igs_getTypeForParameter
 PyObject * getTypeForParameter_wrapper(PyObject * self, PyObject * args);

//igs_getParametersNumber
 PyObject * getParametersNumber_wrapper(PyObject * self, PyObject * args);

//igs_getParametersList
 PyObject * igs_getParametersList_wrapper(PyObject * self, PyObject * args);

//igs_checkParametersExistence
 PyObject * checkParametersExistence_wrapper(PyObject * self, PyObject * args);

//igs_createParameter
 PyObject * createParameter_wrapper(PyObject * self, PyObject * args);


#endif /* parameter_h */
