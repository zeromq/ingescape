/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef INGESCAPEMODELMANAGER_H
#define INGESCAPEMODELMANAGER_H

#include <QObject>
//#include <QtQml>
#include <I2PropertyHelpers.h>
#include <model/enums.h>
#include <model/jsonhelper.h>
#include <model/publishedvaluem.h>
#include <viewModel/agentsgroupedbynamevm.h>

static const QString VERSION_JSON_PLATFORM = QString("1.0");


/**
 * @brief The IngeScapeModelManager class defines the manager for the data model of INGESCAPE
 */
class IngeScapeModelManager : public QObject
{
    Q_OBJECT

    // List of all groups (of agents) grouped by name
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentsGroupedByNameVM, allAgentsGroupsByName)

    // List of all published values
    I2_QOBJECT_LISTMODEL(PublishedValueM, publishedValues)


public:
    /**
     * @brief Constructor
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    explicit IngeScapeModelManager(JsonHelper* jsonHelper,
                                   QString rootDirectoryPath,
                                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeModelManager();


    /**
     * @brief Create a new model of agent with a name, a definition (can be NULL) and some properties
     * @param agentName
     * @param definition optional (NULL by default)
     * @param peerId optional (empty by default)
     * @param ipAddress optional (empty by default)
     * @param hostname optional (default value)
     * @param commandLine optional (empty by default)
     * @param isON optional (false by default)
     * @return
     */
    AgentM* createAgentModel(QString agentName,
                             DefinitionM* definition = nullptr,
                             QString peerId = "",
                             QString ipAddress = "",
                             QString hostname = HOSTNAME_NOT_DEFINED,
                             QString commandLine = "",
                             bool isON = false);


    /**
     * @brief Delete a model of agent
     * @param agent
     */
    void deleteAgentModel(AgentM* agent);


    /**
     * @brief Delete a view model of agents grouped by name
     * @param agentsGroupedByName
     */
    void deleteAgentsGroupedByName(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Get the model of host with a name
     * @param hostName
     * @return
     */
    //HostM* getHostModelWithName(QString hostName);


    /**
     * @brief Get the peer id of the Launcher on a host
     * @param hostName
     * @return
     */
    //QString getPeerIdOfLauncherOnHost(QString hostName);


    /**
     * @brief Get the model of agent from a Peer Id
     * @param peerId
     * @return
     */
    AgentM* getAgentModelFromPeerId(QString peerId);


    /**
     * @brief Get the (view model of) agents grouped for a name
     * @param name
     * @return
     */
    AgentsGroupedByNameVM* getAgentsGroupedForName(QString name);


    /**
     * @brief Get the hash table from a name to the group of agents with this name
     * @return
     */
    //QHash<QString, AgentsGroupedByNameVM*> getHashTableFromNameToAgentsGrouped();


    /**
     * @brief Import an agent or an agents list from selected file (definition)
     */
    Q_INVOKABLE bool importAgentOrAgentsListFromSelectedFile();


    /**
     * @brief Import an agents list from a JSON array
     * @param jsonArrayOfAgents
     * @param versionJsonPlatform
     */
    bool importAgentsListFromJson(QJsonArray jsonArrayOfAgents, QString versionJsonPlatform);


    /**
     * @brief Simulate an exit for each agent ON
     */
    void simulateExitForEachAgentON();


    /**
     * @brief Simulate an exit for each launcher
     */
    //void simulateExitForEachLauncher();


    /**
     * @brief Delete agents OFF
     */
    void deleteAgentsOFF();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a new model of agent has been created
     * @param agent
     */
    void agentModelHasBeenCreated(AgentM* agent);


    /**
     * @brief Signal emitted when a model of agent will be deleted
     * @param agent
     */
    void agentModelWillBeDeleted(AgentM* agent);


    /**
     * @brief Signal emitted when a new view model of agents grouped by name has been created
     * @param agentsGroupedByName
     */
    void agentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Signal emitted when a view model of agents grouped by name will be deleted
     * @param agentsGroupedByName
     */
    void agentsGroupedByNameWillBeDeleted(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Signal emitted when a new view model of agents grouped by definition has been created
     * @param agentsGroupedByDefinition
     */
    void agentsGroupedByDefinitionHasBeenCreated(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Signal emitted when a view model of agents grouped by definition will be deleted
     * @param agentsGroupedByDefinition
     */
    void agentsGroupedByDefinitionWillBeDeleted(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Emitted when inputs must be added to our Editor for a list of outputs
     * @param agentName
     * @param newOutputsIds
     * @param isMappingActivated
     */
    //void addInputsToEditorForOutputs(QString agentName, QStringList newOutputsIds, bool isMappingActivated);


    /**
     * @brief Emitted when inputs must be removed from our Editor for a list of outputs
     * @param agentName
     * @param oldOutputsIds
     * @param isMappingActivated
     */
    //void removeInputsToEditorForOutputs(QString agentName, QStringList oldOutputsIds, bool isMappingActivated);


public Q_SLOTS:

    /**
     * @brief Slot called when an agent enter the network
     * @param peerId
     * @param agentName
     * @param ipAddress
     * @param hostname
     * @param commandLine
     * @param canBeFrozen
     * @param loggerPort
     */
    void onAgentEntered(QString peerId, QString agentName, QString ipAddress, QString hostname, QString commandLine, bool canBeFrozen, QString loggerPort);


    /**
     * @brief Slot called when an agent quit the network
     * @param peer Id
     * @param agent name
     */
    void onAgentExited(QString peerId, QString agentName);


    /**
     * @brief Slot called when a launcher enter the network
     * @param peerId
     * @param hostName
     * @param ipAddress
     */
    //void onLauncherEntered(QString peerId, QString hostName, QString ipAddress, QString streamingPort);


    /**
     * @brief Slot called when a launcher quit the network
     * @param peerId
     * @param hostName
     */
    //void onLauncherExited(QString peerId, QString hostName);
    

    /**
     * @brief Slot called when an agent definition has been received and must be processed
     * @param peer Id
     * @param agent name
     * @param definition in JSON format
     */
    void onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON);


    /**
     * @brief Slot called when an agent mapping has been received and must be processed
     * @param peer Id
     * @param agent name
     * @param mapping in JSON format
     */
    void onMappingReceived(QString peerId, QString agentName, QString mappingJSON);


    /**
     * @brief Slot called when a new value is published
     * @param publishedValue
     */
    void onValuePublished(PublishedValueM* publishedValue);


private Q_SLOTS:

    /**
     * @brief Slot called when a model of agent has to be deleted
     * @param model
     */
    void _onAgentModelHasToBeDeleted(AgentM* model);


    /**
     * @brief Slot called when some view models of outputs have been added to an agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAddedToAgentsGroupedByName(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from an agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemovedFromAgentsGroupedByName(QList<OutputVM*> oldOutputs);


    /**
     * @brief Slot called when a view model of agents grouped by name has become useless (no more agents grouped by definition)
     */
    void _onUselessAgentsGroupedByName();


    /**
     * @brief Slot called when the network data of an agent will be cleared
     * @param peerId
     */
    void _onNetworkDataOfAgentWillBeCleared(QString peerId);


private:

    /**
     * @brief Create a new view model of agents grouped by name
     * @param model
     */
    void _createAgentsGroupedByName(AgentM* model);


private:

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the root directory to load/save files
    QString _rootDirectoryPath;

    // Map from "peer id" to a model of agent
    QHash<QString, AgentM*> _hashFromPeerIdToAgent;

    // Hash table from a name to the group of agents with this name
    QHash<QString, AgentsGroupedByNameVM*> _hashFromNameToAgentsGrouped;

    // Hash table from name to a model of host (corresponding to an INGESCAPE launcher)
    //QHash<QString, HostM*> _hashFromNameToHost;

};

QML_DECLARE_TYPE(IngeScapeModelManager)

#endif // INGESCAPEMODELMANAGER_H
