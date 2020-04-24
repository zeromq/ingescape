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

PyDoc_STRVAR(
             sendCallDoc,
             "igs_sendCall(agentNameOrUUID, callName, argumentTuple)\n"
             "--\n"
             "\n"
             "Send call to agent identified by agentNameOrUUID \n"
             "argumentTuple must be a tuple containing argument formated like (IGS_TYPE, valueType)");

PyDoc_STRVAR(
             removeCallDoc,
             "igs_removeCall(callName)\n"
             "--\n"
             "\n"
             "Remove call named callName ");

PyDoc_STRVAR(
             addArgumentToCallDoc,
             "igs_addArgumentToCall(callName, argName, argType)\n"
             "--\n"
             "\n"
             "Add argument name argName of type argType to call callName ");

PyDoc_STRVAR(
             removeArgumentFromCallDoc,
             "igs_removeArgumentFromCall(callName, argName)\n"
             "--\n"
             "\n"
             "Remove argument name argName from call callName ");

PyDoc_STRVAR(
             getNumberOfCallsDoc,
             "igs_getNumberOfCalls()\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             checkCallExistenceDoc,
             "igs_checkCallExistence(callName)\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             getCallsListDoc,
             "igs_getCallsList()\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             getNumberOfArgumentForCallDoc,
             "igs_getNumberOfArgumentFor(callName)\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             getArgumentListForCallDoc,
             "igs_getArgumentListForCall(callName)\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             checkCallArgumentExistenceDoc,
             "igs_checkCallArgumentExistence(callName, argName)\n"
             "--\n"
             "\n");

PyDoc_STRVAR(
             initCallDoc,
             "igs_initCall(callName, callback, myData)\n"
             "--\n"
             "\n"
             "Initialize a call and set a callback on it");

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
