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
                    // FIXME: test first existing agents and definitions with the name "definition->name()"

                    // Create a new model of agent
                    //AgentM* agent = new AgentM(definition->name(), this);

                    // Manage the new model of agent
                    //_manageNewModelOfAgent(agent);

                    // Manage the new (model of) definition of agent
                    //_manageNewDefinitionOfAgent(definition, agent);
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

            // TODO: ESTIA
        }
    }
}


/**
 * @brief Destructor
 */
MasticModelManager::~MasticModelManager()
{
    // Delete all VM of agents
    _allAgentsVM.deleteAllItems();

    // Delete all models of agents
    //qDeleteAll(_allAgentsModel);

    qInfo() << "Delete MASTIC Model Manager";
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

            // Manage the new model of agent
            _manageNewModelOfAgent(agent);
        }
    }
}


/**
 * @brief Slot when an agent definition has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param definition
 */
void MasticModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definition)
{
    Q_UNUSED(agentName)

    if (!definition.isEmpty())
    {
        AgentM* agent = getAgentModelFromPeerId(peerId);
        if(agent != NULL)
        {
            QByteArray byteArrayOfJson = definition.toUtf8();

            // Create a model of agent definition with JSON
            DefinitionM* definition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
            if (definition != NULL)
            {
                // Manage the new (model of) definition of agent
                _manageNewDefinitionOfAgent(definition, agent);
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
 * @brief Get the list of view models of agent from a name
 * @param name
 * @return
 */
QList<AgentVM*> MasticModelManager::getAgentViewModelsListFromName(QString name)
{
    if (_mapFromNameToAgentViewModelsList.contains(name)) {
        return _mapFromNameToAgentViewModelsList.value(name);
    }
    else {
        return QList<AgentVM*>();
    }
}


/**
 * @brief Manage the new model of agent
 * @param agent
 */
void MasticModelManager::_manageNewModelOfAgent(AgentM* agent)
{
    if (agent != NULL)
    {
        QString agentName = agent->name();

        // Get the list of models and view models of agent from a name
        QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agentName);

        // We don't have yet a definition for this agent, so we create a new VM until we will get its definition

        agentModelsList.append(agent);
        _mapFromNameToAgentModelsList.insert(agentName, agentModelsList);

        // Create a new view model of agent
        AgentVM* agentVM = new AgentVM(agent, this);

        // Add our view model to the list
        _allAgentsVM.append(agentVM);

        agentViewModelsList.append(agentVM);
        _mapFromNameToAgentViewModelsList.insert(agentName, agentViewModelsList);

        /*if ((agentModelsList.count() == 0) && (agentViewModelsList.count() == 0))
        {
            agentModelsList.append(agent);
            _mapFromNameToAgentModelsList.insert(agentName, agentModelsList);

            // Create a new view model of agent
            AgentVM* agentVM = new AgentVM(agent, this);

            // Add our view model to the list
            _allAgentsVM.append(agentVM);

            agentViewModelsList.append(agentVM);
            _mapFromNameToAgentViewModelsList.insert(agentName, agentViewModelsList);
        }
        else
        {
            // FIXME: TODO
            qDebug() << "There is already a model of agent for name" << agentName;

            foreach (AgentM* iterator, agentModelsList) {
                if (iterator != NULL)
                {
                    // Same agent name and same hostname
                    if ((iterator->address() == agent->address()) && (iterator->hostname() == agent->hostname()))
                    {
                        qDebug() << "Same agent name and same hostname";
                    }
                }
            }
        }*/
    }
}


/**
 * @brief Manage the new (model of) definition of agent
 * @param definition
 * @param agent
 */
void MasticModelManager::_manageNewDefinitionOfAgent(DefinitionM* definition, AgentM* agent)
{
    if ((definition != NULL) && (agent != NULL))
    {
        QString definitionName = definition->name();
        QString agentName = agent->name();

        // Get the list (of models) of agent definition from a name
        QList<DefinitionM*> agentDefinitionsList = getAgentDefinitionsListFromName(definitionName);

        // Get the list of models and view models of agent from a name
        //QList<AgentM*> agentModelsList = getAgentModelsListFromName(agentName);
        QList<AgentVM*> agentViewModelsList = getAgentViewModelsListFromName(agentName);

        // New name of definition
        if (agentDefinitionsList.count() == 0)
        {
            // Insert the list in the map
            agentDefinitionsList.append(definition);
            _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

            foreach (AgentVM* agentVM, agentViewModelsList)
            {
                // If this VM contains our model of agent
                if ((agentVM != NULL) && agentVM->models()->contains(agent))
                {
                    // Check that the definition is not yet defined
                    if (agentVM->definition() == NULL)
                    {
                        agentVM->setdefinition(definition);
                    }
                    else {
                        qCritical() << "There is already a definition (" << agentVM->definition()->name() << ") for our VM of agent" << agentName;
                    }
                    break;
                }
            }
        }
        // Already a definition with this name
        else
        {
            qDebug() << "There is already an agent definition for name" << definitionName;

            DefinitionM* sameDefinition = NULL;

            foreach (DefinitionM* iterator, agentDefinitionsList) {
                if ((iterator != NULL)
                        &&
                        // Same version
                        (iterator->version() == definition->version())
                        &&
                        // Same Inputs, Outputs and Parameters
                        (iterator->md5Hash() == definition->md5Hash()))
                {
                    qDebug() << "There is exactly the same agent definition for name" << definitionName << "and version" << definition->version();

                    // Exactly the same definition
                    sameDefinition = iterator;
                    break;
                }
            }

            // Definition is different
            if (sameDefinition == NULL)
            {
                // Update the list in the map
                agentDefinitionsList.append(definition);
                _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

                foreach (AgentVM* agentVM, agentViewModelsList)
                {
                    // If this VM contains our model of agent
                    if ((agentVM != NULL) && agentVM->models()->contains(agent))
                    {
                        // Check that the definition is not yet defined
                        if (agentVM->definition() == NULL)
                        {
                            agentVM->setdefinition(definition);
                        }
                        else {
                            qCritical() << "There is already a definition (" << agentVM->definition()->name() << ") for our VM of agent" << agentName;
                        }
                        break;
                    }
                }
            }
            // Exactly the same definition
            else
            {
                AgentVM* uselessAgentVM = NULL;

                foreach (AgentVM* agentVM, agentViewModelsList)
                {
                    // TODO + If this VM contains our model of agent
                    if ((agentVM != NULL) && (agentVM->definition() == NULL) && agentVM->models()->contains(agent)) {
                        uselessAgentVM = agentVM;
                        break;
                    }
                }

                if (uselessAgentVM != NULL) {
                    // Update the list in the map
                    agentViewModelsList.removeOne(uselessAgentVM);
                    _mapFromNameToAgentViewModelsList.insert(agentName, agentViewModelsList);

                    // Remove this view model from the list
                    _allAgentsVM.remove(uselessAgentVM);

                    // Free memory
                    delete uselessAgentVM;
                }

                // Traverse the updated list of view models
                foreach (AgentVM* agentVM, agentViewModelsList)
                {
                    if ((agentVM != NULL) && (agentVM->definition() == sameDefinition))
                    {
                        bool isSameModel = false;
                        QList<AgentM*> models = agentVM->models()->toList();
                        for (int i = 0; i < models.count(); i++)
                        {
                            AgentM* model = models.at(i);

                            // Same address and status is OFF
                            if ((model != NULL) && (model->address() == agent->address()) && (model->status() == AgentStatus::OFF))
                            {
                                isSameModel = true;

                                // Replace the model
                                agentVM->models()->replace(i, agent);

                                qDebug() << "Replace model of agent" << agentName << "on" << agent->address();

                                // Free previous model of agent
                                delete model;

                                // break loop on models
                                break;
                            }
                        }

                        if (!isSameModel) {
                            // Add the model of agent to the list of the VM
                            agentVM->models()->append(agent);

                            qDebug() << "Add model of agent" << agentName << "on" << agent->address();
                        }

                        // Free useless definition
                        delete definition;

                        // break loop on view models
                        break;
                    }
                }
            }
        }
    }
}
