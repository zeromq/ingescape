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

#include "agentmappingm.h"

/**
 * @brief Constructor
 * @param parent
 */
AgentMappingM::AgentMappingM(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Agent Mapping...";
}


/**
 * @brief Destructor
 */
AgentMappingM::~AgentMappingM()
{
    qInfo() << "Delete Model of Agent Mapping ...";
}
