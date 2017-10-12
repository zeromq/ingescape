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

    // Delete all VM of agents
    _agentsList.deleteAllItems();
}


/**
 * @brief Get the list of view models of agent from a name
 * @param name
 * @return
 */
QList<AgentVM*> AgentsSupervisionController::getAgentViewModelsListFromName(QString name)
{
    if (_mapFromNameToAgentViewModelsList.contains(name)) {
        return _mapFromNameToAgentViewModelsList.value(name);
    }
    else {
        return QList<AgentVM*>();
    }
}


/**
 * @brief Delete the previous view model of Agent
 * @param agent
 */
void AgentsSupervisionController::deleteAgentViewModel(AgentVM* agent)
{
    if (agent != NULL)
    {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());
        agentViewModelsList.removeOne(agent);

        // Update the list in the map
        _mapFromNameToAgentViewModelsList.insert(agent->name(), agentViewModelsList);

        // Free memory
        delete agent;
    }
}



/**
 * @brief Delete an agent from the list
 * @param agent to delete
 */
void AgentsSupervisionController::deleteAgent(AgentVM* agent)
{
    if ((_modelManager != NULL) && (agent != NULL))
    {
        qDebug() << "TODO: Delete agent" << agent->name();

        // TODO
        //_modelManager->delete ???
    }
}


/**
 * @brief Slot when a new model of agent has been created
 * @param agent
 */
void AgentsSupervisionController::onAgentModelCreated(AgentM* agent)
{
    if (agent != NULL)
    {
        // We don't have yet a definition for this agent, so we create a new VM until we will get its definition

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

        // Create a new view model of agent
        AgentVM* agentVM = new AgentVM(agent, this);

        agentViewModelsList.append(agentVM);
        _mapFromNameToAgentViewModelsList.insert(agent->name(), agentViewModelsList);

        // Add our view model to the list
        _agentsList.append(agentVM);
    }
}


/**
 * @brief Slot when a new model of agent definition has been created
 * @param definition
 * @param agent
 */
void AgentsSupervisionController::onAgentDefinitionCreated(DefinitionM* definition, AgentM* agent)
{
    if ((definition != NULL) && (agent != NULL) && (_modelManager != NULL))
    {
        QString agentName = agent->name();

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agentName);

        // Get the view model of agent that corresponds to our model
        AgentVM* agentVM = NULL;
        foreach (AgentVM* iterator, agentViewModelsList)
        {
            // If the view model has not yet a definition and contains our model of agent
            if ((iterator != NULL) && (iterator->definition() == NULL) && iterator->models()->contains(agent))
            {
                agentVM = iterator;
                break;
            }
        }

        if (agentVM != NULL)
        {
            DefinitionM* sameDefinition = NULL;
            AgentVM* agentUsingSameDefinition = NULL;

            foreach (AgentVM* iterator, agentViewModelsList)
            {
                // If this VM contains our model of agent
                if ((iterator != NULL) && (iterator->definition() != NULL)
                        &&
                        // Same version
                        (iterator->definition()->version() == definition->version())
                        &&
                        // Same Inputs, Outputs and Parameters
                        (iterator->definition()->md5Hash() == definition->md5Hash()))
                {
                    qDebug() << "There is exactly the same agent definition for name" << definition->name() << "and version" << definition->version();

                    // Exactly the same definition
                    sameDefinition = iterator->definition();
                    agentUsingSameDefinition = iterator;
                    break;
                }
            }

            // Exactly the same definition
            if ((sameDefinition != NULL) && (agentUsingSameDefinition != NULL))
            {
                //
                // 1- The previous view model of agent is useless, we have to remove it from the list
                //
                _agentsList.remove(agentVM);

                // And delete it
                deleteAgentViewModel(agentVM);
                agentVM = NULL;


                // 2- The definition of agent is useless, we have to delete it
                _modelManager->deleteAgentDefinition(definition);
                definition = NULL;


                //
                // 3- Then, add our model to the view model having the same definition
                //
                bool isSameModel = false;
                QList<AgentM*> models = agentUsingSameDefinition->models()->toList();
                for (int i = 0; i < models.count(); i++)
                {
                    AgentM* model = models.at(i);

                    // Same address and status is OFF
                    if ((model != NULL) && (model->address() == agent->address()) && (model->status() == AgentStatus::OFF))
                    {
                        isSameModel = true;

                        // Replace the model
                        agentUsingSameDefinition->models()->replace(i, agent);

                        qDebug() << "Replace model of agent" << agentName << "on" << agent->address();

                        // Delete the previous model of Agent
                        _modelManager->deleteAgentModel(model);
                        model = NULL;

                        // break loop on models
                        break;
                    }
                }

                if (!isSameModel) {
                    // Add the model of agent to the list of the VM
                    agentUsingSameDefinition->models()->append(agent);

                    qDebug() << "Add model of agent" << agentName << "on" << agent->address();
                }
            }
            // Definition is different
            else
            {
                // Set the definition
                agentVM->setdefinition(definition);
            }
        }
    }
}

