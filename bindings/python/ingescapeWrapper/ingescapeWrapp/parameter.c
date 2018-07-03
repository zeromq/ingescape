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
    long * size;
    int result;
    PyObject * ret;
    
    // parse arguments :  the name of the output, the value and the size
    if (!PyArg_ParseTuple(args, "ssi", &name, &value, &size)) {
        return NULL;
    }

    result = igs_readParameter(name, value, size);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_readparameterAsBool
 PyObject * readParameterAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool result;
    PyObject * ret;
    
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readParameterAsBool(name);
    
    // build the resulting bool into a Python object.
    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
}

//igs_readParameterAsInt
 PyObject * readParameterAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    
    PyObject * ret;
    
    // parse argument and cast it : the name of the Parameter
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    
    result = igs_readParameterAsInt(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_readParameterAsDouble
 PyObject * readParameterAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;
    PyObject * ret;
    
    // parse argument and cast it : the name of the Parameter
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readParameterAsDouble(name);

    ret = PyFloat_FromDouble(result);
    free(&result);
    return ret;
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

    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_writeParameterAsBool
 PyObject * writeParameterAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    
    PyObject * ret;
    
    // parse arguments and cast them : name of the param and the boolean
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }

    if (value == Py_True){
        result = igs_writeParameterAsBool(name, true);
    }else{
        result = igs_writeParameterAsBool(name, false);
    }

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeParameterAsInt
 PyObject * writeParameterAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    PyObject * ret;
    
    // parse arguments and cast them : the name and the integer
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }
    
    // run the actual function
    
    result = igs_writeParameterAsInt(name, value);
    
    
    // build the resulting double into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeParameterAsDouble
 PyObject * writeParameterAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    PyObject * ret;

    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }
    
    result = igs_writeParameterAsDouble(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeParameterAsString
 PyObject * writeParameterAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    
    PyObject * ret;

    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }
    
    result = igs_writeParameterAsString(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//igs_getTypeForParameter
 PyObject * getTypeForParameter_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    int result;
    PyObject * ret;
    
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_getTypeForParameter(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//igs_getParametersNumber
 PyObject * getParametersNumber_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;
    
    result = igs_getParametersNumber();

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
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
    bool result;
    PyObject * ret;

    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_checkParameterExistence(name);
    
    
    // build the resulting double into a Python object.
    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
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
    PyObject * ret;

    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    // get the type of the parameter
    type = (iopType_t)(_type);

    result = igs_createParameter(name, type, value, sizeof(PyObject));

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}
