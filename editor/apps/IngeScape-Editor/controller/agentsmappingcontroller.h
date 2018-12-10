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
#include <viewModel/link/linkvm.h>


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

    // List of all links between agents in the global mapping
    I2_QOBJECT_LISTMODEL(LinkVM, allLinksInMapping)

    // Flag indicating if our mapping is empty
    I2_QML_PROPERTY_READONLY(bool, isEmptyMapping)

    // Selected agent in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, selectedAgent)

    // Selected link between agents in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(LinkVM*, selectedLink)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit AgentsMappingController(IngeScapeModelManager* modelManager,
                                     JsonHelper* jsonHelper,
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
    Q_INVOKABLE bool removeLinkBetweenTwoAgents(LinkVM* link);


    /**
     * @brief Called when an agent from the list is dropped on the current mapping at a position
     * @param agentName
     * @param position
     */
    Q_INVOKABLE void dropAgentNameToMappingAtPosition(QString agentName, QPointF position);


    /**
     * @brief Slot called when a link from an output is dropped over an input on the current mapping
     * Or when a link to an input is dropped over an output
     * @param outputAgent
     * @param linkOutput
     * @param inputAgent
     * @param linkInput
     */
    Q_INVOKABLE void dropLinkBetweenAgents(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput);


    /**
     * @brief Get the (view model of) agent in the global mapping from an agent name
     * @param name
     * @return
     */
    Q_INVOKABLE AgentInMappingVM* getAgentInMappingFromName(QString name);


    /**
     * @brief Get the list of (view model of) links between agents in the global mapping from a link name
     * @param linkName
     * @return
     */
    QList<LinkVM*> getLinksInMappingFromName(QString linkName);


    /**
     * @brief Get the (view model of) link between agents in the global mapping from a link id
     * @param linkId
     * @return
     */
    LinkVM* getLinkInMappingFromId(QString linkId);


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
     * @brief Slot called when the flag "is Mapping Activated" changed
     * @param isMappingActivated
     */
    void onIsMappingActivatedChanged(bool isMappingActivated);


    /**
     * @brief Slot called when the flag "is Mapping Controlled" changed
     * @param isMappingControlled
     */
    void onIsMappingControlledChanged(bool isMappingControlled);


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
     * @brief Slot called when an active agent has been defined
     * @param agent
     */
    //void onActiveAgentDefined(AgentM* agent);


    /**
     * @brief Slot called when the mapping of an active agent has been defined
     * @param agent
     */
    //void onActiveAgentMappingDefined(AgentM* agent);


    /**
     * @brief Slot when two agents are mapped
     * @param mappingElement
     */
    //void onMapped(ElementMappingM* mappingElement);


    /**
     * @brief Slot when two agents are unmapped
     * @param mappingElement
     */
    //void onUnmapped(ElementMappingM* mappingElement);


    /**
     * @brief Slot called when the flag "is ON" of an agent changed
     * @param isON
     */
    void onAgentIsONChanged(bool isON);


    /**
     * @brief Slot called when some view models of mapping elements have been added to an agent(s grouped by name)
     * @param newMappingElements
     */
    void onMappingElementsHaveBeenAdded(QList<MappingElementVM*> newMappingElements);


    /**
     * @brief Slot called when some view models of mapping elements will be removed from an agent(s grouped by name)
     * @param oldMappingElements
     */
    void onMappingElementsWillBeRemoved(QList<MappingElementVM*> oldMappingElements);


    /**
     * @brief Slot called when we receive the command highlight link from a recorder
     * @param parameters
     */
    void onHighlightLink(QStringList parameters);


private Q_SLOTS:

    /**
     * @brief Slot called when the list of all "Agents in Mapping" changed
     */
    void _onAllAgentsInMappingChanged();


    /**
     * @brief Slot called when some view models of link inputs have been added to an agent in mapping
     * @param addedlinkInputs
     */
    void _onLinkInputsListHaveBeenAdded(QList<LinkInputVM*> addedlinkInputs);


    /**
     * @brief Slot called when some view models of link outputs have been added to an agent in mapping
     * @param addedlinkOutputs
     */
    void _onLinkOutputsListHaveBeenAdded(QList<LinkOutputVM*> addedlinkOutputs);


    /**
     * @brief Slot called when some view models of link inputs will be removed from an agent in mapping
     * @param removedLinkInputs
     */
    void _onLinkInputsListWillBeRemoved(QList<LinkInputVM*> removedLinkInputs);


    /**
     * @brief Slot called when some view models of link outputs will be removed from an agent in mapping
     * @param removedLinkOutputs
     */
    void _onLinkOutputsListWillBeRemoved(QList<LinkOutputVM*> removedLinkOutputs);


private:

    /**
     * @brief Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
     * @param agentsGroupedByName
     * @param position
     * @return
     */
    AgentInMappingVM* _createAgentInMappingAtPosition(AgentsGroupedByNameVM* agentsGroupedByName, QPointF position);


    /**
     * @brief Create a link between two agents
     * @param linkName
     * @param outputAgent
     * @param linkOutput
     * @param inputAgent
     * @param linkInput
     * @param mappingElement
     * @param isTemporary
     * @return
     */
    LinkVM* _createLinkBetweenTwoAgents(QString linkName,
                                        AgentInMappingVM* outputAgent,
                                        LinkOutputVM* linkOutput,
                                        AgentInMappingVM* inputAgent,
                                        LinkInputVM* linkInput,
                                        MappingElementVM* mappingElement,
                                        bool isTemporary = false);


    /**
     * @brief Delete a link between two agents
     * @param link
     */
    void _deleteLinkBetweenTwoAgents(LinkVM* link);


    /**
     * @brief Remove all the links with an agent
     * @param agent
     */
    void _removeAllLinksWithAgent(AgentInMappingVM* agent);


    /**
     * @brief OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
     * @param agentModel
     * @param temporaryMapping
     */
    //void _overWriteMappingOfAgentModel(AgentM* agentModel, AgentMappingM* temporaryMapping);


    /**
     * @brief Update the global mapping with agents ON and their links
     */
    void _updateMappingWithAgentsONandLinks();


    /**
     * @brief Link an agent (in the global mapping) on its inputs (add all missing links TO an agent)
     * @param agentInMapping
     */
    void _linkAgentOnInputs(AgentInMappingVM* agentInMapping);


    /**
     * @brief Link an agent (in the global mapping) on its input from a mapping element (add a missing link TO an agent)
     * @param inputAgent
     * @param mappingElement
     */
    void _linkAgentOnInputFromMappingElement(AgentInMappingVM* inputAgent, MappingElementVM* mappingElement);


    /**
     * @brief Link an agent (in the global mapping) on its outputs (add all missing links FROM an agent)
     * @param agentInMapping
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
    QList<MappingElementVM*> _getWaitingMappingElementsOnOutputAgent(QString outputAgentName);


    /**
     * @brief Add a "Waiting Mapping Element" on an output agent (name)
     * @param outputAgentName
     * @param waitingMappingElement
     */
    void _addWaitingMappingElementOnOutputAgent(QString outputAgentName, MappingElementVM* waitingMappingElement);


    /**
     * @brief Remove a "Waiting Mapping Element" on an output agent (name)
     * @param outputAgentName
     * @param waitingMappingElement
     */
    void _removeWaitingMappingElementOnOutputAgent(QString outputAgentName, MappingElementVM* waitingMappingElement);


    /**
     * @brief Helper to get the link input from a name if there is only one input for this name
     * If there are several inputs with the same name (but different value type), we return NULL
     * @param agent
     * @param inputName
     * @param linkName
     * @return
     */
    LinkInputVM* _getAloneLinkInputFromName(AgentInMappingVM* agent, QString inputName, QString linkName);


    /**
     * @brief Helper to get the link output from a name if there is only one output for this name
     * If there are several outputs with the same name (but different value type), we return NULL
     * @param agent
     * @param outputName
     * @param linkName
     * @return
     */
    LinkOutputVM* _getAloneLinkOutputFromName(AgentInMappingVM* agent, QString outputName, QString linkName);


private:

    // Manager for the data model of INGESCAPE
    IngeScapeModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Hash table from agent name to the (view model of) agent in mapping
    QHash<QString, AgentInMappingVM*> _hashFromNameToAgentInMapping;

    // Link name as key is not unique (because the value type of the Input/Output can be different)
    // Hash table from link name to a list of view models of links between agents in mapping
    QHash<QString, QList<LinkVM*>> _hashFromNameToListOfLinksInMapping;

    // Hash table from link id to the (view model of) link between agents in mapping
    QHash<QString, LinkVM*> _hashFromIdToLinkInMapping;

    // Hash table from "output agent name" to a list of waiting mapping elements (where the agent is involved as "output agent")
    QHash<QString, QList<MappingElementVM*>> _hashFromOutputAgentNameToListOfWaitingMappingElements;

    // List of ids of added links while the mapping was UN-activated
    QStringList _idsOfAddedLinks_WhileMappingWasUNactivated;

    // Hash table from "link id" to removed link (mapping element) while the mapping was UN-activated
    QHash<QString, MappingElementVM*> _hashFromLinkIdToRemovedMappingElement_WhileMappingWasUNactivated;

};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
