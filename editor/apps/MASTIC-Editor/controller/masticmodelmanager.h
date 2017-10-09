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
     * @param peer Id
     * @param agent name
     * @param agent address
     */
    void onAgentEntered(QString peerId, QString agentName, QString agentAddress);
    

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

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // List of all models of agents
    QList<AgentM*> _allAgentsModel;

    // Map of all agents VM per name and version
    //QHash<QString, AgentVM*> _mapAgentsVMPerNameAndVersion;

    // Map from Peer ID to model of agent
    QHash<QString, AgentM*> _mapFromPeerIdToAgentM;

    // Map from Peer ID to view model of agent
    QHash<QString, AgentVM*> _mapFromPeerIdToAgentVM;

    // Map from Name to model of agent
    QHash<QString, AgentM*> _mapFromNameToAgentM;

    // Map from Name to view model of agent
    QHash<QString, AgentVM*> _mapFromNameToAgentVM;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
