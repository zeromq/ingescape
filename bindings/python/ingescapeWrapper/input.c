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
    size_t * size;
    int result;
    
    if (!PyArg_ParseTuple(args, "sii", &name, &value, &size)) {
        return NULL;
    }
    result = igs_readInput(name, value, size);
    return PyLong_FromLong(result);
}

//igs_readInputAsBool
 PyObject * readInputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse arguments :  the name of the input
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting bool into a Python object and return it
    if (igs_readInputAsBool(name)) {
        Py_RETURN_TRUE;
    } else{
        Py_RETURN_FALSE;
    }
}

//igs_readInputAsInt
 PyObject * readInputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readInputAsInt(name);
    return PyLong_FromLong(result);
}

//igs_readInputAsDouble
 PyObject * readInputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;

    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readInputAsDouble(name);
    return PyFloat_FromDouble(result);
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
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
    
}

//igs_writeInputAsBool
 PyObject * writeInputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    // parse arguments : the name of the input and the bool we want to write
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }

    if (value == Py_True){
        result = igs_writeInputAsBool(name, true);
    }else{
        result = igs_writeInputAsBool(name, false);
    }
    return PyLong_FromLong(result);
}

//igs_writeInputAsInt
 PyObject * writeInputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    // parse arguments : the name of the iop and the int
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }
    result = igs_writeInputAsInt(name, value);
    return PyLong_FromLong(result);
}

//igs_writeInputAsDouble
 PyObject * writeInputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    
    // parse arguments : the iop aand the double
    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }
    result = igs_writeInputAsDouble(name, value);
    return PyLong_FromLong(result);
}

//igs_writeInputAsString
 PyObject * writeInputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    // parse arguments : the iop and the string
    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }
    result = igs_writeInputAsString(name, value);
    return PyLong_FromLong(result);
}

//igs_writeInputAsImpulsion
 PyObject * writeInputAsImpulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    // parse arguments : the name of the iop
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_writeInputAsImpulsion(name);
    return PyLong_FromLong(result);
}

//igs_getTypeForInput
 PyObject * getTypeForInput_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    // parse the name of the input and cast into a char*
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    int result = igs_getTypeForInput(name);
    return PyLong_FromLong(result);
}

//igs_getInputsNumber
 PyObject * getInputsNumber_wrapper(PyObject * self, PyObject * args)
{
     int result = igs_getInputsNumber();
    return PyLong_FromLong(result);
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
    // parse the name of the input
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    if (igs_checkInputExistence(name)) {
          Py_RETURN_TRUE;
    } else{
        Py_RETURN_FALSE;
    }
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
    
    // parse and cast all the arguments for igs_createInput
    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    type = (iopType_t)(_type);
    result = igs_createInput(name, type, value, sizeof(PyObject));
    return PyLong_FromLong(result);
}
