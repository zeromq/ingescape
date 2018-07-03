//
//  input.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "input.h"
#include <stdio.h>
#include <ingescape/ingescape.h>

//igs_readInput
 PyObject * readInput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    void * value;
    long * size;
    
    int result;
    PyObject * ret;
    
    if (!PyArg_ParseTuple(args, "sii", &name, &value, &size)) {
        return NULL;
    }

    result = igs_readInput(name, value, size);
    
    // build the resulting string into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_readInputAsBool
 PyObject * readInputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool result;
    PyObject * ret;
    
    // parse arguments :  the name of the input
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    
    result = igs_readInputAsBool(name);
    
    // build the resulting bool into a Python object and return it
    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
}

//igs_readInputAsInt
 PyObject * readInputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readInputAsInt(name);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_readInputAsDouble
 PyObject * readInputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;
    
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readInputAsDouble(name);

    // build the resulting double into a Python object.
    ret = PyFloat_FromDouble(result);
    free(&result);
    return ret;
}


//igs_readInputAsStirng
 PyObject * readInputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readInputAsString(name);

    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}


//igs_writeInputAsBool
 PyObject * writeInputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    
    PyObject * ret;
    
    // parse arguments : the name of the input and the bool we want to write
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }

    if (value == Py_True){
        result = igs_writeInputAsBool(name, true);
    }else{
        result = igs_writeInputAsBool(name, false);
    }

    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeInputAsInt
 PyObject * writeInputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    
    PyObject * ret;
    
    // parse arguments : the name of the iop and the int
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }

    result = igs_writeInputAsInt(name, value);

    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeInputAsDouble
 PyObject * writeInputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    PyObject * ret;
    
    // parse arguments : the iop aand the double
    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }

    result = igs_writeInputAsDouble(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeInputAsString
 PyObject * writeInputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    PyObject * ret;
    
    // parse arguments : the iop and the string
    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }

    result = igs_writeInputAsString(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeInputAsImpulsion
 PyObject * writeInputAsImpulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    PyObject * ret;
    
    // parse arguments : the name of the iop
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_writeInputAsImpulsion(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_getTypeForInput
 PyObject * getTypeForInput_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    int result;
    PyObject * ret;
    
    // parse the name of the input and cast into a char*
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_getTypeForInput(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_getInputsNumber
 PyObject * getInputsNumber_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_getInputsNumber();

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//igs_getInputsList
 PyObject * igs_getInputsList_wrapper(PyObject * self, PyObject * args)
{
    long nbOfElements;
    PyObject * ret;
    
    // parse the number of element
    if (!PyArg_ParseTuple(args, "i", &nbOfElements)) {
        return NULL;
    }

    char **result = igs_getInputsList(&nbOfElements);
    
    // build the resulting list into a Python object.
    ret = PyList_New(nbOfElements);
    int i ;
    for (i = 0; i < nbOfElements; i++){
        //set items of the python list one by one
        PyList_SetItem(ret, i, PyBytes_FromString(result[i]));
    }
    return ret;
}


//igs_checkInputExistence
 PyObject * checkInputExistence_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool result;
    PyObject * ret;
    
    // parse the name of the input
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_checkInputExistence(name);

    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
}


//igs_createInput
 PyObject * createInput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    iopType_t type;
    int _type;
    void *value;
    long size;
    
    int result;
    
    PyObject * ret;
    
    // parse and cast all the arguments for igs_createInput
    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    type = (iopType_t)(_type);

    result = igs_createInput(name, type, value, sizeof(PyObject));

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}
