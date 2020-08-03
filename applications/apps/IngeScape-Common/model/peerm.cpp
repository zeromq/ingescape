/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "peerm.h"

PeerM::PeerM(IngeScapeTypes::Value igsType,
             QString uid,
             QString name,
             QString ipAddress,
             QString hostname,
             QString commandLine,
             bool isON,
             QObject *parent) : QObject(parent),
    _igsType(igsType),
    _uid(uid),
    _name(name),
    _ipAddress(ipAddress),
    _hostname(hostname),
    _commandLine(commandLine),
    _isON(isON),
    _loggerPort(""),
    _streamingPort("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Peer" << _name << "(" << _uid << ") of type" << _igsType << "on IP" << _ipAddress << "(" << _hostname << ")";
}


PeerM::~PeerM()
{
    qInfo() << "Delete Model of Peer" << _name << "(" << _uid << ") of type" << _igsType << "on IP" << _ipAddress << "(" << _hostname << ")";
}
