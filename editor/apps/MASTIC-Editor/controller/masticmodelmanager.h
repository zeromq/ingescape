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
#include <model/definitionm.h>
#include <viewModel/mapbetweeniopvm.h>


/**
 * @brief The MasticModelManager class defines a manager for the data model of MASTIC
 */
class MasticModelManager : public QObject
{
    Q_OBJECT

    // List of all models of agents
    //I2_QOBJECT_LISTMODEL(AgentM, allAgents)

    // List of opened definitions
    I2_QOBJECT_LISTMODEL(DefinitionM, openedDefinitions)


public:
    /**
     * @brief Default constructor
     * @param agentsListDirectoryPath
     * @param agentsMappingsDirectoryPath
     * @param parent
     */
    explicit MasticModelManager(QString agentsListDirectoryPath,
                                QString agentsMappingsDirectoryPath,
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
     * @brief Get the list (of models) of agent definition from a definition name
     * @param definitionName
     * @return
     */
    QList<DefinitionM*> getAgentDefinitionsListFromName(QString definitionName);


    /**
     * @brief Delete a model of agent definition
     * @param definition
     */
    void deleteAgentDefinition(DefinitionM* definition);


    /**
     * @brief Add a model of agent mapping
     * @param agentMapping
     */
    void addAgentMapping(AgentMappingM* agentMapping);


    /**
     * @brief Get the list (of models) of agent mapping from a mapping name
     * @param name
     * @return
     */
    QList<AgentMappingM*> getAgentMappingsListFromName(QString mappingName);


    /**
     * @brief Get the merged list of all (models of) mapping elements which connect an input of the agent
     * @param agentName
     * @return
     */
    QList<ElementMappingM*> getMergedListOfInputMappingElementsFromAgentName(QString agentName);


    /**
     * @brief Get the merged list of all (models of) mapping elements which connect an output of the agent
     * @param agentName
     * @return
     */
    QList<ElementMappingM*> getMergedListOfOutputMappingElementsFromAgentName(QString agentName);


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
     * @param agentsListToExport list of pairs <agent name, definition>
     */
    void exportAgentsListToDefaultFile(QList<QPair<QString, DefinitionM*>> agentsListToExport);


    /**
     * @brief Export the agents list to selected file
     * @param agentsListToExport list of pairs <agent name, definition>
     */
    void exportAgentsListToSelectedFile(QList<QPair<QString, DefinitionM*>> agentsListToExport);


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


    /**
     * @brief Signal emitted when a new model of agent mapping has been created
     * @param agentMapping
     * @param agent
     */
    void agentMappingCreated(AgentMappingM* agentMapping, AgentM* agent);


    /**
     * @brief Signal emitted when a new model of mapping element has been created
     * @param mappingElement
     */
    void mappingElementCreated(ElementMappingM* mappingElement);


    /**
     * @brief Signal emitted when the flag "is Muted" from an output of agent updated
     * @param agent
     * @param isMuted
     * @param outputName
     */
    void isMutedFromOutputOfAgentUpdated(AgentM* agent, bool isMuted, QString outputName);


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


    /**
     * @brief Slot when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void onisMutedFromAgentUpdated(QString peerId, bool isMuted);


    /**
     * @brief Slot when the flag "is Frozen" from an agent updated
     * @param peerId
     * @param isFrozen
     */
    void onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen);


    /**
     * @brief Slot when the flag "is Muted" from an output of agent updated
     * @param peerId
     * @param isMuted
     * @param outputName
     */
    void onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName);


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
     * @param agentsListToExport list of pairs <agent name, definition>
     * @param agentsListFilePath
     */
    void _exportAgentsListToFile(QList<QPair<QString, DefinitionM*>> agentsListToExport, QString agentsListFilePath);


    /**
     * @brief Update definition variants of a list of definitions with the same name
     * @param definitionName
     */
    void _updateDefinitionVariants(QString definitionName);


    /**
     * @brief Update merged lists of mapping elements for the agent name
     * @param agentName
     * @param agentMapping
     */
    void _updateMergedListsOfMappingElementsForAgentName(QString agentName, AgentMappingM* agentMapping);


private:

    // Path to the directory containing JSON files to save agents list
    QString _agentsListDirectoryPath;
    QString _agentsListDefaultFilePath;

    // Path to the directory containing JSON files to save agents mappings
    QString _agentsMappingsDirectoryPath;
    QString _agentsMappingsDefaultFilePath;

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // List of all models of agents
    //QList<AgentM*> _allAgentsModel;

    // Map from "peer id" to a model of agent
    QHash<QString, AgentM*> _mapFromPeerIdToAgentM;

    // Map from "agent name" to a list of models of agent
    QHash<QString, QList<AgentM*>> _mapFromNameToAgentModelsList;

    // Map from "definition name" to a list (of models) of agent definition
    QHash<QString, QList<DefinitionM*>> _mapFromNameToAgentDefinitionsList;

    // Map from "mapping name" to a list (of models) of agent mapping
    QHash<QString, QList<AgentMappingM*>> _mapFromNameToAgentMappingsList;

    // Map from agent name to the merged list of all (models of) mapping elements which connect an input of the agent
    QHash<QString, QList<ElementMappingM*>> _mapFromAgentNameToMergedListOfInputMappingElements;

    // Map from agent name to the merged list of all (models of) mapping elements which connect an output of the agent
    QHash<QString, QList<ElementMappingM*>> _mapFromAgentNameToMergedListOfOutputMappingElements;

};

QML_DECLARE_TYPE(MasticModelManager)

#endif // MASTICMODELMANAGER_H
