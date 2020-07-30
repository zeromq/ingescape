//
//  advanced.h
//  ingescapeWrapp
//
//  Created by vaugien on 10/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef advanced_h
#define advanced_h

#include <stdio.h>
#include <Python.h>

PyDoc_STRVAR(
             setPublishingPortDoc,
             "igs_setPublishingPort(port)\n"
             "--\n"
             "\n"
             "Set the publishing port\n");

PyDoc_STRVAR(
             setDiscoveryIntervalDoc,
             "igs_setDiscoveryInterval(interval)\n"
             "--\n"
             "\n"
             "usefull only with gossip discovery\n");

PyDoc_STRVAR(
             setAgentTimeoutDoc,
             "igs_setAgentTimeout(duration)\n"
             "--\n"
             "\n"
             "usefull only with gossip discovery\n");

PyDoc_STRVAR(
             busJoinChannelDoc,
             "igs_busJoinChannel(channel)\n"
             "--\n"
             "\n"
             "Join a ZMQ channel called 'channel' and listen to messages on it \n");

PyDoc_STRVAR(
             busLeaveChannelDoc,
             "igs_busLeaveChannel(channel)\n"
             "--\n"
             "\n"
             "Leave a ZMQ channel called 'channel'\n");

PyDoc_STRVAR(
             busSendStringToChannelDoc,
             "igs_busSendStringToChannel(channel, msg)\n"
             "--\n"
             "\n"
             "Send the string 'msg' to the channel\n"
             "Return 0 if ok\n");

PyDoc_STRVAR(
             busSendDataToChannelDoc,
             "igs_busSendDataToChannel(channel, data, size)\n"
             "--\n"
             "\n"
             "Send data to the channel\n"
             "Data is a buffer containing data and size an int \n"
             "Return 0 if ok\n");

PyDoc_STRVAR(
             busSendStringToAgentDoc,
             "igs_busSendStringToAgent(channel, msg)\n"
             "--\n"
             "\n"
             "Send the string 'msg' to the agent\n"
             "Return 0 if ok\n");

PyDoc_STRVAR(
             busSendDataToAgentDoc,
             "igs_busSendDataToAgent(channel, data, size)\n"
             "--\n"
             "\n"
             "Send data to the agent\n"
             "Data is a buffer containing data and size an int \n"
             "Return 0 if ok\n");

PyDoc_STRVAR(
             busAddServiceDescriptionDoc,
             "igs_busAddServiceDescription(key, char)\n"
             "--\n"
             "\n"
             "\n");

PyDoc_STRVAR(
             busremoveServiceDescriptionDoc,
             "igs_busRemoveServiceDescription(key)\n"
             "--\n"
             "\n"
             "\n");

//network configuration
PyObject * igs_setPublishingPort_wrapper(PyObject *self, PyObject *args);
PyObject * igs_setDiscoveryInterval_wrapper(PyObject *self, PyObject *args);
PyObject * igs_setAgentTimeOut_wrapper(PyObject *self, PyObject *args);

//TODO : add zmq message
//data serialization using ZMQ
//PyObject * igs_writeOutputAsZMQMsg_wrapper(PyObject *self, PyObject *args);
//PyObject * igs_readOutputAsZMQMsg_wrapper(PyObject *self, PyObject *args);

//ZMQ internal bus
PyObject * igs_busJoinChannel_wrapper(PyObject *self, PyObject *args);
PyObject * igs_busLeaveChannel_wrapper(PyObject *self, PyObject *args);

PyObject * igs_busSendStringToChannel_wrapper(PyObject *self, PyObject *args);
PyObject * igs_busSendDataToChannel_wrapper(PyObject *self, PyObject *args);
//PyObject * igs_busSendZMQMsgToChannel_wrapper(PyObject *self, PyObject *args);

// the functions below support mutiple agents with same name
PyObject * igs_busSendStringToAgent_wrapper(PyObject *self, PyObject *args);
PyObject * igs_busSendDataToAgent_wrapper(PyObject *self, PyObject *args);
//PyObject * igs_busSendZMQMsgToAgent_wrapper(PyObject *self, PyObject *args);

PyObject * igs_busAddServiceDescription_wrapper(PyObject *self, PyObject *args);
PyObject * igs_busRemoveServiceDescription_wrapper(PyObject *self, PyObject *args);

// wrapper for igs_observeBus
//PyObject * igs_observeBus_wrapper(PyObject *self, PyObject *args);

#endif /* advanced_h */
