/*  =========================================================================
    core - core contexte and global agent API

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "ingescape_classes.h"
#include "ingescape_private.h"
#include <stdarg.h>
#include <stdio.h>

char *s_strndup (const char *str, size_t chars)
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

igs_core_context_t *core_context = NULL;
igsagent_t *core_agent = NULL;

//////////////////  CORE CONTEXT //////////////////
void core_init_context (void)
{
    if (!core_context) {
        model_read_write_lock(__FUNCTION__, __LINE__);
        core_context = (struct igs_core_context *) zmalloc (sizeof (struct igs_core_context));
        core_context->peer_headers = zhash_new();
        zhash_autofree(core_context->peer_headers);
        core_context->observed_inputs = zhashx_new();
        core_context->observed_outputs = zhashx_new();
        core_context->observed_attributes = zhashx_new();
        core_context->service_cb_wrappers = zhashx_new();
        core_context->mute_cb_wrappers = zlist_new();
        core_context->agent_event_cb_wrappers = zlist_new();
        core_context->freeze_callbacks = zlist_new();
        core_context->external_stop_calbacks = zlist_new();
        core_context->brokers = zhash_new ();
        zhash_autofree(core_context->brokers);
        core_context->monitor_callbacks = zlist_new();
        core_context->elections = zhashx_new();
        core_context->timers = zlist_new();
        core_context->zyre_peers = zhashx_new();
        core_context->zyre_callbacks = zlist_new();
        core_context->agents = zhashx_new();
        core_context->created_agents = zhashx_new ();
        core_context->remote_agents = zhashx_new ();
        core_context->splitters = zlist_new();
        // default values for context variables
        // NB: other values stay at zero / NULL until they are changed
        // by other functions.
        core_context->network_allow_ipc = true;
        core_context->network_allow_inproc = true;
        core_context->network_hwm_value = 1000;
        core_context->network_discovery_interval = 1000;
        core_context->network_agent_timeout = 8000;
        core_context->log_level = IGS_LOG_WARN;
        core_context->log_file_level = IGS_LOG_TRACE;
        core_context->log_file_max_line_length = IGS_MAX_LOG_LENGTH;
        core_context->network_shall_raise_file_descriptors_limit = true;
        core_context->network_ipc_folder_path = strdup (IGS_DEFAULT_IPC_FOLDER_PATH);
        core_context->rt_current_microseconds = INT64_MIN;
        model_read_write_unlock(__FUNCTION__, __LINE__);
    }
}

void s_core_free_observeIOP (observed_io_t **observed_io)
{
    assert (observed_io);
    assert (*observed_io);
    if ((*observed_io)->name) {
        free ((*observed_io)->name);
        (*observed_io)->name = NULL;
    }
    observe_io_cb_wrapper_t *wrapper = zlist_first((*observed_io)->observed_io_wrappers);
    while (wrapper) {
        free (wrapper);
        wrapper = zlist_next((*observed_io)->observed_io_wrappers);
    }
    zlist_destroy(&(*observed_io)->observed_io_wrappers);
    free (*observed_io);
    (*observed_io) = NULL;
}

void s_core_free_service_cb_wrapper (service_cb_wrapper_t **service_cb_wrapper)
{
    assert (service_cb_wrapper);
    assert (*service_cb_wrapper);
    assert ((*service_cb_wrapper)->name);
    free ((*service_cb_wrapper)->name);
    (*service_cb_wrapper)->name = NULL;
    free (*service_cb_wrapper);
    (*service_cb_wrapper) = NULL;
}

void igs_clear_context (void)
{
    if (!core_context)
        return;
    igs_stop ();
    igs_monitor_stop ();
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    
    zhash_destroy(&core_context->peer_headers);
    
    if (core_context->log_file){
        fclose (core_context->log_file);
        core_context->log_file = 0;
    }
    
    core_context->log_file_path[0] = '\0';
    
    observed_io_t *observed_io = zhashx_first(core_context->observed_inputs);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_inputs);
    }
    zhashx_destroy(&core_context->observed_inputs);
    
    observed_io = zhashx_first(core_context->observed_outputs);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_outputs);
    }
    zhashx_destroy(&core_context->observed_outputs);
    
    observed_io = zhashx_first(core_context->observed_attributes);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_attributes);
    }
    zhashx_destroy(&core_context->observed_attributes);
    
    service_cb_wrapper_t *service_cb_wrapper = zhashx_first(core_context->service_cb_wrappers);
    while (service_cb_wrapper) {
        s_core_free_service_cb_wrapper (&service_cb_wrapper);
        service_cb_wrapper = zhashx_next(core_context->service_cb_wrappers);
    }
    zhashx_destroy(&core_context->service_cb_wrappers);
    
    observe_mute_cb_wrapper_t *mute_cb_wrapper = zlist_first(core_context->mute_cb_wrappers);
    while (mute_cb_wrapper) {
        free (mute_cb_wrapper);
        mute_cb_wrapper = zlist_next(core_context->mute_cb_wrappers);
    }
    zlist_destroy(&core_context->mute_cb_wrappers);
    
    observe_agent_events_cb_wrapper_t *agent_event_cb_wrapper = zlist_first(core_context->agent_event_cb_wrappers);
    while (agent_event_cb_wrapper) {
        free (agent_event_cb_wrapper);
        agent_event_cb_wrapper = zlist_next(core_context->agent_event_cb_wrappers);
    }
    zlist_destroy(&core_context->agent_event_cb_wrappers);
    
    igs_freeze_wrapper_t *freeze_elt = zlist_first(core_context->freeze_callbacks);
    while (freeze_elt) {
        freeze_elt->callback_ptr = NULL;
        free(freeze_elt);
        freeze_elt = zlist_next(core_context->freeze_callbacks);
    }
    zlist_destroy(&core_context->freeze_callbacks);
    
    igs_forced_stop_wrapper_t *stop_elt = zlist_first(core_context->external_stop_calbacks);
    while (stop_elt) {
        stop_elt->callback_ptr = NULL;
        free(stop_elt);
        stop_elt = zlist_next(core_context->external_stop_calbacks);
    }
    zlist_destroy(&core_context->external_stop_calbacks);
    
    zhash_destroy (&core_context->brokers);
    
    if (core_context->advertised_endpoint){
        free(core_context->advertised_endpoint);
        core_context->advertised_endpoint = NULL;
    }
    
    if (core_context->our_broker_endpoint){
        free(core_context->our_broker_endpoint);
        core_context->our_broker_endpoint = NULL;
    }
    
    if (core_context->security_auth)
        zactor_destroy (&(core_context->security_auth));
    if (core_context->security_cert)
        zcert_destroy (&(core_context->security_cert));
    if (core_context->security_public_certificates_directory){
        free (core_context->security_public_certificates_directory);
        core_context->security_public_certificates_directory = NULL;
    }
    
    assert(core_context->monitor == NULL);
    
    igs_monitor_wrapper_t *monitor_elt = zlist_first(core_context->monitor_callbacks);
    while (monitor_elt) {
        monitor_elt->callback_ptr = NULL;
        free(monitor_elt);
        monitor_elt = zlist_next(core_context->monitor_callbacks);
    }
    zlist_destroy(&core_context->monitor_callbacks);
    
    zlist_t *election = (zlist_t *) zhashx_first (core_context->elections);
    while (election) {
        zlist_destroy (&election);
        election = zhashx_next (core_context->elections);
    }
    zhashx_destroy (&core_context->elections);
    
    if (core_context->network_device){
        free(core_context->network_device);
        core_context->network_device = NULL;
    }
    
    if (core_context->ip_address){
        free(core_context->ip_address);
        core_context->ip_address = NULL;
    }
    
    if (core_context->our_agent_endpoint){
        free(core_context->our_agent_endpoint);
        core_context->our_agent_endpoint = NULL;
    }
    
    if (core_context->command_line){
        free(core_context->command_line);
        core_context->command_line = NULL;
    }
    
    if (core_context->replay_channel){
        free(core_context->replay_channel);
        core_context->replay_channel = NULL;
    }
    
    assert(zlist_size(core_context->timers)==0);
    
    if (core_context->network_ipc_folder_path){
        free(core_context->network_ipc_folder_path);
        core_context->network_ipc_folder_path = NULL;
    }
    
    if (core_context->network_ipc_full_path){
        free(core_context->network_ipc_full_path);
        core_context->network_ipc_full_path = NULL;
    }
    
    if (core_context->network_ipc_endpoint){
        free(core_context->network_ipc_endpoint);
        core_context->network_ipc_endpoint = NULL;
    }
    
    assert(zhashx_size(core_context->zyre_peers)==0);
    
    igs_channels_wrapper_t *zyre_cb = zlist_first(core_context->zyre_callbacks);
    while (zyre_cb) {
        zyre_cb->callback_ptr = NULL;
        free(zyre_cb);
        zyre_cb = zlist_next(core_context->zyre_callbacks);
    }
    zlist_destroy(&core_context->zyre_callbacks);
    
    
    igsagent_t *a = (igsagent_t *) zhashx_first(core_context->created_agents);
    while (a && a->uuid) {
        model_read_write_unlock(__FUNCTION__, __LINE__);
        igsagent_destroy (&a);
        model_read_write_lock(__FUNCTION__, __LINE__);
        a = zhashx_next(core_context->created_agents);
    }
    zhashx_destroy(&core_context->created_agents);
    core_agent = NULL;
    
    zhashx_destroy(&core_context->agents);
    
    igs_splitter_t *splitter = zlist_first(core_context->splitters);
    while (splitter) {
        split_free_splitter(&splitter);
        splitter = zlist_next(core_context->splitters);
    }
    zlist_destroy(&core_context->splitters);
    
    assert(core_context->network_actor == NULL);
    assert(core_context->internal_pipe == NULL);
    assert(core_context->node == NULL);
    assert(core_context->publisher == NULL);
    assert(core_context->ipc_publisher == NULL);
    assert(core_context->inproc_publisher == NULL);
    assert(core_context->logger == NULL);
    assert(core_context->loop == NULL);
    
    free (core_context);
    core_context = NULL;
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

//////////////////  CORE AGENT //////////////////
void core_init_agent (void)
{
    core_init_context ();
    if (!core_agent) {
        core_agent = igsagent_new (IGS_DEFAULT_AGENT_NAME, true);
        core_agent->context = core_context;
    }
}

void igs_agent_set_name (const char *name)
{
    core_init_agent ();
    igsagent_set_name (core_agent, name);
}

char *igs_agent_name (void)
{
    core_init_agent ();
    return igsagent_name (core_agent);
}

void igs_agent_set_family (const char *family)
{
    core_init_agent ();
    igsagent_set_family (core_agent, family);
}

char *igs_agent_family (void)
{
    core_init_agent ();
    return igsagent_family (core_agent);
}

char * igs_agent_uuid (void)
{
    core_init_agent ();
    return igsagent_uuid (core_agent);
}

void igs_agent_set_state (const char *state)
{
    core_init_agent ();
    igsagent_set_state (core_agent, state);
}

char *igs_agent_state (void)
{
    core_init_agent ();
    return igsagent_state (core_agent);
}

void igs_agent_mute (void)
{
    core_init_agent ();
    igsagent_mute (core_agent);
}

void igs_agent_unmute (void)
{
    core_init_agent ();
    igsagent_unmute (core_agent);
}

bool igs_agent_is_muted (void)
{
    core_init_agent ();
    return igsagent_is_muted (core_agent);
}

void core_observe_mute_callback (igsagent_t *agent,
                                 bool is_muted,
                                 void *my_data)
{
    IGS_UNUSED (agent)
    observe_mute_cb_wrapper_t *wrap = (observe_mute_cb_wrapper_t *) my_data;
    wrap->cb (is_muted, wrap->my_data);
}

void igs_observe_mute (igs_mute_fn cb, void *my_data)
{
    assert (cb);
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    observe_mute_cb_wrapper_t *wrap = (observe_mute_cb_wrapper_t *) zmalloc (sizeof (observe_mute_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    zlist_append(core_context->mute_cb_wrappers, wrap);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_observe_mute (core_agent, core_observe_mute_callback, wrap);
}

void core_observe_agent_events_callback (igsagent_t *agent,
                                         igs_agent_event_t event,
                                         const char *uuid,
                                         const char *name,
                                         void *event_data,
                                         void *my_data)
{
    IGS_UNUSED (agent)
    observe_agent_events_cb_wrapper_t *wrap = (observe_agent_events_cb_wrapper_t *) my_data;
    wrap->cb (event, uuid, name, event_data, wrap->my_data);
}

void igs_observe_agent_events (igs_agent_events_fn cb, void *my_data)
{
    assert (cb);
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    observe_agent_events_cb_wrapper_t *wrap = (observe_agent_events_cb_wrapper_t *) zmalloc (sizeof (observe_agent_events_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    zlist_append(core_context->agent_event_cb_wrappers, wrap);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_observe_agent_events (core_agent, core_observe_agent_events_callback, wrap);
}

// IOP
void igs_constraints_enforce (bool enforce)
{
    core_init_agent ();
    core_agent->enforce_constraints = enforce;
}

bool igs_input_bool (const char *name)
{
    core_init_agent ();
    return igsagent_input_bool (core_agent, name);
}

int igs_input_int (const char *name)
{
    core_init_agent ();
    return igsagent_input_int (core_agent, name);
}

double igs_input_double (const char *name)
{
    core_init_agent ();
    return igsagent_input_double (core_agent, name);
}

char *igs_input_string (const char *name)
{
    core_init_agent ();
    return igsagent_input_string (core_agent, name);
}

igs_result_t igs_input_data (const char *name, void **data, size_t *size)
{
    core_init_agent ();
    return igsagent_input_data (core_agent, name, data, size);
}

bool igs_output_bool (const char *name)
{
    core_init_agent ();
    return igsagent_output_bool (core_agent, name);
}

int igs_output_int (const char *name)
{
    core_init_agent ();
    return igsagent_output_int (core_agent, name);
}

double igs_output_double (const char *name)
{
    core_init_agent ();
    return igsagent_output_double (core_agent, name);
}

char *igs_output_string (const char *name)
{
    core_init_agent ();
    return igsagent_output_string (core_agent, name);
}

igs_result_t igs_output_data (const char *name, void **data, size_t *size)
{
    core_init_agent ();
    return igsagent_output_data (core_agent, name, data, size);
}

bool igs_attribute_bool (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_bool (core_agent, name);
}

int igs_attribute_int (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_int (core_agent, name);
}

double igs_attribute_double (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_double (core_agent, name);
}

char *igs_attribute_string (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_string (core_agent, name);
}

igs_result_t igs_attribute_data (const char *name, void **data, size_t *size)
{
    core_init_agent ();
    return igsagent_attribute_data (core_agent, name, data, size);
}

igs_result_t igs_input_set_bool (const char *name, bool value)
{
    core_init_agent ();
    return igsagent_input_set_bool (core_agent, name, value);
}

igs_result_t igs_input_set_int (const char *name, int value)
{
    core_init_agent ();
    return igsagent_input_set_int (core_agent, name, value);
}

igs_result_t igs_input_set_double (const char *name, double value)
{
    core_init_agent ();
    return igsagent_input_set_double (core_agent, name, value);
}

igs_result_t igs_input_set_string (const char *name, const char *value)
{
    core_init_agent ();
    return igsagent_input_set_string (core_agent, name, value);
}

igs_result_t igs_input_set_impulsion (const char *name)
{
    core_init_agent ();
    return igsagent_input_set_impulsion (core_agent, name);
}

igs_result_t igs_input_set_data (const char *name, void *value, size_t size)
{
    core_init_agent ();
    return igsagent_input_set_data (core_agent, name, value, size);
}

igs_result_t igs_output_set_bool (const char *name, bool value)
{
    core_init_agent ();
    return igsagent_output_set_bool (core_agent, name, value);
}

igs_result_t igs_output_set_int (const char *name, int value)
{
    core_init_agent ();
    return igsagent_output_set_int (core_agent, name, value);
}

igs_result_t igs_output_set_double (const char *name, double value)
{
    core_init_agent ();
    return igsagent_output_set_double (core_agent, name, value);
}

igs_result_t igs_output_set_string (const char *name, const char *value)
{
    core_init_agent ();
    return igsagent_output_set_string (core_agent, name, value);
}

igs_result_t igs_output_set_impulsion (const char *name)
{
    core_init_agent ();
    return igsagent_output_set_impulsion (core_agent, name);
}

igs_result_t igs_output_set_data (const char *name, void *value, size_t size)
{
    core_init_agent ();
    return igsagent_output_set_data (core_agent, name, value, size);
}

igs_result_t igs_attribute_set_bool (const char *name, bool value)
{
    core_init_agent ();
    return igsagent_attribute_set_bool (core_agent, name, value);
}

igs_result_t igs_attribute_set_int (const char *name, int value)
{
    core_init_agent ();
    return igsagent_attribute_set_int (core_agent, name, value);
}

igs_result_t igs_attribute_set_double (const char *name, double value)
{
    core_init_agent ();
    return igsagent_attribute_set_double (core_agent, name, value);
}

igs_result_t igs_attribute_set_string (const char *name, const char *value)
{
    core_init_agent ();
    return igsagent_attribute_set_string (core_agent, name, value);
}

igs_result_t igs_attribute_set_data (const char *name, void *value, size_t size)
{
    core_init_agent ();
    return igsagent_attribute_set_data (core_agent, name, value, size);
}

igs_result_t igs_input_add_constraint (const char *name, const char *constraint)
{
    core_init_agent ();
    return igsagent_input_add_constraint (core_agent, name, constraint);
}

igs_result_t igs_output_add_constraint (const char *name, const char *constraint)
{
    core_init_agent ();
    return igsagent_output_add_constraint (core_agent, name, constraint);
}

igs_result_t igs_attribute_add_constraint (const char *name, const char *constraint)
{
    core_init_agent ();
    return igsagent_attribute_add_constraint (core_agent, name, constraint);
}

igs_result_t igs_input_set_description(const char *name, const char *description)
{
    core_init_agent ();
    return igsagent_input_set_description (core_agent, name, description);
}

igs_result_t igs_output_set_description(const char *name, const char *description)
{
    core_init_agent ();
    return igsagent_output_set_description(core_agent, name, description);
}

igs_result_t igs_attribute_set_description(const char *name, const char *description)
{
    core_init_agent ();
    return igsagent_attribute_set_description (core_agent, name, description);
}

igs_result_t igs_input_set_detailed_type(const char *name, const char *type_name, const char *specification)
{
    core_init_agent ();
    return igsagent_input_set_detailed_type(core_agent, name, type_name, specification);
}

igs_result_t igs_output_set_detailed_type(const char *name, const char *type_name, const char *specification)
{
    core_init_agent ();
    return igsagent_output_set_detailed_type(core_agent, name, type_name, specification);
}

igs_result_t igs_attribute_set_detailed_type(const char *name, const char *type_name, const char *specification)
{
    core_init_agent ();
    return igsagent_attribute_set_detailed_type(core_agent, name, type_name, specification);
}

void igs_clear_input (const char *name)
{
    core_init_agent ();
    igsagent_clear_input (core_agent, name);
}

void igs_clear_output (const char *name)
{
    core_init_agent ();
    igsagent_clear_output (core_agent, name);
}

void igs_clear_attribute (const char *name)
{
    core_init_agent ();
    igsagent_clear_attribute (core_agent, name);
}

void core_observeIOPCallback (igsagent_t *agent,
                              igs_io_type_t type,
                              const char *name,
                              igs_io_value_type_t value_type,
                              void *value,
                              size_t value_size,
                              void *my_data)
{
    IGS_UNUSED (agent)
    observe_io_cb_wrapper_t *wrap = (observe_io_cb_wrapper_t *) my_data;
    wrap->cb (type, name, value_type, value, value_size, wrap->my_data);
}

void igs_observe_input (const char *name, igs_io_fn cb, void *my_data)
{
    core_init_agent ();
    assert(name);
    assert(model_check_string(name, IGS_MAX_IO_NAME_LENGTH));
    assert(cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    observe_io_cb_wrapper_t *wrap = (observe_io_cb_wrapper_t *) zmalloc (sizeof (observe_io_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    observed_io_t *observed_io = zhashx_lookup(core_context->observed_inputs, name);
    if (!observed_io) {
        observed_io = (observed_io_t *) zmalloc (sizeof (observed_io_t));
        observed_io->name = s_strndup (name, IGS_MAX_IO_NAME_LENGTH);
        observed_io->observed_io_wrappers = zlist_new();
        zhashx_insert(core_context->observed_inputs, observed_io->name, observed_io);
    }
    zlist_append(observed_io->observed_io_wrappers, wrap);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_observe_input (core_agent, observed_io->name, core_observeIOPCallback, wrap);
}

void igs_observe_output (const char *name, igs_io_fn cb, void *my_data)
{
    core_init_agent ();
    assert(name);
    assert(model_check_string(name, IGS_MAX_IO_NAME_LENGTH));
    assert(cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    observe_io_cb_wrapper_t *wrap = (observe_io_cb_wrapper_t *) zmalloc (sizeof (observe_io_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    observed_io_t *observed_io = zhashx_lookup(core_context->observed_outputs, name);
    if (!observed_io) {
        observed_io = (observed_io_t *) zmalloc (sizeof (observed_io_t));
        observed_io->name = s_strndup (name, IGS_MAX_IO_NAME_LENGTH);
        observed_io->observed_io_wrappers = zlist_new();
        zhashx_insert(core_context->observed_outputs, observed_io->name, observed_io);
    }
    zlist_append(observed_io->observed_io_wrappers, wrap);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_observe_output (core_agent, observed_io->name, core_observeIOPCallback, wrap);
}

void igs_observe_attribute (const char *name, igs_io_fn cb, void *my_data)
{
    core_init_agent ();
    assert(name);
    assert(model_check_string(name, IGS_MAX_IO_NAME_LENGTH));
    assert(cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    observe_io_cb_wrapper_t *wrap = (observe_io_cb_wrapper_t *) zmalloc (sizeof (observe_io_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    observed_io_t *observed_io = zhashx_lookup(core_context->observed_attributes, name);
    if (!observed_io) {
        observed_io = (observed_io_t *) zmalloc (sizeof (observed_io_t));
        observed_io->name = s_strndup (name, IGS_MAX_IO_NAME_LENGTH);
        observed_io->observed_io_wrappers = zlist_new();
        zhashx_insert(core_context->observed_attributes, observed_io->name, observed_io);
    }
    zlist_append(observed_io->observed_io_wrappers, wrap);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    igsagent_observe_attribute (core_agent, observed_io->name, core_observeIOPCallback, wrap);
}

void igs_output_mute (const char *name)
{
    core_init_agent ();
    igsagent_output_mute (core_agent, name);
}

void igs_output_unmute (const char *name)
{
    core_init_agent ();
    igsagent_output_unmute (core_agent, name);
}

bool igs_output_is_muted (const char *name)
{
    core_init_agent ();
    return igsagent_output_is_muted (core_agent, name);
}

igs_io_value_type_t igs_input_type (const char *name)
{
    core_init_agent ();
    return igsagent_input_type (core_agent, name);
}

igs_io_value_type_t igs_output_type (const char *name)
{
    core_init_agent ();
    return igsagent_output_type (core_agent, name);
}

igs_io_value_type_t igs_attribute_type (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_type (core_agent, name);
}

size_t igs_input_count (void)
{
    core_init_agent ();
    return igsagent_input_count (core_agent);
}

size_t igs_output_count (void)
{
    core_init_agent ();
    return igsagent_output_count (core_agent);
}

size_t igs_attribute_count (void)
{
    core_init_agent ();
    return igsagent_attribute_count (core_agent);
}

char **igs_input_list (size_t *nb_of_elements)
{
    core_init_agent ();
    return igsagent_input_list (core_agent, nb_of_elements);
}

char **igs_output_list (size_t *nb_of_elements)
{
    core_init_agent ();
    return igsagent_output_list (core_agent, nb_of_elements);
}

char **igs_attribute_list (size_t *nb_of_elements)
{
    core_init_agent ();
    return igsagent_attribute_list (core_agent, nb_of_elements);
}

bool igs_input_exists (const char *name)
{
    core_init_agent ();
    return igsagent_input_exists (core_agent, name);
}

bool igs_output_exists (const char *name)
{
    core_init_agent ();
    return igsagent_output_exists (core_agent, name);
}

bool igs_attribute_exists (const char *name)
{
    core_init_agent ();
    return igsagent_attribute_exists (core_agent, name);
}

// definition
igs_result_t igs_definition_load_str (const char *json_str)
{
    core_init_agent ();
    return igsagent_definition_load_str (core_agent, json_str);
}

igs_result_t igs_definition_load_file (const char *file_path)
{
    core_init_agent ();
    return igsagent_definition_load_file (core_agent, file_path);
}

void igs_clear_definition (void)
{
    core_init_agent ();
    igsagent_clear_definition (core_agent);
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    // delete associated callback wrappers
    observed_io_t *observed_io = zhashx_first(core_context->observed_inputs);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_inputs);
    }
    zhashx_purge(core_context->observed_inputs);
    observed_io = zhashx_first(core_context->observed_outputs);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_outputs);
    }
    zhashx_purge(core_context->observed_outputs);
    observed_io = zhashx_first(core_context->observed_attributes);
    while (observed_io) {
        s_core_free_observeIOP (&observed_io);
        observed_io = zhashx_next(core_context->observed_attributes);
    }
    zhashx_purge(core_context->observed_attributes);
    service_cb_wrapper_t *service_cb_wrapper = zhashx_first(core_context->service_cb_wrappers);
    while (service_cb_wrapper) {
        zhashx_delete(core_context->service_cb_wrappers, service_cb_wrapper->name);
        s_core_free_service_cb_wrapper (&service_cb_wrapper);
        service_cb_wrapper = zhashx_next(core_context->service_cb_wrappers);
    }
    zhashx_purge(core_context->service_cb_wrappers);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

char *igs_definition_json (void)
{
    core_init_agent ();
    return igsagent_definition_json (core_agent);
}

void igs_definition_set_package(const char *package)
{
    core_init_agent ();
    igsagent_definition_set_package (core_agent, package);
}

// returned char* must be freed by caller
char * igs_definition_package(void)
{
    core_init_agent ();
    return igsagent_definition_package (core_agent);
}

void igs_definition_set_class(const char *my_class)
{
    core_init_agent ();
    igsagent_definition_set_class (core_agent, my_class);
}

// returned char* must be freed by caller
char * igs_definition_class(void){
    core_init_agent ();
    return igsagent_definition_class (core_agent);
}

// returned char* must be freed by caller
char *igs_definition_description (void)
{
    core_init_agent ();
    return igsagent_definition_description (core_agent);
}

// returned char* must be freed by caller
char *igs_definition_version (void)
{
    core_init_agent ();
    return igsagent_definition_version (core_agent);
}

void igs_definition_set_description (const char *description)
{
    core_init_agent ();
    igsagent_definition_set_description (core_agent, description);
}

void igs_definition_set_version (const char *version)
{
    core_init_agent ();
    igsagent_definition_set_version (core_agent, version);
}

igs_result_t igs_input_create (const char *name,
                               igs_io_value_type_t value_type,
                               void *value,
                               size_t size)
{
    core_init_agent ();
    return igsagent_input_create (core_agent, name, value_type, value, size);
}

igs_result_t igs_output_create (const char *name,
                                igs_io_value_type_t value_type,
                                void *value,
                                size_t size)
{
    core_init_agent ();
    return igsagent_output_create (core_agent, name, value_type, value, size);
}

igs_result_t igs_attribute_create (const char *name,
                                   igs_io_value_type_t value_type,
                                   void *value,
                                   size_t size)
{
    core_init_agent ();
    return igsagent_attribute_create (core_agent, name, value_type, value,
                                       size);
}

igs_result_t igs_input_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_input_remove (core_agent, name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_io_t *observed_io = zhashx_lookup(core_context->observed_inputs, name);
        if (observed_io) {
            zhashx_delete(core_context->observed_inputs, name);
            s_core_free_observeIOP (&observed_io);
        }
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return result;
}

igs_result_t igs_output_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_output_remove (core_agent, name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_io_t *observed_io = zhashx_lookup(core_context->observed_outputs, name);
        if (observed_io) {
            zhashx_delete(core_context->observed_outputs, name);
            s_core_free_observeIOP (&observed_io);
        }
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return result;
}

igs_result_t igs_attribute_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_attribute_remove (core_agent, name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_io_t *observed_io = zhashx_lookup(core_context->observed_attributes, name);
        if (observed_io) {
            zhashx_delete(core_context->observed_attributes, name);
            s_core_free_observeIOP (&observed_io);
        }
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return result;
}

// mapping
igs_result_t igs_mapping_load_str (const char *json_str)
{
    core_init_agent ();
    return igsagent_mapping_load_str (core_agent, json_str);
}

igs_result_t igs_mapping_load_file (const char *file_path)
{
    core_init_agent ();
    return igsagent_mapping_load_file (core_agent, file_path);
}

void igs_clear_mappings (void)
{
    core_init_agent ();
    igsagent_clear_mappings (core_agent);
}

void igs_clear_mappings_with_agent (const char *agent_name)
{
    core_init_agent ();
    igsagent_clear_mappings_with_agent (core_agent, agent_name);
}

void igs_clear_mappings_for_input (const char *input_name)
{
    core_init_agent ();
    igsagent_clear_mappings_for_input (core_agent, input_name);
}

char *igs_mapping_json (void)
{
    core_init_agent ();
    return igsagent_mapping_json (core_agent);
}

size_t igs_mapping_count (void)
{
    core_init_agent ();
    return igsagent_mapping_count (core_agent);
}

uint64_t igs_mapping_add (const char *from_our_input,
                          const char *to_agent,
                          const char *with_output)
{
    core_init_agent ();
    return igsagent_mapping_add (core_agent, from_our_input, to_agent, with_output);
}
// returns mapping id or zero or below if creation failed
igs_result_t igs_mapping_remove_with_id (uint64_t the_id)
{
    core_init_agent ();
    return igsagent_mapping_remove_with_id (core_agent, the_id);
}

igs_result_t igs_mapping_remove_with_name (const char *from_our_input,
                                           const char *to_agent,
                                           const char *with_output)
{
    core_init_agent ();
    return igsagent_mapping_remove_with_name (core_agent, from_our_input,
                                               to_agent, with_output);
}

// split

size_t igs_split_count (void)
{
    core_init_agent ();
    return igsagent_split_count (core_agent);
}

uint64_t igs_split_add (const char *from_our_input,
                        const char *to_agent,
                        const char *with_output)
{
    core_init_agent ();
    return igsagent_split_add (core_agent, from_our_input, to_agent, with_output);
}

igs_result_t igs_split_remove_with_id (uint64_t the_id)
{
    core_init_agent ();
    return igsagent_split_remove_with_id (core_agent, the_id);
}

igs_result_t igs_split_remove_with_name (const char *from_our_input,
                                         const char *to_agent,
                                         const char *with_output)
{
    core_init_agent ();
    return igsagent_split_remove_with_name (core_agent, from_our_input,
                                            to_agent, with_output);
}

// admin

void igs_mapping_set_outputs_request (bool notify)
{
    core_init_agent ();
    igsagent_mapping_set_outputs_request (core_agent, notify);
}

bool igs_mapping_outputs_request (void)
{
    core_init_agent ();
    return igsagent_mapping_outputs_request (core_agent);
}

void igs_definition_set_path (const char *path)
{
    core_init_agent ();
    igsagent_definition_set_path (core_agent, path);
}

void igs_mapping_set_path (const char *path)
{
    core_init_agent ();
    igsagent_mapping_set_path (core_agent, path);
}

void igs_definition_save (void)
{
    core_init_agent ();
    igsagent_definition_save (core_agent);
}

void igs_mapping_save (void)
{
    core_init_agent ();
    igsagent_mapping_save (core_agent);
}

void igs_log (igs_log_level_t level,
              const char *function,
              const char *format,
              ...)
{
    core_init_agent ();
    va_list list;
    va_start (list, format);
    char content[IGS_MAX_LOG_LENGTH] = "";
    vsnprintf (content, IGS_MAX_LOG_LENGTH - 1, format, list);
    va_end (list);
    admin_log (core_agent, level, function, "%s", content);
}

// ADVANCED
igs_result_t igs_election_join (const char *election_name)
{
    core_init_agent ();
    return igsagent_election_join (core_agent, election_name);
}

igs_result_t igs_election_leave (const char *election_name)
{
    core_init_agent ();
    return igsagent_election_leave (core_agent, election_name);
}

igs_result_t igs_output_set_zmsg (const char *name, zmsg_t *msg)
{
    core_init_agent ();
    return igsagent_output_set_zmsg (core_agent, name, msg);
}

igs_result_t igs_input_zmsg (const char *name, zmsg_t **msg)
{
    core_init_agent ();
    return igsagent_input_zmsg (core_agent, name, msg);
}

igs_result_t igs_service_call (const char *agent_name_oruuid,
                               const char *service_name,
                               igs_service_arg_t **list,
                               const char *token)
{
    core_init_agent ();
    return igsagent_service_call (core_agent, agent_name_oruuid, service_name,
                                   list, token);
}

void core_service_callback (igsagent_t *agent,
                            const char *sender_agent_name,
                            const char *sender_agentuuid,
                            const char *service_name,
                            igs_service_arg_t *first_argument,
                            size_t nb_args,
                            const char *token,
                            void *my_data)
{
    IGS_UNUSED (agent)
    service_cb_wrapper_t *wrap = (service_cb_wrapper_t *) my_data;
    wrap->cb (sender_agent_name, sender_agentuuid, service_name, first_argument,
              nb_args, token, wrap->my_data);
}

igs_result_t
igs_service_init (const char *name, igs_service_fn cb, void *my_data)
{
    assert (name && strlen (name) > 0);
    assert(model_check_string(name, IGS_MAX_SERVICE_NAME_LENGTH));
    assert (cb);
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    service_cb_wrapper_t *wrap = zhashx_lookup(core_context->service_cb_wrappers, name);
    if (!wrap){
        wrap = (service_cb_wrapper_t *) zmalloc (sizeof (service_cb_wrapper_t));
        wrap->name = s_strndup (name, IGS_MAX_SERVICE_NAME_LENGTH);
        wrap->cb = cb;
        wrap->my_data = my_data;
        zhashx_insert(core_context->service_cb_wrappers, wrap->name, wrap);
    } else {
        igs_error ("service with name %s exists and already has a callback", name);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_FAILURE;
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    if (igsagent_service_init (core_agent, wrap->name, core_service_callback, wrap) == IGS_FAILURE){
        zhashx_delete(core_context->service_cb_wrappers, wrap->name);
        s_core_free_service_cb_wrapper(&wrap);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

igs_result_t igs_service_remove (const char *name)
{
    assert (name);
    core_init_agent ();
    igs_result_t result = igsagent_service_remove (core_agent, name);
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrapper
        service_cb_wrapper_t *service_cb_wrapper = zhashx_lookup(core_context->service_cb_wrappers, name);
        if (service_cb_wrapper) {
            zhashx_delete(core_context->service_cb_wrappers, name);
            s_core_free_service_cb_wrapper (&service_cb_wrapper);
        }
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return result;
}

igs_result_t igs_service_set_description (const char *name, const char *description)
{
    assert (name);
    core_init_agent ();
    return igsagent_service_set_description (core_agent, name, description);
}

char * igs_service_description (const char *name)
{
    assert (name);
    core_init_agent ();
    return igsagent_service_description (core_agent, name);
}

igs_result_t igs_service_arg_add (const char *service_name,
                                  const char *arg_name,
                                  igs_io_value_type_t type)
{
    core_init_agent ();
    return igsagent_service_arg_add (core_agent, service_name, arg_name, type);
}

igs_result_t igs_service_arg_remove (const char *service_name,
                                     const char *arg_name)
{
    core_init_agent ();
    return igsagent_service_arg_remove (core_agent, service_name, arg_name);
}

igs_result_t igs_service_arg_set_description(const char *service_name,
                                             const char *arg_name,
                                             const char *description)
{
    core_init_agent ();
    return igsagent_service_arg_set_description (core_agent, service_name, arg_name, description);
}

char * igs_service_arg_description (const char *service_name, const char *arg_name)
{
    core_init_agent ();
    return igsagent_service_arg_description (core_agent, service_name, arg_name);
}

igs_result_t igs_service_reply_add(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_add(core_agent, service_name, reply_name);
}

igs_result_t igs_service_reply_set_description(const char *service_name, const char *reply_name, const char *description)
{
    core_init_agent ();
    return igsagent_service_reply_set_description(core_agent, service_name, reply_name, description);
}

char * igs_service_reply_description(const char *service_name, const char *reply_name)
{
    core_init_agent ();
    return igsagent_service_reply_description(core_agent, service_name, reply_name);
}

igs_result_t igs_service_reply_remove(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_remove(core_agent, service_name, reply_name);
}

igs_result_t igs_service_reply_arg_add(const char *service_name, const char *reply_name, const char *arg_name,
                                       igs_io_value_type_t type){
    core_init_agent ();
    return igsagent_service_reply_arg_add(core_agent, service_name, reply_name, arg_name, type);
}

igs_result_t igs_service_reply_arg_set_description(const char *service_name, const char *reply_name, const char *arg_name, const char *description)
{
    core_init_agent ();
    return igsagent_service_reply_arg_set_description(core_agent, service_name, reply_name, arg_name, description);
}

char * igs_service_reply_arg_description(const char *service_name, const char *reply_name, const char *arg_name)
{
    core_init_agent ();
    return igsagent_service_reply_arg_description(core_agent, service_name, reply_name, arg_name);
}

igs_result_t igs_service_reply_arg_remove(const char *service_name, const char *reply_name,
                                          const char *arg_name){
    core_init_agent ();
    return igsagent_service_reply_arg_remove(core_agent, service_name, reply_name, arg_name);
}

size_t igs_service_count (void)
{
    core_init_agent ();
    return igsagent_service_count (core_agent);
}

bool igs_service_exists (const char *name)
{
    core_init_agent ();
    return igsagent_service_exists (core_agent, name);
}

char **igs_service_list (size_t *nb_of_elements)
{
    core_init_agent ();
    return igsagent_service_list (core_agent, nb_of_elements);
}

igs_service_arg_t *igs_service_args_first (const char *service_name)
{
    core_init_agent ();
    return igsagent_service_args_first (core_agent, service_name);
}

size_t igs_service_args_count (const char *service_name)
{
    core_init_agent ();
    return igsagent_service_args_count (core_agent, service_name);
}

bool igs_service_arg_exists (const char *service_name, const char *arg_name)
{
    core_init_agent ();
    return igsagent_service_arg_exists (core_agent, service_name, arg_name);
}

bool igs_service_has_replies(const char *service_name){
    core_init_agent ();
    return igsagent_service_has_replies(core_agent, service_name);
}

bool igs_service_has_reply(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_has_reply(core_agent, service_name, reply_name);
}

char ** igs_service_reply_names(const char *service_name, size_t *service_replies_nbr){
    core_init_agent ();
    return igsagent_service_reply_names(core_agent, service_name, service_replies_nbr);
}

igs_service_arg_t * igs_service_reply_args_first(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_args_first(core_agent, service_name, reply_name);
}

size_t igs_service_reply_args_count(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_args_count(core_agent, service_name, reply_name);
}

bool igs_service_reply_arg_exists(const char *service_name, const char *reply_name, const char *arg_name){
    core_init_agent ();
    return igsagent_service_reply_arg_exists(core_agent, service_name, reply_name, arg_name);
}

int64_t igs_rt_get_current_timestamp(void){
    core_init_agent ();
    return igsagent_rt_get_current_timestamp(core_agent);
}

void igs_rt_set_timestamps(bool enable){
    core_init_agent ();
    igsagent_rt_set_timestamps(core_agent, enable);
}

bool igs_rt_timestamps(void){
    core_init_agent ();
    return  igsagent_rt_timestamps(core_agent);
}

void igs_rt_set_time(int64_t microseconds){
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_info("set rt time to %lld", microseconds);
    core_context->rt_current_microseconds = microseconds;
    igsagent_t *agent = zhashx_first(core_context->agents);
    while (agent) {
        agent->rt_timestamps_enabled = true;
        if (agent->rt_synchronous_mode_enabled
            && agent->definition
            && agent->definition->outputs_table){
            //iterate on outputs and publish them separately
            //NB: we do not create a single gathering message because it would not be compatible
            //with the mapping filters: only the 1st output would serve in the filter.
            igs_io_t *io = zhashx_first(agent->definition->outputs_table);
            while (io) {
                network_publish_output(agent, io);
                io = zhashx_next(agent->definition->outputs_table);
            }
        }
        agent = zhashx_next(core_context->agents);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

int64_t igs_rt_time(void){
    core_init_agent ();
    return core_context->rt_current_microseconds;
}

void igs_rt_set_synchronous_mode(bool enable){
    core_init_agent ();
    igsagent_rt_set_synchronous_mode(core_agent, enable);
}

bool igs_rt_synchronous_mode(void){
    core_init_agent ();
    return igsagent_rt_synchronous_mode(core_agent);
}

//DEPRECATED functions management for parameters
igs_result_t igs_parameter_create(const char *name,
                                  igs_iop_value_type_t value_type,
                                  void *value,
                                  size_t size){
    igs_warn("this function is deprecated, please use igs_attribute_create instead.");
    return igs_attribute_create(name, value_type, value, size);
 }
igs_result_t igs_parameter_remove(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_remove instead.");
    return igs_attribute_remove(name);
}
igs_iop_value_type_t igs_parameter_type(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_type instead.");
    return igs_attribute_type(name);
}
size_t igs_parameter_count(void){
    igs_warn("this function is deprecated, please use igs_attribute_count instead.");
    return igs_attribute_count();
}
char** igs_parameter_list(size_t *parameters_nbr){
    igs_warn("this function is deprecated, please use igs_attribute_list instead.");
    return igs_attribute_list(parameters_nbr);
}
bool igs_parameter_exists(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_exists instead.");
    return igs_attribute_exists(name);
}
bool igs_parameter_bool(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_bool instead.");
    return igs_attribute_bool(name);
}
int igs_parameter_int(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_int instead.");
    return igs_attribute_int(name);
}
double igs_parameter_double(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_double instead.");
    return igs_attribute_double(name);
}
char * igs_parameter_string(const char *name){
    igs_warn("this function is deprecated, please use igs_attribute_string instead.");
    return igs_attribute_string(name);
}
igs_result_t igs_parameter_data(const char *name, void **data, size_t *size){
    igs_warn("this function is deprecated, please use igs_attribute_data instead.");
    return igs_attribute_data(name, data, size);
}
igs_result_t igs_parameter_set_bool(const char *name, bool value){
    igs_warn("this function is deprecated, please use igs_attribute_set_bool instead.");
    return igs_attribute_set_bool(name, value);
}
igs_result_t igs_parameter_set_int(const char *name, int value){
    igs_warn("this function is deprecated, please use igs_attribute_set_int instead.");
    return igs_attribute_set_int(name, value);
}
igs_result_t igs_parameter_set_double(const char *name, double value){
    igs_warn("this function is deprecated, please use igs_attribute_set_double instead.");
    return igs_attribute_set_double(name, value);
}
igs_result_t igs_parameter_set_string(const char *name, const char *value){
    igs_warn("this function is deprecated, please use igs_attribute_set_string instead.");
    return igs_attribute_set_string(name, value);
}
igs_result_t igs_parameter_set_data(const char *name, void *value, size_t size){
    igs_warn("this function is deprecated, please use igs_attribute_set_data instead.");
    return igs_attribute_set_data(name, value, size);
}
igs_result_t igs_parameter_add_constraint(const char *name, const char *constraint){
    igs_warn("this function is deprecated, please use igs_attribute_add_constraint instead.");
    return igs_attribute_add_constraint(name, constraint);
}
igs_result_t igs_parameter_set_description(const char *name, const char *description){
    igs_warn("this function is deprecated, please use igs_attribute_set_description instead.");
    return igs_attribute_set_description(name, description);
}
igs_result_t igs_parameter_set_detailed_type(const char *param_name, const char *type_name, const char *specification){
    igs_warn("this function is deprecated, please use igs_attribute_set_detailed_type instead.");
    return igs_attribute_set_detailed_type(param_name, type_name, specification);
}
void igs_clear_parameter(const char *name){
    igs_warn("this function is deprecated, please use igs_clear_attribute instead.");
    igs_clear_attribute(name);
}
void igs_observe_parameter(const char *name, igs_iop_fn cb, void *my_data){
    igs_warn("this function is deprecated, please use igs_observe_attribute instead.");
    igs_observe_attribute(name, cb, my_data);
}
void igs_free_iop_list(char **list, size_t io_nbr){
    igs_warn("this function is deprecated, please use igs_free_io_list instead.");
    igs_free_io_list(list, io_nbr);
}
