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

#ifndef AGENTSSUPERVISIONCONTROLLER_H
#define AGENTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>
#include <viewModel/agentvm.h>


/**
 * @brief The AgentsSupervisionController class defines the controller for agents supervision
 */
class AgentsSupervisionController : public QObject
{
    Q_OBJECT

    // Sorted list of agents
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentVM, agentsList)

    // Selected agent in the agents list
    I2_QML_PROPERTY_DELETE_PROOF(AgentVM*, selectedAgent)

public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit AgentsSupervisionController(MasticModelManager* modelManager, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsSupervisionController();
    

    /**
     * @brief Get the list of view models of agent from a name
     * @param name
     * @return
     */
    QList<AgentVM*> getAgentViewModelsListFromName(QString name);


    /**
     * @brief Delete an agent from the list
     * @param agent to delete
     */
    Q_INVOKABLE void deleteAgent(AgentVM* agent);


    /**
     * @brief Open the definition of an agent of the list
     * @param agent which contain the definition
     */
    Q_INVOKABLE void openDefinition(AgentVM* agent);


    /**
     * @brief Export the agents list to default file
     */
    Q_INVOKABLE void exportAgentsListToDefaultFile();


    /**
     * @brief Export the agents list to selected file
     */
    Q_INVOKABLE void exportAgentsListToSelectedFile();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param executionPath
     */
    void commandAskedToLauncher(QString command, QString hostname, QString executionPath);


    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param peerIdsList
     */
    void commandAsked(QString command, QStringList peerIdsList);


    /**
     * @brief Signal emitted when a command for an output must be sent on the network
     * @param command
     * @param outputName
     * @param peerIdsList
     */
    void commandAskedForOutput(QString command, QString outputName, QStringList peerIdsList);


    /**
     * @brief agentDefinitionManaged
     * @param agentName
     * @param definition
     */
    void agentDefinitionManaged(QString agentName, bool isON, DefinitionM* definition);


public Q_SLOTS:

    /**
     * @brief Slot when a new model of agent has been created
     * @param agent
     */
    void onAgentModelCreated(AgentM* agent);


    /**
     * @brief Slot when a new model of agent definition has been created
     * @param definition
     * @param agent
     */
    void onAgentDefinitionCreated(DefinitionM* definition, AgentM* agent);


    /**
     * @brief Slot when the flag "is Muted" from an output of agent updated
     * @param agent
     * @param isMuted
     * @param outputName
     */
    void onIsMutedFromOutputOfAgentUpdated(AgentM* agent, bool isMuted, QString outputName);


private:

    /**
     * @brief Delete the view model of Agent
     * @param agent
     */
    void _deleteAgentViewModel(AgentVM* agent);


    /**
     * @brief Get the agents list to export
     * @return List of pairs <agent name, definition>
     */
    QList<QPair<QString, DefinitionM*>> _getAgentsListToExport();


private:
    // Manager for the data model of MASTIC
    MasticModelManager* _modelManager;

    // Map from agent name to a list of view models of agent
    QHash<QString, QList<AgentVM*>> _mapFromNameToAgentViewModelsList;
};

QML_DECLARE_TYPE(AgentsSupervisionController)

#endif // AGENTSSUPERVISIONCONTROLLER_H
