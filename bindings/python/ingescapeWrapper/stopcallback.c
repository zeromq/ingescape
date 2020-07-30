//
//  stopcallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "stopcallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>
#include "uthash/utlist.h"

typedef struct stopCallback{
    PyObject *call;         //stopCallback
    PyObject *argstopList;  // argument for stopCallback
    struct stopCallback *next;
    struct stopCallback *prev;
}stopCallback;

stopCallback *stopList = NULL;



//stopCallback : execute the callback stacked with the igs_observeForcedStop_wrapper
void stop(void *myData){
    //run through all callbacks to execute them
    stopCallback *actuel = NULL;
    DL_FOREACH(stopList, actuel){
        // Lock the GIL to execute the callback safely
        PyGILState_STATE d_gstate;
        d_gstate = PyGILState_Ensure();
        
        //execute the callback
        PyObject_CallObject(actuel->call, actuel->argstopList);
        
        //release the GIL
        PyGILState_Release(d_gstate);
    }
    
}

// wrapper for igs_observeForcedStop
PyObject * igs_observeExternalStop_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject arg;
    PyObject *tempargstopList;

    // parse the callback and arguments sent from python
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) {  // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    Py_XINCREF(temp);               // Add a reference to new callback
    
    tempargstopList = Py_BuildValue("(O)", arg); //cast arglist into a tuple
    
    Py_XINCREF(tempargstopList);    // Add a reference to arglist

    // add the callback to the list of stopCallback
    stopCallback *newElt = calloc(1, sizeof(stopCallback));
    newElt->argstopList = tempargstopList;
    newElt->call = temp;
    DL_APPEND(stopList, newElt);
    
    igs_observeExternalStop(stop, NULL);
    return PyLong_FromLong(1);
}

