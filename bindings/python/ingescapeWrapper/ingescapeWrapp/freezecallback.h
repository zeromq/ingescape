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

// wrapper for igs_observeFreeze
PyObject * igs_observeFreeze_wrapper(PyObject *self, PyObject *args);

#endif /* freezecallback_h */
