#include "util.h"

void call_callback(PyObject* callback, PyObject* args){
    assert(callback);
    assert(args);

    PyObject* result = PyObject_CallObject(callback, args);
    if (result != NULL)
        Py_DECREF(result);
    else{
        if (PyErr_Occurred()){
            PyErr_Print();
        }
    }
}
