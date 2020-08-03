/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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
#include <controller/ingescapenetworkcontroller.h>


// Threshold beyond which we consider that there are too many values
#define TOO_MANY_VALUES 2000


// Define our singleton instance
// Creates a global and static object of type QGlobalStatic, of name _singletonInstance and that behaves as a pointer to IngeScapeModelManager.
// The object created by Q_GLOBAL_STATIC initializes itself on the first use, which means that it will not increase the application or the library's load time.
// Additionally, the object is initialized in a thread-safe manner on all platforms.
Q_GLOBAL_STATIC(IngeScapeModelManager, _singletonInstance)


/**
 * @brief Get our singleton instance
 * @return
 */
IngeScapeModelManager* IngeScapeModelManager::instance()
{
    return _singletonInstance;
}


/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* IngeScapeModelManager::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return _singletonInstance;
}


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeModelManager::IngeScapeModelManager(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Agents grouped are sorted on their name (alphabetical order)
    _allAgentsGroupsByName.setSortProperty("name");
}


/**
 * @brief Destructor
 */
IngeScapeModelManager::~IngeScapeModelManager()
{
    qInfo() << "Delete IngeScape Model Manager";

    // Free memory for published values
    _publishedValues.deleteAllItems();

    // Delete all (models of) actions
    //deleteAllActions();
    qDeleteAll(_hashFromUidToModelOfAction);

    // Free memory for hosts
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
}


/**
 * @brief Create a new model of agent with a name, a definition (can be NULL) and some properties
 * @param agentName
 * @param definition optional (NULL by default)
 * @param hostName optional (default value)
 * @param commandLine optional (empty by default)
 * @param peerId optional (empty by default)
 * @param ipAddress optional (empty by default)
 * @param isON optional (false by default)
 * @return
 */
/*AgentM* IngeScapeModelManager::createAgentModel(QString agentName,
                                                DefinitionM* definition,
                                                QString hostName,
                                                QString commandLine,
                                                QString peerId,
                                                QString ipAddress,
                                                bool isON)
{
    AgentM* agent = nullptr;

    if (!agentName.isEmpty())
    {
        // Create a new model of agent
        agent = new AgentM(agentName,
                           peerId,
                           ipAddress,
                           hostName,
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

        // If there is a host with this name...
        if (_hashFromNameToHost.contains(hostName))
        {
            // ...this agent can be restarted
            agent->setcanBeRestarted(true);
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
}*/

AgentM* IngeScapeModelManager::createAgentModel(PeerM* peer,
                                                QString agentUid,
                                                QString agentName,
                                                DefinitionM* definition,
                                                bool isON)
{
    AgentM* agent = nullptr;

    if (!agentUid.isEmpty() && !agentName.isEmpty())
    {
        agent = new AgentM(agentName,
                           agentUid,
                           peer,
                           isON,
                           this);

        if (definition != nullptr) {
            agent->setdefinition(definition);
        }

        // Connect to signals from this new agent
        connect(agent, &AgentM::networkDataWillBeCleared, this, &IngeScapeModelManager::_onNetworkDataOfAgentWillBeCleared);

        _hashFromUidToAgent.insert(agentUid, agent);

        // FIXME hostName needed
        // If there is a host with this name...
        /*if (_hashFromNameToHost.contains(hostName))
        {
            // ...this agent can be restarted
            agent->setcanBeRestarted(true);
        }*/

        // Emit the signal "Agent Model has been Created"
        Q_EMIT agentModelHasBeenCreated(agent);

        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(agentName);
        if (agentsGroupedByName != nullptr) {
            agentsGroupedByName->addNewAgentModel(agent);
        }
        else {
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
    if ((agent != nullptr) && !agent->uid().isEmpty() && !agent->name().isEmpty())
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

        _hashFromUidToAgent.remove(agent->uid());

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
    return _hashFromNameToHost.value(hostName, nullptr);
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

    if ((host != nullptr) && (host->peer() != nullptr)) {
        return host->peer()->uid();
    }
    else {
        return "";
    }
}


/**
 * @brief Get the model of agent from a UID
 */
AgentM* IngeScapeModelManager::getAgentModelFromUid(QString uid)
{
    return _hashFromUidToAgent.value(uid, nullptr);
}


/**
 * @brief Get the (view model of) agents grouped for a name
 * @param name
 * @return
 */
AgentsGroupedByNameVM* IngeScapeModelManager::getAgentsGroupedForName(QString name)
{
    return _hashFromNameToAgentsGrouped.value(name, nullptr);
}


/**
 * @brief Get the model of action with its (unique) id
 * @param actionId
 * @return
 */
ActionM* IngeScapeModelManager::getActionWithId(int actionId)
{
    return _hashFromUidToModelOfAction.value(actionId, nullptr);
}


/**
 * @brief Store a new model of action
 * @param action
 */
void IngeScapeModelManager::storeNewAction(ActionM* action)
{
    if ((action != nullptr) && !_hashFromUidToModelOfAction.contains(action->uid()))
    {
        _hashFromUidToModelOfAction.insert(action->uid(), action);
    }
}


/**
 * @brief Delete a model of action
 * @param action
 */
void IngeScapeModelManager::deleteAction(ActionM* action)
{
    if (action != nullptr)
    {
        Q_EMIT actionModelWillBeDeleted(action);

        int actionId = action->uid();

        // Remove action form the hash table
        _hashFromUidToModelOfAction.remove(actionId);

        // Free memory
        delete action;

        // Free the UID of the action model
        IngeScapeUtils::freeUIDofActionM(actionId);
    }
}


/**
 * @brief Delete all (models of) actions
 */
void IngeScapeModelManager::deleteAllActions()
{
    //qDeleteAll(_hashFromUidToModelOfAction);

    for (ActionM* action : _hashFromUidToModelOfAction.values())
    {
        deleteAction(action);
    }
    _hashFromUidToModelOfAction.clear();
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
 * @brief Import an agent (with only its definition) or an agents list from a selected file
 * @return
 */
bool IngeScapeModelManager::importAgentOrAgentsListFromSelectedFile()
{
    bool success = true;

    // "File Dialog" to get the file (path) to open
    QString filePath = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Open an agent(s) definition"),
                                                    IngeScapeUtils::getRootPath(),
                                                    tr("Definition (*.igsdefinition *.json)")
                                                    );

    if (!filePath.isEmpty())
    {
        // Import an agent (with only its definition) or an agents list from the file path
        success = importAgentOrAgentsListFromFilePath(filePath);
    }

    return success;
}


/**
 * @brief Import an agent (with only its definition) or an agents list from a file path
 * @param filePath
 * @return
 */
bool IngeScapeModelManager::importAgentOrAgentsListFromFilePath(QString filePath)
{
    bool success = true;

    if (!filePath.isEmpty())
    {
        QFile jsonFile(filePath);
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
                    DefinitionM* agentDefinition = JsonHelper::createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
                    if (agentDefinition != nullptr)
                    {
                        // FIXME importAgentOrAgentsListFromFilePath createAgentModel

                        // Create a new model of agent with the name of the definition
                        //createAgentModel(agentDefinition->name(), agentDefinition);
                    }
                    // An error occured, the definition is NULL
                    else {
                        qWarning() << "The file" << filePath << "does not contain an agent definition !";

                        success = false;
                    }
                }
            }
            else {
                qWarning() << "The file" << filePath << "does not contain one or several agent definition(s) !";

                success = false;
            }
        }
        else {
            qCritical() << "Can not open file" << filePath;

            success = false;
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
                        agentDefinition = JsonHelper::createModelOfAgentDefinitionFromJSON(jsonDefinition.toObject());
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
                        //createAgentModel(agentName, copyOfDefinition);

                        // FIXME importAgentsListFromJson createAgentModel
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
                                //QJsonValue jsonPeerId = jsonClone.value("peerId");
                                //QJsonValue jsonAddress = jsonClone.value("address");

                                //if (jsonHostname.isString() && jsonCommandLine.isString() && jsonPeerId.isString() && jsonAddress.isString())
                                if (jsonHostname.isString() && jsonCommandLine.isString())
                                {
                                    QString hostName = jsonHostname.toString();
                                    QString commandLine = jsonCommandLine.toString();
                                    //QString peerId = jsonPeerId.toString();
                                    //QString ipAddress = jsonAddress.toString();

                                    //if (!hostName.isEmpty() && !commandLine.isEmpty() && !peerId.isEmpty() && !ipAddress.isEmpty())
                                    if (!hostName.isEmpty() && !commandLine.isEmpty())
                                    {
                                        // Emit the signal "Previous Host Parsed"
                                        Q_EMIT previousHostParsed(hostName);

                                        //qDebug() << "Clone of" << agentName << "on" << hostname << "with command line" << commandLine << "(" << peerId << ")";

                                        // Make a copy of the definition
                                        DefinitionM* copyOfDefinition = nullptr;
                                        if (agentDefinition != nullptr) {
                                            copyOfDefinition = agentDefinition->copy();
                                        }

                                        // Create a new model of agent
                                        /*createAgentModel(agentName,
                                                         copyOfDefinition,
                                                         hostName,
                                                         commandLine);*/

                                        // FIXME importAgentsListFromJson createAgentModel
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
    return success;
}


/**
 * @brief Simulate an exit for each agent ON
 */
void IngeScapeModelManager::simulateExitForEachAgentON()
{
    for (AgentM* agent : _hashFromUidToAgent.values())
    {
        if ((agent != nullptr) && (agent->peer() != nullptr)
            && agent->isON())
        {   
            // Simulate an exit for this agent
            onAgentExited(agent->peer(), agent->uid());
        }
    }
}


/**
 * @brief Simulate an exit for each launcher
 */
void IngeScapeModelManager::simulateExitForEachLauncher()
{
    for (HostM* host : _hashFromNameToHost.values())
    {
        if ((host != nullptr) && (host->peer() != nullptr)
                && (host->name() != HOSTNAME_NOT_DEFINED))
        {
            // Simulate an exit for this host
            onLauncherExited(host->peer());
        }
    }
}


/**
 * @brief Delete agents OFF
 * @return namesListOfAgentsON
 */
QStringList IngeScapeModelManager::deleteAgentsOFF()
{   
    QStringList namesListOfAgentsON = QStringList();

    for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
    {
        if (agentsGroupedByName != nullptr)
        {
            // ON
            if (agentsGroupedByName->isON())
            {
                namesListOfAgentsON.append(agentsGroupedByName->name());

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

    return namesListOfAgentsON;
}


/**
 * @brief Delete all published values
 */
void IngeScapeModelManager::deleteAllPublishedValues()
{
    _publishedValues.deleteAllItems();
}


/**
 * @brief Slot called when a peer of agent(s) enter the network
 */
void IngeScapeModelManager::onPeerOfAgentsEntered(PeerM* peer)
{
    if ((peer != nullptr) && (peer->igsType() == IngeScapeTypes::AGENT))
    {
        // FIXME onPeerOfAgentsEntered Nothing to do ?

        /*AgentM* agent = getAgentModelFromPeerId(peerId);

        // An agent with this peer id already exist
        if (agent != nullptr)
        {
            qInfo() << "The agent" << agentName << "with peer id" << peerId << "on" << hostname << "(" << ipAddress << ") is back on the network !";

            // Useless !
            //agent->sethostname(hostname);
            //agent->setcommandLine(commandLine);

            // Usefull ?
            agent->setloggerPort(loggerPort);

            // Update the state (flag "is ON")
            agent->setisON(true);

            // Emit the signal "Agent Model Back On Network"
            Q_EMIT agentModelBackOnNetwork(agent);
        }
        // New peer id
        else
        {
            // Create a new model of agent
            agent = createAgentModel(agentName,
                                     nullptr,
                                     hostname,
                                     commandLine,
                                     peerId,
                                     ipAddress,
                                     true);

            if (agent != nullptr) {
                agent->setloggerPort(loggerPort);
            }
        }*/
    }
}


/**
 * @brief Slot called when a peer of agent(s) quit the network
 */
void IngeScapeModelManager::onPeerOfAgentsExited(PeerM* peer)
{
    if ((peer != nullptr) && (peer->igsType() == IngeScapeTypes::AGENT))
    {
        for (AgentM* agent : _hashFromUidToAgent.values())
        {
            if ((agent != nullptr) && (agent->peer() != nullptr)
                    && (agent->peer() == peer))
            {
                //onAgentExited(peer, agent->uid());
                agent->setisON(false);

                //agent->setpeer(nullptr); // Loose data about hostname, ip, ...
            }
        }
    }
}


void IngeScapeModelManager::onAgentExited(PeerM* peer, QString agentUid)
{
    Q_UNUSED(peer)

    AgentM* agent = getAgentModelFromUid(agentUid);
    if (agent != nullptr)
    {
        qInfo() << "The agent" << agent->name() << "with UID" << agentUid << "exited from the network !";

        agent->setisON(false);
    }
}


/**
 * @brief Slot called when a launcher enter the network
 */
void IngeScapeModelManager::onLauncherEntered(PeerM* peer)
{
    if ((peer != nullptr) && (peer->igsType() == IngeScapeTypes::LAUNCHER)
            && !peer->hostname().isEmpty())
    {
        QString hostname = peer->hostname();

        // Get the model of host with the name
        HostM* host = getHostModelWithName(hostname);
        if (host == nullptr)
        {
            host = new HostM(hostname,
                             peer,
                             this);
            _hashFromNameToHost.insert(host->name(), host);

            Q_EMIT hostModelHasBeenCreated(host);
        }
        else
        {
            host->setpeer(peer);
        }

        // Traverse the list of all agents grouped by name
        for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
        {
            if (agentsGroupedByName != nullptr)
            {
                // Traverse the list of its models
                for (AgentM* agent : agentsGroupedByName->models()->toList())
                {
                    if ((agent != nullptr) && (agent->peer() != nullptr)
                            && (agent->peer()->hostname() == hostname)
                            && !agent->peer()->commandLine().isEmpty())
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
 */
void IngeScapeModelManager::onLauncherExited(PeerM* peer)
{
    if ((peer != nullptr) && (peer->igsType() == IngeScapeTypes::LAUNCHER)
            && !peer->hostname().isEmpty())
    {
        QString hostname = peer->hostname();

        // Get the model of host with the name
        HostM* host = getHostModelWithName(hostname);
        if (host != nullptr)
        {
            Q_EMIT hostModelWillBeDeleted(host);

            _hashFromNameToHost.remove(hostname);

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
                    if ((agent != nullptr) && (agent->peer() != nullptr)
                            && (agent->peer()->hostname() == hostname))
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
 */
void IngeScapeModelManager::onDefinitionReceived(PeerM* peer, QString agentUid, QString agentName, QString definitionJSON)
{
    if (peer != nullptr)
    {
        // Create the new model of agent definition from JSON
        DefinitionM* newDefinition = JsonHelper::createModelOfAgentDefinitionFromBytes(definitionJSON.toUtf8());

        AgentM* agent = getAgentModelFromUid(agentUid);
        if (agent != nullptr) // An agent with this uid already exist
        {
            if (agent->name() != agentName)
            {
                qInfo() << "The name of agent with UID" << agentUid << "has changed from" << agent->name() << "to" << agentName;

                // FIXME TODO: Manage renamed agent !
                //agent->setname(agentName); Manage AgentsGroupedByName...
            }

            if (!agent->isON())
            {
                qInfo() << "The agent" << agentName << "with UID" << agentUid << "on" << peer->hostname() << "(" << peer->uid() << ") is back on the network !";
                agent->setisON(true);
            }

            DefinitionM* previousDefinition = agent->definition();

            if (newDefinition != nullptr)
            {
                // The 2 definitions are strictly identical (only when an agent is back on the network !)
                if ((previousDefinition != nullptr) && (*previousDefinition == *newDefinition))
                {
                    qDebug() << "The received definition" << newDefinition->name() << "(version" << newDefinition->version() << ") is exactly the same";

                    delete newDefinition; // Free memory (new definition will not be used)
                }
                else
                {
                    agent->setdefinition(newDefinition); // Set this new definition to the agent

                    if (previousDefinition != nullptr) {
                        delete previousDefinition; // Free memory (previous definition is not used anymore)
                    }
                }
            }
        }
        else // New uid --> new agent
        {
            agent = createAgentModel(peer,
                                     agentUid,
                                     agentName,
                                     newDefinition,
                                     true);
        }
    }
    else {
        qCritical() << Q_FUNC_INFO << "We receive a definition for the agent" << agentName << "(" << agentUid << ") but the peer is NULL !";
    }
}


/**
 * @brief Slot called when an agent mapping has been received and must be processed
 */
void IngeScapeModelManager::onMappingReceived(PeerM* peer, QString agentUid, QString mappingJSON)
{
    Q_UNUSED(peer)

    AgentM* agent = getAgentModelFromUid(agentUid);
    if (agent != nullptr)
    {
        // Save the previous agent mapping
        AgentMappingM* previousMapping = agent->mapping();

        AgentMappingM* newMapping = nullptr;

        if (mappingJSON.isEmpty())
        {
            QString mappingName = QString("EMPTY MAPPING of %1").arg(agent->name());
            newMapping = new AgentMappingM(mappingName, "", "");
        }
        else
        {
            // Create the new model of agent mapping from the JSON
            newMapping = JsonHelper::createModelOfAgentMappingFromBytes(agent->name(), mappingJSON.toUtf8());
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

            //int numberOfDeletedValues = _publishedValues.count() - numberOfKeptValues;
            //qDebug() << _publishedValues.count() << "values: we delete the" << numberOfDeletedValues << "oldest values and kept the" << numberOfKeptValues << "newest values";

            // Get values to delete
            QList<PublishedValueM*> valuesToDelete;
            for (int index = numberOfKeptValues; index < _publishedValues.count(); index++)
            {
                valuesToDelete.append(_publishedValues.at(index));
            }

            // Remove all values from our list at once
            // => our QML list and associated filters will only be updated once
            _publishedValues.removeRows(numberOfKeptValues, valuesToDelete.count());

            // Delete values
            qDeleteAll(valuesToDelete);
            valuesToDelete.clear();
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

        if (!newOutputsIds.isEmpty() && (IngeScapeNetworkController::instance() != nullptr))
        {
            // Add inputs to our application for these agent outputs
            IngeScapeNetworkController::instance()->addInputsToOurApplicationForAgentOutputs(agentsGroupedByName->name(), newOutputsIds);
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

        if (!oldOutputsIds.isEmpty() && (IngeScapeNetworkController::instance() != nullptr))
        {
            // Remove inputs from our application for these agent outputs
            IngeScapeNetworkController::instance()->removeInputsFromOurApplicationForAgentOutputs(agentsGroupedByName->name(), oldOutputsIds);
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

    // FIXME _onNetworkDataOfAgentWillBeCleared
    /*if (!peerId.isEmpty()) {
        _hashFromPeerIdToAgent.remove(peerId);
    }*/
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
