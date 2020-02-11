/*
 *	I2Quick
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef _I2QUICK_GLOBAL_H_
#define _I2QUICK_GLOBAL_H_


#include <QtCore/qglobal.h>


// Check if we want to build a shared library
#if defined(I2QUICK_BUILD_LIBRARY)
#define I2QUICK_EXPORT Q_DECL_EXPORT
// Check if I2Quick is included via a .pri file (i.e. its source code is included by a .pro file (another project))
#elif defined(I2QUICK_INCLUDED_VIA_PRI)
#define I2QUICK_EXPORT
#else
// I2Quick is used as a shared library
#define I2QUICK_EXPORT Q_DECL_IMPORT
#endif


#endif // _I2QUICK_GLOBAL_H_
