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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentm.h"

#include <QQmlEngine>
#include <QDebug>

/**
 * @brief Constructor
 * @param name
 * @param peerId
 * @param parent
 */
AgentM::AgentM(QString name,
               QString peerId,
               QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _hostname(""),
    _executionPath(""),
    _canBeFrozen(false)
    //_md5Hash("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent" << _name << "(" << _peerId << ")";
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent" << _name << "(" << _peerId << ")";
}
