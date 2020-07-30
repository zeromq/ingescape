//
//  start.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef start_h
#define start_h

#include <Python.h>



// igs_startWithDevice
 PyObject * startWithDevice_wrapper(PyObject * self, PyObject * args);

//igs_startWithIP
 PyObject * startWithIP_wrapper(PyObject * self, PyObject * args);

//igs_stop
 PyObject * stop_wrapper(PyObject * self, PyObject * args);

PyDoc_STRVAR(
             startWithDeviceDoc,
             "igs_startWithDevice(device, port)\n"
             "--\n"
             "\n"
             "start the agent on a device");

PyDoc_STRVAR(
             startWithIPDoc,
             "igs_startWithIP(ipAdress, port)\n"
             "--\n"
             "\n"
             "start the agent on an IP");

PyDoc_STRVAR(
             stopDoc,
             "igs_stop()\n"
             "--\n"
             "\n"
             "stop the agent properly");

#endif /* start_h */
