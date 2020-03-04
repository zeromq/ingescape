/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#ifndef AGENTSMAPPINGCONTROLLER_H
#define AGENTSMAPPINGCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <I2PropertyHelpers.h>
#include <controller/editormodelmanager.h>
#include <viewModel/agentinmappingvm.h>
#include <viewModel/mapping/actioninmappingvm.h>
#include <viewModel/link/linkvm.h>


/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

    // Size of the mapping view
    I2_QML_PROPERTY_FUZZY_COMPARE(double, viewWidth)
    I2_QML_PROPERTY_FUZZY_COMPARE(double, viewHeight)

    // Offset between the viewport's current position and the view's origin position
    // These values are updated when the mapping is dragged.
    // The spawn zone must follow the viewport and not stay at the view's origin position.
    I2_QML_PROPERTY_FUZZY_COMPARE(double, xSpawnZoneOffset)
    I2_QML_PROPERTY_FUZZY_COMPARE(double, ySpawnZoneOffset)

    // List of all agents in the mapping
    I2_QOBJECT_LISTMODEL(AgentInMappingVM, allAgentsInMapping)

    // List of all actions in the mapping
    I2_QOBJECT_LISTMODEL(ActionInMappingVM, allActionsInMapping)

    // List of all links between agents in the global mapping
    I2_QOBJECT_LISTMODEL(LinkVM, allLinksInMapping)

    // Flag indicating if our mapping is empty
    I2_QML_PROPERTY_READONLY(bool, isEmptyMapping)

    // Selected agent in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, selectedAgent)

    // Selected action in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(ActionInMappingVM*, selectedAction)

    // Selected link between agents in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(LinkVM*, selectedLink)

    // Flag indicating if the corresponding view is loaded
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isLoadedView)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit AgentsMappingController(EditorModelManager* modelManager,
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
     * @brief Remove the agent from the mapping and delete the view model
     * @param agent
     */
    Q_INVOKABLE void deleteAgentInMapping(AgentInMappingVM* agent);


    /**
     * @brief Remove the action from the mapping and delete the view model
     * @param action
     */
    Q_INVOKABLE void deleteActionInMapping(ActionInMappingVM* action);


    /**
     * @brief Remove a link between two objects in the mapping
     * @param link
     */
    Q_INVOKABLE void removeLinkBetweenTwoObjectsInMapping(LinkVM* link);


    /**
     * @brief Called when an agent from the list is dropped on the current mapping at a position
     * @param agentName
     * @param position
     */
    Q_INVOKABLE void dropAgentNameToMappingAtPosition(const QString& agentName, QPointF position);


    /**
     * @brief Called when an action from the list is dropped on the current mapping at a position
     * @param action
     * @param position
     */
    Q_INVOKABLE void dropActionToMappingAtPosition(ActionM* action, QPointF position);


    /**
     * @brief Slot called when a link from an output is dropped over an input on the current mapping
     * Or when a link to an input is dropped over an output
     * @param outputAgent
     * @param linkOutput
     * @param inputAgent
     * @param linkInput
     */
    Q_INVOKABLE void dropLinkBetweenTwoAgents(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput);


    /**
     * @brief Slot called when a link from an output is dropped over an input on the current mapping
     * Or when a link to an input is dropped over an output
     * @param outputAction
     * @param linkOutput
     * @param inputAction
     * @param linkInput
     */
    Q_INVOKABLE void dropLinkBetweenTwoActions(ActionInMappingVM* outputAction, LinkOutputVM* linkOutput, ActionInMappingVM* inputAction, LinkInputVM* linkInput);
    Q_INVOKABLE void dropLinkFromActionToAgent(ActionInMappingVM* outputAction, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput);
    Q_INVOKABLE void dropLinkFromAgentToAction(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, ActionInMappingVM* inputAction, LinkInputVM* linkInput);


    /**
     * @brief Get the (view model of) agent in the global mapping from an agent name
     * @param name
     * @return
     */
    Q_INVOKABLE AgentInMappingVM* getAgentInMappingFromName(const QString& name);


    /**
     * @brief Get the (view model of) action in the global mapping from a unique id
     * @param uid
     * @return
     */
    ActionInMappingVM* getActionInMappingFromUid(const QString& uid);


    /**
     * @brief Get the list of (view model of) links between agents in the global mapping from a link name
     * @param linkName
     * @return
     */
    QList<LinkVM*> getLinksInMappingFromName(const QString& linkName);


    /**
     * @brief Get the (view model of) link between agents in the global mapping from a link id
     * @param linkId
     * @return
     */
    LinkVM* getLinkInMappingFromId(const QString& linkId);


    /**
     * @brief Check if an action has been inserted in the global mapping
     * @param actionM
     * @return
     */
    Q_INVOKABLE bool isActionInsertedInMapping(ActionM* actionM);


    /**
     * @brief Export the global mapping (of agents) into JSON
     * @return array of all agents and their mapping
     */
    QJsonArray exportGlobalMappingToJSON();


    /**
     * @brief Import the global mapping (of agents) from JSON
     * @param jsonArrayOfAgentsInMapping
     */
    void importMappingFromJson(QJsonArray jsonArrayOfAgentsInMapping);


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
     * while he made some changes on the links betwwen agents.
     * These changes will be lost if the user stay in mode OBSERVE
     */
    void changesOnLinksWhileMappingUnactivated();


    /**
     * @brief Signal emitted when an action has to be executed
     * @param action
     */
    void executeAction(ActionM* action);


public Q_SLOTS:

    /**
     * @brief Slot called when a new view model of agents grouped by name has been created
     * @param agentsGroupedByName
     */
    void onAgentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Slot called when a view model of agents grouped by name will be deleted
     * @param agentsGroupedByName
     */
    void onAgentsGroupedByNameWillBeDeleted(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Slot called when a model of action will be deleted
     * @param action
     */
    void onActionModelWillBeDeleted(ActionM* action);


    /**
     * @brief Slot called when we receive the command "highlight link" from a recorder
     * @param parameters
     */
    void onHighlightLink(const QStringList& parameters);


private Q_SLOTS:

    /**
     * @brief Slot called when the list of all "Agents in Mapping" changed
     */
    void _onAllAgentsInMappingChanged();


    /**
     * @brief Slot called when the flag "is ON" of an agent(s grouped by name) changed (only happens when we are online)
     * It means that our agent was ALREADY in our platform
     */
    void _onAgentIsONChanged(bool isON);


    /**
     * @brief Slot called when a model of agent "ON" has been added to an agent(s grouped by name) (only happens when we are online)
     * It means that our agent was NOT in our platform
     */
    void _onAgentModelONhasBeenAdded(AgentM* model);


    /**
     * @brief Slot called when some view models of mapping elements have been added to an agent(s grouped by name)
     * @param newMappingElements
     */
    void _onMappingElementsHaveBeenAdded(QList<MappingElementVM*> newMappingElements);


    /**
     * @brief Slot called when some view models of mapping elements will be removed from an agent(s grouped by name)
     * @param oldMappingElements
     */
    void _onMappingElementsWillBeRemoved(QList<MappingElementVM*> oldMappingElements);


    /**
     * @brief Slot called when some view models of link inputs have been added to an agent in mapping
     * @param addedlinkInputs
     */
    void _onLinkInputsListHaveBeenAdded(const QList<LinkInputVM*>& addedlinkInputs);


    /**
     * @brief Slot called when some view models of link outputs have been added to an agent in mapping
     * @param addedlinkOutputs
     */
    void _onLinkOutputsListHaveBeenAdded(const QList<LinkOutputVM*>& addedlinkOutputs);


    /**
     * @brief Slot called when some view models of link inputs will be removed from an agent in mapping
     * @param removedLinkInputs
     */
    void _onLinkInputsListWillBeRemoved(const QList<LinkInputVM*>& removedLinkInputs);


    /**
     * @brief Slot called when some view models of link outputs will be removed from an agent in mapping
     * @param removedLinkOutputs
     */
    void _onLinkOutputsListWillBeRemoved(const QList<LinkOutputVM*>& removedLinkOutputs);


    /**
     * @brief Slot called when an output has been activated, so we have to activate an input (of an action in the global mapping)
     * @param inputObject
     * @param linkInput
     */
    void _onActivateInputOfActionInMapping(ObjectInMappingVM* inputObject, LinkInputVM* linkInput);


    /**
     * @brief Slot called when an output has been activated, so we have to write on an input (of an input agent in the global mapping)
     * @param inputObject
     * @param linkInput
     */
    void _onWriteOnInputOfAgentInMapping(ObjectInMappingVM* inputObject, LinkInputVM* linkInput);


private:

    /**
     * @brief Create a new agent in the global mapping with an "Agents Grouped by Name" and at a specific position
     */
    AgentInMappingVM* _createAgentInMappingAtPosition(AgentsGroupedByNameVM* agentsGroupedByName, QPointF position, qreal width);


    /**
     * @brief Create a new action in the global mapping with a unique id, with a model of action and at a specific position
     */
    ActionInMappingVM* _createActionInMappingAtPosition(QString uid, ActionM* action, QPointF position, qreal width);


    /**
     * @brief Create a link between two objects in the mapping
     * @param linkName
     * @param outputObject
     * @param linkOutput
     * @param inputObject
     * @param linkInput
     * @param mappingElement
     * @param isTemporary
     * @return
     */
    LinkVM* _createLinkBetweenTwoObjectsInMapping(const QString& linkName,
                                                  ObjectInMappingVM* outputObject,
                                                  LinkOutputVM* linkOutput,
                                                  ObjectInMappingVM* inputObject,
                                                  LinkInputVM* linkInput,
                                                  MappingElementVM* mappingElement = nullptr,
                                                  bool isTemporary = false);


    /**
     * @brief Remove a link between two agents from the mapping
     * @param link
     */
    void _removeLinkBetweenTwoAgents(LinkVM* link);


    /**
     * @brief Delete a link between two objects in the mapping
     * @param link
     */
    void _deleteLinkBetweenTwoObjectsInMapping(LinkVM* link);


    /**
     * @brief Remove all the links with an agent
     * @param agent
     */
    void _removeAllLinksWithAgent(AgentInMappingVM* agent);


    /**
     * @brief Link an agent (in the global mapping) on its inputs (add all missing links TO an agent)
     */
    void _linkAgentOnInputs(AgentInMappingVM* agentInMapping);


    /**
     * @brief Link an agent (in the global mapping) on its input from a mapping element (add a missing link TO an agent)
     */
    void _linkAgentOnInputFromMappingElement(AgentInMappingVM* inputAgent, MappingElementVM* mappingElement);


    /**
     * @brief Link an agent (in the global mapping) on its outputs (add all missing links FROM an agent)
     */
    void _linkAgentOnOutputs(AgentInMappingVM* agentInMapping);


    /**
     * @brief Get a random position in the current window
     * @param randomMax
     * @return
     */
    QPointF _getRandomPosition(double randomMax);


    /**
     * @brief Get the list of "Waiting Mapping Elements" on an output agent (name)
     * @param outputAgentName
     * @return
     */
    QList<MappingElementVM*> _getWaitingMappingElementsOnOutputAgent(const QString& outputAgentName);


    /**
     * @brief Add a "Waiting Mapping Element" on an output agent (name)
     * @param outputAgentName
     * @param waitingMappingElement
     */
    void _addWaitingMappingElementOnOutputAgent(const QString& outputAgentName, MappingElementVM* waitingMappingElement);


    /**
     * @brief Remove a "Waiting Mapping Element" on an output agent (name)
     * @param outputAgentName
     * @param waitingMappingElement
     */
    void _removeWaitingMappingElementOnOutputAgent(const QString& outputAgentName, MappingElementVM* waitingMappingElement);


    /**
     * @brief Helper to get the link input from a name if there is only one input for this name
     * If there are several inputs with the same name (but different value type), we return NULL
     * @param agent
     * @param inputName
     * @param linkName
     * @return
     */
    LinkInputVM* _getAloneLinkInputFromName(AgentInMappingVM* agent, const QString& inputName, const QString& linkName);


    /**
     * @brief Helper to get the link output from a name if there is only one output for this name
     * If there are several outputs with the same name (but different value type), we return NULL
     * @param agent
     * @param outputName
     * @param linkName
     * @return
     */
    LinkOutputVM* _getAloneLinkOutputFromName(AgentInMappingVM* agent, const QString& outputName, const QString& linkName);


    /**
     * @brief Get the JSON of the mapping of an agent as displayed in the global mapping
     * @param agentInMapping
     * @return
     */
    QString _getJSONofMappingOfAgentInGlobalMapping(AgentInMappingVM* agentInMapping);


private:

    // Manager for the data model of IngeScape
    EditorModelManager* _modelManager;

    // Hash table from agent name to the (view model of) agent in mapping
    QHash<QString, AgentInMappingVM*> _hashFromNameToAgentInMapping;

    // Hash table:
    // - from unique id of action in the global mapping
    // - to the (view model of) action in the global mapping
    QHash<QString, ActionInMappingVM*> _hashFromUidToActionInMapping;

    // Link name as key is not unique (because the value type of the Input/Output can be different)
    // Hash table from link name to a list of view models of links between agents in mapping
    QHash<QString, QList<LinkVM*>> _hashFromNameToListOfLinksInMapping;

    // Hash table from link id to the (view model of) link between agents in mapping
    QHash<QString, LinkVM*> _hashFromIdToLinkInMapping;

    // Hash table from "output agent name" to a list of waiting mapping elements (where the agent is involved as "output agent")
    QHash<QString, QList<MappingElementVM*>> _hashFromOutputAgentNameToListOfWaitingMappingElements;

    // Hash table from "(unique) link id" to the added link (for which we are waiting a reply to the request "add")
    QHash<QString, LinkVM*> _hashFromLinkIdToAddedLink_WaitingReply;

    // Hash table from "(unique) link id" to the removed link (for which we are waiting a reply to the request "remove")
    QHash<QString, LinkVM*> _hashFromLinkIdToRemovedLink_WaitingReply;

};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
