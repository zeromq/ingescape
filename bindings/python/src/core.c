/*  =========================================================================
 * core.c
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

static char *s_strndup (const char *str, size_t chars)
{
    size_t n = 0;
    char *buffer = (char *) malloc (chars + 1);
    if (buffer) {
        for (n = 0; ((n < chars) && (str[n] != 0)); n++)
            buffer[n] = str[n];
        buffer[n] = 0;
    }
    return buffer;
}

PyObject * igs_clear_context_wrapper(PyObject *self, PyObject *args)
{
    igs_clear_context();
    // Clean observes on IOs
    observe_io_cb_t *actuel = NULL;
    DL_FOREACH(observe_io_cbList, actuel)
    {
        DL_DELETE(observe_io_cbList, actuel);
        Py_CLEAR(actuel->callback);
        Py_CLEAR(actuel->my_data);
        free(actuel->nameArg);
        free(actuel);
    }
    // Clean agent event callbacks
    agentEventCallback_t *elt_event = NULL;
    DL_FOREACH(agentEventCallbackList, elt_event)
    {
        Py_CLEAR(elt_event->callback);
        Py_CLEAR(elt_event->my_data);
        free(elt_event);
    }
    // Clean monitors observe
    monitor_cb_t *elt_monitor = NULL;
    DL_FOREACH(observe_monitor_cbList, elt_monitor)
    {
        Py_CLEAR(elt_monitor->callback);
        Py_CLEAR(elt_monitor->my_data);
        free(elt_monitor);
    }
    // Clean freeze callbacks
    freezeCallback_t *elt_freeze = NULL;
    DL_FOREACH(freezeList, elt_freeze)
    {
        Py_CLEAR(elt_freeze->call);
        Py_CLEAR(elt_freeze->arglist);
        free(elt_freeze);
    }
    // Clean mute observe
    mute_cb_t *elt_mute = NULL;
    DL_FOREACH(observe_mute_cbList, elt_mute)
    {
        Py_CLEAR(elt_mute->callback);
        Py_CLEAR(elt_mute->my_data);
        free(elt_mute);
    }
    // Clean stop observe list
    stopCallback_t *elt_stop = NULL;
    DL_FOREACH(stopList, elt_stop)
    {
        Py_CLEAR(elt_stop->call);
        Py_CLEAR(elt_stop->argstopList);
        free(elt_stop);
    }
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_set_name_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_agent_set_name(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_name_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_agent_name();
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * agent_family_wrapper(PyObject * self, PyObject * args)
{
    char * family = igs_agent_family();
    if(family != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", family);
        free(family);
        family = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * agent_set_family_wrapper(PyObject * self, PyObject * args)
{
    char * family;
    if (!PyArg_ParseTuple(args, "s", &family))
        return NULL;
    igs_agent_set_family(family);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_uuid_wrapper(PyObject * self, PyObject * args)
 {
    char * uuid = igs_agent_uuid();
    if(uuid != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", uuid);
        free(uuid);
        uuid = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
 }

PyObject * agent_set_state_wrapper(PyObject * self, PyObject * args)
{
    char * state;
    if (!PyArg_ParseTuple(args, "s", &state))
        return NULL;
    igs_agent_set_state(state);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_state_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_agent_state();
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * agent_mute_wrapper(PyObject * self, PyObject * args)
{
    igs_agent_mute();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_unmute_wrapper(PyObject * self, PyObject * args)
{
    igs_agent_unmute();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * agent_is_muted_wrapper(PyObject * self, PyObject * args)
{
    if (igs_agent_is_muted())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

mute_cb_t *observe_mute_cbList = NULL;
void observe_mute_callback(bool is_muted, void *my_data)
{
    IGS_UNUSED(my_data);
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    PyObject *tupleArgs = PyTuple_New(2);
    PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", is_muted ? Py_True : Py_False));
    mute_cb_t *actuel = NULL;
    DL_FOREACH(observe_mute_cbList, actuel) {
        Py_INCREF(actuel->my_data);
        PyTuple_SetItem(tupleArgs, 1, actuel->my_data);
        call_callback(actuel->callback, tupleArgs);
        Py_XDECREF(tupleArgs);
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *observe_mute_wrapper(PyObject *self, PyObject *args)
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
    mute_cb_t *newElt = calloc(1, sizeof(mute_cb_t));
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(observe_mute_cbList, newElt);
    igs_observe_mute(observe_mute_callback, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

agentEventCallback_t *agentEventCallbackList = NULL;
void onAgentEvent(igs_agent_event_t event, const char *uuid, const char *name, const void *eventData, void *myData)
{
    agentEventCallback_t *currentCallback = NULL;
    DL_FOREACH(agentEventCallbackList, currentCallback){
        // Lock the GIL to execute the callback safely
        PyGILState_STATE d_gstate = PyGILState_Ensure();
        PyObject *globalArgList = NULL;
        Py_XINCREF(currentCallback->my_data);
        if (event == IGS_AGENT_WON_ELECTION || event == IGS_AGENT_LOST_ELECTION){
            globalArgList = PyTuple_Pack(5, PyLong_FromLong(event)
                                            , Py_BuildValue("s",uuid)
                                            , Py_BuildValue("s",name)
                                            , Py_BuildValue("s",(char*)eventData)
                                            , currentCallback->my_data);
        }else if (event == IGS_PEER_ENTERED){
            globalArgList = PyTuple_Pack(5, PyLong_FromLong(event)
                                            , Py_BuildValue("s",uuid)
                                            , Py_BuildValue("s",name)
                                            , Py_None   // FIXME: Cast zhash into python object
                                            , currentCallback->my_data);
        }else{
            globalArgList = PyTuple_Pack(5, PyLong_FromLong(event)
                                            , Py_BuildValue("s",uuid)
                                            , Py_BuildValue("s",name)
                                            , Py_None
                                            , currentCallback->my_data);
        }
        //execute the callback
        call_callback(currentCallback->callback, globalArgList);
        Py_XDECREF(globalArgList);
        //release the GIL
        PyGILState_Release(d_gstate);
    }
}

PyObject * observe_agent_events_wrapper(PyObject *self, PyObject *args)
{
    PyObject *callback;
    PyObject *my_data;
    if (PyArg_ParseTuple(args, "OO", &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    agentEventCallback_t *newElt = calloc(1, sizeof(agentEventCallback_t));
    Py_INCREF(my_data);
    newElt->my_data = my_data;
    Py_INCREF(callback);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(agentEventCallbackList, newElt);
    igs_observe_agent_events(onAgentEvent, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_constraints_enforce_wrapper(PyObject *self, PyObject *args)
{
    bool enforce = true;
    if (!PyArg_ParseTuple(args, "b", &enforce))
        return NULL;
    igs_constraints_enforce(enforce);
    return PyLong_FromLong(0);
}

typedef igs_result_t (*io_add_constraint)(const char*, const char*);
PyObject * s_io_add_constraint(PyObject *self, PyObject *args, io_add_constraint igs_api)
{
    char * name = NULL;
    char * constraint = NULL;
    if (!PyArg_ParseTuple(args, "ss", &name, &constraint))
        return NULL;
    return PyLong_FromLong(igs_api(name, constraint));
}

PyObject * igs_input_add_constraint_wrapper(PyObject *self, PyObject *args)
{
    return s_io_add_constraint(self, args, igs_input_add_constraint);
}

PyObject * igs_output_add_constraint_wrapper(PyObject *self, PyObject *args)
{
    return s_io_add_constraint(self, args, igs_output_add_constraint);
}

PyObject * igs_attribute_add_constraint_wrapper(PyObject *self, PyObject *args)
{
    return s_io_add_constraint(self, args, igs_attribute_add_constraint);
}

PyObject * igs_parameter_add_constraint_wrapper(PyObject *self, PyObject *args)
{
    return s_io_add_constraint(self, args, igs_parameter_add_constraint);
}

typedef igs_result_t (*io_set_description)(const char*, const char*);
PyObject * s_io_set_description(PyObject *self, PyObject *args, io_set_description igs_api)
{
    const char * name = NULL;
    const char * description = NULL;
    if (!PyArg_ParseTuple(args, "ss", &name, &description))
        return NULL;
    return PyLong_FromLong(igs_api(name, description));
}

PyObject * igs_input_set_description_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_description(self, args, igs_input_set_description);
}

PyObject * igs_output_set_description_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_description(self, args, igs_output_set_description);
}

PyObject * igs_attribute_set_description_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_description(self, args, igs_attribute_set_description);
}

PyObject * igs_parameter_set_description_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_description(self, args, igs_parameter_set_description);
}

typedef igs_result_t (*io_set_detailed_type)(const char*, const char*, const char*);
PyObject * s_io_set_detailed_type(PyObject *self, PyObject *args, io_set_detailed_type igs_api)
{
    const char * input_name = NULL;
    const char * type_name = NULL;
    const char * specification = NULL;
    if (!PyArg_ParseTuple(args, "sss", &input_name, &type_name, &specification))
        return NULL;
    return PyLong_FromLong(igs_api(input_name, type_name, specification));
}

PyObject * igs_input_set_detailed_type_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_detailed_type(self, args, igs_input_set_detailed_type);
}

PyObject * igs_output_set_detailed_type_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_detailed_type(self, args, igs_output_set_detailed_type);
}

//NOTE: igs_parameter_set_detailed_type is not binded because it was already obsolete (in favor of igs_attribute_set_detailed_type) when the binding was updated
PyObject * igs_attribute_set_detailed_type_wrapper(PyObject *self, PyObject *args)
{
    return s_io_set_detailed_type(self, args, igs_attribute_set_detailed_type);
}

PyObject * input_bool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_input_bool(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * input_int_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_input_int(name));
}

PyObject * input_double_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyFloat_FromDouble(igs_input_double(name));
}

PyObject * input_string_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    char * result = igs_input_string(name);
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * input_data_wrapper(PyObject * self, PyObject * args)
{
    char *name;
    void *myData;
    size_t valueSize;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_input_data(name, &myData, &valueSize);
    return Py_BuildValue("y#", myData, valueSize);
}

PyObject * input_set_bool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool value;
    if (!PyArg_ParseTuple(args, "sb", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_input_set_bool(name, value));
}

PyObject * input_set_int_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    if (!PyArg_ParseTuple(args, "si", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_input_set_int(name, value));
}

PyObject * input_set_double_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    if (!PyArg_ParseTuple(args, "sd", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_input_set_double(name, value));
}

PyObject * input_set_string_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_input_set_string(name, value));
}

PyObject * input_set_impulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_input_set_impulsion(name));
}

PyObject * input_set_data_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "sy*", &name, &buf))
        return NULL;
    PyObject* result = PyLong_FromLong(igs_input_set_data(name, buf.buf, (size_t)buf.len));
    PyBuffer_Release(&buf);
    return result;
}

PyObject * output_bool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_output_bool(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * output_int_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_output_int(name));
}

PyObject * output_double_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyFloat_FromDouble(igs_output_double(name));
}

PyObject * output_string_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    char * result = igs_output_string(name);
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * output_data_wrapper(PyObject * self, PyObject * args)
{
    char *name;
    void *myData;
    size_t valueSize;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_output_data(name, &myData, &valueSize);
    return Py_BuildValue("y#", myData, valueSize);
}

PyObject * output_set_bool_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    bool value;
    if (!PyArg_ParseTuple(args, "sb", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_output_set_bool(name, value));
}

PyObject * output_set_int_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int value;
    if (!PyArg_ParseTuple(args, "si", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_output_set_int(name, value));
}

PyObject * output_set_double_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    double value;
    if (!PyArg_ParseTuple(args, "sd", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_output_set_double(name, value));
}

PyObject * output_set_string_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    char * value;
    if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_output_set_string(name, value));
}

PyObject * output_set_impulsion_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_output_set_impulsion(name));
}

PyObject * output_set_data_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "sy*", &name, &buf))
        return NULL;
    PyObject* result = PyLong_FromLong(igs_output_set_data(name, buf.buf, (size_t)buf.len));
    PyBuffer_Release(&buf);
    return result;
}

PyObject * attribute_bool_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_attribute_bool(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * parameter_bool_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_parameter_bool(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * attribute_int_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_attribute_int(name));
}

PyObject * parameter_int_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_parameter_int(name));
}

PyObject * attribute_double_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyFloat_FromDouble(igs_attribute_double(name));
}

PyObject * parameter_double_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyFloat_FromDouble(igs_parameter_double(name));
}

typedef char* (*param_attr_string)(const char*);
PyObject * s_param_attr_string_wrapper(PyObject * self, PyObject * args, param_attr_string igs_api)
{
    const char * name;
    char * result;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    result = igs_api(name);
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * attribute_string_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_string_wrapper(self, args, igs_attribute_string);
}

PyObject * parameter_string_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_string_wrapper(self, args, igs_parameter_string);
}

typedef igs_result_t (*param_attr_data)(const char*, void**, size_t*);
PyObject * s_param_attr_data_wrapper(PyObject * self, PyObject * args, param_attr_data igs_api)
{
    const char *name;
    void *myData;
    size_t valueSize;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_api(name, &myData, &valueSize);
    return Py_BuildValue("y#", myData, &valueSize);
}

PyObject * attribute_data_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_data_wrapper(self, args, igs_attribute_data);
}

PyObject * parameter_data_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_data_wrapper(self, args, igs_parameter_data);
}

typedef igs_result_t (*param_attr_set_bool)(const char*, bool);
PyObject * s_param_attr_set_bool_wrapper(PyObject * self, PyObject * args, param_attr_set_bool igs_api)
{
    const char * name;
    bool value;
    if (!PyArg_ParseTuple(args, "sb", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_api(name, value));
}

PyObject * attribute_set_bool_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_bool_wrapper(self, args, igs_attribute_set_bool);
}

PyObject * parameter_set_bool_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_bool_wrapper(self, args, igs_parameter_set_bool);
}

typedef igs_result_t (*param_attr_set_int)(const char*, int);
PyObject * s_param_attr_set_int_wrapper(PyObject * self, PyObject * args, param_attr_set_int igs_api)
{
    const char * name;
    int value;
    if (!PyArg_ParseTuple(args, "si", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_api(name, value));
}

PyObject * attribute_set_int_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_int_wrapper(self, args, igs_attribute_set_int);
}

PyObject * parameter_set_int_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_int_wrapper(self, args, igs_parameter_set_int);
}

typedef igs_result_t (*param_attr_set_double)(const char*, double);
PyObject * s_param_attr_set_double_wrapper(PyObject * self, PyObject * args, param_attr_set_double igs_api)
{
    const char * name;
    double value;
    if (!PyArg_ParseTuple(args, "sd", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_api(name, value));
}

PyObject * attribute_set_double_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_double_wrapper(self, args, igs_attribute_set_double);
}

PyObject * parameter_set_double_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_double_wrapper(self, args, igs_parameter_set_double);
}

typedef igs_result_t (*param_attr_set_string)(const char*, const char*);
PyObject * s_param_attr_set_string_wrapper(PyObject * self, PyObject * args, param_attr_set_string igs_api)
{
    const char * name;
    const char * value;
    if (!PyArg_ParseTuple(args, "ss", &name, &value))
        return NULL;
    return PyLong_FromLong(igs_api(name, value));
}

PyObject * attribute_set_string_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_string_wrapper(self, args, igs_attribute_set_string);
}

PyObject * parameter_set_string_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_string_wrapper(self, args, igs_parameter_set_string);
}

typedef igs_result_t (*param_attr_set_data)(const char*, void*, size_t);
PyObject * s_param_attr_set_data_wrapper(PyObject * self, PyObject * args, param_attr_set_data igs_api)
{
    const char * name;
    Py_buffer buf;
    if (!PyArg_ParseTuple(args, "sy*", &name, &buf))
        return NULL;
    PyObject* result = PyLong_FromLong(igs_parameter_set_data(name, buf.buf, (size_t)buf.len));
    PyBuffer_Release(&buf);
    return result;
}

PyObject * attribute_set_data_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_data_wrapper(self, args, igs_attribute_set_data);
}

PyObject * parameter_set_data_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_set_data_wrapper(self, args, igs_parameter_set_data);
}

PyObject * clear_input_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_clear_input(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * clear_output_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_clear_output(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

typedef void (*clear_param_attr)(const char*);
PyObject * s_clear_param_attr_wrapper(PyObject * self, PyObject * args, clear_param_attr igs_api)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_api(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * clear_attribute_wrapper(PyObject * self, PyObject * args)
{
    return s_clear_param_attr_wrapper(self, args, igs_clear_attribute);
}

PyObject * clear_parameter_wrapper(PyObject * self, PyObject * args)
{
    return s_clear_param_attr_wrapper(self, args, igs_clear_parameter);
}

observe_io_cb_t *observe_io_cbList = NULL;
void observe(igs_io_type_t ioType, const char* name, igs_io_value_type_t valueType, void* value, unsigned long valueSize, void* myData){
    IGS_UNUSED(myData);
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    PyObject *tupleArgs = PyTuple_New(5);
    PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("i", ioType));
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("s", name));
    PyTuple_SetItem(tupleArgs, 2, Py_BuildValue("i", valueType));
    switch(valueType){
        case IGS_BOOL_T:
            PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("O", *(bool*)value ? Py_True : Py_False));
            break;
        case IGS_INTEGER_T:
            PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("i", *(int*)value));
            break;
        case IGS_DOUBLE_T:
            PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("d", *(double*)value));
            break;
        case IGS_STRING_T:
            PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("s", (char*)value));
            break;
        case IGS_IMPULSION_T:
            PyTuple_SetItem(tupleArgs, 3, Py_None);
            break;
        case IGS_DATA_T:
            PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("y#", value, valueSize));
            break;
        case IGS_UNKNOWN_T:
            break;
    }

    observe_io_cb_t *actuel = NULL;
    DL_FOREACH(observe_io_cbList, actuel) {
        if (streq(actuel->nameArg, name)
            && (actuel->ioType == ioType)) {
            Py_INCREF(actuel->my_data);
            PyTuple_SetItem(tupleArgs, 4, actuel->my_data);
            call_callback(actuel->callback, tupleArgs);
            Py_XDECREF(tupleArgs);
        }
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

typedef void (*observe_wrapper)(const char*, igs_io_fn, void*);
PyObject *s_observe_generic(PyObject *self, PyObject *args, igs_io_type_t ioType, observe_wrapper igs_api)
{
    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    char *ioName = NULL;
    if (PyArg_ParseTuple(args, "sOO", &ioName, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) { // check if the callback is a function
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }
    else {
        return PyLong_FromLong(IGS_FAILURE);
    }

    observe_io_cb_t *newElt = calloc(1, sizeof(observe_io_cb_t));
    newElt->ioType = ioType;
    newElt->nameArg = strdup(ioName);
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(observe_io_cbList, newElt);
    igs_api(ioName, observe, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *observe_input_wrapper(PyObject *self, PyObject *args)
{
    return s_observe_generic(self, args, IGS_INPUT_T, igs_observe_input);
}

PyObject *observe_output_wrapper(PyObject *self, PyObject *args)
{
    return s_observe_generic(self, args, IGS_OUTPUT_T, igs_observe_output);
}

PyObject *observe_attribute_wrapper(PyObject *self, PyObject *args)
{
    return s_observe_generic(self, args, IGS_ATTRIBUTE_T, igs_observe_attribute);
}

PyObject *observe_parameter_wrapper(PyObject *self, PyObject *args)
{
    return s_observe_generic(self, args, IGS_ATTRIBUTE_T, igs_observe_parameter);
}

PyObject * output_mute_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_output_mute(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * output_unmute_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    igs_output_unmute(name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * output_is_muted_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_output_is_muted(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * input_type_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_input_type(name));
}

PyObject * output_type_wrapper(PyObject * self, PyObject * args)
{
    char* name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_output_type(name));
}

typedef igs_iop_value_type_t (*param_attr_type) (const char*);
PyObject * s_param_attr_type_wrapper(PyObject * self, PyObject * args, param_attr_type igs_api)
{
    char* name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_api(name));
}

PyObject * attribute_type_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_type_wrapper(self, args, igs_attribute_type);
}

PyObject * parameter_type_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_type_wrapper(self, args, igs_parameter_type);
}

PyObject * input_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_input_count());
}

PyObject * output_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_output_count());
}

PyObject * attribute_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_attribute_count());
}

PyObject * parameter_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_parameter_count());
}

typedef char ** (*io_list)(size_t*);
PyObject * s_io_list(PyObject * self, PyObject * args, io_list igs_api)
{
    size_t nbOfElements;
    char **result = igs_api(&nbOfElements);
    PyObject * ret = PyList_New(nbOfElements);
    for (size_t i = 0; i < nbOfElements; i++){
        PyList_SetItem(ret, i, Py_BuildValue("s", result[i]));
    }
    return ret;
}

PyObject * input_list_wrapper(PyObject * self, PyObject * args)
{
    return s_io_list(self, args, igs_input_list);
}

PyObject * output_list_wrapper(PyObject * self, PyObject * args)
{
    return s_io_list(self, args, igs_output_list);
}

PyObject * attribute_list_wrapper(PyObject * self, PyObject * args)
{
    return s_io_list(self, args, igs_attribute_list);
}

PyObject * parameter_list_wrapper(PyObject * self, PyObject * args)
{
    return s_io_list(self, args, igs_parameter_list);
}

PyObject * input_exists_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_input_exists(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * output_exists_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_output_exists(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * attribute_exists_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_attribute_exists(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * parameter_exists_wrapper(PyObject * self, PyObject * args)
{
    const char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_parameter_exists(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * definition_load_str_wrapper(PyObject * self, PyObject * args)
{
    char * json_str;
    if (!PyArg_ParseTuple(args, "s", &json_str))
        return NULL;
    return PyLong_FromLong(igs_definition_load_str(json_str));
}

PyObject * definition_load_file_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    if (!PyArg_ParseTuple(args, "s", &file_path))
        return NULL;
    return PyLong_FromLong(igs_definition_load_file(file_path));
}

PyObject * clear_definition_wrapper(PyObject * self, PyObject * args)
{
    igs_clear_definition();
    observe_io_cb_t *actuel = NULL;
    DL_FOREACH(observe_io_cbList, actuel)
    {
        DL_DELETE(observe_io_cbList, actuel);
        Py_CLEAR(actuel->callback);
        Py_CLEAR(actuel->my_data);
        free(actuel->nameArg);
        free(actuel);
    }
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * definition_json_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_definition_json();
    if(result!=NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * definition_set_package_wrapper(PyObject * self, PyObject * args)
{
    const char* package ;
    if (!PyArg_ParseTuple(args, "s", &package))
        return NULL;
    igs_definition_set_package(package);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * definition_package_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_definition_package();
    if(result!=NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * definition_set_class_wrapper(PyObject * self, PyObject * args)
{
    const char* class ;
    if (!PyArg_ParseTuple(args, "s", &class))
        return NULL;
    igs_definition_set_class(class);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * definition_class_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_definition_class();
    if(result!=NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * definition_set_description_wrapper(PyObject * self, PyObject * args)
{
    char* description ;
    if (!PyArg_ParseTuple(args, "s", &description))
        return NULL;
    igs_definition_set_description(description);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * definition_description_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_definition_description();
    if(result!=NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * definition_set_version_wrapper(PyObject * self, PyObject * args)
{
    char* version ;
    if (!PyArg_ParseTuple(args, "s", &version))
        return NULL;
    igs_definition_set_version(version);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * definition_version_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_definition_version();
    if(result!=NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * input_create_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int type;
    PyObject *value;
    int result;
    if (!PyArg_ParseTuple(args, "siO", &name, &type, &value)) {
        return NULL;
    }
    if (value == Py_None)
        result = igs_input_create(name, type, NULL, 0);
    else if (type == IGS_STRING_T)
    {
        char *value_c;
        if (!PyArg_ParseTuple(args, "sis", &name, &type, &value_c))
            return NULL;
        result = igs_input_create(name, type, value_c, strlen(value_c));
    }
    else if (type == IGS_INTEGER_T)
    {
        int value_c;
        if (!PyArg_ParseTuple(args, "sii", &name, &type, &value_c))
            return NULL;
        result = igs_input_create(name, type, &value_c, sizeof(int));
    }
    else if (type == IGS_DOUBLE_T)
    {
        double value_c;
        if (!PyArg_ParseTuple(args, "sid", &name, &type, &value_c))
            return NULL;
        result = igs_input_create(name, type, &value_c, sizeof(double));
    }
    else if (type == IGS_BOOL_T)
    {
        bool value_c;
        if (!PyArg_ParseTuple(args, "sib", &name, &type, &value_c))
            return NULL;
        result = igs_input_create(name, type, &value_c, sizeof(bool));
    }
    else
    {
        result = igs_input_create(name, type, value, (size_t)PyObject_Size(value));
    }
    return PyLong_FromLong(result);
}

PyObject * output_create_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    int type;
    void *value;
    int result;
    if (!PyArg_ParseTuple(args, "siO", &name, &type, &value))
        return NULL;

    if (value == Py_None)
    {
        result = igs_output_create(name, type, NULL, 0);
    }
    else if (type == IGS_STRING_T)
    {
        char *value_c;
        if (!PyArg_ParseTuple(args, "sis", &name, &type, &value_c))
            return NULL;
        result = igs_output_create(name, type, (void*)value_c, strlen(value_c));
    }
    else if (type == IGS_INTEGER_T)
    {
        int value_c;
        if (!PyArg_ParseTuple(args, "sii", &name, &type, &value_c))
            return NULL;
        result = igs_output_create(name, type, &value_c, sizeof(int));
    }
    else if (type == IGS_DOUBLE_T)
    {
        double value_c;
        if (!PyArg_ParseTuple(args, "sid", &name, &type, &value_c))
            return NULL;
        result = igs_output_create(name, type, &value_c, sizeof(double));
    }
    else if (type == IGS_BOOL_T){
        bool value_c;
        if (!PyArg_ParseTuple(args, "sib", &name, &type, &value_c))
            return NULL;
        result = igs_output_create(name, type, &value_c, sizeof(bool));
    }
    else
    {
        result = igs_output_create(name, type, value,(size_t)PyObject_Size(value));
    }
    return PyLong_FromLong(result);
}

typedef igs_result_t (*param_attr_create)(const char*, igs_io_value_type_t, void*, unsigned long);
PyObject* s_param_attr_create_wrapper(PyObject* self, PyObject* args, param_attr_create igs_api)
{
    const char * name;
    int type;
    void *value;
    int result;
    if (!PyArg_ParseTuple(args, "siO", &name, &type, &value))
        return NULL;

    if (value == Py_None)
        result = igs_api(name, type, NULL, 0);
    else if (type == IGS_STRING_T)
    {
        const char *value_c;
        if (!PyArg_ParseTuple(args, "sis", &name, &type, &value_c))
            return NULL;
        result = igs_api(name, type, (void*)value_c, strlen(value_c));
    }
    else if (type == IGS_INTEGER_T)
    {
        int value_c;
        if (!PyArg_ParseTuple(args, "sii", &name, &type, &value_c))
            return NULL;
        result = igs_api(name, type, &value_c, sizeof(int));
    }
    else if (type == IGS_DOUBLE_T)
    {
        double value_c;
        if (!PyArg_ParseTuple(args, "sid", &name, &type, &value_c))
            return NULL;
        result = igs_api(name, type, &value_c, sizeof(double));
    }
    else if (type == IGS_BOOL_T){
        bool value_c;
        if (!PyArg_ParseTuple(args, "sib", &name, &type, &value_c))
            return NULL;
        result = igs_api(name, type, &value_c, sizeof(bool));
    }
    else
        result = igs_api(name, type, value, (size_t)PyObject_Size(value));
    return PyLong_FromLong(result);
}

PyObject * attribute_create_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_create_wrapper(self, args, igs_attribute_create);
}

PyObject * parameter_create_wrapper(PyObject * self, PyObject * args)
{
    return s_param_attr_create_wrapper(self, args, igs_parameter_create);
}

PyObject * input_remove_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    int result = igs_input_remove(name);
    if(result == IGS_SUCCESS)
    {
        observe_io_cb_t *actuel = NULL;
        DL_FOREACH(observe_io_cbList, actuel)
        {
            if (streq(actuel->nameArg, name) && (actuel->ioType == IGS_INPUT_T))
            {
                DL_DELETE(observe_io_cbList, actuel);
                Py_CLEAR(actuel->callback);
                Py_CLEAR(actuel->my_data);
                free(actuel->nameArg);
                free(actuel);
            }
        }
    }
    return PyLong_FromLong(result);
}

PyObject * output_remove_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    int result = igs_output_remove(name);
    if(result == IGS_SUCCESS)
    {
        observe_io_cb_t *actuel = NULL;
        DL_FOREACH(observe_io_cbList, actuel)
        {
            if (streq(actuel->nameArg, name)
                && (actuel->ioType == IGS_OUTPUT_T))
            {
                DL_DELETE(observe_io_cbList, actuel);
                Py_CLEAR(actuel->callback);
                Py_CLEAR(actuel->my_data);
                free(actuel->nameArg);
                free(actuel);
            }
        }
    }
    return PyLong_FromLong(result);
}

PyObject * attribute_remove_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    int result = igs_attribute_remove(name);
    if(result == IGS_SUCCESS)
    {
        observe_io_cb_t *actuel = NULL;
        DL_FOREACH(observe_io_cbList, actuel)
        {
            if (streq(actuel->nameArg, name)
                && (actuel->ioType == IGS_ATTRIBUTE_T))
            {
                DL_DELETE(observe_io_cbList, actuel);
                Py_CLEAR(actuel->callback);
                Py_CLEAR(actuel->my_data);
                free(actuel->nameArg);
                free(actuel);
            }
        }
    }
    return PyLong_FromLong(result);
}

PyObject * parameter_remove_wrapper(PyObject * self, PyObject * args)
{
    char* name ;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    int result = igs_parameter_remove(name);
    if(result == IGS_SUCCESS)
    {
        observe_io_cb_t *actuel = NULL;
        DL_FOREACH(observe_io_cbList, actuel)
        {
            if (streq(actuel->nameArg, name)
                && (actuel->ioType == IGS_ATTRIBUTE_T))
            {
                DL_DELETE(observe_io_cbList, actuel);
                Py_CLEAR(actuel->callback);
                Py_CLEAR(actuel->my_data);
                free(actuel->nameArg);
                free(actuel);
            }
        }
    }
    return PyLong_FromLong(result);
}

PyObject * mapping_load_str_wrapper(PyObject * self, PyObject * args)
{
    char * json_str;
    if (!PyArg_ParseTuple(args, "s", &json_str))
        return NULL;
    return PyLong_FromLong(igs_mapping_load_str(json_str));
}

PyObject * mapping_load_file_wrapper(PyObject * self, PyObject * args)
{
    char* file_path;
    if (!PyArg_ParseTuple(args, "s", &file_path)) {
        return NULL;
    }
    return PyLong_FromLong(igs_mapping_load_file(file_path));
}

PyObject * clear_mappings_wrapper(PyObject * self, PyObject * args)
{
    igs_clear_mappings();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * clear_mappings_for_input_wrapper(PyObject *self, PyObject *args)
{
    char *input_name = NULL;
    if (!PyArg_ParseTuple(args, "s", &input_name))
        return NULL;
    igs_clear_mappings_for_input(input_name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * clear_mappings_with_agent_wrapper(PyObject *self, PyObject *args)
{
    char *agent_name = NULL;
    if (!PyArg_ParseTuple(args, "s", &agent_name))
        return NULL;
    igs_clear_mappings_with_agent(agent_name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * mapping_json_wrapper(PyObject * self, PyObject * args)
{
    char * result = igs_mapping_json();
    if(result != NULL){
        PyObject *ret = PyUnicode_FromFormat("%s", result);
        free(result);
        result = NULL;
        return ret;
    }else
        return PyUnicode_FromFormat("");
}

PyObject * mapping_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_mapping_count());
}

PyObject * mapping_add_wrapper(PyObject * self, PyObject * args)
{
    char * fromOutput;
    char * toAgent;
    char * withOutput;
    if (!PyArg_ParseTuple(args, "sss", &fromOutput, &toAgent, &withOutput))
        return NULL;
    return PyLong_FromUnsignedLongLong((unsigned long long)igs_mapping_add(fromOutput, toAgent, withOutput));
}

PyObject * mapping_remove_with_id_wrapper(PyObject * self, PyObject * args)
{
    unsigned long long theId;
    if (!PyArg_ParseTuple(args, "K", &theId))
        return NULL;
    return PyLong_FromLong(igs_mapping_remove_with_id((uint64_t)theId));
}

PyObject * mapping_remove_with_name_wrapper(PyObject * self, PyObject * args)
{
    char * fromOurInput;
    char * toAgent;
    char * withOutput;
    if (!PyArg_ParseTuple(args, "sss", &fromOurInput, &toAgent, &withOutput))
        return NULL;
    return PyLong_FromLong(igs_mapping_remove_with_name(fromOurInput, toAgent, withOutput));
}

PyObject * split_count_wrapper(PyObject *self, PyObject *args)
{
    return PyLong_FromLong((long)igs_split_count());
}

PyObject * split_add_wrapper(PyObject *self, PyObject *args)
{
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;
    if (!PyArg_ParseTuple(args, "sss", &from_our_input, &to_agent, &with_output))
        return NULL;
    return PyLong_FromUnsignedLongLong((unsigned long long)igs_split_add(from_our_input, to_agent, with_output));

}

PyObject * split_remove_with_id_wrapper(PyObject *self, PyObject *args)
{
    unsigned long long id_mapp = 0;
    if (!PyArg_ParseTuple(args, "K", &id_mapp))
        return NULL;
    return PyLong_FromLong(igs_split_remove_with_id((uint64_t)id_mapp));
}

PyObject * split_remove_with_name_wrapper(PyObject *self, PyObject *args)
{
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;
    if (!PyArg_ParseTuple(args, "sss", &from_our_input, &to_agent, &with_output))
        return NULL;
    return PyLong_FromLong(igs_split_remove_with_name(from_our_input, to_agent, with_output));
}

PyObject * mapping_set_outputs_request_wrapper(PyObject * self, PyObject * args)
{
    int notify;
    if (!PyArg_ParseTuple(args, "i", &notify))
        return NULL;
    igs_mapping_set_outputs_request(notify);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * mapping_outputs_request_wrapper(PyObject * self, PyObject * args)
{
    if(igs_mapping_outputs_request())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * definition_set_path_wrapper(PyObject * self, PyObject * args)
{
    char * path;
    if (!PyArg_ParseTuple(args, "s", &path))
        return NULL;
    igs_definition_set_path(path);
    return PyLong_FromLong(0);
}

PyObject * mapping_set_path_wrapper(PyObject * self, PyObject * args)
{
    char * path;
    if (!PyArg_ParseTuple(args, "s", &path))
        return NULL;
    igs_mapping_set_path(path);
    return PyLong_FromLong(0);
}

PyObject * definition_save_wrapper(PyObject * self, PyObject * args)
{
    igs_definition_save();
    return PyLong_FromLong(0);
}

PyObject * mapping_save_wrapper(PyObject * self, PyObject * args)
{
    igs_mapping_save();
    return PyLong_FromLong(0);
}

PyObject * election_join_wrapper(PyObject * self, PyObject * args)
{
    char * electionName;
    if (!PyArg_ParseTuple(args, "s", &electionName)) {
        return NULL;
    }
    return PyLong_FromLong(igs_election_join(electionName));
}

PyObject * election_leave_wrapper(PyObject * self, PyObject * args)
{
    char * electionName;
    if (!PyArg_ParseTuple(args, "s", &electionName)) {
        return NULL;
    }
    return PyLong_FromLong(igs_election_leave(electionName));
}

// Real-time APIs
PyObject * rt_get_current_timestamp_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_rt_get_current_timestamp());
}

PyObject * rt_set_timestamps_wrapper(PyObject * self, PyObject * args)
{
    bool enable;
    if (!PyArg_ParseTuple(args, "b", &enable)) {
        return NULL;
    }

    igs_rt_set_timestamps(enable);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * rt_timestamps_wrapper(PyObject * self, PyObject * args)
{
    if (igs_rt_timestamps())
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

PyObject * rt_set_time_wrapper(PyObject * self, PyObject * args)
{
    long long micros = 0;
    if (!PyArg_ParseTuple(args, "L", &micros))
        return NULL;
    igs_rt_set_time((int64_t)micros);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * rt_time_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLongLong(igs_rt_time());
}

PyObject * rt_set_synchronous_mode_wrapper(PyObject * self, PyObject * args)
{
    bool enable;
    if (!PyArg_ParseTuple(args, "b", &enable)) {
        return NULL;
    }

    igs_rt_set_synchronous_mode(enable);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * rt_synchronous_mode_wrapper(PyObject * self, PyObject * args)
{
    if (igs_rt_synchronous_mode())
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}


PyObject * service_call_wrapper(PyObject * self, PyObject * args)
{
    igs_service_arg_t *argumentList = NULL;
    char* agentNameOrUUID;
    char *callName;
    char *token;
    PyObject *argTuple = NULL;

    if (PyTuple_Size(args) != 4){
        printf("Expect 4 arguments, %zu were given \n", PyTuple_Size(args));
        return NULL;
    }
    int format = 0;
    if (PyArg_ParseTuple(args, "ssOz",&agentNameOrUUID, &callName, &argTuple, &token))
    {
        if(argTuple == NULL || argTuple == Py_None)
            format = 0;
        else if(PyTuple_Check(argTuple) && PyTuple_Size(argTuple) > 0)
            format = 2;
        else if(!PyTuple_Check(argTuple))
            format = 1;
    }
    int result;
    if(format == 2)
    {
        size_t tupleArgumentSize = PyTuple_Size(argTuple);
        for (size_t index = 0; index < tupleArgumentSize; index++)
        {
            PyObject *newArgument = PyTuple_GetItem(argTuple, index);
            if(newArgument != Py_None)
            {
                if(PyLong_CheckExact(newArgument))
                    igs_service_args_add_int(&argumentList, (int)PyLong_AsLong(newArgument));
                else if(PyFloat_CheckExact(newArgument))
                    igs_service_args_add_double(&argumentList, PyFloat_AsDouble(newArgument));
                else if(PyBool_Check(newArgument))
                {
                    if(newArgument == Py_True)
                        igs_service_args_add_bool(&argumentList, true);
                    else
                        igs_service_args_add_bool(&argumentList, false);
                }
                else if(PyUnicode_Check(newArgument))
                    igs_service_args_add_string(&argumentList, PyUnicode_AsUTF8(newArgument));
                else{
                    if (PyByteArray_Check(newArgument))
                        igs_service_args_add_data(&argumentList, PyByteArray_AsString(newArgument), PyByteArray_Size(newArgument));
                    else if (PyBytes_Check(newArgument))
                        igs_service_args_add_data(&argumentList, PyBytes_AsString(newArgument), PyBytes_Size(newArgument));
                }
            }
        }
        result = igs_service_call(agentNameOrUUID, callName, &argumentList, token);
    }else if (format == 1){
        if(argTuple != Py_None){
            if(PyLong_CheckExact(argTuple))
                igs_service_args_add_int(&argumentList, (int)PyLong_AsLong(argTuple));
            else if(PyFloat_CheckExact(argTuple))
                igs_service_args_add_double(&argumentList, PyFloat_AsDouble(argTuple));
            else if(PyBool_Check(argTuple)){
                if(argTuple == Py_True)
                    igs_service_args_add_bool(&argumentList, true);
                else
                    igs_service_args_add_bool(&argumentList, false);
            }else if(PyUnicode_Check(argTuple))
                igs_service_args_add_string(&argumentList, PyUnicode_AsUTF8(argTuple));
            else{
                if (PyByteArray_Check(argTuple))
                    igs_service_args_add_data(&argumentList, PyByteArray_AsString(argTuple), PyByteArray_Size(argTuple));
                else if (PyBytes_Check(argTuple))
                    igs_service_args_add_data(&argumentList, PyBytes_AsString(argTuple), PyBytes_Size(argTuple));
            }
            result = igs_service_call(agentNameOrUUID, callName, &argumentList, token);
        }else
            result = igs_service_call(agentNameOrUUID, callName, NULL, token);
    }else
        result = igs_service_call(agentNameOrUUID, callName, NULL, token);

    return PyLong_FromLong(result);
}

callCallback_t *callList = NULL;
void observeCall(const char *senderAgentName, const char *senderAgentUUID,
             const char *callName, igs_service_arg_t *firstArgument, size_t nbArgs,
             const char *token, void* myData){
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();
    callCallback_t *actuel = NULL;
    DL_FOREACH(callList, actuel){
        if (streq(actuel->callName, callName)){
            PyObject *tupleArgs = PyTuple_New(nbArgs);
            igs_service_arg_t *currentArg = NULL;
            size_t index = 0;
            LL_FOREACH(firstArgument, currentArg){
                switch(currentArg->type){
                    case IGS_BOOL_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("O", currentArg->b ? Py_True : Py_False));
                        break;
                    case IGS_INTEGER_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("i", currentArg->i));
                        break;
                    case IGS_DOUBLE_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("d", currentArg->d));
                        break;
                    case IGS_STRING_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("s", currentArg->c));
                        break;
                    case IGS_IMPULSION_T:
                        PyTuple_SetItem(tupleArgs, index, Py_None);
                        break;
                    case IGS_DATA_T:
                        PyTuple_SetItem(tupleArgs, index, Py_BuildValue("y#", currentArg->data, currentArg->size));
                        break;
                    case IGS_UNKNOWN_T:
                        break;
                }
                index ++;
            }
            PyObject *pyAgentName = Py_BuildValue("(sssOsO)", senderAgentName, senderAgentUUID, callName, tupleArgs, token, actuel->arglist);
            call_callback(actuel->call, pyAgentName);
            Py_XDECREF(pyAgentName);
            break;
        }
    }
    PyGILState_Release(d_gstate);
}

PyObject * service_init_wrapper(PyObject *self, PyObject *args)
{
    PyObject *temp;
    PyObject *temparglist;
    PyObject *arg;
    char *callName;
    if (PyArg_ParseTuple(args, "sOO", &callName, &temp, &arg)) {
        if (!PyCallable_Check(temp)) {
            PyErr_SetString(PyExc_TypeError, "parameter must be callable");
            return NULL;
        }
    }
    Py_XINCREF(temp);
    temparglist = Py_BuildValue("O", arg);
    Py_INCREF(temparglist);
    callCallback_t *newElt = calloc(1, sizeof(callCallback_t));
    newElt->callName = s_strndup(callName, strlen(callName));
    newElt->arglist = temparglist;
    newElt->call = temp;
    DL_APPEND(callList, newElt);
    igs_result_t ret = igs_service_init(callName, observeCall, NULL);
    if (ret == IGS_SUCCESS){
        DL_APPEND(callList, newElt);
    }
    return PyLong_FromLong(ret);
}

PyObject * service_remove_wrapper(PyObject * self, PyObject * args){
    char *name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    return PyLong_FromLong(igs_service_remove(name));
}

PyObject * service_arg_add_wrapper(PyObject * self, PyObject * args){
    char *callName;
    char *argName;
    int type;
    if (!PyArg_ParseTuple(args, "ssi", &callName, &argName, &type))
        return NULL;
    return PyLong_FromLong(igs_service_arg_add(callName, argName, type));
}

PyObject * service_arg_remove_wrapper(PyObject * self, PyObject * args){
    char *callName;
    char *argName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &argName))
        return NULL;
    return PyLong_FromLong(igs_service_arg_remove(callName, argName));
}

PyObject * service_reply_add_wrapper(PyObject *self, PyObject *args)
{
    char *callName;
    char *replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;

    return PyLong_FromLong(igs_service_reply_add(callName, replyName));
}

PyObject * service_reply_remove_wrapper(PyObject *self, PyObject *args)
{
    char *callName;
    char *replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;

    return PyLong_FromLong(igs_service_reply_remove(callName, replyName));
}

PyObject * service_reply_arg_add_wrapper(PyObject *self, PyObject *args)
{
    char *callName;
    char *replyName;
    char *argName;
    int type;
    if (!PyArg_ParseTuple(args, "sssi", &callName, &replyName, &argName, &type))
        return NULL;

    return PyLong_FromLong(igs_service_reply_arg_add(callName, replyName, argName, type));
}

PyObject * service_reply_arg_remove_wrapper(PyObject *self, PyObject *args)
{
    char *callName;
    char *replyName;
    char *argName;
    if (!PyArg_ParseTuple(args, "sss", &callName, &replyName, &argName))
        return NULL;

    return PyLong_FromLong(igs_service_reply_arg_remove(callName, replyName, argName));
}

PyObject * service_count_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong((int)igs_service_count());
}

PyObject * service_exists_wrapper(PyObject * self, PyObject * args)
{
    char * name;
    if (!PyArg_ParseTuple(args, "s", &name))
        return NULL;
    if (igs_service_exists(name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * service_list_wrapper(PyObject * self, PyObject * args)
{
    size_t nbOfElements = igs_service_count();
    char **result = igs_service_list(&nbOfElements);
    PyObject * ret = PyTuple_New(nbOfElements);
    for (size_t i = 0; i < nbOfElements; i++)
        PyTuple_SetItem(ret, i, Py_BuildValue("s", result[i]));
    igs_free_services_list(result, nbOfElements);
    return ret;
}

PyObject * service_args_count_wrapper(PyObject * self, PyObject * args)
{
    char* callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;
    return PyLong_FromLong((long)igs_service_args_count(callName));
}

PyObject * service_args_list_wrapper(PyObject * self, PyObject * args)
{
    char* callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;
    igs_service_arg_t *firstElement = igs_service_args_first(callName);
    size_t nbOfElements = igs_service_args_count(callName);
    PyObject *ret = PyTuple_New(nbOfElements);
    size_t index = 0;
    igs_service_arg_t *newArg = NULL;
    LL_FOREACH(firstElement, newArg){
        PyTuple_SetItem(ret, index, Py_BuildValue("(si)",newArg->name, newArg->type));
        index ++;
    }
    return ret;
}

PyObject * service_arg_exists_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    char * argName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &argName))
        return NULL;
    if (igs_service_arg_exists(callName, argName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * service_has_replies_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;
    if (igs_service_has_replies(callName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * service_has_reply_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;
    if (igs_service_has_reply(callName, replyName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * service_reply_names_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;

    size_t nbOfElements = 0;
    char** names = igs_service_reply_names(callName, &nbOfElements);
    PyObject *ret = PyList_New(0);
    for (size_t i = 0 ; i < nbOfElements ; ++i)
        PyList_Append(ret, Py_BuildValue("s", names[i]));

    return ret;
}

PyObject * service_reply_args_count_wrapper(PyObject * self, PyObject * args)
{
    char* callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;
    return PyLong_FromLong((long)igs_service_reply_args_count(callName, replyName));
}

PyObject * service_reply_args_list_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;

    igs_service_arg_t *firstElement = igs_service_reply_args_first(callName, replyName);
    size_t nbOfElements = igs_service_reply_args_count(callName, replyName);
    PyObject *ret = PyTuple_New(nbOfElements);
    size_t index = 0;
    igs_service_arg_t *newArg = NULL;
    LL_FOREACH(firstElement, newArg){
        PyTuple_SetItem(ret, index, Py_BuildValue("(si)",newArg->name, newArg->type));
        index ++;
    }
    return ret;
}

PyObject * service_reply_arg_exists_wrapper(PyObject * self, PyObject * args)
{
    char * callName;
    char * replyName;
    char * argName;
    if (!PyArg_ParseTuple(args, "sss", &callName, &replyName, &argName))
        return NULL;

    if (igs_service_reply_arg_exists(callName, replyName, argName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
