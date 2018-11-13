/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "agentsgroupedbynamevm.h"

/**
 * @brief Constructor
 * @param agentName
 * @param parent
 */
AgentsGroupedByNameVM::AgentsGroupedByNameVM(QString agentName,
                                             QObject *parent) : QObject(parent),
    _name(agentName),
    _isON(false),
    _peerIdsList(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New View Model of Agents grouped by name" << _name;

    // Connect to signal "Count Changed" from the list of models
    connect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByNameVM::_onModelsChanged);
}


/**
 * @brief Destructor
 */
AgentsGroupedByNameVM::~AgentsGroupedByNameVM()
{
    qInfo() << "Delete View Model of Agents grouped by name" << _name;

    // DEBUG
    _listOfGroupsByDefinition.clear();

    // Delete all view models of agents grouped by definition
    if (_agentsGroupedByDefinitionNULL != nullptr)
    {
        disconnect(_agentsGroupedByDefinitionNULL, 0, this, 0);

        delete _agentsGroupedByDefinitionNULL;
        _agentsGroupedByDefinitionNULL = nullptr;
    }

    for (AgentsGroupedByDefinitionVM* agentsGroupedByDefinition : _hashFromDefinitionToAgentsGroupedByDefinition.values())
    {
        disconnect(agentsGroupedByDefinition, 0, this, 0);

        delete agentsGroupedByDefinition;
    }
    _hashFromDefinitionToAgentsGroupedByDefinition.clear();


    disconnect(&_models, &AbstractI2CustomItemListModel::countChanged, this, &AgentsGroupedByNameVM::_onModelsChanged);

    // Clear the previous list of models
    _previousAgentsList.clear();

    // Free the memory elsewhere
    //_models.deleteAllItems();
    _models.clear();

    //_hashFromHostnameToModels.clear();
}


/**
 * @brief Manage a new model of agent
 * @param model
 */
void AgentsGroupedByNameVM::manageNewModel(AgentM* model)
{
    if (model != nullptr)
    {
        qDebug() << "Grouped by" << _name << "manage" << model << "entered";

        // FIXME: depends of cases (definition, ...)
        _models.append(model);

        if (model->definition() == nullptr)
        {
            if (_agentsGroupedByDefinitionNULL == nullptr)
            {
                // Create the special view model of agents grouped by definition NULL
                _agentsGroupedByDefinitionNULL = new AgentsGroupedByDefinitionVM(model, nullptr);

                // Save this new view model of agents grouped by definition
                _saveNewAgentsGroupedByDefinition(_agentsGroupedByDefinitionNULL);
            }
            else
            {
                // Add the model to the special view model of agents grouped by definition NULL
                _agentsGroupedByDefinitionNULL->models()->append(model);
            }
        }
        else
        {
            // FIXME TODO: Manage a new model of agent that already have a definition...
            qDebug() << "Manage a new model of agent that already have a definition...TODO";
        }
    }
}


/**
 * @brief Update the current value of an I/O/P of our agent(s)
 * @param publishedValue
 */
void AgentsGroupedByNameVM::updateCurrentValueOfIOP(PublishedValueM* publishedValue)
{
    if (publishedValue != NULL)
    {
        for (AgentM* agent : _models)
        {
            if ((agent != NULL) && (agent->definition() != NULL))
            {
                switch (publishedValue->iopType())
                {
                case AgentIOPTypes::OUTPUT: {
                    OutputM* output = agent->definition()->getOutputWithName(publishedValue->iopName());
                    if (output != NULL) {
                        output->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                case AgentIOPTypes::INPUT: {
                    AgentIOPM* input = agent->definition()->getInputWithName(publishedValue->iopName());
                    if (input != NULL) {
                        input->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                case AgentIOPTypes::PARAMETER: {
                    AgentIOPM* parameter = agent->definition()->getParameterWithName(publishedValue->iopName());
                    if (parameter != NULL) {
                        parameter->setcurrentValue(publishedValue->value());
                    }
                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}


/**
 * @brief Delete the view model of agents grouped by definition
 * @param agentsGroupedByDefinition
 */
void AgentsGroupedByNameVM::deleteAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != NULL)
    {
        // Emit the signal "Agents grouped by definition will be deleted"
        Q_EMIT agentsGroupedByDefinitionWillBeDeleted(agentsGroupedByDefinition);

        // DIS-connect to signals from this view model of agents grouped by definition
        disconnect(_agentsGroupedByDefinitionNULL, 0, this, 0);

        // DEBUG
        _listOfGroupsByDefinition.remove(agentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr) {
            // Remove from the hash table
            _hashFromDefinitionToAgentsGroupedByDefinition.remove(agentsGroupedByDefinition->definition());
        }
        // The definition is NULL
        else if (agentsGroupedByDefinition == _agentsGroupedByDefinitionNULL) {
            _agentsGroupedByDefinitionNULL = nullptr;
        }

        // Make a copy of the list of models
        QList<AgentM*> copy = agentsGroupedByDefinition->models()->toList();

        // Free memory
        delete agentsGroupedByDefinition;

        // Delete each model of agent
        for (AgentM* model : copy)
        {
            if (model != nullptr) {
                // Emit the signal to delete this model of agent
                Q_EMIT agentModelHasToBeDeleted(model);
            }
        }
    }
}


/**
 * @brief Get the list of definitions with a specific name
 * @param definitionName
 * @return
 */
QList<DefinitionM*> AgentsGroupedByNameVM::getDefinitionsWithName(QString definitionName)
{
    QList<DefinitionM*> definitionsList;

    for (DefinitionM* definition : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
    {
        if ((definition != nullptr) && (definition->name() == definitionName))
        {
            definitionsList.append(definition);
        }
    }
    return definitionsList;
}


/**
 * @brief Slot called when the list of models changed
 */
void AgentsGroupedByNameVM::_onModelsChanged()
{
    QList<AgentM*> newAgentsList = _models.toList();

    // Model of agent added
    if (_previousAgentsList.count() < newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> ADD --> " << newAgentsList.count();

        for (AgentM* model : newAgentsList)
        {
            if ((model != NULL) && !_previousAgentsList.contains(model))
            {
                //qDebug() << "New model" << model->name() << "ADDED (" << model->peerId() << ")";

                // Connect to signals of the model
                //connect(model, &AgentM::hostnameChanged, this, &AgentsGroupedByNameVM::_onHostnameOfModelChanged);
                connect(model, &AgentM::isONChanged, this, &AgentsGroupedByNameVM::_onIsONofModelChanged);
                connect(model, &AgentM::definitionChangedWithPreviousAndNewValues, this, &AgentsGroupedByNameVM::_onDefinitionOfModelChangedWithPreviousAndNewValues);

            }
        }
    }
    // Model of agent removed
    else if (_previousAgentsList.count() > newAgentsList.count())
    {
        //qDebug() << _previousAgentsList.count() << "--> REMOVE --> " << newAgentsList.count();

        for (AgentM* model : _previousAgentsList)
        {
            if ((model != NULL) && !newAgentsList.contains(model))
            {
                //qDebug() << "Old model" << model->name() << "REMOVED (" << model->peerId() << ")";

                // DIS-connect from signals of the model
                disconnect(model, 0, this, 0);
            }
        }
    }

    _previousAgentsList = newAgentsList;

    // Update with all models
    _updateWithAllModels();
}


/**
 * @brief Slot called when the flag "is ON" of a model changed
 * @param isON
 */
void AgentsGroupedByNameVM::_onIsONofModelChanged(bool isON)
{
    Q_UNUSED(isON)

    // Update the flag "is ON" in function of flags of all models
    _updateIsON();
}


/**
 * @brief Slot called when the definition of a model changed (with previous and new values)
 * @param previousDefinition
 * @param newDefinition
 */
void AgentsGroupedByNameVM::_onDefinitionOfModelChangedWithPreviousAndNewValues(DefinitionM* previousDefinition, DefinitionM* newDefinition)
{
    AgentM* model = qobject_cast<AgentM*>(sender());
    if ((model != nullptr) && (newDefinition != nullptr))
    {
        // The previous definition was NULL (and the new definition is defined)
        if (previousDefinition == nullptr)
        {
            // Remove the model of agent from the list of the special view model of agents grouped by definition NULL
            if ((_agentsGroupedByDefinitionNULL != nullptr) && _agentsGroupedByDefinitionNULL->models()->contains(model)) {
                _agentsGroupedByDefinitionNULL->models()->remove(model);
            }

            AgentsGroupedByDefinitionVM* groupOfAgentsWithSameDefinition = nullptr;

            // Traverse the list of each definition
            for (DefinitionM* iterator : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
            {
                // The 2 definitions are strictly identicals
                if ((iterator != nullptr) && DefinitionM::areIdenticals(iterator, newDefinition))
                {
                    qDebug() << "There is exactly the same agent definition for name" << newDefinition->name() << "and version" << newDefinition->version();

                    groupOfAgentsWithSameDefinition = _hashFromDefinitionToAgentsGroupedByDefinition.value(iterator);
                    break;
                }
            }

            // Exactly the same definition
            if (groupOfAgentsWithSameDefinition != nullptr)
            {
                // Manage the new agent model
                QString hostname = model->hostname();

                // Hostname is not defined
                // There is already an existing model of agent (in the VM groupOfAgentsWithSameDefinition)
                if (hostname == HOSTNAME_NOT_DEFINED)
                {
                    qDebug() << "Useless Model of agent" << _name << "on" << hostname;

                    // Emit the signal to delete this new (fake) model of agent
                    Q_EMIT agentModelHasToBeDeleted(model);
                }
                // Hostname is a real one
                else
                {
                    // Get the list of agent models on the host
                    QList<AgentM*> modelsOnHost = groupOfAgentsWithSameDefinition->getModelsOnHost(hostname);

                    // There is NO agent on this host yet
                    if (modelsOnHost.isEmpty())
                    {
                        qDebug() << "Add model of agent" << _name << "on" << hostname;

                        // Add the model of agent to the view model of agents grouped by definition
                        groupOfAgentsWithSameDefinition->models()->append(model);
                    }
                    // There is already some agent models on this host
                    else
                    {
                        // Peer id is empty (the agent has never appeared on the network)
                        if (model->peerId().isEmpty())
                        {
                            qDebug() << "Add model of agent" << _name << "on" << hostname;

                            // Add the model of agent to the view model of agents grouped by definition
                            groupOfAgentsWithSameDefinition->models()->append(model);
                        }
                        // Peer id is defined: check if it is an agent that evolve from OFF to ON
                        else
                        {
                            bool hasToDeleteNewModel = false;
                            AgentM* sameModel = NULL;

                            QString peerId = model->peerId();
                            QString commandLine = model->commandLine();

                            // Search a model already added with the same peer id...
                            for (AgentM* iterator : modelsOnHost)
                            {
                                // Same peer id
                                if ((iterator != NULL) && !iterator->peerId().isEmpty() && (iterator->peerId() == peerId))
                                {
                                    // New model is OFF and there is already a model with the same peer id...
                                    if (!model->isON())
                                    {
                                        // the new model is useless, we have to delete it
                                        hasToDeleteNewModel = true;
                                        break;
                                    }
                                    // New model is ON and there is already a model with the same peer id...
                                    else
                                    {
                                        // The model already added is OFF, we have to replace it by the new one
                                        if (!iterator->isON())
                                        {
                                            sameModel = iterator;
                                            break;
                                        }
                                    }
                                }
                            }

                            // We don't found this peer id
                            if (!hasToDeleteNewModel && (sameModel == NULL))
                            {
                                // New model is ON
                                if (model->isON())
                                {
                                    for (AgentM* iterator : modelsOnHost)
                                    {
                                        // Same command line (peer id is defined) and existing agent is OFF --> we consider that it is the same model that evolve from OFF to ON
                                        if ((iterator != NULL) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && !iterator->isON())
                                        {
                                            // We have to replace it by the new one
                                            sameModel = iterator;
                                            break;
                                        }
                                    }
                                }
                                // New model is OFF
                                else
                                {
                                    for (AgentM* iterator : modelsOnHost)
                                    {
                                        // Same command line (peer id is defined) and existing agent is ON --> we consider that it is the same model but OFF
                                        if ((iterator != NULL) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && iterator->isON())
                                        {
                                            // The new model is useless, we have to delete it
                                            hasToDeleteNewModel = true;
                                            break;
                                        }
                                    }
                                }
                            }

                            // We have to remove the new model
                            if (hasToDeleteNewModel)
                            {
                                qDebug() << "Useless Model of agent" << _name << "on" << hostname;

                                // Emit the signal to delete this new (fake) model of agent
                                Q_EMIT agentModelHasToBeDeleted(model);
                            }
                            // Else if we have to replace an existing (same) model by the new one
                            else if (sameModel != NULL)
                            {
                                int index = groupOfAgentsWithSameDefinition->models()->indexOf(sameModel);
                                if (index > -1)
                                {
                                    // Emit the signal "Identical Agent Model will be Replaced"
                                    Q_EMIT identicalAgentModelWillBeReplaced(sameModel, model);

                                    qDebug() << "Replace model of agent" << _name << "on" << hostname << "(" << sameModel->peerId() << "-->" << model->peerId() << ")";

                                    // Replace the model
                                    groupOfAgentsWithSameDefinition->models()->replace(index, model);

                                    // Emit the signal to delete the previous model of agent
                                    Q_EMIT agentModelHasToBeDeleted(sameModel);
                                }
                            }
                            // Else, we add the new model
                            else
                            {
                                qDebug() << "Add model of agent" << _name << "on" << hostname;

                                // Add the model of agent to the view model of agents grouped by definition
                                groupOfAgentsWithSameDefinition->models()->append(model);
                            }
                        }
                    }
                    qDebug() << "There are" << groupOfAgentsWithSameDefinition->models()->count() << "models of agent" << _name << "with the same definition";
                }
            }
            // The definition is a new one
            else
            {
                // Make a copy of the definition
                DefinitionM* copy = newDefinition->copy();

                // Create a new view model of agents grouped by definition
                AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = new AgentsGroupedByDefinitionVM(model, copy);

                // Save this new view model of agents grouped by definition
                _saveNewAgentsGroupedByDefinition(agentsGroupedByDefinition);
            }
        }
        // The previous definition was already defined (and the new definition is defined)
        else
        {
            // Search the current agents grouped (by definition) with the previous definition
            for (DefinitionM* iterator : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
            {
                // The 2 definitions are strictly identicals
                if ((iterator != nullptr) && DefinitionM::areIdenticals(iterator, previousDefinition))
                {
                    AgentsGroupedByDefinitionVM* currentGroup = _hashFromDefinitionToAgentsGroupedByDefinition.value(iterator);
                    if (currentGroup != nullptr) {
                        // Remove the model from the current group
                        currentGroup->models()->remove(model);
                    }
                    break;
                }
            }

            // Check if we have to merge this model of agent with an existing view model of agents (grouped by definition) that have the same definition
            _checkHaveToMergeAgent(model);
        }
    }
}


/**
 * @brief Slot called when a view model of agents grouped by definition has become useless (no more model)
 */
void AgentsGroupedByNameVM::_onUselessAgentsGroupedByDefinition()
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if (agentsGroupedByDefinition != nullptr) {
        // Delete the view model of agents grouped by definition
        deleteAgentsGroupedByDefinition(agentsGroupedByDefinition);
    }
}


/**
 * @brief Update with all models of agents
 */
void AgentsGroupedByNameVM::_updateWithAllModels()
{
    qDebug() << "Grouped by" << _name << ": Update with all (" << _models.count() << ") models (" << this << ")";

    // Update flags in function of models
    _updateIsON();
}


/**
 * @brief Update the flag "is ON" in function of flags of models
 */
void AgentsGroupedByNameVM::_updateIsON()
{
    bool globalIsON = false;

    for (AgentM* model : _models.toList())
    {
        if ((model != NULL) && model->isON())
        {
            globalIsON = true;
            break;
        }
    }

    setisON(globalIsON);
}


/**
 * @brief Check if we have to merge the model of agent with an existing view model of agents (grouped by definition) that have the same definition
 * @param model
 */
void AgentsGroupedByNameVM::_checkHaveToMergeAgent(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        DefinitionM* newDefinition = model->definition();

        AgentsGroupedByDefinitionVM* groupOfAgentsWithSameDefinition = nullptr;

        // Traverse the list of each definition
        for (DefinitionM* iterator : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
        {
            // The 2 definitions are strictly identicals
            if ((iterator != nullptr) && DefinitionM::areIdenticals(iterator, newDefinition))
            {
                qDebug() << "There is exactly the same agent definition for name" << newDefinition->name() << "and version" << newDefinition->version();

                groupOfAgentsWithSameDefinition = _hashFromDefinitionToAgentsGroupedByDefinition.value(iterator);
                break;
            }
        }

        // Exactly the same definition
        if (groupOfAgentsWithSameDefinition != nullptr)
        {
            // FIXME TODO: Manage all cases (this model replace an existing one with state OFF)

            // Manage the new agent model
            //QString hostname = model->hostname();

            //qDebug() << "Add model of agent" << _name << "on" << hostname;

            // Add the model of agent to the view model of agents grouped by definition
            groupOfAgentsWithSameDefinition->models()->append(model);
        }
        else
        {
            // Make a copy of the definition
            DefinitionM* copy = newDefinition->copy();

            // Create a new view model of agents grouped by definition
            AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = new AgentsGroupedByDefinitionVM(model, copy);

            // Save this new view model of agents grouped by definition
            _saveNewAgentsGroupedByDefinition(agentsGroupedByDefinition);
        }
    }
}


/**
 * @brief Save a new view model of agents grouped by definition
 * @param agentsGroupedByDefinition
 */
void AgentsGroupedByNameVM::_saveNewAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Connect to signals from this view model of agents grouped by definition
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::noMoreModelAndUseless, this, &AgentsGroupedByNameVM::_onUselessAgentsGroupedByDefinition);

        // Add to the hash table from definition
        if (agentsGroupedByDefinition->definition() != nullptr) {
            _hashFromDefinitionToAgentsGroupedByDefinition.insert(agentsGroupedByDefinition->definition(), agentsGroupedByDefinition);
        }

        // Emit the signal "Agents grouped by definition has been created"
        Q_EMIT agentsGroupedByDefinitionHasBeenCreated(agentsGroupedByDefinition);

        // DEBUG
        _listOfGroupsByDefinition.append(agentsGroupedByDefinition);
    }
}
