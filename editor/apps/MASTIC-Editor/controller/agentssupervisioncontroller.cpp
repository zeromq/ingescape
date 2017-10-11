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
AgentsSupervisionController::AgentsSupervisionController(MasticModelManager* modelManager, QObject *parent) : QObject(parent),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {
        _agentsList.setSourceModel(modelManager->allAgentsVM());
        _agentsList.setSortProperty("name");

        //_agentsList.setFilterProperty("TODO");
        //_agentsList.setFilterFixedString("true");
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
 * @brief Delete an agent from the list
 * @param agent to delete
 */
void AgentsSupervisionController::deleteAgent(AgentVM* agent)
{
    if ((_modelManager != NULL) && (agent != NULL))
    {
        qDebug() << "Delete agent" << agent->name();

        // TODO
        //_modelManager->deleteAgentVMFromList(agentToDelete);
    }
}

