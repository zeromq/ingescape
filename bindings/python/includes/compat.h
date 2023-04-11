#ifndef _INGESCAPE_PY_COMPAT_H
#define _INGESCAPE_PY_COMPAT_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <frameobject.h>

// PyFrame_GetCode is available since python 3.9
// And the PyFrameObject* struct is opaque since python 3.11
// We make PyFrame_GetCode available for python 3.8 and older
// https://docs.python.org/3/whatsnew/3.11.html#pyframeobject-3-11-hiding
#if PY_VERSION_HEX < 0x030900B1
PyCodeObject* PyFrame_GetCode(PyFrameObject *frame);
#endif


#endif // _INGESCAPE_PY_COMPAT_H
