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
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "si", &networkDevice, &port)) {
        return NULL;
    }
    result = igs_startWithDevice(networkDevice, port);
    return PyLong_FromLong(result);
}


//igs_startWithIP
 PyObject * startWithIP_wrapper(PyObject * self, PyObject * args)
{
    char * ipAdresse;
    int port;
    int result;
    // parse arguments received from python function into c object
    if (!PyArg_ParseTuple(args, "si", &ipAdresse, &port)) {
        return NULL;
    }
    result = igs_startWithDevice(ipAdresse, port);
    return PyLong_FromLong(result);
}


//igs_stop
 PyObject * stop_wrapper(PyObject * self, PyObject * args)
{
    igs_stop();
    return PyLong_FromLong(0);
}