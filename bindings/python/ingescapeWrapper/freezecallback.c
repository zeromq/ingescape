//
//  freezecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "freezecallback.h"
#include <stdio.h>
#include <ingescape/ingescape.h>
#include "uthash/utlist.h"

// chained list to stock all freezeCallbacks
typedef struct freezeCallback{
    PyObject *call;     //freeze callback
    PyObject *arglist;  //arguments for the callback
    struct freezeCallback *next;
    struct freezeCallback *prev;
}freezeCallback;

freezeCallback *freezeList = NULL;


// freezeCallback : execute the callback stacked with 
void freeze(bool isPaused, void *myData){
    freezeCallback *actuel = NULL;
    DL_FOREACH(freezeList, actuel){
        // Lock the GIL in order to execute the callback saffely
        PyGILState_STATE d_gstate;
        d_gstate = PyGILState_Ensure();
        //Call the python callback actuel->call
        PyObject_CallObject(actuel->call, actuel->arglist);
        
        // Release the GIL
        PyGILState_Release(d_gstate);
    }
    
}
 PyObject * igs_observeFreeze_wrapper(PyObject *self, PyObject *args)
{
    
    PyObject *temp;
    PyObject *temparglist;
    PyObject arg;
    
    //parse python argument into a callable object and some arguments
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) { // check if the callback is a callable object
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    
    Py_XINCREF(temp);         // Add a reference to new callback
    
    temparglist = Py_BuildValue("(O)", arg);

    Py_XINCREF(temparglist);         // Add a reference to new arglist
    
    freezeCallback *newElt = calloc(1, sizeof(freezeCallback));
    newElt->call = temp;
    newElt->arglist = temparglist;
    DL_APPEND(freezeList, newElt);
    
    igs_observeFreeze(freeze, NULL);
    return PyLong_FromLong(0);
}

