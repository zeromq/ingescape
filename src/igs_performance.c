/*  =========================================================================
    performance -  performance checks

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of Ingescape, see https://github.com/zeromq/ingescape.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

#include "ingescape_classes.h"
#include "ingescape_private.h"
#include <czmq.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t performance_msg_counter = 0;
size_t performance_msg_count_target = 0;
size_t performance_msg_size = 0;
int64_t performance_start = 0;
int64_t performance_stop = 0;

void igs_net_performance_check (const char *peer_id,
                                size_t msg_size,
                                size_t nb_of_msg)
{
    core_init_context ();
    if (core_context->node == NULL) {
        igs_error ("agent must be started to execute performance tests");
        return;
    }
    if (performance_msg_count_target != 0) {
        igs_error ("check already in progress");
        return;
    }
    if (nb_of_msg == 0) {
        igs_error ("nb_of_msg must be greater than zero");
        return;
    }

    performance_msg_count_target = nb_of_msg;
    performance_msg_counter = 1;
    performance_msg_size = msg_size;

    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, PING_MSG);
    zmsg_addmem (msg, &performance_msg_counter, sizeof (size_t));
    void *mem = (void *) zmalloc (msg_size);
    zmsg_addmem (msg, mem, msg_size);
    zyre_t *node = core_context->node;

    performance_start = zclock_usecs ();
    s_lock_zyre_peer (__FUNCTION__, __LINE__);
    zyre_whisper (node, peer_id, &msg);
    s_unlock_zyre_peer (__FUNCTION__, __LINE__);
    free (mem);
}
