/*
 *	IngeScape Assessments
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

#include "agentnameandoutputsm.h"

/**
 * @brief Constructor
 * @param agentName
 * @param outputNamesList
 * @param parent
 */
AgentNameAndOutputsM::AgentNameAndOutputsM(QString agentName,
                                           QStringList outputNamesList,
                                           QObject *parent) : QObject(parent),
    _agentName(agentName),
    _outputNamesList(outputNamesList)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of (simplified) Agent" << _agentName << "with outputs:" << _outputNamesList;
}


/**
 * @brief Destructor
 */
AgentNameAndOutputsM::~AgentNameAndOutputsM()
{
    qInfo() << "Delete Model of (simplified) Agent" << _agentName << "with outputs:" << _outputNamesList;
}
