//
//  init.c
//  ingescapeWrapp
//
//  Created by vaugien on 06/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "init.h"
#include <stdio.h>
#include <ingescape/ingescape.h>



 // wrapper for igs_setAgentname
 PyObject * setAgentName_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    igs_setAgentName(name);
    return PyLong_FromLong(0);
}

 // wrapper for igs_getAgentname
 PyObject * getAgentName_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getAgentName();
    // build the resulting string into a Python object.
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

 // wrapper for igs_setAgentState
 PyObject * setAgentState_wrapper(PyObject * self, PyObject * args)
{
    char * state;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &state)) {
        return NULL;
    }
    igs_setAgentState(state);
    return PyLong_FromLong(0);
}

 // wrapper for igs_getAgentState
 PyObject * getAgentState_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getAgentState();
    // build the resulting string into a Python object.
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

 // wrapper for igs_mute
 PyObject * mute_wrapper(PyObject * self, PyObject * args)
{
    igs_mute();
    return PyLong_FromLong(0);
}



// wrapper for igs_unmute
 PyObject * unmute_wrapper(PyObject * self, PyObject * args)
{
    igs_unmute();
    return PyLong_FromLong(0);
}


// wrapper for igs_ismute
 PyObject * ismuted_wrapper(PyObject * self, PyObject * args)
{
    if (igs_isMuted()){
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}


// wrapper for igs_freeze
 PyObject * freeze_wrapper(PyObject * self, PyObject * args)
{
    int result = igs_freeze();
    // build the resulting integer into a Python object.
    return PyLong_FromLong(result);
}



// wrapper for igs_unfreeze
 PyObject * unfreeze_wrapper(PyObject * self, PyObject * args)
{
    igs_unfreeze();
    return PyLong_FromLong(0);
}

// wrapper for igs_isFrozen
 PyObject * isFrozen_wrapper(PyObject * self, PyObject * args)
{
    if (igs_isFrozen()){
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}
