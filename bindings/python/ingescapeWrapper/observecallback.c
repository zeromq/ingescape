//
//  observecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//
#include "observecallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>
#include "uthash/utlist.h"

#if (defined WIN32 || defined _WIN32)
#include "unixfunctions.h"
#endif

typedef struct observeCallback {
    char *nameArg;      // name of the iop
    PyObject *call;     //observeCallback
    PyObject *arglist;  //argument of the callback
    struct observeCallback *next;
    struct observeCallback *prev;
}observeCallback;
observeCallback *observeList = NULL;


//observeCallback that execute the callback for the iop that has benn changed
void observe(iop_t iopType, const char* name, iopType_t valueType, void* value, unsigned long valueSize, void* myData){
//    char* received = ((char *)myData);
    //    observeElement * actuel = callbackObserveList -> premier;
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    //run through all callbacks to execute them
    observeCallback *actuel = NULL;
    DL_FOREACH(observeList, actuel){
        if (strcmp(actuel->nameArg, name) == 0){
            // call python code
            PyObject_CallObject(actuel->call, actuel->arglist);
        }
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject * igs_observeInput_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *input;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &input, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(temp);       // Add a reference to new callback
    temparglist = Py_BuildValue("(O)", arg);    //cast arglist into a tuple
    Py_INCREF(temparglist);     // Add a reference to arglist
    
    // add the callback to the list of Callback
    observeCallback *newElt = calloc(1, sizeof(observeCallback));
    newElt->nameArg = strndup(input, strlen(input));
    newElt->arglist = temparglist;
    newElt->call = temp;
    DL_APPEND(observeList, newElt);
    igs_observeInput(input, observe, input);
    return PyLong_FromLong(0);
    
}


PyObject * igs_observeOutput_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *output;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &output, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    temparglist = Py_BuildValue("(O)", arg);    //cast arglist into a tuple
    Py_XINCREF(temparglist);         // Add a reference to arglist
    
    // add the callback to the list of Callback
    observeCallback *newElt = calloc(1, sizeof(observeCallback));
    newElt->nameArg = strndup(output, strlen(output));
    newElt->arglist = temparglist;
    newElt->call = temp;
    DL_APPEND(observeList, newElt);
    igs_observeOutput(output, observe, output);
    return PyLong_FromLong(0);
}

PyObject * igs_observeParameter_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *param;
    
    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "sOO", &param, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {      // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    temparglist = Py_BuildValue("(O)", arg);        //cast arglist into a tuple
    Py_XINCREF(temparglist);         // Add a reference to arglist
    
    // add the callback to the list of Callback
    observeCallback *newElt = calloc(1, sizeof(observeCallback));
    newElt->nameArg = strndup(param, strlen(param));
    newElt->arglist = temparglist;
    newElt->call = temp;
    DL_APPEND(observeList, newElt);
    igs_observeInput(param, observe, param);
    return PyLong_FromLong(0);
}

