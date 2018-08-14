/*
 *	IngeScape Editor
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

#include <controller/ingescapemodelmanager.h>
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
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit AgentsSupervisionController(IngeScapeModelManager* modelManager,
                                         JsonHelper* jsonHelper,
                                         QObject *parent = nullptr);


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
     * @brief Delete the selected agent from the list
     */
    Q_INVOKABLE void deleteSelectedAgent();


    /**
     * @brief Export the agents list to selected file
     */
    Q_INVOKABLE void exportAgentsListToSelectedFile();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToLauncher(QString command, QString hostname, QString commandLine);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent about one of its output
     * @param peerIdsList
     * @param command
     * @param outputName
     */
    void commandAskedToAgentAboutOutput(QStringList peerIdsList, QString command, QString outputName);


    /**
     * @brief Signal emitted when we have to open the values history of an agent
     * @param agentName
     */
    void openValuesHistoryOfAgent(QString agentName);


    /**
     * @brief Signal emitted when we have to open the "Log Stream" of a list of agents
     * @param models
     */
    void openLogStreamOfAgents(QList<AgentM*> models);


    /**
     * @brief Signal emitted when a previous agent model is replaced by a new one strictly identical
     * @param previousModel
     * @param newModel
     */
    void identicalAgentModelReplaced(AgentM* previousModel, AgentM* newModel);


public Q_SLOTS:

    /**
     * @brief Slot when a new model of agent has been created
     * @param agent
     */
    void onAgentModelCreated(AgentM* agent);


private Q_SLOTS:

    /**
     * @brief Slot called when the definition of a view model of agent changed (with previous and new values)
     * @param previousValue
     * @param newValue
     */
    void _onAgentDefinitionChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue);


    /**
     * @brief Slot called when a different definition is detected on a model of agent
     * (compared to the definition of our view model)
     * @param model
     */
    void _onDifferentDefinitionDetectedOnModelOfAgent(AgentM* model);


    /**
     * @brief Slot called when we have to load an agent definition from a JSON file (path)
     * @param peerIdsList
     * @param definitionFilePath
     */
    void _onLoadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath);


    /**
     * @brief Slot called when we have to load an agent mapping from a JSON file (path)
     * @param mappingFilePath
     */
    void _onLoadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath);


    /**
     * @brief Slot called when we have to download an agent definition to a JSON file (path)
     * @param agentDefinition
     * @param definitionFilePath
     */
    void _onDownloadAgentDefinitionToPath(DefinitionM* agentDefinition, QString definitionFilePath);


    /**
     * @brief Slot called when we have to download an agent mapping to a JSON file (path)
     * @param agentMapping
     * @param mappingFilePath
     */
    void _onDownloadAgentMappingToPath(AgentMappingM* agentMapping, QString mappingFilePath);


private:

    /**
     * @brief Check if we have to merge an agent with another one that have the same definition
     * @param agent
     */
    void _checkHaveToMergeAgent(AgentVM* agent);


    /**
     * @brief Delete the view model of Agent
     * @param agent
     */
    void _deleteAgentViewModel(AgentVM* agent);


    /**
     * @brief Get the agents list to export
     * @return List of pairs <agent name (and parameters to restart), definition>
     */
    QList<QPair<QStringList, DefinitionM*>> _getAgentsListToExport();


private:

    // Manager for the data model of INGESCAPE
    IngeScapeModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Map from agent name to a list of view models of agent
    QHash<QString, QList<AgentVM*>> _mapFromNameToAgentViewModelsList;
};

QML_DECLARE_TYPE(AgentsSupervisionController)

#endif // AGENTSSUPERVISIONCONTROLLER_H
