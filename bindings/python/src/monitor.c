/*  =========================================================================
 * monitor.c
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
#include "uthash/utlist.h"
#include "util.h"

monitor_cb_t *observe_monitor_cbList = NULL;
void observe_monitor_callback(igs_monitor_event_t event, const char *device, const char *ip_address, void *my_data)
{
    IGS_UNUSED(my_data);
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    PyObject *tupleArgs = PyTuple_New(4);
    PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("i", event));
    PyTuple_SetItem(tupleArgs, 1, PyUnicode_DecodeLocale(device, NULL));
    PyTuple_SetItem(tupleArgs, 2, PyUnicode_DecodeLocale(ip_address, NULL));
    monitor_cb_t *actuel = NULL;
    DL_FOREACH(observe_monitor_cbList, actuel)
    {
        Py_INCREF(actuel->my_data);
        PyTuple_SetItem(tupleArgs, 3, actuel->my_data);
        call_callback(actuel->callback, tupleArgs);
        Py_XDECREF(tupleArgs);
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *igs_observe_monitor_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    if (PyArg_ParseTuple(args, "OO", &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }else {
        return PyLong_FromLong(IGS_FAILURE);
    }
    monitor_cb_t *newElt = calloc(1, sizeof(monitor_cb_t));
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = callback;
    DL_APPEND(observe_monitor_cbList, newElt);
    igs_observe_monitor(observe_monitor_callback, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_monitor_start_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"period", NULL};
    int period = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &period))
        return NULL;
    igs_monitor_start(period);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_monitor_start_with_network_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"period", "network_device", "port", NULL};
    int period = 0;
    char * network_device = NULL;
    int port = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "isi", kwlist, &period, &network_device, &port))
        return NULL;
    igs_monitor_start_with_network((unsigned int)period, network_device, (unsigned int)port);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_monitor_stop_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    igs_monitor_stop();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_monitor_is_running_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    if(igs_monitor_is_running())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * igs_monitor_set_start_stop_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"flag", NULL};
    bool flag = false;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &flag))
        return NULL;
    igs_monitor_set_start_stop(flag);
    return PyLong_FromLong(IGS_SUCCESS);
}
