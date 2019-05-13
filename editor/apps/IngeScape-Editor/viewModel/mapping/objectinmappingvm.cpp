/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
 * @param name
 * @param position
 * @param parent
 */
ObjectInMappingVM::ObjectInMappingVM(QString name,
                                     QPointF position,
                                     QObject *parent) : QObject(parent),
    _name(name),
    _position(position)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Object" << _name << "in the global mapping";
}


/**
 * @brief Destructor
 */
ObjectInMappingVM::~ObjectInMappingVM()
{
    //qInfo() << "Delete Object" << _name << "in the global mapping";
}
