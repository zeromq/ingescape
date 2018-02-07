/*
 *  Mastic - QML binding
 *
 *  Copyright (c) 2018 Ingenuity i/o. All rights reserved.
 *
 *  See license terms for the rights and conditions
 *  defined by copyright holders.
 *
 *
 *  Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef _MASTIQUICK_GLOBAL_H_
#define _MASTIQUICK_GLOBAL_H_


#include <QtCore/qglobal.h>


// Check if we want to build a shared library
#if defined(MASTICQUICK_BUILD_LIBRARY)
#define MASTICQUICK_EXPORT Q_DECL_EXPORT
// Check if MasticQuick is included via a .pri file (i.e. its source code is included by a .pro file (another project))
#elif defined(MASTICQUICK_INCLUDED_VIA_PRI)
#define MASTICQUICK_EXPORT
#else
// MasticQuick is used as a shared library
#define MASTICQUICK_EXPORT Q_DECL_IMPORT
#endif


#endif // _MASTIQUICK_GLOBAL_H_
