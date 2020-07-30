//
//  observecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef observecallback_h
#define observecallback_h

#include <Python.h>

PyDoc_STRVAR(
             observeInputDoc,
             "igs_observeInput(input, callback, args)\n"
             "--\n"
             "\n"
             "Observe a parameter and associate a callback to it.\n"
             "When the input value will change the associated callback will be called.\n \n"
             "param input The string which contains the name of the input we want to observe. Can't be NULL.\n"
             "param callback is the function we want to be executed when the input is changed. Can't be NULL.\n"
             "param args A tuple containing the args of the callback in python. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             observeOutputDoc,
             "igs_observeOutput(output, callback, args)\n"
             "--\n"
             "\n"
             "Observe a parameter and associate a callback to it.\n"
             "When the output value will change the associated callback will be called.\n \n"
             "param output The string which contains the name of the input we want to observe. Can't be NULL.\n"
             "param callback is the function we want to be executed when the output is changed. Can't be NULL.\n"
             "param args A tuple containing the args of the callback in python. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

PyDoc_STRVAR(
             observeParameterDoc,
             "igs_observeParameter(parameter, callback, args)\n"
             "--\n"
             "\n"
             "Observe a parameter and associate a callback to it.\n"
             "When the parameter value will change the associated callback will be called.\n \n"
             "param parameter The string which contains the name of the parameter we want to observe. Can't be NULL.\n"
             "param callback is the function we want to be executed when the parameter is changed. Can't be NULL.\n"
             "param args A tuple containing the args of the callback in python. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

 PyObject * igs_observeInput_wrapper(PyObject *self, PyObject *args);

 PyObject * igs_observeOutput_wrapper(PyObject *self, PyObject *args);

 PyObject * igs_observeParameter_wrapper(PyObject *self, PyObject *args);

#endif /* observecallback_h */
