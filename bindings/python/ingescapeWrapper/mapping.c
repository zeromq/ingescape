//
//  mapping.h
//  ingescapeWrapp
//
//  Created by vaugien on 12/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "mapping.h"
#include <stdio.h>
#include <ingescape/ingescape.h>

//igs_loadMapping
 PyObject * loadMapping_wrapper(PyObject * self, PyObject * args)
{
    char * json_str;
    int result;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        return NULL;
    }
    result = igs_loadMapping(json_str);
    return PyLong_FromLong(result);
}

//igs_loadMappingFromPath
 PyObject * loadMappingFromPath_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }
    int result = igs_loadMappingFromPath(file_path);
    return PyLong_FromLong(result);
}

//igs_clearMapping
 PyObject * clearMapping_wrapper(PyObject * self, PyObject * args)
{
    igs_clearMapping();
    return PyLong_FromLong(0);
}

//igs_getMapping
 PyObject * getMapping_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getMapping();
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getMappingName
 PyObject * getMappingName_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getMappingName();
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getDefinitionDescription
 PyObject * getMappingDescription_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getMappingDescription();
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//igs_getMappingVersion
 PyObject * getMappingVersion_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    result = igs_getMappingVersion();
    if(result != NULL){
        ret = PyBytes_FromString(result);
        free(&result);
        return ret;
    }else{
        return PyBytes_FromString("");
    }
}

//setMappingName
 PyObject * setMappingName_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    // parse the name of the mapping
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }
    igs_setMappingName(name);
    return PyLong_FromLong(0);
}

//setMappingDescription
 PyObject * setMappingDescription_wrapper(PyObject * self, PyObject * args)
{
    char* description ;
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &description)) {
        return NULL;
    }
    igs_setMappingDescription(description);
    return PyLong_FromLong(0);
}

//setMappingVersion
 PyObject * setMappingVersion_wrapper(PyObject * self, PyObject * args)
{
    char* version ;
    // parse and cast the version of the mapping
    if (!PyArg_ParseTuple(args, "s", &version)) {
        return NULL;
    }
    igs_setMappingVersion(version);
    return PyLong_FromLong(0);
}

//getMappingEntriesNumber
 PyObject * getMappingEntriesNumber_wrapper(PyObject * self, PyObject * args)
{
    int result = igs_getMappingEntriesNumber();
    return PyLong_FromLong(result);
}


//addMappingEntry
PyObject * addMappingEntry_wrapper(PyObject * self, PyObject * args)
{
    char * fromOutput;
    char * toAgent;
    char * withOutput;
    unsigned long result;
    // parse arguments : the input, the agent and the output of the entry
    if (!PyArg_ParseTuple(args, "sss", &fromOutput, &toAgent, &withOutput)) {
        return NULL;
    }
    result = igs_addMappingEntry(fromOutput, toAgent, withOutput);
    return PyLong_FromUnsignedLong(result);
}

//igs_removeMappingEntryWithId
 PyObject * removeMappingEntryWithId_wrapper(PyObject * self, PyObject * args)
{
    unsigned long theId;
    unsigned long result;
    PyObject * ret;
    // parse and cast the id of the entry
    if (!PyArg_ParseTuple(args, "i", &theId)) {
        return NULL;
    }
    result = igs_removeMappingEntryWithId(theId);
    return PyLong_FromLong(result);
}

//igs_removeMappingEntryWithName
 PyObject * removeMappingEntryWithName_wrapper(PyObject * self, PyObject * args)
{
    char * fromOurInput;
    char * toAgent;
    char * withOutput;
    int result;
    // parse arguments : the input, the agent and the output of the entry
    if (!PyArg_ParseTuple(args, "sss", &fromOurInput, &toAgent, &withOutput)) {
        return NULL;
    }
    result = igs_removeMappingEntryWithName(fromOurInput, toAgent, withOutput);
    return PyLong_FromLong(result);
}

 PyObject * setRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args)
{
    int notify;
    // parse arguments
    if (!PyArg_ParseTuple(args, "i", &notify)) {
        return NULL;
    }
    igs_setRequestOutputsFromMappedAgents(notify);
    return PyLong_FromLong(0);
}

 PyObject * getRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args)
{
    bool notify = igs_getRequestOutputsFromMappedAgents();
    // build the resulting bool into a Python object and return it
    if(notify){
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }

}
