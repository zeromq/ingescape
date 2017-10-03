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
#include <I2PropertyHelpers.h>

#include <model/jsonhelper.h>
#include <viewModel/agentvm.h>


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
    explicit MasticModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticModelManager();

    /**
     * @brief Add a new agent model into our list
     * @param agent model
     * @param agent status
     */
    void addNewAgentModelToList(AgentM* agentModelToAdd, AgentStatus::Value status);

    /**
     * @brief Delete an agent from our list
     * @param agent view model
     */
    void deleteAgentVMFromList(AgentVM* agentModelToDelete);

signals:

public slots:
    /**
     * @brief Slot on agent entereing into the network
     *        Agent definition has been received and must be processed
     * @param agent name
     * @param agent adress
     * @param agent peer
     * @param agent definition
     */
    void onAgentEntered(QString agentName, QString agentAdress, QString peer, QString definition);

    /**
     * @brief Slot on agent quitting the network
     * @param agent peer
     */
    void onAgentExited(QString peer);

private:

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // List of all models of agents
    QList<AgentM*> _allAgentsModel;

    // Map of all agents VM per name and version
    QHash<QString, AgentVM*> _mapAgentsVMPerNameAndVersion;

    // Map of all agents VM per peer id
    QHash<QString, AgentVM*> _mapAgentsVMPerPeerId;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
