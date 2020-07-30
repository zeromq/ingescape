//
//  data.h
//  ingescapeWrapp
//
//  Created by vaugien on 14/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//
#include "data.h"
#include <stdio.h>
#include <ingescape/ingescape.h>


//igs_writeOutputAsData
 PyObject * writeOutputAsData_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    long size;
    Py_buffer buf;
    int  result;
    // parse and cast the name, data and size argument given in python
    if (!PyArg_ParseTuple(args, "sy*k", &name, &buf, &size)) {
        return NULL;
    }
    result = igs_writeOutputAsData(name, buf.buf, size);
    return PyLong_FromLong(result);
}


//igs_readOutputAsData
 PyObject * readOutputAsData_wrapper(PyObject * self, PyObject * args)
{
    char *name;
    void *myData;
    long valueSize;
    
    // parse and cast the name argument given in python
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    igs_readOutputAsData(name, &myData, &valueSize);
    
    // Cast the data read and the valueSize into a PyTuple and return it
    PyObject *ret = Py_BuildValue("y#", myData, valueSize);
    PyObject *return_result = PyTuple_New(2);
    PyTuple_SetItem(return_result, 0, ret);
    PyTuple_SetItem(return_result, 1, PyLong_FromLong(valueSize));
    return  return_result;
}

//igs_readInputAsData
 PyObject * readInputAsData_wrapper(PyObject * self, PyObject * args)
{
    char *name;
    void *myData;
    long valueSize;
    
    // parse and cast the name argument given in python
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    igs_readInputAsData(name, &myData, &valueSize);
    
    // Cast the data read and the valueSize into a PyTuple and return it
    PyObject *ret = Py_BuildValue("y#", myData, valueSize);
    PyObject *return_result = PyTuple_New(2);
    PyTuple_SetItem(return_result, 0, ret);
    PyTuple_SetItem(return_result, 1, PyLong_FromLong(valueSize));
    return return_result;
}

//igs_writeInputAsData
 PyObject * writeInputAsData_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    long size;
    Py_buffer buf;
    int  result;
    
    // parse and cast the name, data and size argument given in python
    if (!PyArg_ParseTuple(args, "sy*k", &name, &buf, &size)) {
        return NULL;
    }
    result = igs_writeInputAsData(name, buf.buf, size);
    return PyLong_FromLong(result);
}


//igs_writeParameterAsData
 PyObject * writeParameterAsData_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    long size;
    Py_buffer buf;
    int  result;
    
    // parse and cast the name, data and size argument given in python
    if (!PyArg_ParseTuple(args, "sy*k", &name, &buf, &size)) {
        return NULL;
    }
    result = igs_writeParameterAsData(name, buf.buf, size);
    return PyLong_FromLong(result);
}



//igs_readParameterAsData
 PyObject * readParameterAsData_wrapper(PyObject * self, PyObject * args)
{
    char *name;
    void *myData;
    long valueSize;
    
    // parse and cast the name argument given in python
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    igs_readParameterAsData(name, &myData, &valueSize);
    
    // Cast the data read and the valueSize into a PyTuple and return it
    PyObject *ret = Py_BuildValue("y#", myData, &valueSize);
    PyObject *return_result = PyTuple_New(2);
    PyTuple_SetItem(return_result, 0, ret);
    PyTuple_SetItem(return_result, 1, PyLong_FromLong(valueSize));
    return return_result;
}




