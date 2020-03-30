//
//  call.h
//  ingescapeWrapp
//
//  Created by vaugien on 24/03/2020.
//  Copyright © 2020 ingenuity. All rights reserved.
//

#ifndef call_h
#define call_h

#include <stdio.h>
#include <Python.h>

PyObject *sendCall_wrapper(PyObject * self, PyObject * args);

PyObject *removeCall_wrapper(PyObject * self, PyObject * args);

PyObject *addArgumentToCall_wrapper(PyObject * self, PyObject * args);

PyObject *removeArgumentFromCall_wrapper(PyObject * self, PyObject * args);

PyObject *getNumberOfCalls_wrapper(PyObject * self, PyObject * args);

PyObject *checkCallExistence_wrapper(PyObject * self, PyObject * args);

PyObject *getCallsList_wrapper(PyObject * self, PyObject * args);

PyObject *getNumberOfArgumentForCall_wrapper(PyObject * self, PyObject * args);

PyObject *getArgumentListForCall_wrapper(PyObject * self, PyObject * args);

PyObject *checkCallArgumentExistence_wrapper(PyObject * self, PyObject * args);

PyObject *initCall_wrapper(PyObject *self, PyObject *args);


#endif /* call_h */
