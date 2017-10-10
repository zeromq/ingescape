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
 * @brief Add a new view model of agent into our list
 * @param definition
 * @param agent
 * @param status
 */
void MasticModelManager::addNewAgentVMToList(DefinitionM* definition, AgentM* agent, AgentStatus::Value status)
{
    Q_UNUSED(status)

    if ((definition != NULL) && (agent != NULL))
    {
        /*// Add our model to the list
        //_allAgentsModel.append(agent);

        //QString newAgentKey = agentModelToAdd->name().replace(" ","").trimmed().toUpper() + agentModelToAdd->version().replace(" ","").trimmed().toUpper();

        QString agentName = agent->name();

        if (!_mapFromNameToAgentM.contains(agentName))
        {
            //_mapFromNameToAgentM.insert(agentName, agent);

            // Create a new view model of agent
            AgentVM* newAgentVM = new AgentVM(agent, this);
            newAgentVM->setdefinition(definition);
            newAgentVM->setstatus(status);

            //_mapFromNameToAgentVM.insert(agentName, newAgentVM);

            // Add our view model to the list
            _allAgentsVM.append(newAgentVM);

            if (!agent->peerId().isEmpty())
            {
                QString peerId = agent->peerId();

                if (!_mapFromPeerIdToAgentM.contains(peerId))
                {
                    _mapFromPeerIdToAgentM.insert(peerId, agent);
                }
                if (!_mapFromPeerIdToAgentVM.contains(peerId))
                {
                    _mapFromPeerIdToAgentVM.insert(peerId, newAgentVM);
                }
            }
        }
        else
        {
            AgentM* agentWithSameNameM = _mapFromNameToAgentM.value(agentName);
            AgentVM* agentWithSameNameVM = _mapFromNameToAgentVM.value(agentName);

            if ((agentWithSameNameM != NULL) && (agentWithSameNameVM != NULL) && (agentWithSameNameVM->definition() != NULL))
            {
                agentWithSameNameVM->setstatus(AgentStatus::ON);

                // FIXME: test all the definition
                if (definition->name() == agentWithSameNameVM->definition()->name())
                {
                    //ClonedAgentVM* clonedAgent
                }
            }
        }

        // Name and version are identical, the agents are potentially the same
        if (_mapAgentsVMPerNameAndVersion.contains(newAgentKey) == true)
        {
            AgentVM* mainAgent = _mapAgentsVMPerNameAndVersion.value(newAgentKey);

            // Case 1 : name and version and defintion are exactly the same.
            if(mainAgent->modelM() != NULL && mainAgent->modelM()->md5Hash().compare(agentModelToAdd->md5Hash()) == 0)
            {
                mainAgent->listIdenticalAgentsVM()->append(newAgentVM);
            }
            // Case 2 : name and version are exactly the same, but the definition is different.
            else {
                mainAgent->listSimilarAgentsVM()->append(newAgentVM);
            }
        }
        // Case 3 : agent does not exists, we simply create a new one
        else {
            _allAgentsVM.append(newAgentVM);

            // Create a new entry for our agent
            _mapAgentsVMPerNameAndVersion.insert(newAgentKey,newAgentVM);
        }*/
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
        if(agent != NULL)
        {
            qWarning() << "The agent is back on the network !";
            // FIXME: TODO ?
        }
        else
        {
            // Create a new model of agent
            agent = new AgentM(agentName, peerId, agentAddress, this);

            agent->sethostname(hostname);
            agent->setexecutionPath(executionPath);
            agent->setpid(pid);
            agent->setcanBeFrozen(canBeFrozen);

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
        qInfo() << "Agent" << agentName << "(" << peerId << ") EXITED";

        // TODO...
    }

    /*if (_mapFromPeerIdToAgentVM.contains(peerId))
    {
        AgentVM* agentVM = _mapFromPeerIdToAgentVM.value(peerId);
        if(agentVM != NULL)
        {
            agentVM->setstatus(AgentStatus::OFF);

            // We don't delete the agent when it ran OFF
            //_mapAgentsVMPerPeerId.remove(peer);
            //deleteAgentVMFromList(agentVM);
        }
    }*/
}


/**
 * @brief Delete an agent from our list
 * @param agent view model
 */
/*void MasticModelManager::deleteAgentVMFromList(AgentVM* agentModelToDelete)
{
    if (agentModelToDelete != NULL && agentModelToDelete->modelM() != NULL && agentModelToDelete->status() != AgentStatus::ON)
    {
        QString agentKey = agentModelToDelete->modelM()->name().replace(" ","").trimmed().toUpper() + agentModelToDelete->modelM()->version().replace(" ","").trimmed().toUpper();

        // Name and version are identical, the agents are potentially the same
        if(_mapAgentsVMPerNameAndVersion.contains(agentKey) == true)
        {
            AgentVM* mainAgent = _mapAgentsVMPerNameAndVersion.value(agentKey);

            if(mainAgent == agentModelToDelete)
            {
                _mapAgentsVMPerNameAndVersion.remove(agentKey);

                if(agentModelToDelete->listIdenticalAgentsVM()->count() > 0
                        || agentModelToDelete->listSimilarAgentsVM()->count() > 0)
                {
                    AgentVM* newMainAgent = NULL;
                    if(agentModelToDelete->listIdenticalAgentsVM()->count() > 0)
                    {
                        newMainAgent = agentModelToDelete->listIdenticalAgentsVM()->at(0);
                        agentModelToDelete->listIdenticalAgentsVM()->remove(newMainAgent);
                    } else if (agentModelToDelete->listSimilarAgentsVM()->count() > 0)
                    {
                        newMainAgent = agentModelToDelete->listSimilarAgentsVM()->at(0);
                        agentModelToDelete->listSimilarAgentsVM()->remove(newMainAgent);
                    }

                    if(newMainAgent != NULL)
                    {
                        newMainAgent->listIdenticalAgentsVM()->append(agentModelToDelete->listIdenticalAgentsVM()->toList());
                        newMainAgent->listSimilarAgentsVM()->append(agentModelToDelete->listSimilarAgentsVM()->toList());

                        _mapAgentsVMPerNameAndVersion.insert(agentKey,newMainAgent);

                        // Remove the agent
                        _allAgentsVM.remove(agentModelToDelete);
                        _allAgentsVM.append(newMainAgent);
                        agentModelToDelete->listIdenticalAgentsVM()->clear();
                        agentModelToDelete->listSimilarAgentsVM()->clear();

                        delete agentModelToDelete;
                        agentModelToDelete = NULL;
                    }
                } else {
                    // Remove the agent
                    _allAgentsVM.remove(agentModelToDelete);
                    delete agentModelToDelete;
                    agentModelToDelete = NULL;
                }
            }
            // The item to delete is from a sub list
            else {
                if(mainAgent->listIdenticalAgentsVM()->contains(agentModelToDelete))
                {
                    mainAgent->listIdenticalAgentsVM()->remove(agentModelToDelete);
                } else if(mainAgent->listSimilarAgentsVM()->contains(agentModelToDelete))
                {
                    mainAgent->listSimilarAgentsVM()->remove(agentModelToDelete);
                }

                // Remove the agent
                _allAgentsVM.remove(agentModelToDelete);
                delete agentModelToDelete;
                agentModelToDelete = NULL;
            }
        }
    }
}*/


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

        if ((agentModelsList.count() == 0) && (agentViewModelsList.count() == 0))
        {
            agentModelsList.append(agent);
            _mapFromNameToAgentModelsList.insert(agentName, agentModelsList);

            // Create a new view model of agent
            AgentVM* agentVM = new AgentVM(agent, this);

            // Peer Id and Address are defined, agent is ON
            if (!agent->peerId().isEmpty() && !agent->address().isEmpty()) {
                agentVM->setstatus(AgentStatus::ON);
            }
            else {
                agentVM->setstatus(AgentStatus::OFF);
            }

            // Add our view model to the list
            _allAgentsVM.append(agentVM);

            agentViewModelsList.append(agentVM);
            _mapFromNameToAgentViewModelsList.insert(agentName, agentViewModelsList);
        }
        else {
            // FIXME: TODO
            qDebug() << "There is already a model of agent for name" << agentName;
        }
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

        if (agentDefinitionsList.count() == 0) {
            agentDefinitionsList.append(definition);
            _mapFromNameToAgentDefinitionsList.insert(definitionName, agentDefinitionsList);

            if (agentViewModelsList.count() == 1) {
                AgentVM* agentVM = agentViewModelsList.first();
                // Check that the definition is not yet defined
                if ((agentVM != NULL) && (agentVM->definition() == NULL))
                {
                    agentVM->setdefinition(definition);
                }
            }
            else {
                // FIXME: TODO
                qDebug() << "There are already a model of agent for name" << agentName;
            }
        }
        else
        {
            // FIXME: TODO
            qDebug() << "There is already a (model of) agent definition for name" << definitionName;
        }
    }
}
