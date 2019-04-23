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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "editormodelmanager.h"

#include <QQmlEngine>
#include <QFileDialog>
#include <I2Quick.h>


/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
EditorModelManager::EditorModelManager(JsonHelper* jsonHelper,
                                       QString rootDirectoryPath,
                                       QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                                rootDirectoryPath,
                                                                                parent),
    //_isMappingActivated(false),
    _isMappingControlled(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Editor Model Manager";

}


/**
 * @brief Destructor
 */
EditorModelManager::~EditorModelManager()
{
    qInfo() << "Delete IngeScape Editor Model Manager";

    // Clear all opened definitions
    _openedDefinitions.clear();

    qDeleteAll(_hashFromNameToHost);
    _hashFromNameToHost.clear();

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}


/**
 * @brief Setter for property "is Mapping Controlled"
 * @param value
 */
void EditorModelManager::setisMappingControlled(bool value)
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
 * @brief Get the model of host with a name
 * @param hostName
 * @return
 */
HostM* EditorModelManager::getHostModelWithName(QString hostName)
{
    if (_hashFromNameToHost.contains(hostName)) {
        return _hashFromNameToHost.value(hostName);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the peer id of the Launcher on a host
 * @param hostName
 * @return
 */
QString EditorModelManager::getPeerIdOfLauncherOnHost(QString hostName)
{
    // Get the model of host with the name
    HostM* host = getHostModelWithName(hostName);

    if (host != nullptr) {
        return host->peerId();
    }
    else {
        return "";
    }
}


/**
 * @brief Export the agents into JSON
 * @return array of all agents (grouped by name)
 */
QJsonArray EditorModelManager::exportAgentsToJSON()
{
    QJsonArray jsonArrayAgentsGroupedByName = QJsonArray();

    if (_jsonHelper != nullptr)
    {
        // List of all groups (of agents) grouped by name
        for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
        {
            if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty())
            {
                QJsonObject jsonAgentsGroupedByName = QJsonObject();

                // Name
                jsonAgentsGroupedByName.insert("agentName", agentsGroupedByName->name());

                QJsonArray jsonArrayAgentsGroupedByDefinition = QJsonArray();

                // List of all groups (of agents) grouped by definition
                for (AgentsGroupedByDefinitionVM* agentsGroupedByDefinition : agentsGroupedByName->allAgentsGroupsByDefinition()->toList())
                {
                    if (agentsGroupedByDefinition != nullptr)
                    {
                        QJsonObject jsonAgentsGroupedByDefinition = QJsonObject();

                        // Definition
                        if (agentsGroupedByDefinition->definition() != nullptr)
                        {
                            QJsonObject jsonDefinition = _jsonHelper->exportAgentDefinitionToJson(agentsGroupedByDefinition->definition());
                            jsonAgentsGroupedByDefinition.insert("definition", jsonDefinition);
                        }
                        else {
                            jsonAgentsGroupedByDefinition.insert("definition", QJsonValue());
                        }

                        // Clones (models)
                        QJsonArray jsonClones = QJsonArray();

                        for (AgentM* model : agentsGroupedByDefinition->models()->toList())
                        {
                            // Hostname and Command Line must be defined to be added to the array of clones
                            if ((model != nullptr) && !model->hostname().isEmpty() && !model->commandLine().isEmpty())
                            {
                                qDebug() << "Export" << model->name() << "on" << model->hostname() << "at" << model->commandLine() << "(" << model->peerId() << ")";

                                QJsonObject jsonClone = QJsonObject();
                                jsonClone.insert("hostname", model->hostname());
                                jsonClone.insert("commandLine", model->commandLine());
                                jsonClone.insert("peerId", model->peerId());
                                jsonClone.insert("address", model->address());

                                jsonClones.append(jsonClone);
                            }
                        }

                        jsonAgentsGroupedByDefinition.insert("clones", jsonClones);

                        jsonArrayAgentsGroupedByDefinition.append(jsonAgentsGroupedByDefinition);
                    }
                }

                jsonAgentsGroupedByName.insert("definitions", jsonArrayAgentsGroupedByDefinition);

                jsonArrayAgentsGroupedByName.append(jsonAgentsGroupedByName);
            }
        }
    }
    return jsonArrayAgentsGroupedByName;
}


/**
 * @brief Export the agents list to selected file
 */
void EditorModelManager::exportAgentsListToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString agentsListFilePath = QFileDialog::getSaveFileName(nullptr,
                                                              "Save agents",
                                                              _rootDirectoryPath,
                                                              "JSON (*.json)");

    if (!agentsListFilePath.isEmpty())
    {
        qInfo() << "Save the agents list to JSON file" << agentsListFilePath;

        // Export the agents into JSON
        QJsonArray jsonArrayOfAgents = exportAgentsToJSON();

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
 * @brief Simulate an exit for each launcher
 */
void EditorModelManager::simulateExitForEachLauncher()
{
    for (QString hostName : _hashFromNameToHost.keys())
    {
        if (hostName != HOSTNAME_NOT_DEFINED)
        {
            // Simulate an exit for this host (name)
            onLauncherExited("", hostName);
        }
    }
}


/**
 * @brief Open a definition
 * If there are variants of this definition, we open each variant
 * @param definition
 */
void EditorModelManager::openDefinition(DefinitionM* definition)
{
    if (definition != nullptr)
    {
        QString definitionName = definition->name();

        qDebug() << "Open the definition" << definitionName;

        QList<DefinitionM*> definitionsToOpen;

        /*// Variant --> we have to open each variants of this definition
        if (definition->isVariant())
        {
            for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
            {
                if (agentsGroupedByName != nullptr)
                {
                    // Get the list of definitions with a specific name
                    QList<DefinitionM*> definitionsList = agentsGroupedByName->getDefinitionsWithName(definitionName);

                    if (!definitionsList.isEmpty())
                    {
                        for (DefinitionM* iterator : definitionsList)
                        {
                            // Same name, same version and variant, we have to open it
                            if ((iterator != nullptr) && (iterator->version() == definition->version()) && iterator->isVariant()) {
                                definitionsToOpen.append(iterator);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Simply add the definition
            definitionsToOpen.append(definition);
        }*/

        // Simply add the definition
        definitionsToOpen.append(definition);

        // Open the list of definitions
        _openDefinitions(definitionsToOpen);
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
/*void EditorModelManager::onAgentEntered(QString peerId, QString agentName, QString ipAddress, QString hostname, QString commandLine, bool canBeFrozen, QString loggerPort)
{
    if (!peerId.isEmpty() && !agentName.isEmpty() && !ipAddress.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);

        // An agent with this peer id already exist
        if (agent != nullptr)
        {
            qInfo() << "The agent" << agentName << "with peer id" << peerId << "on" << hostname << "(" << ipAddress << ") is back on the network !";

            // Useless !
            //agent->sethostname(hostname);
            //agent->setcommandLine(commandLine);

            // Usefull ?
            agent->setcanBeFrozen(canBeFrozen);
            agent->setloggerPort(loggerPort);

            // Update the state (flag "is ON")
            agent->setisON(true);
        }
        // New peer id
        else
        {
            // Create a new model of agent
            agent = createAgentModel(agentName,
                                     nullptr,
                                     peerId,
                                     ipAddress,
                                     hostname,
                                     commandLine,
                                     true);

            if (agent != nullptr)
            {
                agent->setcanBeFrozen(canBeFrozen);
                agent->setloggerPort(loggerPort);
            }
        }
    }
}*/


/**
 * @brief Slot called when an agent quit the network
 * @param peer Id
 * @param agent name
 */
/*void EditorModelManager::onAgentExited(QString peerId, QString agentName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr)
    {
        qInfo() << "The agent" << agentName << "with peer id" << peerId << "exited from the network !";

        // Update the state (flag "is ON")
        agent->setisON(false);
    }
}*/


/**
 * @brief Slot called when a launcher enter the network
 * @param peerId
 * @param hostName
 * @param ipAddress
 */
void EditorModelManager::onLauncherEntered(QString peerId, QString hostName, QString ipAddress, QString streamingPort)
{
    if (!hostName.isEmpty())
    {
        // Get the model of host with the name
        HostM* host = getHostModelWithName(hostName);
        if (host == nullptr)
        {
            // Create a new host
            host = new HostM(hostName, peerId, ipAddress, streamingPort, this);

            _hashFromNameToHost.insert(hostName, host);

            Q_EMIT hostModelHasBeenCreated(host);
        }
        else
        {
            // Update peer id
            if (host->peerId() != peerId) {
                host->setpeerId(peerId);
            }

            // Update IP address
            if (host->ipAddress() != ipAddress) {
                host->setipAddress(ipAddress);
            }

            // Update streaming port
            if (host->streamingPort() != streamingPort) {
                host->setstreamingPort(streamingPort);
            }
        }

        // Traverse the list of all agents grouped by name
        for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
        {
            if (agentsGroupedByName != nullptr)
            {
                // Traverse the list of its models
                for (AgentM* agent : agentsGroupedByName->models()->toList())
                {
                    if ((agent != nullptr) && (agent->hostname() == hostName) && !agent->commandLine().isEmpty())
                    {
                        // This agent can be restarted
                        agent->setcanBeRestarted(true);
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot called when a launcher quit the network
 * @param peerId
 * @param hostName
 */
void EditorModelManager::onLauncherExited(QString peerId, QString hostName)
{
    Q_UNUSED(peerId)

    if (!hostName.isEmpty())
    {
        // Get the model of host with the name
        HostM* host = getHostModelWithName(hostName);
        if (host != nullptr)
        {
            Q_EMIT hostModelWillBeDeleted(host);

            _hashFromNameToHost.remove(hostName);

            // Free memory
            delete host;
        }

        // Traverse the list of all agents grouped by name
        for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
        {
            if (agentsGroupedByName != nullptr)
            {
                // Traverse the list of all models
                for (AgentM* agent : agentsGroupedByName->models()->toList())
                {
                    if ((agent != nullptr) && (agent->hostname() == hostName))
                    {
                        // This agent can NOT be restarted
                        agent->setcanBeRestarted(false);
                    }
                }
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
/*void EditorModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
{
    Q_UNUSED(agentName)

    AgentM* agent = getAgentModelFromPeerId(peerId);

    if ((agent != nullptr) && (_jsonHelper != nullptr) && !definitionJSON.isEmpty())
    {
        // Save the previous agent definition
        DefinitionM* previousDefinition = agent->definition();

        // Create the new model of agent definition from JSON
        DefinitionM* newDefinition = _jsonHelper->createModelOfAgentDefinitionFromBytes(definitionJSON.toUtf8());

        if (newDefinition != nullptr)
        {
            // Set this new definition to the agent
            agent->setdefinition(newDefinition);

            // Free memory
            if (previousDefinition != nullptr) {
                delete previousDefinition;
            }
        }
    }
}*/


/**
 * @brief Slot called when an agent mapping has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param mapping in JSON format
 */
/*void EditorModelManager::onMappingReceived(QString peerId, QString agentName, QString mappingJSON)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);

    if ((agent != nullptr) && (_jsonHelper != nullptr))
    {
        // Save the previous agent mapping
        AgentMappingM* previousMapping = agent->mapping();

        AgentMappingM* newMapping = nullptr;

        if (mappingJSON.isEmpty())
        {
            QString mappingName = QString("EMPTY MAPPING of %1").arg(agentName);
            newMapping = new AgentMappingM(mappingName, "", "");
        }
        else
        {
            // Create the new model of agent mapping from the JSON
            newMapping = _jsonHelper->createModelOfAgentMappingFromBytes(agentName, mappingJSON.toUtf8());
        }

        if (newMapping != nullptr)
        {
            // Set this new mapping to the agent
            agent->setmapping(newMapping);

            // Free memory
            if (previousMapping != nullptr) {
                delete previousMapping;
            }
        }
    }
}*/


/**
 * @brief Slot called when the flag "is Muted" from an agent updated
 * @param peerId
 * @param isMuted
 */
void EditorModelManager::onisMutedFromAgentUpdated(QString peerId, bool isMuted)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisMuted(isMuted);
    }
}


/**
 * @brief Slot called when the flag "can be Frozen" from an agent updated
 * @param peerId
 * @param canBeFrozen
 */
void EditorModelManager::onCanBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setcanBeFrozen(canBeFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Frozen" from an agent updated
 * @param peerId
 * @param isFrozen
 */
void EditorModelManager::onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisFrozen(isFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Muted" from an output of agent updated
 * @param peerId
 * @param isMuted
 * @param outputName
 */
void EditorModelManager::onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisMutedOfOutput(isMuted, outputName);
    }
}


/**
 * @brief Slot called when the state of an agent changes
 * @param peerId
 * @param stateName
 */
void EditorModelManager::onAgentStateChanged(QString peerId, QString stateName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setstate(stateName);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In Stream" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void EditorModelManager::onAgentHasLogInStream(QString peerId, bool hasLogInStream)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->sethasLogInStream(hasLogInStream);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In File" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void EditorModelManager::onAgentHasLogInFile(QString peerId, bool hasLogInFile)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->sethasLogInFile(hasLogInFile);
    }
}


/**
 * @brief Slot called when we receive the path of "Log File" for an agent
 * @param peerId
 * @param logFilePath
 */
void EditorModelManager::onAgentLogFilePath(QString peerId, QString logFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setlogFilePath(logFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Definition File" for an agent
 * @param peerId
 * @param definitionFilePath
 */
void EditorModelManager::onAgentDefinitionFilePath(QString peerId, QString definitionFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setdefinitionFilePath(definitionFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Mapping File" for an agent
 * @param peerId
 * @param mappingFilePath
 */
void EditorModelManager::onAgentMappingFilePath(QString peerId, QString mappingFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setmappingFilePath(mappingFilePath);
    }
}


/**
 * @brief Slot called when the definition(s) of an agent (agents grouped by name) must be opened
 * @param definitionsList
 */
void EditorModelManager::_onDefinitionsToOpen(QList<DefinitionM*> definitionsList)
{
    _openDefinitions(definitionsList);
}


/**
 * @brief Create a new view model of agents grouped by name
 * @param model
 */
void EditorModelManager::_createAgentsGroupedByName(AgentM* model)
{
    // Call our mother class
    IngeScapeModelManager::_createAgentsGroupedByName(model);

    if ((model != nullptr) && !model->name().isEmpty())
    {
        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(model->name());
        if (agentsGroupedByName != nullptr)
        {
            // Connect to signals from this new view model of agents grouped by definition
            connect(agentsGroupedByName, &AgentsGroupedByNameVM::definitionsToOpen, this, &EditorModelManager::_onDefinitionsToOpen);
        }
    }
}


/**
 * @brief Open a list of definitions (if the definition is already opened, we bring it to front)
 * @param definitionsToOpen
 */
void EditorModelManager::_openDefinitions(QList<DefinitionM*> definitionsToOpen)
{
    // Traverse the list of definitions to open
    for (DefinitionM* definition : definitionsToOpen)
    {
        if (definition != nullptr)
        {
            if (!_openedDefinitions.contains(definition)) {
                _openedDefinitions.append(definition);
            }
            else {
                qDebug() << "The 'Definition'" << definition->name() << "is already opened...bring it to front !";

                Q_EMIT definition->bringToFront();
            }
        }
    }
}
