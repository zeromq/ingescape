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
    // parse arguments : get the char* sent in python in json_str
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        return NULL;
    }
    result = igs_loadDefinition(json_str);
    // build the resulting int into a Python object.
    return PyLong_FromLong(result);
}

//igs_loadDefinitionFromPath
 PyObject * loadDefinitionFromPath_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    int result;
    // parse the file path file_path argument
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }
    result = igs_loadDefinitionFromPath(file_path);
    // build the resulting int into a Python object.
    return PyLong_FromLong(result);
}

//igs_clearDefinition
 PyObject * clearDefinition_wrapper(PyObject * self, PyObject * args)
{
    igs_clearDefinition();   
    return PyLong_FromLong(0);
}

//igs_getDefinition
 PyObject * getDefinition_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getDefinition();
    // build the resulting string into a Python object.
    if(result!=NULL){
        ret = PyBytes_FromString(result);   
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getDefinitionName
 PyObject * getDefinitionName_wrapper(PyObject * self, PyObject * args)
{
    char * result;   
    PyObject * ret;
    result = igs_getDefinitionName();
    // build the resulting string into a Python object.
    if(result!=NULL){
        ret = PyBytes_FromString(result);   
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getDefinitionDescription
 PyObject * getDefinitionDescription_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getDefinitionDescription();
    // build the resulting string into a Python object.
   if(result!=NULL){
        ret = PyBytes_FromString(result);   
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getDefinitionVersion
 PyObject * getDefinitionVersion_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getDefinitionVersion();
    // build the resulting string into a Python object.
    if(result!=NULL){
        ret = PyBytes_FromString(result);   
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//setDefinitionName
 PyObject * setDefinitionName_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    igs_setDefinitionName(name);
    return PyLong_FromLong(0);
}


//setDefinitionDescription
 PyObject * setDefinitionDescription_wrapper(PyObject * self, PyObject * args)
{
    char* description ;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &description)) {
        return NULL;
    }
    igs_setDefinitionDescription(description);
    return PyLong_FromLong(0);
}

//setDefinitionVersion
 PyObject * setDefinitionVersion_wrapper(PyObject * self, PyObject * args)
{
    char* version ;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &version)) {
        return NULL;
    }
    igs_setDefinitionVersion(version);
    return PyLong_FromLong(0);
}

//igs_removeInput
 PyObject * removeInput_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_removeInput(name);
    // build the resulting int into a Python object.
    return PyLong_FromLong(result);
}


//igs_removeOutput
 PyObject * removeOutput_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_removeOutput(name);
    // build the resulting int into a Python object.
    return PyLong_FromLong(result);
}

//igs_removeParameter
 PyObject * removeParameter_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    result = igs_removeParameter(name);
    // build the resulting int into a Python object.
    return PyLong_FromLong(result);
}
