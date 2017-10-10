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
 *
 */

#ifndef MASTICMODELMANAGER_H
#define MASTICMODELMANAGER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/jsonhelper.h>
#include <viewModel/agentvm.h>
#include <viewModel/clonedagentvm.h>
#include <viewModel/linkvm.h>


/**
 * @brief The MasticModelManager class defines a manager for the data model of MASTIC
 */
class MasticModelManager : public QObject
{
    Q_OBJECT

    // List of all models of agents
    //I2_QOBJECT_LISTMODEL(AgentM, allAgents)

    // List of all view model of agents
    I2_QOBJECT_LISTMODEL(AgentVM, allAgentsVM)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit MasticModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticModelManager();
    

    /**
     * @brief Get the model of agent from a Peer Id
     * @param peerId
     * @return
     */
    AgentM* getAgentModelFromPeerId(QString peerId);


    /**
     * @brief Get the list of models of agent from a name
     * @param name
     * @return
     */
    QList<AgentM*> getAgentModelsListFromName(QString name);


    /**
     * @brief Get the list (of models) of agent definition from a name
     * @param name
     * @return
     */
    QList<DefinitionM*> getAgentDefinitionsListFromName(QString name);


    /**
     * @brief Get the list of view models of agent from a name
     * @param name
     * @return
     */
    QList<AgentVM*> getAgentViewModelsListFromName(QString name);


    /**
     * @brief Add a new view model of agent into our list
     * @param definition
     * @param agent
     * @param status
     */
    void addNewAgentVMToList(DefinitionM* definition, AgentM* agent, AgentStatus::Value status);
    

    /**
     * @brief Delete an agent from our list
     * @param agent view model
     */
    //void deleteAgentVMFromList(AgentVM* agentModelToDelete);


Q_SIGNALS:

public Q_SLOTS:

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
    void onAgentEntered(QString peerId, QString agentName, QString agentAddress, int pid, QString hostname, QString executionPath, bool canBeFrozen);
    

    /**
     * @brief Slot when an agent definition has been received and must be processed
     * @param peer Id
     * @param agent name
     * @param definition
     */
    void onDefinitionReceived(QString peerId, QString agentName, QString definition);


    /**
     * @brief Slot when an agent quit the network
     * @param peer Id
     * @param agent name
     */
    void onAgentExited(QString peerId, QString agentName);


private:

    /**
     * @brief Manage the new model of agent
     * @param agent
     */
    void _manageNewModelOfAgent(AgentM* agent);


    /**
     * @brief Manage the new (model of) definition of agent
     * @param definition
     * @param agent
     */
    void _manageNewDefinitionOfAgent(DefinitionM* definition, AgentM* agent);


private:

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // List of all models of agents
    //QList<AgentM*> _allAgentsModel;

    // Map of all agents VM per name and version
    //QHash<QString, AgentVM*> _mapAgentsVMPerNameAndVersion;

    // Map from Peer ID to a model of agent
    QHash<QString, AgentM*> _mapFromPeerIdToAgentM;

    // Map from agent name to a list of models of agent
    QHash<QString, QList<AgentM*>> _mapFromNameToAgentModelsList;

    // Map from "definition name" to a list (of models) of agent definition
    QHash<QString, QList<DefinitionM*>> _mapFromNameToAgentDefinitionsList;

    // Map from agent name to a list of view models of agent
    QHash<QString, QList<AgentVM*>> _mapFromNameToAgentViewModelsList;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
