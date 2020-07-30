//
//  output.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//
#include "output.h"
#include <stdio.h>
#include <ingescape/ingescape.h>


//igs_readOutput
 PyObject * readOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    void * value;
    size_t * size;
    int result;
    // parse arguments :  the name of the output, the value and the size
    if (!PyArg_ParseTuple(args, "ssi", &name, &value, &size)) {
        return NULL;
    }
    result = igs_readOutput(name, value, size);
    return PyLong_FromLong(result);
}

//igs_readOutputAsBool
 PyObject * readOutputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse arguments and cast them : the name of the output and the boolean
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting bool into a Python object.
    if (igs_readOutputAsBool(name)) {
        Py_RETURN_TRUE;
    } else{
        Py_RETURN_FALSE;
    }
}

//igs_readOutputAsInt
 PyObject * readOutputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readOutputAsInt(name);
    return PyLong_FromLong(result);
}

//igs_readOutputAsDouble
 PyObject * readOutputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readOutputAsDouble(name);
    return PyFloat_FromDouble(result);
}


//igs_readOutputAsStirng
 PyObject * readOutputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * result;
    PyObject * ret;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_readOutputAsString(name);
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
       return PyBytes_FromString("");
    }
}

//igs_writeOutputAsBool
 PyObject * writeOutputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    // parse arguments and cast them : the name of the output and the boolean
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }
    if (value == Py_True){
        result = igs_writeOutputAsBool(name, true);
    }else{
        result = igs_writeOutputAsBool(name, false);
    }
    return PyLong_FromLong(result);
}

//igs_writeOutputAsInt
 PyObject * writeOutputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    // parse arguments and cast them : the name of the output and the integer
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }
    result = igs_writeOutputAsInt(name, value);
    return PyLong_FromLong(result);
}

//igs_writeOutputAsDouble
 PyObject * writeOutputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    // parse arguments and cast them : the name of the output and the double
    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }
    result = igs_writeOutputAsDouble(name, value);
    return PyLong_FromLong(result);
}

//igs_writeOutputAsString
 PyObject * writeOutputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    // parse arguments and cast them : the name of the output and the string/char*
    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }
    result = igs_writeOutputAsString(name, value);
    return PyLong_FromLong(result);
}

//igs_writeOutputAsImpulsion
 PyObject * writeOutputAsImpulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_writeOutputAsImpulsion(name);
    return PyLong_FromLong(result);
}

//igs_getTypeForOutput
 PyObject * getTypeForOutput_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    int result;
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_getTypeForOutput(name);
    return PyLong_FromLong(result);
}


//igs_getOutputsNumber
 PyObject * getOutputsNumber_wrapper(PyObject * self, PyObject * args)
{
    int result;
    result = igs_getOutputsNumber();
    return PyLong_FromLong(result);
}


//igs_getOutputsList

 PyObject * igs_getOutputsList_wrapper(PyObject * self, PyObject * args)
{
    long nbOfElements;
    PyObject * ret;
    
    // parse argument : the number of outputs and cast it
    if (!PyArg_ParseTuple(args, "i", &nbOfElements)) {
        return NULL;
    }
    char **result = igs_getOutputsList(&nbOfElements);
    ret = PyList_New(nbOfElements);
    int i ;
    for (i = 0; i < nbOfElements; i++){
        PyList_SetItem(ret, i, PyBytes_FromString(result[i]));
    }
    return ret;
}

//igs_checkOutputExistence
 PyObject * checkOutputExistence_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting bool into a Python object and return it
    if (igs_checkOutputExistence(name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

//igs_muteOutput
 PyObject * muteOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    igs_muteOutput(name);
    return PyLong_FromLong(0);
}

//igs_unmuteOutput
 PyObject * unmuteOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    igs_unmuteOutput(name);
    return PyLong_FromLong(0);
}

//igs_isOutputMuted
 PyObject * isOutputMuted_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    // build the resulting bool into a Python object and return it
    if (igs_isOutputMuted(name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

//igs_createOutput
 PyObject * createOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    iopType_t type;
    int _type;
    void *value;
    long size;
    int result;
    // parse arguments and cast them : name of the output, type, value and size of the value
    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    // get the type of the iop
    type = (iopType_t)(_type);
    result = igs_createOutput(name, type, value, sizeof(PyObject));
    return PyLong_FromLong(result);
}
