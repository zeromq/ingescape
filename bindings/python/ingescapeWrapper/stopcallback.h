//
//  stopcallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef stopcallback_h
#define stopcallback_h

#include <Python.h>

PyDoc_STRVAR(
             observeForcedStopDoc,
             "igs_observeForcedStop(callback, args)\n"
             "--\n"
             "\n"
             "Observe an agent and exeute a callback when stopped.\n"
             "When the agent is stopped the associated callback will be called.\n \n"
             "param callback is the function we want to be executed when the agent is stopped. Can't be NULL.\n"
             "param args A tuple containing the arguments of the callback in python. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

// wrapper for igs_observeForcedStop
PyObject * igs_observeExternalStop_wrapper(PyObject *self, PyObject *args);

#endif /* stopcallback_h */
