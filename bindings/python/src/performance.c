/*  =========================================================================
 * network.c
 *
 * Copyright (c) the Contributors as noted in the AUTHORS file.
 * This file is part of Ingescape, see https://github.com/zeromq/ingescape.
 * 
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *=========================================================================
 */

#define PY_SSIZE_T_CLEAN
#include <stdio.h>
#include <Python.h>

#include "ingescape_python.h"

PyObject * igs_net_performance_check_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"peer_id", "msg_size", "msgs_nbr", NULL};
    char * peer_id = NULL;
    int msg_size = 0;
    int msgs_nbr = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &peer_id, &msg_size, &msgs_nbr))
        return NULL;
    igs_net_performance_check(peer_id, (size_t)msg_size, (size_t)msgs_nbr);
    return PyLong_FromLong(IGS_SUCCESS);
}
