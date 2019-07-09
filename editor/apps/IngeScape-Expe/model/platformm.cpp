/*
 *	IngeScape Expe
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "platformm.h"

/**
 * @brief Constructor
 * @param name
 * @param filePath
 * @param parent
 */
PlatformM::PlatformM(QString name,
                     QString filePath,
                     QObject *parent) : QObject(parent),
    _name(name),
    _filePath(filePath)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Platform" << _name << "at" << _filePath;
}


/**
 * @brief Destructor
 */
PlatformM::~PlatformM()
{
    qInfo() << "Delete Model of Platform" << _name << "at" << _filePath;

}
