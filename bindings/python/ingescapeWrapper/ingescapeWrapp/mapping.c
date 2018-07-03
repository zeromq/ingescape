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
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &json_str)) {
        return NULL;
    }

    result = igs_loadMapping(json_str);

    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_loadMappingFromPath
 PyObject * loadMappingFromPath_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }

    result = igs_loadMappingFromPath(file_path);

    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_clearMapping
 PyObject * clearMapping_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_clearMapping();

    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//igs_getMapping
 PyObject * getMapping_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    
    result = igs_getMapping();

    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getMappingName
 PyObject * getMappingName_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;
    
    result = igs_getMappingName();

    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getDefinitionDescription
 PyObject * getMappingDescription_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getMappingDescription();
    
    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}

//igs_getMappingVersion
 PyObject * getMappingVersion_wrapper(PyObject * self, PyObject * args)
{
    char * result;
    PyObject * ret;

    result = igs_getMappingVersion();

    ret = PyBytes_FromString(result);
    free(&result);
    return ret;
}


//setMappingName
 PyObject * setMappingName_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    int result;
    PyObject * ret;
    
    // parse the name of the mapping
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    result = igs_setMappingName(name);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//setMappingDescription
 PyObject * setMappingDescription_wrapper(PyObject * self, PyObject * args)
{
    char* description ;
    int result;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "s", &description)) {
        return NULL;
    }

    result = igs_setMappingDescription(description);

    ret = PyLong_FromLong(result);
    free(&result);
    
    return ret;
}

//setMappingVersion
 PyObject * setMappingVersion_wrapper(PyObject * self, PyObject * args)
{
    char* version ;
    int result;
    PyObject * ret;
    
    // parse and cast the version of the mapping
    if (!PyArg_ParseTuple(args, "s", &version)) {
        return NULL;
    }

    result = igs_setMappingVersion(version);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//getMappingEntriesNumber
 PyObject * getMappingEntriesNumber_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_getMappingEntriesNumber();
    
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//addMappingEntry
PyObject * addMappingEntry_wrapper(PyObject * self, PyObject * args)
{
    char * fromOutput;
    char * toAgent;
    char * withOutput;
    unsigned long result;
    PyObject * ret;
    
    // parse arguments : the input, the agent and the output of the entry
    if (!PyArg_ParseTuple(args, "sss", &fromOutput, &toAgent, &withOutput)) {
        return NULL;
    }

    result = igs_addMappingEntry(fromOutput, toAgent, withOutput);

    ret = PyLong_FromUnsignedLong(result);
    free(&result);
    return ret;
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
    
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_removeMappingEntryWithName
 PyObject * removeMappingEntryWithName_wrapper(PyObject * self, PyObject * args)
{
    char * fromOurInput;
    char * toAgent;
    char * withOutput;
    int result;
    PyObject * ret;
    
    // parse arguments : the input, the agent and the output of the entry
    if (!PyArg_ParseTuple(args, "sss", &fromOurInput, &toAgent, &withOutput)) {
        return NULL;
    }

    result = igs_removeMappingEntryWithName(fromOurInput, toAgent, withOutput);

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

 PyObject * setRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args)
{
    int notify;
    PyObject * ret;
    
    // parse arguments
    if (!PyArg_ParseTuple(args, "i", &notify)) {
        return NULL;
    }

    igs_setRequestOutputsFromMappedAgents(notify);

    ret = PyLong_FromLong(1);
    return ret;
}

 PyObject * getRequestOutputsFromMappedAgents_wrapper(PyObject * self, PyObject * args)
{
    bool notify;

    notify = igs_getRequestOutputsFromMappedAgents();
    
    // build the resulting bool into a Python object and return it
    if(notify){
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }

}
