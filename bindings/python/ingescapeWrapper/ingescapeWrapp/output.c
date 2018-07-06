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
    PyObject * ret;
    
    // parse arguments :  the name of the output, the value and the size
    if (!PyArg_ParseTuple(args, "ssi", &name, &value, &size)) {
        return NULL;
    }

    result = igs_readOutput(name, value, size);
    
    // build the resulting string into a Python object and return it
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_readOutputAsBool
 PyObject * readOutputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool result;
    
    // parse arguments and cast them : the name of the output and the boolean
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readOutputAsBool(name);

    // build the resulting bool into a Python object.
    if (result) {
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
    PyObject * ret;
    
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_readOutputAsInt(name);

    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_readOutputAsDouble
 PyObject * readOutputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double result;
    PyObject * ret;
    
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    
    result = igs_readOutputAsDouble(name);
    
    
    // build the resulting double into a Python object.
    ret = PyFloat_FromDouble(result);
    free(&result);
    return ret;
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

    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}


//igs_writeOutputAsBool
 PyObject * writeOutputAsBool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    PyObject *value;
    int result;
    PyObject * ret;
    
    // parse arguments and cast them : the name of the output and the boolean
    if (!PyArg_ParseTuple(args, "sO", &name, &value)) {
        return NULL;
    }

    if (value == Py_True){
        result = igs_writeOutputAsBool(name, true);
    }else{
        result = igs_writeOutputAsBool(name, false);
    }
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
//    free(&result);
    return ret;
}

//igs_writeOutputAsInt
 PyObject * writeOutputAsInt_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    int result;
    PyObject * ret;
    
    // parse arguments and cast them : the name of the output and the integer
    if (!PyArg_ParseTuple(args, "si", &name, &value)) {
        return NULL;
    }

    result = igs_writeOutputAsInt(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeOutputAsDouble
 PyObject * writeOutputAsDouble_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    int result;
    
    PyObject * ret;
    
    // parse arguments and cast them : the name of the output and the double
    if (!PyArg_ParseTuple(args, "sd", &name, &value)) {
        return NULL;
    }

    result = igs_writeOutputAsDouble(name, value);
    
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeOutputAsString
 PyObject * writeOutputAsString_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    int result;
    PyObject * ret;
    
    // parse arguments and cast them : the name of the output and the string/char*
    if (!PyArg_ParseTuple(args, "ss", &name, &value)) {
        return NULL;
    }

    result = igs_writeOutputAsString(name, value);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_writeOutputAsImpulsion
 PyObject * writeOutputAsImpulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    PyObject * ret;
    
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_writeOutputAsImpulsion(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_getTypeForOutput
 PyObject * getTypeForOutput_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    int result;
    PyObject * ret;
    
    // parse argument and cast it : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_getTypeForOutput(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//igs_getOutputsNumber
 PyObject * getOutputsNumber_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_getOutputsNumber();

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
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
    
    // run the actual function
    
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
    bool result;
    PyObject * ret;
    
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_checkOutputExistence(name);

    // build the resulting bool into a Python object and return it
    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
}

//igs_muteOutput
 PyObject * muteOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    PyObject * ret;
    
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_muteOutput(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_unmuteOutput
 PyObject * unmuteOutput_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int result;
    PyObject * ret;
    
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_unmuteOutput(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_isOutputMuted
 PyObject * isOutputMuted_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool result;
    PyObject * ret;
    
    // parse argument : the name of the output
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_isOutputMuted(name);
    
    // build the resulting bool into a Python object and return it
    if (result) {
        ret = Py_True;
    } else{
        ret = Py_False;
    }
    free(&result);
    return ret;
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
    
    PyObject * ret;
    
    // parse arguments and cast them : name of the output, type, value and size of the value
    if (!PyArg_ParseTuple(args, "siOi", &name, &_type, &value, &size)) {
        return NULL;
    }
    // get the type of the iop
    type = (iopType_t)(_type);
    
    result = igs_createOutput(name, type, value, sizeof(PyObject));

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}
