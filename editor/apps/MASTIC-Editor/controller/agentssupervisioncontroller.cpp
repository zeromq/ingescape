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

#include "agentssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
AgentsSupervisionController::AgentsSupervisionController(MasticModelManager* modelManager, QObject *parent)
    : QObject(parent),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {
        // TODO: Gestion du tri et du filtre des agents
    }
}


/**
 * @brief Destructor
 */
AgentsSupervisionController::~AgentsSupervisionController()
{
    _modelManager = NULL;
}

/**
 * @brief Delete an agent VM from the list
 * @param agent VM to delete
 */
void AgentsSupervisionController::deleteAgentFromList(AgentVM * agentToDelete)
{
    if(_modelManager != NULL && agentToDelete != NULL)
    {
        _modelManager->deleteAgentVMFromList(agentToDelete);
    }
}

