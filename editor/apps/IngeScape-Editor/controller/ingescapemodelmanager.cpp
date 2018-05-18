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

#include "ingescapemodelmanager.h"

#include <QQmlEngine>
#include <QDebug>
#include <QFileDialog>

#include <I2Quick.h>

#include "controller/ingescapelaunchermanager.h"


/**
 * @brief Default constructor
 * @param agentsListDirectoryPath
 * @param agentsMappingsDirectoryPath
 * @param dataDirectoryPath
 * @param parent
 */
IngeScapeModelManager::IngeScapeModelManager(QString agentsListDirectoryPath,
                                       QString agentsMappingsDirectoryPath,
                                       QString dataDirectoryPath,
                                       QObject *parent) : QObject(parent),
    _isMappingActivated(false),
    _isMappingControlled(false),
    _agentsListDirectoryPath(agentsListDirectoryPath),
    _agentsMappingsDirectoryPath(agentsMappingsDirectoryPath),
    _dataDirectoryPath(dataDirectoryPath),
    _jsonHelper(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New INGESCAPE Model Manager";

    QDate today = QDate::currentDate();

    _agentsListDefaultFilePath = QString("%1agents_list_%2.json").arg(_agentsListDirectoryPath, today.toString("yyyyMMdd"));
    _agentsMappingsDefaultFilePath = QString("%1agents_mappings_%2.json").arg(_agentsMappingsDirectoryPath, today.toString("yyyyMMdd"));

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);
}


/**
 * @brief Destructor
 */
IngeScapeModelManager::~IngeScapeModelManager()
{
    qInfo() << "Delete INGESCAPE Model Manager";

    // Clear all opened definitions
    _openedDefinitions.clear();

    // Free memory
    _publishedValues.deleteAllItems();

    // Delete json helper
    if(_jsonHelper != NULL)
    {
        delete _jsonHelper;
        _jsonHelper = NULL;
    }
}


/**
 * @brief Setter for property "is Activated Mapping"
 * @param value
 */
void IngeScapeModelManager::setisMappingActivated(bool value)
{
    if (_isMappingActivated != value)
    {
        _isMappingActivated = value;

        if (_isMappingActivated) {
            qInfo() << "Mapping Activated";
        }
        else {
            qInfo() << "Mapping DE-activated";
        }

        Q_EMIT isMappingActivatedChanged(value);
    }
}


/**
 * @brief Setter for property "is Controlled Mapping"
 * @param value
 */
void IngeScapeModelManager::setisMappingControlled(bool value)
{
    if (_isMappingControlled != value)
    {
        _isMappingControlled = value;

        if (_isMappingControlled) {
            qInfo() << "Mapping Controlled";
        }
        else {
            qInfo() << "Mapping Observed";
        }

        Q_EMIT isMappingControlledChanged(value);
    }
}


/**
 * @brief Import the agents list from default file
 */
void IngeScapeModelManager::importAgentsListFromDefaultFile()
{
    // Import the agents list from JSON file
    _importAgentsListFromFile(_agentsListDefaultFilePath);
}


/**
 * @brief Import an agents list from selected file
 */
void IngeScapeModelManager::importAgentsListFromSelectedFile()
{
    // "File Dialog" to get the file (path) to open
    QString agentsListFilePath = QFileDialog::getOpenFileName(NULL,
                                                              "Open agents",
                                                              _agentsListDirectoryPath,
                                                              "JSON (*.json)");

    if (!agentsListFilePath.isEmpty())
    {
        // Import the agents list from JSON file
        _importAgentsListFromFile(agentsListFilePath);
    }
}


/**
 * @brief Import an agent or an agents list from selected file (definition)
 */
bool IngeScapeModelManager::importAgentOrAgentsListFromSelectedFile()
{
    bool success = true;

    // "File Dialog" to get the file (path) to open
    QString agentFilePath = QFileDialog::getOpenFileName(NULL,
                                                         "Open an agent(s) definition",
                                                         _dataDirectoryPath,
                                                         "JSON (*.json)");

    if (!agentFilePath.isEmpty())
    {
        QFile jsonFile(agentFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            // One JSON object
            if (jsonDocument.isObject())
            {
                // Create a model of agent definition with JSON
                DefinitionM* agentDefinition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
                if (agentDefinition != NULL)
                {
                    QString agentName = agentDefinition->name();

                    // Create a new model of agent with the name of the definition
                    AgentM* agent = new AgentM(agentName, this);

                    // Add this new model of agent
                    addAgentModel(agent);

                    // Add this new model of agent definition for the agent name
                    addAgentDefinitionForAgentName(agentDefinition, agentName);

                    // Set its definition
                    agent->setdefinition(agentDefinition);
                }
                // An error occured, the definition is NULL
                else {
                    qWarning() << "The file" << agentFilePath << "does not contain an agent definition !";
                    success = false;
                }
            }
            // Several JSON objects
            else if (jsonDocument.isArray())
            {
                // Import the agents list from JSON file
                _importAgentsListFromFile(agentFilePath);
            }
        }
        else {
            qCritical() << "Can not open file" << agentFilePath;
            success = false;
        }
    }

    return success;
}


/**
 * @brief Export the agents list to default file
 * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
 */
void IngeScapeModelManager::exportAgentsListToDefaultFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport)
{
    // Export the agents list to JSON file
    _exportAgentsListToFile(agentsListToExport, _agentsListDefaultFilePath);
}


/**
 * @brief Export the agents list to selected file
 * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
 */
void IngeScapeModelManager::exportAgentsListToSelectedFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport)
{
    // "File Dialog" to get the file (path) to save
    QString agentsListFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save agents",
                                                              _agentsListDirectoryPath,
                                                              "JSON (*.json)");

    if(!agentsListFilePath.isEmpty()) {
        // Export the agents list to JSON file
        _exportAgentsListToFile(agentsListToExport, agentsListFilePath);
    }
}


/**
 * @brief Get the JSON of a mapping
 * @param agentMapping
 * @return
 */
QString IngeScapeModelManager::getJsonOfMapping(AgentMappingM* agentMapping)
{
    if (_jsonHelper != NULL) {
        return _jsonHelper->getJsonOfMapping(agentMapping);
    }
    else {
        return "";
    }
}


/**
 * @brief Simulate an exit for each agent
 */
void IngeScapeModelManager::simulateExitForEachActiveAgent()
{
    for (AgentM* agent : _mapFromPeerIdToAgentM.values())
    {
        if (agent != NULL) {
            onAgentExited(agent->peerId(), agent->name());
        }
    }
}


/**
 * @brief Slot called when an agent enter the network
 * @param peerId
 * @param agentName
 * @param agentAddress
 * @param pid
 * @param hostname
 * @param commandLine
 * @param canBeFrozen
 */
void IngeScapeModelManager::onAgentEntered(QString peerId, QString agentName, QString agentAddress, int pid, QString hostname, QString commandLine, bool canBeFrozen, bool isRecorder)
{
    if (!peerId.isEmpty() && !agentName.isEmpty() && !agentAddress.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);

        // An agent with this peer id already exist
        if (agent != NULL)
        {
            qInfo() << "The agent" << agentName << "with peer id" << peerId << "and address" << agentAddress << "is back on the network !";

            // Update the state (flag "is ON")
            agent->setisON(true);
        }
        // New peer id
        else
        {
            // Create a new model of agent
            agent = new AgentM(agentName, peerId, agentAddress, this);

            agent->sethostname(hostname);
            agent->setcommandLine(commandLine);
            agent->setisRecorder(isRecorder);

            if (!hostname.isEmpty() && !isRecorder)
            {
                HostM* host = IngeScapeLauncherManager::Instance().getHostWithName(hostname);
                if (host != NULL)
                {
                    // Add this agent to the host
                    //host->agents()->append(agent);

                    if (!commandLine.isEmpty()) {
                        agent->setcanBeRestarted(true);
                    }
                }
            }

            agent->setpid(pid);
            agent->setcanBeFrozen(canBeFrozen);

            // Update the state (flag "is ON")
            agent->setisON(true);

            // Add this new model of agent
            addAgentModel(agent);
        }
    }
}


/**
 * @brief Slot called when an agent quit the network
 * @param peer Id
 * @param agent name
 */
void IngeScapeModelManager::onAgentExited(QString peerId, QString agentName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL)
    {
        qInfo() << "The agent" << agentName << "with peer id" << peerId << "exited from the network !";

        // Update the state (flag "is ON")
        agent->setisON(false);

        if (agent->definition() != NULL) {
            // Emit the signal "Remove Inputs to Editor for Outputs"
            Q_EMIT removeInputsToEditorForOutputs(agentName, agent->definition()->outputsList()->toList());
        }
    }
}


/**
 * @brief Slot called when a launcher enter the network
 * @param peerId
 * @param hostname
 * @param ipAddress
 */
void IngeScapeModelManager::onLauncherEntered(QString peerId, QString hostname, QString ipAddress, QString streamingPort)
{
    // Add a IngeScape Launcher to the manager
    IngeScapeLauncherManager::Instance().addIngeScapeLauncher(peerId, hostname, ipAddress, streamingPort);

    // Traverse the list of all agents
    foreach (QString agentName, _mapFromNameToAgentModelsList.keys())
    {
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);

        foreach (AgentM* agent, agentModelsList)
        {
            if ((agent != NULL) && (agent->hostname() == hostname) && !agent->commandLine().isEmpty()) {
                agent->setcanBeRestarted(true);
            }
        }
    }
}


/**
 * @brief Slot called when a launcher quit the network
 * @param peerId
 * @param hostname
 */
void IngeScapeModelManager::onLauncherExited(QString peerId, QString hostname)
{
    // Remove a IngeScape Launcher to the manager
    IngeScapeLauncherManager::Instance().removeIngeScapeLauncher(peerId, hostname);

    // Traverse the list of all agents
    foreach (QString agentName, _mapFromNameToAgentModelsList.keys())
    {
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);

        foreach (AgentM* agent, agentModelsList)
        {
            if ((agent != NULL) && (agent->hostname() == hostname)) {
                agent->setcanBeRestarted(false);
            }
        }
    }
}


/**
 * @brief Slot called when an agent definition has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param definition in JSON format
 */
void IngeScapeModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
{
    if (!definitionJSON.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if (agent != NULL)
        {
            QByteArray byteArrayOfJson = definitionJSON.toUtf8();

            // Create a model of agent definition with JSON
            DefinitionM* agentDefinition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
            if (agentDefinition != NULL)
            {
                 if (agent->definition() == NULL)
                 {
                     // Add this new model of agent definition for the agent name
                     addAgentDefinitionForAgentName(agentDefinition, agentName);

                     // Set this definition to the agent
                     agent->setdefinition(agentDefinition);

                     // Emit the signal "Add Inputs to Editor for Outputs"
                     Q_EMIT addInputsToEditorForOutputs(agentName, agentDefinition->outputsList()->toList());

                     // Emit the signal "Active Agent Defined"
                     Q_EMIT activeAgentDefined(agent);
                 }
                 // Update with the new definition
                 else
                 {
                     DefinitionM* previousDefinition = agent->definition();

                     // Add this new model of agent definition for the agent name
                     addAgentDefinitionForAgentName(agentDefinition, agentName);

                     // Set this definition to the agent
                     agent->setdefinition(agentDefinition);

                     // Emit the signal "Add Inputs to Editor for Outputs"
                     Q_EMIT addInputsToEditorForOutputs(agentName, agentDefinition->outputsList()->toList());

                     if (previousDefinition != NULL) {
                         deleteAgentDefinition(previousDefinition);
                     }
                 }
            }
        }
    }
}


/**
 * @brief Slot called when an agent mapping has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param mapping in JSON format
 */
void IngeScapeModelManager::onMappingReceived(QString peerId, QString agentName, QString mappingJSON)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL)
    {
        AgentMappingM* agentMapping = NULL;

        if (mappingJSON.isEmpty())
        {
            QString mappingName = QString("EMPTY MAPPING of %1").arg(agentName);
            agentMapping = new AgentMappingM(mappingName, "", "");
        }
        else
        {
            // Create a model of agent mapping with JSON
            agentMapping = _jsonHelper->createModelOfAgentMapping(agentName, mappingJSON.toUtf8());
        }

        if (agentMapping != NULL)
        {
            if (agent->mapping() == NULL)
            {
                // Add this new model of agent mapping
                addAgentMappingForAgentName(agentMapping, agentName);

                // Set this mapping to the agent
                agent->setmapping(agentMapping);

                // Emit the signal "Active Agent Mapping Defined"
                Q_EMIT activeAgentMappingDefined(agent);
            }
            // There is already a mapping for this agent
            else
            {
                qWarning() << "Update the mapping of agent" << agentName << "(if this mapping has changed)";

                AgentMappingM* previousMapping = agent->mapping();

                QStringList idsOfRemovedMappingElements;
                foreach (QString idPreviousList, previousMapping->idsOfMappingElements()) {
                    if (!agentMapping->idsOfMappingElements().contains(idPreviousList)) {
                        idsOfRemovedMappingElements.append(idPreviousList);
                    }
                }

                QStringList idsOfAddedMappingElements;
                foreach (QString idNewList, agentMapping->idsOfMappingElements()) {
                    if (!previousMapping->idsOfMappingElements().contains(idNewList)) {
                        idsOfAddedMappingElements.append(idNewList);
                    }
                }

                // If there are some Removed mapping elements
                if (idsOfRemovedMappingElements.count() > 0)
                {
                    //qDebug() << "unmapped" << idsOfRemovedMappingElements;

                    foreach (ElementMappingM* mappingElement, previousMapping->mappingElements()->toList()) {
                        if ((mappingElement != NULL) && idsOfRemovedMappingElements.contains(mappingElement->id()))
                        {
                            // Emit the signal "UN-mapped"
                            Q_EMIT unmapped(mappingElement);
                        }
                    }
                }
                // If there are some Added mapping elements
                if (idsOfAddedMappingElements.count() > 0)
                {
                    //qDebug() << "mapped" << idsOfAddedMappingElements;

                    foreach (ElementMappingM* mappingElement, agentMapping->mappingElements()->toList()) {
                        if ((mappingElement != NULL) && idsOfAddedMappingElements.contains(mappingElement->id()))
                        {
                            // Emit the signal "Mapped"
                            Q_EMIT mapped(mappingElement);
                        }
                    }
                }

                // Add a model of agent mapping for an agent name
                addAgentMappingForAgentName(agentMapping, agentName);

                // Set this new mapping to the agent
                agent->setmapping(agentMapping);

                // Delete a model of agent mapping
                deleteAgentMapping(previousMapping);
            }
        }
    }
}


/**
 * @brief Occurs when records from DB have been received
 * @param mappingJSON
 */
void IngeScapeModelManager::onAllRecordsReceived(QString recordsJSON)
{
    if (!recordsJSON.isEmpty())
    {
        QByteArray byteArrayOfJson = recordsJSON.toUtf8();
        _recordsList = _jsonHelper->createRecordModelList(byteArrayOfJson);

        Q_EMIT recordsListChanged(_recordsList);
    }
}

/**
 * @brief Occurs when records from DB have been received
 * @param mappingJSON
 */
void IngeScapeModelManager::onNewRecordReceived(QString recordJSON)
{
    if (!recordJSON.isEmpty())
    {
        QByteArray byteArrayOfJson = recordJSON.toUtf8();
        QList<RecordM*> tmpRecords = _jsonHelper->createRecordModelList(byteArrayOfJson);

        if(tmpRecords.count() > 0)
        {
            RecordM* newRecord = tmpRecords.at(0);
            _recordsList.insert(0, newRecord);
            Q_EMIT recordAdded(newRecord);
        }
    }
}



/**
 * @brief Slot called when a new value is published
 * @param publishedValue
 */
void IngeScapeModelManager::onValuePublished(PublishedValueM* publishedValue)
{
    if (publishedValue != NULL)
    {
        // Add to the list at the first position
        _publishedValues.prepend(publishedValue);

        QList<AgentM*> agentsList = getAgentModelsListFromName(publishedValue->agentName());
        foreach (AgentM* agent, agentsList)
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
 * @brief Slot called when the flag "is Muted" from an agent updated
 * @param peerId
 * @param isMuted
 */
void IngeScapeModelManager::onisMutedFromAgentUpdated(QString peerId, bool isMuted)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL) {
        agent->setisMuted(isMuted);
    }
}


/**
 * @brief Slot called when the flag "is Frozen" from an agent updated
 * @param peerId
 * @param isFrozen
 */
void IngeScapeModelManager::onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL) {
        agent->setisFrozen(isFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Muted" from an output of agent updated
 * @param peerId
 * @param isMuted
 * @param outputName
 */
void IngeScapeModelManager::onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL) {
        agent->setisMutedOfOutput(isMuted, outputName);
    }
}

/**
 * @brief Slot called when the state of an agent changes
 * @param peerId
 * @param stateName
 */
void IngeScapeModelManager::onAgentStateChanged(QString peerId, QString stateName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL) {
        agent->setstate(stateName);
    }
}


/**
 * @brief Add a model of agent
 * @param agent
 */
void IngeScapeModelManager::addAgentModel(AgentM* agent)
{
    if (agent != NULL)
    {
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agent->name());
        agentModelsList.append(agent);

        // Update the list in the map
        _mapFromNameToAgentModelsList.insert(agent->name(), agentModelsList);

        if (!agent->peerId().isEmpty()) {
            _mapFromPeerIdToAgentM.insert(agent->peerId(), agent);
        }

        // Emit the signal "Agent Model Created"
        Q_EMIT agentModelCreated(agent);

        //_printAgents();
    }
}


/**
 * @brief Get the model of agent from a Peer Id
 * @param peerId
 * @return
 */
AgentM* IngeScapeModelManager::getAgentModelFromPeerId(QString peerId)
{
    if (_mapFromPeerIdToAgentM.contains(peerId)) {
        return _mapFromPeerIdToAgentM.value(peerId);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Get the list of models of agent from a name
 * @param name
 * @return
 */
QList<AgentM*> IngeScapeModelManager::getAgentModelsListFromName(QString name)
{
    if (_mapFromNameToAgentModelsList.contains(name)) {
        return _mapFromNameToAgentModelsList.value(name);
    }
    else {
        return QList<AgentM*>();
    }
}


/**
 * @brief Get the map from agent name to list of active agents
 * @return
 */
QHash<QString, QList<AgentM*>> IngeScapeModelManager::getMapFromAgentNameToActiveAgentsList()
{
    QHash<QString, QList<AgentM*>> mapFromAgentNameToActiveAgentsList;

    foreach (QString agentName, _mapFromNameToAgentModelsList.keys())
    {
        QList<AgentM*> allAgentsList = getAgentModelsListFromName(agentName);
        QList<AgentM*> activeAgentsList;
        foreach (AgentM* agent, allAgentsList) {
            if ((agent != NULL) && agent->isON()) {
                activeAgentsList.append(agent);
            }
        }
        if (activeAgentsList.count() > 0) {
            mapFromAgentNameToActiveAgentsList.insert(agentName, activeAgentsList);
        }
    }

    return mapFromAgentNameToActiveAgentsList;
}


/**
 * @brief Delete a model of Agent
 * @param agent
 */
void IngeScapeModelManager::deleteAgentModel(AgentM* agent)
{
    if (agent != NULL)
    {
        // Emit the signal "Agent Model Will Be Deleted"
        Q_EMIT agentModelWillBeDeleted(agent);

        // Delete its model of definition if needed
        if (agent->definition() != NULL) {
            DefinitionM* temp = agent->definition();
            agent->setdefinition(NULL);
            deleteAgentDefinition(temp);
        }

        // Delete its model of mapping if needed
        if (agent->mapping() != NULL) {
            AgentMappingM* temp = agent->mapping();
            agent->setmapping(NULL);
            deleteAgentMapping(temp);
        }

        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agent->name());
        agentModelsList.removeOne(agent);

        // Update the list in the map
        _mapFromNameToAgentModelsList.insert(agent->name(), agentModelsList);

        if (!agent->peerId().isEmpty()) {
            _mapFromPeerIdToAgentM.remove(agent->peerId());
        }

        // Free memory
        delete agent;

        //_printAgents();
    }
}

/**
 * @brief Delete a model of Record
 * @param agent
 */
void IngeScapeModelManager::deleteRecordModel(RecordM* record)
{
    if (record != NULL)
    {
        int indexOfRecord = _recordsList.indexOf(record);
        if(indexOfRecord != -1)
            _recordsList.removeAt(indexOfRecord);

        // Free memory
        delete record;
    }
}

/**
 * @brief Add a model of agent definition for an agent name
 * @param agentDefinition
 * @param agentName
 */
void IngeScapeModelManager::addAgentDefinitionForAgentName(DefinitionM* agentDefinition, QString agentName)
{
    Q_UNUSED(agentName)

    if (agentDefinition != NULL)
    {
        QString definitionName = agentDefinition->name();

        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definitionName);
        agentDefinitionsList.append(agentDefinition);

        // Update the list in the map
        _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

        //_printDefinitions();

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(definitionName);
    }
}


/**
 * @brief Get the list (of models) of agent definition from a definition name
 * @param name
 * @return
 */
QList<DefinitionM*> IngeScapeModelManager::getAgentDefinitionsListFromDefinitionName(QString definitionName)
{
    if (_mapFromNameToAgentDefinitionsList.contains(definitionName)) {
        return _mapFromNameToAgentDefinitionsList.value(definitionName);
    }
    else {
        return QList<DefinitionM*>();
    }
}


/**
 * @brief Delete a model of agent definition
 * @param definition
 */
void IngeScapeModelManager::deleteAgentDefinition(DefinitionM* definition)
{
    if (definition != NULL)
    {
        QString definitionName = definition->name();

        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definitionName);
        agentDefinitionsList.removeOne(definition);

        // Update the list in the map
        _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

        // Free memory
        delete definition;

        //_printDefinitions();

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(definitionName);
    }
}


/**
 * @brief Add a model of agent mapping for an agent name
 * @param agentMapping
 * @param agentName
 */
void IngeScapeModelManager::addAgentMappingForAgentName(AgentMappingM* agentMapping, QString agentName)
{
    Q_UNUSED(agentName)

    if (agentMapping != NULL)
    {
        QString mappingName = agentMapping->name();

        QList<AgentMappingM*> agentMappingsList = getAgentMappingsListFromMappingName(mappingName);
        agentMappingsList.append(agentMapping);

        // Update the list in the map
        _mapFromNameToAgentMappingsList.insert(mappingName, agentMappingsList);

        //_printMappings();
    }
}


/**
 * @brief Get the list (of models) of agent mapping from a mapping name
 * @param mappingName
 * @return
 */
QList<AgentMappingM*> IngeScapeModelManager::getAgentMappingsListFromMappingName(QString mappingName)
{
    if (_mapFromNameToAgentMappingsList.contains(mappingName)) {
        return _mapFromNameToAgentMappingsList.value(mappingName);
    }
    else {
        return QList<AgentMappingM*>();
    }
}


/**
 * @brief Delete a model of agent mapping
 * @param agentMapping
 */
void IngeScapeModelManager::deleteAgentMapping(AgentMappingM* agentMapping)
{
    if (agentMapping != NULL)
    {
        QString mappingName = agentMapping->name();

        QList<AgentMappingM*> agentMappingsList = getAgentMappingsListFromMappingName(mappingName);
        agentMappingsList.removeOne(agentMapping);

        // Update the list in the map
        _mapFromNameToAgentMappingsList.insert(mappingName, agentMappingsList);

        // Free memory
        delete agentMapping;

        //_printMappings();
    }
}


/**
 * @brief Import the agents list from JSON file
 * @param agentsListFilePath
 */
void IngeScapeModelManager::_importAgentsListFromFile(QString agentsListFilePath)
{
    if (!agentsListFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Import the agents list from JSON file" << agentsListFilePath;

        QFile jsonFile(agentsListFilePath);
        if (jsonFile.exists()) {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                // Initialize agents list from JSON file
                QList<QPair<QStringList, DefinitionM*>> agentsListToImport = _jsonHelper->initAgentsList(byteArrayOfJson);

                for (int i = 0; i < agentsListToImport.count(); i++)
                {
                    QPair<QStringList, DefinitionM*> pair = agentsListToImport.at(i);
                    QStringList agentNameAndParametersToRestart = pair.first;
                    DefinitionM* agentDefinition = pair.second;

                    QString agentName = agentNameAndParametersToRestart.first();

                    if (!agentName.isEmpty() && (agentDefinition != NULL))
                    {
                        // Create a new model of agent with the name
                        AgentM* agent = new AgentM(agentName, this);

                        if (agentNameAndParametersToRestart.count() == 3)
                        {
                            QString hostname = agentNameAndParametersToRestart.at(1);
                            QString commandLine = agentNameAndParametersToRestart.at(2);

                            agent->sethostname(hostname);
                            agent->setcommandLine(commandLine);

                            if (!hostname.isEmpty())
                            {
                                HostM* host = IngeScapeLauncherManager::Instance().getHostWithName(hostname);
                                if (host != NULL)
                                {
                                    // Add this agent to the host
                                    //host->agents()->append(agent);

                                    if (!commandLine.isEmpty()) {
                                        agent->setcanBeRestarted(true);
                                    }
                                }
                            }
                        }

                        // Add this new model of agent
                        addAgentModel(agent);

                        // Add this new model of agent definition for the agent name
                        addAgentDefinitionForAgentName(agentDefinition, agentName);

                        // Set its definition
                        agent->setdefinition(agentDefinition);
                    }
                }
            }
            else {
                qCritical() << "Can not open file" << agentsListFilePath;
            }
        }
        else {
            qWarning() << "There is no file" << agentsListFilePath;
        }
    }
}


/**
 * @brief Export the agents list to JSON file
 * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
 * @param agentsListFilePath
 */
void IngeScapeModelManager::_exportAgentsListToFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport, QString agentsListFilePath)
{
    if (!agentsListFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Save the agents list to JSON file" << agentsListFilePath;

        // Export the agents list
        QByteArray byteArrayOfJson = _jsonHelper->exportAgentsList(agentsListToExport);

        QFile jsonFile(agentsListFilePath);
        if (jsonFile.open(QIODevice::WriteOnly))
        {
            jsonFile.write(byteArrayOfJson);
            jsonFile.close();
        }
        else {
            qCritical() << "Can not open file" << agentsListFilePath;
        }
    }
}


/**
 * @brief Update definition variants of the list of definitions with the same name
 * @param definitionName
 */
void IngeScapeModelManager::_updateDefinitionVariants(QString definitionName)
{
    QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definitionName);

    //qDebug() << "Update Definition Variants for definition name" << definitionName << "(" << agentDefinitionsList.count() << "definitions)";

    // We can use versions as keys of the map because the list contains only definition with the same name
    QHash<QString, QList<DefinitionM*>> mapFromVersionToDefinitionsList;
    QList<QString> versionsWithVariant;

    foreach (DefinitionM* iterator, agentDefinitionsList)
    {
        if ((iterator != NULL) && !iterator->version().isEmpty())
        {
            // First, reset all
            iterator->setisVariant(false);

            QString version = iterator->version();
            QList<DefinitionM*> definitionsListForVersion;

            // Other(s) definition(s) have the same version (and the same name)
            if (mapFromVersionToDefinitionsList.contains(version)) {
                definitionsListForVersion = mapFromVersionToDefinitionsList.value(version);

                // The lists of I/O/P must be different to have a variant !
                if (!versionsWithVariant.contains(version))
                {
                    // We compare I/O/P between current iterator and the first one
                    DefinitionM* first = definitionsListForVersion.first();
                    if ((first != NULL) && !DefinitionM::areIdenticals(first, iterator)) {
                        versionsWithVariant.append(version);
                    }
                }
            }

            definitionsListForVersion.append(iterator);
            mapFromVersionToDefinitionsList.insert(version, definitionsListForVersion);
        }
    }

    // The list contains only the versions that have variants
    foreach (QString version, versionsWithVariant)
    {
        QList<DefinitionM*> definitionsListForVersion = mapFromVersionToDefinitionsList.value(version);
        foreach (DefinitionM* iterator, definitionsListForVersion)
        {
            if (iterator != NULL) {
                iterator->setisVariant(true);
                //qDebug() << iterator->name() << iterator->version() << "is variant";
            }
        }
    }
}


/**
 * @brief Print all models of agents (for Debug)
 */
void IngeScapeModelManager::_printAgents()
{
    qDebug() << "Print Agents:";
    foreach (QString agentName, _mapFromNameToAgentModelsList.keys()) {
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);
        qDebug() << agentName << ":" << agentModelsList.count() << "agents";
    }
}


/**
 * @brief Print all models of agent definitions (for Debug)
 */
void IngeScapeModelManager::_printDefinitions()
{
    qDebug() << "Print Definitions:";
    foreach (QString definitionName, _mapFromNameToAgentDefinitionsList.keys()) {
        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definitionName);
        qDebug() << definitionName << ":" << agentDefinitionsList.count() << "definitions";
    }
}


/**
 * @brief Print all models of agent mappings (for Debug)
 */
void IngeScapeModelManager::_printMappings()
{
    qDebug() << "Print Mappings:";
    foreach (QString mappingName, _mapFromNameToAgentMappingsList.keys()) {
        QList<AgentMappingM*> agentMappingsList = getAgentMappingsListFromMappingName(mappingName);
        qDebug() << mappingName << ":" << agentMappingsList.count() << "mappings";
    }
}
