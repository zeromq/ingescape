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

#include "agentexecutiononhostm.h"

/**
 * @brief Constructor
 * @param agentName
 * @param hostname
 * @param commandLine
 * @param parent
 */
AgentExecutionOnHostM::AgentExecutionOnHostM(QString agentName,
                                             QString hostname,
                                             QString commandLine,
                                             QObject *parent) : QObject(parent),
    _agentName(agentName),
    _hostname(hostname),
    _commandLine(commandLine)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent (" << _agentName << ") Execution on Host" << _hostname;
}


/**
 * @brief Destructor
 */
AgentExecutionOnHostM::~AgentExecutionOnHostM()
{
    qInfo() << "Delete Model of Agent (" << _agentName << ") Execution on Host" << _hostname;
}
