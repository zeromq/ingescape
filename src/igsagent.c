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
            DL_FOREACH (agent->agent_event_callbacks, cb){
                cb->callback_ptr (agent, event, uuid, name, event_data, cb->my_data);
            }
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
    igsagent_clear_definition (
      agent); // set valid but empty definition, preserve name
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
    model_read_write_lock ();
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
    DL_FOREACH_SAFE ((*agent)->activate_callbacks, activate_cb, activatetmp)
    {
        DL_DELETE ((*agent)->activate_callbacks, activate_cb);
        free (activate_cb);
    }
    igs_mute_wrapper_t *mute_cb, *mutetmp;
    DL_FOREACH_SAFE ((*agent)->mute_callbacks, mute_cb, mutetmp)
    {
        DL_DELETE ((*agent)->mute_callbacks, mute_cb);
        free (mute_cb);
    }
    igs_agent_event_wrapper_t *event_cb, *eventtmp;
    DL_FOREACH_SAFE ((*agent)->agent_event_callbacks, event_cb, eventtmp)
    {
        DL_DELETE ((*agent)->agent_event_callbacks, event_cb);
        free (event_cb);
    }
    if ((*agent)->mapping)
        mapping_free_mapping (&(*agent)->mapping);
    if ((*agent)->definition)
        definition_free_definition (&(*agent)->definition);
    free (*agent);
    *agent = NULL;
    model_read_write_unlock ();
}

igs_result_t igsagent_activate (igsagent_t *agent)
{
    assert (agent);
    igsagent_t *a = NULL;
    HASH_FIND_STR (core_context->agents, agent->uuid, a);
    if (a != NULL) {
        igsagent_error (agent, "agent %s (%s) is already activated",
                         agent->definition->name, agent->uuid);
        return IGS_FAILURE;
    }
    agent->context = core_context;
    agent->network_need_to_send_definition_update =
      true; // will also trigger mapping update
    agent->network_activation_during_runtime = true;
    HASH_ADD_STR (core_context->agents, uuid, agent);
    igsagent_wrapper_t *agent_wrapper_cb;
    DL_FOREACH (agent->activate_callbacks, agent_wrapper_cb)
    {
        agent_wrapper_cb->callback_ptr (agent, true, agent_wrapper_cb->my_data);
    }

    if (agent->context && agent->context->node) {
        s_lock_zyre_peer ();
        zyre_join (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer ();
    }

    // notify all other agents inside this context that we have arrived
    s_agent_propagate_agent_event (IGS_AGENT_ENTERED, agent->uuid,
                                   agent->definition->name, NULL);
    s_agent_propagate_agent_event (IGS_AGENT_KNOWS_US, agent->uuid,
                                   agent->definition->name, NULL);

    // notify this agent with all the other agents already present in our context
    // locally and remotely
    igsagent_t *tmp;
    HASH_ITER (hh, core_context->agents, a, tmp)
    {
        if (!streq (a->uuid, agent->uuid)) {
            igs_agent_event_wrapper_t *agent_event_wrapper_cb;
            DL_FOREACH (agent->agent_event_callbacks, agent_event_wrapper_cb)
            {
                agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_ENTERED, a->uuid,
                                                      a->definition->name, NULL, agent_event_wrapper_cb->my_data);
                // in our local context, other agents already know us
                agent_event_wrapper_cb->callback_ptr (agent, IGS_AGENT_KNOWS_US, a->uuid,
                                                      a->definition->name, NULL, agent_event_wrapper_cb->my_data);
            }
        }
    }
    igs_remote_agent_t *r, *rtmp;
    HASH_ITER (hh, core_context->remote_agents, r, rtmp)
    {
        igs_agent_event_wrapper_t *cb;
        DL_FOREACH (agent->agent_event_callbacks, cb)
        {
            cb->callback_ptr (agent, IGS_AGENT_ENTERED, r->uuid,
                              r->definition->name, NULL, cb->my_data);
        }
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
    HASH_DEL (core_context->agents, agent);
    igsagent_wrapper_t *cb;
    DL_FOREACH (agent->activate_callbacks, cb)
    {
        cb->callback_ptr (agent, false, cb->my_data);
    }
    if (agent->context && agent->context->network_actor
        && agent->context->node) {
        s_lock_zyre_peer ();
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, REMOTE_AGENT_EXIT_MSG);
        zmsg_addstr (msg, agent->uuid);
        zmsg_addstr (msg, agent->definition->name);
        zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
        zyre_leave (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer ();
    }
    agent->context = NULL;

    s_agent_propagate_agent_event (IGS_AGENT_EXITED, agent->uuid,
                                   agent->definition->name, NULL);
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
