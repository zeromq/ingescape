/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


/*!
    This header defines a couple of helper macros that can help saving development time,
    by avoiding manual code duplication, often leading to heavy copy-and-paste, which is
    largely error-prone and not productive at all.
*/


#ifndef _I2_PROPERTY_HELPERS_
#define _I2_PROPERTY_HELPERS_


// To export symbols
#include "i2quick_global.h"

// Helpers for C++ only properties
#include "helpers/i2cpponlypropertyhelpers.h"


// Helpers for simple QML properties
#include "helpers/i2qmlpropertyhelpers.h"


// Helpers for hashtable properties
#include "helpers/i2customobjecthashtablepropertyhelpers.h"


// Helpers for custom list properties
#include "helpers/i2customobjectlistpropertyhelpers.h"


// Helpers for enum properties
#include "helpers/i2enumpropertyhelpers.h"




#endif // _I2_PROPERTY_HELPERS_

