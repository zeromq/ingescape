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
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    
    result = igs_setAgentName(name);
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}


 // wrapper for igs_getAgentname
 PyObject * getAgentName_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    
    result = igs_getAgentName();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    
    free(&result);
    return ret;
}

 // wrapper for igs_setAgentState
 PyObject * setAgentState_wrapper(PyObject * self, PyObject * args)
{
    char * state;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &state)) {
        return NULL;
    }

    result = igs_setAgentState(state);
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

 // wrapper for igs_getAgentState
 PyObject * getAgentState_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getAgentState();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    
    return ret;
}

 // wrapper for igs_mute
 PyObject * mute_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_mute();
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}



// wrapper for igs_unmute
 PyObject * unmute_wrapper(PyObject * self, PyObject * args)
{
    
    int result;
    PyObject * ret;

    result = igs_unmute();
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}


// wrapper for igs_ismute
 PyObject * ismuted_wrapper(PyObject * self, PyObject * args)
{
    bool result;
    
    result = igs_isMuted();
    
    // build the resulting bool into a Python Boolean and return it
    if (result){
        free(&result);
        return Py_True;
    }else{
        free(&result);
        return Py_False;
    }
}


// wrapper for igs_freeze
 PyObject * freeze_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_freeze();
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}



// wrapper for igs_unfreeze
 PyObject * unfreeze_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_unfreeze();
    
    // build the resulting integer into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}


// wrapper for igs_setCanBeFrozen
 PyObject * setCanBeFrozen_wrapper(PyObject * self, PyObject * args)
{
    int result = 1;
    PyObject *canBeFrozen;

    // parse arguments
    if (!PyArg_ParseTuple(args, "O", &canBeFrozen)){
        return NULL;
    }
    
    
    if (canBeFrozen == Py_True){
        igs_setCanBeFrozen(true);
    }else if (canBeFrozen == Py_False){
        igs_setCanBeFrozen(false);
    }else{
        result = 0;
    }
    return PyLong_FromLong(result);
}

// wrapper for igs_canBeFrozen
 PyObject * canBeFrozen_wrapper(PyObject * self, PyObject * args)
{
    bool result ;
    PyObject *canBeFrozen;
    
    result = igs_canBeFrozen();
    
    //Cast the bool into a Python boolean and return it
    if (result){
        canBeFrozen = Py_True;
    }else{
        canBeFrozen = Py_False;
    }
    return canBeFrozen;
}

// wrapper for igs_isFrozen
 PyObject * isFrozen_wrapper(PyObject * self, PyObject * args)
{
    bool result;

    result = igs_isFrozen();
    
    // build the resulting bool into a Python object and return it
    if (result){
        free(&result);
        return Py_True;
    }else{
        free(&result);
        return Py_False;
    }
}
