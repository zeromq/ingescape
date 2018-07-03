//
//  start.c
//  ingescapeWrapp
//
//  Created by vaugien on 06/02/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#include "start.h"
#include <stdio.h>
#include <ingescape/ingescape.h>


/* igs_startWithDevice
 *Function in c that wrapp the igs_startWithdevice function in Python
 */
 PyObject * startWithDevice_wrapper(PyObject * self, PyObject * args)
{
    char * networkDevice;
    int port;
    int result;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "si", &networkDevice, &port)) {
        return NULL;
    }
    result = igs_startWithDevice(networkDevice, port);
    
    // cast the result into a Python object.
    ret = PyLong_FromLong(result);
    
    return ret;
}


//igs_startWithIP
 PyObject * startWithIP_wrapper(PyObject * self, PyObject * args)
{
    char * ipAdresse;
    int port;
    int result;
    PyObject * ret;
    
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "si", &ipAdresse, &port)) {
        return NULL;
    }

    result = igs_startWithDevice(ipAdresse, port);
    
    // build the resulting int into a Python object and return it
    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}


//igs_stop
 PyObject * stop_wrapper(PyObject * self, PyObject * args)
{
    int result;
    PyObject * ret;

    result = igs_stop();

    ret = PyLong_FromLong(result);
    free(&result);
    return ret;
}

//igs_die
 PyObject * die_wrapper(PyObject * self, PyObject * args)
{

    PyObject * ret;

    igs_die();

    ret = PyLong_FromLong(1);
    return(ret);
}

