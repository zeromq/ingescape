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

// Callback wrappers
typedef struct observe_iop_cb_wrapper
{
    igs_iop_fn *cb;
    void *my_data;
    struct observe_iop_cb_wrapper *next;
} observe_iop_cb_wrapper_t;

typedef struct observed_iop
{
    char *name;
    observe_iop_cb_wrapper_t *firstCBWrapper;
    UT_hash_handle hh;
} observed_iop_t;

typedef struct
{
    char *name;
    igs_service_fn *cb;
    void *my_data;
    UT_hash_handle hh;
} service_cb_wrapper_t;

typedef struct observe_mute_cb_wrapper
{
    igs_mute_fn *cb;
    void *my_data;
    struct observe_mute_cb_wrapper *next;
} observe_mute_cb_wrapper_t;

typedef struct observe_agent_events_cb_wrapper
{
    igs_agent_events_fn *cb;
    void *my_data;
    struct observe_agent_events_cb_wrapper *next;
} observe_agent_events_cb_wrapper_t;

observed_iop_t *observed_inputs = NULL;
observed_iop_t *observed_outputs = NULL;
observed_iop_t *observed_parameters = NULL;
service_cb_wrapper_t *service_cb_wrappers = NULL;
observe_mute_cb_wrapper_t *mute_cb_wrappers = NULL;
observe_agent_events_cb_wrapper_t *agent_event_cb_wrappers = NULL;

//////////////////  CORE CONTEXT //////////////////
void core_init_context ()
{
    if (core_context == NULL) {
        core_context = (struct igs_core_context *) zmalloc (sizeof (struct igs_core_context));
        core_context->created_agents = zhash_new ();
        core_context->brokers = zhash_new ();
        zhash_autofree (core_context->brokers);
        // default values for context variables
        // NB: other values stay at zero / NULL until they are changed
        // by other functions.
        core_context->network_allow_ipc = true;
        core_context->network_allow_inproc = true;
        core_context->network_hwm_value = 1000;
        core_context->network_discovery_interval = 1000;
        core_context->network_agent_timeout = 8000;
        core_context->log_level = IGS_LOG_INFO;
        core_context->log_file_level = IGS_LOG_TRACE;
        core_context->log_file_max_line_length = IGS_MAX_LOG_LENGTH;
        core_context->network_shall_raise_file_descriptors_limit = true;
        core_context->network_ipc_folder_path = strdup (IGS_DEFAULT_IPC_FOLDER_PATH);
    }
}

void s_core_free_observeIOP (observed_iop_t **observed_iop)
{ // Internal
    assert (observed_iop);
    assert (*observed_iop);
    if ((*observed_iop)->name) {
        free ((*observed_iop)->name);
        (*observed_iop)->name = NULL;
    }
    observe_iop_cb_wrapper_t *iop_cb_wrapper = NULL, *iop_cb_wrapper_tmp = NULL;
    LL_FOREACH_SAFE ((*observed_iop)->firstCBWrapper, iop_cb_wrapper,
                     iop_cb_wrapper_tmp)
    {
        LL_DELETE ((*observed_iop)->firstCBWrapper, iop_cb_wrapper);
        free (iop_cb_wrapper);
        iop_cb_wrapper = NULL;
    }
    free (*observed_iop);
    (*observed_iop) = NULL;
}

void s_core_free_service_cb_wrapper (service_cb_wrapper_t **service_cb_wrapper)
{ // Internal
    assert (service_cb_wrapper);
    assert (*service_cb_wrapper);
    if ((*service_cb_wrapper)->name) {
        free ((*service_cb_wrapper)->name);
        (*service_cb_wrapper)->name = NULL;
    }
    free (*service_cb_wrapper);
    (*service_cb_wrapper) = NULL;
}

void igs_clear_context (void)
{
    if (core_context) {
        igs_stop ();
        igs_monitor_stop ();
        if (core_context->created_agents) {
            igsagent_t *a =
              (igsagent_t *) zhash_first (core_context->created_agents);
            while (a) {
                igsagent_destroy (&a);
                a = zhash_next (core_context->created_agents);
            }
            zhash_destroy (&core_context->created_agents);
        }
        core_agent = NULL;
        // delete core agent callback wrappers
        observed_iop_t *observed_iop, *observed_iop_tmp;
        HASH_ITER (hh, observed_inputs, observed_iop, observed_iop_tmp)
        {
            HASH_DEL (observed_inputs, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
        HASH_ITER (hh, observed_outputs, observed_iop, observed_iop_tmp)
        {
            HASH_DEL (observed_outputs, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
        HASH_ITER (hh, observed_parameters, observed_iop, observed_iop_tmp)
        {
            HASH_DEL (observed_parameters, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
        service_cb_wrapper_t *service_cb_wrapper, *service_cb_wrapper_tmp;
        HASH_ITER (hh, service_cb_wrappers, service_cb_wrapper,
                   service_cb_wrapper_tmp)
        {
            HASH_DEL (service_cb_wrappers, service_cb_wrapper);
            s_core_free_service_cb_wrapper (&service_cb_wrapper);
        }
        observe_mute_cb_wrapper_t *mute_cb_wrapper = NULL,
                                  *mute_cb_wrapper_tmp = NULL;
        LL_FOREACH_SAFE (mute_cb_wrappers, mute_cb_wrapper, mute_cb_wrapper_tmp)
        {
            LL_DELETE (mute_cb_wrappers, mute_cb_wrapper);
            free (mute_cb_wrapper);
            mute_cb_wrapper = NULL;
        }
        observe_agent_events_cb_wrapper_t *agent_event_cb_wrapper = NULL,
                                          *agent_event_cb_wrapper_tmp = NULL;
        LL_FOREACH_SAFE (agent_event_cb_wrappers, agent_event_cb_wrapper,
                         agent_event_cb_wrapper_tmp)
        {
            LL_DELETE (agent_event_cb_wrappers, agent_event_cb_wrapper);
            free (agent_event_cb_wrapper);
            agent_event_cb_wrapper = NULL;
        }

        if (core_context->log_file)
            fclose (core_context->log_file);

        igs_freeze_wrapper_t *freeze_elt, *freeze_tmp;
        DL_FOREACH_SAFE (core_context->freeze_callbacks, freeze_elt, freeze_tmp)
        {
            DL_DELETE (core_context->freeze_callbacks, freeze_elt);
            free (freeze_elt);
        }
        igs_forced_stop_wrapper_t *stop_elt, *stop_tmp;
        DL_FOREACH_SAFE (core_context->external_stop_calbacks, stop_elt,
                         stop_tmp)
        {
            DL_DELETE (core_context->external_stop_calbacks, stop_elt);
            free (stop_elt);
        }
        zhash_destroy (&core_context->brokers);

        if (core_context->security_auth)
            zactor_destroy (&(core_context->security_auth));
        if (core_context->security_cert)
            zcert_destroy (&(core_context->security_cert));
        if (core_context->security_public_certificates_directory)
            free (core_context->security_public_certificates_directory);

        if (core_context->elections) {
            zlist_t *e = (zlist_t *) zhash_first (core_context->elections);
            while (e) {
                zlist_destroy (&e);
                e = zhash_next (core_context->elections);
            }
            zhash_destroy (&core_context->elections);
        }

        free (core_context);
        core_context = NULL;
    }
}

//////////////////  CORE AGENT //////////////////
void core_external_stop_cb (void *my_data)
{
    IGS_UNUSED (my_data)
}

void core_init_agent ()
{
    core_init_context ();
    if (core_agent == NULL) {
        core_agent = igsagent_new (IGS_DEFAULT_AGENT_NAME, false);
        igs_observe_forced_stop (core_external_stop_cb, NULL);
        core_agent->context = core_context;
        igsagent_activate (core_agent);
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

const char * igs_agent_uuid (void)
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
    observe_mute_cb_wrapper_t *wrap = (observe_mute_cb_wrapper_t *) zmalloc (
      sizeof (observe_mute_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    LL_APPEND (mute_cb_wrappers, wrap); // store wrapper to delete it later
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
    observe_agent_events_cb_wrapper_t *wrap =
      (observe_agent_events_cb_wrapper_t *) zmalloc (
        sizeof (observe_agent_events_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    LL_APPEND (agent_event_cb_wrappers, wrap);
    igsagent_observe_agent_events (core_agent,
                                   core_observe_agent_events_callback, wrap);
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

bool igs_parameter_bool (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_bool (core_agent, name);
}

int igs_parameter_int (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_int (core_agent, name);
}

double igs_parameter_double (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_double (core_agent, name);
}

char *igs_parameter_string (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_string (core_agent, name);
}

igs_result_t igs_parameter_data (const char *name, void **data, size_t *size)
{
    core_init_agent ();
    return igsagent_parameter_data (core_agent, name, data, size);
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

igs_result_t igs_parameter_set_bool (const char *name, bool value)
{
    core_init_agent ();
    return igsagent_parameter_set_bool (core_agent, name, value);
}

igs_result_t igs_parameter_set_int (const char *name, int value)
{
    core_init_agent ();
    return igsagent_parameter_set_int (core_agent, name, value);
}

igs_result_t igs_parameter_set_double (const char *name, double value)
{
    core_init_agent ();
    return igsagent_parameter_set_double (core_agent, name, value);
}

igs_result_t igs_parameter_set_string (const char *name, const char *value)
{
    core_init_agent ();
    return igsagent_parameter_set_string (core_agent, name, value);
}

igs_result_t igs_parameter_set_data (const char *name, void *value, size_t size)
{
    core_init_agent ();
    return igsagent_parameter_set_data (core_agent, name, value, size);
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

igs_result_t igs_parameter_add_constraint (const char *name, const char *constraint)
{
    core_init_agent ();
    return igsagent_parameter_add_constraint (core_agent, name, constraint);
}

void igs_input_set_description(const char *name, const char *description)
{
    core_init_agent ();
    igsagent_input_set_description (core_agent, name, description);
}

void igs_output_set_description(const char *name, const char *description)
{
    core_init_agent ();
    igsagent_output_set_description (core_agent, name, description);
}

void igs_parameter_set_description(const char *name, const char *description)
{
    core_init_agent ();
    igsagent_parameter_set_description (core_agent, name, description);
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

void igs_clear_parameter (const char *name)
{
    core_init_agent ();
    igsagent_clear_parameter (core_agent, name);
}

void core_observeIOPCallback (igsagent_t *agent,
                              igs_iop_type_t type,
                              const char *name,
                              igs_iop_value_type_t value_type,
                              void *value,
                              size_t value_size,
                              void *my_data)
{
    IGS_UNUSED (agent)
    observe_iop_cb_wrapper_t *wrap = (observe_iop_cb_wrapper_t *) my_data;
    wrap->cb (type, name, value_type, value, value_size, wrap->my_data);
}

void igs_observe_input (const char *name, igs_iop_fn cb, void *my_data)
{
    core_init_agent ();
    observe_iop_cb_wrapper_t *wrap =
      (observe_iop_cb_wrapper_t *) zmalloc (sizeof (observe_iop_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    // store wrapper to delete it later
    observed_iop_t *observed_iop = NULL;
    HASH_FIND_STR (observed_inputs, name, observed_iop);
    if (observed_iop == NULL) {
        observed_iop = (observed_iop_t *) zmalloc (sizeof (observed_iop_t));
        observed_iop->name = strdup (name);
        observed_iop->firstCBWrapper = NULL;
        HASH_ADD_STR (observed_inputs, name, observed_iop);
    }
    LL_APPEND (observed_iop->firstCBWrapper, wrap);
    igsagent_observe_input (core_agent, name, core_observeIOPCallback, wrap);
}

void igs_observe_output (const char *name, igs_iop_fn cb, void *my_data)
{
    core_init_agent ();
    observe_iop_cb_wrapper_t *wrap =
      (observe_iop_cb_wrapper_t *) zmalloc (sizeof (observe_iop_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    // store wrapper to delete it later
    observed_iop_t *observed_iop = NULL;
    HASH_FIND_STR (observed_outputs, name, observed_iop);
    if (observed_iop == NULL) {
        observed_iop = (observed_iop_t *) zmalloc (sizeof (observed_iop_t));
        observed_iop->name = strdup (name);
        observed_iop->firstCBWrapper = NULL;
        HASH_ADD_STR (observed_outputs, name, observed_iop);
    }
    LL_APPEND (observed_iop->firstCBWrapper, wrap);
    igsagent_observe_output (core_agent, name, core_observeIOPCallback, wrap);
}

void igs_observe_parameter (const char *name, igs_iop_fn cb, void *my_data)
{
    core_init_agent ();
    observe_iop_cb_wrapper_t *wrap =
      (observe_iop_cb_wrapper_t *) zmalloc (sizeof (observe_iop_cb_wrapper_t));
    wrap->cb = cb;
    wrap->my_data = my_data;
    // store wrapper to delete it later
    observed_iop_t *observed_iop = NULL;
    HASH_FIND_STR (observed_parameters, name, observed_iop);
    if (observed_iop == NULL) {
        observed_iop = (observed_iop_t *) zmalloc (sizeof (observed_iop_t));
        observed_iop->name = strdup (name);
        observed_iop->firstCBWrapper = NULL;
        HASH_ADD_STR (observed_parameters, name, observed_iop);
    }
    LL_APPEND (observed_iop->firstCBWrapper, wrap);
    igsagent_observe_parameter (core_agent, name, core_observeIOPCallback,
                                 wrap);
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

igs_iop_value_type_t igs_input_type (const char *name)
{
    core_init_agent ();
    return igsagent_input_type (core_agent, name);
}

igs_iop_value_type_t igs_output_type (const char *name)
{
    core_init_agent ();
    return igsagent_output_type (core_agent, name);
}

igs_iop_value_type_t igs_parameter_type (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_type (core_agent, name);
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

size_t igs_parameter_count (void)
{
    core_init_agent ();
    return igsagent_parameter_count (core_agent);
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

char **igs_parameter_list (size_t *nb_of_elements)
{
    core_init_agent ();
    return igsagent_parameter_list (core_agent, nb_of_elements);
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

bool igs_parameter_exists (const char *name)
{
    core_init_agent ();
    return igsagent_parameter_exists (core_agent, name);
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
    // delete associated callback wrappers
    observed_iop_t *observed_iop, *observed_iop_tmp;
    HASH_ITER (hh, observed_inputs, observed_iop, observed_iop_tmp)
    {
        HASH_DEL (observed_inputs, observed_iop);
        s_core_free_observeIOP (&observed_iop);
    }
    HASH_ITER (hh, observed_outputs, observed_iop, observed_iop_tmp)
    {
        HASH_DEL (observed_outputs, observed_iop);
        s_core_free_observeIOP (&observed_iop);
    }
    HASH_ITER (hh, observed_parameters, observed_iop, observed_iop_tmp)
    {
        HASH_DEL (observed_parameters, observed_iop);
        s_core_free_observeIOP (&observed_iop);
    }
    service_cb_wrapper_t *service_cb_wrapper, *service_cb_wrapper_tmp;
    HASH_ITER (hh, service_cb_wrappers, service_cb_wrapper,
               service_cb_wrapper_tmp)
    {
        HASH_DEL (service_cb_wrappers, service_cb_wrapper);
        s_core_free_service_cb_wrapper (&service_cb_wrapper);
    }
}

char *igs_definition_json (void)
{
    core_init_agent ();
    return igsagent_definition_json (core_agent);
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
                               igs_iop_value_type_t value_type,
                               void *value,
                               size_t size)
{
    core_init_agent ();
    return igsagent_input_create (core_agent, name, value_type, value, size);
}

igs_result_t igs_output_create (const char *name,
                                igs_iop_value_type_t value_type,
                                void *value,
                                size_t size)
{
    core_init_agent ();
    return igsagent_output_create (core_agent, name, value_type, value, size);
}

igs_result_t igs_parameter_create (const char *name,
                                   igs_iop_value_type_t value_type,
                                   void *value,
                                   size_t size)
{
    core_init_agent ();
    return igsagent_parameter_create (core_agent, name, value_type, value,
                                       size);
}

igs_result_t igs_input_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_input_remove (core_agent, name);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_iop_t *observed_iop = NULL;
        HASH_FIND_STR (observed_inputs, name, observed_iop);
        if (observed_iop) {
            HASH_DEL (observed_inputs, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
    }
    return result;
}

igs_result_t igs_output_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_output_remove (core_agent, name);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_iop_t *observed_iop = NULL;
        HASH_FIND_STR (observed_outputs, name, observed_iop);
        if (observed_iop) {
            HASH_DEL (observed_outputs, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
    }
    return result;
}

igs_result_t igs_parameter_remove (const char *name)
{
    core_init_agent ();
    igs_result_t result = igsagent_parameter_remove (core_agent, name);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrappers
        observed_iop_t *observed_iop = NULL;
        HASH_FIND_STR (observed_parameters, name, observed_iop);
        if (observed_iop) {
            HASH_DEL (observed_parameters, observed_iop);
            s_core_free_observeIOP (&observed_iop);
        }
    }
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
    return igsagent_mapping_add (core_agent, from_our_input, to_agent,
                                  with_output);
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
    return igsagent_split_add (core_agent, from_our_input, to_agent,
                                with_output);
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
    char content[IGS_MAX_STRING_MSG_LENGTH] = "";
    vsnprintf (content, IGS_MAX_STRING_MSG_LENGTH - 1, format, list);
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
    assert (cb);
    core_init_agent ();
    service_cb_wrapper_t *wrap = (service_cb_wrapper_t *) zmalloc (sizeof (service_cb_wrapper_t));
    wrap->name = strdup (name);
    wrap->cb = cb;
    wrap->my_data = my_data;
    HASH_ADD_STR (service_cb_wrappers, name, wrap); // store wrapper to delete it later
    return igsagent_service_init (core_agent, name, core_service_callback, wrap);
}

igs_result_t igs_service_remove (const char *name)
{
    assert (name);
    core_init_agent ();
    igs_result_t result = igsagent_service_remove (core_agent, name);
    if (result == IGS_SUCCESS) {
        // delete associated callback wrapper
        service_cb_wrapper_t *service_cb_wrapper = NULL;
        HASH_FIND_STR (service_cb_wrappers, name, service_cb_wrapper);
        if (service_cb_wrapper) {
            HASH_DEL (service_cb_wrappers, service_cb_wrapper);
            s_core_free_service_cb_wrapper (&service_cb_wrapper);
        }
    }
    return result;
}

igs_result_t igs_service_arg_add (const char *service_name,
                                  const char *arg_name,
                                  igs_iop_value_type_t type)
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

igs_result_t igs_service_reply_add(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_add(core_agent, service_name, reply_name);
}

igs_result_t igs_service_reply_remove(const char *service_name, const char *reply_name){
    core_init_agent ();
    return igsagent_service_reply_remove(core_agent, service_name, reply_name);
}

igs_result_t igs_service_reply_arg_add(const char *service_name, const char *reply_name, const char *arg_name,
                                       igs_iop_value_type_t type){
    core_init_agent ();
    return igsagent_service_reply_arg_add(core_agent, service_name, reply_name, arg_name, type);
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
