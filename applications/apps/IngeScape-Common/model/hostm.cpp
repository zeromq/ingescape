/*
 *	IngeScape Editor
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
 */
HostM::HostM(QString name,
             PeerM* peer,
             QObject *parent) : QObject(parent),
    _name(name),
    _peer(peer)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_peer != nullptr) {
        qInfo() << "New Model of Host" << _name << "at" << _peer->ipAddress();
    }
    else {
        qInfo() << "New Model of Host" << _name << "with no peer";
    }
}


/**
 * @brief Destructor
 */
HostM::~HostM()
{
    if (_peer != nullptr) {
        qInfo() << "Delete Model of Host" << _name << "at" << _peer->ipAddress();
    }
    else {
        qInfo() << "Delete Model of Host" << _name << "with no peer";
    }
}


void HostM::setpeer(PeerM *value)
{
    if (_peer != value)
    {
        _peer = value;

        Q_EMIT peerChanged(value);
    }
}
