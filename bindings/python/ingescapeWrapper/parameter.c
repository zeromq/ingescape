//
//  parameter.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "parameter.h"
#include <stdio.h>
#include <ingescape/ingescape.h>


//igs_readParameter
 PyObject * readParameter_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    void * value;
    size_t * size;
    int result;
    // parse arguments :  the name of the output, the value and the size
    if (!PyArg_ParseTuple(args, "ssi", &name, &value, &size)) {
        return NULL;
    }
    result = igs_readParameter(name, value, size);
    return PyLong_FromLong(result);
}

//igs_readparameterAsBool
 PyObject * readParameterAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting bool into a Python object.
    if (igs_readParameterAsBool(name)) {
        Py_RETURN_TRUE;
    } else{
        Py_RETURN_FALSE;
    }
}

//igs_readParameterAsInt
 PyObject * readParameterAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    // parse argument and cast it : the name of the Parameter
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readParameterAsInt(name);
    return PyLong_FromLong(result);
}

//igs_readParameterAsDouble
 PyObject * readParameterAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;
    // parse argument and cast it : the name of the Parameter
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readParameterAsDouble(name);
    return PyFloat_FromDouble(result);
}


//igs_readParameterAsString
 PyObject * readParameterAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * result;
    PyObject * ret;
    // parse argument and cast it : the name of the Parameter
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readParameterAsString(name);
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_writeParameterAsBool
 PyObject * writeParameterAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    // parse arguments and cast them : name of the param and the boolean
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }

    if (value == Py_True){
        result = igs_writeParameterAsBool(name, true);
    }else{
        result = igs_writeParameterAsBool(name, false);
    }
    return PyLong_FromLong(result);
}

//igs_writeParameterAsInt
 PyObject * writeParameterAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    // parse arguments and cast them : the name and the integer
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }
    result = igs_writeParameterAsInt(name, value);
    return PyLong_FromLong(result);
}

//igs_writeParameterAsDouble
 PyObject * writeParameterAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }
    result = igs_writeParameterAsDouble(name, value);
    return PyLong_FromLong(result);
}

//igs_writeParameterAsString
 PyObject * writeParameterAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }
    result = igs_writeParameterAsString(name, value);
    return PyLong_FromLong(result);
}


//igs_getTypeForParameter
 PyObject * getTypeForParameter_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    int result;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_getTypeForParameter(name);
    return PyLong_FromLong(result);
}


//igs_getParametersNumber
 PyObject * getParametersNumber_wrapper(PyObject * self, PyObject * args)
{
    int result = igs_getParametersNumber();
    return PyLong_FromLong(result);
}


//igs_getParametersList
 PyObject * igs_getParametersList_wrapper(PyObject * self, PyObject * args)
{
    long nbOfElements;
    PyObject * ret;

    if (!PyArg_ParseTuple(args, "i", &nbOfElements)) {
        return NULL;
    }
    char **result = igs_getParametersList(&nbOfElements);
    // create a Python List and add element one by one
    ret = PyList_New(nbOfElements);
    int i ;
    for (i = 0; i < nbOfElements; i++){
        PyList_SetItem(ret, i, PyBytes_FromString(result[i]));
    }
    return ret;
}


//igs_checkParametersExistence
 PyObject * checkParametersExistence_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting double into a Python object.
    if (igs_checkParameterExistence(name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

//igs_createParameter
 PyObject * createParameter_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    iopType_t type;
    int _type;
    void *value;
    long size;
    int result;
    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    // get the type of the parameter
    type = (iopType_t)(_type);
    result = igs_createParameter(name, type, value, sizeof(PyObject));
    return PyLong_FromLong(result);
}
