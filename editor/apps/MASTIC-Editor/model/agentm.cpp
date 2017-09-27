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
 * @brief Default constructor
 * @param parent
 */
AgentM::AgentM(QObject *parent) : QObject(parent),
    _name(""),
    _networkDevice(""),
    _ipAddress(""),
    _port(-1),
    _version(""),
    _description(""),
    _canBeFrozen(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Model of Agent ...";
}


/**
 * @brief Destructor
 */
AgentM::~AgentM()
{
    qInfo() << "Delete Model of Agent ...";
}
