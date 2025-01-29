/*  =========================================================================
    agent - multiple agents in processes, beyond the global API

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "igsagent.h"
#include "ingescape_private.h"


void agent_LOCKED_propagate_agent_event (igs_agent_event_t event,
                                         const char *uuid,
                                         const char *name,
                                         void *event_data)
{
    if (!uuid)
        return;
    if (!name)
        return;
    model_read_write_lock(__FUNCTION__, __LINE__);
    zlistx_t *agents = zhashx_values(core_context->agents);
    igsagent_t *agent = zlistx_first(agents);
    while (agent && agent->uuid) {
        if (!streq (uuid, agent->uuid)) {
            zlist_t *event_callbacks = zlist_dup(agent->agent_event_callbacks);
            igs_agent_event_wrapper_t *cb = zlist_first(event_callbacks);
            while (cb && cb->callback_ptr && agent && agent->uuid) {
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid)
                    cb->callback_ptr (agent, event, uuid, name, event_data, cb->my_data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                cb = zlist_next(event_callbacks);
            }
            zlist_destroy(&event_callbacks);
        }
        agent = zlistx_next(agents);
    }
    zlistx_destroy(&agents);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

igsagent_t *igsagent_new (const char *name, bool activate_immediately)
{
    assert (name);
    core_init_context ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    igsagent_t *agent = (igsagent_t *) zmalloc (sizeof (igsagent_t));
    zuuid_t *uuid = zuuid_new ();
    agent->uuid = strdup (zuuid_str (uuid));
    zuuid_destroy (&uuid);
    agent->activate_callbacks = zlist_new();
    agent->agent_event_callbacks = zlist_new();
    agent->mute_callbacks = zlist_new();
    zhashx_insert (core_context->created_agents, agent->uuid, agent);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    
    igsagent_clear_definition (agent); // set valid but empty definition, preserve name
    igsagent_set_name (agent, name);
    igsagent_clear_mappings (agent); // set valid but empty mapping
    if (activate_immediately)
        igsagent_activate (agent);
    return agent;
}

void igsagent_destroy (igsagent_t **agent)
{
    assert (agent);
    assert (*agent);
    if (igsagent_is_activated (*agent))
        igsagent_deactivate (*agent);
    if (!(*agent)->uuid)
        return;
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    zhashx_delete (core_context->created_agents, (*agent)->uuid);
    free ((*agent)->uuid);
    (*agent)->uuid = NULL;
    
    if ((*agent)->state)
        free ((*agent)->state);
    if ((*agent)->definition_path)
        free ((*agent)->definition_path);
    if ((*agent)->mapping_path)
        free ((*agent)->mapping_path);
    if ((*agent)->igs_channel)
        free ((*agent)->igs_channel);

    igsagent_wrapper_t *activate_cb = zlist_first((*agent)->activate_callbacks);
    while (activate_cb) {
        activate_cb->callback_ptr = NULL;
        free(activate_cb);
        activate_cb = zlist_next((*agent)->activate_callbacks);
    }
    zlist_destroy(&(*agent)->activate_callbacks);
    igs_agent_event_wrapper_t *event_cb = zlist_first((*agent)->agent_event_callbacks);
    while (event_cb) {
        event_cb->callback_ptr = NULL;
        free(event_cb);
        event_cb = zlist_next((*agent)->agent_event_callbacks);
    }
    zlist_destroy(&(*agent)->agent_event_callbacks);
    igs_mute_wrapper_t *mute_cb = zlist_first((*agent)->mute_callbacks);
    while (mute_cb) {
        mute_cb->callback_ptr = NULL;
        free(mute_cb);
        mute_cb = zlist_next((*agent)->mute_callbacks);
    }
    zlist_destroy(&(*agent)->mute_callbacks);
    
    if ((*agent)->mapping)
        mapping_free_mapping (&(*agent)->mapping);
    if ((*agent)->definition)
        definition_free_definition (&(*agent)->definition);
    free (*agent);
    *agent = NULL;
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

igs_result_t igsagent_activate (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    agent->context = core_context;
    if (agent->context->rt_current_microseconds != INT64_MIN)
        agent->rt_timestamps_enabled = true;
    igsagent_t *a = zhashx_lookup(core_context->agents, agent->uuid);
    if (a) {
        igsagent_warn (agent, "agent %s (%s) is already activated", agent->definition->name, agent->uuid);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    agent->network_need_to_send_definition_update = true; // will also trigger mapping update
    agent->network_activation_during_runtime = true;
    zhashx_insert (core_context->agents, agent->uuid, agent);
    
    if (agent->context && agent->context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zyre_join (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }
    
    zlistx_t *local_agents = zhashx_values(core_context->agents);
    zlistx_t *remote_agents = zhashx_values(core_context->remote_agents);
    zlist_t *agent_event_callbacks = zlist_dup(agent->agent_event_callbacks);
    
    // notify this agent with all the other agents already present in our context locally and remotely
    //NB: checking agent, local and remote UUIDs in the conditions below is a way to ensure they have
    //not been destroyed while we are iterating.
    igsagent_t *local = zlistx_first(local_agents);
    while (agent->uuid && local) {
        if (local->uuid && agent->uuid && !streq (local->uuid, agent->uuid)) {
            igs_agent_event_wrapper_t *agent_event_wrapper_cb = zlist_first(agent_event_callbacks);
            while (agent_event_wrapper_cb && agent->uuid) {
                model_read_write_unlock(__FUNCTION__, __LINE__);
                if (agent->uuid && local->uuid)
                    agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_ENTERED, local->uuid,
                                                          local->definition->name, local->definition->json, agent_event_wrapper_cb->my_data);
                // in our local context, other agents already know us
                if (agent->uuid && local->uuid)
                    agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_KNOWS_US, local->uuid,
                                                          local->definition->name, NULL, agent_event_wrapper_cb->my_data);
                model_read_write_lock(__FUNCTION__, __LINE__);
                //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?
                agent_event_wrapper_cb = zlist_next(agent_event_callbacks);
            }
        }
        local = zlistx_next(local_agents);
    }
    igs_remote_agent_t *remote = zlistx_first(remote_agents);
    while (agent->uuid && remote && agent->uuid) {
        igs_agent_event_wrapper_t *cb = zlist_first(agent_event_callbacks);
        while (remote->uuid && cb && agent->uuid) {
            model_read_write_unlock(__FUNCTION__, __LINE__);
            if (agent->uuid && remote->uuid)
                cb->callback_ptr (agent, IGS_AGENT_ENTERED, remote->uuid,
                                  remote->definition->name, remote->definition->json, cb->my_data);
            model_read_write_lock(__FUNCTION__, __LINE__);
            //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?
            cb = zlist_next(agent_event_callbacks);
        }
        remote = zlistx_next(remote_agents);
    }
    zlistx_destroy(&local_agents);
    zlistx_destroy(&remote_agents);
    zlist_destroy(&agent_event_callbacks);
    char *uuid = strdup(agent->uuid);
    char *name = strdup(agent->definition->name);
    char *json = strdup(agent->definition->json);
    
    model_read_write_unlock(__FUNCTION__, __LINE__);
    // notify all other agents inside our context that we have activated
    agent_LOCKED_propagate_agent_event (IGS_AGENT_ENTERED, uuid, name, json);
    agent_LOCKED_propagate_agent_event (IGS_AGENT_KNOWS_US, uuid, name, NULL);
    //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?
    model_read_write_lock(__FUNCTION__, __LINE__);
    
    free(uuid);
    free(name);
    free(json);
    
    if (agent->uuid){
        zlist_t *activate_callbacks = zlist_dup(agent->activate_callbacks);
        igsagent_wrapper_t *agent_wrapper_cb = zlist_first(activate_callbacks);
        while (agent->uuid && agent_wrapper_cb && agent_wrapper_cb->callback_ptr) {
            model_read_write_unlock(__FUNCTION__, __LINE__);
            if (agent->uuid)
                agent_wrapper_cb->callback_ptr (agent, true, agent_wrapper_cb->my_data);
            model_read_write_lock(__FUNCTION__, __LINE__);
            agent_wrapper_cb = zlist_next(activate_callbacks);
        }
        zlist_destroy(&activate_callbacks);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    
    return IGS_SUCCESS;
}

igs_result_t igsagent_deactivate (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return IGS_FAILURE;
    model_read_write_lock(__FUNCTION__, __LINE__);
    igsagent_t *a = zhashx_lookup(core_context->agents, agent->uuid);
    if (!a) {
        igsagent_warn (agent, "agent %s (%s) is unknown or not activated", agent->definition->name, agent->uuid);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        return IGS_SUCCESS;
    }
    if (agent->context && agent->context->network_actor && agent->context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, REMOTE_AGENT_EXIT_MSG);
        zmsg_addstr (msg, agent->uuid);
        zmsg_addstr (msg, agent->definition->name);
        zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
        zyre_leave (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }

    zhashx_delete(core_context->agents, agent->uuid);
    agent->context = NULL;
    char *uuid = strdup(agent->uuid);
    char *name = strdup(agent->definition->name);
    
    zlist_t *activate_callbacks = zlist_dup(agent->activate_callbacks);
    igsagent_wrapper_t *cb = zlist_first(activate_callbacks);
    while (cb && agent->uuid) {
        model_read_write_unlock(__FUNCTION__, __LINE__);
        if (agent->uuid)
            cb->callback_ptr (agent, false, cb->my_data);
        model_read_write_lock(__FUNCTION__, __LINE__);
        cb = zlist_next(activate_callbacks);
    }
    zlist_destroy(&activate_callbacks);
    
    model_read_write_unlock(__FUNCTION__, __LINE__);
    agent_LOCKED_propagate_agent_event (IGS_AGENT_EXITED, uuid, name, NULL);
    free(uuid);
    free(name);
    return IGS_SUCCESS;
}

bool igsagent_is_activated (igsagent_t *agent)
{
    assert (agent);
    if (!agent->uuid)
        return false;
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool is_activated = false;
    igsagent_t *a = zhashx_lookup(core_context->agents, agent->uuid);
    if (a)
        is_activated = true;
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return is_activated;
}

void igsagent_observe (igsagent_t *agent, igsagent_fn cb, void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igsagent_wrapper_t *new_cb = (igsagent_wrapper_t *) zmalloc (sizeof (igsagent_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    zlist_append(agent->activate_callbacks, new_cb);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_observe_agent_events (igsagent_t *agent,
                                    igsagent_agent_events_fn cb,
                                    void *my_data)
{
    assert (agent);
    if (!agent->uuid)
        return;
    assert (cb);
    core_init_context ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_agent_event_wrapper_t *new_cb = (igs_agent_event_wrapper_t *) zmalloc (sizeof (igs_agent_event_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    zlist_append(agent->agent_event_callbacks, new_cb);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igsagent_log (igs_log_level_t level,
                    const char *function,
                    igsagent_t *agent,
                    const char *format,
                    ...)
{
    assert (function);
    assert (agent);
    if (!agent->uuid)
        return;
    assert (format);
    va_list list;
    va_start (list, format);
    char content[IGS_MAX_LOG_LENGTH] = "";
    vsnprintf (content, IGS_MAX_LOG_LENGTH - 1, format, list);
    va_end (list);
    admin_log (agent, level, function, "%s", content);
}

int64_t igsagent_rt_get_current_timestamp(igsagent_t *agent){
    assert(agent);
    if (!agent->uuid)
        return 0;
    return agent->rt_current_timestamp_microseconds;
}

void igsagent_rt_set_timestamps(igsagent_t *agent, bool enable){
    assert(agent);
    if (!agent->uuid)
        return;
    agent->rt_timestamps_enabled = enable;
}

bool igsagent_rt_timestamps(igsagent_t *agent){
    assert(agent);
    if (!agent->uuid)
        return false;
    return agent->rt_timestamps_enabled;
}

void igsagent_rt_set_synchronous_mode(igsagent_t *agent, bool enable){
    assert(agent);
    if (!agent->uuid)
        return;
    agent->rt_synchronous_mode_enabled = enable;
}

bool igsagent_rt_synchronous_mode(igsagent_t *agent){
    assert(agent);
    if (!agent->uuid)
        return false;
    return agent->rt_synchronous_mode_enabled;
}

//DEPRECATED functions management for parameters
igs_result_t igsagent_parameter_create(igsagent_t *self, const char *name,
                                  igs_iop_value_type_t value_type,
                                  void *value,
                                  size_t size){
    igs_warn("this function is deprecated, please use igsagent_attribute_create instead.");
    return igsagent_attribute_create(self, name, value_type, value, size);
 }
igs_result_t igsagent_parameter_remove(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_remove instead.");
    return igsagent_attribute_remove(self, name);
}
igs_iop_value_type_t igsagent_parameter_type(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_type instead.");
    return igsagent_attribute_type(self, name);
}
size_t igsagent_parameter_count(igsagent_t *self){
    igs_warn("this function is deprecated, please use igsagent_attribute_count instead.");
    return igsagent_attribute_count(self);
}
char** igsagent_parameter_list(igsagent_t *self, size_t *parameters_nbr){
    igs_warn("this function is deprecated, please use igsagent_attribute_list instead.");
    return igsagent_attribute_list(self, parameters_nbr);
}
bool igsagent_parameter_exists(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_exists instead.");
    return igsagent_attribute_exists(self, name);
}
bool igsagent_parameter_bool(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_bool instead.");
    return igsagent_attribute_bool(self, name);
}
int igsagent_parameter_int(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_int instead.");
    return igsagent_attribute_int(self, name);
}
double igsagent_parameter_double(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_double instead.");
    return igsagent_attribute_double(self, name);
}
char * igsagent_parameter_string(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_attribute_string instead.");
    return igsagent_attribute_string(self, name);
}
igs_result_t igsagent_parameter_data(igsagent_t *self, const char *name, void **data, size_t *size){
    igs_warn("this function is deprecated, please use igsagent_attribute_data instead.");
    return igsagent_attribute_data(self, name, data, size);
}
igs_result_t igsagent_parameter_set_bool(igsagent_t *self, const char *name, bool value){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_bool instead.");
    return igsagent_attribute_set_bool(self, name, value);
}
igs_result_t igsagent_parameter_set_int(igsagent_t *self, const char *name, int value){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_int instead.");
    return igsagent_attribute_set_int(self, name, value);
}
igs_result_t igsagent_parameter_set_double(igsagent_t *self, const char *name, double value){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_double instead.");
    return igsagent_attribute_set_double(self, name, value);
}
igs_result_t igsagent_parameter_set_string(igsagent_t *self, const char *name, const char *value){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_string instead.");
    return igsagent_attribute_set_string(self, name, value);
}
igs_result_t igsagent_parameter_set_data(igsagent_t *self, const char *name, void *value, size_t size){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_data instead.");
    return igsagent_attribute_set_data(self, name, value, size);
}
igs_result_t igsagent_parameter_add_constraint(igsagent_t *self, const char *name, const char *constraint){
    igs_warn("this function is deprecated, please use igsagent_attribute_add_constraint instead.");
    return igsagent_attribute_add_constraint(self, name, constraint);
}
igs_result_t igsagent_parameter_set_description(igsagent_t *self, const char *name, const char *description){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_description instead.");
    return igsagent_attribute_set_description(self, name, description);
}
igs_result_t igsagent_parameter_set_detailed_type(igsagent_t *self, const char *param_name, const char *type_name, const char *specification){
    igs_warn("this function is deprecated, please use igsagent_attribute_set_detailed_type instead.");
    return igsagent_attribute_set_detailed_type(self, param_name, type_name, specification);
}
void igsagent_clear_parameter(igsagent_t *self, const char *name){
    igs_warn("this function is deprecated, please use igsagent_clear_attribute instead.");
    igsagent_clear_attribute(self, name);
}
void igsagent_observe_parameter(igsagent_t *self, const char *name, igsagent_iop_fn cb, void *my_data){
    igs_warn("this function is deprecated, please use igsagent_observe_attribute instead.");
    igsagent_observe_attribute(self, name, cb, my_data);
}
