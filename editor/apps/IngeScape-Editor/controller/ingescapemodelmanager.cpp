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
 * @brief Constructor
 * @param jsonHelper
 * @param agentsListDirectoryPath
 * @param agentsMappingsDirectoryPath
 * @param dataDirectoryPath
 * @param parent
 */
IngeScapeModelManager::IngeScapeModelManager(JsonHelper* jsonHelper,
                                             QString rootDirectoryPath,
                                             //QString agentsListDirectoryPath,
                                             QObject *parent) : QObject(parent),
    _isMappingActivated(false),
    _isMappingControlled(false),
    _jsonHelper(jsonHelper),
    _rootDirectoryPath(rootDirectoryPath)
    //_agentsListDirectoryPath(agentsListDirectoryPath)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New INGESCAPE Model Manager";

    //_agentsListDefaultFilePath = QString("%1last.json").arg(_agentsListDirectoryPath);

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

    // Reset pointers
    _jsonHelper = NULL;
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
 * @brief Import an agent or an agents list from selected file (definition)
 */
bool IngeScapeModelManager::importAgentOrAgentsListFromSelectedFile()
{
    bool success = true;

    // "File Dialog" to get the file (path) to open
    QString agentFilePath = QFileDialog::getOpenFileName(NULL,
                                                         "Open an agent(s) definition",
                                                         _rootDirectoryPath,
                                                         "JSON (*.json)");

    if (!agentFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        QFile jsonFile(agentFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            QJsonObject jsonRoot = jsonDocument.object();

            // List of agents
            if (jsonRoot.contains("agents"))
            {
                // Import the agents list from a json byte content
                importAgentsListFromJson(jsonRoot.value("agents").toArray());
            }
            // One agent
            else
            {
                // Create a model of agent definition from the JSON
                DefinitionM* agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromBytes(byteArrayOfJson);
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

            // FIXME: à tester
            /*else if (jsonRoot.contains("definition"))
            {
                QJsonValue jsonValue = jsonRoot.value("definition");
                if (jsonValue.isObject())
                {
                    // Create a model of agent definition from the JSON
                    DefinitionM* agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromJSON(jsonValue.toObject());
                }
            }*/
        }
        else {
            qCritical() << "Can not open file" << agentFilePath;
            success = false;
        }
    }

    return success;
}


/**
 * @brief Import an agents list from a JSON array
 * @param jsonArrayOfAgents
 */
void IngeScapeModelManager::importAgentsListFromJson(QJsonArray jsonArrayOfAgents)
{
    if (_jsonHelper != NULL)
    {
        for (QJsonValue jsonValue : jsonArrayOfAgents)
        {
            if (jsonValue.isObject())
            {
                QJsonObject jsonAgent = jsonValue.toObject();

                // Get values for keys "agentName", "definition" and "clones"
                QJsonValue jsonName = jsonAgent.value("agentName");
                QJsonValue jsonDefinition = jsonAgent.value("definition");
                QJsonValue jsonClones = jsonAgent.value("clones");

                if (jsonName.isString() && jsonDefinition.isObject() && jsonClones.isArray())
                {
                    QString agentName = jsonName.toString();
                    QJsonArray arrayOfClones = jsonClones.toArray();

                    // Create a model of agent definition from JSON object
                    DefinitionM* agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                    if (agentDefinition != NULL)
                    {
                        // None clones have a defined hostname (agent is only defined by a definition)
                        if (arrayOfClones.isEmpty())
                        {
                            qDebug() << "Clone of" << agentName << "without hostname and command line";

                            // Create a new model of agent with the name
                            AgentM* agent = new AgentM(agentName, this);

                            // Add this new model of agent
                            addAgentModel(agent);

                            // Add this new model of agent definition for the agent name
                            addAgentDefinitionForAgentName(agentDefinition, agentName);

                            // Set its definition
                            agent->setdefinition(agentDefinition);
                        }
                        // There are some clones with a defined hostname
                        else
                        {
                            for (QJsonValue jsonValue : arrayOfClones)
                            {
                                if (jsonValue.isObject())
                                {
                                    QJsonObject jsonClone = jsonValue.toObject();

                                    QJsonValue jsonHostname = jsonClone.value("hostname");
                                    QJsonValue jsonCommandLine = jsonClone.value("commandLine");
                                    QJsonValue jsonPeerId = jsonClone.value("peerId");
                                    QJsonValue jsonAddress = jsonClone.value("address");

                                    if (jsonHostname.isString() && jsonCommandLine.isString() && jsonPeerId.isString() && jsonAddress.isString())
                                    {
                                        QString hostname = jsonHostname.toString();
                                        QString commandLine = jsonCommandLine.toString();
                                        QString peerId = jsonPeerId.toString();
                                        QString ipAddress = jsonAddress.toString();

                                        if (!hostname.isEmpty() && !commandLine.isEmpty())
                                        {
                                            qDebug() << "Clone of" << agentName << "on" << hostname << "with command line" << commandLine << "(" << peerId << ")";

                                            // Create a new model of agent with the name
                                            AgentM* agent = new AgentM(agentName, peerId, ipAddress,  this);

                                            // Update the hostname and the command line
                                            agent->sethostname(hostname);
                                            agent->setcommandLine(commandLine);

                                            // Update corresponding host
                                            HostM* host = IngeScapeLauncherManager::Instance().getHostWithName(hostname);
                                            if (host != NULL) {
                                                agent->setcanBeRestarted(true);
                                            }

                                            // Add this new model of agent
                                            addAgentModel(agent);

                                            // Make a copy of the definition
                                            DefinitionM* copyOfDefinition = agentDefinition->copy();
                                            if (copyOfDefinition != NULL)
                                            {
                                                // Add this new model of agent definition for the agent name
                                                addAgentDefinitionForAgentName(copyOfDefinition, agentName);

                                                // Set its definition
                                                agent->setdefinition(copyOfDefinition);
                                            }
                                        }
                                    }
                                }
                            }

                            // Free memory
                            delete agentDefinition;
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Export the agents list to selected file
 * @param jsonArrayOfAgents
 */
void IngeScapeModelManager::exportAgentsListToSelectedFile(QJsonArray jsonArrayOfAgents)
{
    // "File Dialog" to get the file (path) to save
    QString agentsListFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save agents",
                                                              _rootDirectoryPath,
                                                              "JSON (*.json)");

    if (!agentsListFilePath.isEmpty())
    {
        qInfo() << "Save the agents list to JSON file" << agentsListFilePath;

        QJsonObject jsonRoot = QJsonObject();
        jsonRoot.insert("agents", jsonArrayOfAgents);

        QByteArray byteArrayOfJson = QJsonDocument(jsonRoot).toJson(QJsonDocument::Indented);

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
 * @brief Simulate an exit for each agent
 */
void IngeScapeModelManager::simulateExitForEachActiveAgent()
{
    for (AgentM* agent : _mapFromPeerIdToAgentM.values())
    {
        if ((agent != NULL) && agent->isON())
        {
            // Simulate an exit for each agent
            onAgentExited(agent->peerId(), agent->name());

            // Reset the peer id and address
            if (!agent->peerId().isEmpty())
            {
                _mapFromPeerIdToAgentM.remove(agent->peerId());

                agent->setpeerId("");
                agent->setaddress("");
            }

            // Reset the mapping
            if (agent->mapping() != NULL)
            {
                AgentMappingM* mapping = agent->mapping();

                // Reset its model of mapping
                agent->setmapping(NULL);

                // Delete the model of agent mapping
                deleteAgentMapping(mapping);
            }
        }
    }
}


/**
 * @brief Open the definition with an agent name
 * @param agentName
 */
void IngeScapeModelManager::openDefinitionWithAgentName(QString agentName)
{
    if (!agentName.isEmpty())
    {
        // Get the list of models of agent with the name
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);
        if (!agentModelsList.isEmpty())
        {
            // By default, we take the first one
            AgentM* agent = agentModelsList.at(0);
            if ((agent != NULL) && (agent->definition() != NULL))
            {
                // Open its definition
                openDefinition(agent->definition());
            }
        }
    }
}


/**
 * @brief Open a definition
 * If there are variants of this definition, we open each variant
 * @param definition
 */
void IngeScapeModelManager::openDefinition(DefinitionM* definition)
{
    if (definition != NULL)
    {
        qDebug() << "Open the definition" << definition->name();

        QList<DefinitionM*> definitionsToOpen;

        // Variant --> we have to open each variants of this definition
        if (definition->isVariant())
        {
            // Get the list (of models) of agent definition from a definition name
            QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromDefinitionName(definition->name());

            for (DefinitionM* iterator : agentDefinitionsList)
            {
                // Same name, same version and variant, we have to open it
                if ((iterator != NULL) && (iterator->version() == definition->version()) && iterator->isVariant()) {
                    definitionsToOpen.append(iterator);
                }
            }
        }
        else {
            // Simply add the definition
            definitionsToOpen.append(definition);
        }

        // Traverse the list of definitions to open
        for (DefinitionM* iterator : definitionsToOpen)
        {
            if (iterator != NULL)
            {
                if (!_openedDefinitions.contains(iterator)) {
                    _openedDefinitions.append(iterator);
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
 * @brief Slot called when an agent enter the network
 * @param peerId
 * @param agentName
 * @param ipAddress
 * @param hostname
 * @param commandLine
 * @param canBeFrozen
 * @param loggerPort
 */
void IngeScapeModelManager::onAgentEntered(QString peerId, QString agentName, QString ipAddress, QString hostname, QString commandLine, bool canBeFrozen, QString loggerPort)
{
    if (!peerId.isEmpty() && !agentName.isEmpty() && !ipAddress.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);

        // An agent with this peer id already exist
        if (agent != NULL)
        {
            qInfo() << "The agent" << agentName << "with peer id" << peerId << "on" << hostname << "(" << ipAddress << ") is back on the network !";

            // Update the state (flag "is ON")
            agent->setisON(true);

            // When this agent exited, we set its flag to "OFF" and emited "removeInputsToEditorForOutputs"
            // Now, we just set its flag to ON and we have to emit "addInputsToEditorForOutputs"
            // Because we consider that its definition will be the same...consequently, when "onDefinitionReceived" will be called,
            // there will be no change detected and the signal "addInputsToEditorForOutputs" will not be called
            if ((agent->definition() != NULL) && !agent->definition()->outputsList()->isEmpty())
            {
                Q_EMIT addInputsToEditorForOutputs(agentName, agent->definition()->outputsList()->toList());
            }
        }
        // New peer id
        else
        {
            // Create a new model of agent
            agent = new AgentM(agentName, peerId, ipAddress, this);

            agent->sethostname(hostname);
            agent->setcommandLine(commandLine);

            if (!hostname.isEmpty() && !commandLine.isEmpty())
            {
                HostM* host = IngeScapeLauncherManager::Instance().getHostWithName(hostname);
                if (host != NULL)
                {
                    agent->setcanBeRestarted(true);
                }
            }

            agent->setcanBeFrozen(canBeFrozen);
            agent->setloggerPort(loggerPort);

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
    if (agent != NULL)
    {
        qInfo() << "The agent" << agentName << "with peer id" << peerId << "exited from the network !";

        // Update the state (flag "is ON")
        agent->setisON(false);

        if ((agent->definition() != NULL) && !agent->definition()->outputsList()->isEmpty()) {
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
    if (!definitionJSON.isEmpty() && (_jsonHelper != NULL))
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if (agent != NULL)
        {
            // Create a model of agent definition from JSON
            DefinitionM* agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromBytes(definitionJSON.toUtf8());
            if (agentDefinition != NULL)
            {
                 if (agent->definition() == NULL)
                 {
                     // Add this new model of agent definition for the agent name
                     addAgentDefinitionForAgentName(agentDefinition, agentName);

                     // Set this definition to the agent
                     agent->setdefinition(agentDefinition);

                     if (!agentDefinition->outputsList()->isEmpty()) {
                         Q_EMIT addInputsToEditorForOutputs(agentName, agentDefinition->outputsList()->toList());
                     }
                 }
                 // Update with the new definition
                 else
                 {
                     DefinitionM* previousDefinition = agent->definition();
                     if (previousDefinition != NULL)
                     {
                         //
                         // Check if output(s) have been removed
                         //
                         QList<OutputM*> removedOutputsList;
                         for (OutputM* output : previousDefinition->outputsList()->toList()) {
                             if ((output != NULL) && !output->id().isEmpty() && !agentDefinition->outputsIdsList().contains(output->id())) {
                                 removedOutputsList.append(output);
                             }
                         }
                         if (!removedOutputsList.isEmpty()) {
                             // Emit the signal "Remove Inputs to Editor for Outputs"
                             Q_EMIT removeInputsToEditorForOutputs(agentName, removedOutputsList);
                         }


                         // Add this new model of agent definition for the agent name
                         addAgentDefinitionForAgentName(agentDefinition, agentName);

                         // Set this definition to the agent
                         agent->setdefinition(agentDefinition);


                         //
                         // Check if output(s) have been added
                         //
                         QList<OutputM*> addedOutputsList;
                         for (OutputM* output : agentDefinition->outputsList()->toList()) {
                             if ((output != NULL) && !output->id().isEmpty() && !previousDefinition->outputsIdsList().contains(output->id())) {
                                 addedOutputsList.append(output);
                             }
                         }
                         if (!addedOutputsList.isEmpty()) {
                             // Emit the signal "Add Inputs to Editor for Outputs"
                             Q_EMIT addInputsToEditorForOutputs(agentName, addedOutputsList);
                         }


                         // Delete the previous model of agent definition
                         deleteAgentDefinition(previousDefinition);
                     }
                 }

                 // Emit the signal "Active Agent Defined"
                 Q_EMIT activeAgentDefined(agent);
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
    if ((agent != NULL) && (_jsonHelper != NULL))
    {
        AgentMappingM* agentMapping = NULL;

        if (mappingJSON.isEmpty())
        {
            QString mappingName = QString("EMPTY MAPPING of %1").arg(agentName);
            agentMapping = new AgentMappingM(mappingName, "", "");
        }
        else
        {
            // Create a model of agent mapping from the JSON
            agentMapping = _jsonHelper->createModelOfAgentMappingFromBytes(agentName, mappingJSON.toUtf8());
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
                if (!idsOfRemovedMappingElements.isEmpty())
                {
                    foreach (ElementMappingM* mappingElement, previousMapping->mappingElements()->toList()) {
                        if ((mappingElement != NULL) && idsOfRemovedMappingElements.contains(mappingElement->id()))
                        {
                            // Emit the signal "UN-mapped"
                            Q_EMIT unmapped(mappingElement);
                        }
                    }
                }
                // If there are some Added mapping elements
                if (!idsOfAddedMappingElements.isEmpty())
                {
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
    if (agent != NULL) {
        agent->setstate(stateName);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In Stream" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void IngeScapeModelManager::onAgentHasLogInStream(QString peerId, bool hasLogInStream)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL) {
        qDebug() << agent->name() << "Has Log in STREAM ?" << hasLogInStream;

        agent->sethasLogInStream(hasLogInStream);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In File" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void IngeScapeModelManager::onAgentHasLogInFile(QString peerId, bool hasLogInFile)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL) {
        //qDebug() << agent->name() << "Has Log in FILE ?" << hasLogInFile;
        agent->sethasLogInFile(hasLogInFile);
    }
}


/**
 * @brief Slot called when we receive the path of "Log File" for an agent
 * @param peerId
 * @param logFilePath
 */
void IngeScapeModelManager::onAgentLogFilePath(QString peerId, QString logFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL) {
        qDebug() << agent->name() << "Log file path:" << logFilePath;

        agent->setlogFilePath(logFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Definition File" for an agent
 * @param peerId
 * @param definitionFilePath
 */
void IngeScapeModelManager::onAgentDefinitionFilePath(QString peerId, QString definitionFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL) {
        qDebug() << agent->name() << "Definition file path:" << definitionFilePath;

        agent->setdefinitionFilePath(definitionFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Mapping File" for an agent
 * @param peerId
 * @param mappingFilePath
 */
void IngeScapeModelManager::onAgentMappingFilePath(QString peerId, QString mappingFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != NULL) {
        qDebug() << agent->name() << "Mapping file path:" << mappingFilePath;

        agent->setmappingFilePath(mappingFilePath);
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
        if (!activeAgentsList.isEmpty()) {
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

        // Free memory...later
        // the call to "agent->setdefinition" will produce the call of the slot _onAgentDefinitionChangedWithPreviousAndNewValues
        // ...and in some cases, the call to deleteAgentModel on this agent. So we cannot call directly "delete agent;"
        //delete agent;
        agent->deleteLater();

        //_printAgents();
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
            if (mapFromVersionToDefinitionsList.contains(version))
            {
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
