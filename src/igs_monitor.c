/*  =========================================================================
    monitor - monitor network events and failures

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

// Timer callback to check network
int igs_monitor_trigger_network_check (zloop_t *loop, int timer_id, void *arg)
{
    core_init_agent ();
    IGS_UNUSED (loop)
    IGS_UNUSED (timer_id)
    IGS_UNUSED (arg)

#if defined(__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 && HIBYTE (wsa_data.wVersion) == 2);
#endif
    bool need_to_call_callbacks = false;
    bool cb_stop_start = false;
    bool cb_stop = false;
    unsigned int cb_port = 0;
    char *cb_ip_address = NULL;
    char *cb_device = NULL;
    igs_monitor_event_t cb_event = IGS_NETWORK_OK;
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool found_network_device = false;
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    const char *name = ziflist_first (iflist);
    // go through the available devices to check network consistency depending on previous state
    while (name) {
        if (core_context->monitor->status == IGS_NETWORK_OK) {
            // peer was previously OK : check if it still is
            if (core_context->node && core_context->network_device && streq (name, core_context->network_device)) {
                // peer is started and device is there
                found_network_device = true;
                // check if IP address has changed : a changed address might cause trouble in peer communication
                if (strneq (core_context->ip_address, ziflist_address (iflist))) {
                    igs_warn ("IP address has changed from %s to %s", core_context->ip_address, ziflist_address (iflist));
                    need_to_call_callbacks = true;
                    cb_event = IGS_NETWORK_ADDRESS_CHANGED;
                    cb_device = strdup(core_context->network_device);
                    cb_ip_address = strdup(ziflist_address (iflist));

                    // check if we need to restart after IP address change
                    if (core_context->monitor_shall_start_stop_agent) {
                        cb_stop_start = true;
                        cb_port = core_context->network_zyre_port;
                        igs_warn ("restarting peer after IP address has changed on %s", cb_device);
                    }
                }
                break;
            }
            // peer is stopped : check if this is our expected network device
            else if ((core_context->node == NULL) && core_context->monitor->network_device
                     && (streq (name, core_context->monitor->network_device))) {
                // peer is not started BUT network device is available
                found_network_device = true;
                // nothing special to do here
                break;
            }
        }
        else if (core_context->monitor->status == IGS_NETWORK_DEVICE_NOT_AVAILABLE) {
            // network device was missing : check if situation has changed
            if ((core_context->node == NULL) && core_context->monitor->network_device
                && (streq (name, core_context->monitor->network_device))) {
                found_network_device = true;
                igs_warn ("network device %s has come back", core_context->monitor->network_device);
                cb_event = core_context->monitor->status = IGS_NETWORK_OK;
                need_to_call_callbacks = true;
                cb_device = strdup(core_context->monitor->network_device);
                cb_ip_address = NULL;

                // check if we need to restart peer
                if (core_context->monitor_shall_start_stop_agent) {
                    cb_stop_start = true;
                    cb_port = core_context->monitor->port;
                    igs_warn ("restarting agent after network device %s is back", core_context->monitor->network_device);
                }

                // clean-up
                if (core_context->monitor->network_device)
                    free (core_context->monitor->network_device);
                core_context->monitor->network_device = NULL;
                break;
            } else if (core_context->node && (streq (name, core_context->network_device))) {
                // peer is now started : it was restarted manually
                found_network_device = true;

                // update our status
                cb_event = core_context->monitor->status = IGS_NETWORK_OK;
                need_to_call_callbacks = true;
                cb_device = strdup(core_context->monitor->network_device);
                cb_ip_address = strdup(core_context->ip_address);
                // NB: we don't need to (re)start our peer because it is already started

                // clean-up
                if (core_context->monitor->network_device)
                    free (core_context->monitor->network_device);
                core_context->monitor->network_device = NULL;
                break;
            }
        }
        name = ziflist_next (iflist);
    }
    ziflist_destroy (&iflist);

    // NB: if expected network device has disappeared, we cannot detect an IP
    // address change before detecting device reappearance. This has consequences
    // in the logic used here.

    if (!found_network_device && (core_context->monitor->status != IGS_NETWORK_DEVICE_NOT_AVAILABLE)) {
        // we did not find our expected network device and this is new

        // check if our peer is started
        if (core_context->node) {
            // peer is started but network device was not found
            igs_warn ("network device %s has disappeared", core_context->network_device);

            // update status
            core_context->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;

            // save network values
            core_context->monitor->port = core_context->network_zyre_port;
            if (core_context->monitor->network_device){
                free (core_context->monitor->network_device);
                core_context->monitor->network_device = NULL;
            }
            if (core_context->network_device)
                core_context->monitor->network_device = strdup (core_context->network_device);
                

            need_to_call_callbacks = true;
            cb_event = IGS_NETWORK_DEVICE_NOT_AVAILABLE;
            if (core_context->network_device)
                cb_device = strdup (core_context->network_device);
            if (core_context->ip_address)
                cb_ip_address = strdup(core_context->ip_address);

            // check if we need to stop peer
            if (core_context->monitor_shall_start_stop_agent) {
                igs_warn ("stopping agent after network device %s has disappeared", cb_device);
                cb_stop = true;
            }
        }
        else if (core_context->monitor->network_device) {
            // peer is not started AND we have an expected network device
            igs_warn ("network device %s not found", core_context->monitor->network_device);

            // update status
            need_to_call_callbacks = true;
            cb_event = core_context->monitor->status = IGS_NETWORK_DEVICE_NOT_AVAILABLE;
            cb_device = strdup (core_context->monitor->network_device);
            cb_ip_address = NULL;
            // NB: we don't need to call igs_stop because our agent is not running
        }
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    
    if (cb_stop_start){
        igs_stop ();
        igs_start_with_device (cb_device, cb_port);
    } else if (cb_stop)
        igs_stop ();
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    if (need_to_call_callbacks){
        zlist_t *dup = zlist_dup(core_context->monitor_callbacks);
        model_read_write_unlock(__FUNCTION__, __LINE__);
        igs_monitor_wrapper_t *cb = zlist_first(dup);
        while (cb && cb->callback_ptr) {
            model_read_write_unlock(__FUNCTION__, __LINE__);
            cb->callback_ptr (cb_event, cb_device, cb_ip_address, cb->my_data);
            model_read_write_lock(__FUNCTION__, __LINE__);
            cb = zlist_next(dup);
        }
        if (cb_device)
            free(cb_device);
        if (cb_ip_address)
            free(cb_ip_address);
        zlist_destroy(&dup);
    }
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return 0;
}

// manage messages from the parent thread
int s_monitor_manage_parent (zloop_t *loop, zsock_t *socket, void *arg)
{
    IGS_UNUSED (loop)
    IGS_UNUSED (arg)
    zmsg_t *msg = zmsg_recv (socket);
    if (!msg) {
        igs_error ("Could not read message from main thread : Ingescape will "
                   "interrupt immediately.");
        return -1;
    }
    char *command = zmsg_popstr (msg);
    if (streq (command, "$TERM")) {
        free (command);
        zmsg_destroy (&msg);
        return -1;
    }
    // else : nothing to do so far
    free (command);
    zmsg_destroy (&msg);
    return 0;
}

static void s_monitor_init_loop (zsock_t *pipe, void *args)
{
    IGS_UNUSED (args)
    core_context->monitor->loop = zloop_new ();
    // zloop_set_verbose (core_context->monitor->loop, false);
    zloop_reader (core_context->monitor->loop, pipe, s_monitor_manage_parent, NULL);
    zloop_reader_set_tolerant (core_context->monitor->loop, pipe);
    zloop_timer (core_context->monitor->loop, core_context->monitor->period, 0, igs_monitor_trigger_network_check, NULL);
    zsock_signal (pipe, 0);
    zloop_start (core_context->monitor->loop);
    zloop_destroy (&core_context->monitor->loop);
}

void igs_monitor_start (unsigned int period)
{
    core_init_agent ();
    if (core_context->monitor) {
        igs_warn ("monitor is already started");
        return;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    core_context->monitor = (igs_monitor_t *) zmalloc (sizeof (igs_monitor_t));
    core_context->monitor->period = period;
    core_context->monitor->status = IGS_NETWORK_OK;
    core_context->monitor->monitor_actor = zactor_new (s_monitor_init_loop, NULL);
    assert (core_context->monitor->monitor_actor);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igs_monitor_start_with_network (unsigned int period,
                                     const char *network_device,
                                     unsigned int port)
{
    assert (network_device);
    assert (strlen (network_device) > 0);
    core_init_agent ();
    if (core_context->monitor) {
        igs_warn ("monitor is already started");
        return;
    }
    model_read_write_lock(__FUNCTION__, __LINE__);
    core_context->monitor = (igs_monitor_t *) zmalloc (sizeof (igs_monitor_t));
    core_context->monitor->period = period;
    core_context->monitor->status = IGS_NETWORK_OK;
    core_context->monitor->network_device = strdup (network_device);
    core_context->monitor->port = port;
    core_context->monitor->monitor_actor = zactor_new (s_monitor_init_loop, NULL);
    assert (core_context->monitor->monitor_actor);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igs_monitor_stop (void)
{
    core_init_agent ();
    if (!core_context->monitor)
        return;
    
    model_read_write_lock(__FUNCTION__, __LINE__);
    zstr_sendx (core_context->monitor->monitor_actor, "$TERM", NULL);
    if (core_context->monitor->monitor_actor)
        zactor_destroy (&core_context->monitor->monitor_actor);
    if (core_context->monitor->network_device)
        free (core_context->monitor->network_device);
    core_context->monitor->network_device = NULL;
    free (core_context->monitor);
    core_context->monitor = NULL;
    // igsagent_debug(agent, "still %d agents running in process",
    // igs_nb_of_agents_in_process);
#if defined(__WINDOWS__)
    // NB: If we don't call zsys_shutdown on windows, the application will crash
    // on exit (WSASTARTUP assertion failure)
    if (core_context->network_actor == NULL) {
        igs_debug ("calling zsys_shutdown");
        zsys_shutdown ();
    }
#endif
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

bool igs_monitor_is_running (void)
{
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    bool res = (core_context->monitor);
    model_read_write_unlock(__FUNCTION__, __LINE__);
    return res;
}

void igs_observe_monitor (igs_monitor_fn cb, void *my_data)
{
    core_init_agent ();
    assert (cb);
    model_read_write_lock(__FUNCTION__, __LINE__);
    igs_monitor_wrapper_t *new_cb = (igs_monitor_wrapper_t *) zmalloc (sizeof (igs_monitor_wrapper_t));
    new_cb->callback_ptr = cb;
    new_cb->my_data = my_data;
    zlist_append (core_context->monitor_callbacks, new_cb);
    model_read_write_unlock(__FUNCTION__, __LINE__);
}

void igs_monitor_set_start_stop (bool flag)
{
    core_init_agent ();
    model_read_write_lock(__FUNCTION__, __LINE__);
    core_context->monitor_shall_start_stop_agent = flag;
    model_read_write_unlock(__FUNCTION__, __LINE__);
}
