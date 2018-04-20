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
#include <model/publishedvaluem.h>


/**
 * @brief The MasticModelManager class defines the manager for the data model of MASTIC
 */
class MasticModelManager : public QObject
{
    Q_OBJECT

    // List of opened definitions
    I2_QOBJECT_LISTMODEL(DefinitionM, openedDefinitions)

    // Flag indicating if our global mapping is activated
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isActivatedMapping)

    // List of all published values
    I2_QOBJECT_LISTMODEL(PublishedValueM, publishedValues)


public:
    /**
     * @brief Default constructor
     * @param agentsListDirectoryPath
     * @param agentsMappingsDirectoryPath
     * @param dataDirectoryPath
     * @param parent
     */
    explicit MasticModelManager(QString agentsListDirectoryPath,
                                QString agentsMappingsDirectoryPath,
                                QString dataDirectoryPath,
                                QObject *parent = nullptr);


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
     * @brief Get the map from agent name to list of active agents
     * @return
     */
    QHash<QString, QList<AgentM*>> getMapFromAgentNameToActiveAgentsList();


    /**
     * @brief Delete a model of Agent
     * @param agent
     */
    void deleteAgentModel(AgentM* agent);


    /**
     * @brief Add a model of agent definition for an agent name
     * @param agentDefinition
     * @param agentName
     */
    void addAgentDefinitionForAgentName(DefinitionM* agentDefinition, QString agentName);


    /**
     * @brief Get the list (of models) of agent definition from a definition name
     * @param definitionName
     * @return
     */
    QList<DefinitionM*> getAgentDefinitionsListFromDefinitionName(QString definitionName);


    /**
     * @brief Delete a model of agent definition
     * @param definition
     */
    void deleteAgentDefinition(DefinitionM* definition);


    /**
     * @brief Add a model of agent mapping for an agent name
     * @param agentMapping
     * @param agentName
     */
    void addAgentMappingForAgentName(AgentMappingM* agentMapping, QString agentName);


    /**
     * @brief Get the list (of models) of agent mapping from a mapping name
     * @param name
     * @return
     */
    QList<AgentMappingM*> getAgentMappingsListFromMappingName(QString mappingName);


    /**
     * @brief Delete a model of agent mapping
     * @param agentMapping
     */
    void deleteAgentMapping(AgentMappingM* agentMapping);


    /**
     * @brief Import the agents list from default file
     */
    void importAgentsListFromDefaultFile();


    /**
     * @brief Import an agents list from selected file
     */
    Q_INVOKABLE void importAgentsListFromSelectedFile();


    /**
     * @brief Import an agent from selected files (definition and mapping)
     */
    Q_INVOKABLE void importAgentFromSelectedFiles();


    /**
     * @brief Export the agents list to default file
     * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
     */
    void exportAgentsListToDefaultFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport);


    /**
     * @brief Export the agents list to selected file
     * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
     */
    void exportAgentsListToSelectedFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport);


    /**
     * @brief Get the JSON of a mapping
     * @param agentMapping
     * @return
     */
    QString getJsonOfMapping(AgentMappingM* agentMapping);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a new model of agent has been created
     * @param agent
     */
    void agentModelCreated(AgentM* agent);


    /**
     * @brief Signal emitted when a model of agent will be deleted
     * @param agent
     */
    void agentModelWillBeDeleted(AgentM* agent);


    /**
     * @brief Emitted when inputs must be added to our Editor for a list of outputs
     * @param agentName
     * @param outputsList
     */
    void addInputsToEditorForOutputs(QString agentName, QList<OutputM*> outputsList);


    /**
     * @brief Emitted when inputs must be removed to our Editor for a list of outputs
     * @param agentName
     * @param outputsList
     */
    void removeInputsToEditorForOutputs(QString agentName, QList<OutputM*> outputsList);


    /**
     * @brief Emitted when two agents are mapped
     * @param mappingElement
     */
    void mapped(ElementMappingM* mappingElement);


    /**
     * @brief Emitted when two agents are unmapped
     * @param mappingElement
     */
    void unmapped(ElementMappingM* mappingElement);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


public Q_SLOTS:

    /**
     * @brief Slot called when an agent enter the network
     * @param peerId
     * @param agentName
     * @param agentAddress
     * @param pid
     * @param hostname
     * @param commandLine
     * @param canBeFrozen
     */
    void onAgentEntered(QString peerId, QString agentName, QString agentAddress, int pid, QString hostname, QString commandLine, bool canBeFrozen, bool isRecorder);


    /**
     * @brief Slot called when an agent quit the network
     * @param peer Id
     * @param agent name
     */
    void onAgentExited(QString peerId, QString agentName);


    /**
     * @brief Slot called when a launcher enter the network
     * @param peerId
     * @param hostname
     * @param ipAddress
     */
    void onLauncherEntered(QString peerId, QString hostname, QString ipAddress, QString streamingPort);


    /**
     * @brief Slot called when a launcher quit the network
     * @param peerId
     * @param hostname
     */
    void onLauncherExited(QString peerId, QString hostname);
    

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


    /**
     * @brief Slot called when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void onisMutedFromAgentUpdated(QString peerId, bool isMuted);


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

private:

    /**
     * @brief Import the agents list from JSON file
     * @param agentsListFilePath
     */
    void _importAgentsListFromFile(QString agentsListFilePath);


    /**
     * @brief Import an agent from JSON files (definition and mapping)
     * @param subDirectoryPath
     */
    void _importAgentFromFiles(QStringList agentFilesPaths);


    /**
     * @brief Export the agents list to JSON file
     * @param agentsListToExport list of pairs <agent name (and parameters to restart), definition>
     * @param agentsListFilePath
     */
    void _exportAgentsListToFile(QList<QPair<QStringList, DefinitionM*>> agentsListToExport, QString agentsListFilePath);


    /**
     * @brief Update definition variants of the list of definitions with the same name
     * @param definitionName
     */
    void _updateDefinitionVariants(QString definitionName);


    /**
     * @brief Print all models of agents (for Debug)
     */
    void _printAgents();


    /**
     * @brief Print all models of agent definitions (for Debug)
     */
    void _printDefinitions();


    /**
     * @brief Print all models of agent mappings (for Debug)
     */
    void _printMappings();


private:

    // Path to the directory containing JSON files to save agents list
    QString _agentsListDirectoryPath;
    QString _agentsListDefaultFilePath;

    // Path to the directory containing JSON files to save agents mappings
    QString _agentsMappingsDirectoryPath;
    QString _agentsMappingsDefaultFilePath;

    // Path to the directory containing data files
    QString _dataDirectoryPath;

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // Map from "peer id" to a model of agent
    QHash<QString, AgentM*> _mapFromPeerIdToAgentM;

    // Map from "agent name" to a list of models of agent
    QHash<QString, QList<AgentM*>> _mapFromNameToAgentModelsList;

    // Map from "definition name" to a list (of models) of agent definition
    QHash<QString, QList<DefinitionM*>> _mapFromNameToAgentDefinitionsList;

    // Map from "mapping name" to a list (of models) of agent mapping
    QHash<QString, QList<AgentMappingM*>> _mapFromNameToAgentMappingsList;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
