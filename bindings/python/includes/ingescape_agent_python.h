/*  =========================================================================
 * ingescape_agent_python.h - Sub agent functions
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#ifndef ingescape_agent_python_h
#define ingescape_agent_python_h

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <frameobject.h>

#include "compat.h"

#ifdef FROM_SOURCES
#include "igsagent.h"
#include "czmq.h"
#else
#include <ingescape/igsagent.h>
#include <czmq.h>
#endif


typedef struct {
    PyObject_HEAD
    igsagent_t *agent;
} AgentObject;

PyObject *Agent_activate(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_deactivate(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_is_activated(AgentObject *self, PyObject *args, PyObject *kwds);

typedef struct agentObserveCB {
    AgentObject* agent;
    PyObject* callback;
    PyObject* my_data;
    struct agentObserveCB *prev;
    struct agentObserveCB *next;
} agentObserveCB_t;
extern agentObserveCB_t* agentObserveCBList;
PyObject *Agent_observe(AgentObject *self, PyObject *args, PyObject *kwds);


PyObject *Agent_trace(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_debug(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_info(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_warn(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_error(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_fatal(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_name(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_set_name(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_family(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_set_family(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_uuid(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_state(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_set_state(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mute(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_unmute(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_is_muted(AgentObject *self, PyObject *args, PyObject *kwds);

typedef struct agentMuteCB {
    AgentObject* agent;
    PyObject* callback;
    PyObject* my_data;
    struct agentMuteCB *prev;
    struct agentMuteCB *next;
} agentMuteCB_t;
extern agentMuteCB_t *agentMuteCBList;
PyObject *Agent_observe_mute(AgentObject *self, PyObject *args, PyObject *kwds);

typedef struct agentObserveEventsCB {
    AgentObject* agent;
    PyObject* callback;
    PyObject* my_data;
    struct agentObserveEventsCB *prev;
    struct agentObserveEventsCB *next;
} agentObserveEventsCB_t;
extern agentObserveEventsCB_t *agentObserveEventsCBList;
PyObject *Agent_observe_agent_event(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_definition_load_str(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_load_file(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_clear_definition(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_json(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_description(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_version(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_set_description(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_set_version(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_create(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_create(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_create(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_remove(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_type(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_type(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_type(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_count(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_list(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_list(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_list(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_exists(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_exists(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_exists(AgentObject *self, PyObject *args, PyObject *kwds);


PyObject *Agent_input_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_output_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_parameter_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_input_set_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_set_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_set_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_set_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_set_impulsion(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_input_set_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_output_set_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_set_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_set_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_set_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_set_impulsion(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_set_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_parameter_set_bool(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_set_int(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_set_double(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_set_string(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_parameter_set_data(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject * Agent_constraints_enforce(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject * Agent_input_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject * Agent_output_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject * Agent_parameter_add_constraint(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject * Agent_input_set_description(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject * Agent_output_set_description(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject * Agent_parameter_set_description(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_clear_input(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_clear_output(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_clear_parameter(AgentObject *self, PyObject *args, PyObject *kwds);

typedef struct agentobserve_iop_cb {
    AgentObject* agent;     // Agent ref
    char *nameArg;          // name of the iop
    igs_iop_type_t iopType;
    PyObject *callback;
    PyObject *my_data;
    struct agentobserve_iop_cb *next;
    struct agentobserve_iop_cb *prev;
} agentobserve_iop_cb_t;
extern agentobserve_iop_cb_t *agentobserve_iop_cbList;
PyObject *Agent_observe_input(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_observe_output(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_observe_parameter(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_output_mute(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_unmute(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_output_is_muted(AgentObject *self, PyObject *args, PyObject *kwds);


PyObject *Agent_mapping_load_str(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_load_file(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_json(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_count(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_clear_mappings(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_clear_mappings_with_agent(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_mapping_add(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_remove_with_id(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_remove_with_name(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_split_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_split_add(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_split_remove_with_id(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_split_remove_with_name(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_mapping_outputs_request(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_set_outputs_request(AgentObject *self, PyObject *args, PyObject *kwds);


PyObject *Agent_service_call(AgentObject *self, PyObject *args, PyObject *kwds);

typedef struct agentServicesCB {
    AgentObject* agent;      // Agent ref
    const char* serviceName; // Service name
    PyObject *callback;
    PyObject *my_data;
    struct agentServicesCB *next;
    struct agentServicesCB *prev;
} agentServiceCB_t;
extern agentServiceCB_t* agentServiceCBList;

PyObject *Agent_service_call(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_init(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_arg_add(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_arg_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_add(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_names(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_has_replies(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_has_reply(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_arg_add(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_arg_remove(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_args_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_args_list(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_reply_arg_exists(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_exists(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_list(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_args_count(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_args_list(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_service_args_exists(AgentObject *self, PyObject *args, PyObject *kwds);


PyObject *Agent_election_join(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_election_leave(AgentObject *self, PyObject *args, PyObject *kwds);

PyObject *Agent_definition_set_path(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_definition_save(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_set_path(AgentObject *self, PyObject *args, PyObject *kwds);
PyObject *Agent_mapping_save(AgentObject *self, PyObject *args, PyObject *kwds);

#endif /* ingescape_agent_python_h */
