//
//  observecallback.h
//  ingescapeWrapp
//
//  Created by vaugien on 06/04/2018.
//  Copyright © 2018 ingenuity. All rights reserved.
//

#ifndef observecallback_h
#define observecallback_h

#include <Python.h>

 PyObject * igs_observeInput_wrapper(PyObject *self, PyObject *args);

 PyObject * igs_observeOutput_wrapper(PyObject *self, PyObject *args);

 PyObject * igs_observeParameter_wrapper(PyObject *self, PyObject *args);

#endif /* observecallback_h */
