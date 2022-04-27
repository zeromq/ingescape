/*  =========================================================================
 * channels.c
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <Python.h>

#include "ingescape_python.h"

PyObject * channel_join_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    if (!PyArg_ParseTuple(args, "s", &channel))
        return NULL;
    return PyLong_FromLong(igs_channel_join(channel));
}

PyObject * channel_leave_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    if (!PyArg_ParseTuple(args, "s", &channel)) 
        return NULL;
    igs_channel_leave(channel);
    return PyLong_FromLong(IGS_SUCCESS);
}

// FIXME: channels are to ZMQ dependent
//observe_channel_cb_t *observe_channel_list = NULL;
// PyObject * observe_channels_wrapper(PyObject *self, PyObject *args, PyObject *kwds){ }

PyObject * channel_shout_str_wrapper(PyObject *self, PyObject *args)
{
    char *channel;
    char *msg;
    if (!PyArg_ParseTuple(args, "ss", &channel, &msg))
        return NULL;
    return PyLong_FromLong(igs_channel_shout_str(channel, msg));
}

PyObject * channel_shout_data_wrapper(PyObject *self, PyObject *args)
{
    char * channel;
    size_t size;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "sy*k", &channel, &buf, &size)) 
        return NULL;
    return PyLong_FromLong(igs_channel_shout_data(channel, buf.buf, size));
}

PyObject * channel_whisper_str_wrapper(PyObject *self, PyObject *args)
{
    char *agentNameOrPeerID;
    char *msg;
    if (!PyArg_ParseTuple(args, "ss", &agentNameOrPeerID, &msg))
        return NULL;
    return PyLong_FromLong(igs_channel_whisper_str(agentNameOrPeerID, msg));
}
PyObject * channel_whisper_data_wrapper(PyObject *self, PyObject *args)
{
    char * agentNameOrPeerID;
    size_t size;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "sy*k", &agentNameOrPeerID, &buf, &size))
        return NULL;
    return PyLong_FromLong(igs_channel_whisper_data(agentNameOrPeerID, buf.buf, size));
}

PyObject * peer_add_header_wrapper(PyObject *self, PyObject *args)
{
    char *key;
    char *value;
    if (!PyArg_ParseTuple(args, "ss", &key, &value))
        return NULL;
    return PyLong_FromLong(igs_peer_add_header(key, value));
}

PyObject * peer_remove_header_wrapper(PyObject *self, PyObject *args)
{
    char *key;
    if (!PyArg_ParseTuple(args, "s", &key))
        return NULL;
    return PyLong_FromLong(igs_peer_remove_header(key));
}