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

#include <I2Quick.h>

#include <QJsonDocument>



/**
 * @brief Default constructor
 * @param parent
 */
MasticModelManager::MasticModelManager(QObject *parent) : QObject(parent),
    _jsonHelper(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New MASTIC Model Manager";

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);
}


/**
 * @brief Destructor
 */
MasticModelManager::~MasticModelManager()
{
    // Delete all models of agents
    //qDeleteAll(_allAgentsModel);

    qInfo() << "Delete MASTIC Model Manager";
}


/**
 * @brief Initialize agents with JSON files
 */
void MasticModelManager::initAgentsWithFiles()
{
    QStringList nameFilters;
    nameFilters << "*.json";

    // Get the root path of our application ([DocumentsLocation]/MASTIC/)
    QString rootDirectoryPath = I2Utils::getOrCreateAppRootPathInDocumentDir("MASTIC");

    //------------------------------
    //
    // Agents definitions
    //
    //------------------------------
    QString agentsDefinitionsDirectoryPath = QString("%1AgentsDefinitions").arg(rootDirectoryPath);
    QDir agentsDefinitionsDirectory(agentsDefinitionsDirectoryPath);
    if (agentsDefinitionsDirectory.exists())
    {
        QFileInfoList agentsDefinitionsFilesList = agentsDefinitionsDirectory.entryInfoList(nameFilters);
        qInfo() << agentsDefinitionsFilesList.count() << "files in directory" << agentsDefinitionsDirectoryPath;

        // Traverse the list of JSON files
        foreach (QFileInfo fileInfo, agentsDefinitionsFilesList)
        {
            qDebug() << "File" << fileInfo.fileName() << "at" << fileInfo.absoluteFilePath();

            QFile jsonFile(fileInfo.absoluteFilePath());
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();

                // Create a model of agent definition with JSON
                DefinitionM* definition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
                if (definition != NULL)
                {
                    // Create a new model of agent with the name of the definition
                    AgentM* agent = new AgentM(definition->name(), this);

                    // Add this new model of agent
                    addAgentModel(agent);

                    // Emit the signal "Agent Model Created"
                    Q_EMIT agentModelCreated(agent);

                    // Add this new model of agent definition
                    addAgentDefinition(definition);

                    // Emit the signal "Agent Definition Created"
                    Q_EMIT agentDefinitionCreated(definition, agent);
                }

                jsonFile.close();
            }
            else
            {
                qCritical() << "Can not open file" << fileInfo.absoluteFilePath();
            }
        }
    }


    //------------------------------
    //
    // Agents mappings
    //
    //------------------------------
    QString agentsMappingsDirectoryPath = QString("%1AgentsMappings").arg(rootDirectoryPath);
    QDir agentsMappingsDirectory(agentsMappingsDirectoryPath);
    if (agentsMappingsDirectory.exists())
    {
        QFileInfoList agentsMappingsFilesList = agentsMappingsDirectory.entryInfoList(nameFilters);
        qInfo() << agentsMappingsFilesList.count() << "files in directory" << agentsMappingsDirectoryPath;

        // Traverse the list of JSON files
        foreach (QFileInfo fileInfo, agentsMappingsFilesList)
        {
            qDebug() << "File" << fileInfo.fileName() << "at" << fileInfo.absoluteFilePath();

            QFile jsonFile(fileInfo.absoluteFilePath());
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();

                // Create a model of agent mapping with JSON
                AgentMappingM* agentMapping = _jsonHelper->createModelOfAgentMapping("agentUndefined", byteArrayOfJson);
                if (agentMapping != NULL)
                {
                    //TODOESTIA : manage a mapping
                }

                jsonFile.close();
            }
            else
            {
                qCritical() << "Can not open file" << fileInfo.absoluteFilePath();
            }
        }
    }
}


/**
 * @brief Slot when an agent enter the network
 * @param peerId
 * @param agentName
 * @param agentAddress
 * @param pid
 * @param hostname
 * @param executionPath
 * @param canBeFrozen
 */
void MasticModelManager::onAgentEntered(QString peerId, QString agentName, QString agentAddress, int pid, QString hostname, QString executionPath, bool canBeFrozen)
{
    if (!peerId.isEmpty() && !agentName.isEmpty() && !agentAddress.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);

        // An agent with this peer id already exist
        if (agent != NULL)
        {
            qInfo() << "The agent" << agentName << "with peer id" << peerId << "and address" << agentAddress << "is back on the network !";

            // Update the status
            agent->setstatus(AgentStatus::ON);
        }
        // New peer id
        else
        {
            // Create a new model of agent
            agent = new AgentM(agentName, peerId, agentAddress, this);

            agent->sethostname(hostname);
            agent->setexecutionPath(executionPath);
            agent->setpid(pid);
            agent->setcanBeFrozen(canBeFrozen);
            agent->setstatus(AgentStatus::ON);

            _mapFromPeerIdToAgentM.insert(peerId, agent);

            // Add this new model of agent
            addAgentModel(agent);

            // Emit the signal "Agent Model Created"
            Q_EMIT agentModelCreated(agent);
        }
    }
}


/**
 * @brief Slot when an agent definition has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param definition in JSON format
 */
void MasticModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
{
    Q_UNUSED(agentName)

    if (!definitionJSON.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if(agent != NULL)
        {
            QByteArray byteArrayOfJson = definitionJSON.toUtf8();

            // Create a model of agent definition with JSON
            DefinitionM* definition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
            if (definition != NULL)
            {
                // Add this new model of agent definition
                addAgentDefinition(definition);

                // Emit the signal "Agent Definition Created"
                Q_EMIT agentDefinitionCreated(definition, agent);
            }
        }
    }
}


/**
 * @brief Slot when an agent mapping has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param mapping in JSON format
 */
void MasticModelManager::onMappingReceived(QString peerId, QString agentName, QString mappingJSON)
{
    if (!mappingJSON.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if(agent != NULL)
        {
            QByteArray byteArrayOfJson = mappingJSON.toUtf8();

            // Create a model of agent mapping with JSON
            AgentMappingM* agentMapping = _jsonHelper->createModelOfAgentMapping(agentName, byteArrayOfJson);
            if (agentMapping != NULL)
            {
                //TODOESTIA : manage a mapping

                // Manage the new (model of) agent mapping
                //_manageNewMappingOfAgent(agentMapping, agent);
            }
        }
    }
}


/**
 * @brief Slot when an agent quit the network
 * @param peer Id
 * @param agent name
 */
void MasticModelManager::onAgentExited(QString peerId, QString agentName)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
    if(agent != NULL)
    {
        qInfo() << "The agent" << agentName << "with peer id" << peerId << "exited from the network !";

        // Update the status
        agent->setstatus(AgentStatus::OFF);

        // FIXME: nothing more ?
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
 * @brief Delete a model of Agent
 * @param agant
 */
void MasticModelManager::deleteAgentModel(AgentM* agent)
{
    if (agent != NULL)
    {
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agent->name());
        agentModelsList.removeOne(agent);

        // Update the list in the map
        _mapFromNameToAgentModelsList.insert(agent->name(), agentModelsList);

        // Free memory
        delete agent;
    }
}


/**
 * @brief Add a model of agent definition
 * @param definition
 */
void MasticModelManager::addAgentDefinition(DefinitionM* definition)
{
    if (definition != NULL)
    {
        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromName(definition->name());
        agentDefinitionsList.append(definition);

        // Update the list in the map
        _mapFromNameToAgentDefinitionsList.insert(definition->name(), agentDefinitionsList);

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(agentDefinitionsList);
    }
}


/**
 * @brief Get the list (of models) of agent definition from a name
 * @param name
 * @return
 */
QList<DefinitionM*> MasticModelManager::getAgentDefinitionsListFromName(QString name)
{
    if (_mapFromNameToAgentDefinitionsList.contains(name)) {
        return _mapFromNameToAgentDefinitionsList.value(name);
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
        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromName(definition->name());
        agentDefinitionsList.removeOne(definition);

        // Update the list in the map
        _mapFromNameToAgentDefinitionsList.insert(definition->name(), agentDefinitionsList);

        // Free memory
        delete definition;

        // Update definition variants of a list of definitions with the same name
        _updateDefinitionVariants(agentDefinitionsList);
    }
}


/**
 * @brief Update definition variants of a list of definitions with the same name
 * @param agentDefinitionsList
 */
void MasticModelManager::_updateDefinitionVariants(QList<DefinitionM*> agentDefinitionsList)
{
    // We can use versions as keys because the list contains only definition with the same name
    QHash<QString, QList<DefinitionM*>> mapFromVersionToDefinitionsList;
    QList<QString> versionsWithVariant;
    QString version;

    foreach (DefinitionM* iterator, agentDefinitionsList)
    {
        if ((iterator != NULL) && !iterator->version().isEmpty())
        {
            // First, reset all
            iterator->setisVariant(false);

            version = iterator->version();
            QList<DefinitionM*> definitionsListForVersion;

            // Other(s) definition(s) have the same version (and the same name)
            if (mapFromVersionToDefinitionsList.contains(version)) {
                definitionsListForVersion = mapFromVersionToDefinitionsList.value(version);

                // The lists of I/O/P must be different to have a variant !
                if (!versionsWithVariant.contains(version))
                {
                    // We compare I/O/P between current iterator and the first one
                    DefinitionM* first = definitionsListForVersion.first();
                    if ((first != NULL) && (first->md5Hash() != iterator->md5Hash())) {
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
                //qDebug() << iterator->name() << iterator->version() << "is variant (" << iterator->md5Hash() << ")";
            }
        }
    }
}

