/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2019 Ingenuity i/o. All rights reserved.
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


#ifndef _I2_CUSTOM_OBJECT_LIST_PROPERTYHELPERS_H_
#define _I2_CUSTOM_OBJECT_LIST_PROPERTYHELPERS_H_


#include <QObject>
#include <QtQml>
#include <QQmlProperty>
#include <QtGlobal>

#include "i2quick_global.h"
#include "base/I2CustomItemListModel.h"
#include "base/I2CustomItemListSortFilterProxyModel.h"





/*
 **************************************************************
 *
 *  Lists of QObjects
 *
 **************************************************************
 */


/*!
   Define a list property that is read-only from QML AND can be associated to a sort-filter proxy

   NB: this type does not provide sorting or filtering operations BUT it can be used as a source model
       by a I2_QOBJECT_SORTFILTERPROXY (sort-filter proxy only)

   \param type The C++ type of items of our list property. C++ type must derive from QObject
   \param name The name of our property

    It generates the following code :
    \code
        I2CustomItemListModel<{type}> * {name}(); // Public getter method
        I2CustomItemListModel<{type}> _{name}; // Protected list used to store our items
    \endcode
*/
#define I2_QOBJECT_LISTMODEL(type, name) \
      Q_PROPERTY(AbstractI2CustomItemListModel * name READ name CONSTANT) \
    public: \
        I2CustomItemListModel<type> * name() { \
            return &_##name; \
        } \
    protected: \
        I2CustomItemListModel<type> _##name;




/*!
   Define a sort-filter proxy for a list property, i.e. it allows to filter and sort
   a I2_QOBJECT_LISTMODEL property
   Multiple sort-filter proxies can be associated to the same I2_QOBJECT_LISTMODEL property

   \param name The name of our property

    It generates the following code :
    \code
        I2CustomItemSortFilterProxyModel<{type}> * {name}(); // Public getter method
        I2CustomItemSortFilterListModel<{type}> _{name}; // Protected sort/filter proxy
    \endcode
*/
#define I2_QOBJECT_SORTFILTERPROXY(type, name) \
      Q_PROPERTY(AbstractI2CustomItemSortFilterProxyModel * name READ name CONSTANT) \
    public: \
        I2CustomItemSortFilterProxyModel<type> * name() { \
            return &_##name; \
        } \
    protected: \
        I2CustomItemSortFilterProxyModel<type> _##name;



/*!
   Define a sortable and filterable list (all-in-one) property that is read-only from QML
   i.e. it's basically the equivalent of a combination of a I2_QOBJECT_LISTMODEL and
   a I2_QOBJECT_SORTFILTERPROXY (all-in-one instead of two separated properties)

   \param type The C++ type of items of our list property. C++ type must derive from QObject
   \param name The name of our property

    It generates the following code :
    \code
        I2CustomItemSortFilterListModel<{type}> * {name}(); // Public getter method
        I2CustomItemSortFilterListModel<{type}> _{name}; // Protected list used to store our items
    \endcode
*/
#define I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(type, name) \
      Q_PROPERTY(AbstractI2CustomItemSortFilterProxyModel* name READ name CONSTANT) \
    public: \
        I2CustomItemSortFilterListModel<type> * name() { \
            return &_##name; \
        } \
    protected: \
        I2CustomItemSortFilterListModel<type> _##name;







#endif // _I2_CUSTOM_OBJECT_LIST_PROPERTYHELPERS_H_
