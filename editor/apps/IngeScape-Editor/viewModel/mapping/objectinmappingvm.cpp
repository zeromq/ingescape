/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "objectinmappingvm.h"

/**
 * @brief Constructor
 * @param type
 * @param name
 * @param uid
 * @param position
 * @param parent
 */
ObjectInMappingVM::ObjectInMappingVM(ObjectInMappingTypes::Value type,
                                     //QString name,
                                     //QString uid,
                                     QPointF position,
                                     QObject *parent) : QObject(parent),
    _type(type),
    _name(""),
    _uid(""),
    _position(position),
    _isReduced(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New (Object)" << ObjectInMappingTypes::staticEnumToKey(_type) << _name << "in the global mapping";
}


/**
 * @brief Destructor
 */
ObjectInMappingVM::~ObjectInMappingVM()
{
    //qInfo() << "Delete (Object)" << ObjectInMappingTypes::staticEnumToKey(_type) << _name << "in the global mapping";
}
