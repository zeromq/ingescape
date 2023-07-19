/*  =========================================================================
 * network.c
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

PyObject * start_with_device_wrapper(PyObject * self, PyObject * args)
{
    PyObject* netDeviceUnicode;
    int port;
    if (!PyArg_ParseTuple(args, "Ui", &netDeviceUnicode, &port))
        return NULL;
    PyObject* localeEncoded = PyUnicode_EncodeLocale(netDeviceUnicode, NULL);
    Py_DECREF(netDeviceUnicode);
    char * networkDevice = PyBytes_AsString(localeEncoded);
    Py_DECREF(localeEncoded);
    return PyLong_FromLong(igs_start_with_device(networkDevice, port));
}

PyObject * start_with_ip_wrapper(PyObject * self, PyObject * args)
{
    char * ipAdresse;
    int port;
    if (!PyArg_ParseTuple(args, "si", &ipAdresse, &port))
        return NULL;
    return PyLong_FromLong(igs_start_with_ip(ipAdresse, port));
}

PyObject * stop_wrapper(PyObject * self, PyObject * args)
{
    igs_stop();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_broker_add_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"broker_endpoint",  NULL};
    char *broker_endpoint = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &broker_endpoint))
        return NULL;
    return PyLong_FromLong(igs_broker_add(broker_endpoint));
}

PyObject * igs_clear_brokers_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    igs_clear_brokers();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_broker_enable_with_endpoint_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"our_broker_endpoint",  NULL};
    char *our_broker_endpoint = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &our_broker_endpoint))
        return NULL;
    igs_broker_enable_with_endpoint(our_broker_endpoint);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_broker_set_advertized_endpoint_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"advertised_endpoint",  NULL};
    char *advertised_endpoint = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &advertised_endpoint))
        return NULL;
    igs_broker_set_advertized_endpoint(advertised_endpoint);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_start_with_brokers_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"agent_endpoint",  NULL};
    char *agent_endpoint = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &agent_endpoint))
        return NULL;
    return PyLong_FromLong(igs_start_with_brokers(agent_endpoint));
}


PyObject * igs_broker_add_secure_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"broker_endpoint", "path_to_public_certificate_for_broker",  NULL};
    char *broker_endpoint = NULL;
    char *path_to_public_certificate_for_broker = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &broker_endpoint, &path_to_public_certificate_for_broker))
        return NULL;
    return PyLong_FromLong(igs_broker_add_secure(broker_endpoint, path_to_public_certificate_for_broker));
}

PyObject * is_started_wrapper(PyObject * self, PyObject * args)
{
    if (igs_is_started())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * freeze_wrapper(PyObject * self, PyObject * args)
{
    igs_freeze();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * unfreeze_wrapper(PyObject * self, PyObject * args)
{
    igs_unfreeze();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * is_frozen_wrapper(PyObject * self, PyObject * args)
{
    if (igs_is_frozen())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

freezeCallback_t *freezeList = NULL;
void freeze(bool isPaused, void *myData){
    freezeCallback_t *actuel = NULL;
    DL_FOREACH(freezeList, actuel){
        // Lock the GIL in order to execute the callback saffely
        PyGILState_STATE d_gstate = PyGILState_Ensure();
        PyObject* args = PyTuple_New(2);
        PyTuple_SetItem(args, 0, Py_BuildValue("b", isPaused));
        Py_XINCREF(actuel->arglist);
        PyTuple_SetItem(args, 1, actuel->arglist);
        call_callback(actuel->call, actuel->arglist);
        Py_DECREF(args);
        // Release the GIL
        PyGILState_Release(d_gstate);
    }
}

PyObject * observe_freeze_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject arg;
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(temp);
    temparglist = Py_BuildValue("O", arg);
    freezeCallback_t *newElt = calloc(1, sizeof(freezeCallback_t));
    newElt->call = temp;
    newElt->arglist = temparglist;
    DL_APPEND(freezeList, newElt);
    igs_observe_freeze(freeze, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * set_command_line_wrapper(PyObject * self, PyObject * args)
{
    char * line;
    if (!PyArg_ParseTuple(args, "s", &line))
        return NULL;
    igs_set_command_line(line);
    return PyLong_FromLong(0);
}

PyObject * command_line_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    char * result = igs_command_line();
    if(result != NULL){
        PyObject * ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return Py_BuildValue("s", "");
}

PyObject * net_devices_list_wrapper(PyObject * self, PyObject * args)
{
    int nbList = 0;
    char **resultList = igs_net_devices_list(&nbList);
    PyObject *ret = PyList_New(nbList);
    for (int i = 0; i < nbList; i++)
        PyList_SetItem(ret, i, PyUnicode_DecodeLocale(resultList[i], NULL));
    igs_free_net_devices_list(resultList, nbList);
    return ret;
}

PyObject * net_addresses_list_wrapper(PyObject * self, PyObject * args)
{
    int nbList;
    char **resultList = igs_net_addresses_list(&nbList);
    PyObject *ret = PyList_New(nbList);
    for (int i = 0; i < nbList; i++)
        PyList_SetItem(ret, i, Py_BuildValue("s",resultList[i]));
    igs_free_net_addresses_list(resultList, nbList);
    return ret;
}

stopCallback_t *stopList = NULL;
void stop_callback(void *myData){
    stopCallback_t *actuel = NULL;
    DL_FOREACH(stopList, actuel){
        // Lock the GIL to execute the callback safely
        PyGILState_STATE d_gstate = PyGILState_Ensure();
        Py_XINCREF(actuel->argstopList);
        call_callback(actuel->call, actuel->argstopList);
        //release the GIL
        PyGILState_Release(d_gstate);
    }
}

PyObject * observe_forced_stop_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject arg;
    PyObject *tempargstopList;
    if (PyArg_ParseTuple(args, "OO", &temp, &arg)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(temp);
    tempargstopList = Py_BuildValue("(O)", arg);
    stopCallback_t *newElt = calloc(1, sizeof(stopCallback_t));
    newElt->argstopList = tempargstopList;
    newElt->call = temp;
    DL_APPEND(stopList, newElt);
    igs_observe_forced_stop(stop_callback, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_enable_security_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"private_certificate_file", "public_certificates_directory",  NULL};
    char *private_certificate_file = NULL;
    char *public_certificates_directory = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &private_certificate_file, &public_certificates_directory))
        return NULL;
    return PyLong_FromLong(igs_enable_security(private_certificate_file, public_certificates_directory));
}

PyObject * igs_disable_security_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    igs_disable_security();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * net_set_discovery_interval_wrapper(PyObject *self, PyObject *args)
{
    unsigned int interval;
    if (!PyArg_ParseTuple(args, "i", &interval))
        return NULL;
    igs_net_set_discovery_interval(interval);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_net_set_timeout_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"duration", NULL};
    int duration = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &duration))
        return NULL;
    igs_net_set_timeout((unsigned int)duration);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * net_set_publishing_port_wrapper(PyObject *self, PyObject *args)
{
    unsigned int port;
    if (!PyArg_ParseTuple(args, "i", &port))
        return NULL;
    igs_net_set_publishing_port(port);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_net_set_log_stream_port_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"port", NULL};
    int port = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &port))
        return NULL;
    igs_net_set_log_stream_port((unsigned int)port);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_set_ipc_dir_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
#if defined (__UNIX__)
    static char *kwlist[] = {"path", NULL};
    char * path = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &path))
        return NULL;
    igs_set_ipc_dir(path);
    return PyLong_FromLong(IGS_SUCCESS);
#endif
    return PyLong_FromLong(IGS_FAILURE);
}

PyObject * igs_ipc_dir_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
#if defined (__UNIX__)
    const char * result = igs_ipc_dir();
    if(result != NULL){
        return Py_BuildValue("s", result);
    }else{
        return Py_BuildValue("s", "");
    }
#endif
    Py_RETURN_NONE;
}

PyObject * igs_set_ipc_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"allow", NULL};
    bool allow = true;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &allow))
        return NULL;
    igs_set_ipc(allow);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_has_ipc_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    if(igs_has_ipc())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * igs_net_set_high_water_marks_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"hwm_value", NULL};
    int hwm_value = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &hwm_value))
        return NULL;
    igs_net_set_high_water_marks(hwm_value);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_net_raise_sockets_limit_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
#if defined (__UNIX__)
    igs_net_raise_sockets_limit();
    return PyLong_FromLong(IGS_SUCCESS);
#endif
    return PyLong_FromLong(IGS_FAILURE);
}

void timers_callback (int timer_id, void *my_data)
{
    if(my_data != NULL)
    {
        PyGILState_STATE d_gstate;
        d_gstate = PyGILState_Ensure();
        timer_callback_element_t *callback_elt = (timer_callback_element_t *)my_data;
        PyObject *tupleArgs = PyTuple_New(2);
        PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("i", timer_id));

        Py_INCREF(callback_elt->my_data);
        PyTuple_SetItem(tupleArgs, 1, callback_elt->my_data);
        Py_XINCREF(tupleArgs);

        call_callback(callback_elt->callback, tupleArgs);
        Py_XDECREF(tupleArgs);

        PyGILState_Release(d_gstate);
    }
}

PyObject * igs_timer_start_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"delay", "times", "callback", "my_data", NULL};
    int delay = 0;
    int times = 0;
    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    if (PyArg_ParseTupleAndKeywords(args, NULL, "iiOO", kwlist, &delay, &times, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);
        }
    }else{
        return PyLong_FromLong(IGS_FAILURE);
    }
    timer_callback_element_t *python_timer_callback = calloc(1, sizeof(timer_callback_element_t));
    python_timer_callback->my_data = Py_BuildValue("O", my_data);
    python_timer_callback->callback = Py_BuildValue("O", callback);
    Py_XINCREF(python_timer_callback->my_data);
    return PyLong_FromLong(igs_timer_start(delay, times, timers_callback, python_timer_callback));
}

PyObject * igs_timer_stop_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"timer_id", NULL};
    int timer_id = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &timer_id))
        return NULL;
    igs_timer_stop(timer_id);
    return PyLong_FromLong(IGS_SUCCESS);
}
