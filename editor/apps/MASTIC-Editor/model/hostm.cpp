/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "hostm.h"

/**
 * @brief Constructor
 * @param name
 * @param peerId
 * @param ipAddress
 * @param parent
 */
HostM::HostM(QString name,
             QString peerId,
             QString ipAddress,
             QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _ipAddress(ipAddress)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Host" << _name << "(" << _peerId << ") at" << _ipAddress;
}


/**
 * @brief Destructor
 */
HostM::~HostM()
{
    qInfo() << "Delete Model of Host" << _name << "(" << _peerId << ") at" << _ipAddress;
}
