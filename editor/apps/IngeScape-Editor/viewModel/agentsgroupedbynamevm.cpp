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
    _peerIdsList(QStringList()),
    _isON(false),
    _numberOfAgentsON(0),
    _numberOfAgentsOFF(0),
    _canBeDeleted_whenListOfAllAgentsGroupsByDefinition_isEmpty(true),
    _agentsGroupedByDefinitionNULL(nullptr)

{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New View Model of Agents grouped by name" << _name;

}


/**
 * @brief Destructor
 */
AgentsGroupedByNameVM::~AgentsGroupedByNameVM()
{
    qInfo() << "Delete View Model of Agents grouped by name" << _name;

    // Set the flag to prevent the deletion of our "agents grouped by name" because the list "allAgentsGroupsByDefinition"
    // will be empty during the execution of this destructor (prevent several call to this destructor)
    _canBeDeleted_whenListOfAllAgentsGroupsByDefinition_isEmpty = false;

    // Models are managed by agents grouped by definition (do not manage them here)
    /*if (!_models.isEmpty())
    {
        QList<AgentM*> copy = _models.toList();
        for (AgentM* model : copy)
        {
            if (model != nullptr) {
                Q_EMIT agentModelHasToBeDeleted(model);
            }
        }
    }*/

    // Clear hash tables of Inputs, Outputs and Parameters
    _hashFromNameToInputsList.clear();
    _hashFromIdToInput.clear();
    _hashFromNameToOutputsList.clear();
    _hashFromIdToOutput.clear();
    _hashFromNameToParametersList.clear();
    _hashFromIdToParameter.clear();

    // Delete all view models of Inputs, Outputs and Parameters
    _inputsList.deleteAllItems();
    _outputsList.deleteAllItems();
    _parametersList.deleteAllItems();


    // If the list of groups of agent(s grouped by definition) is not empty
    if (!_allAgentsGroupsByDefinition.isEmpty())
    {
        // Delete all view models of agents grouped by definition
        for (AgentsGroupedByDefinitionVM* agentsGroupedByDefinition : _allAgentsGroupsByDefinition.toList())
        {
            // Delete the view model of agents grouped by definition
            // And emit the signal "agentModelHasToBeDeleted" for each of its model of agent
            deleteAgentsGroupedByDefinition(agentsGroupedByDefinition);
        }

        //_agentsGroupedByDefinitionNULL = nullptr;
        //_hashFromDefinitionToAgentsGroupedByDefinition.clear();
        //_allAgentsGroupsByDefinition.clear();
    }


    // All models have already been deleted (the signal "agentModelHasToBeDeleted" is emitted for each of them
    // in the method "AgentsGroupedByNameVM::deleteAgentsGroupedByDefinition" just before)
    //_models.deleteAllItems();
    _models.clear();
}


/**
 * @brief Add a new model of agent
 * @param model
 */
void AgentsGroupedByNameVM::addNewAgentModel(AgentM* model)
{
    if (model != nullptr)
    {
        qDebug() << "Grouped by" << _name << ": Add new model" << model;

        if (!_models.contains(model))
        {
            // Connect to signals from the model
            connect(model, &AgentM::isONChanged, this, &AgentsGroupedByNameVM::_onIsONofModelChanged);
            connect(model, &AgentM::definitionChangedWithPreviousAndNewValues, this, &AgentsGroupedByNameVM::_onDefinitionOfModelChangedWithPreviousAndNewValues);

            _models.append(model);

            // Update with all models
            _updateWithAllModels();
        }

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
            // FIXME TODO ? Manage a new model of agent that already have a definition. Possible ?
            qWarning() << "Manage a new model of agent that already have a definition...TODO";

            //Q_EMIT model->definitionChangedWithPreviousAndNewValues(nullptr, model->definition());

            _toRENAME(model);
        }
    }
}


/**
 * @brief Remove an old model of agent
 * @param model
 */
void AgentsGroupedByNameVM::removeOldAgentModel(AgentM* model)
{
    if (model != nullptr)
    {
        qDebug() << "Grouped by" << _name << ": Remove old model" << model;

        if (_models.contains(model))
        {
            // DIS-connect to signals from the model
            disconnect(model, 0, this, 0);

            // Remove from our list
            _models.remove(model);
        }

        if (_models.isEmpty())
        {
            // There is no more model, our VM is useless
            //Q_EMIT noMoreModelAndUseless();
        }
        else {
            // Update with all models
            _updateWithAllModels();
        }
    }
}


/**
 * @brief Update the current value of an I/O/P of our agent(s)
 * @param publishedValue
 */
void AgentsGroupedByNameVM::updateCurrentValueOfIOP(PublishedValueM* publishedValue)
{
    if (publishedValue != nullptr)
    {
        AgentIOPTypes::Value iopType = publishedValue->iopType();
        QString iopName = publishedValue->iopName();
        QVariant currentValue = publishedValue->value();

        // FIXME: Is it usefull to store values in the definition of each model ?
        for (AgentM* agent : _models)
        {
            if ((agent != nullptr) && (agent->definition() != nullptr))
            {
                // Update the current value of an I/O/P of the agent definition
                agent->definition()->updateCurrentValueOfIOP(iopType, iopName, currentValue);
            }
        }

        // Traverse the list of definitions (groups of agents grouped by definition)
        for (DefinitionM* definition : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
        {
            if (definition != nullptr)
            {
                // Update the current value of an I/O/P of the agents grouped by definition
                definition->updateCurrentValueOfIOP(iopType, iopName, currentValue);
            }
        }
    }
}


/**
 * @brief Delete the view model of agents grouped by definition
 * And emit the signal "agentModelHasToBeDeleted" for each of its model of agent
 * @param agentsGroupedByDefinition
 */
void AgentsGroupedByNameVM::deleteAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Emit the signal "Agents grouped by definition will be deleted"
        Q_EMIT agentsGroupedByDefinitionWillBeDeleted(agentsGroupedByDefinition);

        // DIS-connect to signals from this view model of agents grouped by definition
        disconnect(agentsGroupedByDefinition, 0, this, 0);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();

            // Remove from the hash table
            _hashFromDefinitionToAgentsGroupedByDefinition.remove(definition);

            // Manage the list of Inputs / Outputs / Parameters of the removed definition
            _manageInputsOfRemovedDefinition(definition);
            _manageOutputsOfRemovedDefinition(definition);
            _manageParametersOfRemovedDefinition(definition);

            // Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
            _updateIsDefinedInAllDefinitionsForEachIOP(_hashFromDefinitionToAgentsGroupedByDefinition.count());

            // Emit signal "models of Inputs/Outputs/Parameters Changed"
            //Q_EMIT modelsOfIOPChanged();
        }
        // The definition is NULL
        else if (agentsGroupedByDefinition == _agentsGroupedByDefinitionNULL)
        {
            _agentsGroupedByDefinitionNULL = nullptr;
        }

        // Remove from the list
        _allAgentsGroupsByDefinition.remove(agentsGroupedByDefinition);

        // Make a copy of the list of models
        QList<AgentM*> copy = agentsGroupedByDefinition->models()->toList();

        // Free memory
        delete agentsGroupedByDefinition;

        if (!copy.isEmpty())
        {
            // Delete each model of agent
            for (AgentM* model : copy)
            {
                if (model != nullptr) {
                    // Emit the signal to delete this model of agent
                    Q_EMIT agentModelHasToBeDeleted(model);
                }
            }
        }

        // Check the flag indicating if our "agents grouped by name" can be deleted when its list of all agents grouped by definition is empty
        // The list "allAgentsGroupsByDefinition" can be TEMPORARY empty during the execution of "_onDefinitionOfModelChangedWithPreviousAndNewValues"
        if (_allAgentsGroupsByDefinition.isEmpty() && _canBeDeleted_whenListOfAllAgentsGroupsByDefinition_isEmpty)
        {
            // Emit the signal "No more Agents Grouped by Definition" (our view model if useless)
            Q_EMIT noMoreAgentsGroupedByDefinitionAndUseless();
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
 * @brief Open the definition(s)
 * If there are several "Agents Grouped by Definition", we have to open each definition
 */
void AgentsGroupedByNameVM::openDefinition()
{
    QList<DefinitionM*> allDefinitions = _hashFromDefinitionToAgentsGroupedByDefinition.keys();

    Q_EMIT definitionsToOpen(allDefinitions);
}


/**
 * @brief Return the list of view models of input from an input name
 * @param inputName
 */
QList<InputVM*> AgentsGroupedByNameVM::getInputsListFromName(QString inputName)
{
    if (_hashFromNameToInputsList.contains(inputName)) {
        return _hashFromNameToInputsList.value(inputName);
    }
    else {
        return QList<InputVM*>();
    }
}


/**
 * @brief Return the view model of input from an input id
 * @param inputId
 */
InputVM* AgentsGroupedByNameVM::getInputFromId(QString inputId)
{
    if (_hashFromIdToInput.contains(inputId)) {
        return _hashFromIdToInput.value(inputId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Return the list of view models of output from an output name
 * @param outputName
 */
QList<OutputVM*> AgentsGroupedByNameVM::getOutputsListFromName(QString outputName)
{
    if (_hashFromNameToOutputsList.contains(outputName)) {
        return _hashFromNameToOutputsList.value(outputName);
    }
    else {
        return QList<OutputVM*>();
    }
}


/**
 * @brief Return the view model of output from an output id
 * @param outputId
 */
OutputVM* AgentsGroupedByNameVM::getOutputFromId(QString outputId)
{
    if (_hashFromIdToOutput.contains(outputId)) {
        return _hashFromIdToOutput.value(outputId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Return the list of view models of parameter from a parameter name
 * @param parameterName
 */
QList<ParameterVM*> AgentsGroupedByNameVM::getParametersListFromName(QString parameterName)
{
    if (_hashFromNameToParametersList.contains(parameterName)) {
        return _hashFromNameToParametersList.value(parameterName);
    }
    else {
        return QList<ParameterVM*>();
    }
}


/**
 * @brief Return the view model of parameter from a parameter id
 * @param parameterId
 */
ParameterVM* AgentsGroupedByNameVM::getParameterFromId(QString parameterId)
{
    if (_hashFromIdToParameter.contains(parameterId)) {
        return _hashFromIdToParameter.value(parameterId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Slot called when the flag "is ON" of a model changed
 * @param isON
 */
void AgentsGroupedByNameVM::_onIsONofModelChanged(bool isON)
{
    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        setisON(isON);

        if (isON) {
            setnumberOfAgentsON(1);
            setnumberOfAgentsOFF(0);
        }
        else {
            setnumberOfAgentsON(0);
            setnumberOfAgentsOFF(1);
        }
    }
    // Several models
    else
    {
        bool globalIsON = false;
        int numberOfAgentsON = 0;
        int numberOfAgentsOFF = 0;

        for (AgentM* model : _models.toList())
        {
            if (model != nullptr)
            {
                if (model->isON())
                {
                    globalIsON = true;

                    numberOfAgentsON++;
                }
                else {
                    numberOfAgentsOFF++;
                }
            }
        }

        setisON(globalIsON);
        setnumberOfAgentsON(numberOfAgentsON);
        setnumberOfAgentsOFF(numberOfAgentsOFF);
    }
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
            // Set the flag to prevent the deletion of our "agents grouped by name" because the list "allAgentsGroupsByDefinition"
            // will be TEMPORARY empty during the execution of "_onDefinitionOfModelChangedWithPreviousAndNewValues"
            _canBeDeleted_whenListOfAllAgentsGroupsByDefinition_isEmpty = false;

            if ((_agentsGroupedByDefinitionNULL != nullptr) && _agentsGroupedByDefinitionNULL->models()->contains(model))
            {
                // Remove the model of agent from the list of the special view model of agents grouped by definition NULL
                _agentsGroupedByDefinitionNULL->models()->remove(model);
            }

            _toRENAME(model);

            /*AgentsGroupedByDefinitionVM* groupOfAgentsWithSameDefinition = nullptr;

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

                        // Emit the signal to notify that the new model has been added
                        Q_EMIT agentModelHasBeenAdded(model);
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

                            // Emit the signal to notify that the new model has been added
                            Q_EMIT agentModelHasBeenAdded(model);
                        }
                        // Peer id is defined: check if it is an agent that evolve from OFF to ON
                        else
                        {
                            bool hasToDeleteNewModel = false;
                            AgentM* sameModel = nullptr;

                            QString peerId = model->peerId();
                            QString commandLine = model->commandLine();

                            // Search a model already added with the same peer id...
                            for (AgentM* iterator : modelsOnHost)
                            {
                                // Same peer id
                                if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->peerId() == peerId))
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
                            if (!hasToDeleteNewModel && (sameModel == nullptr))
                            {
                                // New model is ON
                                if (model->isON())
                                {
                                    for (AgentM* iterator : modelsOnHost)
                                    {
                                        // Same command line (peer id is defined) and existing agent is OFF --> we consider that it is the same model that evolve from OFF to ON
                                        if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && !iterator->isON())
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
                                        if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && iterator->isON())
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
                            else if (sameModel != nullptr)
                            {
                                if (groupOfAgentsWithSameDefinition->models()->contains(sameModel))
                                {
                                    qDebug() << "Replace model of agent" << _name << "on" << hostname << "(" << sameModel->peerId() << "-->" << model->peerId() << ")";

                                    // First add the new model before remove the previous model
                                    // (allows to prevent to have 0 model at a given moment and to prevent to emit signal noMoreModelAndUseless that remove the groupOfAgentsWithSameDefinition)
                                    groupOfAgentsWithSameDefinition->models()->append(model);

                                    // Emit the signal to notify that the new model has been added
                                    Q_EMIT agentModelHasBeenAdded(model);

                                    groupOfAgentsWithSameDefinition->models()->remove(sameModel);

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

                                // Emit the signal to notify that the new model has been added
                                Q_EMIT agentModelHasBeenAdded(model);
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
            }*/

            // Reset the flag to allow the deletion of our "agents grouped by name" when the list "allAgentsGroupsByDefinition" is empty
            _canBeDeleted_whenListOfAllAgentsGroupsByDefinition_isEmpty = true;
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

    _peerIdsList.clear();

    bool globalIsON = false;
    int numberOfAgentsON = 0;
    int numberOfAgentsOFF = 0;

    // Most of the time, there is only one model
    if (_models.count() == 1)
    {
        AgentM* model = _models.at(0);
        if (model != nullptr)
        {
            if (!model->peerId().isEmpty()) {
                _peerIdsList = QStringList(model->peerId());
            }

            globalIsON = model->isON();

            if (model->isON()) {
                numberOfAgentsON = 1;
                numberOfAgentsOFF = 0;
            }
            else {
                numberOfAgentsON = 0;
                numberOfAgentsOFF = 1;
            }
        }
    }
    // Several models
    else
    {
        // Update with all models of agents
        for (AgentM* model : _models.toList())
        {
            if (model != nullptr)
            {
                if (!model->peerId().isEmpty()) {
                    _peerIdsList.append(model->peerId());
                }

                if (model->isON())
                {
                    //if (!globalIsON) {
                    globalIsON = true;
                    //}

                    numberOfAgentsON++;
                }
                else {
                    numberOfAgentsOFF++;
                }
            }
        }
    }


    //
    // Update properties
    //
    setisON(globalIsON);
    setnumberOfAgentsON(numberOfAgentsON);
    setnumberOfAgentsOFF(numberOfAgentsOFF);
}



void AgentsGroupedByNameVM::_toRENAME(AgentM* model)
{
    if ((model != nullptr) && (model->definition() != nullptr))
    {
        DefinitionM* definition = model->definition();

        AgentsGroupedByDefinitionVM* groupOfAgentsWithSameDefinition = nullptr;

        // Traverse the list of each definition
        for (DefinitionM* iterator : _hashFromDefinitionToAgentsGroupedByDefinition.keys())
        {
            // The 2 definitions are strictly identicals
            if ((iterator != nullptr) && DefinitionM::areIdenticals(iterator, definition))
            {
                qDebug() << "There is exactly the same agent definition for name" << definition->name() << "and version" << definition->version();

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

                    // Emit the signal to notify that the new model has been added
                    Q_EMIT agentModelHasBeenAdded(model);
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

                        // Emit the signal to notify that the new model has been added
                        Q_EMIT agentModelHasBeenAdded(model);
                    }
                    // Peer id is defined: check if it is an agent that evolve from OFF to ON
                    else
                    {
                        bool hasToDeleteNewModel = false;
                        AgentM* sameModel = nullptr;

                        QString peerId = model->peerId();
                        QString commandLine = model->commandLine();

                        // Search a model already added with the same peer id...
                        for (AgentM* iterator : modelsOnHost)
                        {
                            // Same peer id
                            if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->peerId() == peerId))
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
                        if (!hasToDeleteNewModel && (sameModel == nullptr))
                        {
                            // New model is ON
                            if (model->isON())
                            {
                                for (AgentM* iterator : modelsOnHost)
                                {
                                    // Same command line (peer id is defined) and existing agent is OFF --> we consider that it is the same model that evolve from OFF to ON
                                    if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && !iterator->isON())
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
                                    if ((iterator != nullptr) && !iterator->peerId().isEmpty() && (iterator->commandLine() == commandLine) && iterator->isON())
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
                        else if (sameModel != nullptr)
                        {
                            if (groupOfAgentsWithSameDefinition->models()->contains(sameModel))
                            {
                                qDebug() << "Replace model of agent" << _name << "on" << hostname << "(" << sameModel->peerId() << "-->" << model->peerId() << ")";

                                // First add the new model before remove the previous model
                                // (allows to prevent to have 0 model at a given moment and to prevent to emit signal noMoreModelAndUseless that remove the groupOfAgentsWithSameDefinition)
                                groupOfAgentsWithSameDefinition->models()->append(model);

                                // Emit the signal to notify that the new model has been added
                                Q_EMIT agentModelHasBeenAdded(model);

                                groupOfAgentsWithSameDefinition->models()->remove(sameModel);

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

                            // Emit the signal to notify that the new model has been added
                            Q_EMIT agentModelHasBeenAdded(model);
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
            DefinitionM* copy = definition->copy();

            // Create a new view model of agents grouped by definition
            AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = new AgentsGroupedByDefinitionVM(model, copy);

            // Save this new view model of agents grouped by definition
            _saveNewAgentsGroupedByDefinition(agentsGroupedByDefinition);
        }
    }
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

            // Emit the signal to notify that the new model has been added
            Q_EMIT agentModelHasBeenAdded(model);
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
        // Connect slots to signals from this new view model of agents grouped by definition
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::noMoreModelAndUseless, this, &AgentsGroupedByNameVM::_onUselessAgentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();

            _hashFromDefinitionToAgentsGroupedByDefinition.insert(definition, agentsGroupedByDefinition);

            // Manage the list of Inputs / Outputs / Parameters of the added definition
            _manageInputsOfAddedDefinition(definition);
            _manageOutputsOfAddedDefinition(definition);
            _manageParametersOfAddedDefinition(definition);

            // Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
            _updateIsDefinedInAllDefinitionsForEachIOP(_hashFromDefinitionToAgentsGroupedByDefinition.count());

            // Emit signal "models of Inputs/Outputs/Parameters Changed"
            //Q_EMIT modelsOfIOPChanged();
        }

        _allAgentsGroupsByDefinition.append(agentsGroupedByDefinition);

        // Emit the signal "Agents grouped by definition has been created"
        Q_EMIT agentsGroupedByDefinitionHasBeenCreated(agentsGroupedByDefinition);
    }
}


/**
 * @brief Manage the list of inputs of the added definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageInputsOfAddedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->inputsList()->isEmpty())
    {
        QList<InputVM*> inputsListToAdd;

        for (AgentIOPM* input : definition->inputsList()->toList())
        {
            if (input != nullptr)
            {
                // Manage this new model of input
                QPair<bool, InputVM*> pair = _manageNewInputModel(input);

                // We have to add a new view model of input
                if (pair.first)
                {
                    InputVM* inputVM = pair.second;
                    if (inputVM != nullptr) {
                        inputsListToAdd.append(inputVM);
                    }
                }
            }
        }

        if (!inputsListToAdd.isEmpty())
        {
            _inputsList.append(inputsListToAdd);

            // Emit the signal "Inputs have been Added"
            Q_EMIT inputsHaveBeenAdded(inputsListToAdd);
        }
    }
}


/**
 * @brief Manage the list of outputs of the added definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageOutputsOfAddedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->outputsList()->isEmpty())
    {
        QList<OutputVM*> outputsListToAdd;

        for (OutputM* output : definition->outputsList()->toList())
        {
            if (output != nullptr)
            {
                // Manage this new model of output
                QPair<bool, OutputVM*> pair = _manageNewOutputModel(output);

                // We have to add a new view model of output
                if (pair.first)
                {
                    OutputVM* outputVM = pair.second;
                    if (outputVM != nullptr) {
                        outputsListToAdd.append(outputVM);
                    }
                }
            }
        }

        if (!outputsListToAdd.isEmpty())
        {
            _outputsList.append(outputsListToAdd);

            // Emit the signal "Outputs have been Added"
            Q_EMIT outputsHaveBeenAdded(outputsListToAdd);
        }
    }
}


/**
 * @brief Manage the list of parameters of the added definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageParametersOfAddedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->parametersList()->isEmpty())
    {
        QList<ParameterVM*> parametersListToAdd;

        for (AgentIOPM* parameter : definition->parametersList()->toList())
        {
            if (parameter != nullptr)
            {
                // Manage this new model of parameter
                QPair<bool, ParameterVM*> pair = _manageNewParameterModel(parameter);

                // We have to add a new view model of output
                if (pair.first)
                {
                    ParameterVM* parameterVM = pair.second;
                    if (parameterVM != nullptr) {
                        parametersListToAdd.append(parameterVM);
                    }
                }
            }
        }

        if (!parametersListToAdd.isEmpty())
        {
            _parametersList.append(parametersListToAdd);

            // Emit the signal "Parameters have been Added"
            //Q_EMIT parametersHaveBeenAdded(parametersListToAdd);
        }
    }
}


/**
 * @brief Manage the list of inputs of the removed definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageInputsOfRemovedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->inputsList()->isEmpty())
    {
        QList<InputVM*> inputsListToRemove;

        for (AgentIOPM* input : definition->inputsList()->toList())
        {
            if (input != nullptr)
            {
                // Manage this old model of input
                QPair<bool, InputVM*> pair = _manageOldInputModel(input);

                // We have to remove an old view model of input
                if (pair.first)
                {
                    InputVM* inputVM = pair.second;
                    if (inputVM != nullptr) {
                        inputsListToRemove.append(inputVM);
                    }
                }
            }
        }

        if (!inputsListToRemove.isEmpty())
        {
            // Emit the signal "Inputs will be Removed"
            Q_EMIT inputsWillBeRemoved(inputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_inputsList.remove(inputsListToRemove);
            for (InputVM* inputVM : inputsListToRemove)
            {
                if (inputVM != nullptr) {
                    _inputsList.remove(inputVM);
                }
            }
        }
    }
}


/**
 * @brief Manage the list of outputs of the removed definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageOutputsOfRemovedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->outputsList()->isEmpty())
    {
        QList<OutputVM*> outputsListToRemove;

        for (OutputM* output : definition->outputsList()->toList())
        {
            if (output != nullptr)
            {
                // Manage this old model of output
                QPair<bool, OutputVM*> pair = _manageOldOutputModel(output);

                // We have to remove an old view model of output
                if (pair.first)
                {
                    OutputVM* outputVM = pair.second;
                    if (outputVM != nullptr) {
                        outputsListToRemove.append(outputVM);
                    }
                }
            }
        }

        if (!outputsListToRemove.isEmpty())
        {
            // Emit the signal "Outputs will be Removed"
            Q_EMIT outputsWillBeRemoved(outputsListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_outputsList.remove(outputsListToRemove);
            for (OutputVM* outputVM : outputsListToRemove)
            {
                if (outputVM != nullptr) {
                    _outputsList.remove(outputVM);
                }
            }
        }
    }
}


/**
 * @brief Manage the list of parameters of the removed definition
 * @param definition
 */
void AgentsGroupedByNameVM::_manageParametersOfRemovedDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && !definition->parametersList()->isEmpty())
    {
        QList<ParameterVM*> parametersListToRemove;

        for (AgentIOPM* parameter : definition->parametersList()->toList())
        {
            if (parameter != nullptr)
            {
                // Manage this old model of parameter
                QPair<bool, ParameterVM*> pair = _manageOldParameterModel(parameter);

                // We have to remove an old view model of parameter
                if (pair.first)
                {
                    ParameterVM* parameterVM = pair.second;
                    if (parameterVM != nullptr) {
                        parametersListToRemove.append(parameterVM);
                    }
                }
            }
        }

        if (!parametersListToRemove.isEmpty())
        {
            // Emit the signal "Parameters will be Removed"
            //Q_EMIT parametersWillBeRemoved(parametersListToRemove);

            // FIXME TODO I2 Quick: Allow to remove a QList
            //_parametersList.remove(parametersListToRemove);
            for (ParameterVM* parameterVM : parametersListToRemove)
            {
                if (parameterVM != nullptr) {
                    _parametersList.remove(parameterVM);
                }
            }
        }
    }
}


/**
 * @brief Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
 * @param numberOfDefinitions
 */
void AgentsGroupedByNameVM::_updateIsDefinedInAllDefinitionsForEachIOP(int numberOfDefinitions)
{
    // Update the flag "Is Defined in All Definitions" for each input
    for (InputVM* input : _inputsList.toList())
    {
        if (input != nullptr)
        {
            if (input->models()->count() == numberOfDefinitions) {
                input->setisDefinedInAllDefinitions(true);
            }
            else {
                input->setisDefinedInAllDefinitions(false);
            }
        }
    }

    // Update the flag "Is Defined in All Definitions" for each output
    for (OutputVM* output : _outputsList.toList())
    {
        if (output != nullptr)
        {
            if (output->models()->count() == numberOfDefinitions) {
                output->setisDefinedInAllDefinitions(true);
            }
            else {
                output->setisDefinedInAllDefinitions(false);
            }
        }
    }

    // Update the flag "Is Defined in All Definitions" for each parameter
    for (ParameterVM* parameter : _parametersList.toList())
    {
        if (parameter != nullptr)
        {
            if (parameter->models()->count() == numberOfDefinitions) {
                parameter->setisDefinedInAllDefinitions(true);
            }
            else {
                parameter->setisDefinedInAllDefinitions(false);
            }
        }
    }
}


/**
 * @brief Manage a new model of input
 * @param input
 * @return
 */
QPair<bool, InputVM*> AgentsGroupedByNameVM::_manageNewInputModel(AgentIOPM* input)
{
    bool haveToAdd = false;
    InputVM* inputVM = nullptr;

    if ((input != nullptr) && !input->name().isEmpty())
    {
        // Input id is defined
        if (!input->id().isEmpty())
        {
            inputVM = getInputFromId(input->id());

            // There is already a view model for this id
            if (inputVM != nullptr)
            {
                // Add this new model to the list
                inputVM->models()->append(input);
            }
            // There is not yet a view model for this id
            else
            {
                haveToAdd = true;

                // Create a new view model of input
                inputVM = new InputVM(input->name(),
                                      input->id(),
                                      input,
                                      this);

                // Don't add to the list here (this input will be added globally via temporary list)

                _hashFromIdToInput.insert(input->id(), inputVM);

                // Update the list of view models of input for this name
                QList<InputVM*> inputsWithSameName = getInputsListFromName(input->name());
                inputsWithSameName.append(inputVM);
                _hashFromNameToInputsList.insert(input->name(), inputsWithSameName);
            }
        }
        // Input id is NOT defined
        else
        {
            // FIXME TODO: Manage the model of input with an empty id (defined name but no type)
            qCritical() << "FIXME TODO: Manage the model of input" << input->name() << "with an empty id (agent" << _name << ")";
        }
    }

    return QPair<bool, InputVM*>(haveToAdd, inputVM);
}


/**
 * @brief Manage a new model of output
 * @param output
 * @return
 */
QPair<bool, OutputVM*> AgentsGroupedByNameVM::_manageNewOutputModel(OutputM* output)
{
    bool haveToAdd = false;
    OutputVM* outputVM = nullptr;

    if ((output != nullptr) && !output->name().isEmpty())
    {
        // Output id is defined
        if (!output->id().isEmpty())
        {
            outputVM = getOutputFromId(output->id());

            // There is already a view model for this id
            if (outputVM != nullptr)
            {
                // Add this new model to the list
                outputVM->models()->append(output);
            }
            // There is not yet a view model for this id
            else
            {
                haveToAdd = true;

                // Create a new view model of output
                outputVM = new OutputVM(output->name(),
                                        output->id(),
                                        output,
                                        this);

                // Don't add to the list here (this output will be added globally via temporary list)

                _hashFromIdToOutput.insert(output->id(), outputVM);

                // Update the list of view models of output for this name
                QList<OutputVM*> outputsWithSameName = getOutputsListFromName(output->name());
                outputsWithSameName.append(outputVM);
                _hashFromNameToOutputsList.insert(output->name(), outputsWithSameName);
            }
        }
        // Output id is NOT defined
        else
        {
            // FIXME TODO: Manage the model of output with an empty id (defined name but no type)
            qCritical() << "FIXME TODO: Manage the model of output" << output->name() << "with an empty id (agent" << _name << ")";
        }
    }

    return QPair<bool, OutputVM*>(haveToAdd, outputVM);
}


/**
 * @brief Manage a new model of parameter
 * @param parameter
 * @return
 */
QPair<bool, ParameterVM*> AgentsGroupedByNameVM::_manageNewParameterModel(AgentIOPM* parameter)
{
    bool haveToAdd = false;
    ParameterVM* parameterVM = nullptr;

    if ((parameter != nullptr) && !parameter->name().isEmpty())
    {
        // Parameter id is defined
        if (!parameter->id().isEmpty())
        {
            parameterVM = getParameterFromId(parameter->id());

            // There is already a view model for this id
            if (parameterVM != nullptr)
            {
                // Add this new model to the list
                parameterVM->models()->append(parameter);
            }
            // There is not yet a view model for this id
            else
            {
                haveToAdd = true;

                // Create a new view model of parameter
                parameterVM = new ParameterVM(parameter->name(),
                                              parameter->id(),
                                              parameter,
                                              this);

                // Don't add to the list here (this parameter will be added globally via temporary list)

                _hashFromIdToParameter.insert(parameter->id(), parameterVM);

                // Update the list of view models of parameter for this name
                QList<ParameterVM*> parametersWithSameName = getParametersListFromName(parameter->name());
                parametersWithSameName.append(parameterVM);
                _hashFromNameToParametersList.insert(parameter->name(), parametersWithSameName);
            }
        }
        // Parameter id is NOT defined
        else
        {
            // FIXME TODO: Manage the model of parameter with an empty id (defined name but no type)
            qCritical() << "FIXME TODO: Manage the model of parameter" << parameter->name() << "with an empty id (agent" << _name << ")";
        }
    }

    return QPair<bool, ParameterVM*>(haveToAdd, parameterVM);
}


/**
 * @brief Manage an old model of input (just before being deleted)
 * @param input
 * @return
 */
QPair<bool, InputVM*> AgentsGroupedByNameVM::_manageOldInputModel(AgentIOPM* input)
{
    bool haveToRemove = false;
    InputVM* inputVM = nullptr;

    if ((input != nullptr) && !input->name().isEmpty())
    {
        // Input id is defined
        if (!input->id().isEmpty())
        {
            inputVM = getInputFromId(input->id());

            if (inputVM != nullptr)
            {
                // Remove this old model from the list
                inputVM->models()->remove(input);

                // The view model of input is empty !
                if (inputVM->models()->isEmpty())
                {
                    haveToRemove = true;

                    // Don't remove from the list here (this input will be removed globally via temporary list)

                    // Remove from the hash table with the input id
                    _hashFromIdToInput.remove(input->id());

                    // Update the list of view models of input for this name
                    QList<InputVM*> inputsWithSameName = getInputsListFromName(input->name());
                    inputsWithSameName.removeOne(inputVM);
                    _hashFromNameToInputsList.insert(input->name(), inputsWithSameName);
                }
            }
        }
    }

    return QPair<bool, InputVM*>(haveToRemove, inputVM);
}


/**
 * @brief Manage an old model of output (just before being deleted)
 * @param output
 * @return
 */
QPair<bool, OutputVM*> AgentsGroupedByNameVM::_manageOldOutputModel(OutputM* output)
{
    bool haveToRemove = false;
    OutputVM* outputVM = nullptr;

    if ((output != nullptr) && !output->name().isEmpty())
    {
        // Input id is defined
        if (!output->id().isEmpty())
        {
            outputVM = getOutputFromId(output->id());

            if (outputVM != nullptr)
            {
                // Remove this old model from the list
                outputVM->models()->remove(output);

                // The view model of output is empty !
                if (outputVM->models()->isEmpty())
                {
                    haveToRemove = true;

                    // Don't remove from the list here (this output will be removed globally via temporary list)

                    // Remove from the hash table with the output id
                    _hashFromIdToOutput.remove(output->id());

                    // Update the list of view models of output for this name
                    QList<OutputVM*> outputsWithSameName = getOutputsListFromName(output->name());
                    outputsWithSameName.removeOne(outputVM);
                    _hashFromNameToOutputsList.insert(output->name(), outputsWithSameName);
                }
            }
        }
    }

    return QPair<bool, OutputVM*>(haveToRemove, outputVM);
}


/**
 * @brief Manage an old model of parameter (just before being deleted)
 * @param parameter
 * @return
 */
QPair<bool, ParameterVM*> AgentsGroupedByNameVM::_manageOldParameterModel(AgentIOPM* parameter)
{
    bool haveToRemove = false;
    ParameterVM* parameterVM = nullptr;

    if ((parameter != nullptr) && !parameter->name().isEmpty())
    {
        // parameter id is defined
        if (!parameter->id().isEmpty())
        {
            parameterVM = getParameterFromId(parameter->id());

            if (parameterVM != nullptr)
            {
                // Remove this old model from the list
                parameterVM->models()->remove(parameter);

                // The view model of parameter is empty !
                if (parameterVM->models()->isEmpty())
                {
                    haveToRemove = true;

                    // Don't remove from the list here (this parameter will be removed globally via temporary list)

                    // Remove from the hash table with the parameter id
                    _hashFromIdToParameter.remove(parameter->id());

                    // Update the list of view models of parameter for this name
                    QList<ParameterVM*> parametersWithSameName = getParametersListFromName(parameter->name());
                    parametersWithSameName.removeOne(parameterVM);
                    _hashFromNameToParametersList.insert(parameter->name(), parametersWithSameName);
                }
            }
        }
    }

    return QPair<bool, ParameterVM*>(haveToRemove, parameterVM);
}

