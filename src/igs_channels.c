/*  =========================================================================
    channels - using zyre through ingescape

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "ingescape.h"
#include "ingescape_private.h"
#include <stdio.h>

#ifndef W_OK
#define W_OK 02
#endif

igs_mutex_t bus_zyre_mutex;
static bool s_bus_zyre_mutex_initialized = false;

////////////////////////////////////////////////////////////////////////
// INTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////

void s_lock_zyre_peer (void)
{
    if (!s_bus_zyre_mutex_initialized) {
        IGS_MUTEX_INIT (bus_zyre_mutex);
        s_bus_zyre_mutex_initialized = true;
    }
    IGS_MUTEX_LOCK (bus_zyre_mutex);
}

void s_unlock_zyre_peer (void)
{
    assert (s_bus_zyre_mutex_initialized);
    IGS_MUTEX_UNLOCK (bus_zyre_mutex);
}

////////////////////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
////////////////////////////////////////////////////////////////////////
igs_result_t igs_channel_join (const char *channel)
{
    core_init_context ();
    assert (channel && strlen (channel) > 0);
    if (streq (IGS_PRIVATE_CHANNEL, channel)) {
        igs_error ("channel name %s is reserved and cannot be joined", channel);
        return IGS_FAILURE;
    }
    if (core_context->network_actor != NULL && core_context->node != NULL) {
        s_lock_zyre_peer ();
        zyre_join (core_context->node, channel);
        s_unlock_zyre_peer ();
    } else {
        igs_error ("Ingescape must be started before joining a channel");
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void igs_channel_leave (const char *channel)
{
    core_init_context ();
    assert (channel && strlen (channel) > 0);
    if (core_context->network_actor != NULL && core_context->node != NULL) {
        s_lock_zyre_peer ();
        zyre_leave (core_context->node, channel);
        s_unlock_zyre_peer ();
    }
    else
        igs_warn ("Ingescape is not started, this command will be ignored");
}

igs_result_t igs_channel_shout_str (const char *channel, const char *msg, ...)
{
    core_init_context ();
    assert (channel);
    assert (msg);
    if (streq (IGS_PRIVATE_CHANNEL, channel)) {
        igs_error ("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    igs_result_t res = IGS_SUCCESS;
    char content[IGS_MAX_STRING_MSG_LENGTH] = "";
    va_list list;
    va_start (list, msg);
    vsnprintf (content, IGS_MAX_STRING_MSG_LENGTH - 1, msg, list);
    va_end (list);
    s_lock_zyre_peer ();
    if (zyre_shouts (core_context->node, channel, "%s", content) != 0)
        res = IGS_FAILURE;
    s_unlock_zyre_peer ();
    return res;
}

igs_result_t igs_channel_shout_data (const char *channel, void *data, size_t size)
{
    core_init_context ();
    assert (channel);
    if (streq (IGS_PRIVATE_CHANNEL, channel)) {
        igs_error ("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    igs_result_t res = IGS_SUCCESS;
    zframe_t *frame = zframe_new (data, size);
    zmsg_t *msg = zmsg_new ();
    zmsg_append (msg, &frame);
    s_lock_zyre_peer ();
    if (zyre_shout (core_context->node, channel, &msg) != 0)
        res = IGS_FAILURE;
    s_unlock_zyre_peer ();
    return res;
}

igs_result_t igs_channel_shout_zmsg (const char *channel, zmsg_t **msg_p)
{
    core_init_context ();
    assert (channel);
    assert (msg_p);
    assert (*msg_p);
    if (streq (IGS_PRIVATE_CHANNEL, channel)) {
        igs_error ("channel name %s is reserved and cannot be used", channel);
        return IGS_FAILURE;
    }
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    igs_result_t res = IGS_SUCCESS;
    s_lock_zyre_peer ();
    if (zyre_shout (core_context->node, channel, msg_p) != 0)
        res = IGS_FAILURE;
    s_unlock_zyre_peer ();
    return res;
}

igs_result_t igs_channel_whisper_str (const char *agent_name_or_agent_id_or_peerid, const char *msg, ...)
{
    core_init_context ();
    assert (agent_name_or_agent_id_or_peerid);
    assert (msg);
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool has_sent = false;
    int res = IGS_SUCCESS;
    // we iterate first on agents
    igs_remote_agent_t *agent, *tmp_agent;
    HASH_ITER (hh, core_context->remote_agents, agent, tmp_agent)
    {
        if (streq (agent->definition->name, agent_name_or_agent_id_or_peerid)
            || streq (agent->uuid, agent_name_or_agent_id_or_peerid)) {
            char content[IGS_MAX_STRING_MSG_LENGTH] = "";
            va_list list;
            va_start (list, msg);
            vsnprintf (content, IGS_MAX_STRING_MSG_LENGTH - 1, msg, list);
            va_end (list);
            s_lock_zyre_peer ();
            zmsg_t *msg_to_send = zmsg_new ();
            zmsg_addstr (msg_to_send, content);
            zmsg_addstr (msg_to_send, agent->uuid);
            if (zyre_whisper (core_context->node, agent->peer->peer_id, &msg_to_send) != 0)
                res = IGS_FAILURE;
            s_unlock_zyre_peer ();
            has_sent = true;
            // NB: no break to support multiple agents with same name
        }
    }

    // if no agent found, we iterate on peers
    if (!has_sent) {
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER (hh, core_context->zyre_peers, el, tmp)
        {
            if (streq (el->name, agent_name_or_agent_id_or_peerid)
                || streq (el->peer_id, agent_name_or_agent_id_or_peerid)) {
                char content[IGS_MAX_STRING_MSG_LENGTH] = "";
                va_list list;
                va_start (list, msg);
                vsnprintf (content, IGS_MAX_STRING_MSG_LENGTH - 1, msg, list);
                va_end (list);
                s_lock_zyre_peer ();
                if (zyre_whispers (core_context->node, el->peer_id, "%s", content) != 0)
                    res = IGS_FAILURE;
                s_unlock_zyre_peer ();
            }
        }
    }
    return res;
}

igs_result_t igs_channel_whisper_data (const char *agent_name_or_agent_id_or_peerid, void *data, size_t size)
{
    core_init_context ();
    assert (agent_name_or_agent_id_or_peerid);
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool has_sent = false;
    igs_result_t res = IGS_SUCCESS;
    // we iterate first on agents
    igs_remote_agent_t *agent, *tmp_agent;
    HASH_ITER (hh, core_context->remote_agents, agent, tmp_agent)
    {
        if (streq (agent->definition->name, agent_name_or_agent_id_or_peerid)
            || streq (agent->uuid, agent_name_or_agent_id_or_peerid)) {
            zframe_t *frame = zframe_new (data, size);
            zmsg_t *msg = zmsg_new ();
            zmsg_append (msg, &frame);
            zmsg_addstr (msg, agent->uuid);
            s_lock_zyre_peer ();
            if (zyre_whisper (core_context->node, agent->peer->peer_id, &msg) != 0)
                res = IGS_FAILURE;
            s_unlock_zyre_peer ();
            has_sent = true;
            // NB: no break to support multiple agents with same name
        }
    }

    // if no agent found, we iterate on peers
    if (!has_sent) {
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER (hh, core_context->zyre_peers, el, tmp)
        {
            if (streq (el->name, agent_name_or_agent_id_or_peerid)
                || streq (el->peer_id, agent_name_or_agent_id_or_peerid)) {
                zframe_t *frame = zframe_new (data, size);
                zmsg_t *msg = zmsg_new ();
                zmsg_append (msg, &frame);
                s_lock_zyre_peer ();
                if (zyre_whisper (core_context->node, el->peer_id, &msg) != 0)
                    res = IGS_FAILURE;
                s_unlock_zyre_peer ();
            }
        }
    }
    return res;
}

igs_result_t igs_channel_whisper_zmsg (const char *agent_name_or_agent_id_or_peer_id, zmsg_t **msg_p)
{
    core_init_context ();
    assert (agent_name_or_agent_id_or_peer_id);
    assert (msg_p);
    assert (*msg_p);
    if (core_context->node == NULL) {
        igs_error ("Ingescape must be started before trying to send a message");
        return IGS_FAILURE;
    }
    bool has_sent = false;
    igs_result_t res = IGS_SUCCESS;
    // we iterate first on agents
    igs_remote_agent_t *agent, *tmp_agent;
    HASH_ITER (hh, core_context->remote_agents, agent, tmp_agent)
    {
        if (streq (agent->definition->name, agent_name_or_agent_id_or_peer_id)
            || streq (agent->uuid, agent_name_or_agent_id_or_peer_id)) {
            zmsg_t *dup = zmsg_dup (*msg_p);
            zmsg_addstr ( dup, agent->uuid); // add agent uuid at the end of the message
            s_lock_zyre_peer ();
            if (zyre_whisper (core_context->node, agent->peer->peer_id, &dup) != 0)
                res = IGS_FAILURE;
            s_unlock_zyre_peer ();
            has_sent = true;
            // NB: no break to support multiple agents with same name
        }
    }

    // if no agent found, we iterate on peers
    if (!has_sent) {
        igs_zyre_peer_t *el, *tmp;
        HASH_ITER (hh, core_context->zyre_peers, el, tmp)
        {
            if (streq (el->name, agent_name_or_agent_id_or_peer_id)
                || streq (el->peer_id, agent_name_or_agent_id_or_peer_id)) {
                zmsg_t *dup = zmsg_dup (*msg_p);
                s_lock_zyre_peer ();
                if (zyre_whisper (core_context->node, el->peer_id, &dup) != 0)
                    res = IGS_FAILURE;
                s_unlock_zyre_peer ();
                has_sent = true;
            }
        }
    }

    if (has_sent)
        zmsg_destroy (msg_p);
    return res;
}

igs_result_t igs_peer_add_header (const char *key, const char *value)
{
    core_init_context ();
    assert (key);
    assert (value);
    if (strneq (key, "publisher") && strneq (key, "logger")
        && strneq (key, "pid") && strneq (key, "commandline")
        && strneq (key, "hostname")) {
        igs_peer_header_t *header;
        HASH_FIND_STR (core_context->peer_headers, key, header);
        if (header != NULL) {
            igs_error ("service key '%s' already defined : new value will be ignored", key);
            return IGS_FAILURE;
        }
        header = (igs_peer_header_t *) zmalloc (sizeof (igs_peer_header_t));
        header->key = s_strndup (key, IGS_MAX_STRING_MSG_LENGTH);
        header->value = s_strndup (value, IGS_MAX_STRING_MSG_LENGTH * 16);
        HASH_ADD_STR (core_context->peer_headers, key, header);
    }
    else {
        igs_error ("service key '%s' is reserved and cannot be edited", key);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

igs_result_t igs_peer_remove_header (const char *key)
{
    core_init_context ();
    assert (key);
    if (strneq (key, "publisher") && strneq (key, "logger")
        && strneq (key, "pid") && strneq (key, "commandline")
        && strneq (key, "hostname")) {
        igs_peer_header_t *header;
        HASH_FIND_STR (core_context->peer_headers, key, header);
        if (header == NULL) {
            igs_error ("service key '%s' does not exist", key);
            return IGS_FAILURE;
        }
        HASH_DEL (core_context->peer_headers, header);
        free (header->key);
        free (header->value);
        free (header);
        if (core_context != NULL && core_context->node != NULL) {
            igs_error ("agent is started : restart the agent to actually remove the service description");
            return IGS_FAILURE;
        }
    }
    else {
        igs_error ("service key '%s' is reserved and cannot be removed", key);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}
