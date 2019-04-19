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

#ifndef EDITORMODELMANAGER_H
#define EDITORMODELMANAGER_H

#include <QObject>
//#include <QtQml>
#include <I2PropertyHelpers.h>
#include <model/editorenums.h>
#include <controller/ingescapemodelmanager.h>
#include <model/hostm.h>


/**
 * @brief The EditorModelManager class defines the manager for the data model of IngeScape
 */
class EditorModelManager : public IngeScapeModelManager
{
    Q_OBJECT

    // Flag indicating if our global mapping is activated
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isMappingActivated)

    // Flag indicating if our global mapping is controlled (or passive)
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isMappingControlled)

    // List of opened definitions
    I2_QOBJECT_LISTMODEL(DefinitionM, openedDefinitions)


public:
    /**
     * @brief Constructor
     * @param jsonHelper
     * @param rootDirectoryPath
     * @param parent
     */
    explicit EditorModelManager(JsonHelper* jsonHelper,
                                QString rootDirectoryPath,
                                QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~EditorModelManager();


    /**
     * @brief Get the model of host with a name
     * @param hostName
     * @return
     */
    HostM* getHostModelWithName(QString hostName);


    /**
     * @brief Get the peer id of the Launcher on a host
     * @param hostName
     * @return
     */
    QString getPeerIdOfLauncherOnHost(QString hostName);


    /**
     * @brief Get the hash table from a name to the group of agents with this name
     * @return
     */
    QHash<QString, AgentsGroupedByNameVM*> getHashTableFromNameToAgentsGrouped();


    /**
     * @brief Export the agents into JSON
     * @return array of all agents (grouped by name)
     */
    QJsonArray exportAgentsToJSON();


    /**
     * @brief Export the agents list to selected file
     */
    Q_INVOKABLE void exportAgentsListToSelectedFile();


    /**
     * @brief Simulate an exit for each launcher
     */
    void simulateExitForEachLauncher();


    /**
     * @brief Open a definition
     * If there are variants of this definition, we open each variant
     * @param definition
     */
    Q_INVOKABLE void openDefinition(DefinitionM* definition);


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
     * @brief Signal emitted when a model of host has been created
     * @param host
     */
    void hostModelHasBeenCreated(HostM* host);


    /**
     * @brief Signal emitted when a model of host will be deleted
     * @param host
     */
    void hostModelWillBeDeleted(HostM* host);


    /**
     * @brief Emitted when inputs must be added to our Editor for a list of outputs
     * @param agentName
     * @param newOutputsIds
     * @param isMappingActivated
     */
    void addInputsToEditorForOutputs(QString agentName, QStringList newOutputsIds, bool isMappingActivated);


    /**
     * @brief Emitted when inputs must be removed from our Editor for a list of outputs
     * @param agentName
     * @param oldOutputsIds
     * @param isMappingActivated
     */
    void removeInputsToEditorForOutputs(QString agentName, QStringList oldOutputsIds, bool isMappingActivated);


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
    void onLauncherEntered(QString peerId, QString hostName, QString ipAddress, QString streamingPort);


    /**
     * @brief Slot called when a launcher quit the network
     * @param peerId
     * @param hostName
     */
    void onLauncherExited(QString peerId, QString hostName);
    

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
     * @brief Slot called when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void onisMutedFromAgentUpdated(QString peerId, bool isMuted);


    /**
     * @brief Slot called when the flag "can be Frozen" from an agent updated
     * @param peerId
     * @param canBeFrozen
     */
    void onCanBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen);


    /**
     * @brief Slot called when the flag "is Frozen" from an agent updated
     * @param peerId
     * @param isFrozen
     */
    void onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen);


    /**
     * @brief Slot called when the flag "is Muted" from an output of agent updated
     * @param peerId
     * @param isMuted
     * @param outputName
     */
    void onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName);


    /**
     * @brief Slot called when the state of an agent changes
     * @param peerId
     * @param stateName
     */
    void onAgentStateChanged(QString peerId, QString stateName);


    /**
     * @brief Slot called when we receive the flag "Log In Stream" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void onAgentHasLogInStream(QString peerId, bool hasLogInStream);


    /**
     * @brief Slot called when we receive the flag "Log In File" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void onAgentHasLogInFile(QString peerId, bool hasLogInFile);


    /**
     * @brief Slot called when we receive the path of "Log File" for an agent
     * @param peerId
     * @param logFilePath
     */
    void onAgentLogFilePath(QString peerId, QString logFilePath);


    /**
     * @brief Slot called when we receive the path of "Definition File" for an agent
     * @param peerId
     * @param definitionFilePath
     */
    void onAgentDefinitionFilePath(QString peerId, QString definitionFilePath);


    /**
     * @brief Slot called when we receive the path of "Mapping File" for an agent
     * @param peerId
     * @param mappingFilePath
     */
    void onAgentMappingFilePath(QString peerId, QString mappingFilePath);


private Q_SLOTS:

    /**
     * @brief Slot called when a model of agent has to be deleted
     * @param model
     */
    void _onAgentModelHasToBeDeleted(AgentM* model);


    /**
     * @brief Slot called when the definition(s) of an agent (agents grouped by name) must be opened
     * @param definitionsList
     */
    void _onDefinitionsToOpen(QList<DefinitionM*> definitionsList);


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


    /**
     * @brief Open a list of definitions (if the definition is already opened, we bring it to front)
     * @param definitionsToOpen
     */
    void _openDefinitions(QList<DefinitionM*> definitionsToOpen);


private:

    // Hash table from name to a model of host (corresponding to an INGESCAPE launcher)
    QHash<QString, HostM*> _hashFromNameToHost;

};

QML_DECLARE_TYPE(EditorModelManager)

#endif // EDITORMODELMANAGER_H
