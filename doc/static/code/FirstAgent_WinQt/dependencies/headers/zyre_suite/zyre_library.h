/*  =========================================================================
    zyre - generated layer of public API

    Copyright (c) the Contributors as noted in the AUTHORS file.           
                                                                           
    This file is part of Zyre, an open-source framework for proximity-based
    peer-to-peer applications -- See http://zyre.org.                      
                                                                           
    This Source Code Form is subject to the terms of the Mozilla Public    
    License, v. 2.0. If a copy of the MPL was not distributed with this    
    file, You can obtain one at http://mozilla.org/MPL/2.0/.               

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef ZYRE_LIBRARY_H_INCLUDED
#define ZYRE_LIBRARY_H_INCLUDED

//  Set up environment for the application

//  External dependencies
#include <czmq.h>

//  ZYRE version macros for compile-time API detection
#define ZYRE_VERSION_MAJOR 2
#define ZYRE_VERSION_MINOR 0
#define ZYRE_VERSION_PATCH 1

#define ZYRE_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define ZYRE_VERSION \
    ZYRE_MAKE_VERSION(ZYRE_VERSION_MAJOR, ZYRE_VERSION_MINOR, ZYRE_VERSION_PATCH)

#if defined (__WINDOWS__)
#   if defined ZYRE_STATIC
#       define ZYRE_EXPORT
#   elif defined ZYRE_INTERNAL_BUILD
#       if defined DLL_EXPORT
#           define ZYRE_EXPORT __declspec(dllexport)
#       else
#           define ZYRE_EXPORT
#       endif
#   elif defined ZYRE_EXPORTS
#       define ZYRE_EXPORT __declspec(dllexport)
#   else
#       define ZYRE_EXPORT __declspec(dllimport)
#   endif
#   define ZYRE_PRIVATE
#else
#   define ZYRE_EXPORT
#   if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define ZYRE_PRIVATE __attribute__ ((visibility ("hidden")))
#   else
#       define ZYRE_PRIVATE
#   endif
#endif

//  Opaque class structures to allow forward references
//  These classes are stable or legacy and built in all releases
typedef struct _zyre_t zyre_t;
#define ZYRE_T_DEFINED
typedef struct _zyre_event_t zyre_event_t;
#define ZYRE_EVENT_T_DEFINED

//  Public constants
#define ZRE_DISCOVERY_PORT  5670               //  IANA-assigned UDP port for ZRE

//  Public classes, each with its own header file
#include "zyre_event.h"

#ifdef ZYRE_BUILD_DRAFT_API
//  Self test for private classes
ZYRE_EXPORT void
    zyre_private_selftest (bool verbose);
#endif // ZYRE_BUILD_DRAFT_API

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
