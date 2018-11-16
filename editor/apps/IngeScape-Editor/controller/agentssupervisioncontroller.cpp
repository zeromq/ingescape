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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
AgentsSupervisionController::AgentsSupervisionController(IngeScapeModelManager* modelManager,
                                                         JsonHelper* jsonHelper,
                                                         QObject *parent) : QObject(parent),
    _selectedAgent(NULL),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != nullptr)
    {
        // Agents are sorted on their name (alphabetical order)
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
    setselectedAgent(nullptr);

    _hashFromDefinitionNameToDefinitionsList.clear();

    //_mapFromNameToAgentViewModelsList.clear();

    // Deleted elsewhere (in the destructor of AgentsGroupedByNameVM)
    //_agentsList.deleteAllItems();
    _agentsList.clear();

    // Reset pointers
    _modelManager = NULL;
    _jsonHelper = NULL;
}


/**
 * @brief Remove the agent from the list and delete it
 * @param agent
 */
void AgentsSupervisionController::deleteAgentInList(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if ((_modelManager != nullptr) && (agentsGroupedByDefinition != nullptr))
    {
        qInfo() << "Delete the agent" << agentsGroupedByDefinition->name() << "in the List";

        AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentsGroupedByDefinition->name());
        if (agentsGroupedByName != nullptr) {
            // Delete the view model of agents grouped by definition
            agentsGroupedByName->deleteAgentsGroupedByDefinition(agentsGroupedByDefinition);
        }
    }
}


/**
 * @brief Delete the model of agent
 * If it is the last model of a view model, we reset all its network data (only defined by the agent definition)
 * @param agent
 * @return
 */
/*void AgentsSupervisionController::deleteModelOfAgent(AgentM* agent)
{
    // Check that the agent is OFF
    if ((agent != nullptr) && !agent->isON() && !agent->name().isEmpty() && (_modelManager != nullptr))
    {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

        for (AgentVM* iterator : agentViewModelsList)
        {
            // We found the view model that contains this model
            if ((iterator != nullptr) && iterator->models()->contains(agent))
            {
                // It is the last model of this view model
                if (iterator->models()->count() == 1)
                {
                    // Clear the data about the network
                    agent->clearNetworkData();
                }
                // There are other(s) model(s) in this view model
                else
                {
                    // Remove the model from the list of the view model
                    iterator->models()->remove(agent);

                    // Delete the model of agent
                    _modelManager->deleteAgentModel(agent);
                }
                break;
            }
        }
    }
}*/


/**
 * @brief Export the agents list to selected file
 */
void AgentsSupervisionController::exportAgentsListToSelectedFile()
{
    if (_modelManager != nullptr)
    {
        // Export the agents list to JSON
        QJsonArray arrayOfAgents = exportAgentsListToJSON();

        // Export the agents list to selected file
        _modelManager->exportAgentsListToSelectedFile(arrayOfAgents);
    }
}


/**
 * @brief Export the agents list to JSON
 * @return
 */
QJsonArray AgentsSupervisionController::exportAgentsListToJSON()
{
    QJsonArray jsonArray = QJsonArray();

    if (_jsonHelper != nullptr)
    {
        // FIXME REPAIR: exportAgentsListToJSON
        /*for (AgentVM* agent : _agentsList.toList())
        {
            if ((agent != nullptr) && !agent->name().isEmpty())
            {
                QJsonObject jsonAgent = QJsonObject();

                // Name
                jsonAgent.insert("agentName", agent->name());

                if (agent->definition() != nullptr)
                {
                    // Definition
                    QJsonObject jsonDefinition = _jsonHelper->exportAgentDefinitionToJson(agent->definition());
                    jsonAgent.insert("definition", jsonDefinition);
                }
                else {
                    jsonAgent.insert("definition", QJsonValue());
                }

                // Clones (models)
                QJsonArray jsonClones = QJsonArray();

                for (AgentM* model : agent->models()->toList())
                {
                    // Hostname and Command Line must be defined to be added to the array of clones
                    if ((model != nullptr) && !model->hostname().isEmpty() && !model->commandLine().isEmpty())
                    {
                        qDebug() << "Export" << agent->name() << "on" << model->hostname() << "at" << model->commandLine() << "(" << model->peerId() << ")";

                        QJsonObject jsonClone = QJsonObject();
                        jsonClone.insert("hostname", model->hostname());
                        jsonClone.insert("commandLine", model->commandLine());
                        jsonClone.insert("peerId", model->peerId());
                        jsonClone.insert("address", model->address());

                        jsonClones.append(jsonClone);
                    }
                }

                jsonAgent.insert("clones", jsonClones);

                jsonArray.append(jsonAgent);
            }
        }*/
    }
    return jsonArray;
}


/**
 * @brief Remove (and delete) each UN-active agent (agent with state OFF) from the current list of agents
 */
void AgentsSupervisionController::removeUNactiveAgents()
{
    qInfo() << "Remove each UN-active agent from the current list of agents";

    if (_modelManager != nullptr)
    {
        // FIXME REPAIR: removeUNactiveAgents
        /*for (AgentVM* agent : _agentsList.toList())
        {
            if (agent != nullptr)
            {
                // ON
                if (agent->isON())
                {
                    // Delete each model with state OFF
                    for (AgentM* model : agent->models()->toList())
                    {
                        if ((model != nullptr) && !model->isON()) {
                            _modelManager->deleteAgentModel(model);
                        }
                    }
                }
                // OFF
                else
                {
                    // Remove the agent from the list and delete it
                    deleteAgentInList(agent);
                }
            }
        }*/
    }
}


/**
 * @brief Slot when a new model of agent has been created
 * @param agent
 */
/*void AgentsSupervisionController::onAgentModelCreated(AgentM* model)
{
    if (model != nullptr)
    {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(model->name());

        AgentVM* agentWithoutDefinition = NULL;

        if (!agentViewModelsList.isEmpty())
        {
            for (AgentVM* iterator : agentViewModelsList)
            {
                // If this agent VM does not have any definition either
                if ((iterator != nullptr) && (iterator->definition() == nullptr))
                {
                    qDebug() << "There is already an agent with the same name" << model->name() << "and no definition !";

                    agentWithoutDefinition = iterator;
                    break;
                }
            }
        }

        if (agentWithoutDefinition != nullptr)
        {
            // Manage the new model inside the existing view model without definition
            _manageNewModelInsideExistingVM(model, agentWithoutDefinition);
        }
        else
        {
            // Create a new view model of agent
            AgentVM* agent = new AgentVM(model, this);

            // Connect slots to signals from this new view model of agent
            connect(agent, &AgentVM::definitionChangedWithPreviousAndNewValues, this, &AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousAndNewValues);
            connect(agent, &AgentVM::differentDefinitionDetectedOnModelOfAgent, this, &AgentsSupervisionController::_onDifferentDefinitionDetectedOnModelOfAgent);

            // Propagate some signals from this new view model of agent
            connect(agent, &AgentVM::commandAskedToLauncher, this, &AgentsSupervisionController::commandAskedToLauncher);
            connect(agent, &AgentVM::commandAskedToAgent, this, &AgentsSupervisionController::commandAskedToAgent);
            connect(agent, &AgentVM::commandAskedToAgentAboutOutput, this, &AgentsSupervisionController::commandAskedToAgentAboutOutput);
            connect(agent, &AgentVM::openValuesHistoryOfAgent, this, &AgentsSupervisionController::openValuesHistoryOfAgent);
            connect(agent, &AgentVM::openLogStreamOfAgents, this, &AgentsSupervisionController::openLogStreamOfAgents);


            agentViewModelsList.append(agent);
            _mapFromNameToAgentViewModelsList.insert(model->name(), agentViewModelsList);

            // Add our view model to the list
            _agentsList.append(agent);
        }
    }
}*/


/**
 * @brief Slot called when a new view model of agents grouped by definition has been created
 * @param agentsGroupedByDefinition
 */
void AgentsSupervisionController::onAgentsGroupedByDefinitionHasBeenCreated(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Propagate some signals from this new view model of agents grouped by definition
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToLauncher, this, &AgentsSupervisionController::commandAskedToLauncher);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToAgent, this, &AgentsSupervisionController::commandAskedToAgent);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToAgentAboutOutput, this, &AgentsSupervisionController::commandAskedToAgentAboutOutput);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::openValuesHistoryOfAgent, this, &AgentsSupervisionController::openValuesHistoryOfAgent);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::openLogStreamOfAgents, this, &AgentsSupervisionController::openLogStreamOfAgents);

        // Connect some signals from this new view model of agents grouped by definition to slots
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::loadAgentDefinitionFromPath, this, &AgentsSupervisionController::_onLoadAgentDefinitionFromPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::loadAgentMappingFromPath, this, &AgentsSupervisionController::_onLoadAgentMappingFromPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::downloadAgentDefinitionToPath, this, &AgentsSupervisionController::_onDownloadAgentDefinitionToPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::downloadAgentMappingToPath, this, &AgentsSupervisionController::_onDownloadAgentMappingToPath);

        // Add our view model to the list
        _agentsList.append(agentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();
            QString definitionName = definition->name();

            qDebug() << "on Agents Grouped by Definition" << definitionName << "has been Created" << agentsGroupedByDefinition->name();

            // Get the list of definitions with a name
            QList<DefinitionM*> definitionsList = _getDefinitionsListWithName(definitionName);

            definitionsList.append(definition);
            _hashFromDefinitionNameToDefinitionsList.insert(definitionName, definitionsList);

            // Update the definition variants (same name, same version but the lists of I/O/P are differents)
            _updateDefinitionVariants(definitionName, definitionsList);
        }
        else
        {
            qDebug() << "on Agents Grouped by Definition 'NULL' has been Created" << agentsGroupedByDefinition->name();
        }
    }
}


/**
 * @brief Slot called when a view model of agents grouped by definition will be deleted
 * @param agentsGroupedByDefinition
 */
void AgentsSupervisionController::onAgentsGroupedByDefinitionWillBeDeleted(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Unselect our agent if needed
        if (_selectedAgent == agentsGroupedByDefinition) {
            setselectedAgent(nullptr);
        }

        // DIS-connect to signals from this view model of agents grouped by definition
        disconnect(agentsGroupedByDefinition, 0, this, 0);

        // Remove it from the list
        _agentsList.remove(agentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();
            QString definitionName = definition->name();

            qDebug() << "on Agents Grouped by Definition" << definitionName << "will be Deleted" << agentsGroupedByDefinition->name();

            // Get the list of definitions with a name
            QList<DefinitionM*> definitionsList = _getDefinitionsListWithName(definitionName);

            definitionsList.removeOne(definition);
            _hashFromDefinitionNameToDefinitionsList.insert(definitionName, definitionsList);

            // Update the definition variants (same name, same version but the lists of I/O/P are differents)
            _updateDefinitionVariants(definitionName, definitionsList);
        }
        else
        {
            qDebug() << "on Agents Grouped by Definition 'NULL' will be Deleted" << agentsGroupedByDefinition->name();
        }
    }
}


/**
 * @brief Slot when the definition of a view model of agent changed (with previous and new values)
 * @param previousDefinition
 * @param newDefinition
 */
/*void AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousAndNewValues(DefinitionM* previousDefinition, DefinitionM* newDefinition)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((agent != nullptr) && (_modelManager != nullptr) && (newDefinition != nullptr))
    {
        // The previous definition was NULL (and the new definition is defined)
        if (previousDefinition == nullptr)
        {
            //
            // Update our list of agents with the new definition for this agent
            //

            // Get the list of view models of agent from a name
            QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

            AgentVM* agentUsingSameDefinition = NULL;

            for (AgentVM* iterator : agentViewModelsList)
            {
                // If this agent VM does not correspond to the sender agent
                if ((iterator != nullptr) && (iterator != agent) && (iterator->definition() != nullptr)
                        &&
                        // The 2 definitions are strictly identicals
                        DefinitionM::areIdenticals(iterator->definition(), newDefinition))
                {
                    qDebug() << "There is exactly the same agent definition for name" << newDefinition->name() << "and version" << newDefinition->version();

                    agentUsingSameDefinition = iterator;
                    break;
                }
            }

            // Exactly the same definition
            if (agentUsingSameDefinition != nullptr)
            {
                // It must have only one model of agent
                if (agent->models()->count() == 1)
                {
                    AgentM* model = agent->models()->at(0);
                    if (model != nullptr)
                    {
                        // The current view model for this model of agent is useless, we have to remove it from the list
                        _agentsList.remove(agent);

                        // Reset its definition
                        agent->setdefinition(nullptr);

                        // Delete it
                        _deleteAgentViewModel(agent);
                        agent = NULL;


                        // Manage the new model inside the existing view model with the same definition
                        _manageNewModelInsideExistingVM(model, agentUsingSameDefinition);
                    }
                }
            }
        }
        // The previous definition was already defined (and the new definition is defined)
        else
        {
            // Check if we have to merge this agent with another one that have the same definition
            _checkHaveToMergeAgent(agent);
        }
    }
}*/


/**
 * @brief Slot called when a different definition is detected on a model of agent
 * (compared to the definition of our view model)
 * @param model
 */
/*void AgentsSupervisionController::_onDifferentDefinitionDetectedOnModelOfAgent(AgentM* model)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((agent != nullptr) && (model != nullptr) && (agent->definition() != nullptr) && (model->definition() != nullptr))
    {
        // Remove the model of agent from the list of the view model
        agent->models()->remove(model);

        AgentVM* agentUsingSameDefinition = NULL;

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(model->name());
        for (AgentVM* iterator : agentViewModelsList)
        {
            if ((iterator != nullptr) && (iterator->definition() != nullptr)
                    // The 2 definitions are strictly identicals
                    && DefinitionM::areIdenticals(iterator->definition(), model->definition()))
            {
                agentUsingSameDefinition = iterator;
                break;
            }
        }

        if (agentUsingSameDefinition == nullptr)
        {
            // Allows to create a new view model of agent for this model
            onAgentModelCreated(model);
        }
        // A view model of agent already exists with exactly the same definition
        else
        {
            // Add the model of agent to the list of the view model
            agentUsingSameDefinition->models()->append(model);
        }
    }
}*/


/**
 * @brief Slot called when we have to load an agent definition from a JSON file (path)
 * @param peerIdsList
 * @param definitionFilePath
 */
void AgentsSupervisionController::_onLoadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && !peerIdsList.isEmpty() && !definitionFilePath.isEmpty())
    {
        QFile jsonFile(definitionFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            // Compact JSON
            QString jsonOfDefinition = QString(jsonDocument.toJson(QJsonDocument::Compact));

            // Create the command "Load Definition"
            QString command = QString("%1%2").arg(command_LoadDefinition, jsonOfDefinition);

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }
        else {
            qCritical() << "Can not open file" << definitionFilePath << "(to load the definition of" << agentsGroupedByDefinition->name() << ")";
        }
    }
}


/**
 * @brief Slot called when we have to load an agent mapping from a JSON file (path)
 * @param mappingFilePath
 */
void AgentsSupervisionController::_onLoadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && !peerIdsList.isEmpty() && !mappingFilePath.isEmpty())
    {
        QFile jsonFile(mappingFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            // Compact JSON
            QString jsonOfMapping = QString(jsonDocument.toJson(QJsonDocument::Compact));

            // Create the command "Load Mapping"
            QString command = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }
        else {
            qCritical() << "Can not open file" << mappingFilePath << "(to load the mapping of" << agentsGroupedByDefinition->name() << ")";
        }
    }
}


/**
 * @brief Slot called when we have to download an agent definition to a JSON file (path)
 * @param agentDefinition
 * @param definitionFilePath
 */
void AgentsSupervisionController::_onDownloadAgentDefinitionToPath(DefinitionM* agentDefinition, QString definitionFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && (agentDefinition != nullptr) && !definitionFilePath.isEmpty())
    {
        // Get the JSON of the agent definition
        QString jsonOfDefinition = _jsonHelper->getJsonOfAgentDefinition(agentDefinition, QJsonDocument::Indented);
        if (!jsonOfDefinition.isEmpty())
        {
            QFile jsonFile(definitionFilePath);
            //if (jsonFile.exists()) {
            //    qWarning() << "The file" << definitionFilePath << "already exist !";
            //}

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfDefinition.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << definitionFilePath << "(to save the definition of" << agentsGroupedByDefinition->name() << ")";
            }
        }
    }
}


/**
 * @brief Slot called when we have to download an agent mapping to a JSON file (path)
 * @param agentMapping
 * @param mappingFilePath
 */
void AgentsSupervisionController::_onDownloadAgentMappingToPath(AgentMappingM* agentMapping, QString mappingFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && (agentMapping != nullptr) && !mappingFilePath.isEmpty())
    {
        // Get the JSON of the agent mapping
        QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(agentMapping, QJsonDocument::Indented);
        if (!jsonOfMapping.isEmpty())
        {
            QFile jsonFile(mappingFilePath);
            //if (jsonFile.exists()) {
            //    qWarning() << "The file" << mappingFilePath << "already exist !";
            //}

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfMapping.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << mappingFilePath << "(to save the mapping of" << agentsGroupedByDefinition->name() << ")";
            }
        }
    }
}


/**
 * @brief Manage a new model inside an existing view model
 * @param model
 * @param agentVM
 */
/*void AgentsSupervisionController::_manageNewModelInsideExistingVM(AgentM* model, AgentVM* agentVM)
{
    if ((model != nullptr) && (agentVM != nullptr) && (_modelManager != nullptr))
    {
        // Manage the new agent model
        QString hostname = model->hostname();

        // Hostname is not defined
        // There is already an existing model of agent (in the VM agentUsingSameDefinition)
        if (hostname == HOSTNAME_NOT_DEFINED)
        {
            //qDebug() << "Delete Model of agent" << model->name() << "on" << hostname;

            // Delete this new (fake) model of agent
            _modelManager->deleteAgentModel(model);
        }
        // Hostname is a real one
        else
        {
            // Get the list of agent models on the same host
            QList<AgentM*> modelsOnHost = agentVM->getModelsOnHost(hostname);

            // There is NO agent on this host yet
            if (modelsOnHost.isEmpty())
            {
                qDebug() << "Add model of agent" << model->name() << "on" << hostname;

                // Add the model of agent to the list of the VM
                agentVM->models()->append(model);
            }
            // There is already agent models on this host
            else
            {
                // Peer id is empty (the agent has never appeared on the network)
                if (model->peerId().isEmpty())
                {
                    qDebug() << "Add model of agent" << model->name() << "on" << hostname;

                    // Add the model of agent to the list of the VM
                    agentVM->models()->append(model);
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
                        //qDebug() << "Delete Model of agent" << model->name() << "on" << hostname;

                        // Delete this new (fake) model of agent
                        _modelManager->deleteAgentModel(model);
                    }
                    // Else if we have to replace an existing (same) model by the new one
                    else if (sameModel != nullptr)
                    {
                        int index = agentVM->models()->indexOf(sameModel);
                        if (index > -1)
                        {
                            // Emit signal "Identical Agent Model Replaced"
                            Q_EMIT identicalAgentModelReplaced(sameModel, model);

                            qDebug() << "Replace model of agent" << model->name() << "on" << hostname << "(" << sameModel->peerId() << "-->" << model->peerId() << ")";

                            // Replace the model
                            agentVM->models()->replace(index, model);

                            // Delete the previous model of agent
                            _modelManager->deleteAgentModel(sameModel);
                        }
                    }
                    // Else, we add the new model
                    else
                    {
                        qDebug() << "Add model of agent" << model->name() << "on" << hostname;

                        // Add the model of agent to the list of the VM
                        agentVM->models()->append(model);
                    }

                    qDebug() << "There are" << agentVM->models()->count() << "models of agent" << model->name() << "with the same definition";
                }
            }
        }
    }
}*/


/**
 * @brief Delete the view model of agent
 * @param agent
 */
/*void AgentsSupervisionController::_deleteAgentViewModel(AgentVM* agent)
{
    if (agent != nullptr)
    {
        // Unselect our agent if needed
        if (_selectedAgent == agent) {
            setselectedAgent(nullptr);
        }

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());
        agentViewModelsList.removeOne(agent);

        // Update the list in the map
        //_mapFromNameToAgentViewModelsList.insert(agent->name(), agentViewModelsList);

        // DIS-connect from signals from this old view model of agent
        disconnect(agent, 0, this, 0);

        // Free memory
        delete agent;
    }
}*/


/**
 * @brief Get the list of definitions with a name
 * @param definitionName
 * @return
 */
QList<DefinitionM*> AgentsSupervisionController::_getDefinitionsListWithName(QString definitionName)
{
    if (_hashFromDefinitionNameToDefinitionsList.contains(definitionName)) {
        return _hashFromDefinitionNameToDefinitionsList.value(definitionName);
    }
    else {
        return QList<DefinitionM*>();
    }
}


/**
 * @brief Update the definition variants (same name, same version but the lists of I/O/P are differents)
 * @param definitionName
 * @param definitionsList
 */
void AgentsSupervisionController::_updateDefinitionVariants(QString definitionName, QList<DefinitionM*> definitionsList)
{
    Q_UNUSED(definitionName)
    //qDebug() << "Update the definition variants for definition name" << definitionName << "(" << definitionsList.count() << "definitions)";

    // We can use versions as keys of the map because the list contains only definition with the same name
    QHash<QString, QList<DefinitionM*>> hashFromVersionToDefinitionsList;
    QList<QString> versionsWithVariant;

    for (DefinitionM* iterator : definitionsList)
    {
        if ((iterator != nullptr) && !iterator->version().isEmpty())
        {
            // First, reset all
            iterator->setisVariant(false);

            QString version = iterator->version();
            QList<DefinitionM*> definitionsListForVersion;

            // Other(s) definition(s) have the same version (and the same name)
            if (hashFromVersionToDefinitionsList.contains(version))
            {
                definitionsListForVersion = hashFromVersionToDefinitionsList.value(version);

                // If this version is not already in the list of versions with variant
                if (!versionsWithVariant.contains(version))
                {
                    // We compare I/O/P between current iterator and the first one
                    DefinitionM* first = definitionsListForVersion.first();
                    if ((first != nullptr) && !DefinitionM::areIdenticals(first, iterator)) {
                        versionsWithVariant.append(version);
                    }
                }
            }

            definitionsListForVersion.append(iterator);
            hashFromVersionToDefinitionsList.insert(version, definitionsListForVersion);
        }
    }

    // The list contains only the versions that have variants
    for (QString version : versionsWithVariant)
    {
        QList<DefinitionM*> definitionsListForVersion = hashFromVersionToDefinitionsList.value(version);
        for (DefinitionM* iterator : definitionsListForVersion)
        {
            if (iterator != nullptr) {
                iterator->setisVariant(true);
                //qDebug() << iterator->name() << iterator->version() << "is variant";
            }
        }
    }
}
