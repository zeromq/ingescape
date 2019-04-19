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
#include <QDebug>
#include <QFileDialog>
#include <I2Quick.h>


// Threshold beyond which we consider that there are too many values
#define TOO_MANY_VALUES 2000


/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
IngeScapeModelManager::IngeScapeModelManager(JsonHelper* jsonHelper,
                                             QString rootDirectoryPath,
                                             QObject *parent) : QObject(parent),
    _isMappingActivated(false),
    _isMappingControlled(false),
    _jsonHelper(jsonHelper),
    _rootDirectoryPath(rootDirectoryPath)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Editor Model Manager";

    // Agents grouped are sorted on their name (alphabetical order)
    _allAgentsGroupsByName.setSortProperty("name");
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

    qDeleteAll(_hashFromNameToHost);
    _hashFromNameToHost.clear();

    // Free memory
    _hashFromNameToAgentsGrouped.clear();

    // Delete all view model of agents grouped by name
    for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
    {
        if (agentsGroupedByName != nullptr) {
            deleteAgentsGroupedByName(agentsGroupedByName);
        }
    }
    _allAgentsGroupsByName.clear();

    // Reset pointers
    _jsonHelper = nullptr;
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
 * @brief Create a new model of agent with a name, a definition (can be NULL) and some properties
 * @param agentName
 * @param definition optional (NULL by default)
 * @param peerId optional (empty by default)
 * @param ipAddress optional (empty by default)
 * @param hostname optional (default value)
 * @param commandLine optional (empty by default)
 * @param isON optional (false by default)
 * @return
 */
AgentM* IngeScapeModelManager::createAgentModel(QString agentName,
                                                DefinitionM* definition,
                                                QString peerId,
                                                QString ipAddress,
                                                QString hostname,
                                                QString commandLine,
                                                bool isON)
{
    AgentM* agent = nullptr;

    if (!agentName.isEmpty())
    {
        // Create a new model of agent
        agent = new AgentM(agentName,
                           peerId,
                           ipAddress,
                           hostname,
                           commandLine,
                           isON,
                           this);

        // If defined, set the definition
        if (definition != nullptr) {
            agent->setdefinition(definition);
        }

        // Connect to signals from this new agent
        connect(agent, &AgentM::networkDataWillBeCleared, this, &IngeScapeModelManager::_onNetworkDataOfAgentWillBeCleared);

        if (!agent->peerId().isEmpty()) {
            _hashFromPeerIdToAgent.insert(agent->peerId(), agent);
        }

        // Emit the signal "Agent Model has been Created"
        Q_EMIT agentModelHasBeenCreated(agent);


        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(agent->name());
        if (agentsGroupedByName != nullptr)
        {
            // Add the new model of agent
            agentsGroupedByName->addNewAgentModel(agent);
        }
        else
        {
            // Create a new view model of agents grouped by name
            _createAgentsGroupedByName(agent);
        }
    }

    return agent;
}


/**
 * @brief Delete a model of agent
 * @param agent
 */
void IngeScapeModelManager::deleteAgentModel(AgentM* agent)
{
    if ((agent != nullptr) && !agent->name().isEmpty())
    {
        // Emit the signal "Agent Model will be Deleted"
        Q_EMIT agentModelWillBeDeleted(agent);

        // Reset the definition of the agent and free memory
        if (agent->definition() != nullptr)
        {
            DefinitionM* agentDefinition = agent->definition();
            agent->setdefinition(nullptr);
            delete agentDefinition;
        }

        // Reset the mapping of the agent and free memory
        if (agent->mapping() != nullptr)
        {
            AgentMappingM* agentMapping = agent->mapping();
            agent->setmapping(nullptr);
            delete agentMapping;
        }

        // DIS-connect to signals from the agent
        disconnect(agent, nullptr, this, nullptr);

        if (!agent->peerId().isEmpty()) {
            _hashFromPeerIdToAgent.remove(agent->peerId());
        }

        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(agent->name());
        if (agentsGroupedByName != nullptr)
        {
            // Remove the old model of agent
            agentsGroupedByName->removeOldAgentModel(agent);
        }

        // Free memory
        delete agent;
    }
}


/**
 * @brief Delete a view model of agents grouped by name
 * @param agentsGroupedByName
 */
void IngeScapeModelManager::deleteAgentsGroupedByName(AgentsGroupedByNameVM* agentsGroupedByName)
{
    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty())
    {
        // Clear our agent just before its deletion
        agentsGroupedByName->clearBeforeDeletion();

        // Else, signals "agentsGroupedByDefinitionWillBeDeleted" and "agentModelHasToBeDeleted" will not be catched (after "disconnect")

        // DIS-connect to its signals
        disconnect(agentsGroupedByName, nullptr, this, nullptr);

        // Remove from the hash table
        _hashFromNameToAgentsGrouped.remove(agentsGroupedByName->name());

        // Remove from the sorted list
        _allAgentsGroupsByName.remove(agentsGroupedByName);

        // Emit the signal "Agents grouped by name will be deleted"
        Q_EMIT agentsGroupedByNameWillBeDeleted(agentsGroupedByName);

        // Free memory
        delete agentsGroupedByName;
    }
}


/**
 * @brief Get the model of host with a name
 * @param hostName
 * @return
 */
HostM* IngeScapeModelManager::getHostModelWithName(QString hostName)
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
QString IngeScapeModelManager::getPeerIdOfLauncherOnHost(QString hostName)
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
 * @brief Get the model of agent from a Peer Id
 * @param peerId
 * @return
 */
AgentM* IngeScapeModelManager::getAgentModelFromPeerId(QString peerId)
{
    if (_hashFromPeerIdToAgent.contains(peerId)) {
        return _hashFromPeerIdToAgent.value(peerId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the (view model of) agents grouped for a name
 * @param name
 * @return
 */
AgentsGroupedByNameVM* IngeScapeModelManager::getAgentsGroupedForName(QString name)
{
    if (_hashFromNameToAgentsGrouped.contains(name)) {
        return _hashFromNameToAgentsGrouped.value(name);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the hash table from a name to the group of agents with this name
 * @return
 */
QHash<QString, AgentsGroupedByNameVM*> IngeScapeModelManager::getHashTableFromNameToAgentsGrouped()
{
    return _hashFromNameToAgentsGrouped;
}


/**
 * @brief Import an agent or an agents list from selected file (definition)
 */
bool IngeScapeModelManager::importAgentOrAgentsListFromSelectedFile()
{
    bool success = true;

    if (_jsonHelper != nullptr)
    {
        // "File Dialog" to get the file (path) to open
        QString agentFilePath = QFileDialog::getOpenFileName(nullptr,
                                                             "Open an agent(s) definition",
                                                             _rootDirectoryPath,
                                                             "JSON (*.json)");

        if (!agentFilePath.isEmpty())
        {
            QFile jsonFile(agentFilePath);
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
                //if (jsonDocument.isObject())

                QJsonObject jsonRoot = jsonDocument.object();

                // List of agents
                if (jsonRoot.contains("agents"))
                {
                    // Version
                    QString versionJsonPlatform = "";
                    if (jsonRoot.contains("version"))
                    {
                        versionJsonPlatform = jsonRoot.value("version").toString();

                        qDebug() << "Version of JSON platform is" << versionJsonPlatform;
                    }
                    else {
                        qDebug() << "UNDEFINED version of JSON platform";
                    }

                    // Import the agents list from a json byte content
                    success = importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);
                }
                // One agent
                else if (jsonRoot.contains("definition"))
                {
                    QJsonValue jsonDefinition = jsonRoot.value("definition");
                    if (jsonDefinition.isObject())
                    {
                        // Create a model of agent definition from the JSON
                        DefinitionM* agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                        if (agentDefinition != nullptr)
                        {
                            // Create a new model of agent with the name of the definition
                            createAgentModel(agentDefinition->name(), agentDefinition);
                        }
                        // An error occured, the definition is NULL
                        else {
                            qWarning() << "The file" << agentFilePath << "does not contain an agent definition !";

                            success = false;
                        }
                    }
                }
                else {
                    qWarning() << "The file" << agentFilePath << "does not contain one or several agent definition(s) !";

                    success = false;
                }
            }
            else {
                qCritical() << "Can not open file" << agentFilePath;

                success = false;
            }
        }
    }
    return success;
}


/**
 * @brief Import an agents list from a JSON array
 * @param jsonArrayOfAgents
 * @param versionJsonPlatform
 */
bool IngeScapeModelManager::importAgentsListFromJson(QJsonArray jsonArrayOfAgents, QString versionJsonPlatform)
{
    bool success = true;

    if (_jsonHelper != nullptr)
    {
        for (QJsonValue jsonIteratorAgent : jsonArrayOfAgents)
        {
            if (jsonIteratorAgent.isObject())
            {
                QJsonObject jsonAgentsGroupedByName = jsonIteratorAgent.toObject();

                QJsonValue jsonName = jsonAgentsGroupedByName.value("agentName");
                QJsonArray jsonArrayOfDefinitions;

                // The version is the current one, use directly the array of definitions
                if (versionJsonPlatform == VERSION_JSON_PLATFORM)
                {
                    QJsonValue jsonDefinitions = jsonAgentsGroupedByName.value("definitions");
                    if (jsonDefinitions.isArray()) {
                        jsonArrayOfDefinitions = jsonDefinitions.toArray();
                    }
                }
                // Convert the previous format of JSON into the new format of JSON
                else
                {
                    jsonArrayOfDefinitions = QJsonArray();
                    QJsonValue jsonDefinition = jsonAgentsGroupedByName.value("definition");
                    QJsonValue jsonClones = jsonAgentsGroupedByName.value("clones");

                    // The definition can be NULL
                    if ((jsonDefinition.isObject() || jsonDefinition.isNull())
                            && jsonClones.isArray())
                    {
                        // Create a temporary json object and add it to the array of definitions
                        QJsonObject jsonObject = QJsonObject();
                        jsonObject.insert("definition", jsonDefinition);
                        jsonObject.insert("clones", jsonClones);

                        jsonArrayOfDefinitions.append(jsonObject);
                    }
                }

                if (jsonName.isString() && !jsonArrayOfDefinitions.isEmpty())
                {
                    QString agentName = jsonName.toString();

                    for (QJsonValue jsonIteratorDefinition : jsonArrayOfDefinitions)
                    {
                        QJsonObject jsonAgentsGroupedByDefinition = jsonIteratorDefinition.toObject();

                        QJsonValue jsonDefinition = jsonAgentsGroupedByDefinition.value("definition");
                        QJsonValue jsonClones = jsonAgentsGroupedByDefinition.value("clones");

                        // Manage the definition
                        DefinitionM* agentDefinition = nullptr;

                        if (jsonDefinition.isObject())
                        {
                            // Create a model of agent definition from JSON object
                            agentDefinition = _jsonHelper->createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                        }
                        // The definition can be NULL
                        /*else if (jsonDefinition.isNull()) {
                            // Nothing to do
                        }*/

                        // Manage the list of clones
                        QJsonArray arrayOfClones = jsonClones.toArray();

                        // None clone have a defined hostname (the agent is only defined by a definition)
                        if (arrayOfClones.isEmpty())
                        {
                            qDebug() << "Clone of" << agentName << "without hostname and command line";

                            // Make a copy of the definition
                            DefinitionM* copyOfDefinition = nullptr;
                            if (agentDefinition != nullptr) {
                                copyOfDefinition = agentDefinition->copy();
                            }

                            // Create a new model of agent
                            createAgentModel(agentName,
                                             copyOfDefinition);
                        }
                        // There are some clones with a defined hostname
                        else
                        {
                            for (QJsonValue jsonIteratorClone : arrayOfClones)
                            {
                                if (jsonIteratorClone.isObject())
                                {
                                    QJsonObject jsonClone = jsonIteratorClone.toObject();

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

                                        //if (!hostname.isEmpty() && !commandLine.isEmpty())
                                        if (!hostname.isEmpty() && !commandLine.isEmpty() && !peerId.isEmpty() && !ipAddress.isEmpty())
                                        {
                                            // Check that there is not yet an agent with this peer id
                                            AgentM* agent = getAgentModelFromPeerId(peerId);
                                            if (agent == nullptr)
                                            {
                                                qDebug() << "Clone of" << agentName << "on" << hostname << "with command line" << commandLine << "(" << peerId << ")";

                                                // Make a copy of the definition
                                                DefinitionM* copyOfDefinition = nullptr;
                                                if (agentDefinition != nullptr) {
                                                    copyOfDefinition = agentDefinition->copy();
                                                }

                                                // Create a new model of agent
                                                createAgentModel(agentName,
                                                                 copyOfDefinition,
                                                                 peerId,
                                                                 ipAddress,
                                                                 hostname,
                                                                 commandLine);
                                            }
                                            else {
                                                qWarning() << "The agent" << agent->name() << "already exists with the peer id" << peerId;
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        // Free memory
                        if (agentDefinition != nullptr) {
                            delete agentDefinition;
                        }
                    }
                }
                else
                {
                    qWarning() << "The JSON object does not contain an agent name !";

                    success = false;
                }
            }
        }
    }
    return success;
}


/**
 * @brief Export the agents into JSON
 * @return array of all agents (grouped by name)
 */
QJsonArray IngeScapeModelManager::exportAgentsToJSON()
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
void IngeScapeModelManager::exportAgentsListToSelectedFile()
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
 * @brief Simulate an exit for each agent ON
 */
void IngeScapeModelManager::simulateExitForEachAgentON()
{
    for (AgentM* agent : _hashFromPeerIdToAgent.values())
    {
        if ((agent != nullptr) && agent->isON())
        {
            // Simulate an exit for this agent
            onAgentExited(agent->peerId(), agent->name());
        }
    }
}


/**
 * @brief Simulate an exit for each launcher
 */
void IngeScapeModelManager::simulateExitForEachLauncher()
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
 * @brief Delete agents OFF
 */
void IngeScapeModelManager::deleteAgentsOFF()
{   
    for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
    {
        if (agentsGroupedByName != nullptr)
        {
            // ON
            if (agentsGroupedByName->isON())
            {
                // Delete agents OFF
                agentsGroupedByName->deleteAgentsOFF();
            }
            // OFF
            else
            {
                // Delete the view model of agents grouped by name
                deleteAgentsGroupedByName(agentsGroupedByName);
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
void IngeScapeModelManager::onAgentEntered(QString peerId, QString agentName, QString ipAddress, QString hostname, QString commandLine, bool canBeFrozen, QString loggerPort)
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
}


/**
 * @brief Slot called when an agent quit the network
 * @param peer Id
 * @param agent name
 */
void IngeScapeModelManager::onAgentExited(QString peerId, QString agentName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr)
    {
        qInfo() << "The agent" << agentName << "with peer id" << peerId << "exited from the network !";

        // Update the state (flag "is ON")
        agent->setisON(false);
    }
}


/**
 * @brief Slot called when a launcher enter the network
 * @param peerId
 * @param hostName
 * @param ipAddress
 */
void IngeScapeModelManager::onLauncherEntered(QString peerId, QString hostName, QString ipAddress, QString streamingPort)
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
void IngeScapeModelManager::onLauncherExited(QString peerId, QString hostName)
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
void IngeScapeModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
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
}


/**
 * @brief Slot called when a new value is published
 * @param publishedValue
 */
void IngeScapeModelManager::onValuePublished(PublishedValueM* publishedValue)
{
    if (publishedValue != nullptr)
    {
        // Add to the list at the first position
        _publishedValues.prepend(publishedValue);

        // Check if there are too many values
        if (_publishedValues.count() > TOO_MANY_VALUES)
        {
            // We kept 80% of the values
            int numberOfKeptValues = static_cast<int>(0.8 * TOO_MANY_VALUES);

            int numberOfDeletedValues = _publishedValues.count() - numberOfKeptValues;

            qDebug() << _publishedValues.count() << "values: we delete the" << numberOfDeletedValues << "oldest values and kept the" << numberOfKeptValues << "newest values";

            // FIXME: More efficient ?
            //_publishedValues.removeRows()
            //_publishedValues.removeColumns()

            while (_publishedValues.count() > numberOfKeptValues)
            {
                PublishedValueM* tempPublishedValue = _publishedValues.takeAt(_publishedValues.count() - 1);
                delete tempPublishedValue;
            }
        }


        // Get the (view model of) agents grouped for the name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(publishedValue->agentName());
        if (agentsGroupedByName != nullptr)
        {
            // Update the current value of an I/O/P of this agent(s)
            agentsGroupedByName->updateCurrentValueOfIOP(publishedValue);
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
    if (agent != nullptr) {
        agent->setisMuted(isMuted);
    }
}


/**
 * @brief Slot called when the flag "can be Frozen" from an agent updated
 * @param peerId
 * @param canBeFrozen
 */
void IngeScapeModelManager::onCanBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen)
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
void IngeScapeModelManager::onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen)
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
void IngeScapeModelManager::onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName)
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
void IngeScapeModelManager::onAgentStateChanged(QString peerId, QString stateName)
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
void IngeScapeModelManager::onAgentHasLogInStream(QString peerId, bool hasLogInStream)
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
void IngeScapeModelManager::onAgentHasLogInFile(QString peerId, bool hasLogInFile)
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
void IngeScapeModelManager::onAgentLogFilePath(QString peerId, QString logFilePath)
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
void IngeScapeModelManager::onAgentDefinitionFilePath(QString peerId, QString definitionFilePath)
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
void IngeScapeModelManager::onAgentMappingFilePath(QString peerId, QString mappingFilePath)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setmappingFilePath(mappingFilePath);
    }
}


/**
 * @brief Slot called when a model of agent has to be deleted
 * @param model
 */
void IngeScapeModelManager::_onAgentModelHasToBeDeleted(AgentM* model)
{
    if (model != nullptr) {
        deleteAgentModel(model);
    }
}


/**
 * @brief Slot called when the definition(s) of an agent (agents grouped by name) must be opened
 * @param definitionsList
 */
void IngeScapeModelManager::_onDefinitionsToOpen(QList<DefinitionM*> definitionsList)
{
    _openDefinitions(definitionsList);
}


/**
 * @brief Slot called when some view models of outputs have been added to an agent(s grouped by name)
 * @param newOutputs
 */
void IngeScapeModelManager::_onOutputsHaveBeenAddedToAgentsGroupedByName(QList<OutputVM*> newOutputs)
{
    AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty() && !newOutputs.isEmpty())
    {
        QStringList newOutputsIds;

        for (OutputVM* output : newOutputs)
        {
            if ((output != nullptr) && !output->uid().isEmpty())
            {
                newOutputsIds.append(output->uid());
            }
        }

        if (!newOutputsIds.isEmpty())
        {
            // Emit the signal "Add Inputs to Editor for Outputs"
            Q_EMIT addInputsToEditorForOutputs(agentsGroupedByName->name(), newOutputsIds, _isMappingActivated);
        }
    }
}


/**
 * @brief Slot called when some view models of outputs will be removed from an agent(s grouped by name)
 * @param oldOutputs
 */
void IngeScapeModelManager::_onOutputsWillBeRemovedFromAgentsGroupedByName(QList<OutputVM*> oldOutputs)
{
    AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty() && !oldOutputs.isEmpty())
    {
        QStringList oldOutputsIds;

        for (OutputVM* output : oldOutputs)
        {
            if ((output != nullptr) && !output->uid().isEmpty())
            {
                oldOutputsIds.append(output->uid());
            }
        }

        if (!oldOutputsIds.isEmpty())
        {
            // Emit the signal "Remove Inputs to Editor for Outputs"
            Q_EMIT removeInputsToEditorForOutputs(agentsGroupedByName->name(), oldOutputsIds, _isMappingActivated);
        }
    }
}


/**
 * @brief Slot called when a view model of agents grouped by name has become useless (no more agents grouped by definition)
 */
void IngeScapeModelManager::_onUselessAgentsGroupedByName()
{
    AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
    if (agentsGroupedByName != nullptr)
    {
        // Delete the view model of agents grouped by name
        deleteAgentsGroupedByName(agentsGroupedByName);
    }
}


/**
 * @brief Slot called when the network data of an agent will be cleared
 * @param peerId
 */
void IngeScapeModelManager::_onNetworkDataOfAgentWillBeCleared(QString peerId)
{
    /*AgentM* agent = qobject_cast<AgentM*>(sender());
    if (agent != nullptr)
    {
        qDebug() << "[Model Manager] on Network Data of agent" << agent->name() << "will be Cleared:" << agent->hostname() << "(" << agent->peerId() << ")";
    }*/

    if (!peerId.isEmpty()) {
        _hashFromPeerIdToAgent.remove(peerId);
    }
}


/**
 * @brief Create a new view model of agents grouped by name
 * @param model
 */
void IngeScapeModelManager::_createAgentsGroupedByName(AgentM* model)
{
    if ((model != nullptr) && !model->name().isEmpty())
    {
        // Create a new view model of agents grouped by name
        AgentsGroupedByNameVM* agentsGroupedByName = new AgentsGroupedByNameVM(model->name(), this);

        // Connect to signals from this new view model of agents grouped by definition
        //connect(agentsGroupedByName, &AgentsGroupedByNameVM::noMoreModelAndUseless, this, &IngeScapeModelManager::_onUselessAgentsGroupedByName);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::noMoreAgentsGroupedByDefinitionAndUseless, this, &IngeScapeModelManager::_onUselessAgentsGroupedByName);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::agentsGroupedByDefinitionHasBeenCreated, this, &IngeScapeModelManager::agentsGroupedByDefinitionHasBeenCreated);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::agentsGroupedByDefinitionWillBeDeleted, this, &IngeScapeModelManager::agentsGroupedByDefinitionWillBeDeleted);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::agentModelHasToBeDeleted, this, &IngeScapeModelManager::_onAgentModelHasToBeDeleted);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::definitionsToOpen, this, &IngeScapeModelManager::_onDefinitionsToOpen);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::outputsHaveBeenAdded, this, &IngeScapeModelManager::_onOutputsHaveBeenAddedToAgentsGroupedByName);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::outputsWillBeRemoved, this, &IngeScapeModelManager::_onOutputsWillBeRemovedFromAgentsGroupedByName);

        _hashFromNameToAgentsGrouped.insert(agentsGroupedByName->name(), agentsGroupedByName);

        _allAgentsGroupsByName.append(agentsGroupedByName);

        // Emit the signal "Agents grouped by name has been created"
        Q_EMIT agentsGroupedByNameHasBeenCreated(agentsGroupedByName);

        // Add the new model of agent
        agentsGroupedByName->addNewAgentModel(model);
    }
}


/**
 * @brief Open a list of definitions (if the definition is already opened, we bring it to front)
 * @param definitionsToOpen
 */
void IngeScapeModelManager::_openDefinitions(QList<DefinitionM*> definitionsToOpen)
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
