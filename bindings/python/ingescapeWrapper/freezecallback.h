//
//  freezecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef freezecallback_h
#define freezecallback_h

#include <Python.h>

PyDoc_STRVAR(
             observeFreezeDoc,
             "igs_observeFreeze(callback, args)\n"
             "--\n"
             "\n"
             "Observe an agent and exeute a callback when froze.\n"
             "When the agent is frozen the associated callback will be called.\n \n"
             "param callback is the function we want to be executed when the agent is frozen. Can't be NULL.\n"
             "param args A tuple containing the arguments of the callback in python. Can't be NULL.\n "
             "return The error.\n 0 is ok\n");

// wrapper for igs_observeFreeze
PyObject * igs_observeFreeze_wrapper(PyObject *self, PyObject *args);

#endif /* freezecallback_h */
