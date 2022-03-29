/*  =========================================================================
    network - ingescape network ayer

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <czmq.h>
#include <zyre.h>

#if defined(__UTYPE_LINUX)
#include <unistd.h>
#endif

#if (defined(__UTYPE_IOS) || defined(__UTYPE_OSX))
#include <TargetConditionals.h>
#if defined(__UTYPE_OSX)
#include <libproc.h>
#endif
#endif

#if defined(__UNIX__)
#include <sys/resource.h>
#endif

#include "ingescape.h"
#include "ingescape_private.h"
#include "uthash/uthash.h"
#include "uthash/utlist.h"

#if defined(__WINDOWS__)
#define MALLOC(x) heap_alloc (get_process_heap (), 0, (x))
#define FREE(x) heap_free (get_process_heap (), 0, (x))
#define MAX_TRIES 3

size_t count;
char *friendly_name = NULL;
char buff[100];
DWORD bufflen = 100;

/* Declare and initialize variables */
DWORD dw_ret_val = 0;

unsigned int i = 0;

// Set the flags to pass to get_adapters_addresses
ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

// default to unspecified address family (both)
ULONG family = AF_UNSPEC;

LPVOID lp_msg_buf = NULL;

PIP_ADAPTER_ADDRESSES p_addresses = NULL;
ULONG out_buf_len = 0;
ULONG Iterations = 0;

PIP_ADAPTER_ADDRESSES p_curr_addresses = NULL;
PIP_ADAPTER_UNICAST_ADDRESS p_unicast = NULL;
#endif

#define IGS_DEFAULT_SECURITY_DIRECTORY "*"

#ifndef W_OK
#define W_OK 02
#endif

// Removes filter to 'subscribe' socket for a spectific output of a given remote
// agent
//FIXME UNUSED
void s_unsubscribe_to_remote_agent_output (igs_remote_agent_t *remote_agent,
                                           const char *output_name)
{
    assert (remote_agent);
    assert (output_name);
    if (strlen (output_name) > 0) {
        igs_mapping_filter_t *filter = NULL;
        DL_FOREACH (remote_agent->mapping_filters, filter)
        {
            if (streq (filter->filter, output_name)) {
                assert (remote_agent->peer->subscriber);
                igs_debug ("unsubscribe to agent %s output %s",
                           remote_agent->definition->name, output_name);
                zsock_set_unsubscribe (remote_agent->peer->subscriber,
                                       output_name);
                free (filter->filter);
                DL_DELETE (remote_agent->mapping_filters, filter);
                free (filter);
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// ZMQ callbacks
////////////////////////////////////////////////////////////////////////

// function actually handling messages from one of the remote agents we
// subscribed to
void s_handle_publication_from_remote_agent (zmsg_t *msg,
                                             igs_remote_agent_t *remote_agent)
{
    assert (msg);
    assert (remote_agent);
    assert (remote_agent->context);
    if (remote_agent->context->is_frozen == true) {
        igs_debug ("Message received from %s but all traffic in our process is "
                   "currently frozen",
                   remote_agent->definition->name);
        return;
    }

    model_read_write_lock ();
    // Publication does not provide information about the targeted agents.
    // At this stage, we only know that one or more of our agents are targeted.
    // We need to iterate through our agents and their mapping to check which
    // inputs need to be updated on which agent.
    igsagent_t *agent, *tmp_agent;
    HASH_ITER (hh, remote_agent->context->agents, agent, tmp_agent)
    {
        if (!agent || !agent->uuid || (strlen (agent->uuid) == 0))
            continue;

        zmsg_t *dup = zmsg_dup (msg);
        size_t msg_size = zmsg_size (dup);
        char *output = NULL;
        char *v_type = NULL;
        igs_iop_value_type_t value_type = 0;
        unsigned long i = 0;
        for (i = 0; i < msg_size; i += 3) {
            // Each message part must contain 3 elements
            // 1 : output name
            // 2 : output iopt_type
            // 3 : value of the output as a string or zframe
            output = zmsg_popstr (dup);
            if (output == NULL) {
                igs_error (
                  "output name is NULL in received publication : rejecting");
                break;
            }
            v_type = zmsg_popstr (dup);
            if (v_type == NULL) {
                igs_error (
                  "output type is NULL in received publication : rejecting");
                free (output);
                break;
            }
            value_type = atoi (v_type);
            if (value_type < IGS_INTEGER_T || value_type > IGS_DATA_T) {
                igs_error ("output type is not valid (%d) in received "
                           "publication : rejecting",
                           value_type);
                free (output);
                free (v_type);
                break;
            }
            free (v_type);
            v_type = NULL;

            zframe_t *frame = NULL;
            void *data = NULL;
            size_t size = 0;
            char *value = NULL;
            // get data before iterating to all the mapping elements using it
            if (value_type == IGS_STRING_T) {
                value = zmsg_popstr (dup);
                if (value == NULL) {
                    igs_error (
                      "value is NULL in received publication : rejecting");
                    break;
                }
            }
            else {
                frame = zmsg_pop (dup);
                if (frame == NULL) {
                    igs_error (
                      "value is NULL in received publication : rejecting");
                    break;
                }
                data = zframe_data (frame);
                size = zframe_size (frame);
            }
            // try to find mapping elements matching with this subscriber's output
            // and update mapped input(s) value accordingly
            // TODO : optimize mapping storage to avoid iterating
            igs_map_t *elmt, *tmp;
            // check that this agent has not been destroyed when we were locked
            if (agent->mapping) {
                HASH_ITER (hh, agent->mapping->map_elements, elmt, tmp){
                    if (streq (elmt->to_agent, remote_agent->definition->name)
                        && streq (elmt->to_output, output)) {
                        // we have a match on emitting agent name and its ouput name :
                        // still need to check the targeted input existence in our
                        // definition
                        igs_iop_t *found_input = NULL;
                        if (agent->definition->inputs_table)
                            HASH_FIND_STR (agent->definition->inputs_table,
                                           elmt->from_input, found_input);
                        if (!found_input)
                            igsagent_warn (agent,
                                           "Input %s is missing in our definition but "
                                           "expected in our mapping with %s.%s",
                                           elmt->from_input, elmt->to_agent,
                                           elmt->to_output);
                        else {
                            // we have a fully matching mapping element : write from received
                            // output to our input
                            if (value_type == IGS_STRING_T) {
                                model_read_write_unlock ();
                                model_write_iop (agent, elmt->from_input,
                                                 IGS_INPUT_T, value_type, value,
                                                 strlen (value) + 1);
                                model_read_write_lock ();

                            }
                            else {
                                model_read_write_unlock ();
                                model_write_iop (agent, elmt->from_input,
                                                 IGS_INPUT_T, value_type, data,
                                                 size);
                                model_read_write_lock ();
                            }
                            if (!agent->uuid)
                                break;
                        }
                    }
                }
            }
            if (frame != NULL)
                zframe_destroy (&frame);
            if (value != NULL)
                free (value);
            free (output);
            output = NULL;
        }
        zmsg_destroy (&dup);
    }
    model_read_write_unlock ();
}

// Timer callback to send GET_CURRENT_OUTPUTS notification for an agent we
// subscribed to
int s_trigger_outputs_request_to_newcomer (zloop_t *loop,
                                           int timer_id,
                                           void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (timer_id)
    igs_remote_agent_t *remote_agent = (igs_remote_agent_t *) arg;
    assert (remote_agent);
    assert (remote_agent->context);
    assert (remote_agent->context->node);

    if (remote_agent->shall_send_outputs_request) {
        s_lock_zyre_peer ();
        zmsg_t *msg = zmsg_new ();
        zmsg_addstr (msg, GET_CURRENT_OUTPUTS_MSG);
        zmsg_addstr (msg, remote_agent->uuid);
        zyre_whisper (remote_agent->context->node, remote_agent->peer->peer_id,
                      &msg);
        s_unlock_zyre_peer ();
        remote_agent->shall_send_outputs_request = false;
    }
    return 0;
}

// manage incoming messages from one of the remote agents we subscribed to
int s_manage_remote_publication (zloop_t *loop, zsock_t *socket, void *arg)
{
    IGS_UNUSED (loop)
    igs_core_context_t *context = (igs_core_context_t *) arg;
    assert (socket);
    assert (context);

    zmsg_t *msg = zmsg_recv (socket);
    // The output name now includes the agent uuid as prefix.
    // We merged them to keep the ZeroMQ PUB/SUB filters working
    // in a context where a peer now possibly hosts multiple agents.
    char *output_name = zmsg_popstr (msg);
    if (output_name == NULL) {
        igs_error ("output name is NULL in received publication : rejecting");
        return 0;
    }
    char uuid[IGS_AGENT_UUID_LENGTH + 1] = "";
    if (strlen (output_name) < IGS_AGENT_UUID_LENGTH) {
        igs_error ("output name '%s' is missing information : rejecting",
                   output_name);
        free (output_name);
        return 0;
    }
    snprintf (uuid, IGS_AGENT_UUID_LENGTH + 1, "%s", output_name);
    char *real_output_name = output_name + IGS_AGENT_UUID_LENGTH + 1;

    // NB: We push the output name again at the beginning of
    // the message for proper use by s_handle_publication_from_remote_agent
    zmsg_pushstr (msg, real_output_name);
    free (output_name);

    igs_remote_agent_t *remote_agent = NULL;
    HASH_FIND_STR (context->remote_agents, uuid, remote_agent);
    if (remote_agent == NULL) {
        igs_error ("no remote agent with uuid '%s' : rejecting", uuid);
        return 0;
    }
    s_handle_publication_from_remote_agent (msg, remote_agent);
    zmsg_destroy (&msg);
    return 0;
}

void s_clean_and_free_zyre_peer (igs_zyre_peer_t **zyre_peer, zloop_t *loop)
{
    assert (zyre_peer);
    assert (*zyre_peer);
    assert (loop);
    igs_debug ("cleaning peer %s (%s)", (*zyre_peer)->name,
               (*zyre_peer)->peer_id);
    if ((*zyre_peer)->peer_id != NULL)
        free ((*zyre_peer)->peer_id);
    if ((*zyre_peer)->name != NULL)
        free ((*zyre_peer)->name);
    if ((*zyre_peer)->protocol != NULL)
        free ((*zyre_peer)->protocol);
    if ((*zyre_peer)->subscriber != NULL) {
        zloop_reader_end (loop, (*zyre_peer)->subscriber);
        zsock_destroy (&((*zyre_peer)->subscriber));
    }
    free (*zyre_peer);
    *zyre_peer = NULL;
}

#define NOTIFY_REMOTE_AGENT_TIMER 500

// Adds proper filter to 'subscribe' socket for a spectific output of a given
// remote agent
void s_subscribe_to_remote_agent_output (igs_remote_agent_t *remote_agent,
                                         const char *output_name)
{
    assert (remote_agent);
    assert (output_name);
    if (strlen (output_name) > 0) {
        char filter_value[IGS_MAX_IOP_NAME_LENGTH + IGS_AGENT_UUID_LENGTH + 1] =
          "";
        snprintf (filter_value,
                  IGS_MAX_IOP_NAME_LENGTH + IGS_AGENT_UUID_LENGTH + 1, "%s-%s",
                  remote_agent->uuid, output_name);
        bool filter_already_exists = false;
        igs_mapping_filter_t *filter = NULL;
        DL_FOREACH (remote_agent->mapping_filters, filter)
        {
            if (streq (filter->filter, filter_value)) {
                filter_already_exists = true;
                break;
            }
        }
        if (!filter_already_exists) {
            // Set subscriber to the output filter
            assert (remote_agent->peer->subscriber);
            igs_debug ("subscribe to agent %s output %s (%s)",
                       remote_agent->definition->name, output_name,
                       filter_value);
            zsock_set_subscribe (remote_agent->peer->subscriber, filter_value);
            igs_mapping_filter_t *f = (igs_mapping_filter_t *) zmalloc (
              sizeof (igs_mapping_filter_t));
            f->filter = strdup (filter_value);
            DL_APPEND (remote_agent->mapping_filters, f);
        }
        else {
            // printf("\n****************\nFILTER BIS %s - %s\n***************\n",
            // subscriber->agent->name, output_name);
        }
    }
}

int s_network_configure_mapping_to_remote_agent (
  igsagent_t *agent, igs_remote_agent_t *remote_agent)
{
    assert (agent);
    assert (remote_agent);
    igs_map_t *el, *tmp;
    if (agent->mapping != NULL) {
        HASH_ITER (hh, agent->mapping->map_elements, el, tmp)
        {
            if (streq (remote_agent->definition->name, el->to_agent)
                || streq (el->to_agent, "*")) {
                // mapping element is compatible with subscriber name
                // check if we find a compatible output in subscriber definition
                igs_iop_t *found_output = NULL;
                if (remote_agent->definition != NULL)
                    HASH_FIND_STR (remote_agent->definition->outputs_table,
                                   el->to_output, found_output);

                // check if we find a valid input in our own definition
                igs_iop_t *found_input = NULL;
                if (agent->definition != NULL)
                    HASH_FIND_STR (agent->definition->inputs_table,
                                   el->from_input, found_input);

                // check type compatibility between input and output value types
                // including implicit conversions
                if (found_output != NULL && found_input != NULL
                    && mapping_check_input_output_compatibility (
                      agent, found_input, found_output)) {
                    // we have validated input, agent and output names : we can map
                    // NOTE: the call below may happen several times if our agent uses
                    // the remote agent ouput on several of its inputs. This should not
                    // have any consequence.
                    s_subscribe_to_remote_agent_output (remote_agent,
                                                        el->to_output);

                    // mapping was successful : we set timer to notify remote agent if not
                    // already done
                    if (!remote_agent->shall_send_outputs_request
                        && agent->network_request_outputs_from_mapped_agents) {
                        remote_agent->shall_send_outputs_request = true;
                        remote_agent->timer_id = zloop_timer (
                          core_context->loop, NOTIFY_REMOTE_AGENT_TIMER, 1,
                          s_trigger_outputs_request_to_newcomer, remote_agent);
                    }
                }
                // NOTE: we do not clean subscriptions here because we cannot check if
                // an output is not used in another mapping element
            }
        }
    }
    return 0;
}

void s_send_definition_to_zyre_peer (igsagent_t *agent,
                                     const char *peer,
                                     const char *def,
                                     bool notif)
{
    assert (agent);
    assert (agent->context);
    assert (agent->context->node);
    assert (peer);
    assert (def);
    s_lock_zyre_peer ();
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, EXTERNAL_DEFINITION_MSG);
    zmsg_addstr (msg, def);
    zmsg_addstr (msg, agent->uuid);
    zmsg_addstr (msg, agent->definition->name);
    if (notif) {
        // Agent has been activated during runtime: we must
        // indicate that our peer already knows the distant peer
        zmsg_addstr (msg, "1");
    }
    zyre_whisper (core_context->node, peer, &msg);
    s_unlock_zyre_peer ();
}

void s_send_mapping_to_zyre_peer (igsagent_t *agent,
                                  const char *peer,
                                  const char *mapping)
{
    assert (agent);
    assert (agent->context);
    assert (agent->context->node);
    assert (peer);
    assert (mapping);
    s_lock_zyre_peer ();
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, EXTERNAL_MAPPING_MSG);
    zmsg_addstr (msg, mapping);
    zmsg_addstr (msg, agent->uuid);
    zyre_whisper (core_context->node, peer, &msg);
    s_unlock_zyre_peer ();
}

void s_send_state_to (igsagent_t *agent,
                      const char *peer_or_channel,
                      bool is_for_peer)
{
    assert (agent);
    assert (agent->context);
    assert (agent->context->node);
    assert (peer_or_channel);
    igs_core_context_t *context = agent->context;
    zmsg_t *msg = NULL;

    if (agent->definition && agent->definition->outputs_table) {
        igs_iop_t *current_iop, *tmp_iop;
        HASH_ITER (hh, agent->definition->outputs_table, current_iop, tmp_iop)
        {
            if (current_iop->name) {
                s_lock_zyre_peer ();
                msg = zmsg_new ();
                zmsg_addstr (msg, (current_iop->is_muted) ? OUTPUT_MUTED_MSG
                                                          : OUTPUT_UNMUTED_MSG);
                zmsg_addstr (msg, current_iop->name);
                zmsg_addstr (msg, agent->uuid);
                if (is_for_peer)
                    zyre_whisper (context->node, peer_or_channel, &msg);
                else
                    zyre_shout (context->node, peer_or_channel, &msg);
                s_unlock_zyre_peer ();
            }
        }
    }

    s_lock_zyre_peer ();
    msg = zmsg_new ();
    zmsg_addstr (msg, AGENT_MUTED_MSG);
    zmsg_addstr (msg, (agent->is_whole_agent_muted) ? "1" : "0");
    zmsg_addstr (msg, agent->uuid);
    if (is_for_peer)
        zyre_whisper (context->node, peer_or_channel, &msg);
    else
        zyre_shout (context->node, peer_or_channel, &msg);
    s_unlock_zyre_peer ();

    if (agent->state) {
        s_lock_zyre_peer ();
        msg = zmsg_new ();
        zmsg_addstr (msg, STATE_MSG);
        zmsg_addstr (msg, agent->state);
        zmsg_addstr (msg, agent->uuid);
        if (is_for_peer)
            zyre_whisper (context->node, peer_or_channel, &msg);
        else
            zyre_shout (context->node, peer_or_channel, &msg);
        s_unlock_zyre_peer ();
    }

    if (agent->definition_path) {
        s_lock_zyre_peer ();
        msg = zmsg_new ();
        zmsg_addstr (msg, DEFINITION_FILE_PATH_MSG);
        zmsg_addstr (msg, agent->definition_path);
        zmsg_addstr (msg, agent->uuid);
        if (is_for_peer)
            zyre_whisper (context->node, peer_or_channel, &msg);
        else
            zyre_shout (context->node, peer_or_channel, &msg);
        s_unlock_zyre_peer ();
    }

    if (agent->mapping_path) {
        s_lock_zyre_peer ();
        msg = zmsg_new ();
        zmsg_addstr (msg, MAPPING_FILE_PATH_MSG);
        zmsg_addstr (msg, agent->mapping_path);
        zmsg_addstr (msg, agent->uuid);
        if (is_for_peer)
            zyre_whisper (context->node, peer_or_channel, &msg);
        else
            zyre_shout (context->node, peer_or_channel, &msg);
        s_unlock_zyre_peer ();
    }

    s_lock_zyre_peer ();
    msg = zmsg_new ();
    zmsg_addstr (msg, FROZEN_MSG);
    zmsg_addstr (msg, (context->is_frozen) ? "1" : "0");
    zmsg_addstr (msg, agent->uuid);
    if (is_for_peer)
        zyre_whisper (context->node, peer_or_channel, &msg);
    else
        zyre_shout (context->node, peer_or_channel, &msg);
    s_unlock_zyre_peer ();

    s_lock_zyre_peer ();
    msg = zmsg_new ();
    zmsg_addstr (msg, LOG_IN_STREAM_MSG);
    zmsg_addstr (msg, (context->log_in_stream) ? "1" : "0");
    zmsg_addstr (msg, agent->uuid);
    if (is_for_peer)
        zyre_whisper (context->node, peer_or_channel, &msg);
    else
        zyre_shout (context->node, peer_or_channel, &msg);
    s_unlock_zyre_peer ();

    s_lock_zyre_peer ();
    msg = zmsg_new ();
    zmsg_addstr (msg, LOG_IN_FILE_MSG);
    zmsg_addstr (msg, (context->log_in_file) ? "1" : "0");
    zmsg_addstr (msg, agent->uuid);
    if (is_for_peer)
        zyre_whisper (context->node, peer_or_channel, &msg);
    else
        zyre_shout (context->node, peer_or_channel, &msg);
    s_unlock_zyre_peer ();

    s_lock_zyre_peer ();
    msg = zmsg_new ();
    zmsg_addstr (msg, LOG_FILE_PATH_MSG);
    zmsg_addstr (msg, context->log_file_path);
    zmsg_addstr (msg, agent->uuid);
    if (is_for_peer)
        zyre_whisper (context->node, peer_or_channel, &msg);
    else
        zyre_shout (context->node, peer_or_channel, &msg);
    s_unlock_zyre_peer ();
}

void s_clean_and_free_remote_agent (igs_remote_agent_t **remote_agent)
{
    assert (remote_agent);
    assert (*remote_agent);
    assert ((*remote_agent)->context);
    igs_debug ("cleaning remote agent %s (%s)",
               (*remote_agent)->definition->name, (*remote_agent)->uuid);

    // clean the agent definition & mapping
    if ((*remote_agent)->definition != NULL)
        definition_free_definition (&(*remote_agent)->definition);
    if ((*remote_agent)->mapping != NULL)
        mapping_free_mapping (&(*remote_agent)->mapping);

    // clean the remote_agent itself
    igs_mapping_filter_t *elt, *tmp;
    DL_FOREACH_SAFE ((*remote_agent)->mapping_filters, elt, tmp)
    {
        zsock_set_unsubscribe ((*remote_agent)->peer->subscriber, elt->filter);
        DL_DELETE ((*remote_agent)->mapping_filters, elt);
        free (elt->filter);
        free (elt);
    }
    if ((*remote_agent)->uuid)
        free ((*remote_agent)->uuid);
    if ((*remote_agent)->context->loop != NULL
        && (*remote_agent)->timer_id > 0) {
        zloop_timer_end ((*remote_agent)->context->loop,
                         (*remote_agent)->timer_id);
        (*remote_agent)->timer_id = -2;
    }
    free (*remote_agent);
    *remote_agent = NULL;
}

// manage messages received on the private channel
int s_manage_zyre_incoming (zloop_t *loop, zsock_t *socket, void *arg)
{
    IGS_UNUSED (socket)
    igs_core_context_t *context = (igs_core_context_t *) arg;
    assert (context);
    zyre_t *node = context->node;
    assert (node);

    zyre_event_t *zyre_event = zyre_event_new (node);
    const char *event = zyre_event_type (zyre_event);
    const char *peerUUID = zyre_event_peer_uuid (zyre_event);
    const char *name = zyre_event_peer_name (zyre_event);
    const char *address = zyre_event_peer_addr (zyre_event);
    zhash_t *headers = zyre_event_headers (zyre_event);
    const char *group = zyre_event_group (zyre_event);
    zmsg_t *msg = zyre_event_msg (zyre_event);
    zmsg_t *msg_duplicate = zmsg_dup (msg);

    // parse event
    if (streq (event, "ENTER")) {
        igs_debug (
          "->%s has entered the network with peer id %s and endpoint %s", name,
          peerUUID, address);
        igs_zyre_peer_t *zyre_peer = NULL;
        HASH_FIND_STR (context->zyre_peers, peerUUID, zyre_peer);
        if (zyre_peer == NULL) {
            zyre_peer = (igs_zyre_peer_t *) zmalloc (sizeof (igs_zyre_peer_t));
            zyre_peer->peer_id = s_strndup (peerUUID, IGS_MAX_PEER_ID_LENGTH);
            HASH_ADD_STR (context->zyre_peers, peer_id, zyre_peer);
            zyre_peer->name = s_strndup (name, IGS_MAX_AGENT_NAME_LENGTH);
            zlist_t *keys = zhash_keys (headers);
            size_t s = zlist_size (keys);
            if (s > 0) {
                igs_debug ("Handling headers for peer %s (%s)", name, peerUUID);
                char *k = zlist_first (keys);
                const char *v;
                while (k) {
                    v = zyre_event_header (zyre_event, k);
                    igs_debug ("\t%s -> %s", k, v);
                    k = zlist_next (keys);
                }
                zlist_destroy (&keys);
            }

            const char *peer_public_key = zyre_event_header (zyre_event, "X-PUBLICKEY");
            const char *protocol_version = zyre_event_header (zyre_event, "protocol");
            if (protocol_version)
                zyre_peer->protocol = s_strndup (protocol_version, 16);

            const char *publisher_port = zyre_event_header (zyre_event, "publisher");
            if (publisher_port) {
                // we extract the publisher adress to subscribe to from the zyre message
                // header
                char endpoint_address[128];
                strncpy (endpoint_address, address, 127);

                // IP adress extraction
                char *insert = endpoint_address + strlen (endpoint_address);
                bool extractOK = true;
                while (*insert != ':') {
                    insert--;
                    if (insert == endpoint_address) {
                        igs_error ("Could not extract port from address %s", address);
                        extractOK = false;
                        break;
                    }
                }

                if (extractOK) {
                    // we found a possible publisher to subscribe to
                    *(insert + 1) =
                      '\0'; // close endpoint_address string after ':' location

                    // check towards our own ip address (without port)
                    char *incoming_ip_address =
                      endpoint_address + 6; // ignore tcp://
                    *insert = '\0';
                    bool useIPC = false;
                    bool use_inproc = false;
                    const char *ipc_address = NULL;
                    const char *inproc_address = NULL;
                    if (streq (context->ip_address, incoming_ip_address)) {
                        // same IP address : we can try to use ipc (or loopback on windows)
                        // instead of TCP or we can use inproc if both agents are in the same
                        // process
                        int pid = atoi (zyre_event_header (zyre_event, "pid"));
                        if (context->process_id == pid) {
                            // FIXME: certainly useless with new architecture
                            // same ip address and same process : we can use inproc
                            inproc_address = zyre_event_header (zyre_event, "inproc");
                            if (inproc_address != NULL) {
                                use_inproc = true;
                                igs_debug ("Use address %s to subscribe to %s", inproc_address, name);
                            }
                        }
                        else {
                            // try to recover agent ipc/loopback address
#if defined(__UNIX__)
                            ipc_address = zyre_event_header (zyre_event, "ipc");
#elif defined(__WINDOWS__)
                            ipc_address = zyre_event_header (zyre_event, "loopback");
#endif
                            if (ipc_address != NULL) {
                                useIPC = true;
                                igs_debug ("Use address %s to subscribe to %s", ipc_address, name);
                            }
                        }
                    }
                    *insert = ':';
                    // add port to the endpoint to compose it fully
                    strcat (endpoint_address, publisher_port);
                    if (context->network_allow_inproc && use_inproc) {
                        zyre_peer->subscriber = zsock_new_sub (inproc_address, NULL);
                        zsock_set_rcvhwm (zyre_peer->subscriber, context->network_hwm_value);
                        igs_debug ("Subscription created for %s at %s (inproc)",
                                   zyre_peer->name, inproc_address);
                    }
                    else
                    if (context->network_allow_ipc && useIPC) {
                        zyre_peer->subscriber = zsock_new_sub (ipc_address, NULL);
                        zsock_set_rcvhwm (zyre_peer->subscriber, context->network_hwm_value);
                        igs_debug ("Subscription created for %s at %s (ipc)",
                                   zyre_peer->name, ipc_address);
                    }
                    else {
                        zyre_peer->subscriber = zsock_new_sub (endpoint_address, NULL);
                        zsock_set_rcvhwm (zyre_peer->subscriber, context->network_hwm_value);
                        igs_debug ("Subscription created for %s at %s (tcp)",
                                   zyre_peer->name, endpoint_address);
                    }
                    assert (zyre_peer->subscriber);

                    if (context->security_is_enabled && peer_public_key) {
                        zcert_apply (context->security_cert, zyre_peer->subscriber);
                        zsock_set_curve_serverkey (zyre_peer->subscriber, peer_public_key);
                    }
                    zloop_reader (loop, zyre_peer->subscriber, s_manage_remote_publication, context);
                    zloop_reader_set_tolerant (loop, zyre_peer->subscriber);
                }
            }
            zhash_t *headers_bis = zhash_dup (headers);
            s_agent_propagate_agent_event (IGS_PEER_ENTERED, peerUUID, name, headers_bis);
            zhash_destroy (&headers_bis);
        }
        else {
            // Agent already exists, we set its reconnected flag
            //(this is used below to avoid agent destruction on EXIT received after
            //timeout)
            zyre_peer->reconnected++;
        }
    }
    else
    if (streq (event, "JOIN")) {
        igs_debug ("+%s has joined %s", name, group);
        if (streq (group, IGS_PRIVATE_CHANNEL)) {
            // send information for all our agents to the newcomer
            igs_zyre_peer_t *zyre_peer = NULL;
            HASH_FIND_STR (context->zyre_peers, peerUUID, zyre_peer);
            assert (zyre_peer);

            igsagent_t *agent, *tmp;
            char *definition_str = NULL;
            char *mapping_str = NULL;
            HASH_ITER (hh, context->agents, agent, tmp)
            {
                // definition is sent to every newcomer on the channel (whether it is a
                // ingescape agent or not)
                if (zyre_peer->protocol
                    && (streq (zyre_peer->protocol, "v2")
                        || streq (zyre_peer->protocol, "v3")))
                    definition_str = parser_export_definition_legacy (agent->definition);
                else
                    definition_str =
                      parser_export_definition (agent->definition);
                if (definition_str) {
                    s_send_definition_to_zyre_peer (agent, peerUUID,
                                                    definition_str, false);
                    free (definition_str);
                    definition_str = NULL;
                }
                else
                    s_send_definition_to_zyre_peer (agent, peerUUID, "", false);
                // and so is our mapping
                if (zyre_peer->protocol && streq (zyre_peer->protocol, "v2"))
                    mapping_str = parser_export_mapping_legacy (agent->mapping);
                else
                    mapping_str = parser_export_mapping (agent->mapping);
                if (mapping_str) {
                    s_send_mapping_to_zyre_peer (agent, peerUUID, mapping_str);
                    free (mapping_str);
                    mapping_str = NULL;
                }
                else
                    s_send_mapping_to_zyre_peer (agent, peerUUID, "");
                // and so is the state of our internal variables
                s_send_state_to (agent, peerUUID, true);
            }
            zyre_peer->has_joined_private_channel = true;
        }
    }
    else
    if (streq (event, "SHOUT")) {
        if (streq (group, context->replay_channel)) {
            // this is a replay message for one of our inputs
            char *agent_name = zmsg_popstr (msg_duplicate);
            char *input = zmsg_popstr (msg_duplicate);
            if (agent_name == NULL) {
                igs_error ("agent name is NULL for replay message from %s(%s): "
                           "rejecting",
                           name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            if (input == NULL) {
                igs_error (
                  "input is NULL for replay message from %s(%s): rejecting",
                  name, peerUUID);
                free (agent_name);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }

            char *value = NULL;
            zframe_t *frame = NULL;
            void *data = NULL;
            size_t size = 0;
            igsagent_t *target_agent, *targettmp;
            HASH_ITER (hh, context->agents, target_agent, targettmp)
            {
                if (streq (agent_name, target_agent->definition->name)) {
                    igs_iop_value_type_t input_type =
                      igsagent_input_type (target_agent, input);
                    if (zmsg_size (msg_duplicate) > 0) {
                        igs_debug ("replaying %s.%s", agent_name, input);
                        if (input_type == IGS_STRING_T) {
                            value = zmsg_popstr (msg_duplicate);
                            if (value == NULL) {
                                igs_error ("value is NULL for replay message "
                                           "from %s(%s): rejecting",
                                           name, peerUUID);
                                free (agent_name);
                                free (input);
                                zmsg_destroy (&msg_duplicate);
                                zyre_event_destroy (&zyre_event);
                                return 0;
                            }
                            igsagent_input_set_string (target_agent, input,
                                                        value);
                            free (value);
                        }
                        else {
                            frame = zmsg_pop (msg_duplicate);
                            if (frame == NULL) {
                                igs_error ("value is NULL for replay message "
                                           "from %s(%s): rejecting",
                                           name, peerUUID);
                                free (agent_name);
                                free (input);
                                zmsg_destroy (&msg_duplicate);
                                zyre_event_destroy (&zyre_event);
                                return 0;
                            }
                            data = zframe_data (frame);
                            size = zframe_size (frame);
                            model_write_iop (target_agent, input, IGS_INPUT_T,
                                             input_type, data, size);
                            zframe_destroy (&frame);
                        }
                    }
                    else
                        igsagent_error (target_agent,
                                         "replay message for input %s is not "
                                         "correct and was ignored",
                                         input);
                }
            }
            free (agent_name);
            free (input);

        }
        else
        if (streq (group, IGS_PRIVATE_CHANNEL)) {
            char *title = zmsg_popstr (msg_duplicate);
            if (streq (title, REMOTE_AGENT_EXIT_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                igs_remote_agent_t *remote = NULL;
                HASH_FIND_STR (context->remote_agents, uuid, remote);
                if (remote) {
                    igs_debug ("<-%s (%s) exited", remote->definition->name,
                               uuid);
                    split_remove_worker (context, uuid, NULL);
                    HASH_DEL (context->remote_agents, remote);
                    s_agent_propagate_agent_event (
                      IGS_AGENT_EXITED, uuid, remote->definition->name, NULL);
                    s_clean_and_free_remote_agent (&remote);
                }
                else
                    igs_error ("%s is not a known remote agent", uuid);
                if (uuid)
                    free (uuid);
            }
            free (title);
        }
    }
    else
    if (streq (event, "WHISPER")) {
        char *title = zmsg_popstr (msg_duplicate);
        if (title == NULL) {
            igs_error ("no header in message received from %s(%s): rejecting",
                       name, peerUUID);
            zmsg_destroy (&msg_duplicate);
            zyre_event_destroy (&zyre_event);
            return 0;
        }

        if (streq (title, REMOTE_PEER_KNOWS_AGENT_MSG)) {
            // distant peer has received one of our agents definition
            // => all agents in this peer know us
            char *uuid = zmsg_popstr (msg_duplicate);
            if (uuid == NULL) {
                igs_error (
                  "no valid uuid in %s message received from %s(%s): rejecting",
                  title, name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            igsagent_t *agent = NULL;
            HASH_FIND_STR (context->agents, uuid, agent);
            if (agent) {
                igs_agent_event_wrapper_t *cb;
                DL_FOREACH (agent->agent_event_callbacks, cb)
                {
                    // iterate on all remote agents *for this peer* : all its agents know
                    // this agent
                    igs_remote_agent_t *r, *rtmp;
                    HASH_ITER (hh, core_context->remote_agents, r, rtmp)
                    {
                        if (streq (r->peer->peer_id, peerUUID))
                            cb->callback_ptr (agent, IGS_AGENT_KNOWS_US,
                                              r->uuid, r->definition->name,
                                              NULL, cb->my_data);
                    }
                }
            } // else agent has disappeared on our side (disabled or destroyed)
            free (uuid);
        }
        else
        if (streq (title, EXTERNAL_DEFINITION_MSG)) {
            // identify remote agent or create it if unknown.
            // NB: we suppose that remote agent creation is achieved when
            // the agent sends its definition for the first time.
            // Agents without definition are considered impossible.
            char *str_definition = zmsg_popstr (msg_duplicate);
            if (str_definition == NULL) {
                igs_error ("no valid definition in %s message received from "
                           "%s(%s): rejecting",
                           title, name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msg_duplicate);
            if (uuid == NULL) {
                igs_error (
                  "no valid uuid in %s message received from %s(%s): rejecting",
                  title, name, peerUUID);
                free (str_definition);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            char *remote_agent_name = zmsg_popstr (msg_duplicate);
            if (remote_agent_name == NULL) {
                igs_error ("no valid agent name in %s message received from "
                           "%s(%s): rejecting",
                           title, name, peerUUID);
                free (str_definition);
                free (uuid);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }

            // Load definition from string content
            igs_definition_t *new_definition =
              parser_load_definition (str_definition);
            if (new_definition && new_definition->name) {
                bool is_agent_new = false;
                igs_remote_agent_t *remote_agent = NULL;
                HASH_FIND_STR (context->remote_agents, uuid, remote_agent);
                if (remote_agent == NULL) {
                    remote_agent = (igs_remote_agent_t *) zmalloc (
                      sizeof (igs_remote_agent_t));
                    remote_agent->context = context;
                    remote_agent->uuid = strdup (uuid);
                    igs_zyre_peer_t *zyre_peer = NULL;
                    HASH_FIND_STR (context->zyre_peers, peerUUID, zyre_peer);
                    assert (zyre_peer);
                    remote_agent->peer = zyre_peer;
                    remote_agent->definition = new_definition;
                    HASH_ADD_STR (context->remote_agents, uuid, remote_agent);
                    igs_debug ("registering agent %s(%s)", uuid,
                               remote_agent_name);
                    is_agent_new = true;
                }
                else {
                    // else we already know this agent, its definition (possibly including
                    // name) has been updated
                    igs_debug (
                      "Definition already exists for remote agent %s : new "
                      "definition will overwrite the previous one...",
                      remote_agent->definition->name);
                    if (strneq (remote_agent->definition->name,
                                new_definition->name))
                        igs_debug (
                          "Remote agent is changing name from %s to %s",
                          remote_agent->definition->name, new_definition->name);

                    igs_definition_t *old_def = remote_agent->definition;
                    remote_agent->definition = new_definition;
                    definition_free_definition (&old_def);
                }
                assert (remote_agent);

                igs_debug ("store definition for remote agent %s(%s)",
                           remote_agent->definition->name, remote_agent->uuid);
                // Check the involvement of this new remote agent and its definition in
                // our agent mappings and update subscriptions. We check here because
                // remote agent definition is required to handle received data.
                igsagent_t *agent, *tmp;
                HASH_ITER (hh, context->agents, agent, tmp)
                    s_network_configure_mapping_to_remote_agent (agent, remote_agent);

                if (is_agent_new) {
                    s_agent_propagate_agent_event (IGS_AGENT_ENTERED, uuid,
                                                   remote_agent_name, NULL);

                    // Additonal notification flag means that the remote agent has been
                    // started during runtime: remote peer init has already been done and
                    // this remote agent knows our agents already => propagate to our
                    // agents immediately.
                    char *notification = zmsg_popstr (msg_duplicate);
                    if (notification) {
                        s_agent_propagate_agent_event (IGS_AGENT_KNOWS_US, uuid,
                                                       remote_agent_name, NULL);
                        free (notification);
                    }

                    // notify remote agent that our agents knows it
                    s_lock_zyre_peer ();
                    zmsg_t *msg_know = zmsg_new ();
                    zmsg_addstr (msg_know, REMOTE_PEER_KNOWS_AGENT_MSG);
                    zmsg_addstr (msg_know, uuid);
                    zyre_whisper (node, peerUUID, &msg_know);
                    s_unlock_zyre_peer ();

                    // Send ready message for splitter creation if a split exist with
                    // the new remote agent.
                    igsagent_t *elt_agent, *tmp_agent;
                    HASH_ITER (hh, context->agents, elt_agent, tmp_agent)
                    {
                        bool found_split_element = false;
                        char *input_split_element;
                        char *output_split_element;
                        igs_split_t *elt, *tmp_split;
                        HASH_ITER (hh, elt_agent->mapping->split_elements,
                                   elt, tmp_split)
                        {
                            if (elt
                                && streq (elt->to_agent,
                                          remote_agent->definition->name)) {
                                found_split_element = true;
                                input_split_element = elt->from_input;
                                output_split_element = elt->to_output;
                                break;
                            }
                        }
                        if (found_split_element) {
                            zmsg_t *ready_message = zmsg_new ();
                            zmsg_addstr (ready_message, WORKER_HELLO_MSG);
                            zmsg_addstr (ready_message, elt_agent->uuid);
                            zmsg_addstr (ready_message, input_split_element);
                            zmsg_addstr (ready_message,
                                         output_split_element);
                            zmsg_addstrf (ready_message, "%i",
                                          IGS_DEFAULT_WORKER_CREDIT);
                            igs_channel_whisper_zmsg (remote_agent->uuid,
                                                      &ready_message);
                        }
                    }
                }
                else
                    s_agent_propagate_agent_event (IGS_AGENT_UPDATED_DEFINITION,
                                                   uuid, remote_agent_name,
                                                   NULL);
            }
            else {
                if (new_definition && !new_definition->name)
                    igs_error (
                      "received definition from remote agent %s(%s) does not "
                      "contain a name : rejecting",
                      remote_agent_name, uuid);
                else
                    igs_error ("received definition from remote agent %s(%s) "
                               "is empty or "
                               "invalid : agent will not be registered",
                               remote_agent_name, uuid);
            }
            free (str_definition);
            free (uuid);
            free (remote_agent_name);
        }
        else
        if (streq (title, EXTERNAL_MAPPING_MSG)) {
            // identify remote agent
            char *str_mapping = zmsg_popstr (msg_duplicate);
            if (str_mapping == NULL) {
                igs_error ("no valid mapping in %s message received from "
                           "%s(%s): rejecting",
                           title, name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msg_duplicate);
            if (uuid == NULL) {
                igs_error (
                  "uuid is NULL in %s message received from %s(%s): rejecting",
                  title, name, peerUUID);
                free (str_mapping);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            igs_remote_agent_t *remote_agent = NULL;
            HASH_FIND_STR (context->remote_agents, uuid, remote_agent);
            if (remote_agent == NULL) {
                igs_error ("no known remote agent with uuid '%s': rejecting",
                           uuid);
                free (str_mapping);
                free (uuid);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }

            igs_mapping_t *new_mapping = NULL;
            if (strlen (str_mapping) > 0) {
                // load mapping from string content
                new_mapping = parser_load_mapping (str_mapping);
                if (new_mapping == NULL)
                    igs_error ("received mapping for agent %s(%s) could not be "
                               "parsed properly",
                               remote_agent->definition->name,
                               remote_agent->uuid);
            }
            else {
                igs_debug ("received mapping from agent %s(%s) is empty",
                           remote_agent->definition->name, remote_agent->uuid);
                if (remote_agent != NULL && remote_agent->mapping != NULL) {
                    mapping_free_mapping (&remote_agent->mapping);
                    remote_agent->mapping = NULL;
                    s_agent_propagate_agent_event (
                      IGS_AGENT_UPDATED_MAPPING, uuid,
                      remote_agent->definition->name, NULL);
                }
            }

            if (new_mapping != NULL && remote_agent != NULL) {
                // look if this agent already has a mapping
                if (remote_agent->mapping != NULL) {
                    igs_debug (
                      "mapping already exists for agent %s(%s) : new mapping "
                      "will overwrite the previous one...",
                      remote_agent->definition->name, remote_agent->uuid);
                    mapping_free_mapping (&remote_agent->mapping);
                }

                igs_debug ("store mapping for agent %s(%s)",
                           remote_agent->definition->name, remote_agent->uuid);
                remote_agent->mapping = new_mapping;
                s_agent_propagate_agent_event (IGS_AGENT_UPDATED_MAPPING, uuid,
                                               remote_agent->definition->name,
                                               NULL);
            }
            free (str_mapping);
            free (uuid);
        }
        else
        if (streq (title, LOAD_DEFINITION_MSG)) {
            // identify agent
            char *str_definition = zmsg_popstr (msg_duplicate);
            if (str_definition == NULL) {
                igs_error ("no valid definition in %s message received from "
                           "%s(%s): rejecting",
                           title, name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msg_duplicate);
            if (uuid == NULL) {
                igs_error (
                  "no valid uuid in %s message received from %s(%s): rejecting",
                  title, name, peerUUID);
                free (str_definition);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            igsagent_t *agent = NULL;
            HASH_FIND_STR (context->agents, uuid, agent);
            if (agent == NULL) {
                igs_error (
                  "no agent with uuid '%s' in %s message received from %s(%s): "
                  "rejecting",
                  uuid, title, name, peerUUID);
                free (str_definition);
                if (uuid)
                    free (uuid);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }

            // load definition
            if (igsagent_definition_load_str (agent, str_definition)
                == IGS_SUCCESS) {
                // recheck mapping towards our new definition
                igs_remote_agent_t *remote, *tmp;
                HASH_ITER (hh, context->remote_agents, remote, tmp)
                {
                    s_network_configure_mapping_to_remote_agent (agent,
                                                                  remote);
                }
            }
            free (str_definition);
            free (uuid);
        }
        else
        if (streq (title, LOAD_MAPPING_MSG)) {
            // identify agent
            char *str_mapping = zmsg_popstr (msg_duplicate);
            if (str_mapping == NULL) {
                igs_error ("no valid mapping in %s message received from "
                           "%s(%s): rejecting",
                           title, name, peerUUID);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            char *uuid = zmsg_popstr (msg_duplicate);
            if (uuid == NULL) {
                igs_error (
                  "no valid uuid in %s message received from %s(%s): rejecting",
                  title, name, peerUUID);
                free (str_mapping);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }
            igsagent_t *agent = NULL;
            HASH_FIND_STR (context->agents, uuid, agent);
            if (agent == NULL) {
                igs_error (
                  "no agent with uuid '%s' in %s message received from %s(%s): "
                  "rejecting",
                  uuid, title, name, peerUUID);
                free (str_mapping);
                if (uuid)
                    free (uuid);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                return 0;
            }

            // Load mapping from string content
            igs_mapping_t *new_mapping = parser_load_mapping (str_mapping);
            if (new_mapping) {
                if (agent->mapping != NULL)
                    mapping_free_mapping (&agent->mapping);
                agent->mapping = new_mapping;
                // check and activate mapping
                igs_remote_agent_t *remote, *tmp;
                HASH_ITER (hh, context->remote_agents, remote, tmp)
                {
                    s_network_configure_mapping_to_remote_agent (agent,
                                                                  remote);
                }
                agent->network_need_to_send_mapping_update = true;
            }
            free (str_mapping);
            free (uuid);
        }
        else {
            //
            // OTHER SUPPORTED MESSAGES
            //
            if (streq (title, GET_CURRENT_OUTPUTS_MSG)) {
                // identify agent
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                model_read_write_lock ();
                // check that this agent has not been destroyed when we were locked
                if (!agent || !(agent->uuid)) {
                    model_read_write_unlock ();
                    return 0;
                }
                zmsg_t *msg_to_send = zmsg_new ();
                zmsg_addstr (msg_to_send, CURRENT_OUTPUTS_MSG);
                zmsg_addstr (msg_to_send, agent->uuid);
                igs_iop_t *outputs = agent->definition->outputs_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL;
                     current = current->hh.next) {
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.i),
                                         sizeof (int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.d),
                                         sizeof (double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addstr (msg_to_send, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.b),
                                         sizeof (bool));
                            break;
                        case IGS_IMPULSION_T:
                            // FIXME: we had to disable outputs sending for data and impulsions
                            // but this is not consistent with inputs and parameters disabled
                            //                                    zmsg_addstr(msg_to_send,
                            //                                    found_iop->name);
                            //                                    zmsg_addstrf(msg_to_send,
                            //                                    "%d", found_iop->value_type);
                            //                                    zmsg_addmem(msg_to_send, NULL,
                            //                                    0);
                            break;
                        case IGS_DATA_T:
                            // disabled
                            //                                    zmsg_addstr(msg_to_send,
                            //                                    found_iop->name);
                            //                                    zmsg_addstrf(msg_to_send,
                            //                                    "%d", found_iop->value_type);
                            //                                    zmsg_addmem(msg_to_send,
                            //                                    (found_iop->value.data),
                            //                                    found_iop->value_size);
                            break;

                        default:
                            break;
                    }
                }
                model_read_write_unlock ();
                s_lock_zyre_peer ();
                igs_debug ("send output values to %s", peerUUID);
                zyre_whisper (node, peerUUID, &msg_to_send);
                s_unlock_zyre_peer ();
                free (uuid);
            }
            else
            if (streq (title, CURRENT_OUTPUTS_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_remote_agent_t *remote_agent = NULL;
                HASH_FIND_STR (context->remote_agents, uuid, remote_agent);
                if (remote_agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("privately received output values from %s (%s)",
                           remote_agent->definition->name, remote_agent->uuid);
                s_handle_publication_from_remote_agent (msg_duplicate,
                                                      remote_agent);
                zmsg_destroy (&msg_duplicate);
                free (uuid);
            }
            else
            if (streq (title, GET_CURRENT_INPUTS_MSG)) {
                // identify agent
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                model_read_write_lock ();
                // check that this agent has not been destroyed when we were locked
                if (!agent || !(agent->uuid)) {
                    model_read_write_unlock ();
                    return 0;
                }
                zmsg_t *msg_to_send = zmsg_new ();
                zmsg_addstr (msg_to_send, CURRENT_INPUTS_MSG);
                zmsg_addstr (msg_to_send, agent->uuid);
                igs_iop_t *outputs = agent->definition->inputs_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL;
                     current = current->hh.next) {
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.i),
                                         sizeof (int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.d),
                                         sizeof (double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addstr (msg_to_send, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.b),
                                         sizeof (bool));
                            break;
                        case IGS_IMPULSION_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, NULL, 0);
                            break;
                        case IGS_DATA_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, (current->value.data),
                                         current->value_size);
                            break;

                        default:
                            break;
                    }
                }
                model_read_write_unlock ();
                s_lock_zyre_peer ();
                igs_debug ("send input values to %s", peerUUID);
                zyre_whisper (node, peerUUID, &msg_to_send);
                s_unlock_zyre_peer ();
                free (uuid);
            }
            else
            if (streq (title, GET_CURRENT_PARAMETERS_MSG)) {
                // identify agent
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                model_read_write_lock ();
                // check that this agent has not been destroyed when we were locked
                if (!agent || !(agent->uuid)) {
                    model_read_write_unlock ();
                    return 0;
                }
                zmsg_t *msg_to_send = zmsg_new ();
                zmsg_addstr (msg_to_send, CURRENT_PARAMETERS_MSG);
                zmsg_addstr (msg_to_send, agent->uuid);
                igs_iop_t *outputs = agent->definition->params_table;
                igs_iop_t *current = NULL;
                for (current = outputs; current != NULL;
                     current = current->hh.next) {
                    switch (current->value_type) {
                        case IGS_INTEGER_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.i),
                                         sizeof (int));
                            break;
                        case IGS_DOUBLE_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.d),
                                         sizeof (double));
                            break;
                        case IGS_STRING_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addstr (msg_to_send, current->value.s);
                            break;
                        case IGS_BOOL_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, &(current->value.b),
                                         sizeof (bool));
                            break;
                        case IGS_IMPULSION_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, NULL, 0);
                            break;
                        case IGS_DATA_T:
                            zmsg_addstr (msg_to_send, current->name);
                            zmsg_addstrf (msg_to_send, "%d",
                                          current->value_type);
                            zmsg_addmem (msg_to_send, (current->value.data),
                                         current->value_size);
                            break;

                        default:
                            break;
                    }
                }
                model_read_write_unlock ();
                s_lock_zyre_peer ();
                igs_debug ("send parameters values to %s", peerUUID);
                zyre_whisper (node, peerUUID, &msg_to_send);
                s_unlock_zyre_peer ();
                free (uuid);
            }
            else
            if (streq (title, START_AGENT_MSG)) {
                char *agent_name = zmsg_popstr (msg_duplicate);
                if (agent_name == NULL) {
                    igs_error ("no agent name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received 'START_AGENT %s' command from %s (%s)",
                           agent_name, name, peerUUID);
                igsagent_t *a = zhash_first (core_context->created_agents);
                while (a) {
                    if (streq (a->definition->name, agent_name)) {
                        igs_info ("activating agent %s (%s)",
                                  a->definition->name, a->uuid);
                        igsagent_activate (a);
                    }
                    a = zhash_next (core_context->created_agents);
                }
            }
            else
            if (streq (title, STOP_AGENT_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received 'STOP_AGENT %s' command from %s (%s)",
                           uuid, name, peerUUID);
                igs_info ("deactivating agent %s (%s)", agent->definition->name,
                          agent->uuid);
                igsagent_deactivate (agent);
            }
            else
            if (streq (title, STOP_PEER_MSG)) {
                context->external_stop = true;
                igs_debug ("received STOP_PEER command from %s (%s)", name,
                           peerUUID);
                free (title);
                zmsg_destroy (&msg_duplicate);
                zyre_event_destroy (&zyre_event);
                // stop our zyre loop by returning -1 : this will start the cleaning
                // process
                return -1;
            }
            else
            if (streq (title, CLEAR_MAPPING_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received CLEAR_MAPPING command from %s (%s)", name,
                           peerUUID);
                igsagent_clear_mappings (agent);
                free (uuid);
            }
            else
            if (streq (title, FREEZE_MSG)) {
                igs_debug ("received FREEZE command from %s (%s)", name,
                           peerUUID);
                igs_freeze ();
            }
            else
            if (streq (title, UNFREEZE_MSG)) {
                igs_debug ("received UNFREEZE command from %s (%s)", name,
                           peerUUID);
                igs_unfreeze ();
            }
            else
            if (streq (title, MUTE_ALL_MSG)) {
                igs_debug ("received MUTE_ALL command from %s (%s)", name,
                           peerUUID);
                igsagent_t *agent, *tmp;
                HASH_ITER (hh, context->agents, agent, tmp)
                {
                    igsagent_mute (agent);
                }
            }
            else
            if (streq (title, UNMUTE_ALL_MSG)) {
                igs_debug ("received UNMUTE_ALL command from %s (%s)", name,
                           peerUUID);
                igsagent_t *agent, *tmp;
                HASH_ITER (hh, context->agents, agent, tmp)
                {
                    igsagent_unmute (agent);
                }
            }
            else
            if (streq (title, MUTE_AGENT_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received MUTE_AGENT command from %s (%s)", name,
                           peerUUID);
                igsagent_mute (agent);
            }
            else
            if (streq (title, UNMUTE_AGENT_MSG)) {
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received UNMUTE_AGENT command from %s (%s)", name,
                           peerUUID);
                igsagent_unmute (agent);
            }
            else
            if (streq (title, MUTE_OUTPUT_MSG)) {
                char *iop_name = zmsg_popstr (msg_duplicate);
                if (iop_name == NULL) {
                    igs_error ("no valid iop name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received MUTE command from %s (%s)", name,
                           peerUUID);
                igsagent_output_mute (agent, iop_name);
                free (iop_name);
                free (uuid);
            }
            else
            if (streq (title, UNMUTE_OUTPUT_MSG)) {
                char *iop_name = zmsg_popstr (msg_duplicate);
                if (iop_name == NULL) {
                    igs_error ("no valid iop name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received UNMUTE command from %s (%s)", name,
                           peerUUID);
                igsagent_output_unmute (agent, iop_name);
                free (iop_name);
                free (uuid);
            }
            else
            if (streq (title, SET_INPUT_MSG)) {
                char *iop_name = zmsg_popstr (msg_duplicate);
                if (iop_name == NULL) {
                    igs_error ("no valid iop name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msg_duplicate);
                if (value == NULL) {
                    igs_error ("no valid value in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received SET_INPUT command from %s (%s)", name,
                           peerUUID);
                if (iop_name != NULL && value != NULL)
                    igsagent_input_set_string (agent, iop_name, value);
                free (iop_name);
                free (value);
                free (uuid);
            }
            else
            if (streq (title, SET_OUTPUT_MSG)) {
                char *iop_name = zmsg_popstr (msg_duplicate);
                if (iop_name == NULL) {
                    igs_error ("no valid iop name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msg_duplicate);
                if (value == NULL) {
                    igs_error ("no valid value in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received SET_OUTPUT command from %s (%s)", name,
                           peerUUID);
                if (iop_name != NULL && value != NULL)
                    igsagent_output_set_string (agent, iop_name, value);
                free (iop_name);
                free (value);
                free (uuid);
            }
            else
            if (streq (title, SET_PARAMETER_MSG)) {
                char *iop_name = zmsg_popstr (msg_duplicate);
                if (iop_name == NULL) {
                    igs_error ("no valid iop name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *value = zmsg_popstr (msg_duplicate);
                if (value == NULL) {
                    igs_error ("no valid value in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (iop_name);
                    free (value);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received SET_PARAMETER command from %s (%s)", name,
                           peerUUID);
                if (iop_name != NULL && value != NULL)
                    igsagent_parameter_set_string (agent, iop_name, value);
                free (iop_name);
                free (value);
                free (uuid);
            }
            else
            if (streq (title, MAP_MSG)) {
                char *input = zmsg_popstr (msg_duplicate);
                if (input == NULL) {
                    igs_error ("no valid input in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *remote_agent = zmsg_popstr (msg_duplicate);
                if (remote_agent == NULL) {
                    igs_error (
                      "no valid agent name in %s message received from %s(%s): "
                      "rejecting",
                      title, name, peerUUID);
                    free (input);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msg_duplicate);
                if (output == NULL) {
                    igs_error ("no valid output in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received MAP command from %s (%s)", name, peerUUID);
                if (input != NULL && remote_agent != NULL && output != NULL)
                    igsagent_mapping_add (agent, input, remote_agent, output);
                free (input);
                free (remote_agent);
                free (output);
                free (uuid);
            }
            else
            if (streq (title, UNMAP_MSG)) {
                char *input = zmsg_popstr (msg_duplicate);
                if (input == NULL) {
                    igs_error ("no valid input in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *remote_agent = zmsg_popstr (msg_duplicate);
                if (remote_agent == NULL) {
                    igs_error (
                      "no valid agent name in %s message received from %s(%s): "
                      "rejecting",
                      title, name, peerUUID);
                    free (input);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msg_duplicate);
                if (output == NULL) {
                    igs_error ("no valid output in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received UNMAP command from %s (%s)", name,
                           peerUUID);
                if (input != NULL && remote_agent != NULL && output != NULL)
                    igsagent_mapping_remove_with_name (agent, input,
                                                        remote_agent, output);
                free (input);
                free (remote_agent);
                free (output);
                free (uuid);
            }
            else
            if (streq (title, ADD_SPLIT_ENTRY_MSG)) {
                char *input = zmsg_popstr (msg_duplicate);
                if (input == NULL) {
                    igs_error ("no valid input in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *remote_agent = zmsg_popstr (msg_duplicate);
                if (remote_agent == NULL) {
                    igs_error (
                      "no valid agent name in %s message received from %s(%s): "
                      "rejecting",
                      title, name, peerUUID);
                    free (input);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msg_duplicate);
                if (output == NULL) {
                    igs_error ("no valid output in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug ("received ADD_SPLIT_ENTRY command from %s (%s)",
                           name, peerUUID);
                if (input != NULL && remote_agent != NULL && output != NULL)
                    igsagent_split_add (agent, input, remote_agent, output);
                free (input);
                free (remote_agent);
                free (output);
                free (uuid);
            }
            else
            if (streq (title, REMOVE_SPLIT_ENTRY_MSG)) {
                char *input = zmsg_popstr (msg_duplicate);
                if (input == NULL) {
                    igs_error ("no valid input in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *remote_agent = zmsg_popstr (msg_duplicate);
                if (remote_agent == NULL) {
                    igs_error (
                      "no valid agent name in %s message received from %s(%s): "
                      "rejecting",
                      title, name, peerUUID);
                    free (input);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *output = zmsg_popstr (msg_duplicate);
                if (output == NULL) {
                    igs_error ("no valid output in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (input);
                    free (remote_agent);
                    free (output);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                igs_debug (
                  "received REMOVE_SPLIT_ENTRY_MSG command from %s (%s)", name,
                  peerUUID);
                if (input != NULL && remote_agent != NULL && output != NULL)
                    igsagent_split_remove_with_name (agent, input,
                                                      remote_agent, output);
                free (input);
                free (remote_agent);
                free (output);
                free (uuid);
            }
            // admin API
            else
            if (streq (title, ENABLE_LOG_STREAM_MSG)) {
                igs_debug ("received ENABLE_LOG_STREAM command from %s (%s)",
                           name, peerUUID);
                igs_log_set_stream (true);
            }
            else
            if (streq (title, DISABLE_LOG_STREAM_MSG)) {
                igs_debug ("received DISABLE_LOG_STREAM command from %s (%s)",
                           name, peerUUID);
                igs_log_set_stream (false);
            }
            else
            if (streq (title, ENABLE_LOG_FILE_MSG)) {
                igs_debug ("received ENABLE_LOG_FILE command from %s (%s)",
                           name, peerUUID);
                igs_log_set_file (true, core_context->log_file_path);
            }
            else
            if (streq (title, DISABLE_LOG_FILE_MSG)) {
                igs_debug ("received DISABLE_LOG_FILE command from %s (%s)",
                           name, peerUUID);
                igs_log_set_file (false, core_context->log_file_path);
            }
            else
            if (streq (title, SET_LOG_PATH_MSG)) {
                char *log_path = zmsg_popstr (msg_duplicate);
                igs_debug ("received SET_LOG_PATH command from %s (%s)", name,
                           peerUUID);
                igs_log_set_file(igs_log_file(), log_path);
            }
            else
            if (streq (title, SET_DEFINITION_PATH_MSG)) {
                char *definition_path = zmsg_popstr (msg_duplicate);
                if (definition_path == NULL) {
                    igs_error (
                      "no valid definition path in %s message received from "
                      "%s(%s): rejecting",
                      title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (definition_path);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (definition_path);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received SET_DEFINITION_PATH command from %s (%s)",
                           name, peerUUID);
                igsagent_definition_set_path (agent, definition_path);
                free (definition_path);
                free (uuid);
            }
            else
            if (streq (title, SET_MAPPING_PATH_MSG)) {
                char *mapping_path = zmsg_popstr (msg_duplicate);
                if (mapping_path == NULL) {
                    igs_error ("no valid mapping path in %s message received "
                               "from %s(%s): "
                               "rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (mapping_path);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    free (mapping_path);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received SET_MAPPING_PATH command from %s (%s)",
                           name, peerUUID);
                igsagent_mapping_set_path (agent, mapping_path);
                free (mapping_path);
                free (uuid);
            }
            else
            if (streq (title, SAVE_DEFINITION_TO_PATH_MSG)) {
                // identify agent
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug (
                  "received SAVE_DEFINITION_TO_PATH command from %s (%s)", name,
                  peerUUID);
                igsagent_definition_save (agent);
                free (uuid);
            }
            else
            if (streq (title, SAVE_MAPPING_TO_PATH_MSG)) {
                // identify agent
                char *uuid = zmsg_popstr (msg_duplicate);
                if (uuid == NULL) {
                    igs_error ("no valid uuid in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igsagent_t *agent = NULL;
                HASH_FIND_STR (context->agents, uuid, agent);
                if (agent == NULL) {
                    igs_error (
                      "no agent with uuid '%s' in %s message received from "
                      "%s(%s): rejecting",
                      uuid, title, name, peerUUID);
                    if (uuid)
                        free (uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }
                igs_debug ("received SAVE_MAPPING_TO_PATH command from %s (%s)",
                           name, peerUUID);
                igsagent_mapping_save (agent);
                free (uuid);
            }
            else
            if (streq (title, CALL_SERVICE_MSG)
                || streq (title, CALL_SERVICE_MSG_DEPRECATED)) {

                // identify agent
                char *caller_uuid = zmsg_popstr (msg_duplicate);
                char *callee_uuid = zmsg_popstr (msg_duplicate);

                const char *caller_name = name; // default caller name is the one of the peer

                if (streq (title, CALL_SERVICE_MSG_DEPRECATED))
                    igs_warn ("Remote agent %s(%s) uses an older version of Ingescape with deprecated messages. Please upgrade this agent.", caller_name, caller_uuid);

                igs_remote_agent_t *caller_agent = NULL;
                HASH_FIND_STR (context->remote_agents, caller_uuid, caller_agent);
                if (caller_agent) {
                    // replace caller name by the one of an actual agent
                    // NB: this will happen all the time, except when ingeprobe
                    //(which is not an agent) emulates a call.
                    caller_name = caller_agent->definition->name;
                }

                igsagent_t *callee_agent = NULL;
                HASH_FIND_STR (context->agents, callee_uuid, callee_agent);
                if (callee_agent == NULL) {
                    igs_error (
                      "no callee agent with uuid '%s' in %s message received "
                      "from %s(%s): rejecting",
                      callee_uuid, title, name, peerUUID);
                    if (callee_uuid)
                        free (callee_uuid);
                    if (caller_uuid)
                        free (caller_uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                char *service_name = zmsg_popstr (msg_duplicate);
                if (service_name == NULL) {
                    igs_error ("no service name in %s message received from "
                               "%s(%s): rejecting",
                               title, name, peerUUID);
                    free (caller_uuid);
                    free (callee_uuid);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                char *token = zmsg_popstr (msg_duplicate);
                if (token == NULL) {
                    igs_error (
                      "no token in %s message received from %s(%s): rejecting",
                      title, name, peerUUID);
                    free (caller_uuid);
                    free (callee_uuid);
                    free (service_name);
                    zmsg_destroy (&msg_duplicate);
                    zyre_event_destroy (&zyre_event);
                    return 0;
                }

                if (callee_agent->definition
                    && callee_agent->definition->services_table) {
                    igs_service_t *service = NULL;
                    HASH_FIND_STR (callee_agent->definition->services_table,
                                   service_name, service);
                    if (service != NULL) {
                        if (service->cb != NULL) {
                            s_lock_zyre_peer ();
                            zyre_shouts (context->node,
                                         callee_agent->igs_channel,
                                         "CALLED %s from %s (%s)", service_name,
                                         caller_name, caller_uuid);
                            s_unlock_zyre_peer ();
                            size_t nb_args = 0;
                            igs_service_arg_t *_arg = NULL;
                            LL_COUNT (service->arguments, _arg, nb_args);
                            if (service_add_values_to_arguments_from_message (service_name,
                                                                              service->arguments,
                                                                              msg_duplicate) == IGS_SUCCESS) {
                                if (core_context->enable_service_logging)
                                    service_log_received_service (callee_agent, caller_name, caller_uuid,
                                                                  service_name, service->arguments);
                                (service->cb) (callee_agent, caller_name,
                                               caller_uuid, service_name,
                                               service->arguments, nb_args,
                                               token, service->cb_data);
                                service_free_values_in_arguments (service->arguments);
                            }
                        }
                        else
                            igsagent_warn (callee_agent,
                                            "no defined callback to handle "
                                            "received service %s",
                                            service_name);
                    }
                    else
                        igsagent_warn (callee_agent,
                                        "agent %s(%s) has no service named %s",
                                        callee_agent->definition->name,
                                        callee_uuid, service_name);
                }
                free (caller_uuid);
                free (callee_uuid);
                free (service_name);
                if (token)
                    free (token);
            }
            // Performance
            else
            if (streq (title, PING_MSG)) {
                // we are pinged by another agent
                zframe_t *countF = zmsg_pop (msg_duplicate);
                size_t count = 0;
                memcpy (&count, zframe_data (countF), sizeof (size_t));
                zframe_t *payload = zmsg_pop (msg_duplicate);
                // igsagent_info(agent, "ping %zu from %s", count, peer);
                zmsg_t *back = zmsg_new ();
                zmsg_addstr (back, PONG_MSG);
                zmsg_addmem (back, &count, sizeof (size_t));
                zmsg_append (back, &payload);
                s_lock_zyre_peer ();
                zyre_whisper (node, peerUUID, &back);
                s_unlock_zyre_peer ();
            }
            else
            if (streq (title, PONG_MSG)) {
                // continue performance measurement
                zframe_t *countF = zmsg_pop (msg_duplicate);
                size_t count = 0;
                memcpy (&count, zframe_data (countF), sizeof (size_t));
                zframe_t *payload = zmsg_pop (msg_duplicate);
                // igsagent_info(agent, "pong %zu from %s", count, peer);
                if (count != context->performance_msg_counter)
                    igs_error ("pong message lost at index %zu from %s", count,
                               peerUUID);
                else
                if (count == context->performance_msg_count_target) {
                    // last message received
                    context->performance_stop = zclock_usecs ();
                    igs_info ("message size: %zu bytes",
                              context->performance_msg_size);
                    igs_info ("roundtrip count: %zu",
                              context->performance_msg_count_target);
                    igs_info ("average latency: %.3f µs",
                              ((double) context->performance_stop
                               - (double) context->performance_start)
                                / context->performance_msg_count_target);
                    size_t throughput =
                      (size_t) ((double) context->performance_msg_count_target
                                / ((double) context->performance_stop
                                   - (double) context->performance_start)
                                * 1000000);
                    double megabytes = (double) throughput
                                       * context->performance_msg_size
                                       / (1024 * 1024);
                    igs_info ("average roundtrip throughput: %zu msg/s",
                              (size_t) throughput);
                    igs_info ("average roundtrip throughput: %.3f MB/s",
                              megabytes);
                    context->performance_msg_count_target = 0;
                }
                else {
                    context->performance_msg_counter++;
                    zmsg_t *back = zmsg_new ();
                    zmsg_addstr (back, PING_MSG);
                    zmsg_addmem (back, &context->performance_msg_counter,
                                 sizeof (size_t));
                    zmsg_append (back, &payload);
                    s_lock_zyre_peer ();
                    zyre_whisper (node, peerUUID, &back);
                    s_unlock_zyre_peer ();
                }
            }
            else
            if (streq (title, WORKER_GOODBYE_MSG)
                    || streq (title, WORKER_HELLO_MSG)
                    || streq (title, WORKER_READY_MSG))
                split_message_from_worker (title, msg_duplicate, context);
            else
            if (streq (title, SPLITTER_WORK_MSG))
                split_message_from_splitter (msg_duplicate, context);
        }
        free (title);
    }
    else
    if (streq (event, "LEADER")) {
        const char *our_peer_uuid = zyre_uuid (context->node);
        bool is_leader = streq (our_peer_uuid, peerUUID);
        if (is_leader)
            igs_info ("\\o/ peer %s(%s) -that's us- is leader in '%s'", name,
                      peerUUID, group);
        else
            igs_info ("\\o/ peer %s(%s) is leader in '%s'", name, peerUUID,
                      group);

        zlist_t *election = zhash_lookup (context->elections, group);
        assert (election);
        // inform all our agents participating in the election
        char *attendeeUUID = zlist_first (election);
        while (attendeeUUID) {
            igsagent_t *agent = NULL;
            HASH_FIND_STR (context->agents, attendeeUUID, agent);
            assert (agent);
            if (is_leader)
                igs_info ("\\o/ agent %s(%s) is leader in '%s'",
                          agent->definition->name, agent->uuid, group);
            else
                igs_info ("\\o/ agent %s(%s) is NOT leader in '%s'",
                          agent->definition->name, agent->uuid, group);
            igs_agent_event_wrapper_t *cb;
            char *election_name = strdup (group);
            DL_FOREACH (agent->agent_event_callbacks, cb)
            {
                if (is_leader)
                    cb->callback_ptr (agent, IGS_AGENT_WON_ELECTION,
                                      agent->uuid, agent->definition->name,
                                      election_name, cb->my_data);
                else
                    cb->callback_ptr (agent, IGS_AGENT_LOST_ELECTION,
                                      agent->uuid, agent->definition->name,
                                      election_name, cb->my_data);
            }
            free (election_name);
            attendeeUUID = zlist_next (election);
        }
    }
    else
    if (streq (event, "LEAVE")) {
        igs_debug ("-%s has left %s", name, group);

        // check if we are last in an elections channel
        if (core_context->elections) {
            zlist_t *elections = zhash_keys (context->elections);
            char *election_name = zlist_first (elections);
            while (election_name) {
                if (streq (election_name, group)) {
                    zlist_t *peer_attendees =
                      zyre_peers_by_group (context->node, election_name);
                    size_t nb = zlist_size (peer_attendees);
                    if (nb == 0) {
                        // we are last for this election group
                        // inform all our agents participating in the election
                        zlist_t *agent_attendees = (zlist_t *) zhash_lookup (
                          context->elections, election_name);
                        assert (agent_attendees);
                        char *attendeeUUID = zlist_first (agent_attendees);
                        while (attendeeUUID) {
                            igsagent_t *agent = NULL;
                            HASH_FIND_STR (context->agents, attendeeUUID,
                                           agent);
                            assert (agent);
                            igs_info ("\\o/ agent %s(%s) is leader in '%s'",
                                      agent->definition->name, agent->uuid,
                                      election_name);
                            igs_agent_event_wrapper_t *cb;
                            DL_FOREACH (agent->agent_event_callbacks, cb)
                            {
                                cb->callback_ptr (agent, IGS_AGENT_WON_ELECTION,
                                                  agent->uuid,
                                                  agent->definition->name,
                                                  election_name, cb->my_data);
                            }
                            attendeeUUID = zlist_next (agent_attendees);
                        }
                    }
                    zlist_destroy (&peer_attendees);
                }
                election_name = zlist_next (elections);
            }
            zlist_destroy (&elections);
        }
    }
    else
    if (streq (event, "EXIT")) {
        igs_debug ("<-%s (%s) exited", name, peerUUID);

        // check if we are last in an elections channel
        // FIXME: every time a peer exits, we check empty election channels.
        // This can make IGS_AGENT_WON_ELECTION notifed multiple times for a given
        // agent.
        if (context->elections) {
            zlist_t *elections = zhash_keys (context->elections);
            char *election_name = zlist_first (elections);
            while (election_name) {
                zlist_t *peer_attendees =
                  zyre_peers_by_group (context->node, election_name);
                size_t nb = zlist_size (peer_attendees);
                if (nb == 0) {
                    // we are last for this election group
                    // inform all our agents participating in the election
                    zlist_t *agent_attendees = (zlist_t *) zhash_lookup (
                      context->elections, election_name);
                    assert (agent_attendees);
                    char *attendeeUUID = zlist_first (agent_attendees);
                    while (attendeeUUID) {
                        igsagent_t *agent = NULL;
                        HASH_FIND_STR (context->agents, attendeeUUID, agent);
                        assert (agent);
                        igs_info ("\\o/ agent %s(%s) is leader in '%s'",
                                  agent->definition->name, agent->uuid,
                                  election_name);
                        igs_agent_event_wrapper_t *cb;
                        DL_FOREACH (agent->agent_event_callbacks, cb)
                        {
                            cb->callback_ptr (agent, IGS_AGENT_WON_ELECTION,
                                              agent->uuid,
                                              agent->definition->name,
                                              election_name, cb->my_data);
                        }
                        attendeeUUID = zlist_next (agent_attendees);
                    }
                }
                zlist_destroy (&peer_attendees);
                election_name = zlist_next (elections);
            }
            zlist_destroy (&elections);
        }

        igs_zyre_peer_t *zyre_peer = NULL;
        HASH_FIND_STR (context->zyre_peers, peerUUID, zyre_peer);
        if (zyre_peer != NULL) {
            if (zyre_peer->reconnected > 0) {
                // do not clean: we are getting a timemout now whereas
                // the agent is reconnected
                zyre_peer->reconnected--;
            }
            else {
                igs_remote_agent_t *remote, *tmpremote;
                HASH_ITER (hh, context->remote_agents, remote, tmpremote)
                {
                    // destroy all remote agents attached to this peer
                    if (streq (remote->peer->peer_id, zyre_peer->peer_id)) {
                        HASH_DEL (context->remote_agents, remote);
                        split_remove_worker (context, remote->uuid, NULL);
                        s_agent_propagate_agent_event (
                          IGS_AGENT_EXITED, remote->uuid,
                          remote->definition->name, NULL);
                        s_clean_and_free_remote_agent (&remote);
                    }
                }
                HASH_DEL (context->zyre_peers, zyre_peer);
                s_agent_propagate_agent_event (IGS_PEER_EXITED, peerUUID, name,
                                               NULL);
                s_clean_and_free_zyre_peer (&zyre_peer, loop);
            }
        }
    }

    // handle callbacks
    // NB: as explained earlier, agent may be NULL
    // depending on the event type.
    igs_channels_wrapper_t *elt;
    DL_FOREACH (context->zyre_callbacks, elt){
        if (zyre_event){
            zmsg_t *dup = zmsg_dup (msg);
            elt->callback_ptr (event, peerUUID, name, address, group, headers,
                               dup, elt->my_data);
            zmsg_destroy (&dup);
        }else{
            igs_error ("previous callback certainly destroyed the zyre event : next "
                       "callbacks will not be executed");
            break;
        }
    }
    zmsg_destroy (&msg_duplicate);
    zyre_event_destroy (&zyre_event);
    return 0;
}

// Timer callback to (re)send our definition to agents present on the private
// channel
int trigger_definition_update (zloop_t *loop, int timer_id, void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (timer_id)
    igs_core_context_t *context = (igs_core_context_t *) arg;
    assert (context);

    igsagent_t *agent, *tmp;
    HASH_ITER (hh, context->agents, agent, tmp)
    {
        if (agent->network_need_to_send_definition_update) {
            model_read_write_lock ();
            // check that this agent has not been destroyed when we were locked
            if (!agent || !(agent->uuid)) {
                model_read_write_unlock ();
                return 0;
            }
            char *definition_str = NULL;
            igs_zyre_peer_t *p, *ptmp;
            HASH_ITER (hh, context->zyre_peers, p, ptmp)
            {
                if (p->has_joined_private_channel) {
                    if (p->protocol
                        && (streq (p->protocol, "v2")
                            || streq (p->protocol, "v3")))
                        definition_str = parser_export_definition_legacy (agent->definition);
                    else
                        definition_str =
                          parser_export_definition (agent->definition);
                    if (definition_str) {
                        s_send_definition_to_zyre_peer (
                          agent, p->peer_id, definition_str,
                          agent->network_activation_during_runtime);
                        free (definition_str);
                        definition_str = NULL;
                    }
                }
            }
            agent->network_activation_during_runtime =
              false; // reset flag if needed
            free (definition_str);
            // NB: this is not optimal to resend state details on definition change
            // but it is the cleanest way to send state on after-start agent
            // activation. State details are still sent individually when they change.
            s_send_state_to (agent, IGS_PRIVATE_CHANNEL, false);
            agent->network_need_to_send_definition_update = false;
            s_agent_propagate_agent_event (IGS_AGENT_UPDATED_DEFINITION,
                                           agent->uuid, agent->definition->name,
                                           NULL);
            // when definition changes, mapping may need to be updated as well
            agent->network_need_to_send_mapping_update = true;
            model_read_write_unlock ();
        }
    }
    return 0;
}

// Timer callback to update and (re)send our mapping to agents on the private
// channel
int s_trigger_mapping_update (zloop_t *loop, int timer_id, void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (timer_id)
    igs_core_context_t *context = (igs_core_context_t *) arg;
    assert (context);

    igsagent_t *agent, *tmp;
    HASH_ITER (hh, context->agents, agent, tmp)
    {
        if (agent->network_need_to_send_mapping_update) {
            model_read_write_lock ();
            // check that this agent has not been destroyed when we were locked
            if (!agent || !(agent->uuid)) {
                model_read_write_unlock ();
                return 0;
            }
            char *mapping_str = NULL;
            igs_zyre_peer_t *p, *ptmp;
            HASH_ITER (hh, context->zyre_peers, p, ptmp)
            {
                if (p->has_joined_private_channel) {
                    if (p->protocol && streq (p->protocol, "v2"))
                        mapping_str = parser_export_mapping_legacy (agent->mapping);
                    else
                        mapping_str = parser_export_mapping (agent->mapping);
                    if (mapping_str) {
                        s_send_mapping_to_zyre_peer (agent, p->peer_id,
                                                     mapping_str);
                        free (mapping_str);
                        mapping_str = NULL;
                    }
                }
            }
            igs_remote_agent_t *remote, *rtmp;
            HASH_ITER (hh, context->remote_agents, remote, rtmp)
            {
                s_network_configure_mapping_to_remote_agent (agent, remote);
            }
            agent->network_need_to_send_mapping_update = false;
            s_agent_propagate_agent_event (IGS_AGENT_UPDATED_MAPPING,
                                           agent->uuid, agent->definition->name,
                                           NULL);
            model_read_write_unlock ();
        }
    }
    return 0;
}

/*
 Network mutex is used to avoid collisions between starting and stopping
 an agent, and between the s_manage_zyre_incoming, s_init_loop and start/stop
 functions. IT SHOULD NO BE USED FOR ANYTHING ELSE
 */
igs_mutex_t s_network_mutex;
static bool s_network_Mutex_initialized = false;

void s_network_lock (void)
{
    if (!s_network_Mutex_initialized) {
        IGS_MUTEX_INIT (s_network_mutex);
        s_network_Mutex_initialized = true;
    }
    IGS_MUTEX_LOCK (s_network_mutex);
}

void s_network_unlock (void)
{
    assert (s_network_Mutex_initialized);
    IGS_MUTEX_UNLOCK (s_network_mutex);
}

// manage messages from the parent thread
int s_manage_parent (zloop_t *loop, zsock_t *pipe, void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (arg)

    zmsg_t *msg = zmsg_recv (pipe);
    assert (msg);
    char *command = zmsg_popstr (msg);
    if (command == NULL) {
        igs_error ("command is NULL for parent message: rejecting");
        zmsg_destroy (&msg);
        return 0;
    }
    if (streq (command, "STOP_LOOP") || streq (command, "$TERM")) {
        free (command);
        zmsg_destroy (&msg);
        return -1;
    }
    //else: nothing to do so far
    free (command);
    zmsg_destroy (&msg);
    return 0;
}

static void s_run_loop (zsock_t *mypipe, void *args)
{
    s_network_lock ();
    igs_core_context_t *context = (igs_core_context_t *) args;
    assert (context);
    assert (context->node);
    assert (context->publisher);
    assert (context->replay_channel);
#if defined(__UNIX__) && !defined(__UTYPE_IOS)
    assert (context->network_ipc_full_path);
    assert (context->network_ipc_endpoint);
    assert (context->ipc_publisher);
    assert (context->inproc_publisher);
#endif

    if (context->security_is_enabled && !context->security_auth) {
        context->security_auth = zactor_new (zauth, NULL);
        assert (context->security_auth);
        assert (zstr_send (context->security_auth, "VERBOSE") == 0);
        assert (zsock_wait (context->security_auth) >= 0);
        if (!core_context->security_public_certificates_directory)
            core_context->security_public_certificates_directory =
              s_strndup (IGS_DEFAULT_SECURITY_DIRECTORY, IGS_MAX_PATH_LENGTH);
        assert (zstr_sendx (context->security_auth, "CURVE",
                            context->security_public_certificates_directory,
                            NULL)
                == 0);
        assert (zsock_wait (context->security_auth) >= 0);
    }

    // iterate on agents to avoid sending definition and mapping update at startup
    // to all peers (they will receive def & map when joining INGESCAPE_PRIVATE)
    igsagent_t *agent, *tmp;
    HASH_ITER (hh, context->agents, agent, tmp)
    {
        agent->network_need_to_send_mapping_update = false;
        agent->network_need_to_send_definition_update = false;
        agent->network_activation_during_runtime = false;
    }

    // start zyre now that everything is set
    s_lock_zyre_peer ();
    int zyre_start_res = zyre_start (context->node);
    s_unlock_zyre_peer ();
    if (zyre_start_res != IGS_SUCCESS) {
        igs_error (
          "could not start zyre node : Ingescape will interrupt immediately.");
        return;
    }

    context->loop = zloop_new ();
    assert (context->loop);
    zloop_set_verbose (context->loop, false);
    zloop_reader (context->loop, mypipe, s_manage_parent, context);
    zloop_reader_set_tolerant (context->loop, mypipe);
    zloop_reader (context->loop, zyre_socket (context->node),
                  s_manage_zyre_incoming, context);
    zloop_reader_set_tolerant (context->loop, zyre_socket (context->node));
    zloop_timer (context->loop, 1000, 0, trigger_definition_update, context);
    zloop_timer (context->loop, 1000, 0, s_trigger_mapping_update, context);

    zsock_signal (mypipe, 0);
    s_network_unlock ();

    /////////////////////
    igs_debug ("loop starting");
    zloop_start (context->loop); // returns when one of the pollers returns -1
    /////////////////////

    s_network_lock ();
    igs_debug ("loop stopping..."); // clean dynamic part of the context

    igs_remote_agent_t *remote, *tmpremote;
    HASH_ITER (hh, context->remote_agents, remote, tmpremote)
    {
        HASH_DEL (context->remote_agents, remote);
        s_clean_and_free_remote_agent (&remote);
    }

    igs_zyre_peer_t *zyre_peer, *tmp_peer;
    HASH_ITER (hh, context->zyre_peers, zyre_peer, tmp_peer)
    {
        HASH_DEL (context->zyre_peers, zyre_peer);
        s_clean_and_free_zyre_peer (&zyre_peer, context->loop);
    }
    zloop_destroy (&context->loop);

    igs_timer_t *current_timer, *tmp_timer;
    HASH_ITER (hh, context->timers, current_timer, tmp_timer)
    {
        HASH_DEL (context->timers, current_timer);
        free (current_timer);
    }

    // zmq stack cleaning
    zyre_stop (context->node);
    zyre_destroy (&context->node);
    zsock_destroy (&context->publisher);
    zsock_destroy (&context->ipc_publisher);
#if defined(__UNIX__) && !defined(__UTYPE_IOS)
    zsys_file_delete (
      context->network_ipc_full_path); // destroy ipc_path in file system
    // NB: ipc_path is based on peer id which is unique. It will never be used
    // again.
    free (context->network_ipc_full_path);
    context->network_ipc_full_path = NULL;
#endif
#if !defined(__UTYPE_IOS)
    if (context->inproc_publisher != NULL)
        zsock_destroy (&context->inproc_publisher);
#endif
    if (context->logger)
        zsock_destroy (&context->logger);

    // handle external stop if needed
    if (context->external_stop) {
        igs_forced_stop_wrapper_t *cb = NULL;
        DL_FOREACH (context->external_stop_calbacks, cb)
            cb->callback_ptr (cb->my_data);
    }

    // clean remaining dynamic data
    if (context->replay_channel) {
        free (context->replay_channel);
        context->replay_channel = NULL;
    }
    if (context->network_ipc_full_path) {
        free (context->network_ipc_full_path);
        context->network_ipc_full_path = NULL;
    }
    if (context->network_ipc_endpoint) {
        free (context->network_ipc_endpoint);
        context->network_ipc_endpoint = NULL;
    }
    if (context->security_auth)
        zactor_destroy (&(context->security_auth));

    igs_debug ("loop stopped");
    zstr_send (mypipe, "LOOP_STOPPED");
    s_network_unlock ();
}

void s_init_loop (igs_core_context_t *context)
{
    core_init_agent (); // to be sure to have a default agent name

    igs_debug ("loop init");
    s_network_lock ();

    context->external_stop = false;
    bool can_continue = true;
    // prepare zyre
    s_lock_zyre_peer ();
    context->node = zyre_new (core_agent->definition->name);
    assert (context->node);
    if (context->security_is_enabled) {
        if (context->security_cert
            && context->security_public_certificates_directory) {
            // NB: zyre_set_zcert MUST be called before zyre_gossip_connect_curve
            zyre_set_zcert (context->node, context->security_cert);
            zyre_set_zap_domain (context->node, "INGESCAPE");
        }
        else {
            if (!context->security_cert)
                igs_error (
                  "security is enabled but certificate is missing : rejecting");
            if (!context->security_public_certificates_directory)
                igs_error (
                  "security is enabled but public certificates directory is "
                  "missing : rejecting");
            s_unlock_zyre_peer ();
            s_network_unlock ();
            return;
        }
    }
    // zyre_set_verbose(context->node);
    s_unlock_zyre_peer ();
    if (context->our_agent_endpoint) {
        s_lock_zyre_peer ();
        zlist_t *brokers = zhash_keys (context->brokers);
        char *broker = zlist_first (brokers);
        while (broker) {
            if (context->security_is_enabled) {
                char *cert_path = zhash_lookup (context->brokers, broker);
                if (strlen (cert_path) > 0) {
                    zcert_t *cert_to_gossip_server = zcert_load (cert_path);
                    if (cert_to_gossip_server)
                        zyre_gossip_connect_curve (
                          context->node,
                          zcert_public_txt (cert_to_gossip_server), "%s",
                          broker);
                    else
                        igs_warn ("could not open public certificate '%s' for "
                                  "server '%s' : "
                                  "server is ignored",
                                  cert_path, broker);
                }
                else
                    igs_warn (
                      "no public certificate path for server '%s' : server has "
                      "been ignored",
                      broker);
            }
            else
                zyre_gossip_connect (context->node, "%s", broker);
            broker = zlist_next (brokers);
        }
        zlist_destroy (&brokers);
        zyre_set_endpoint (context->node, "%s", context->our_agent_endpoint);
        if (context->our_broker_endpoint)
            zyre_gossip_bind (context->node, "%s",
                              context->our_broker_endpoint);
        if (context->advertised_endpoint)
            zyre_set_advertised_endpoint (context->node,
                                          context->advertised_endpoint);
        s_unlock_zyre_peer ();
    }
    else {
        s_lock_zyre_peer ();
        zyre_set_interface (context->node, context->network_device);
        zyre_set_port (context->node, context->network_zyre_port);
        s_unlock_zyre_peer ();
    }
    s_lock_zyre_peer ();
    zyre_set_interval (context->node, context->network_discovery_interval);
    zyre_set_expired_timeout (context->node, context->network_agent_timeout);
    zyre_join (context->node, IGS_PRIVATE_CHANNEL);

    if (context->elections) {
        // NB: no need to clean at loop stop because node destruction
        // will stop elections for this peer and trigger channel leave.
        zlist_t *keys = zhash_keys (context->elections);
        char *election_name = zlist_first (keys);
        while (election_name) {
            zyre_set_contest_in_group (context->node, election_name);
            zyre_join (context->node, election_name);
            election_name = zlist_next (keys);
        }
        zlist_destroy (&keys);
    }
    s_unlock_zyre_peer ();

    // create channel for replay
    assert (context->replay_channel == NULL);
    context->replay_channel = (char *) zmalloc (
      strlen (core_agent->definition->name) + strlen ("-IGS-REPLAY") + 1);
    snprintf (context->replay_channel, IGS_MAX_AGENT_NAME_LENGTH + 15,
              "%s-IGS-REPLAY", core_agent->definition->name);
    s_lock_zyre_peer ();
    zyre_join (context->node, context->replay_channel);
    s_unlock_zyre_peer ();

    // create channel for services feedback for each agent
    igsagent_t *agent, *tmp_agent;
    HASH_ITER (hh, context->agents, agent, tmp_agent){
        assert (agent->igs_channel);
        s_lock_zyre_peer ();
        zyre_join (context->node, agent->igs_channel);
        s_unlock_zyre_peer ();
    }

    // Add version and protocol to headers
    s_lock_zyre_peer ();
    zyre_set_header (
      context->node, "ingescape", "v%d.%d.%d", (int) igs_version () / 10000,
      (int) (igs_version () % 10000) / 100, (int) (igs_version () % 100));
    zyre_set_header (context->node, "protocol", "v%d", igs_protocol ());
    s_unlock_zyre_peer ();

    // Add stored headers to zyre
    igs_peer_header_t *el, *tmp;
    s_lock_zyre_peer ();
    HASH_ITER (hh, context->peer_headers, el, tmp)
    {
        zyre_set_header (context->node, el->key, "%s", el->value);
    }
    s_unlock_zyre_peer ();

    // start TCP publisher
    char endpoint[512];
    if (context->network_publishing_port == 0)
        snprintf (endpoint, 512, "tcp://%s:*", context->ip_address);
    else
        snprintf (endpoint, 512, "tcp://%s:%d", context->ip_address,
                  context->network_publishing_port);

    context->publisher = zsock_new_pub (endpoint);
    if (!context->publisher)
        igs_error("zsock_new_pub(%s): %s", endpoint, strerror(errno));
    assert (context->publisher);
    if (context->security_is_enabled) {
        zcert_apply (context->security_cert, context->publisher);
        zsock_set_curve_server (context->publisher, 1);
    }
    zsock_set_sndhwm (context->publisher, context->network_hwm_value);
    strncpy (endpoint, zsock_endpoint (context->publisher), 256);
    char *insert = endpoint + strlen (endpoint) - 1;
    while (*insert != ':' && insert > endpoint) {
        insert--;
    }
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "publisher", "%s", insert + 1);
    s_unlock_zyre_peer ();

    // start ipc publisher
#if defined(__UNIX__) && !defined(__UTYPE_IOS)
    if (context->network_ipc_folder_path == NULL)
        context->network_ipc_folder_path = strdup (IGS_DEFAULT_IPC_FOLDER_PATH);

    if (!zsys_file_exists (context->network_ipc_folder_path)) {
        zsys_dir_create ("%s", context->network_ipc_folder_path);
        if (!zsys_file_exists (context->network_ipc_folder_path)) {
            igs_error ("could not create ipc folder path '%s'",
                       context->network_ipc_folder_path);
            can_continue = false;
        }
    }
    s_lock_zyre_peer ();
    context->network_ipc_full_path =
      (char *) zmalloc (strlen (context->network_ipc_folder_path)
                        + strlen (zyre_uuid (context->node)) + 2);
    sprintf (context->network_ipc_full_path, "%s/%s",
             context->network_ipc_folder_path, zyre_uuid (context->node));
    context->network_ipc_endpoint =
      (char *) zmalloc (strlen (context->network_ipc_folder_path)
                        + strlen (zyre_uuid (context->node)) + 8);
    sprintf (context->network_ipc_endpoint, "ipc://%s/%s",
             context->network_ipc_folder_path, zyre_uuid (context->node));
    s_unlock_zyre_peer ();
    context->ipc_publisher = zsock_new_pub (context->network_ipc_endpoint);
    assert (context->ipc_publisher);
    if (context->security_is_enabled) {
        zcert_apply (context->security_cert, context->ipc_publisher);
        zsock_set_curve_server (context->ipc_publisher, 1);
    }
    zsock_set_sndhwm (context->ipc_publisher, context->network_hwm_value);
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "ipc", "%s", context->network_ipc_endpoint);
    s_unlock_zyre_peer ();

#elif defined(__WINDOWS__)
    context->network_ipc_endpoint = strdup ("tcp://127.0.0.1:*");
    zsock_t *ipc_publisher = context->ipc_publisher =
      zsock_new_pub (context->network_ipc_endpoint);
    assert (context->ipc_publisher);
    if (context->security_is_enabled) {
        zcert_apply (context->security_cert, context->ipc_publisher);
        zsock_set_curve_server (context->ipc_publisher, 1);
    }
    zsock_set_sndhwm (context->ipc_publisher, context->network_hwm_value);
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "loopback", "%s",
                     zsock_endpoint (ipc_publisher));
    s_unlock_zyre_peer ();
#endif

    // start inproc publisher
#if !defined(__UYTPE_IOS)
    s_lock_zyre_peer ();
    char *inproc_endpoint = (char *) zmalloc (
      sizeof (char) * (12 + strlen (zyre_uuid (context->node))));
    sprintf (inproc_endpoint, "inproc://%s", zyre_uuid (context->node));
    s_unlock_zyre_peer ();
    context->inproc_publisher = zsock_new_pub (inproc_endpoint);
    assert (context->inproc_publisher);
    if (context->security_is_enabled) {
        zcert_apply (context->security_cert, context->inproc_publisher);
        zsock_set_curve_server (context->inproc_publisher, 1);
    }
    zsock_set_sndhwm (context->inproc_publisher, context->network_hwm_value);
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "inproc", "%s", inproc_endpoint);
    s_unlock_zyre_peer ();
    free (inproc_endpoint);
#endif

    // logger stream
    if (context->network_log_stream_port == 0)
        sprintf (endpoint, "tcp://%s:*", context->ip_address);
    else
        sprintf (endpoint, "tcp://%s:%d", context->ip_address,
                 context->network_log_stream_port);

    context->logger = zsock_new_pub (endpoint);
    assert (context->logger);
    if (context->security_is_enabled) {
        zcert_apply (context->security_cert, context->logger);
        zsock_set_curve_server (context->logger, 1);
    }
    zsock_set_sndhwm (context->logger, context->network_hwm_value);
    strncpy (endpoint, zsock_endpoint (context->logger), 256);
    char *insert_point = endpoint + strlen (endpoint) - 1;
    while (*insert_point != ':' && insert_point > endpoint) {
        insert_point--;
    }
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "logger", "%s", insert_point + 1);
    s_unlock_zyre_peer ();

    // process PID and path
#if defined(__UNIX__)
    ssize_t ret;
    context->process_id = getpid ();
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "pid", "%i", context->process_id);
    s_unlock_zyre_peer ();
    if (context->command_line == NULL) {
        // command line was not set manually : we try to get exec path instead
#if defined(__UTYPE_IOS)
        char pathbuf[64] = "no_path";
        ret = 1;
#elif defined(__UTYPE_OSX)
        char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
        ret = proc_pidpath (context->process_id, pathbuf, sizeof (pathbuf));
#else
        char pathbuf[4 * 1024];
        memset (pathbuf, 0, 4 * 1024);
        ret = readlink ("/proc/self/exe", pathbuf, sizeof (pathbuf));
#endif
        if (ret <= 0)
            igs_debug ("PID %d: proc_pidpath () - %s", context->process_id,
                       strerror (errno));
        else
            igs_debug ("proc %d: %s", context->process_id, pathbuf);
        s_lock_zyre_peer ();
        zyre_set_header (context->node, "commandline", "%s", pathbuf);
        s_unlock_zyre_peer ();
    }
    else {
        s_lock_zyre_peer ();
        zyre_set_header (context->node, "commandline", "%s",
                         context->command_line);
        s_unlock_zyre_peer ();
    }
#endif
#if defined(__WINDOWS__)
    WSADATA wsa_data;
    WSAStartup (MAKEWORD (2, 2), &wsa_data);
    if ((context->command_line == NULL)
        || (strlen (context->command_line) == 0)) {
        // command line was not set manually : we try to get exec path instead

        // Use GetModuleFileName() to get exec path
        char exe_file_path[IGS_MAX_PATH_LENGTH];
#ifdef UNICODE
        WCHAR temp[IGS_MAX_PATH_LENGTH];
        GetModuleFileName (NULL, temp, IGS_MAX_PATH_LENGTH);
        // Conversion in char *
        wcstombs_s (NULL, exe_file_path, sizeof (exe_file_path), temp,
                    sizeof (temp));
#else
        GetModuleFileName (NULL, exe_file_path, IGS_MAX_PATH_LENGTH);
#endif
        s_lock_zyre_peer ();
        zyre_set_header (context->node, "commandline", "%s", exe_file_path);
        s_unlock_zyre_peer ();
    }
    else {
        s_lock_zyre_peer ();
        zyre_set_header (context->node, "commandline", "%s",
                         context->command_line);
        s_unlock_zyre_peer ();
    }
    DWORD pid = GetCurrentProcessId ();
    context->process_id = (int) pid;
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "pid", "%i", (int) pid);
    s_unlock_zyre_peer ();
#endif

    // hostname
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname (hostname, 1023);
#if defined(__WINDOWS__)
    WSACleanup ();
#endif
    s_lock_zyre_peer ();
    zyre_set_header (context->node, "hostname", "%s", hostname);
    s_unlock_zyre_peer ();
    s_network_unlock ();

    if (can_continue)
        context->network_actor = zactor_new (s_run_loop, context);
}

////////////////////////////////////////////////////////////////////////
// PRIVATE API
////////////////////////////////////////////////////////////////////////

igs_result_t network_publish_output (igsagent_t *agent, const igs_iop_t *iop)
{
    assert (agent);
    assert (agent->context);
    assert (agent->uuid);
    assert (iop);
    assert (iop->name);
    int result = IGS_SUCCESS;


    if (!agent->is_whole_agent_muted && !iop->is_muted
        && !agent->context->is_frozen) {
        model_read_write_lock ();
        split_add_work_to_queue (agent->context, agent->uuid, iop);
        // check that this agent has not been destroyed when we were locked
        if (!agent || !(agent->uuid)) {
            model_read_write_unlock ();
            return IGS_SUCCESS;
        }
        zmsg_t *msg = zmsg_new ();
        zmsg_addstrf (msg, "%s-%s", agent->uuid, iop->name);
        zmsg_addstrf (msg, "%d", iop->value_type);
        switch (iop->value_type) {
            case IGS_INTEGER_T:
                zmsg_addmem (msg, &(iop->value.i), sizeof (int));
                igsagent_debug (agent, "%s(%s) publishes %s -> %d",
                                 agent->definition->name, agent->uuid,
                                 iop->name, iop->value.i);
                break;
            case IGS_DOUBLE_T:
                zmsg_addmem (msg, &(iop->value.d), sizeof (double));
                igsagent_debug (agent, "%s(%s) publishes %s -> %f",
                                 agent->definition->name, agent->uuid,
                                 iop->name, iop->value.d);
                break;
            case IGS_BOOL_T:
                zmsg_addmem (msg, &(iop->value.b), sizeof (bool));
                igsagent_debug (agent, "%s(%s) publishes %s -> %d",
                                 agent->definition->name, agent->uuid,
                                 iop->name, iop->value.b);
                break;
            case IGS_STRING_T:
                zmsg_addstr (msg, iop->value.s);
                igsagent_debug (agent, "%s(%s) publishes %s -> '%s'",
                                 agent->definition->name, agent->uuid,
                                 iop->name, iop->value.s);
                break;
            case IGS_IMPULSION_T:
                zmsg_addmem (msg, NULL, 0);
                igsagent_debug (agent, "%s(%s) publishes impulsion %s",
                                 agent->definition->name, agent->uuid,
                                 iop->name);
                break;
            case IGS_DATA_T: {
                zframe_t *frame = zframe_new (iop->value.data, iop->value_size);
                zmsg_append (msg, &frame);
                igsagent_debug (agent, "%s(%s) publishes data %s (%zu bytes)",
                                 agent->definition->name, agent->uuid,
                                 iop->name, iop->value_size);
            } break;
            default:
                break;
        }

        // 1- publish to TCP
        zmsg_t *msg_quater = zmsg_dup (msg);
        if (agent->context->network_actor && agent->context->publisher) {
            zmsg_t *msg_bis = zmsg_dup (msg);
            zmsg_t *msg_ter = zmsg_dup (msg);
            if (zmsg_send (&msg, core_context->publisher) != 0) {
                igsagent_error (agent,
                                 "Could not publish output %s on the network\n",
                                 iop->name);
                zmsg_destroy (&msg);
                result = IGS_FAILURE;
            }
            // 2- publish to IPC
            if (core_context->ipc_publisher != NULL) {
                // publisher can be NULL on IOS or for read/write problems with assigned
                // IPC path in both cases, an error message has been issued at start
                if (zmsg_send (&msg_bis, core_context->ipc_publisher) != 0) {
                    igsagent_error (agent,
                                     "Could not publish output %s using IPC\n",
                                     iop->name);
                    zmsg_destroy (&msg_bis);
                    result = IGS_FAILURE;
                }
            }
            // 3- publish to inproc
            if (core_context->inproc_publisher != NULL) {
                if (zmsg_send (&msg_ter, core_context->inproc_publisher) != 0) {
                    igsagent_error (
                      agent, "Could not publish output %s using inproc\n",
                      iop->name);
                    zmsg_destroy (&msg_ter);
                    result = IGS_FAILURE;
                }
            }
        }
        else {
            zmsg_destroy (&msg);
            igsagent_warn (
              agent,
              "agent not started : could not publish output %s to the "
              "network (published to agents in same process only)",
              iop->name);
        }
        // 4- distribute publication message to other agents inside our context
        // without using network
        if (!agent->is_virtual) {
            free (zmsg_popstr (msg_quater)); // remove composite uuid/iop name
            zmsg_pushstr (msg_quater,
                          iop->name); // replace it by simple iop name
            // Generate a temporary fake remote agent, containing only
            // necessary information for s_handle_publication_from_remote_agent.
            igs_remote_agent_t *fake_remote =
              (igs_remote_agent_t *) zmalloc (sizeof (igs_remote_agent_t));
            fake_remote->context = core_context;
            fake_remote->definition =
              (igs_definition_t *) zmalloc (sizeof (igs_definition_t));
            fake_remote->definition->name = agent->definition->name;

            model_read_write_unlock (); // to avoid deadlock inside
                                        // s_handle_publication_from_remote_agent

            s_handle_publication_from_remote_agent (msg_quater, fake_remote);
            free (fake_remote->definition);
            free (fake_remote);
        }
        else
            model_read_write_unlock ();
        zmsg_destroy (&msg_quater);
    }
    else {
        if (agent->is_whole_agent_muted)
            igsagent_debug (
              agent, "Should publish output %s but the agent has been muted",
              iop->name);
        if (iop->is_muted)
            igsagent_debug (agent,
                             "Should publish output %s but it has been muted",
                             iop->name);
        if (agent->context->is_frozen == true)
            igsagent_debug (
              agent, "Should publish output %s but the agent has been frozen",
              iop->name);
    }
    return result;
}

int network_timer_callback (zloop_t *loop, int timer_id, void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (timer_id)
    igs_timer_t *timer = (igs_timer_t *) arg;
    timer->cb (timer->timer_id, timer->my_data);
    return 1;
}

////////////////////////////////////////////////////////////////////////
// PUBLIC API
////////////////////////////////////////////////////////////////////////
void igs_observe_channels (igs_channels_fn cb, void *my_data)
{
    core_init_context ();
    assert (cb);
    igs_channels_wrapper_t *new_cb =
      (igs_channels_wrapper_t *) zmalloc (sizeof (igs_channels_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    DL_APPEND (core_context->zyre_callbacks, new_cb);
}

igs_result_t igs_start_with_device (const char *network_device,
                                    unsigned int port)
{
    core_init_agent ();
    assert (network_device);
    assert (port > 0);

    if (core_context->network_actor != NULL) {
        // Agent is active : need to stop it first
        igs_stop ();
    }

    core_context->network_device =
      s_strndup (network_device, IGS_NETWORK_DEVICE_LENGTH);

#if defined(__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 && HIBYTE (wsa_data.wVersion) == 2);
#endif

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        //        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
        //                name, ziflist_address (iflist), ziflist_netmask (iflist),
        //                ziflist_broadcast (iflist));
        if (streq (name, network_device)) {
            core_context->ip_address =
              s_strndup (ziflist_address (iflist), IGS_IP_ADDRESS_LENGTH);
            igs_info ("Starting with ip address %s and port %d on device %s",
                      core_context->ip_address, port, network_device);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);

    if (core_context->ip_address == NULL) {
        igs_error (
          "IP address could not be determined on device %s : our agent "
          "will NOT start",
          network_device);
        igs_stop ();
        return IGS_FAILURE;
    }
    core_context->network_zyre_port = port;
    s_init_loop (core_context);
    assert (core_context->network_actor);
    return IGS_SUCCESS;
}

igs_result_t igs_start_with_ip (const char *ip_address, unsigned int port)
{
    assert (ip_address);
    assert (port > 0);
    core_init_agent ();

    if (core_context->network_actor != NULL) {
        // Agent is already active : need to stop it first
        igs_stop ();
    }

    core_context->ip_address = s_strndup (ip_address, IGS_IP_ADDRESS_LENGTH);

#if defined(__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 && HIBYTE (wsa_data.wVersion) == 2);
#endif

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        //        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
        //                name, ziflist_address (iflist), ziflist_netmask (iflist),
        //                ziflist_broadcast (iflist));
        if (streq (ziflist_address (iflist), ip_address)) {
            core_context->network_device =
              s_strndup (name, IGS_NETWORK_DEVICE_LENGTH);
            igs_info ("starting with ip address %s and port %d on device %s",
                      ip_address, port, core_context->network_device);
            break;
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);

    if (core_context->network_device == NULL) {
        igs_error (
          "device name could not be determined for IP address %s : our "
          "agent will NOT start",
          ip_address);
        igs_stop ();
        return IGS_FAILURE;
    }
    core_context->network_zyre_port = port;
    s_init_loop (core_context);
    assert (core_context->network_actor);
    return IGS_SUCCESS;
}

igs_result_t igs_broker_add (const char *broker_endpoint)
{
    core_init_context ();
    assert (broker_endpoint);
    assert (core_context->brokers);
    if (core_context->security_is_enabled) {
        igs_error (
          "security is enabled : you must use igs_broker_add_secure instead");
        return IGS_FAILURE;
    }
    if (zhash_insert (core_context->brokers, strdup (broker_endpoint),
                      strdup (""))
        != IGS_SUCCESS) {
        igs_error ("could not add '%s' (certainly because it is already added)",
                   broker_endpoint);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void igs_clear_brokers (void)
{
    core_init_context ();
    if (core_context->brokers)
        zhash_destroy (&(core_context->brokers));
    core_context->brokers = zhash_new ();
    zhash_autofree (core_context->brokers);
}

igs_result_t
igs_broker_add_secure (const char *broker_endpoint,
                        const char *path_to_public_certificate_for_broker)
{
    core_init_context ();
    assert (broker_endpoint);
    assert (path_to_public_certificate_for_broker);
    char public_key_path[IGS_MAX_PATH_LENGTH] = "";
    s_admin_make_file_path (path_to_public_certificate_for_broker,
                            public_key_path, IGS_MAX_PATH_LENGTH);
    if (!zsys_file_exists (public_key_path)) {
        igs_error ("'%s' does not exist for %s", public_key_path,
                   broker_endpoint);
        return IGS_FAILURE;
    }
    assert (core_context->brokers);
    char *end_pt = strdup (broker_endpoint);
    char *cert_dir = s_strndup (public_key_path, IGS_MAX_PATH_LENGTH);
    if (zhash_insert (core_context->brokers, end_pt, cert_dir) != IGS_SUCCESS) {
        igs_error ("could not add '%s' (certainly because it is already added)",
                   broker_endpoint);
        free (end_pt);
        free (cert_dir);
        return IGS_FAILURE;
    }
    return IGS_SUCCESS;
}

void igs_broker_enable_with_endpoint (const char *our_broker_endpoint)
{
    core_init_context ();
    assert (our_broker_endpoint);
    if (core_context->our_broker_endpoint)
        free (core_context->our_broker_endpoint);
    core_context->our_broker_endpoint = strdup (our_broker_endpoint);
}

void igs_broker_set_advertized_endpoint (const char *advertised_endpoint)
{
    core_init_context ();
    if (core_context->advertised_endpoint)
        free (core_context->advertised_endpoint);

    if (advertised_endpoint == NULL) {
        igs_info ("endpoint advertisement is disabled");
        core_context->advertised_endpoint = NULL;
    }
    else
        core_context->advertised_endpoint = strdup (advertised_endpoint);
}

igs_result_t igs_start_with_brokers (const char *agent_endpoint)
{
    core_init_agent ();
    assert (agent_endpoint);
    char address[512] = "";
    size_t port = 0;
    sscanf (agent_endpoint, "tcp://%[^:]:%zd", address, &port);
    assert (strlen (address) > 0);
    assert (port > 0);
    core_context->ip_address = strdup (address);
    if (core_context->network_actor != NULL) {
        // Agent is already active : need to stop it first
        igs_stop ();
    }
    if (core_context->our_agent_endpoint)
        free (core_context->our_agent_endpoint);
    core_context->our_agent_endpoint =
      s_strndup (agent_endpoint, IGS_IP_ADDRESS_LENGTH);

    assert (core_context->brokers);
    if (zhash_size (core_context->brokers) == 0
        && core_context->our_broker_endpoint == NULL) {
        igs_error (
          "no broker to connect to and we do not serve as broker : our "
          "agent will NOT start");
        return IGS_FAILURE;
    }

    s_init_loop (core_context);
    assert (core_context->network_actor);
    return IGS_SUCCESS;
}

void igs_stop ()
{
    core_init_context ();
    if (core_context->network_actor != NULL) {
        // interrupting and destroying ingescape thread and zyre layer
        // this will also clean all agent->subscribers
        if (!core_context->external_stop) {
            // NB: if agent has been forcibly stopped, actor is already stopping
            // and this command would deadlock.
            zstr_send (core_context->network_actor, "STOP_LOOP");
        }
        zactor_destroy (&core_context->network_actor);
#if defined(__WINDOWS__)
        // On Windows, if we don't call zsys_shutdown, the application will crash on
        // exit (WSASTARTUP assertion failure) NB: Monitoring also uses a zactor, we
        // cannot call zsys_shutdown() when it is running
        if (core_context->monitor == NULL) {
            igs_debug ("calling zsys_shutdown");
            zsys_shutdown ();
        }
#endif
        igs_info ("ingescape stopped properly");
    }
    else
        igs_debug ("ingescape already stopped");

    if (core_context->network_device) {
        free (core_context->network_device);
        core_context->network_device = NULL;
    }
    if (core_context->ip_address) {
        free (core_context->ip_address);
        core_context->ip_address = NULL;
    }
    if (core_context->our_agent_endpoint) {
        free (core_context->our_agent_endpoint);
        core_context->our_agent_endpoint = NULL;
    }
}

bool igs_is_started ()
{
    core_init_context ();
    s_network_lock ();
    if (core_context->loop != NULL) {
        s_network_unlock ();
        return true;
    } else {
        s_network_unlock ();
        return false;
    }
}

void igsagent_set_name (igsagent_t *agent, const char *name)
{
    assert (agent);
    assert (name && strlen (name) > 0);
    if (streq (agent->definition->name, name))
        return;

    char *n = s_strndup (name, IGS_MAX_AGENT_NAME_LENGTH);
    if (strlen (name) > IGS_MAX_AGENT_NAME_LENGTH)
        igsagent_warn (
          agent,
          "Agent name '%s' exceeds maximum size and will be truncated to '%s'",
          name, n);
    bool invalid_name = false;
    size_t length_ofn = strlen (n);
    size_t i = 0;
    for (i = 0; i < length_ofn; i++) {
        if (n[i] == ' ' || n[i] == '.') {
            n[i] = '_';
            invalid_name = true;
        }
    }
    if (invalid_name)
        igsagent_warn (
          agent,
          "Spaces and dots are not allowed in an agent name: '%s' has been changed to '%s'",
          name, n);
    char *previous = agent->definition->name;
    agent->definition->name = n;
    agent->network_need_to_send_definition_update = true;
    
    if (agent->igs_channel)
        free (agent->igs_channel);
    agent->igs_channel = (char *) zmalloc (strlen (agent->definition->name) + strlen ("-IGS") + 1);
    snprintf (agent->igs_channel,
              IGS_MAX_AGENT_NAME_LENGTH + strlen ("-IGS") + 1,
              "%s-IGS", agent->definition->name);
    
    if (agent->context && agent->context->node) {
        if (previous) {
            char *previous_igs_channel = (char *) zmalloc (strlen (previous) + strlen ("-IGS") + 1);
            snprintf (previous_igs_channel, IGS_MAX_AGENT_NAME_LENGTH + strlen("-IGS") + 1, "%s-IGS", previous);
            s_lock_zyre_peer ();
            zyre_leave (agent->context->node, previous_igs_channel);
            s_unlock_zyre_peer ();
            free (previous_igs_channel);
        }
        s_lock_zyre_peer ();
        zyre_join (agent->context->node, agent->igs_channel);
        s_unlock_zyre_peer ();
    }

    if (previous) {
        igsagent_debug (agent, "Agent (%s) name changed from %s to %s",
                         agent->uuid, previous, agent->definition->name);
        free (previous);
    }
}

char *igsagent_name (igsagent_t *agent)
{
    assert (agent);
    assert (agent->definition);
    assert (agent->definition->name);
    return strdup (agent->definition->name);
}

char *igsagent_uuid (igsagent_t *agent)
{
    assert (agent);
    assert (agent->uuid);
    return (agent->uuid)?strdup (agent->uuid):NULL;
}

void igs_freeze (void)
{
    core_init_context ();
    if (core_context->is_frozen == false) {
        if ((core_context != NULL) && (core_context->node != NULL)) {
            s_lock_zyre_peer ();
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                zmsg_t *msg = zmsg_new ();
                zmsg_addstr (msg, FROZEN_MSG);
                zmsg_addstr (msg, "1");
                zmsg_addstr (msg, agent->uuid);
                zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            s_unlock_zyre_peer ();
        }
        core_context->is_frozen = true;
        igs_freeze_wrapper_t *elt;
        DL_FOREACH (core_context->freeze_callbacks, elt)
        {
            elt->callback_ptr (core_context->is_frozen, elt->my_data);
        }
    }
}

bool igs_is_frozen (void)
{
    core_init_context ();
    return core_context->is_frozen;
}

void igs_unfreeze (void)
{
    core_init_context ();
    if (core_context->is_frozen == true) {
        if ((core_context->network_actor != NULL)
            && (core_context->node != NULL)) {
            s_lock_zyre_peer ();
            igsagent_t *agent, *tmp;
            HASH_ITER (hh, core_context->agents, agent, tmp)
            {
                zmsg_t *msg = zmsg_new ();
                zmsg_addstr (msg, FROZEN_MSG);
                zmsg_addstr (msg, "0");
                zmsg_addstr (msg, agent->uuid);
                zyre_shout (core_context->node, IGS_PRIVATE_CHANNEL, &msg);
            }
            s_unlock_zyre_peer ();
        }
        core_context->is_frozen = false;
        igs_freeze_wrapper_t *elt;
        DL_FOREACH (core_context->freeze_callbacks, elt)
        {
            elt->callback_ptr (core_context->is_frozen, elt->my_data);
        }
    }
}

void igs_observe_freeze (igs_freeze_fn cb, void *my_data)
{
    core_init_context ();
    if (cb != NULL) {
        igs_freeze_wrapper_t *new_cb =
          (igs_freeze_wrapper_t *) zmalloc (sizeof (igs_freeze_wrapper_t));
        new_cb->callback_ptr = cb;
        new_cb->my_data = my_data;
        DL_APPEND (core_context->freeze_callbacks, new_cb);
    }
    else
        igs_warn ("callback is null");
}

void igsagent_set_state (igsagent_t *agent, const char *state)
{
    assert (agent);
    assert (state);
    if (agent->state == NULL || !streq (state, agent->state)) {
        if (agent->state != NULL)
            free (agent->state);
        agent->state = s_strndup (state, IGS_MAX_AGENT_NAME_LENGTH);
        if (agent->context->node != NULL) {
            s_lock_zyre_peer ();
            zmsg_t *msg = zmsg_new ();
            zmsg_addstr (msg, STATE_MSG);
            zmsg_addstr (msg, agent->state);
            zmsg_addstr (msg, agent->uuid);
            zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            s_unlock_zyre_peer ();
        }
    }
}

char *igsagent_state (igsagent_t *agent)
{
    assert (agent);
    if (agent->state == NULL)
        return NULL;
    else
        return strdup (agent->state);
}

void igsagent_mute (igsagent_t *agent)
{
    assert (agent);
    if (!agent->is_whole_agent_muted) {
        agent->is_whole_agent_muted = true;
        if ((agent->context->network_actor != NULL)
            && (agent->context->node != NULL)) {
            s_lock_zyre_peer ();
            zmsg_t *msg = zmsg_new ();
            zmsg_addstr (msg, AGENT_MUTED_MSG);
            zmsg_addstrf (msg, "%i", agent->is_whole_agent_muted);
            zmsg_addstr (msg, agent->uuid);
            zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            s_unlock_zyre_peer ();
        }
        igs_mute_wrapper_t *elt;
        DL_FOREACH (agent->mute_callbacks, elt)
        {
            elt->callback_ptr (agent, agent->is_whole_agent_muted,
                               elt->my_data);
        }
    }
}

void igsagent_unmute (igsagent_t *agent)
{
    assert (agent);
    if (agent->is_whole_agent_muted) {
        agent->is_whole_agent_muted = false;
        if ((agent->context->network_actor != NULL)
            && (agent->context->node != NULL)) {
            s_lock_zyre_peer ();
            zmsg_t *msg = zmsg_new ();
            zmsg_addstr (msg, AGENT_MUTED_MSG);
            zmsg_addstrf (msg, "%i", agent->is_whole_agent_muted);
            zmsg_addstr (msg, agent->uuid);
            zyre_shout (agent->context->node, IGS_PRIVATE_CHANNEL, &msg);
            s_unlock_zyre_peer ();
        }
        igs_mute_wrapper_t *elt;
        DL_FOREACH (agent->mute_callbacks, elt)
        {
            elt->callback_ptr (agent, agent->is_whole_agent_muted,
                               elt->my_data);
        }
    }
}

bool igsagent_is_muted (igsagent_t *agent)
{
    assert (agent);
    return agent->is_whole_agent_muted;
}

void igsagent_observe_mute (igsagent_t *agent,
                             igsagent_mute_fn cb,
                             void *my_data)
{
    assert (agent);
    assert (cb);
    igs_mute_wrapper_t *new_cb =
      (igs_mute_wrapper_t *) zmalloc (sizeof (igs_mute_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    DL_APPEND (agent->mute_callbacks, new_cb);
}

char *igs_command_line (void)
{
    core_init_context ();
    if (core_context->command_line == NULL)
        return NULL;
    else
        return s_strndup (core_context->command_line, IGS_COMMAND_LINE_LENGTH);
}

void igs_set_command_line (const char *line)
{
    core_init_context ();
    assert (line);
    if (core_context->command_line != NULL)
        free (core_context->command_line);
    core_context->command_line = s_strndup (line, IGS_COMMAND_LINE_LENGTH);
    igs_debug ("Command line set to %s", core_context->command_line);
}

void igs_set_command_line_from_args (int argc, const char **argv)
{
    core_init_context ();
    if (argc < 1 || argv == NULL || argv[0] == NULL) {
        igs_error ("passed args must at least contain one element");
        return;
    }
    char cmd[IGS_COMMAND_LINE_LENGTH] = "";

#if defined(__UNIX__)
    size_t ret;
    pid_t pid;
    pid = getpid ();
#if defined(__UTYPE_IOS)
    char pathbuf[64] = "no_path";
    ret = 1;
#elif defined(__UTYPE_OSX)
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    ret = proc_pidpath (pid, pathbuf, sizeof (pathbuf));
#else
    char pathbuf[4 * 1024];
    memset (pathbuf, 0, 4 * 1024);
    ret = readlink ("/proc/self/exe", pathbuf, 4 * 1024);
#endif
    if (ret <= 0) {
        igs_error ("PID %d: proc_pidpath () - %s", pid, strerror (errno));
        return;
    }
    igs_debug ("proc %d: %s", pid, pathbuf);


    if (strlen (pathbuf) > IGS_COMMAND_LINE_LENGTH) {
        igs_error ("path is too long: %s", pathbuf);
        return;
    }
    strcat (cmd, pathbuf);

#elif defined(__WINDOWS__)
    // Use GetModuleFileName() to get exec path, argv[0] do not contain full path
    char exe_file_path[IGS_MAX_PATH_LENGTH];
#ifdef UNICODE
    WCHAR temp[IGS_MAX_PATH_LENGTH];
    GetModuleFileName (NULL, temp, IGS_MAX_PATH_LENGTH);
    // Conversion in char *
    wcstombs_s (NULL, exe_file_path, sizeof (exe_file_path), temp,
                sizeof (temp));
#else
    GetModuleFileName (NULL, exe_file_path, IGS_MAX_PATH_LENGTH);
#endif
    if (strchr (exe_file_path, ' ') || strchr (exe_file_path, '\"')) {
        if (strlen (cmd) + 1 > IGS_COMMAND_LINE_LENGTH) {
            igs_error (
              "Path to our executable exceeds the maximum buffer size of %d. "
              "Command line won't be set.",
              IGS_COMMAND_LINE_LENGTH);
            return;
        }
        strcat (cmd, "\"");
        char *prev = exe_file_path;
        char *it = strchr (exe_file_path, '\"');
        while (it) {
            if (strlen (cmd) + (it - prev + 2) > IGS_COMMAND_LINE_LENGTH) {
                igs_error (
                  "Path to our executable exceeds the maximum buffer size of "
                  "%d. Command line won't be set.",
                  IGS_COMMAND_LINE_LENGTH);
                return;
            }
            strncat (cmd, prev, (it - prev + 1));
            strcat (cmd, "\"");
            prev = it + 1;
            it = strchr (it + 1, '\"');
        }
        if (strlen (cmd) + strlen (prev) + 2 > IGS_COMMAND_LINE_LENGTH) {
            igs_error (
              "Path to our executable exceeds the maximum buffer size of %d. "
              "Command line won't be set.",
              IGS_COMMAND_LINE_LENGTH);
            return;
        }
        strcat (cmd, prev);
        strcat (cmd, "\"");
    }
    else
        strcpy (cmd, exe_file_path);
#endif

    int i = 1;
    for (; i < argc; i++) {
#if defined(__WINDOWS__)
        char *formated_arg =
          (char *) zmalloc (IGS_COMMAND_LINE_LENGTH * sizeof (char));
        if (strchr (argv[i], ' ') || strchr (argv[i], '\"')) {
            strcat (formated_arg, "\"");
            char *prev = argv[i];
            char *it = strchr (argv[i], '\"');
            while (it) {
                strncat (formated_arg, prev, (it - prev + 1));
                strcat (formated_arg, "\"");
                prev = it + 1;
                it = strchr (it + 1, '\"');
            }
            strcat (formated_arg, prev);
            strcat (formated_arg, "\"");
        }
        else
            strcpy (formated_arg, argv[i]);
#else
        char *formated_arg = strdup (argv[i]);
#endif
        if (strlen (cmd) + strlen (formated_arg) + 2
            > IGS_COMMAND_LINE_LENGTH) { // 2 is for space and EOL
            igs_error (
              "passed arguments exceed buffer size: concatenation will stop "
              "here with '%s'",
              cmd);
            break;
        }
        else
            strcat (cmd, " ");

        strcat (cmd, formated_arg);
        free (formated_arg);
    }
    if (core_context->command_line != NULL)
        free (core_context->command_line);
    core_context->command_line = s_strndup (cmd, IGS_COMMAND_LINE_LENGTH);
}

void igsagent_mapping_set_outputs_request (igsagent_t *agent, bool notify)
{
    agent->network_request_outputs_from_mapped_agents = notify;
}

bool igsagent_mapping_outputs_request (igsagent_t *agent)
{
    return agent->network_request_outputs_from_mapped_agents;
}

#define MAX_NUMBER_OF_NETDEVICES 16

char** igs_net_devices_list (int *nb)
{
#if defined(__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 && HIBYTE (wsa_data.wVersion) == 2);
#endif
    char** devices = (char **) zmalloc (MAX_NUMBER_OF_NETDEVICES * sizeof (char *));
    assert(devices);
    int current_device_nb = 0;

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        //        printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
        //                name, ziflist_address (iflist), ziflist_netmask (iflist),
        //                ziflist_broadcast (iflist));
        devices[current_device_nb] = (char *) zmalloc ((IGS_NETWORK_DEVICE_LENGTH + 1) * sizeof (char));
        strncpy (devices[current_device_nb], name, IGS_NETWORK_DEVICE_LENGTH);
        current_device_nb++;
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    *nb = current_device_nb;
#if defined(__WINDOWS__)
    WSACleanup ();
#endif
    return devices;
}

char** igs_net_addresses_list (int *nb)
{
#if defined(__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 && HIBYTE (wsa_data.wVersion) == 2);
#endif
    char** addresses = (char **) zmalloc (MAX_NUMBER_OF_NETDEVICES * sizeof (char *));
    assert(addresses);
    int current_device_nb = 0;

    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    while (name) {
        addresses[current_device_nb] = strdup (ziflist_address (iflist));
        current_device_nb++;
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);
    *nb = current_device_nb;
#if defined(__WINDOWS__)
    WSACleanup ();
#endif
    return addresses;
}

void igs_free_net_devices_list (char **devices, int nb)
{
    int i = 0;
    for (i = 0; i < nb; i++) {
        if (devices != NULL && devices[i] != NULL)
            free (devices[i]);
    }
    if (devices != NULL)
        free (devices);
}

void igs_free_net_addresses_list (char **addresses, int nb)
{
    igs_free_net_devices_list (addresses, nb);
}

void igs_observe_forced_stop (igs_forced_stop_fn cb, void *my_data)
{
    core_init_agent ();
    assert (cb);
    igs_forced_stop_wrapper_t *new_cb = (igs_forced_stop_wrapper_t *) zmalloc (
      sizeof (igs_forced_stop_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    DL_APPEND (core_context->external_stop_calbacks, new_cb);
}

igs_result_t igs_enable_security (const char *private_certificate_file,
                                  const char *public_certificates_directory)
{
    core_init_context ();
    core_context->security_is_enabled = true;

    if (private_certificate_file) {
        char private_key_path[IGS_MAX_PATH_LENGTH] = "";
        s_admin_make_file_path (private_certificate_file, private_key_path,
                                IGS_MAX_PATH_LENGTH);
        zcert_t *new_certificate = zcert_load (private_key_path);
        if (new_certificate == NULL) {
            igs_error ("could not load private certificate at '%s'",
                       private_key_path);
            return IGS_FAILURE;
        }
        if (core_context->security_cert)
            zcert_destroy (&(core_context->security_cert));
        core_context->security_cert = new_certificate;


        if (public_certificates_directory == NULL) {
            igs_error (
              "public certificates directory cannot be NULL when a private "
              "certificate is provided");
            return IGS_FAILURE;
        }
        char public_certificates_path[IGS_MAX_PATH_LENGTH] = "";
        s_admin_make_file_path (public_certificates_directory,
                                public_certificates_path, IGS_MAX_PATH_LENGTH);
        if (!zsys_file_exists (public_certificates_path)) {
            igs_error ("public certificates directory '%s' does not exist",
                       public_certificates_path);
            return IGS_FAILURE;
        }

        if (core_context->security_public_certificates_directory)
            free (core_context->security_public_certificates_directory);
        core_context->security_public_certificates_directory =
          s_strndup (public_certificates_path, IGS_MAX_PATH_LENGTH);
    }
    else {
        if (core_context->security_cert)
            zcert_destroy (&(core_context->security_cert));
        core_context->security_cert = zcert_new ();
        assert (core_context->security_cert);
        if (core_context->security_public_certificates_directory)
            free (core_context->security_public_certificates_directory);
        core_context->security_public_certificates_directory =
          strdup (IGS_DEFAULT_SECURITY_DIRECTORY);
    }

    if (!core_context->security_auth) {
        core_context->security_auth = zactor_new (zauth, NULL);
        assert (core_context->security_auth);
        assert (zstr_send (core_context->security_auth, "VERBOSE") == 0);
        assert (zsock_wait (core_context->security_auth) >= 0);
        if (!core_context->security_public_certificates_directory)
            core_context->security_public_certificates_directory =
              s_strndup (IGS_DEFAULT_SECURITY_DIRECTORY, IGS_MAX_PATH_LENGTH);
        assert (zstr_sendx (
                  core_context->security_auth, "CURVE",
                  core_context->security_public_certificates_directory, NULL)
                == 0);
        assert (zsock_wait (core_context->security_auth) >= 0);
    }
    else {
        // auth already exists : we just need to update public certs path
        assert (zstr_sendx (
                  core_context->security_auth, "CURVE",
                  core_context->security_public_certificates_directory, NULL)
                == 0);
        assert (zsock_wait (core_context->security_auth) >= 0);
    }
    return IGS_SUCCESS;
}

void igs_disable_security (void)
{
    core_context->security_is_enabled = false;
    if (core_context->security_cert)
        zcert_destroy (&(core_context->security_cert));
    if (core_context->security_public_certificates_directory) {
        free (core_context->security_public_certificates_directory);
        core_context->security_public_certificates_directory = NULL;
    }
    if (core_context->security_auth)
        zactor_destroy (&core_context->security_auth);
}

zactor_t *igs_zmq_authenticator (void)
{
    core_init_context ();
    if (!core_context->security_auth) {
        core_context->security_auth = zactor_new (zauth, NULL);
        assert (core_context->security_auth);
        assert (zstr_send (core_context->security_auth, "VERBOSE") == 0);
        assert (zsock_wait (core_context->security_auth) >= 0);
        if (!core_context->security_public_certificates_directory)
            core_context->security_public_certificates_directory =
              s_strndup (IGS_DEFAULT_SECURITY_DIRECTORY, IGS_MAX_PATH_LENGTH);
        assert (zstr_sendx (
                  core_context->security_auth, "CURVE",
                  core_context->security_public_certificates_directory, NULL)
                == 0);
        assert (zsock_wait (core_context->security_auth) >= 0);
    }
    return core_context->security_auth;
}

igs_result_t igsagent_election_join (igsagent_t *agent,
                                     const char *election_name)
{
    assert (agent);
    assert (election_name);
    if (streq (election_name, IGS_PRIVATE_CHANNEL)) {
        igsagent_error (
          agent, "this name is reserved and not allowed for an election name");
        return IGS_FAILURE;
    }
    char el_name[256] = "\\o/ ";
    if (strlen (election_name) + strlen (el_name) > 255) {
        igsagent_error (agent, "election name is limited to %lu characters",
                         255 - strlen (el_name));
        return IGS_FAILURE;
    }
    strncat (el_name, election_name, 255);
    core_init_context ();

    s_network_lock ();
    if (!core_context->elections)
        core_context->elections = zhash_new ();
    // elections hash
    zlist_t *election =
      (zlist_t *) zhash_lookup (core_context->elections, el_name);
    if (!election) {
        election = zlist_new ();
        zlist_autofree (election);
        zhash_insert (core_context->elections, el_name, election);
        if (core_context->node) {
            s_lock_zyre_peer ();
            zyre_set_contest_in_group (core_context->node, el_name);
            zyre_join (core_context->node, el_name);
            s_unlock_zyre_peer ();
        }
    }
    else
    if (zlist_size (election) > 0) {
        char *attendee = zlist_first (election);
        while (attendee) {
            if (streq (attendee, agent->uuid)) {
                igsagent_error (
                  agent, "agent %s(%s) already participates in election '%s'",
                  agent->definition->name, agent->uuid, el_name);
                s_network_unlock ();
                return IGS_FAILURE;
            }
            attendee = zlist_next (election);
        }
    }
    zlist_append (election, agent->uuid);

    // elections by agent
    if (!agent->elections) {
        agent->elections = zlist_new ();
        zlist_autofree (agent->elections);
    }
    zlist_append (agent->elections, el_name);

    s_network_unlock ();
    return IGS_SUCCESS;
}

igs_result_t igsagent_election_leave (igsagent_t *agent,
                                      const char *election_name)
{
    assert (agent);
    assert (election_name);
    if (!agent->elections) {
        igsagent_error (agent, "%s(%s) does not participate in any election",
                         agent->definition->name, agent->uuid);
        return IGS_FAILURE;
    }
    if (!core_context->elections) {
        igsagent_error (agent, "no election started at the moment");
        return IGS_FAILURE;
    }
    char el_name[256] = "\\o/ ";
    if (strlen (election_name) + strlen (el_name) > 255) {
        igsagent_error (agent, "election name is limited to %lu characters",
                         255 - strlen (el_name));
        return IGS_FAILURE;
    }
    strncat (el_name, election_name, 255);
    core_init_context ();

    s_network_lock ();
    // elections hash
    zlist_t *election = zhash_lookup (core_context->elections, el_name);
    if (!election) {
        igsagent_error (agent, "election '%s' has no attendee", el_name);
        s_network_unlock ();
        return IGS_FAILURE;
    }
    char *attendee = zlist_first (election);
    while (attendee) {
        if (streq (attendee, agent->uuid)) {
            zlist_remove (election, attendee);
            break;
        }
        attendee = zlist_next (election);
    }
    if (zlist_size (election) == 0) {
        zhash_delete (core_context->elections, el_name);
        zlist_destroy (&election);
        if (core_context->node) {
            s_lock_zyre_peer ();
            zyre_leave (core_context->node, el_name);
            s_unlock_zyre_peer ();
        }
    }
    if (zhash_size (core_context->elections) == 0)
        zhash_destroy (&core_context->elections);

    // elections by agent
    char *agent_election = zlist_first (agent->elections);
    while (agent_election) {
        if (streq (agent_election, el_name)) {
            zlist_remove (agent->elections, agent_election);
            break;
        }
        agent_election = zlist_next (agent->elections);
    }
    if (zlist_size (agent->elections) == 0)
        zlist_destroy (&agent->elections);

    s_network_unlock ();
    return IGS_SUCCESS;
}

void igs_net_set_discovery_interval (unsigned int interval)
{
    core_init_context ();
    if (core_context->network_actor != NULL && core_context->node != NULL) {
        s_lock_zyre_peer ();
        zyre_set_interval (core_context->node, interval);
        s_unlock_zyre_peer ();
    }
    core_context->network_discovery_interval = interval;
}

void igs_net_set_timeout (unsigned int duration)
{
    core_init_context ();
    if (core_context->network_actor != NULL && core_context->node != NULL) {
        s_lock_zyre_peer ();
        zyre_set_expired_timeout (core_context->node, duration);
        s_unlock_zyre_peer ();
    }
    core_context->network_agent_timeout = duration;
}

void igs_net_set_publishing_port (unsigned int port)
{
    core_init_context ();
    if (core_context->network_actor != NULL
        && core_context->publisher != NULL) {
        igs_error ("agent is already started : stop it first to change its "
                   "publishing port");
        return;
    }
    core_context->network_publishing_port = port;
}

void igs_net_set_log_stream_port (unsigned int port)
{
    core_init_context ();
    if (core_context->network_actor != NULL && core_context->logger != NULL) {
        igs_error (
          "agent is already started : stop it first to change its logger port");
        return;
    }
    core_context->network_log_stream_port = port;
}

#if defined(__UNIX__)
void igs_set_ipc_dir (const char *path)
{
    core_init_context ();
    assert (path);
    if (core_context->network_ipc_folder_path == NULL
        || !streq (path, core_context->network_ipc_folder_path)) {
        if (*path == '/') {
            if (core_context->network_ipc_folder_path != NULL)
                free (core_context->network_ipc_folder_path);
            if (!zsys_file_exists (path)) {
                igs_info ("folder %s was created automatically", path);
                zsys_dir_create ("%s", path);
            }
            core_context->network_ipc_folder_path = strdup (path);
        }
        else
            igs_error ("IPC folder path must be absolute");
    }
}

const char *igs_ipc_dir ()
{
    core_init_context ();
    return strdup (core_context->network_ipc_folder_path);
}
#endif

void igs_set_allow_inproc (bool allow)
{
    core_init_context ();
    core_context->network_allow_inproc = allow;
}

bool igs_get_allow_inproc (void)
{
    core_init_context ();
    return core_context->network_allow_inproc;
}

void igs_set_ipc (bool allow)
{
    core_init_context ();
    core_context->network_allow_ipc = allow;
}

bool igs_has_ipc ()
{
    core_init_context ();
    return core_context->network_allow_ipc;
}

void igs_net_set_high_water_marks (int hwm_value)
{
    core_init_context ();
    if (hwm_value < 0) {
        igs_error ("HWM value must be zero or higher");
        return;
    }
    if (core_context->network_actor != NULL
        && core_context->publisher != NULL) {
        zsock_set_sndhwm (core_context->publisher, hwm_value);
        if (core_context->ipc_publisher)
            zsock_set_sndhwm (core_context->ipc_publisher, hwm_value);
        if (core_context->inproc_publisher)
            zsock_set_sndhwm (core_context->inproc_publisher, hwm_value);
        zsock_set_sndhwm (core_context->logger, hwm_value);
        igs_zyre_peer_t *tmp = NULL, *peer = NULL;
        HASH_ITER (hh, core_context->zyre_peers, peer, tmp)
        {
            zsock_set_rcvhwm (peer->subscriber, hwm_value);
        }
    }
    core_context->network_hwm_value = hwm_value;
}

void igs_net_raise_sockets_limit ()
{
    core_init_context ();
#if defined(__UNIX__)
    if (core_context->network_shall_raise_file_descriptors_limit) {
        struct rlimit limit;
        if (getrlimit (RLIMIT_NOFILE, &limit) != 0)
            igs_error ("getrlimit() failed with errno=%d", errno);
        else {
            rlim_t prev_cur = limit.rlim_cur;
#if defined(__UTYPE_OSX) || defined(__UTYPE_IOS)
            limit.rlim_cur = MIN (
              OPEN_MAX,
              limit
                .rlim_max); // OPEN_MAX is the actual per process limit in macOS
#else
            limit.rlim_cur = limit.rlim_max;
#endif
            if (setrlimit (RLIMIT_NOFILE, &limit) != 0)
                igs_error ("setrlimit() failed with errno=%d", errno);
            else {
                if (getrlimit (RLIMIT_NOFILE, &limit) != 0)
                    igs_error ("getrlimit() failed with errno=%d", errno);
                else {
                    // adjust allowed number of sockets per process in ZeroMQ
                    zsys_set_max_sockets (
                      0); // 0 = use maximum value allowed by the OS
                    igs_debug (
                      "raised file descriptors limit from %llu to %llu",
                      prev_cur, limit.rlim_cur);
                    core_context->network_shall_raise_file_descriptors_limit =
                      false;
                }
            }
        }
    }
#else
    igs_info ("this function has no effect on non-UNIX systems");
#endif
}

zsock_t *igs_pipe_to_ingescape (void)
{
    core_init_context ();
    if (core_context->network_actor != NULL)
        return zactor_sock (core_context->network_actor);
    else {
        igs_warn ("ingescape is not started yet");
        return NULL;
    }
}

int igs_timer_start (size_t delay, size_t times, igs_timer_fn cb, void *my_data)
{
    core_init_context ();
    if (core_context->loop == NULL) {
        igs_error ("Ingescape must be started to create a timer");
        return -1;
    }
    if (cb == NULL) {
        igs_error ("callback function cannot be NULL");
        return -1;
    }
    s_network_lock ();
    igs_timer_t *timer = (igs_timer_t *) zmalloc (sizeof (igs_timer_t));
    timer->cb = cb;
    timer->my_data = my_data;
    timer->timer_id = zloop_timer (core_context->loop, delay, times,
                                   network_timer_callback, timer);
    HASH_ADD_INT (core_context->timers, timer_id, timer);
    s_network_unlock ();
    return timer->timer_id;
}

void igs_timer_stop (int timer_id)
{
    core_init_context ();
    if (core_context->loop == NULL) {
        igs_error ("Ingescape must be started to destroy a timer");
        return;
    }
    s_network_lock ();
    igs_timer_t *timer = NULL;
    HASH_FIND_INT (core_context->timers, &timer_id, timer);
    if (timer != NULL) {
        zloop_timer_end (core_context->loop, timer_id);
        HASH_DEL (core_context->timers, timer);
        free (timer);
    }
    else
        igs_error ("could not find timer with id %d", timer_id);
    s_network_unlock ();
}
