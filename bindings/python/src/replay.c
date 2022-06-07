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

PyObject * igs_replay_init_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"log_file_path", "speed", "start_time", "wait_for_start", "replay_mode", "agent", NULL};
    char * log_file_path = NULL;
    int speed = 0;
    char * start_time = NULL;
    bool wait_for_start = false;
    int replay_mode = 0;
    char * agent = NULL;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "sisbis", kwlist, &log_file_path, &speed, &start_time, &wait_for_start, &replay_mode, &agent))
        return NULL;
    igs_replay_init(log_file_path,(size_t) speed, start_time, wait_for_start, (uint)replay_mode, agent);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_replay_start_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    igs_replay_start();
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_replay_pause_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"pause", NULL};
    bool pause = false;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &pause))
        return NULL;
    igs_replay_pause(pause);
    return PyLong_FromLong(IGS_SUCCESS);
}

PyObject * igs_replay_terminate_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    igs_replay_terminate();
    return PyLong_FromLong(IGS_SUCCESS);
}
