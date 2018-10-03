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

#ifndef AGENTSMAPPINGCONTROLLER_H
#define AGENTSMAPPINGCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/ingescapemodelmanager.h>
#include <viewModel/agentinmappingvm.h>
#include <viewModel/mapbetweeniopvm.h>


/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

    // Size of the mapping view
    I2_QML_PROPERTY(double, viewWidth)
    I2_QML_PROPERTY(double, viewHeight)

    // List of all agents in mapping
    I2_QOBJECT_LISTMODEL(AgentInMappingVM, allAgentsInMapping)

    // List of all links between agents in mapping
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allLinksInMapping)

    // Flag indicating if our mapping is empty
    I2_QML_PROPERTY_READONLY(bool, isEmptyMapping)

    // Selected agent in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, selectedAgent)

    // Selected link between agents in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(MapBetweenIOPVM*, selectedLink)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param directoryPath
     * @param parent
     */
    explicit AgentsMappingController(IngeScapeModelManager* modelManager,
                                     JsonHelper* jsonHelper,
                                     QString directoryPath,
                                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsMappingController();


    /**
     * @brief Clear the current mapping
     */
    void clearMapping();


    /**
     * @brief Remove the agent from the mapping and delete it
     * @param agent
     */
    Q_INVOKABLE void deleteAgentInMapping(AgentInMappingVM* agent);


    /**
     * @brief Remove a link between two agents from the mapping
     * @param link
     * @return true if the link has been deleted during the call of our method
     */
    Q_INVOKABLE bool removeLinkBetweenTwoAgents(MapBetweenIOPVM* link);


    /**
     * @brief Called when an agent from the list is dropped on the current mapping at a position
     * @param agentName
     * @param models
     * @param position
     */
    Q_INVOKABLE void dropAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* models, QPointF position);


    /**
     * @brief Slot when a link from an output is dropped over an input on the current mapping (or when a link to an input is dropped over an output)
     * @param outputAgent
     * @param output
     * @param inputAgent
     * @param input
     */
    Q_INVOKABLE void dropLinkBetweenAgents(AgentInMappingVM* outputAgent, OutputVM* output, AgentInMappingVM* inputAgent, InputVM* input);


    /**
     * @brief Get the agent in mapping from an agent name
     * @param name
     * @return
     */
    Q_INVOKABLE AgentInMappingVM* getAgentInMappingFromName(QString name);


    /**
     * @brief Import a mapping of agents from a JSON array
     * @param jsonArrayOfAgentsMapping
     */
    void importMappingFromJson(QJsonArray jsonArrayOfAgentsMapping);


    /**
     * @brief Reset the modifications made while the mapping was UN-activated
     */
    Q_INVOKABLE void resetModificationsWhileMappingWasUNactivated();


Q_SIGNALS:

    /**
     * @brief Emitted to zoom-in our view
     */
    void zoomIn();


    /**
     * @brief Emitted to zoom-out our view
     */
    void zoomOut();


    /**
     * @brief Emitted to reset zoom (100%)
     */
    void resetZoom();


    /**
     * @brief Emitted to fit all nodes in our view
     */
    void fitToView();


    /**
     * @brief Signal emitted when the user activates the mapping in mode OBSERVE
     * while he made some modifications on the links betwwen agents.
     * These modifications will be lost if the user stay in mode OBSERVE
     */
    void modificationsOnLinksWhileMappingUnactivated();


    /**
     * @brief Emitted when a new "Agent in Mapping" is added
     * @param agent
     */
    void agentInMappingAdded(AgentInMappingVM* addedAgent);


    /**
     * @brief Emitted when an "Agent in Mapping" is removed
     * @param agent
     */
    void agentInMappingRemoved(AgentInMappingVM* removedAgent);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Emitted when a command must be sent on the network to an agent about mapping one of its input
     * @param peerIdsList
     * @param command
     * @param inputName
     * @param outputAgentName
     * @param outputName
     */
    void commandAskedToAgentAboutMappingInput(QStringList peerIdsList, QString command, QString inputName, QString outputAgentName, QString outputName);


public Q_SLOTS:

    /**
     * @brief Slot when a previous agent model is replaced by a new one strictly identical
     * @param previousModel
     * @param newModel
     */
    void onIdenticalAgentModelReplaced(AgentM* previousModel, AgentM* newModel);


    /**
     * @brief Slot when the flag "is Mapping Activated" changed
     * @param isMappingActivated
     */
    void onIsMappingActivatedChanged(bool isMappingActivated);


    /**
     * @brief Slot when the flag "is Mapping Controlled" changed
     * @param isMappingControlled
     */
    void onIsMappingControlledChanged(bool isMappingControlled);


    /**
     * @brief Slot when a model of agent will be deleted
     * @param agent
     */
    void onAgentModelWillBeDeleted(AgentM* agent);


    /**
     * @brief Slot when an active agent has been defined
     * @param agent
     */
    void onActiveAgentDefined(AgentM* agent);


    /**
     * @brief Slot called when the mapping of an active agent has been defined
     * @param agent
     */
    void onActiveAgentMappingDefined(AgentM* agent);


    /**
     * @brief Slot when two agents are mapped
     * @param mappingElement
     */
    void onMapped(ElementMappingM* mappingElement);


    /**
     * @brief Slot when two agents are unmapped
     * @param mappingElement
     */
    void onUnmapped(ElementMappingM* mappingElement);


    /**
     * @brief Slot called when we receive the command highlight link from a recorder
     * @param parameters
     */
    void onHighlightLink(QStringList parameters);


private Q_SLOTS:

    /**
     * @brief Slot when the list of "Agents in Mapping" changed
     */
    void _onAgentsInMappingChanged();


    /**
     * @brief Slot called when some view models of inputs have been added to an agent in mapping
     * @param inputsListHaveBeenAdded
     */
    void _onInputsListHaveBeenAdded(QList<InputVM*> inputsListHaveBeenAdded);


    /**
     * @brief Slot called when some view models of outputs have been added to an agent in mapping
     * @param outputsListHaveBeenAdded
     */
    void _onOutputsListHaveBeenAdded(QList<OutputVM*> outputsListHaveBeenAdded);


    /**
     * @brief Slot called when some view models of inputs will be removed from an agent in mapping
     * @param inputsListWillBeRemoved
     */
    void _onInputsListWillBeRemoved(QList<InputVM*> inputsListWillBeRemoved);


    /**
     * @brief Slot called when some view models of outputs will be removed from an agent in mapping
     * @param outputsListWillBeRemoved
     */
    void _onOutputsListWillBeRemoved(QList<OutputVM*> outputsListWillBeRemoved);


private:
    /**
     * @brief Add new model(s) of agent to the current mapping at a specific position
     * @param agentName
     * @param agentsList
     * @param position
     */
    void _addAgentModelsToMappingAtPosition(QString agentName, QList<AgentM*> agentsList, QPointF position);


    /**
     * @brief Delete a link between two agents
     * @param link
     */
    void _deleteLinkBetweenTwoAgents(MapBetweenIOPVM* link);


    /**
     * @brief Get the view model of link which corresponds to a mapping element
     * @param mappingElement
     * @return
     */
    MapBetweenIOPVM* _getLinkFromMappingElement(ElementMappingM* mappingElement);


    /**
     * @brief Get the view model of link which corresponds to names
     * @param outputAgentName
     * @param outputName
     * @param inputAgentName
     * @param inputName
     * @return
     */
    MapBetweenIOPVM* _getLinkFromNames(QString outputAgentName, QString outputName, QString inputAgentName, QString inputName);


    /**
     * @brief Remove all the links with this agent
     * @param agent
     */
    void _removeAllLinksWithAgent(AgentInMappingVM* agent);


    /**
     * @brief OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
     * @param agentModel
     * @param temporaryMapping
     */
    void _overWriteMappingOfAgentModel(AgentM* agentModel, AgentMappingM* temporaryMapping);


    /**
     * @brief FIXME TO-RENAME
     */
    void _toRename();


    /**
     * @brief Get a random position in the current window
     * @param randomMax
     * @return
     */
    QPointF _getRandomPosition(double randomMax);


private:

    // Manager for the data model of INGESCAPE
    IngeScapeModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the directory to load/save files
    QString _directoryPath;

    // Map from agent name to the (view model of) agent in mapping
    QHash<QString, AgentInMappingVM*> _mapFromNameToAgentInMapping;

    // Previous list of agents in mapping
    QList<AgentInMappingVM*> _previousListOfAgentsInMapping;

    // Hash table from "agent name" to a list of waiting links (where the agent is involved as "Output Agent")
    QHash<QString, QList<ElementMappingM*>> _hashFromAgentNameToListOfWaitingLinks;

    // Hash table from "link id" to added/removed link while the mapping was UN-activated
    QHash<QString, ElementMappingM*> _hashFromLinkIdToAddedLinkWhileMappingWasUNactivated;
    QHash<QString, ElementMappingM*> _hashFromLinkIdToRemovedLinkWhileMappingWasUNactivated;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
