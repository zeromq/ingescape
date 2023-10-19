#ifndef _UTIL_H
#define _UTIL_H

#include <Python.h>

/**
 * Helper method used when python callbacks need to be called from C
 * This method checks the success of the python callback after calling it
 * and prints any exception that might have been raised
 */
void call_callback(PyObject* callback, PyObject* args);

#endif // #_UTIL_H
