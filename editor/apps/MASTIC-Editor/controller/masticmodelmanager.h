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
#include <model/agentm.h>
#include <viewModel/linkvm.h>


/**
 * @brief The MasticModelManager class defines a manager for the data model of MASTIC
 */
class MasticModelManager : public QObject
{
    Q_OBJECT

    // List of all models of agents
    //I2_QOBJECT_LISTMODEL(AgentM, allAgents)


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
     * @brief Add a model of agent
     * @param agent
     */
    void addAgentModel(AgentM* agent);


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
     * @brief Delete a model of Agent
     * @param agant
     */
    void deleteAgentModel(AgentM* agent);


    /**
     * @brief Add a model of agent definition
     * @param definition
     */
    void addAgentDefinition(DefinitionM* definition);


    /**
     * @brief Get the list (of models) of agent definition from a name
     * @param name
     * @return
     */
    QList<DefinitionM*> getAgentDefinitionsListFromName(QString name);


    /**
     * @brief Delete a model of agent definition
     * @param definition
     */
    void deleteAgentDefinition(DefinitionM* definition);


    /**
     * @brief Initialize agents with JSON files
     */
    void initAgentsWithFiles();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a new model of agent has been created
     * @param agent
     */
    void agentModelCreated(AgentM* agent);


    /**
     * @brief Signal emitted when a new model of agent definition has been created
     * @param definition
     * @param agent
     */
    void agentDefinitionCreated(DefinitionM* definition, AgentM* agent);


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
     * @param definition in JSON format
     */
    void onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON);


    /**
     * @brief Slot when an agent mapping has been received and must be processed
     * @param peer Id
     * @param agent name
     * @param mapping in JSON format
     */
    void onMappingReceived(QString peerId, QString agentName, QString mappingJSON);


    /**
     * @brief Slot when an agent quit the network
     * @param peer Id
     * @param agent name
     */
    void onAgentExited(QString peerId, QString agentName);


private:

    /**
     * @brief Update definition variants of a list of definitions with the same name
     * @param definitionName
     */
    void _updateDefinitionVariants(QString definitionName);

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

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
