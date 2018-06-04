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

    // Reset pointers
    _modelManager = NULL;
    _jsonHelper = NULL;
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

            foreach (DefinitionM* iterator, agentDefinitionsList)
            {
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
        foreach (DefinitionM* iterator, definitionsToOpen)
        {
            if (iterator != NULL)
            {
                if (!_modelManager->openedDefinitions()->contains(iterator)) {
                    _modelManager->openedDefinitions()->append(iterator);
                }
                else {
                    qDebug() << "The 'Definition'" << iterator->name() << "is already opened...bring to front !";

                    Q_EMIT iterator->bringToFront();
                }
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
    if ((model != NULL) && !model->isRecorder())
    {
        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(model->name());

        // Create a new view model of agent
        AgentVM* agent = new AgentVM(model, this);

        // Connect slots to signals from this new view model of agent
        connect(agent, &AgentVM::definitionChangedWithPreviousAndNewValues, this, &AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousAndNewValues);
        connect(agent, &AgentVM::differentDefinitionDetectedOnModelOfAgent, this, &AgentsSupervisionController::_onDifferentDefinitionDetectedOnModelOfAgent);
        connect(agent, &AgentVM::loadAgentDefinitionFromPath, this, &AgentsSupervisionController::_onLoadAgentDefinitionFromPath);
        connect(agent, &AgentVM::loadAgentMappingFromPath, this, &AgentsSupervisionController::_onLoadAgentMappingFromPath);
        connect(agent, &AgentVM::downloadAgentDefinitionToPath, this, &AgentsSupervisionController::_onDownloadAgentDefinitionToPath);
        connect(agent, &AgentVM::downloadAgentMappingToPath, this, &AgentsSupervisionController::_onDownloadAgentMappingToPath);

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


/**
 * @brief Slot when the definition of a view model of agent changed (with previous and new values)
 * @param previousDefinition
 * @param newDefinition
 */
void AgentsSupervisionController::_onAgentDefinitionChangedWithPreviousAndNewValues(DefinitionM* previousDefinition, DefinitionM* newDefinition)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((agent != NULL) && (_modelManager != NULL) && (newDefinition != NULL))
    {
        // The previous definition was NULL (and the new definition is defined)
        if (previousDefinition == NULL)
        {
            //
            // Update our list of agents with the new definition for this agent
            //

            // Get the list of view models of agent from a name
            QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());

            AgentVM* agentUsingSameDefinition = NULL;

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
                    break;
                }
            }

            // Exactly the same definition
            if (agentUsingSameDefinition != NULL)
            {
                // It must have only one model of agent
                if (agent->models()->count() == 1)
                {
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
                            }
                        }
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
}


/**
 * @brief Slot called when a different definition is detected on a model of agent
 * (compared to the definition of our view model)
 * @param model
 */
void AgentsSupervisionController::_onDifferentDefinitionDetectedOnModelOfAgent(AgentM* model)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((agent != NULL) && (model != NULL) && (agent->definition() != NULL) && (model->definition() != NULL))
    {
        // Remove the model of agent from the list of the view model
        agent->models()->remove(model);

        AgentVM* agentUsingSameDefinition = NULL;

        // Get the list of view models of agent from a name
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(model->name());
        for (AgentVM* iterator : agentViewModelsList)
        {
            if ((iterator != NULL) && (iterator->definition() != NULL)
                    // The 2 definitions are strictly identicals
                    && DefinitionM::areIdenticals(iterator->definition(), model->definition()))
            {
                agentUsingSameDefinition = iterator;
                break;
            }
        }

        if (agentUsingSameDefinition == NULL)
        {
            // Allows to create a new view model of agent for this model
            onAgentModelCreated(model);
        }
        // A view model of agent already exists with exactly the same definition
        else
        {
            // View model never yet appeared on the network
            if (agentUsingSameDefinition->neverAppearedOnNetwork() && (agentUsingSameDefinition->models()->count() == 1))
            {
                AgentM* modelUsingSameDefinition = agentUsingSameDefinition->models()->at(0);
                if (modelUsingSameDefinition != NULL)
                {
                    // We replace the fake model of agent
                    agentUsingSameDefinition->models()->replace(0, model);

                    qDebug() << "Replace model (which never appeared on network) by agent" << model->name() << "on" << model->address() << "(" << model->hostname() << ")";

                    // Delete the previous (fake) model of agent
                    _modelManager->deleteAgentModel(modelUsingSameDefinition);

                    // Update the flag "Never Appeared on the Network"
                    agentUsingSameDefinition->setneverAppearedOnNetwork(false);
                }
            }
            else
            {
                // Add the model of agent to the list of the view model
                agentUsingSameDefinition->models()->append(model);
            }
        }
    }
}


/**
 * @brief Slot called when we have to load an agent definition from a JSON file (path)
 * @param peerIdsList
 * @param definitionFilePath
 */
void AgentsSupervisionController::_onLoadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((_jsonHelper != NULL) && (agent != NULL) && !peerIdsList.isEmpty() && !definitionFilePath.isEmpty())
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
            qCritical() << "Can not open file" << definitionFilePath << "(to load the definition of" << agent->name() << ")";
        }
    }
}


/**
 * @brief Slot called when we have to load an agent mapping from a JSON file (path)
 * @param mappingFilePath
 */
void AgentsSupervisionController::_onLoadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath)
{
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((_jsonHelper != NULL) && (agent != NULL) && !peerIdsList.isEmpty() && !mappingFilePath.isEmpty())
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
            qCritical() << "Can not open file" << mappingFilePath << "(to load the mapping of" << agent->name() << ")";
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
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((_jsonHelper != NULL) && (agent != NULL) && (agentDefinition != NULL) && !definitionFilePath.isEmpty())
    {
        // Get the JSON of the agent definition
        QString jsonOfDefinition = _jsonHelper->getJsonOfAgentDefinition(agentDefinition, QJsonDocument::Indented);
        if (!jsonOfDefinition.isEmpty())
        {
            QFile jsonFile(definitionFilePath);
            /*if (jsonFile.exists()) {
                qWarning() << "The file" << definitionFilePath << "already exist !";
            }*/

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfDefinition.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << definitionFilePath << "(to save the definition of" << agent->name() << ")";
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
    AgentVM* agent = qobject_cast<AgentVM*>(sender());
    if ((_jsonHelper != NULL) && (agent != NULL) && (agentMapping != NULL) && !mappingFilePath.isEmpty())
    {
        // Get the JSON of the agent mapping
        QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(agentMapping, QJsonDocument::Indented);
        if (!jsonOfMapping.isEmpty())
        {
            QFile jsonFile(mappingFilePath);
            /*if (jsonFile.exists()) {
                qWarning() << "The file" << mappingFilePath << "already exist !";
            }*/

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfMapping.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << mappingFilePath << "(to save the mapping of" << agent->name() << ")";
            }
        }
    }
}


/**
 * @brief Check if we have to merge an agent with another one that have the same definition
 * @param agent
 */
void AgentsSupervisionController::_checkHaveToMergeAgent(AgentVM* agent)
{
    if ((agent != NULL) && (agent->definition() != NULL) && (agent->models()->count() == 1))
    {
        AgentM* model = agent->models()->at(0);
        if (model != NULL)
        {
            AgentVM* agentUsingSameDefinition = NULL;

            // Get the list of view models of agent from a name
            QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agent->name());
            for (AgentVM* iterator : agentViewModelsList)
            {
                if ((iterator != NULL) && (iterator != agent) && (iterator->definition() != NULL)
                        // The 2 definitions are strictly identicals
                        && DefinitionM::areIdenticals(iterator->definition(), agent->definition()))
                {
                    agentUsingSameDefinition = iterator;
                    break;
                }
            }

            // A view model of agent already exists with exactly the same definition
            if (agentUsingSameDefinition != NULL)
            {
                // The previous view model of agent is useless, we have to remove it from the list
                _agentsList.remove(agent);

                // Reset its definition
                agent->setdefinition(NULL);

                // Delete it
                _deleteAgentViewModel(agent);
                agent = NULL;

                // View model never yet appeared on the network
                if (agentUsingSameDefinition->neverAppearedOnNetwork() && (agentUsingSameDefinition->models()->count() == 1))
                {
                    AgentM* modelUsingSameDefinition = agentUsingSameDefinition->models()->at(0);
                    if (modelUsingSameDefinition != NULL)
                    {
                        // We replace the fake model of agent
                        agentUsingSameDefinition->models()->replace(0, model);

                        qDebug() << "Replace model (which never appeared on network) by agent" << model->name() << "on" << model->address() << "(" << model->hostname() << ")";

                        // Delete the previous (fake) model of agent
                        _modelManager->deleteAgentModel(modelUsingSameDefinition);

                        // Update the flag "Never Appeared on the Network"
                        agentUsingSameDefinition->setneverAppearedOnNetwork(false);
                    }
                }
                else
                {
                    // Add the model of agent to the list of the new view model
                    agentUsingSameDefinition->models()->append(model);
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
        disconnect(agent, 0, this, 0);

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
