//
//  advanced.c
//  ingescapeWrapp
//
//  Created by vaugien on 10/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "advanced.h"
#include <ingescape/ingescape_advanced.h>

PyObject * igs_setPublishingPort_wrapper(PyObject *self, PyObject *args)
{
    unsigned int port;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "i", &port)) {
        return NULL;
    }
    igs_setPublishingPort(port);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}

PyObject * igs_setDiscoveryInterval_wrapper(PyObject *self, PyObject *args)
{
    unsigned int interval;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "i", &interval)) {
        return NULL;
    }
    igs_setDiscoveryInterval(interval);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}

PyObject * igs_setAgentTimeOut_wrapper(PyObject *self, PyObject *args)
{
    unsigned int duration;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "i", &duration)) {
        return NULL;
    }
    igs_setDiscoveryInterval(duration);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}

PyObject * igs_busJoinChannel_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "s", &channel)) {
        return NULL;
    }
    igs_busJoinChannel(channel);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}

PyObject * igs_busLeaveChannel_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "s", &channel)) {
        return NULL;
    }
    igs_busLeaveChannel(channel);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}

PyObject * igs_busSendStringToChannel_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    char *msg;
    int result;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "ss", &channel, &msg)) {
        return NULL;
    }
    result = igs_busSendStringToChannel(channel, msg);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(result);
    
    return ret;
}
PyObject * igs_busSendDataToChannel_wrapper(PyObject *self, PyObject *args)
{
    char * channel;
    size_t size;
    Py_buffer buf;
    
    int  result;
    PyObject * ret;
    
    // parse and cast the channel, data and size argument given in python
    if (!PyArg_ParseTuple(args, "sy*k", &channel, &buf, &size)) {
        return NULL;
    }
    
    result = igs_busSendDataToChannel(channel, buf.buf, size);
    
    ret = PyLong_FromLong(result);
    return ret;
}


PyObject * igs_busSendStringToAgent_wrapper(PyObject *self, PyObject *args)
{
    char *agentNameOrPeerID;
    char *msg;
    int result;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "ss", &agentNameOrPeerID, &msg)) {
        return NULL;
    }
    result = igs_busSendStringToAgent(agentNameOrPeerID, msg);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(result);
    
    return ret;
}
PyObject * igs_busSendDataToAgent_wrapper(PyObject *self, PyObject *args)
{
    char * agentNameOrPeerID;
    size_t size;
    Py_buffer buf;
    
    int  result;
    PyObject * ret;
    
    // parse and cast the channel, data and size argument given in python
    if (!PyArg_ParseTuple(args, "sy*k", &agentNameOrPeerID, &buf, &size)) {
        return NULL;
    }
    
    result = igs_busSendDataToAgent(agentNameOrPeerID, buf.buf, size);
    
    ret = PyLong_FromLong(result);
    return ret;
}

PyObject * igs_busAddServiceDescription_wrapper(PyObject *self, PyObject *args)
{
    char *key;
    char *value;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "ss", &key, &value)) {
        return NULL;
    }
    igs_busAddServiceDescription(key, value);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}
PyObject * igs_busRemoveServiceDescription_wrapper(PyObject *self, PyObject *args)
{
    char *key;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "s", &key)) {
        return NULL;
    }
    igs_busRemoveServiceDescription(key);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(1);
    
    return ret;
}



