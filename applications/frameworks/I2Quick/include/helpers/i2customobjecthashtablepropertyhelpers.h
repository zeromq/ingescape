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

#ifndef _I2_CUSTOM_OBJECT_HASHTABLE_PROPERTYHELPERS_H_
#define _I2_CUSTOM_OBJECT_HASHTABLE_PROPERTYHELPERS_H_


#include <QObject>
#include <QtQml>
#include <QQmlProperty>
#include <QtGlobal>

#include "i2quick_global.h"
#include "base/I2CustomItemHashModel.h"




/*
 **************************************************************
 *
 *  Hash of QObjects
 *
 **************************************************************
 */


/*!
   Define a hashtable property that is read-only from QML

   \param type The C++ type of items used as value of our hashtable property. C++ type must derive from QObject
   \param name The name of our property

    It generates the following code :
    \code
        I2CustomItemHashModel<{type}> * {name}(); // Public getter method
        I2CustomItemHashModel<{type}> _{name};    // Protected hashtable used to store our items
    \endcode
*/
#define I2_QOBJECT_HASHMODEL(type, name) \
      Q_PROPERTY(AbstractI2CustomItemHashModel * name READ name CONSTANT) \
    public: \
        I2CustomItemHashModel<type> * name() { \
            return &_##name; \
        } \
    protected: \
        I2CustomItemHashModel<type> _##name;





#endif // _I2_CUSTOM_OBJECT_HASHTABLE_PROPERTYHELPERS_H_
