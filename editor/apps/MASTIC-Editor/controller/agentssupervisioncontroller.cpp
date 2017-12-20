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
AgentsSupervisionController::AgentsSupervisionController(MasticModelManager* modelManager,
                                                         QObject *parent) : QObject(parent),
    _selectedAgent(NULL),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {
        _agentsList.setSortProperty("name");

        // FIXME TODO: Add another property for sorting, because variants can provoke re-order in the list
        //_agentsList.setSortProperty("TODO");

        //_agentsList.setFilterProperty("TODO");
        //_agentsList.setFilterFixedString("true");
        //_agentsList.setFilterProperty("currentState");
        //_agentsList.setFilterFixedString(QString::number(SegmentZoneStates::ENCOMBRE));
    }
}


/**
 * @brief Destructor
 */
AgentsSupervisionController::~AgentsSupervisionController()
{
    // Clean-up current selection
    setselectedAgent(NULL);

    _mapFromNameToAgentViewModelsList.clear();

    // Delete all VM of agents
    _agentsList.deleteAllItems();

    _modelManager = NULL;
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
 * @brief Delete the selected agent from the list
 */
void AgentsSupervisionController::deleteSelectedAgent()
{
    if ((_modelManager != NULL) && (_selectedAgent != NULL))
    {
        qDebug() << "Delete _selectedAgent " << _selectedAgent->name();

        // Remove it from the list
        _agentsList.remove(_selectedAgent);

        // Reset its definition
        _selectedAgent->setdefinition(NULL);

        // Delete each model of _selectedAgent
        foreach (AgentM* model, _selectedAgent->models()->toList()) {
            _modelManager->deleteAgentModel(model);
        }

        // Delete the view model of _selectedAgent
        _deleteAgentViewModel(_selectedAgent);
        setselectedAgent(NULL);
    }
}


/**
 * @brief Open the definition of an agent of the list
 * @param agent which contain the definition
 */
void AgentsSupervisionController::openDefinition(AgentVM* agent)
{
    if ((agent != NULL) && (agent->definition() != NULL) && (_modelManager != NULL))
    {
        DefinitionM* definition = agent->definition();

        QList<DefinitionM*> definitionsToOpen;

        // Variant --> we have to open each variants of this definition
        if (definition->isVariant())
        {
            // Get the list (of models) of agent definition from a definition name
            QList<DefinitionM*> agentDefinitionsList = _modelManager->getAgentDefinitionsListFromDefinitionName(definition->name());

            foreach (DefinitionM* iterator, agentDefinitionsList) {
                // Same name, same version and variant, we have to open it
                if ((iterator != NULL) && iterator->isVariant() && (iterator->version() == definition->version())) {
                    definitionsToOpen.append(iterator);
                }
            }
        }
        else {
            // Simply add our definition
            definitionsToOpen.append(definition);
        }

        // Traverse the list of definitions to open
        foreach (DefinitionM* iterator, definitionsToOpen) {
            if (!_modelManager->openedDefinitions()->contains(iterator)) {
                _modelManager->openedDefinitions()->append(iterator);
            }
        }
    }
}


/**
 * @brief Export the agents list to default file
 */
void AgentsSupervisionController::exportAgentsListToDefaultFile()
{
    if (_modelManager != NULL)
    {   
        // Get the agents list to export
        QList<QPair<QStringList, DefinitionM*>> agentsListToExport = _getAgentsListToExport();

        // Export the agents list to default file
        _modelManager->exportAgentsListToDefaultFile(agentsListToExport);
    }
}


/**
 * @brief Export the agents list to selected file
 */
void AgentsSupervisionController::exportAgentsListToSelectedFile()
{
    if (_modelManager != NULL)
    {
        // Get the agents list to export
        QList<QPair<QStringList, DefinitionM*>> agentsListToExport = _getAgentsListToExport();

        // Export the agents list to selected file
        _modelManager->exportAgentsListToSelectedFile(agentsListToExport);
    }
}


/**
 * @brief Slot when a new model of agent has been created
 * @param agent
 */
void AgentsSupervisionController::onAgentModelCreated(AgentM* model)
{
    if (model != NULL)
    {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(model->name());

        // Create a new view model of agent
        AgentVM* agent = new AgentVM(model, this);

        // Connect to signals from this new view model of agent
        connect(agent, &AgentVM::definitionChangedWithPreviousValue, this, &AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousValue);
        connect(agent, &AgentVM::commandAskedToLauncher, this, &AgentsSupervisionController::commandAskedToLauncher);
        connect(agent, &AgentVM::commandAskedToAgent, this, &AgentsSupervisionController::commandAskedToAgent);
        connect(agent, &AgentVM::commandAskedToAgentAboutOutput, this, &AgentsSupervisionController::commandAskedToAgentAboutOutput);
        connect(agent, &AgentVM::openValuesHistoryOfAgent, this, &AgentsSupervisionController::openValuesHistoryOfAgent);

        agentViewModelsList.append(agent);
        _mapFromNameToAgentViewModelsList.insert(model->name(), agentViewModelsList);

        // Add our view model to the list
        _agentsList.append(agent);
    }
}


/**
 * @brief Slot when the definition of a view model of agent changed
 * @param previousDefinition
 * @param newDefinition
 */
void AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousValue(DefinitionM* previousDefinition, DefinitionM* newDefinition)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((agent != NULL) && (_modelManager != NULL)

            // Only if the previous definition was NULL and if the new definition is defined
            && (previousDefinition == NULL) && (newDefinition != NULL))
    {
        //
        // Update our list of agents with the new definition for this agent
        //

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

        AgentVM* agentUsingSameDefinition = NULL;
        DefinitionM* sameDefinition = NULL;

        foreach (AgentVM* iterator, agentViewModelsList)
        {
            // If this VM contains our model of agent
            if ((iterator != NULL) && (iterator != agent) && (iterator->definition() != NULL)
                    &&
                    // The 2 definitions are strictly identicals
                    DefinitionM::areIdenticals(iterator->definition(), newDefinition))
            {
                qDebug() << "There is exactly the same agent definition for name" << newDefinition->name() << "and version" << newDefinition->version();

                agentUsingSameDefinition = iterator;
                sameDefinition = iterator->definition();
                break;
            }
        }

        // Exactly the same definition
        if ((agentUsingSameDefinition != NULL) && (sameDefinition != NULL))
        {
            // It must have only one model of agent
            if (agent->models()->count() == 1) {
                AgentM* model = agent->models()->at(0);
                if (model != NULL)
                {
                    // The current view model for this model of agent is useless, we have to remove it from the list
                    _agentsList.remove(agent);

                    // Reset its definition
                    agent->setdefinition(NULL);

                    // Delete it
                    _deleteAgentViewModel(agent);
                    agent = NULL;

                    // 1- View model never yet appeared on the network
                    if (agentUsingSameDefinition->neverAppearedOnNetwork())
                    {
                        // It must have only one (fake) model of agent
                        if (agentUsingSameDefinition->models()->count() == 1)
                        {
                            AgentM* modelUsingSameDefinition = agentUsingSameDefinition->models()->at(0);
                            if (modelUsingSameDefinition != NULL)
                            {
                                // If the new model already appeared on the network
                                if (!model->neverAppearedOnNetwork())
                                {
                                    // Emit signal "Identical Agent Model Replaced"
                                    Q_EMIT identicalAgentModelReplaced(modelUsingSameDefinition, model);

                                    // We replace the fake model of agent
                                    agentUsingSameDefinition->models()->replace(0, model);

                                    qDebug() << "Replace model (which never appeared on network) by agent" << model->name() << "on" << model->address() << "(" << model->hostname() << ")";

                                    // Delete the previous (fake) model of agent
                                    _modelManager->deleteAgentModel(modelUsingSameDefinition);

                                    // Update the flag "Never Appeared on the Network"
                                    agentUsingSameDefinition->setneverAppearedOnNetwork(false);
                                }
                                else {
                                    // Delete this new (fake) model of agent
                                    _modelManager->deleteAgentModel(model);
                                }
                            }
                        }
                    }
                    // 2- View model is about a real agent, which already appeared on the network
                    else
                    {
                        //
                        // Then, replace (2.1) / add (2.2) our model to the view model having the same definition
                        //
                        bool isSameModel = false;
                        QList<AgentM*> models = agentUsingSameDefinition->models()->toList();
                        for (int i = 0; i < models.count(); i++)
                        {
                            AgentM* iterator = models.at(i);

                            // Same address and state is OFF --> we consider that it is the same model
                            if ((iterator != NULL) && (iterator->address() == model->address()) && !iterator->isON())
                            {
                                isSameModel = true;

                                // Emit signal "Identical Agent Model Replaced"
                                Q_EMIT identicalAgentModelReplaced(iterator, model);

                                // 2.1- We replace the model
                                agentUsingSameDefinition->models()->replace(i, model);

                                qDebug() << "Replace model by agent" << model->name() << "on" << model->address() << "(" << model->hostname() << ")";

                                // Delete the previous model of agent
                                _modelManager->deleteAgentModel(iterator);

                                break;
                            }
                        }

                        if (!isSameModel) {
                            // 2.2- Add the model of agent to the list of the VM
                            agentUsingSameDefinition->models()->append(model);

                            qDebug() << "Add model of agent" << model->name() << "on" << model->address();

                            // Emit signal "Identical Agent Model Added"
                            Q_EMIT identicalAgentModelAdded(model);
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Delete the view model of Agent
 * @param agent
 */
void AgentsSupervisionController::_deleteAgentViewModel(AgentVM* agent)
{
    if (agent != NULL)
    {
        // Unselect our agent if needed
        if (_selectedAgent == agent) {
            setselectedAgent(NULL);
        }

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());
        agentViewModelsList.removeOne(agent);

        // Update the list in the map
        _mapFromNameToAgentViewModelsList.insert(agent->name(), agentViewModelsList);

        // DIS-connect from signals from this old view model of agent
        disconnect(agent, &AgentVM::definitionChangedWithPreviousValue, this, &AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousValue);
        disconnect(agent, &AgentVM::commandAskedToLauncher, this, &AgentsSupervisionController::commandAskedToLauncher);
        disconnect(agent, &AgentVM::commandAskedToAgent, this, &AgentsSupervisionController::commandAskedToAgent);
        disconnect(agent, &AgentVM::commandAskedToAgentAboutOutput, this, &AgentsSupervisionController::commandAskedToAgentAboutOutput);
        disconnect(agent, &AgentVM::openValuesHistoryOfAgent, this, &AgentsSupervisionController::openValuesHistoryOfAgent);

        // Free memory
        delete agent;
    }
}


/**
 * @brief Get the agents list to export
 * @return List of pairs <agent name (and parameters to restart), definition>
 */
QList<QPair<QStringList, DefinitionM*>> AgentsSupervisionController::_getAgentsListToExport()
{
    // List of pairs <agent name (and parameters to restart), definition>
    QList<QPair<QStringList, DefinitionM*>> agentsListToExport;

    foreach (AgentVM* agent, _agentsList.toList())
    {
        if ((agent != NULL) && !agent->name().isEmpty() && (agent->definition() != NULL))
        {
            QStringList agentNameAndParametersToRestart;
            agentNameAndParametersToRestart.append(agent->name());

            if (agent->models()->count() > 0)
            {
                AgentM* firstModel = agent->models()->at(0);
                if ((firstModel != NULL) && !firstModel->hostname().isEmpty() && !firstModel->commandLine().isEmpty())
                {
                    agentNameAndParametersToRestart.append(firstModel->hostname());
                    agentNameAndParametersToRestart.append(firstModel->commandLine());
                }
            }

            QPair<QStringList, DefinitionM*> pair;
            pair.first = agentNameAndParametersToRestart;
            pair.second = agent->definition();

            agentsListToExport.append(pair);
        }
    }
    return agentsListToExport;
}
