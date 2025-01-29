/*  =========================================================================
 * agent.c - Sub agent functions
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#include "ingescape_agent_python.h"
#include <stdio.h>
#ifdef FROM_SOURCES
#include "ingescape.h"
#else
#include <ingescape/ingescape.h>
#endif
#include "uthash/utlist.h"
#include "util.h"

PyObject *Agent_activate(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    return PyLong_FromLong(igsagent_activate(self->agent));
}

PyObject *Agent_deactivate(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    return PyLong_FromLong(igsagent_deactivate(self->agent));
}

PyObject *Agent_is_activated(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    if(igsagent_is_activated(self->agent))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

agentObserveCB_t* agentObserveCBList = NULL;
void agentObserveCB(igsagent_t *agent, bool is_activated, void *my_data)
{
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    //run through all callbacks to execute them
    PyObject *tupleArgs = PyTuple_New(3);
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("O", is_activated ? Py_True : Py_False));

    agentObserveCB_t* agentCBIt = NULL;
    DL_FOREACH(agentObserveCBList, agentCBIt) {
        if (agentCBIt->agent->agent == agent) {
            PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", agentCBIt->agent));
            Py_INCREF(agentCBIt->my_data);
            PyTuple_SetItem(tupleArgs, 2, agentCBIt->my_data);
            call_callback(agentCBIt->callback, tupleArgs);
            Py_XDECREF(tupleArgs);
        }
    }

    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *Agent_observe(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return PyLong_FromLong(IGS_FAILURE);
    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    static char *kwlist[] = {"callback", "my_data", NULL};
    if (PyArg_ParseTupleAndKeywords(args, NULL, "OO", kwlist, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }else
        return PyLong_FromLong(IGS_FAILURE);

    agentObserveCB_t* newElt = calloc(1, sizeof(agentObserveCB_t));
    newElt->agent = self;
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(agentObserveCBList, newElt);
    igsagent_observe(self->agent, agentObserveCB, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_trace(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igsagent_log(IGS_LOG_TRACE, "main", self->agent, "%s", log);
    else
        igsagent_log(IGS_LOG_DEBUG, functionStr, self->agent, "%s", log);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_debug(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;

    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igsagent_log(IGS_LOG_DEBUG, "main", self->agent, "%s", log);
    else
        igsagent_log(IGS_LOG_DEBUG, functionStr, self->agent, "%s", log);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_info(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igsagent_log(IGS_LOG_INFO, "main", self->agent, "%s", log);
    else
        igsagent_log(IGS_LOG_INFO, functionStr, self->agent, "%s", log);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_warn(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;

    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }
    Py_DECREF(funcTuple);

    if(streq(functionStr, "<module>")){
        igsagent_log(IGS_LOG_WARN, "main", self->agent, "%s", log);
    }else{
        igsagent_log(IGS_LOG_WARN, functionStr, self->agent, "%s", log);
    }
    return PyLong_FromLong(0);
}

PyObject *Agent_error(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log)) {
        return NULL;
    }

    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }

    Py_DECREF(funcTuple);

    if(streq(functionStr, "<module>")){
        igsagent_log(IGS_LOG_ERROR, "main", self->agent, "%s", log);
    }else{
        igsagent_log(IGS_LOG_ERROR, functionStr, self->agent, "%s", log);
    }
    return PyLong_FromLong(0);
}

PyObject *Agent_fatal(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log)) {
        return NULL;
    }

    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return NULL;
    }

    Py_DECREF(funcTuple);

    if(streq(functionStr, "<module>")){
        igsagent_log(IGS_LOG_FATAL, "main", self->agent, "%s", log);
    }else{
        igsagent_log(IGS_LOG_FATAL, functionStr, self->agent, "%s", log);
    }
    return PyLong_FromLong(0);
}

PyObject *Agent_name(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *name = igsagent_name(self->agent);
        if(name != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", name);
            Py_INCREF(ret);
            free(name);
            name = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_set_name(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"name",  NULL};
    char *name = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        return NULL;
    if(self->agent)
    {
        igsagent_set_name(self->agent, name);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return PyLong_FromLong(IGS_FAILURE);
}

PyObject *Agent_family(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *family = igsagent_family(self->agent);
        if(family != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", family);
            Py_INCREF(ret);
            free(family);
            family = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_set_family(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"family",  NULL};
    char *family = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &family))
        return NULL;
    if(self->agent)
    {
        igsagent_set_family(self->agent, family);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_uuid(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *uuid = igsagent_uuid(self->agent);
        if(uuid != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", uuid);
            Py_INCREF(ret);
            free(uuid);
            uuid = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_state(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *state = igsagent_state(self->agent);
        if(state != NULL){
            printf("######## %s \n", state);
            PyObject *ret = PyUnicode_FromFormat("%s", state);
            Py_INCREF(ret);
            free(state);
            state = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_set_state(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"state",  NULL};
    char *state = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &state))
        return NULL;
    if(self->agent)
    {
        igsagent_set_state(self->agent, state);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_mute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        igsagent_mute(self->agent);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_unmute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        igsagent_unmute(self->agent);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_is_muted(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    if(igsagent_is_muted(self->agent))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

agentMuteCB_t *agentMuteCBList = NULL;
void agentObserveMute(igsagent_t* agent, bool mute, void* my_data)
{
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    PyObject *tupleArgs = PyTuple_New(3);
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("O", mute ? Py_True : Py_False));
    agentMuteCB_t* agentMuteCBIt = NULL;
    DL_FOREACH(agentMuteCBList, agentMuteCBIt) {
        if (agentMuteCBIt->agent->agent == agent) {
            PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", agentMuteCBIt->agent));
            Py_INCREF(agentMuteCBIt->my_data);
            PyTuple_SetItem(tupleArgs, 2, agentMuteCBIt->my_data);
            call_callback(agentMuteCBIt->callback, tupleArgs);
            Py_XDECREF(tupleArgs);
        }
    }
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *Agent_observe_mute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return PyLong_FromLong(IGS_FAILURE);

    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    static char *kwlist[] = {"callback", "my_data", NULL};
    if (PyArg_ParseTupleAndKeywords(args, NULL, "OO", kwlist, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }
    else {
        return PyLong_FromLong(IGS_FAILURE);
    }
    agentMuteCB_t* newElt = calloc(1, sizeof(agentMuteCB_t));
    newElt->agent = self;
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(agentMuteCBList, newElt);
    igsagent_observe_mute(self->agent, agentObserveMute, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

agentObserveEventsCB_t *agentObserveEventsCBList = NULL;
void agentObserveEventsCB(igsagent_t *agent,
                          igs_agent_event_t event,
                          const char *uuid,
                          const char *name,
                          void *event_data,
                          void *data)
{
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    //run through all callbacks to execute them
    PyObject *tupleArgs = PyTuple_New(6);
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("i", event));
    PyTuple_SetItem(tupleArgs, 2, Py_BuildValue("s", uuid));
    PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("s", name));
    if (event == IGS_AGENT_WON_ELECTION || event == IGS_AGENT_LOST_ELECTION)
        PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("s", (char*)event_data));
    else if (event == IGS_PEER_ENTERED)
        PyTuple_SetItem(tupleArgs, 4,Py_None); // FIXME: cast zhash into python item
    else
        PyTuple_SetItem(tupleArgs, 4, Py_None);

    agentObserveEventsCB_t* agentEventCBIt = NULL;
    DL_FOREACH(agentObserveEventsCBList, agentEventCBIt) {
        if (agentEventCBIt->agent->agent == agent) {
            PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", agentEventCBIt->agent));
            Py_INCREF(agentEventCBIt->my_data);
            PyTuple_SetItem(tupleArgs, 5, agentEventCBIt->my_data);
            call_callback(agentEventCBIt->callback, tupleArgs);
            Py_XDECREF(tupleArgs);
        }
    }

    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *Agent_observe_agent_event(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return PyLong_FromLong(IGS_FAILURE);

    // parse the callback and arguments sent from python
    PyObject *callback;
    PyObject *my_data;
    static char *kwlist[] = {"callback", "my_data", NULL};
    if (PyArg_ParseTupleAndKeywords(args, NULL, "OO", kwlist, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) { // check if the callback is callable
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }
    else {
        return PyLong_FromLong(IGS_FAILURE);
    }

    agentObserveEventsCB_t* newElt = calloc(1, sizeof(agentObserveEventsCB_t));
    newElt->agent = self;
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(agentObserveEventsCBList, newElt);
    igsagent_observe_agent_events(self->agent, agentObserveEventsCB, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_definition_load_str(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"json_str",  NULL};
    char *json_str = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &json_str))
        Py_RETURN_NONE;
    if(self->agent)
        return PyLong_FromLong(igsagent_definition_load_str(self->agent, json_str));
    Py_RETURN_NONE;
}

PyObject *Agent_definition_load_file(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"file_path",  NULL};
    char *file_path = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &file_path))
        Py_RETURN_NONE;
    if(self->agent)
        return PyLong_FromLong(igsagent_definition_load_file(self->agent, file_path));
    Py_RETURN_NONE;
}

PyObject *Agent_clear_definition(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        igsagent_clear_definition(self->agent);
        agentobserve_io_cb_t *it = NULL;
        do {
            DL_FOREACH(agentobserve_io_cbList, it) {
                if (it->agent == self) break;
            }
            if (it) {
                DL_DELETE(agentobserve_io_cbList, it);
                Py_CLEAR(it->callback);
                Py_CLEAR(it->my_data);
                free(it);
            }
        } while(it);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    Py_RETURN_NONE;
}

PyObject *Agent_definition_json(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *def = igsagent_definition_json(self->agent);
        if(def != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", def);
            Py_INCREF(ret);
            free(def);
            def = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_definition_set_package(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"package",  NULL};
    char *package = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &package))
        Py_RETURN_NONE;
    if(self->agent)
    {
        igsagent_definition_set_package(self->agent, package);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    Py_RETURN_NONE;
}

PyObject *Agent_definition_package(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *package = igsagent_definition_package(self->agent);
        if(package != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", package);
            Py_INCREF(ret);
            free(package);
            package = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_definition_set_class(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"class",  NULL};
    char *class = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &class))
        Py_RETURN_NONE;
    if(self->agent)
    {
        igsagent_definition_set_class(self->agent, class);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    Py_RETURN_NONE;
}

PyObject *Agent_definition_class(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *class = igsagent_definition_class(self->agent);
        if(class != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", class);
            Py_INCREF(ret);
            free(class);
            class = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_definition_set_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"descritpion",  NULL};
    char *descritpion = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &descritpion))
        Py_RETURN_NONE;
    if(self->agent)
    {
        igsagent_definition_set_description(self->agent, descritpion);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    Py_RETURN_NONE;
}

PyObject *Agent_definition_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *description = igsagent_definition_description(self->agent);
        if(description != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", description);
            Py_INCREF(ret);
            free(description);
            description = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

PyObject *Agent_definition_set_version(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"version",  NULL};
    char *version = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &version))
        Py_RETURN_NONE;
    if(self->agent)
    {
        igsagent_definition_set_version(self->agent, version);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    Py_RETURN_NONE;
}

PyObject *Agent_definition_version(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *version = igsagent_definition_version(self->agent);
        if(version != NULL){
            PyObject *ret = PyUnicode_FromFormat("%s", version);
            Py_INCREF(ret);
            free(version);
            version = NULL;
            return ret;
        }else
            return PyUnicode_FromFormat("");
    }
    return NULL;
}

typedef igs_result_t (*agent_io_create_api)(igsagent_t*, const char*, igs_io_value_type_t, void*, size_t);
PyObject *s_agent_io_create(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_create_api igs_api)
{
    const char * name;
    int given_value_type;
    PyObject *value;
    int result;

    if (!PyArg_ParseTuple(args, "siO", &name, &given_value_type, &value)) {
        Py_RETURN_NONE;
    }

    igs_io_value_type_t io_value_type = (igs_io_value_type_t)(given_value_type);
    if (value == Py_None)
        result = igs_api(self->agent, name, io_value_type, NULL, 0);
    else if (io_value_type == IGS_STRING_T)
    {
        char *value_c;
        if (!PyArg_ParseTuple(args, "sis", &name, &given_value_type, &value_c))
            return NULL;
        result = igs_api(self->agent, name, io_value_type, value_c, strlen(value_c));
    }else if (io_value_type == IGS_INTEGER_T){
        int value_c;
        if (!PyArg_ParseTuple(args, "sii", &name, &given_value_type, &value_c))
            return NULL;
        result = igs_api(self->agent, name, io_value_type, &value_c, sizeof(int));
    }else if (io_value_type == IGS_DOUBLE_T){
        double value_c;
        if (!PyArg_ParseTuple(args, "sid", &name, &given_value_type, &value_c))
            return NULL;
        result = igs_api(self->agent, name, io_value_type, &value_c, sizeof(double));
    }else if (io_value_type == IGS_BOOL_T){
        bool bool_value = (value == Py_True);
        result = igs_api(self->agent, name, io_value_type, &bool_value, sizeof(bool));
    }else
        result = igs_api(self->agent, name, io_value_type, value, (size_t)PyObject_Size(value));

    return PyLong_FromLong(result);
}

PyObject *Agent_input_create(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_create(self, args, kwds, igsagent_input_create);
}

PyObject *Agent_output_create(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_create(self, args, kwds, igsagent_output_create);
}

PyObject *Agent_attribute_create(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_create(self, args, kwds, igsagent_attribute_create);
}

PyObject *Agent_parameter_create(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_create(self, args, kwds, igsagent_parameter_create);
}

typedef igs_result_t (*agent_io_remove_api)(igsagent_t*, const char*);
PyObject *s_agent_io_remove(AgentObject *self, PyObject *args, PyObject *kwds, igs_io_type_t io_type, agent_io_remove_api igs_api)
{
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        return PyLong_FromLong(IGS_FAILURE);
    agentobserve_io_cb_t *it = NULL;
    do {
        DL_FOREACH(agentobserve_io_cbList, it) {
            if (it->agent == self
                && it->nameArg == name
                && it->ioType == io_type) break;
        }
        if (it) {
            DL_DELETE(agentobserve_io_cbList, it);
            Py_CLEAR(it->callback);
            Py_CLEAR(it->my_data);
            free(it->nameArg);
            free(it);
        }
    } while(it);
    return PyLong_FromLong(igs_api(self->agent, name));
}

PyObject *Agent_input_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_remove(self, args, kwds, IGS_INPUT_T, igsagent_input_remove);
}

PyObject *Agent_output_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_remove(self, args, kwds, IGS_OUTPUT_T, igsagent_output_remove);
}

PyObject *Agent_attribute_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_remove(self, args, kwds, IGS_ATTRIBUTE_T, igsagent_attribute_remove);
}

PyObject *Agent_parameter_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_remove(self, args, kwds, IGS_ATTRIBUTE_T, igsagent_parameter_remove);
}

typedef igs_io_value_type_t (*agent_io_type)(igsagent_t*, const char*);
PyObject *s_agent_io_type(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_type igs_api)
{
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    if(self->agent)
        return PyLong_FromLong(igs_api(self->agent, name));
    Py_RETURN_NONE;
}

PyObject *Agent_input_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_type(self, args, kwds, igsagent_input_type);
}

PyObject *Agent_output_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_type(self, args, kwds, igsagent_output_type);
}

PyObject *Agent_attribute_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_type(self, args, kwds, igsagent_attribute_type);
}

PyObject *Agent_parameter_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_type(self, args, kwds, igsagent_parameter_type);
}

PyObject *Agent_input_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
        return PyLong_FromLong(igsagent_input_count(self->agent));
    Py_RETURN_NONE;
}

PyObject *Agent_output_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
        return PyLong_FromLong(igsagent_output_count(self->agent));
    Py_RETURN_NONE;
}

PyObject *Agent_attribute_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
        return PyLong_FromLong(igsagent_attribute_count(self->agent));
    Py_RETURN_NONE;
}

PyObject *Agent_parameter_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
        return PyLong_FromLong(igsagent_parameter_count(self->agent));
    Py_RETURN_NONE;
}

typedef char** (*agent_io_list)(igsagent_t*, size_t*);
PyObject *s_agent_io_list(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_list igs_api)
{
    if(!self->agent)
        return PyLong_FromLong(IGS_FAILURE);
    size_t nbOfElements;
    PyObject * ret;

    char **result = igs_api(self->agent, &nbOfElements);
    ret = PyList_New(nbOfElements);
    for (size_t i = 0; i < nbOfElements; i++)
        PyList_SetItem(ret, i, Py_BuildValue("s",result[i]));
    return ret;
}

PyObject *Agent_input_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_list(self, args, kwds, igsagent_input_list);
}

PyObject *Agent_output_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_list(self, args, kwds, igsagent_output_list);
}

PyObject *Agent_attribute_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_list(self, args, kwds, igsagent_attribute_list);
}

PyObject *Agent_parameter_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_list(self, args, kwds, igsagent_parameter_list);
}

typedef bool (*agent_io_exists)(igsagent_t*, const char*);
PyObject *s_agent_io_exists(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_exists igs_api)
{
    static char *kwlist[] = {"name",  NULL};
    char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    if(igs_api(self->agent,name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

PyObject *Agent_input_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_exists(self, args, kwds, igsagent_input_exists);
}

PyObject *Agent_output_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_exists(self, args, kwds, igsagent_output_exists);
}

PyObject *Agent_attribute_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_exists(self, args, kwds, igsagent_attribute_exists);
}

PyObject *Agent_parameter_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_exists(self, args, kwds, igsagent_parameter_exists);
}

typedef bool (*agent_io_bool)(igsagent_t*, const char*);
PyObject *s_agent_io_bool(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_bool igs_api)
{
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    if (igs_api(self->agent, name)) {
        Py_RETURN_TRUE;
    } else{
        Py_RETURN_FALSE;
    }
}

typedef int (*agent_io_int)(igsagent_t*, const char*);
PyObject *s_agent_io_int(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_int igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    return PyLong_FromLong(igs_api(self->agent, name));
}

typedef double (*agent_io_double)(igsagent_t*, const char*);
PyObject *s_agent_io_double(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_double igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    return PyFloat_FromDouble(igs_api(self->agent, name));
}

typedef char* (*agent_io_string)(igsagent_t*, const char*);
PyObject *s_agent_io_string(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_string igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    char *value = igs_api(self->agent, name);
    if(value == NULL)
        Py_RETURN_NONE;
    else{
        PyObject *ret =  PyUnicode_FromFormat("%s", value);
        free(value);
        value = NULL;
        return ret;
    }
}

typedef igs_result_t (*agent_io_data)(igsagent_t*, const char*, void**, size_t*);
PyObject *s_agent_io_data(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_data igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;

    void *my_data = NULL;
    size_t valueSize = 0;
    igs_api(self->agent, name, &my_data, &valueSize);
    if(my_data == NULL)
        Py_RETURN_NONE;
    else{
        PyObject *ret = Py_BuildValue("y#", my_data, valueSize);
        free(my_data);
        my_data = NULL;
        return ret;
    }
}

PyObject *Agent_input_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_bool(self, args, kwds, igsagent_input_bool);
}

PyObject *Agent_input_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_int(self, args, kwds, igsagent_input_int);
}

PyObject *Agent_input_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_double(self, args, kwds, igsagent_input_double);
}

PyObject *Agent_input_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_string(self, args, kwds, igsagent_input_string);
}

PyObject *Agent_input_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_data(self, args, kwds, igsagent_input_data);
}


PyObject *Agent_output_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_bool(self, args, kwds, igsagent_output_bool);
}

PyObject *Agent_output_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_int(self, args, kwds, igsagent_output_int);
}

PyObject *Agent_output_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_double(self, args, kwds, igsagent_output_double);
}

PyObject *Agent_output_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_string(self, args, kwds, igsagent_output_string);
}

PyObject *Agent_output_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_data(self, args, kwds, igsagent_output_data);
}

PyObject *Agent_attribute_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_bool(self, args, kwds, igsagent_attribute_bool);
}

PyObject *Agent_attribute_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_int(self, args, kwds, igsagent_attribute_int);
}

PyObject *Agent_attribute_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_double(self, args, kwds, igsagent_attribute_double);
}

PyObject *Agent_attribute_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_string(self, args, kwds, igsagent_attribute_string);
}

PyObject *Agent_attribute_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_data(self, args, kwds, igsagent_attribute_data);
}

PyObject *Agent_parameter_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_bool(self, args, kwds, igsagent_parameter_bool);
}

PyObject *Agent_parameter_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_int(self, args, kwds, igsagent_parameter_int);
}

PyObject *Agent_parameter_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_double(self, args, kwds, igsagent_parameter_double);
}

PyObject *Agent_parameter_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_string(self, args, kwds, igsagent_parameter_string);
}

PyObject *Agent_parameter_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_data(self, args, kwds, igsagent_parameter_data);
}

typedef igs_result_t (*agent_io_set_bool)(igsagent_t*, const char*, bool);
PyObject *s_agent_io_set_bool(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_bool igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name", "value", NULL};
    const char *name = NULL;
    int value;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sp", kwlist, &name, &value))
        Py_RETURN_NONE;

    return PyLong_FromLong(igs_api(self->agent, name, (value == 1)));
}

typedef igs_result_t (*agent_io_set_int)(igsagent_t*, const char*, int);
PyObject *s_agent_io_set_int(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_int igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name", "value", NULL};
    const char *name = NULL;
    int value;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "si", kwlist, &name, &value))
        Py_RETURN_NONE;

    return PyLong_FromLong(igs_api(self->agent, name, value));
}

typedef igs_result_t (*agent_io_set_double)(igsagent_t*, const char*, double);
PyObject *s_agent_io_set_double(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_double igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name", "value", NULL};
    char *name = NULL;
    double value;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sd", kwlist, &name, &value))
        Py_RETURN_NONE;

    return PyLong_FromLong(igs_api(self->agent, name, value));
}

typedef igs_result_t (*agent_io_set_string)(igsagent_t*, const char*, const char*);
PyObject *s_agent_io_set_string(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_string igs_api)
{
   if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name", "value", NULL};
    const char *name = NULL;
    const char *value = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "ss", kwlist, &name, &value))
        Py_RETURN_NONE;

    return PyLong_FromLong(igs_api(self->agent, name, value));
}

typedef igs_result_t (*agent_io_set_impulsion)(igsagent_t*, const char*);
PyObject *s_agent_io_set_impulsion(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_impulsion igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;

    return PyLong_FromLong(igs_api(self->agent, name));
}

typedef igs_result_t (*agent_io_set_data)(igsagent_t*, const char*, void*, size_t);
PyObject *s_agent_io_set_data(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_data igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name", "value", NULL};
    const char * name;
    Py_buffer buf;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sy*", kwlist, &name, &buf))
        Py_RETURN_NONE;

    PyObject* result = PyLong_FromLong(igs_api(self->agent, name, buf.buf, (size_t)buf.len));
    PyBuffer_Release(&buf);
    return result;
}

// input setters
PyObject *Agent_input_set_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_bool(self, args, kwds, igsagent_input_set_bool);
}

PyObject *Agent_input_set_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_int(self, args, kwds, igsagent_input_set_int);
}

PyObject *Agent_input_set_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_double(self, args, kwds, igsagent_input_set_double);
}

PyObject *Agent_input_set_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_string(self, args, kwds, igsagent_input_set_string);
}

PyObject *Agent_input_set_impulsion(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_impulsion(self, args, kwds, igsagent_input_set_impulsion);
}

PyObject *Agent_input_set_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_data(self, args, kwds, igsagent_input_set_data);
}

// output setters
PyObject *Agent_output_set_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_bool(self, args, kwds, igsagent_output_set_bool);
}

PyObject *Agent_output_set_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_int(self, args, kwds, igsagent_output_set_int);
}

PyObject *Agent_output_set_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_double(self, args, kwds, igsagent_output_set_double);
}

PyObject *Agent_output_set_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_string(self, args, kwds, igsagent_output_set_string);
}

PyObject *Agent_output_set_impulsion(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_impulsion(self, args, kwds, igsagent_output_set_impulsion);
}

PyObject *Agent_output_set_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_data(self, args, kwds, igsagent_output_set_data);
}

// attribute setters
PyObject *Agent_attribute_set_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_bool(self, args, kwds, igsagent_attribute_set_bool);
}

PyObject *Agent_attribute_set_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_int(self, args, kwds, igsagent_attribute_set_int);
}

PyObject *Agent_attribute_set_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_double(self, args, kwds, igsagent_attribute_set_double);
}

PyObject *Agent_attribute_set_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_string(self, args, kwds, igsagent_attribute_set_string);
}

PyObject *Agent_attribute_set_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_data(self, args, kwds, igsagent_attribute_set_data);
}

// parameter setters
PyObject *Agent_parameter_set_bool(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_bool(self, args, kwds, igsagent_parameter_set_bool);
}

PyObject *Agent_parameter_set_int(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_int(self, args, kwds, igsagent_parameter_set_int);
}

PyObject *Agent_parameter_set_double(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_double(self, args, kwds, igsagent_parameter_set_double);
}

PyObject *Agent_parameter_set_string(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_string(self, args, kwds, igsagent_parameter_set_string);
}

PyObject *Agent_parameter_set_data(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_data(self, args, kwds, igsagent_parameter_set_data);
}

typedef void (*agent_io_clear)(igsagent_t*, const char* name);
PyObject *s_agent_io_clear(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_clear igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    const char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    igs_api(self->agent, name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_clear_input(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_clear(self, args, kwds, igsagent_clear_input);
}

PyObject *Agent_clear_output(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_clear(self, args, kwds, igsagent_clear_output);
}

PyObject *Agent_clear_attribute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_clear(self, args, kwds, igsagent_clear_attribute);
}

PyObject *Agent_clear_parameter(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_clear(self, args, kwds, igsagent_clear_parameter);
}

agentobserve_io_cb_t *agentobserve_io_cbList = NULL;
void agent_observe(igsagent_t* agent, igs_io_type_t ioType, const char* name, igs_io_value_type_t valueType, void* value, unsigned long valueSize, void* myData){
    IGS_UNUSED(myData);
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    PyObject *tupleArgs = PyTuple_New(6);
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("i", ioType));
    PyTuple_SetItem(tupleArgs, 2, Py_BuildValue("s", name));
    PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("i", valueType));
    switch(valueType){
        case IGS_BOOL_T:
            PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("O", *(bool*)value ? Py_True : Py_False));
            break;
        case IGS_INTEGER_T:
            PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("i", *(int*)value));
            break;
        case IGS_DOUBLE_T:
            PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("d", *(double*)value));
            break;
        case IGS_STRING_T:
            PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("s", (char*)value));
            break;
        case IGS_IMPULSION_T:
            PyTuple_SetItem(tupleArgs, 4, Py_None);
            break;
        case IGS_DATA_T:
            PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("y#", value, valueSize));
            break;
        case IGS_UNKNOWN_T:
            break;
    }

    agentobserve_io_cb_t *actuel = NULL;
    DL_FOREACH(agentobserve_io_cbList, actuel) {
        if (streq(actuel->nameArg, name)
            && (actuel->agent->agent == agent)
            && (actuel->ioType == ioType)) {
            PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", actuel->agent));
            Py_INCREF(actuel->my_data);
            PyTuple_SetItem(tupleArgs, 5, actuel->my_data);
            call_callback(actuel->callback, tupleArgs);
            Py_XDECREF(tupleArgs);
        }
    }
    PyGILState_Release(d_gstate);
}

typedef void (*agent_io_observe)(igsagent_t*, const char*, igsagent_io_fn, void*);
PyObject *s_agent_io_observe(AgentObject *self, PyObject *args, PyObject *kwds, igs_io_type_t ioType, agent_io_observe igs_api)
{
    if(!self->agent)
        return PyLong_FromLong(IGS_FAILURE);

    PyObject *callback = NULL;
    PyObject *my_data = NULL;
    char *ioName = NULL;
    if (PyArg_ParseTuple(args, "sOO", &ioName, &callback, &my_data)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "'callback' parameter must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }
    else {
        return PyLong_FromLong(IGS_FAILURE);
    }
    agentobserve_io_cb_t *newElt = calloc(1, sizeof(agentobserve_io_cb_t));
    newElt->agent = self;
    newElt->ioType = ioType;
    newElt->nameArg = strdup(ioName);
    newElt->my_data = Py_BuildValue("O", my_data);
    newElt->callback = Py_BuildValue("O", callback);
    DL_APPEND(agentobserve_io_cbList, newElt);
    igs_api(self->agent, ioName, agent_observe, NULL);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_observe_input(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_observe(self, args, NULL, IGS_INPUT_T, igsagent_observe_input);
}

PyObject *Agent_observe_output(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_observe(self, args, NULL, IGS_OUTPUT_T, igsagent_observe_output);
}

PyObject *Agent_observe_attribute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_observe(self, args, NULL, IGS_ATTRIBUTE_T, igsagent_observe_attribute);
}

PyObject *Agent_observe_parameter(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_observe(self, args, NULL, IGS_ATTRIBUTE_T, igsagent_observe_parameter);
}

PyObject *Agent_output_mute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    igsagent_output_mute(self->agent, name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_output_unmute(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    igsagent_output_unmute(self->agent, name);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_output_is_muted(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"name",  NULL};
    char *name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &name))
        Py_RETURN_NONE;
    if (igsagent_output_is_muted(self->agent, name)) {
        Py_RETURN_TRUE;
    }else{
        Py_RETURN_FALSE;
    }
}

PyObject * Agent_constraints_enforce(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;

    static char *kwlist[] = {"enforce",  NULL};
    bool enforce = false;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &enforce))
        Py_RETURN_NONE;
    igsagent_constraints_enforce(self->agent, enforce);
    return PyLong_FromLong(0);
}

typedef igs_result_t (*agent_io_add_constraint)(igsagent_t*, const char*, const char*);
PyObject * s_agent_io_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_add_constraint igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;

    static char *kwlist[] = {"name", "constraint", NULL};
    const char * name = NULL;
    const char * constraint = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "ss", kwlist, &name, &constraint))
        Py_RETURN_NONE;
    igs_api(self->agent, name, constraint);
    return PyLong_FromLong(0);
}

PyObject * Agent_input_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_add_constraint(self, args, kwds, igsagent_input_add_constraint);
}

PyObject * Agent_output_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_add_constraint(self, args, kwds, igsagent_output_add_constraint);
}

PyObject * Agent_attribute_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_add_constraint(self, args, kwds, igsagent_attribute_add_constraint);
}

PyObject * Agent_parameter_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_add_constraint(self, args, kwds, igsagent_parameter_add_constraint);
}

typedef igs_result_t (*agent_io_set_description)(igsagent_t*, const char*, const char*);
PyObject * s_agent_io_set_description(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_description igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;

    static char *kwlist[] = {"name", "description", NULL};
    const char * name = NULL;
    const char * description = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "ss", kwlist, &name, &description))
        return NULL;
    igs_api(self->agent, name, description);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * Agent_input_set_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_description(self, args, kwds, igsagent_input_set_description);
}

PyObject * Agent_output_set_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_description(self, args, kwds, igsagent_output_set_description);
}

PyObject * Agent_attribute_set_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_description(self, args, kwds, igsagent_attribute_set_description);
}

PyObject * Agent_parameter_set_description(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_description(self, args, kwds, igsagent_parameter_set_description);
}

typedef igs_result_t (*agent_io_set_detailed_type)(igsagent_t*, const char*, const char*, const char*);
PyObject * s_agent_io_set_detailed_type(AgentObject *self, PyObject *args, PyObject *kwds, agent_io_set_detailed_type igs_api)
{
    if(!self->agent)
        Py_RETURN_NONE;

    static char *kwlist[] = {"input_name", "type_name", "specification", NULL};
    const char * input_name = NULL;
    const char * type_name = NULL;
    const char * specification = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sss", kwlist, &input_name, &type_name, &specification))
        return NULL;
    igs_api(self->agent, input_name, type_name, specification);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * Agent_input_set_detailed_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_detailed_type(self, args, kwds, igsagent_input_set_detailed_type);
}

PyObject * Agent_output_set_detailed_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_detailed_type(self, args, kwds, igsagent_output_set_detailed_type);
}

PyObject * Agent_attribute_set_detailed_type(AgentObject *self, PyObject *args, PyObject *kwds)
{
    return s_agent_io_set_detailed_type(self, args, kwds, igsagent_attribute_set_detailed_type);
}

//NOTE: igsagent_parameter_set_detailed_type is not binded because it was already obsolete (in favor of igsagent_attribute_set_detailed_type) when the binding was updated

PyObject *Agent_mapping_load_str(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"json_str",  NULL};
    char *json_str = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &json_str))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_mapping_load_str(self->agent, json_str));
    return NULL;
}

PyObject *Agent_mapping_load_file(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"file_path",  NULL};
    char *file_path = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &file_path))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_mapping_load_file(self->agent, file_path));
    return NULL;
}

PyObject *Agent_mapping_json(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        char *json_mapp = igsagent_mapping_json(self->agent);
        if(json_mapp == NULL)
            Py_RETURN_NONE;
        else{
            PyObject *ret = PyUnicode_FromFormat("%s", json_mapp);
            free(json_mapp);
            json_mapp = NULL;
            return ret;
        }
    }
    return NULL;
}

PyObject *Agent_mapping_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
       return PyLong_FromLong((long)igsagent_mapping_count(self->agent));
    }
    return NULL;
}

PyObject *Agent_clear_mappings(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        igsagent_clear_mappings(self->agent);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_clear_mappings_with_agent(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"agent_name",  NULL};
    char *agent_name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &agent_name))
        return NULL;
    if(self->agent){
        igsagent_clear_mappings_with_agent(self->agent, agent_name);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_clear_mappings_for_input(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"input_name",  NULL};
    const char *input_name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &input_name))
        return NULL;
    if(self->agent){
        igsagent_clear_mappings_for_input(self->agent, input_name);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_mapping_add(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"from_our_input", "to_agent", "with_output",  NULL};
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sss", kwlist, &from_our_input, &to_agent, &with_output))
        return NULL;
    if(self->agent)
        return PyLong_FromUnsignedLongLong((unsigned long long)igsagent_mapping_add(self->agent, from_our_input, to_agent, with_output));
    return NULL;
}

PyObject *Agent_mapping_remove_with_id(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"id",  NULL};
    unsigned long long id_mapp = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "K", kwlist, &id_mapp))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_mapping_remove_with_id(self->agent, (uint64_t)id_mapp));
    return NULL;
}

PyObject *Agent_mapping_remove_with_name(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    static char *kwlist[] = {"from_our_input", "to_agent", "with_output",  NULL};
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sss", kwlist, &from_our_input, &to_agent, &with_output))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_mapping_remove_with_name(self->agent, from_our_input, to_agent, with_output));
    return NULL;
}

PyObject *Agent_split_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
       return PyLong_FromLong((long)igsagent_split_count(self->agent));
    return NULL;
}

PyObject *Agent_split_add(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"from_our_input", "to_agent", "with_output",  NULL};
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sss", kwlist, &from_our_input, &to_agent, &with_output))
        return NULL;
    if(self->agent)
        return PyLong_FromUnsignedLongLong((unsigned long long)igsagent_split_add(self->agent, from_our_input, to_agent, with_output));
    return NULL;
}

PyObject *Agent_split_remove_with_id(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"id",  NULL};
    unsigned long long id_mapp = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "K", kwlist, &id_mapp))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_split_remove_with_id(self->agent, (uint64_t)id_mapp));
    return NULL;
}

PyObject *Agent_split_remove_with_name(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    static char *kwlist[] = {"from_our_input", "to_agent", "with_output",  NULL};
    char *from_our_input = NULL;
    char *to_agent = NULL;
    char *with_output = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sss", kwlist, &from_our_input, &to_agent, &with_output))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_split_remove_with_name(self->agent, from_our_input, to_agent, with_output));
    return NULL;
}

PyObject *Agent_mapping_outputs_request(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    if(igsagent_mapping_outputs_request(self->agent))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_mapping_set_outputs_request(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    static char *kwlist[] = {"notify",  NULL};
    bool notify = false;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "p", kwlist, &notify))
        return NULL;

    igsagent_mapping_set_outputs_request(self->agent, notify);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_service_call(AgentObject *self, PyObject *args, PyObject *kwds)
{
    igs_service_arg_t *argumentList = NULL;
    char* agentNameOrUUID;
    char *serviceName;
    char *token;
    PyObject *argTuple = NULL;
    if (PyTuple_Size(args) != 4){
        printf("Expect 4 arguments, %zu were given \n", PyTuple_Size(args));
        return PyLong_FromLong(-1);
    }
    int format = 0;
    static char *kwlist[] = {"agent_name","service_name","argument_list", "token", NULL};
    if (PyArg_ParseTupleAndKeywords(args, NULL, "ssOz", kwlist, &agentNameOrUUID, &serviceName, &argTuple, &token ) == true)
    {
        if(argTuple == NULL || argTuple == Py_None)
            format = 0;
        else if(PyTuple_Check(argTuple) && PyTuple_Size(argTuple) > 0)
            format = 2;
        else if(!PyTuple_Check(argTuple))
            format = 1;
    }
    else
        return PyLong_FromLong(-1);
    int result = 0;
    if(format == 2){
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
                {
                    Py_ssize_t size;
                    igs_service_args_add_string(&argumentList, PyUnicode_AsUTF8AndSize(newArgument, &size));
                }
                else
                {
                    if (PyByteArray_Check(newArgument))
                        igs_service_args_add_data(&argumentList, PyByteArray_AsString(newArgument), PyByteArray_Size(newArgument));
                    else if (PyBytes_Check(newArgument))
                        igs_service_args_add_data(&argumentList, PyBytes_AsString(newArgument), PyBytes_Size(newArgument));
                }
            }
        }
        result = igsagent_service_call(self->agent, agentNameOrUUID, serviceName, &argumentList, token);
        igs_service_args_destroy(&argumentList);
    }else if (format == 1){
        if(PyLong_CheckExact(argTuple))
            igs_service_args_add_int(&argumentList, (int)PyLong_AsLong(argTuple));
        else if(PyFloat_CheckExact(argTuple))
            igs_service_args_add_double(&argumentList, PyFloat_AsDouble(argTuple));
        else if(PyBool_Check(argTuple))
        {
            if(argTuple == Py_True)
                igs_service_args_add_bool(&argumentList, true);
            else
                igs_service_args_add_bool(&argumentList, false);
        }
        else if(PyUnicode_Check(argTuple))
        {
            Py_ssize_t size;
            igs_service_args_add_string(&argumentList, PyUnicode_AsUTF8AndSize(argTuple, &size));
        }
        else{
            if (PyByteArray_Check(argTuple))
                igs_service_args_add_data(&argumentList, PyByteArray_AsString(argTuple), PyByteArray_Size(argTuple));
            else if (PyBytes_Check(argTuple))
                igs_service_args_add_data(&argumentList, PyBytes_AsString(argTuple), PyBytes_Size(argTuple));
        }
        result = igsagent_service_call(self->agent, agentNameOrUUID, serviceName, &argumentList, token);
        igs_service_args_destroy(&argumentList);
    }else
        result = igsagent_service_call(self->agent, agentNameOrUUID, serviceName, NULL, token);

    return PyLong_FromLong(result);
}

agentServiceCB_t* agentServiceCBList = NULL;
void agentServiceCB(igsagent_t *agent,
                    const char *sender_agent_name,
                    const char *sender_agent_uuid,
                    const char *service_name,
                    igs_service_arg_t *first_argument,
                    size_t args_nbr,
                    const char *token,
                    void *data)
{
    IGS_UNUSED(data);
    // Lock the GIL to execute the callback safely
    PyGILState_STATE d_gstate;
    d_gstate = PyGILState_Ensure();

    PyObject *tupleArgs = PyTuple_New(7);
    PyTuple_SetItem(tupleArgs, 1, Py_BuildValue("s", sender_agent_name));
    PyTuple_SetItem(tupleArgs, 2, Py_BuildValue("s", sender_agent_uuid));
    PyTuple_SetItem(tupleArgs, 3, Py_BuildValue("s", service_name));

    PyObject *serviceArgsTuple = PyTuple_New(args_nbr);
    igs_service_arg_t* argIt = first_argument;
    for (size_t argIdx = 0 ; argIdx < args_nbr ; ++argIdx) {
        switch(argIt->type){
            case IGS_BOOL_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_BuildValue("O", argIt->b ? Py_True : Py_False));
                break;
            case IGS_INTEGER_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_BuildValue("i", argIt->i));
                break;
            case IGS_DOUBLE_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_BuildValue("d", argIt->d));
                break;
            case IGS_STRING_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_BuildValue("s", argIt->c));
                break;
            case IGS_IMPULSION_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_None);
                break;
            case IGS_DATA_T:
                PyTuple_SetItem(serviceArgsTuple, argIdx, Py_BuildValue("y#", argIt->data, argIt->size));
                break;
            case IGS_UNKNOWN_T:
                break;
        }
        argIt = argIt->next;
    }
    PyTuple_SetItem(tupleArgs, 4, Py_BuildValue("O", serviceArgsTuple));
    PyTuple_SetItem(tupleArgs, 5, Py_BuildValue("s", token));
    agentServiceCB_t* service_it = NULL;
    DL_FOREACH(agentServiceCBList, service_it) {
        if ((service_it->agent->agent == agent) && streq(service_it->serviceName, service_name)) {
            PyTuple_SetItem(tupleArgs, 0, Py_BuildValue("O", service_it->agent));
            PyTuple_SetItem(tupleArgs, 6, service_it->my_data);
            call_callback(service_it->callback, tupleArgs);
        }
    }
    Py_DECREF(tupleArgs);
    //release the GIL
    PyGILState_Release(d_gstate);
}

PyObject *Agent_service_init(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char* serviceName = NULL;
    PyObject *callback = NULL;
    PyObject *myData = NULL;

    if (PyTuple_Size(args) != 3){
        printf("Expect 3 arguments, %zu were given \n", PyTuple_Size(args));
        return PyLong_FromLong(IGS_FAILURE);
    }
    if (PyArg_ParseTuple(args, "sOO", &serviceName, &callback, &myData)) {
        if (!PyCallable_Check(callback)) {
            PyErr_SetString(PyExc_TypeError, "parameter 3 must be callable");
            return PyLong_FromLong(IGS_FAILURE);;
        }
    }
    igs_result_t result = igsagent_service_init(self->agent, serviceName, agentServiceCB, NULL);
    if (result == IGS_SUCCESS) {
        agentServiceCB_t *newElt = calloc(1, sizeof(agentServiceCB_t));
        newElt->agent = self;
        newElt->serviceName = strdup(serviceName);
        newElt->my_data = Py_BuildValue("O", myData);
        newElt->callback = Py_BuildValue("O", callback);
        DL_APPEND(agentServiceCBList, newElt);
    }
    return PyLong_FromLong(result);
}

PyObject *Agent_service_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"service_name",  NULL};
    char *service_name = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &service_name))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_service_remove(self->agent, service_name));
    return NULL;
}

PyObject *Agent_service_arg_add(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char *service_name;
    char *arg_name;
    int value_type;

    if (!PyArg_ParseTuple(args, "ssi", &service_name, &arg_name, &value_type))
        return NULL;

    return PyLong_FromLong(igsagent_service_arg_add(self->agent, service_name, arg_name, value_type));
}

PyObject *Agent_service_arg_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"service_name", "arg_name", NULL};
    char *service_name;
    char *arg_name;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "ss", kwlist, &service_name, &arg_name))
        return NULL;

    return PyLong_FromLong(igsagent_service_arg_remove(self->agent, service_name, arg_name));
}

PyObject *Agent_service_reply_add(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char *service_name;
    char *reply_name;
    if (!PyArg_ParseTuple(args, "ss", &service_name, &reply_name))
        return NULL;

    return PyLong_FromLong(igsagent_service_reply_add(self->agent, service_name, reply_name));
}

PyObject *Agent_service_reply_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char *service_name;
    char *reply_name;
    if (!PyArg_ParseTuple(args, "ss", &service_name, &reply_name))
        return NULL;

    return PyLong_FromLong(igsagent_service_reply_remove(self->agent, service_name, reply_name));
}

PyObject *Agent_service_reply_arg_add(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char *service_name;
    char *reply_name;
    char* arg_name;
    int type;
    if (!PyArg_ParseTuple(args, "sssi", &service_name, &reply_name, &arg_name, &type))
        return NULL;

    return PyLong_FromLong(igsagent_service_reply_arg_add(self->agent, service_name, reply_name, arg_name, type));
}

PyObject *Agent_service_reply_arg_remove(AgentObject *self, PyObject *args, PyObject *kwds)
{
    char *service_name;
    char *reply_name;
    char* arg_name;
    if (!PyArg_ParseTuple(args, "sss", &service_name, &reply_name, &arg_name))
        return NULL;

    return PyLong_FromLong(igsagent_service_reply_arg_remove(self->agent, service_name, reply_name, arg_name));
}

PyObject * Agent_service_reply_args_count(AgentObject *self, PyObject * args, PyObject *kwds)
{
    char* callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;

    return PyLong_FromLong((long)igsagent_service_reply_args_count(self->agent, callName, replyName));
}

PyObject * Agent_service_reply_args_list(AgentObject *self, PyObject * args, PyObject *kwds)
{
    char * callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;

    igs_service_arg_t *firstElement = igsagent_service_reply_args_first(self->agent, callName, replyName);
    size_t nbOfElements = igsagent_service_reply_args_count(self->agent, callName, replyName);
    PyObject *ret = PyTuple_New(nbOfElements);
    size_t index = 0;
    igs_service_arg_t *newArg = NULL;
    LL_FOREACH(firstElement, newArg){
        PyTuple_SetItem(ret, index, Py_BuildValue("(si)",newArg->name, newArg->type));
        index ++;
    }
    return ret;
}

PyObject * Agent_service_reply_arg_exists(AgentObject *self, PyObject * args, PyObject *kwds)
{
    char * callName;
    char * replyName;
    char * argName;
    if (!PyArg_ParseTuple(args, "sss", &callName, &replyName, &argName))
        return NULL;

    if (igsagent_service_reply_arg_exists(self->agent, callName, replyName, argName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * Agent_service_has_replies(AgentObject* self, PyObject * args, PyObject *kwds)
{
    char * callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;
    if (igsagent_service_has_replies(self->agent, callName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * Agent_service_has_reply(AgentObject * self, PyObject * args, PyObject *kwds)
{
    char * callName;
    char * replyName;
    if (!PyArg_ParseTuple(args, "ss", &callName, &replyName))
        return NULL;
    if (igsagent_service_has_reply(self->agent, callName, replyName))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * Agent_service_reply_names(AgentObject * self, PyObject * args, PyObject * kwds)
{
    char * callName;
    if (!PyArg_ParseTuple(args, "s", &callName))
        return NULL;

    size_t nbOfElements = 0;
    char** names = igsagent_service_reply_names(self->agent, callName, &nbOfElements);
    PyObject *ret = PyList_New(0);
    for (size_t i = 0 ; i < nbOfElements ; ++i)
        PyList_Append(ret, Py_BuildValue("s", names[i]));

    return ret;
}

PyObject *Agent_service_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
       return PyLong_FromLong((long)igsagent_service_count(self->agent));
    }
    return NULL;
}

PyObject *Agent_service_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    static char *kwlist[] = {"service_name", NULL};
    char *service_name;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &service_name))
        return NULL;
    if(igsagent_service_exists(self->agent, service_name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_service_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    PyObject * ret;

    size_t nbOfElements = igsagent_service_count(self->agent);
    char **result = igsagent_service_list(self->agent, &nbOfElements);

    ret = PyTuple_New(nbOfElements);
    size_t i ;
    for (i = 0; i < nbOfElements; i++){
        PyTuple_SetItem(ret, i, Py_BuildValue("s",result[i]));
    }
    igs_free_services_list(result, nbOfElements);
    return ret;
}

PyObject *Agent_service_args_count(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"service_name", NULL};
    char *service_name;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &service_name))
        return NULL;
    if(self->agent)
    {
       return PyLong_FromLong((long)igsagent_service_args_count(self->agent, service_name));
    }
    return NULL;
}

PyObject *Agent_service_args_list(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"service_name", NULL};
    char *service_name;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &service_name))
        return NULL;

    igs_service_arg_t *firstElement = igsagent_service_args_first(self->agent, service_name);
    size_t nbOfElements = igsagent_service_args_count(self->agent, service_name);

    PyObject *ret = PyTuple_New(nbOfElements);
    size_t index = 0;
    igs_service_arg_t *newArg = NULL;
    LL_FOREACH(firstElement, newArg){
        PyTuple_SetItem(ret, index, Py_BuildValue("(si)",newArg->name, newArg->type));
        index ++;
    }
    return ret;
}

PyObject *Agent_service_args_exists(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        return NULL;
    static char *kwlist[] = {"service_name", "arg_name", NULL};
    char *service_name;
    char *arg_name;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "ss", kwlist, &service_name, &arg_name))
        return NULL;
    if(igsagent_service_arg_exists(self->agent, service_name, arg_name))
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject *Agent_election_join(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"election_name",  NULL};
    char *election_name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &election_name))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_election_join(self->agent, election_name));
    return NULL;
}

PyObject *Agent_election_leave(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"election_name",  NULL};
    char *election_name = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &election_name))
        return NULL;
    if(self->agent)
        return PyLong_FromLong(igsagent_election_leave(self->agent, election_name));
    return NULL;
}

// Real-time APIs
PyObject * Agent_rt_get_current_timestamp(AgentObject * self, PyObject * args)
{
    return PyLong_FromLong(igsagent_rt_get_current_timestamp(self->agent));
}

PyObject * Agent_rt_set_timestamps(AgentObject * self, PyObject * args)
{
    bool enable;
    if (!PyArg_ParseTuple(args, "b", &enable)) {
        return NULL;
    }

    igsagent_rt_set_timestamps(self->agent, enable);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * Agent_rt_timestamps(AgentObject * self, PyObject * args)
{
    if (igsagent_rt_timestamps(self->agent))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

PyObject * Agent_rt_set_synchronous_mode(AgentObject * self, PyObject * args)
{
    bool enable;
    if (!PyArg_ParseTuple(args, "b", &enable)) {
        return NULL;
    }

    igsagent_rt_set_synchronous_mode(self->agent, enable);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * Agent_rt_synchronous_mode(AgentObject * self, PyObject * args)
{
    if (igsagent_rt_synchronous_mode(self->agent))
        Py_RETURN_TRUE;
    Py_RETURN_FALSE;
}

PyObject *Agent_definition_set_path(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;
    static char *kwlist[] = {"path",  NULL};
    char *path = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &path))
        Py_RETURN_NONE;
    igsagent_definition_set_path(self->agent, path);
    return PyLong_FromLong(0);
}

PyObject *Agent_definition_save(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(!self->agent)
        Py_RETURN_NONE;
    igsagent_definition_save(self->agent);
    return PyLong_FromLong(0);
}

PyObject *Agent_mapping_set_path(AgentObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"path",  NULL};
    char *path = NULL;

    if (!PyArg_ParseTupleAndKeywords(args, NULL, "s", kwlist, &path))
        return NULL;
    if(self->agent){
        igsagent_mapping_set_path(self->agent, path);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}

PyObject *Agent_mapping_save(AgentObject *self, PyObject *args, PyObject *kwds)
{
    if(self->agent)
    {
        igsagent_mapping_save(self->agent);
        return PyLong_FromLong(IGS_SUCCESS);
    }
    return NULL;
}
