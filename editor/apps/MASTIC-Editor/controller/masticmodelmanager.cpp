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
        QStringList nameFilters;
        nameFilters << "*.json";

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
                    // Create a new model of agent
                    AgentM* agent = new AgentM(definition->name(), "", this);

                    addNewAgentVMToList(definition, agent, AgentStatus::OFF);
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
 * @brief Destructor
 */
MasticModelManager::~MasticModelManager()
{
    // Delete all VM of agents
    _allAgentsVM.deleteAllItems();

    // Delete all models of agents
    qDeleteAll(_allAgentsModel);

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
    if ((definition != NULL) && (agent != NULL))
    {
        // Add our model to the list
        _allAgentsModel.append(agent);

        //QString newAgentKey = agentModelToAdd->name().replace(" ","").trimmed().toUpper() + agentModelToAdd->version().replace(" ","").trimmed().toUpper();

        QString agentName = agent->name();

        if (!_mapFromNameToAgentM.contains(agentName))
        {
            _mapFromNameToAgentM.insert(agentName, agent);

            // Create a new view model of agent
            AgentVM* newAgentVM = new AgentVM(agent, this);
            newAgentVM->setdefinition(definition);
            newAgentVM->setstatus(status);

            _mapFromNameToAgentVM.insert(agentName, newAgentVM);

            // Add our view model to the list
            _allAgentsVM.append(newAgentVM);

            if (!agent->peerId().isEmpty())
            {
                QString peerID = agent->peerId();

                if (!_mapFromPeerIdToAgentM.contains(peerID))
                {
                    _mapFromPeerIdToAgentM.insert(peerID, agent);
                }
                if (!_mapFromPeerIdToAgentVM.contains(peerID))
                {
                    _mapFromPeerIdToAgentVM.insert(peerID, newAgentVM);
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
        /*if (_mapAgentsVMPerNameAndVersion.contains(newAgentKey) == true)
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
 * @brief Slot on agent entering into the network
 *        Agent definition has been received and must be processed
 * @param agent name
 * @param agent adress
 * @param agent definition
 */
void MasticModelManager::onAgentEntered(QString agentName, QString agentAdress, QString peer, QString definition)
{
    if (!definition.isEmpty())
    {
        QByteArray byteArrayOfJson = definition.toUtf8();

        // Create a model of agent definition with JSON
        DefinitionM* definition = _jsonHelper->createModelOfDefinition(byteArrayOfJson);
        if (definition != NULL)
        {
            // Create a new model of agent
            AgentM* agent = new AgentM(agentName, peer, this);

            // FIXME: networkDevice, IP address or HostName of our agent ?
            agent->setipAddress(agentAdress);

            addNewAgentVMToList(definition, agent, AgentStatus::ON);
        }
    }
}


/**
 * @brief Slot on agent quitting the network
 * @param agent peer id
 */
void MasticModelManager::onAgentExited(QString peer)
{
    if (_mapFromPeerIdToAgentVM.contains(peer))
    {
        AgentVM* agentVM = _mapFromPeerIdToAgentVM.value(peer);
        if(agentVM != NULL)
        {
            agentVM->setstatus(AgentStatus::OFF);

            // We don't delete the agent when it ran OFF
            //_mapAgentsVMPerPeerId.remove(peer);
            //deleteAgentVMFromList(agentVM);
        }
    }
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
