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
#include "uthash/uthash.h"


void s_agent_propagate_agent_event (igs_agent_event_t event,
                                    const char *uuid,
                                    const char *name,
                                    void *event_data)
{
    // propagate event on all local agents
    igsagent_t *agent, *tmp;
    HASH_ITER (hh, core_context->agents, agent, tmp){
        if (!streq (uuid, agent->uuid)) {
            igs_agent_event_wrapper_t *cb;
            DL_FOREACH (agent->agent_event_callbacks, cb)
                cb->callback_ptr (agent, event, uuid, name, event_data, cb->my_data);
        }
    }
}

igsagent_t *igsagent_new (const char *name, bool activate_immediately)
{
    assert (name);
    core_init_context ();
    igsagent_t *agent = (igsagent_t *) zmalloc (sizeof (igsagent_t));
    zuuid_t *uuid = zuuid_new ();
    agent->uuid = strdup (zuuid_str (uuid));
    zuuid_destroy (&uuid);
    igsagent_clear_definition (agent); // set valid but empty definition, preserve name
    igsagent_set_name (agent, name);
    assert (agent->definition);
    igsagent_clear_mappings (agent); // set valid but empty mapping
    zhash_insert (core_context->created_agents, agent->uuid, agent);
    if (activate_immediately)
        igsagent_activate (agent);
    return agent;
}

void igsagent_destroy (igsagent_t **agent)
{
    assert (agent);
    assert (*agent);
    model_read_write_lock (__FUNCTION__, __LINE__);
    if (igsagent_is_activated (*agent))
        igsagent_deactivate (*agent);

    zhash_delete (core_context->created_agents, (*agent)->uuid);
    if ((*agent)->uuid) {
        free ((*agent)->uuid);
        (*agent)->uuid = NULL;
    }
    if ((*agent)->state)
        free ((*agent)->state);
    if ((*agent)->definition_path)
        free ((*agent)->definition_path);
    if ((*agent)->mapping_path)
        free ((*agent)->mapping_path);
    if ((*agent)->igs_channel)
        free ((*agent)->igs_channel);

    igsagent_wrapper_t *activate_cb, *activatetmp;
    DL_FOREACH_SAFE ((*agent)->activate_callbacks, activate_cb, activatetmp){
        DL_DELETE ((*agent)->activate_callbacks, activate_cb);
        free (activate_cb);
    }
    igs_mute_wrapper_t *mute_cb, *mutetmp;
    DL_FOREACH_SAFE ((*agent)->mute_callbacks, mute_cb, mutetmp){
        DL_DELETE ((*agent)->mute_callbacks, mute_cb);
        free (mute_cb);
    }
    igs_agent_event_wrapper_t *event_cb, *eventtmp;
    DL_FOREACH_SAFE ((*agent)->agent_event_callbacks, event_cb, eventtmp){
        DL_DELETE ((*agent)->agent_event_callbacks, event_cb);
        free (event_cb);
    }
    if ((*agent)->mapping)
        mapping_free_mapping (&(*agent)->mapping);
    if ((*agent)->definition)
        definition_free_definition (&(*agent)->definition);
    free (*agent);
    *agent = NULL;
    model_read_write_unlock (__FUNCTION__, __LINE__);
}

igs_result_t igsagent_activate (igsagent_t *agent)
{
    assert (agent);
    agent->context = core_context;
    if (agent->context->rt_current_microseconds != INT64_MIN)
        agent->rt_timestamps_enabled = true;
    igsagent_t *a = NULL;
    HASH_FIND_STR (core_context->agents, agent->uuid, a);
    if (a) {
        igsagent_warn (agent, "agent %s (%s) is already activated",
                       agent->definition->name, agent->uuid);
        return IGS_SUCCESS;
    }
    agent->network_need_to_send_definition_update = true; // will also trigger mapping update
    agent->network_activation_during_runtime = true;
    HASH_ADD_STR (core_context->agents, uuid, agent);
    igsagent_wrapper_t *agent_wrapper_cb;
    DL_FOREACH (agent->activate_callbacks, agent_wrapper_cb)
        agent_wrapper_cb->callback_ptr (agent, true, agent_wrapper_cb->my_data);

    if (agent->context && agent->context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zyre_join (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }

    // notify all other agents inside our context that we have arrived
    s_agent_propagate_agent_event (IGS_AGENT_ENTERED, agent->uuid,
                                   agent->definition->name, agent->definition->json);
    s_agent_propagate_agent_event (IGS_AGENT_KNOWS_US, agent->uuid,
                                   agent->definition->name, NULL);
    //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?

    // notify this agent with all the other agents already present in our context
    // locally and remotely
    igsagent_t *tmp;
    HASH_ITER (hh, core_context->agents, a, tmp){
        if (!streq (a->uuid, agent->uuid)) {
            igs_agent_event_wrapper_t *agent_event_wrapper_cb;
            DL_FOREACH (agent->agent_event_callbacks, agent_event_wrapper_cb){
                agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_ENTERED, a->uuid,
                                                      a->definition->name, a->definition->json, agent_event_wrapper_cb->my_data);
                // in our local context, other agents already know us
                agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_KNOWS_US, a->uuid,
                                                      a->definition->name, NULL, agent_event_wrapper_cb->my_data);
                //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?
            }
        }
    }
    igs_remote_agent_t *r, *rtmp;
    HASH_ITER (hh, core_context->remote_agents, r, rtmp) {
        igs_agent_event_wrapper_t *cb;
        DL_FOREACH (agent->agent_event_callbacks, cb)
            cb->callback_ptr (agent, IGS_AGENT_ENTERED, r->uuid,
                              r->definition->name, r->definition->json, cb->my_data);
        //FIXME: shall we send an IGS_AGENT_UPDATED_MAPPING event here ?
    }
    return IGS_SUCCESS;
}

igs_result_t igsagent_deactivate (igsagent_t *agent)
{
    assert (agent);
    igsagent_t *a = NULL;
    HASH_FIND_STR (core_context->agents, agent->uuid, a);
    if (a == NULL) {
        igsagent_error (agent, "agent %s (%s) is not activated",
                         agent->definition->name, agent->uuid);
        return IGS_FAILURE;
    }
    igsagent_wrapper_t *cb;
    DL_FOREACH (agent->activate_callbacks, cb)
        cb->callback_ptr (agent, false, cb->my_data);
    if (agent->context && agent->context->network_actor
        && agent->context->node) {
        s_lock_zyre_peer (__FUNCTION__, __LINE__);
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, REMOTE_AGENT_EXIT_MSG);
        zmsg_addstr (msg, agent->uuid);
        zmsg_addstr (msg, agent->definition->name);
        zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
        zyre_leave (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    }

    s_agent_propagate_agent_event (IGS_AGENT_EXITED, agent->uuid,
                                   agent->definition->name, NULL);
    HASH_DEL (core_context->agents, agent);
    agent->context = NULL;
    return IGS_SUCCESS;
}

bool igsagent_is_activated (igsagent_t *agent)
{
    assert (agent);
    igsagent_t *a = NULL;
    HASH_FIND_STR (core_context->agents, agent->uuid, a);
    return (a != NULL);
}

void igsagent_observe (igsagent_t *agent, igsagent_fn cb, void *my_data)
{
    assert (agent);
    assert (cb);
    igsagent_wrapper_t *new_cb =
      (igsagent_wrapper_t *) zmalloc (sizeof (igsagent_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    DL_APPEND (agent->activate_callbacks, new_cb);
}

void igsagent_observe_agent_events (igsagent_t *agent,
                                    igsagent_agent_events_fn cb,
                                    void *my_data)
{
    assert (agent);
    assert (cb);
    core_init_context ();
    igs_agent_event_wrapper_t *new_cb = (igs_agent_event_wrapper_t *) zmalloc (
      sizeof (igs_agent_event_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    DL_APPEND (agent->agent_event_callbacks, new_cb);
}

void igsagent_log (igs_log_level_t level,
                    const char *function,
                    igsagent_t *agent,
                    const char *format,
                    ...)
{
    assert (function);
    assert (agent);
    assert (format);
    va_list list;
    va_start (list, format);
    char content[IGS_MAX_STRING_MSG_LENGTH] = "";
    vsnprintf (content, IGS_MAX_STRING_MSG_LENGTH - 1, format, list);
    va_end (list);
    admin_log (agent, level, function, "%s", content);
}

int64_t igsagent_rt_get_current_timestamp(igsagent_t *agent){
    assert(agent);
    return agent->rt_current_timestamp_microseconds;
}

void igsagent_rt_set_timestamps(igsagent_t *agent, bool enable){
    assert(agent);
    agent->rt_timestamps_enabled = enable;
}

bool igsagent_rt_timestamps(igsagent_t *agent){
    assert(agent);
    return agent->rt_timestamps_enabled;
}

void igsagent_rt_set_synchronous_mode(igsagent_t *agent, bool enable){
    assert(agent);
    agent->rt_synchronous_mode_enabled = enable;
}

bool igsagent_rt_synchronous_mode(igsagent_t *agent){
    assert(agent);
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
