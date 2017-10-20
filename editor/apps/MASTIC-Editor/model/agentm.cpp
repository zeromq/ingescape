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
 * @brief Constructor without peer id and address
 * @param name
 * @param parent
 */
AgentM::AgentM(QString name,
               QObject *parent) : AgentM(name,
                                         "",
                                         "",
                                         parent)
{
}


/**
 * @brief Constructor with peer id and address
 * @param name
 * @param peerId
 * @param address
 * @param parent
 */
AgentM::AgentM(QString name,
               QString peerId,
               QString address,
               QObject *parent) : QObject(parent),
    _name(name),
    _peerId(peerId),
    _address(address),
    _hostname(""),
    _executionPath(""),
    _pid(0),
    _isON(false),
    _isMuted(false),
    _canBeFrozen(false),
    _isFrozen(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent" << _name << "(" << _peerId << ") at" << _address;
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent" << _name << "(" << _peerId << ") at" << _address;
}
