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

#include "masticmodelmanager.h"

#include <QQmlEngine>
#include <QDebug>
#include <QFileDialog>

#include <I2Quick.h>

/**
 * @brief Default constructor
 * @param agentsListDirectoryPath
 * @param agentsMappingsDirectoryPath
 * @param dataDirectoryPath
 * @param parent
 */
MasticModelManager::MasticModelManager(QString agentsListDirectoryPath,
                                       QString agentsMappingsDirectoryPath,
                                       QString dataDirectoryPath,
                                       QObject *parent) : QObject(parent),
    _isActivatedMapping(false),
    _agentsListDirectoryPath(agentsListDirectoryPath),
    _agentsMappingsDirectoryPath(agentsMappingsDirectoryPath),
    _dataDirectoryPath(dataDirectoryPath),
    _jsonHelper(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New MASTIC Model Manager";

    QDate today = QDate::currentDate();

    _agentsListDefaultFilePath = QString("%1agents_list_%2.json").arg(_agentsListDirectoryPath, today.toString("yyyyMMdd"));
    _agentsMappingsDefaultFilePath = QString("%1agents_mappings_%2.json").arg(_agentsMappingsDirectoryPath, today.toString("yyyyMMdd"));

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);
}


/**
 * @brief Destructor
 */
MasticModelManager::~MasticModelManager()
{
    qInfo() << "Delete MASTIC Model Manager";

    // Clear all opened definitions
    _openedDefinitions.clear();

    // Free memory
    _publishedValues.deleteAllItems();
}


/**
 * @brief Import the agents list from default file
 */
void MasticModelManager::importAgentsListFromDefaultFile()
{
    // Import the agents list from JSON file
    _importAgentsListFromFile(_agentsListDefaultFilePath);
}


/**
 * @brief Import an agents list from selected file
 */
void MasticModelManager::importAgentsListFromSelectedFile()
{
    // "File Dialog" to get the file (path) to open
    QString agentsListFilePath = QFileDialog::getOpenFileName(NULL,
                                                              "Importer un fichier JSON avec une liste d'agents",
                                                              _agentsListDirectoryPath,
                                                              "JSON (*.json)");

    if(!agentsListFilePath.isEmpty()) {
        // Import the agents list from JSON file
        _importAgentsListFromFile(agentsListFilePath);
    }
}


/**
 * @brief Import an agent from selected files (definition and mapping)
 */
void MasticModelManager::importAgentFromSelectedFiles()
{
    // "File Dialog" to get the files (paths) to open
    QStringList agentFilesPaths = QFileDialog::getOpenFileNames(NULL,
                                                                "Importer le fichier de définition (et de mapping) d'un agent",
                                                                _dataDirectoryPath,
                                                                "JSON (*.json)");

    // Import the agent from JSON files (definition and mapping)
    _importAgentFromFiles(agentFilesPaths);
}


/**
 * @brief Export the agents list to default file
 * @param agentsListToExport list of pairs <agent name, definition>
 */
void MasticModelManager::exportAgentsListToDefaultFile(QList<QPair<QString, DefinitionM*>> agentsListToExport)
{
    // Export the agents list to JSON file
    _exportAgentsListToFile(agentsListToExport, _agentsListDefaultFilePath);
}


/**
 * @brief Export the agents list to selected file
 * @param agentsListToExport list of pairs <agent name, definition>
 */
void MasticModelManager::exportAgentsListToSelectedFile(QList<QPair<QString, DefinitionM*>> agentsListToExport)
{
    // "File Dialog" to get the file (path) to save
    QString agentsListFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Sauvegarder dans un fichier JSON la liste d'agents",
                                                              _agentsListDirectoryPath,
                                                              "JSON (*.json)");

    if(!agentsListFilePath.isEmpty()) {
        // Export the agents list to JSON file
        _exportAgentsListToFile(agentsListToExport, agentsListFilePath);
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
void MasticModelManager::onAgentEntered(QString peerId, QString agentName, QString agentAddress, int pid, QString hostname, QString commandLine, bool canBeFrozen)
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
 * @brief Slot called when an agent definition has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param definition in JSON format
 */
void MasticModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
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
                 }
                 else {
                     // FIXME TODO
                     qWarning() << "Update the definition of agent" << agentName << "(if this definition has changed)";
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
void MasticModelManager::onMappingReceived(QString peerId, QString agentName, QString mappingJSON)
{
    if (!mappingJSON.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if (agent != NULL)
        {
            QByteArray byteArrayOfJson = mappingJSON.toUtf8();

            // Create a model of agent mapping with JSON
            AgentMappingM* agentMapping = _jsonHelper->createModelOfAgentMapping(agentName, byteArrayOfJson);
            if (agentMapping != NULL)
            {
                if (agent->mapping() == NULL)
                {
                    // Add this new model of agent mapping
                    addAgentMappingForAgentName(agentMapping, agentName);

                    // Update the merged list of mapping elements for the agent name
                    _updateMergedListsOfMappingElementsForAgentName(agentName, agentMapping);

                    // Set this mapping to the agent
                    agent->setmapping(agentMapping);
                }
                // There is already a mapping for this agent
                else
                {
                    qWarning() << "Update the mapping of agent" << agentName << "(if this mapping has changed)";

                    AgentMappingM* previousMapping = agent->mapping();

                    // Same name, version and description
                    if ((agentMapping->name() == previousMapping->name())
                            && (agentMapping->version() == previousMapping->version())
                            && (agentMapping->description() == previousMapping->description()))
                    {
                        //qDebug() << "Previous Mapping" << previousMapping->mappingElementsIds();
                        QStringList idsOfRemovedMappingElements;
                        foreach (QString idPreviousList, previousMapping->mappingElementsIds()) {
                            if (!agentMapping->mappingElementsIds().contains(idPreviousList)) {
                                idsOfRemovedMappingElements.append(idPreviousList);
                            }
                        }

                        //qDebug() << "New Mapping" << agentMapping->mappingElementsIds();
                        QStringList idsOfAddedMappingElements;
                        foreach (QString idNewList, agentMapping->mappingElementsIds()) {
                            if (!previousMapping->mappingElementsIds().contains(idNewList)) {
                                idsOfAddedMappingElements.append(idNewList);
                            }
                        }

                        // If there are some Removed mapping elements
                        if (idsOfRemovedMappingElements.count() > 0)
                        {
                            foreach (ElementMappingM* mappingElement, previousMapping->elementMappingsList()->toList()) {
                                if ((mappingElement != NULL) && idsOfRemovedMappingElements.contains(mappingElement->id())) {
                                    qDebug() << "Remove" << mappingElement->id();
                                }
                            }
                        }
                        // If there are some Added mapping elements
                        if (idsOfAddedMappingElements.count() > 0)
                        {
                            foreach (ElementMappingM* mappingElement, agentMapping->elementMappingsList()->toList()) {
                                if ((mappingElement != NULL) && idsOfAddedMappingElements.contains(mappingElement->id())) {
                                    qDebug() << "Add" << mappingElement->id();
                                }
                            }
                        }

                        // Add a model of agent mapping for an agent name
                        addAgentMappingForAgentName(agentMapping, agentName);

                        // Update the merged list of mapping elements for the agent name
                        //_updateMergedListsOfMappingElementsForAgentName(agentName, agentMapping);

                        // Set this new mapping to the agent
                        agent->setmapping(agentMapping);

                        // Delete a model of agent mapping
                        deleteAgentMapping(previousMapping);
                    }
                    else {
                        // FIXME TODO
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot called when an agent quit the network
 * @param peer Id
 * @param agent name
 */
void MasticModelManager::onAgentExited(QString peerId, QString agentName)
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

        // Check if all agents with this name are OFF
        bool allAgentsAreOFF = true;
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);
        foreach (AgentM* model, agentModelsList) {
            if (model->isON()) {
                allAgentsAreOFF = false;
                break;
            }
        }

        if (allAgentsAreOFF)
        {
            // Clean merged lists of mapping elements for the agent name
            _cleanMergedListsOfMappingElementsForAgentName(agentName);
        }
    }
}


/**
 * @brief Slot called when a new value is published
 * @param publishedValue
 */
void MasticModelManager::onValuePublished(PublishedValueM* publishedValue)
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
void MasticModelManager::onisMutedFromAgentUpdated(QString peerId, bool isMuted)
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
void MasticModelManager::onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen)
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
void MasticModelManager::onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL) {
        agent->setisMutedOfOutput(isMuted, outputName);
    }
}


/**
 * @brief Add a model of agent
 * @param agent
 */
void MasticModelManager::addAgentModel(AgentM* agent)
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

        _printAgents();
    }
}


/**
 * @brief Get the model of agent from a Peer Id
 * @param peerId
 * @return
 */
AgentM* MasticModelManager::getAgentModelFromPeerId(QString peerId)
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
QList<AgentM*> MasticModelManager::getAgentModelsListFromName(QString name)
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
QHash<QString, QList<AgentM*>> MasticModelManager::getMapFromAgentNameToActiveAgentsList()
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
void MasticModelManager::deleteAgentModel(AgentM* agent)
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

        _printAgents();
    }
}


/**
 * @brief Add a model of agent definition for an agent name
 * @param agentDefinition
 * @param agentName
 */
void MasticModelManager::addAgentDefinitionForAgentName(DefinitionM* agentDefinition, QString agentName)
{
    Q_UNUSED(agentName)

    if (agentDefinition != NULL)
    {
        QString definitionName = agentDefinition->name();

        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definitionName);
        agentDefinitionsList.append(agentDefinition);

        // Update the list in the map
        _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

        _printDefinitions();

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(definitionName);
    }
}


/**
 * @brief Get the list (of models) of agent definition from a definition name
 * @param name
 * @return
 */
QList<DefinitionM*> MasticModelManager::getAgentDefinitionsListFromDefinitionName(QString definitionName)
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
void MasticModelManager::deleteAgentDefinition(DefinitionM* definition)
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

        _printDefinitions();

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(definitionName);
    }
}


/**
 * @brief Add a model of agent mapping for an agent name
 * @param agentMapping
 * @param agentName
 */
void MasticModelManager::addAgentMappingForAgentName(AgentMappingM* agentMapping, QString agentName)
{
    Q_UNUSED(agentName)

    if (agentMapping != NULL)
    {
        QString mappingName = agentMapping->name();

        QList<AgentMappingM*> agentMappingsList = getAgentMappingsListFromMappingName(mappingName);
        agentMappingsList.append(agentMapping);

        // Update the list in the map
        _mapFromNameToAgentMappingsList.insert(mappingName, agentMappingsList);

        _printMappings();
    }
}


/**
 * @brief Get the list (of models) of agent mapping from a mapping name
 * @param mappingName
 * @return
 */
QList<AgentMappingM*> MasticModelManager::getAgentMappingsListFromMappingName(QString mappingName)
{
    if (_mapFromNameToAgentMappingsList.contains(mappingName)) {
        return _mapFromNameToAgentMappingsList.value(mappingName);
    }
    else {
        return QList<AgentMappingM*>();
    }
}


/**
 * @brief Get the merged list of all (models of) mapping elements which connect an input of the agent
 * @param agentName
 * @return
 */
QList<ElementMappingM*> MasticModelManager::getMergedListOfInputMappingElementsFromAgentName(QString agentName)
{
    if (_mapFromAgentNameToMergedListOfInputMappingElements.contains(agentName)) {
        return _mapFromAgentNameToMergedListOfInputMappingElements.value(agentName);
    }
    else {
        return QList<ElementMappingM*>();
    }
}


/**
 * @brief Get the merged list of all (models of) mapping elements which connect an output of the agent
 * @param agentName
 * @return
 */
QList<ElementMappingM*> MasticModelManager::getMergedListOfOutputMappingElementsFromAgentName(QString agentName)
{
    if (_mapFromAgentNameToMergedListOfOutputMappingElements.contains(agentName)) {
        return _mapFromAgentNameToMergedListOfOutputMappingElements.value(agentName);
    }
    else {
        return QList<ElementMappingM*>();
    }
}


/**
 * @brief Delete a model of agent mapping
 * @param agentMapping
 */
void MasticModelManager::deleteAgentMapping(AgentMappingM* agentMapping)
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

        _printMappings();
    }
}


/**
 * @brief Import the agents list from JSON file
 * @param agentsListFilePath
 */
void MasticModelManager::_importAgentsListFromFile(QString agentsListFilePath)
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
                QList<QPair<QString, DefinitionM*>> agentsListToImport = _jsonHelper->initAgentsList(byteArrayOfJson);

                for (int i = 0; i < agentsListToImport.count(); i++)
                {
                    QPair<QString, DefinitionM*> pair = agentsListToImport.at(i);
                    QString agentName = pair.first;
                    DefinitionM* agentDefinition = pair.second;

                    if (!agentName.isEmpty() && (agentDefinition != NULL))
                    {
                        // Create a new model of agent with the name
                        AgentM* agent = new AgentM(agentName, this);

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
 * @brief Import an agent from JSON files (definition and mapping)
 * @param subDirectoryPath
 */
void MasticModelManager::_importAgentFromFiles(QStringList agentFilesPaths)
{
    if ((agentFilesPaths.count() == 1) || (agentFilesPaths.count() == 2))
    {
        DefinitionM* agentDefinition = NULL;
        AgentMappingM* agentMapping = NULL;

        // Only 1 file, it must be the definition
        if (agentFilesPaths.count() == 1)
        {
            QString agentFilePath = agentFilesPaths.first();

            QFile jsonFile(agentFilePath);
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                // Create a model of agent definition with JSON
                agentDefinition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
            }
            else {
                qCritical() << "Can not open file" << agentFilePath;
            }
        }
        // 2 files, they must be the definition and the mapping
        else if (agentFilesPaths.count() == 2)
        {
            QString agentFilePath1 = agentFilesPaths.at(0);
            QString agentFilePath2 = agentFilesPaths.at(1);

            QFile jsonFile1(agentFilePath1);
            QFile jsonFile2(agentFilePath2);

            if (jsonFile1.open(QIODevice::ReadOnly) && jsonFile2.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson1 = jsonFile1.readAll();
                jsonFile1.close();

                QByteArray byteArrayOfJson2 = jsonFile2.readAll();
                jsonFile2.close();

                // Try to create a model of agent definition with the first JSON
                agentDefinition = _jsonHelper->createModelOfDefinition(byteArrayOfJson1);

                // If succeeded
                if (agentDefinition != NULL)
                {
                    // Create a model of agent mapping with the second JSON
                    agentMapping = _jsonHelper->createModelOfAgentMapping(agentDefinition->name(), byteArrayOfJson2);
                }
                else
                {
                    // Try to create a model of agent definition with the second JSON
                    agentDefinition = _jsonHelper->createModelOfDefinition(byteArrayOfJson2);

                    // If succeeded
                    if (agentDefinition != NULL)
                    {
                        // Create a model of agent mapping with the first JSON
                        agentMapping = _jsonHelper->createModelOfAgentMapping(agentDefinition->name(), byteArrayOfJson1);
                    }
                }
            }
            else {
                qCritical() << "Can not open 2 files" << agentFilePath1 << "and" << agentFilePath2;
            }
        }

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

            if (agentMapping != NULL) {
                // Add this new model of agent mapping for the agent name
                addAgentMappingForAgentName(agentMapping, agentName);

                // Update the merged list of mapping elements for the agent name
                _updateMergedListsOfMappingElementsForAgentName(agentName, agentMapping);

                // Set its mapping
                agent->setmapping(agentMapping);
            }
        }
    }
}


/**
 * @brief Export the agents list to JSON file
 * @param agentsListToExport list of pairs <agent name, definition>
 * @param agentsListFilePath
 */
void MasticModelManager::_exportAgentsListToFile(QList<QPair<QString, DefinitionM*>> agentsListToExport, QString agentsListFilePath)
{
    if (!agentsListFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Export the agents list to JSON file" << agentsListFilePath;

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
void MasticModelManager::_updateDefinitionVariants(QString definitionName)
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
 * @brief Update merged lists of mapping elements for the agent name
 * @param agentName
 * @param agentMapping
 */
void MasticModelManager::_updateMergedListsOfMappingElementsForAgentName(QString agentName, AgentMappingM* agentMapping)
{
    if (!agentName.isEmpty() && (agentMapping != NULL) && (agentMapping->elementMappingsList()->count() > 0))
    {
        // Get the merged list of all (models of) mapping elements which connect an input of the agent
        QList<ElementMappingM*> mergedListOfInputMappingElements = getMergedListOfInputMappingElementsFromAgentName(agentName);

        qDebug() << mergedListOfInputMappingElements.count() << "INPUT mapping elements from name" << agentName;

        qDebug() << "AVANT: From Agent Name To Merged List of INPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfInputMappingElements;
        qDebug() << "AVANT: From Agent Name To Merged List of OUTPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfOutputMappingElements;

        foreach (ElementMappingM* elementMapping, agentMapping->elementMappingsList()->toList())
        {
            if ((elementMapping != NULL) && (elementMapping->inputAgent() == agentName) && !elementMapping->outputAgent().isEmpty())
            {
                qDebug() << elementMapping->outputAgent() << "." << elementMapping->output() << "-->" << elementMapping->inputAgent() << "." << elementMapping->input();

                bool isAlreadyInMergedListOfInputMappingElements = false;
                foreach (ElementMappingM* iterator, mergedListOfInputMappingElements)
                {
                    // Exactly the same
                    if ((iterator != NULL)
                            && (iterator->outputAgent() == elementMapping->outputAgent())
                            && (iterator->output() == elementMapping->output())
                            && (iterator->input() == elementMapping->input())) {
                        isAlreadyInMergedListOfInputMappingElements = true;
                        break;
                    }
                }
                // Not already in merged list of mapping elements which connect an INPUT of the agent, we add it
                if (!isAlreadyInMergedListOfInputMappingElements) {
                    mergedListOfInputMappingElements.append(elementMapping);
                }


                //
                // Update merged list for output agent name
                //
                // Get the merged list of all (models of) mapping elements which connect the output agent
                QList<ElementMappingM*> mergedListOfOutputMappingElements = getMergedListOfOutputMappingElementsFromAgentName(elementMapping->outputAgent());

                bool isAlreadyInMergedListOfOutputMappingElements = false;
                foreach (ElementMappingM* iterator, mergedListOfOutputMappingElements)
                {
                    // Exactly the same
                    if ((iterator != NULL)
                            && (iterator->inputAgent() == elementMapping->inputAgent())
                            && (iterator->output() == elementMapping->output())
                            && (iterator->input() == elementMapping->input())) {
                        isAlreadyInMergedListOfOutputMappingElements = true;
                        break;
                    }
                }
                // Not already in merged list of mapping elements which connect an OUTPUT of the output agent, we add it
                if (!isAlreadyInMergedListOfOutputMappingElements) {
                    mergedListOfOutputMappingElements.append(elementMapping);

                    // Update the merged list in the map
                    _mapFromAgentNameToMergedListOfOutputMappingElements.insert(elementMapping->outputAgent(), mergedListOfOutputMappingElements);
                }
            }
        }

        // Update the merged list in the map
        _mapFromAgentNameToMergedListOfInputMappingElements.insert(agentName, mergedListOfInputMappingElements);

        qDebug() << "APRES: From Agent Name To Merged List of INPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfInputMappingElements;
        qDebug() << "APRES: From Agent Name To Merged List of OUTPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfOutputMappingElements;
    }
}


/**
 * @brief Clean merged lists of mapping elements for the agent name
 * @param agentName
 */
void MasticModelManager::_cleanMergedListsOfMappingElementsForAgentName(QString agentName)
{
    if (!agentName.isEmpty())
    {
        qDebug() << "AVANT: From Agent Name To Merged List of INPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfInputMappingElements;
        qDebug() << "AVANT: From Agent Name To Merged List of OUTPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfOutputMappingElements;

        // Get the merged list of all (models of) mapping elements which connect an input of the agent
        QList<ElementMappingM*> mergedListOfInputMappingElements = getMergedListOfInputMappingElementsFromAgentName(agentName);

        foreach (ElementMappingM* mappingElement, mergedListOfInputMappingElements) {
            if ((mappingElement != NULL) && (mappingElement->inputAgent() == agentName) && !mappingElement->outputAgent().isEmpty())
            {
                // Get the merged list of all (models of) mapping elements which connect the output agent
                QList<ElementMappingM*> mergedListOfOutputMappingElements = getMergedListOfOutputMappingElementsFromAgentName(mappingElement->outputAgent());

                mergedListOfOutputMappingElements.removeOne(mappingElement);

                // Update the merged list in the map
                _mapFromAgentNameToMergedListOfOutputMappingElements.insert(mappingElement->outputAgent(), mergedListOfOutputMappingElements);
            }
        }

        _mapFromAgentNameToMergedListOfInputMappingElements.remove(agentName);

        qDebug() << "APRES: From Agent Name To Merged List of INPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfInputMappingElements;
        qDebug() << "APRES: From Agent Name To Merged List of OUTPUT Mapping Elements:" << _mapFromAgentNameToMergedListOfOutputMappingElements;

        // Free memory
        qDeleteAll(mergedListOfInputMappingElements);
        mergedListOfInputMappingElements.clear();
    }
}


/**
 * @brief Print all models of agents (for Debug)
 */
void MasticModelManager::_printAgents()
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
void MasticModelManager::_printDefinitions()
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
void MasticModelManager::_printMappings()
{
    qDebug() << "Print Mappings:";
    foreach (QString mappingName, _mapFromNameToAgentMappingsList.keys()) {
        QList<AgentMappingM*> agentMappingsList = getAgentMappingsListFromMappingName(mappingName);
        qDebug() << mappingName << ":" << agentMappingsList.count() << "mappings";
    }
}
