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
 * @brief Export the agents list to selected file
 */
void AgentsSupervisionController::exportAgentsListToSelectedFile()
{
    if (_modelManager != NULL)
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

    if (_jsonHelper != NULL)
    {
        for (AgentVM* agent : _agentsList.toList())
        {
            if ((agent != NULL) && !agent->name().isEmpty() && (agent->definition() != NULL))
            {
                QJsonObject jsonAgent = QJsonObject();

                // Name
                jsonAgent.insert("agentName", agent->name());

                // Definition
                QJsonObject jsonDefinition = _jsonHelper->exportAgentDefinitionToJson(agent->definition());
                jsonAgent.insert("definition", jsonDefinition);

                // Clones (models)
                QJsonArray jsonClones = QJsonArray();

                QHash<QString, QStringList> hashFromHostnameToCommandLine;

                for (AgentM* model : agent->models()->toList())
                {
                    if ((model != NULL) && !model->hostname().isEmpty() && !model->commandLine().isEmpty())
                    {
                        bool alreadySameClone = false;
                        QStringList listOfCommandLine;

                        if (hashFromHostnameToCommandLine.contains(model->hostname()))
                        {
                            listOfCommandLine = hashFromHostnameToCommandLine.value(model->hostname());

                            if (listOfCommandLine.contains(model->commandLine())) {
                                alreadySameClone = true;
                            }
                        }

                        if (!alreadySameClone)
                        {
                            qDebug() << agent->name() << model->hostname() << model->commandLine();

                            listOfCommandLine.append(model->commandLine());
                            hashFromHostnameToCommandLine.insert(model->hostname(), listOfCommandLine);

                            QJsonObject jsonClone = QJsonObject();
                            jsonClone.insert("hostname", model->hostname());
                            jsonClone.insert("commandLine", model->commandLine());

                            jsonClones.append(jsonClone);
                        }
                    }
                }

                jsonAgent.insert("clones", jsonClones);

                jsonArray.append(jsonAgent);
            }
        }
    }
    return jsonArray;
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


                        // Manage the new agent model
                        QString hostname = model->hostname();

                        // Hostname is not defined
                        // There is already an existing (fake) model of agent (in the VM agentUsingSameDefinition)
                        if (hostname == HOSTNAME_NOT_DEFINED)
                        {
                            // Delete this new (fake) model of agent
                            _modelManager->deleteAgentModel(model);
                        }
                        // Hostname is a real one
                        else
                        {
                            // Get the list of agent models on the same host
                            QList<AgentM*> modelsOnHost = agentUsingSameDefinition->getModelsOnHost(hostname);

                            // There is NO agent on this host yet
                            if (modelsOnHost.isEmpty())
                            {
                                // Add the model of agent to the list of the VM
                                agentUsingSameDefinition->models()->append(model);

                                qDebug() << "Add model of agent" << model->name() << "on" << hostname;
                            }
                            // There is already agent models on this host
                            else
                            {
                                // Peer id is empty (the agent has never appeared on the network)
                                if (model->peerId().isEmpty())
                                {
                                    // Add the model of agent to the list of the VM
                                    agentUsingSameDefinition->models()->append(model);

                                    qDebug() << "Add model of agent" << model->name() << "on" << hostname;
                                }
                                // Peer id is defined: check if it is an agent that evolve from OFF to ON
                                else
                                {
                                    for (AgentM* iterator : modelsOnHost)
                                    {
                                        // Peer id is defined and agent is OFF --> we consider that it is the same model
                                        if ((iterator != NULL) && !iterator->peerId().isEmpty() && !iterator->isON())
                                        {
                                            int index = agentUsingSameDefinition->models()->indexOf(iterator);
                                            if (index > -1)
                                            {
                                                // Emit signal "Identical Agent Model Replaced"
                                                Q_EMIT identicalAgentModelReplaced(iterator, model);

                                                // Replace the model
                                                agentUsingSameDefinition->models()->replace(index, model);

                                                qDebug() << "Replace model of agent" << model->name() << "on" << hostname << "(" << iterator->peerId() << "-->" << model->peerId() << ")";

                                                // Delete the previous model of agent
                                                _modelManager->deleteAgentModel(iterator);
                                            }
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        // 1- View model never yet appeared on the network
                        /*if (agentUsingSameDefinition->neverAppearedOnNetwork())
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

                            if (!isSameModel)
                            {
                                if (!model->hostname().isEmpty())
                                {
                                    // 2.2- Add the model of agent to the list of the VM
                                    agentUsingSameDefinition->models()->append(model);

                                    qDebug() << "Add model of agent" << model->name() << "on" << model->address();
                                }
                            }
                        }*/
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
