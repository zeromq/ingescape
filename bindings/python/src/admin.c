/*  =========================================================================
 * admin.c
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

PyObject * version_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_version());
}

PyObject * protocol_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_protocol());
}

PyObject * log_set_console_wrapper(PyObject * self, PyObject * args)
{
    bool verbose;
    if (!PyArg_ParseTuple(args, "b", &verbose))
        return NULL;
    igs_log_set_console(verbose);
    return PyLong_FromLong(0);
}

PyObject * log_console_wrapper(PyObject * self, PyObject * args)
{
    if(igs_log_console())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * log_syslog_wrapper(PyObject * self, PyObject * args)
{
    if(igs_log_syslog())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * log_set_syslog_wrapper(PyObject * self, PyObject * args)
{
    bool verbose;
    if (!PyArg_ParseTuple(args, "b", &verbose))
        return NULL;
    igs_log_set_syslog(verbose);
    return PyLong_FromLong(0);
}

PyObject * log_set_stream_wrapper(PyObject * self, PyObject * args)
{
    bool stream;
    if (!PyArg_ParseTuple(args, "b", &stream))
        return NULL;
    igs_log_set_stream(stream);
    return PyLong_FromLong(0);
}

PyObject * log_stream_wrapper(PyObject * self, PyObject * args)
{
    if(igs_log_stream())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * log_set_file_wrapper(PyObject * self, PyObject * args)
{
    bool useLogFile;
    PyObject *pathObject;
    if (!PyArg_ParseTuple(args, "bO", &useLogFile, &pathObject)) // First cast the second parameter as PyObject to check against Py_None
        return NULL;
    if(pathObject != Py_None)
    {
        char *path_c;
        if (!PyArg_ParseTuple(args, "bs", &useLogFile, &path_c)) // Second patameter is not Py_None, we cast it to string
            return NULL;
        igs_log_set_file(useLogFile, path_c);
    }
    else
        igs_log_set_file(useLogFile, NULL);
    return PyLong_FromLong(0);
}

PyObject * log_file_wrapper(PyObject * self, PyObject * args)
{
    if(igs_log_file())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * log_set_console_color_wrapper(PyObject * self, PyObject * args)
{
    bool useColorVerbose;
    if (!PyArg_ParseTuple(args, "b", &useColorVerbose))
        return NULL;
    igs_log_set_console_color(useColorVerbose);
    return PyLong_FromLong(0);
}

PyObject * log_console_color_wrapper(PyObject * self, PyObject * args)
{
    if(igs_log_console_color())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyObject * log_set_file_path_wrapper(PyObject * self, PyObject * args)
{
    char * path;
    if (!PyArg_ParseTuple(args, "s", &path))
        return NULL;
    igs_log_set_file_path(path);
    return PyLong_FromLong(0);
}

PyObject * log_file_path_wrapper(PyObject * self, PyObject * args)
{
    char * path = igs_log_file_path();
    if(path != NULL)
        return Py_BuildValue("s", path);
    else
        return Py_BuildValue("s", "");;
}

PyObject * trace_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    // Get function name to print it in log
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return 0;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_TRACE, "main", "%s", log);
    else
        igs_log(IGS_LOG_DEBUG, functionStr, "%s", log);
    return PyLong_FromLong(0);
}

PyObject * debug_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    // Get function name to print it in log
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return 0;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_DEBUG, "main", "%s", log);
    else
        igs_log(IGS_LOG_DEBUG, functionStr, "%s", log);
    return PyLong_FromLong(0);
}


PyObject * info_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return 0;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_INFO, "main", "%s", log);
    else
        igs_log(IGS_LOG_INFO, functionStr, "%s", log);
    return PyLong_FromLong(0);
}


PyObject * warn_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return 0;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_WARN, "main", "%s", log);
    else
        igs_log(IGS_LOG_WARN, functionStr, "%s", log);
    return PyLong_FromLong(0);
}

PyObject * error_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);
    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return 0;
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_ERROR, "main", "%s", log);
    else
        igs_log(IGS_LOG_ERROR, functionStr, "%s", log);
    return PyLong_FromLong(0);
}

PyObject * fatal_wrapper(PyObject * self, PyObject * args)
{
    char * log;
    if (!PyArg_ParseTuple(args, "s", &log))
        return NULL;
    PyFrameObject* frame = PyEval_GetFrame();
    Py_INCREF(frame);
    PyCodeObject* f_code = PyFrame_GetCode(frame);
    PyObject *function = f_code->co_name;
    Py_INCREF(function);
    Py_DECREF(frame);

    PyObject* funcTuple = Py_BuildValue("(O)", function);
    Py_DECREF(function);
    if (!funcTuple) return 0;
    const char* functionStr = 0;
    if (!PyArg_ParseTuple(funcTuple, "s", &functionStr)) {
        Py_DECREF(args);
        return PyLong_FromLong(1);
    }
    Py_DECREF(funcTuple);
    if(streq(functionStr, "<module>"))
        igs_log(IGS_LOG_FATAL, "main", "%s", log);
    else
        igs_log(IGS_LOG_FATAL, functionStr, "%s", log);
    return PyLong_FromLong(0);
}

PyObject * log_set_console_level_wrapper(PyObject * self, PyObject * args)
{
    int logLevel;
    if (!PyArg_ParseTuple(args, "i", &logLevel))
        return NULL;
    igs_log_set_console_level(logLevel);
    return PyLong_FromLong(0);
}

PyObject * log_console_level_wrapper(PyObject * self, PyObject * args)
{
    return PyLong_FromLong(igs_log_console_level());
}

PyObject * log_set_file_level_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"level", NULL};
    int level = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &level))
        return NULL;
    igs_log_set_file_level(level);
    return PyLong_FromLong(0);
}


PyObject * log_set_file_max_line_length_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"size", NULL};
    int size = 0;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "i", kwlist, &size))
        return NULL;
    igs_log_set_file_max_line_length((size_t)size);
    return PyLong_FromLong(0);
}

PyObject * log_include_data_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"enable", NULL};
    bool enable = true;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &enable))
        return NULL;
    igs_log_include_data(enable);
    return PyLong_FromLong(0);
}

PyObject * igs_log_include_services_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"enable", NULL};
    bool enable = true;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &enable))
        return NULL;
    igs_log_include_services(enable);
    return PyLong_FromLong(0);
}

PyObject * igs_log_no_warning_if_undefined_service_wrapper(PyObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"enable", NULL};
    bool enable = true;
    if (!PyArg_ParseTupleAndKeywords(args, NULL, "b", kwlist, &enable))
        return NULL;
    igs_log_no_warning_if_undefined_service(enable);
    return PyLong_FromLong(0);
}
