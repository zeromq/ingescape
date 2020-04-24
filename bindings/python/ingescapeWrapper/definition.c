//
//  definition.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "definition.h"
#include <stdio.h>
#include <ingescape/ingescape.h>


//igs_loadDefinition
 PyObject * loadDefinition_wrapper(PyObject * self, PyObject * args)
{
    char * json_str;
    int result;
    PyObject * ret;
    
    // parse arguments : get the char* sent in python in json_str
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        return NULL;
    }
    
    result = igs_loadDefinition(json_str);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_loadDefinitionFromPath
 PyObject * loadDefinitionFromPath_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    int result;
    PyObject * ret;
    
    // parse the file path file_path argument
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    result = igs_loadDefinitionFromPath(file_path);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_clearDefinition
 PyObject * clearDefinition_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_clearDefinition();
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_getDefinition
 PyObject * getDefinition_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getDefinition();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getDefinitionName
 PyObject * getDefinitionName_wrapper(PyObject * self, PyObject * args)
{
    
    char * result;
    
    PyObject * ret;

    result = igs_getDefinitionName();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getDefinitionDescription
 PyObject * getDefinitionDescription_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getDefinitionDescription();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getDefinitionVersion
 PyObject * getDefinitionVersion_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getDefinitionVersion();
    
    // build the resulting string into a Python object.
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//setDefinitionName
 PyObject * setDefinitionName_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_setDefinitionName(name);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}


//setDefinitionDescription
 PyObject * setDefinitionDescription_wrapper(PyObject * self, PyObject * args)
{
    char* description ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &description)) {
        return NULL;
    }

    result = igs_setDefinitionDescription(description);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//setDefinitionVersion
 PyObject * setDefinitionVersion_wrapper(PyObject * self, PyObject * args)
{
    char* version ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &version)) {
        return NULL;
    }
    
    result = igs_setDefinitionVersion(version);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}




//igs_removeInput
 PyObject * removeInput_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_removeInput(name);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}


//igs_removeOutput
 PyObject * removeOutput_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_removeOutput(name);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_removeParameter
 PyObject * removeParameter_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_removeParameter(name);
    
    // build the resulting int into a Python object.
    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}
