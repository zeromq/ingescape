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
    _selectedAgent(NULL),
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

    // Clean-up current selection
    setselectedAgent(NULL);

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
 * @brief Delete an agent from the list
 * @param agent to delete
 */
void AgentsSupervisionController::deleteAgent(AgentVM* agent)
{
    if ((_modelManager != NULL) && (agent != NULL))
    {
        qDebug() << "Delete agent" << agent->name();

        // Remove it from the list
        _agentsList.remove(agent);

        // Save temporarily its definition
        DefinitionM* temp = agent->definition();

        // Reset it
        agent->setdefinition(NULL);

        // Delete the definition
        if (temp != NULL) {
            _modelManager->deleteAgentDefinition(temp);
        }

        // Delete each model of agent
        foreach (AgentM* model, agent->models()->toList()) {
            _modelManager->deleteAgentModel(model);
        }

        // Delete the view model of agent
        _deleteAgentViewModel(agent);
        agent = NULL;
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
            QList<DefinitionM*> agentDefinitionsList = _modelManager->getAgentDefinitionsListFromName(definition->name());

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
        QList<QPair<QString, DefinitionM*>> agentsListToExport = _getAgentsListToExport();

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
        QList<QPair<QString, DefinitionM*>> agentsListToExport = _getAgentsListToExport();

        // Export the agents list to selected file
        _modelManager->exportAgentsListToSelectedFile(agentsListToExport);
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

        // Propagate signals about "Command Asked"
        connect(agentVM, &AgentVM::commandAskedToLauncher, this, &AgentsSupervisionController::commandAskedToLauncher);
        connect(agentVM, &AgentVM::commandAsked, this, &AgentsSupervisionController::commandAsked);
        connect(agentVM, &AgentVM::commandAskedForOutput, this, &AgentsSupervisionController::commandAskedForOutput);

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
                // 1- The current view model for this new agent is useless, we have to remove it from the list
                _agentsList.remove(agentVM);

                // 2- Delete it
                _deleteAgentViewModel(agentVM);
                agentVM = NULL;

                // 3- The current definition for this new agent is useless, we have to delete it
                _modelManager->deleteAgentDefinition(definition);
                definition = NULL;

                // 4.1- View model has only definition (never yet appeared on the network)
                if (agentUsingSameDefinition->hasOnlyDefinition())
                {
                    // It must have only one (fake) model of agent
                    if (agentUsingSameDefinition->models()->count() == 1)
                    {
                        AgentM* model = agentUsingSameDefinition->models()->at(0);
                        if (model != NULL)
                        {
                            // If the new model has a peer id (not only definition)
                            if (model->peerId().isEmpty() && !agent->peerId().isEmpty())
                            {
                                // We replace the fake model of agent
                                agentUsingSameDefinition->models()->replace(0, agent);

                                qDebug() << "Replace model (which had only definition) by agent" << agentName << "on" << agent->address();

                                // Delete the previous (fake) model of agent
                                _modelManager->deleteAgentModel(model);
                                model = NULL;

                                // Update the flag "Has Only Definition"
                                agentUsingSameDefinition->sethasOnlyDefinition(false);
                            }
                            else {
                                // Delete this new (fake) model of agent
                                _modelManager->deleteAgentModel(agent);
                                agent = NULL;
                            }
                        }
                    }
                }
                // 4.2- View model is about a real agent, which is (or has been) on the network
                else
                {
                    //
                    // Then, replace (4.2.1) / add (4.2.2) our model to the view model having the same definition
                    //
                    bool isSameModel = false;
                    QList<AgentM*> models = agentUsingSameDefinition->models()->toList();
                    for (int i = 0; i < models.count(); i++)
                    {
                        AgentM* model = models.at(i);

                        // Same address and state is OFF --> we consider that it is the same model
                        if ((model != NULL) && (model->address() == agent->address()) && !model->isON())
                        {
                            isSameModel = true;

                            // 4.2.1- We replace the model
                            agentUsingSameDefinition->models()->replace(i, agent);

                            qDebug() << "Replace model by agent" << agentName << "on" << agent->address();

                            // Delete the previous model of agent
                            _modelManager->deleteAgentModel(model);
                            model = NULL;

                            break;
                        }
                    }

                    if (!isSameModel) {
                        // 4.2.2- Add the model of agent to the list of the VM
                        agentUsingSameDefinition->models()->append(agent);

                        qDebug() << "Add model of agent" << agentName << "on" << agent->address();
                    }
                }
            }
            // Definition is different
            else
            {
                // Set the definition
                agentVM->setdefinition(definition);

                // Emit the signal "Agent Definition Managed"
                Q_EMIT agentDefinitionManaged(agentName, definition);
            }
        }
    }
}


/**
 * @brief Slot when the flag "is Muted" from an output of agent updated
 * @param agent
 * @param isMuted
 * @param outputName
 */
void AgentsSupervisionController::onIsMutedFromOutputOfAgentUpdated(AgentM* agent, bool isMuted, QString outputName)
{
    if (agent != NULL) {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

        // Get the view model of agent that corresponds to our model
        foreach (AgentVM* agentVM, agentViewModelsList)
        {
            // If the view model has not yet a definition and contains our model of agent
            if ((agentVM != NULL) && agentVM->models()->contains(agent))
            {
                if (agentVM->definition() != NULL) {
                    agentVM->definition()->setisMutedOfOutput(isMuted, outputName);
                }
                break;
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
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());
        agentViewModelsList.removeOne(agent);

        // Update the list in the map
        _mapFromNameToAgentViewModelsList.insert(agent->name(), agentViewModelsList);

        // Stop propagation of signals "Command Asked (for output)"
        disconnect(agent, 0, this, 0);

        // Free memory
        delete agent;
    }
}


/**
 * @brief Get the agents list to export
 * @return List of pairs <agent name, definition>
 */
QList<QPair<QString, DefinitionM*>> AgentsSupervisionController::_getAgentsListToExport()
{
    // List of pairs <agent name, definition>
    QList<QPair<QString, DefinitionM*>> agentsListToExport;

    foreach (AgentVM* agent, _agentsList.toList())
    {
        if ((agent != NULL) && !agent->name().isEmpty() && (agent->definition() != NULL))
        {
            QPair<QString, DefinitionM*> pair;
            pair.first = agent->name();
            pair.second = agent->definition();

            agentsListToExport.append(pair);
        }
    }
    return agentsListToExport;
}
