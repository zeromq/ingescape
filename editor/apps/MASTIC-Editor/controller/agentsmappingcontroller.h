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

#ifndef AGENTSMAPPINGCONTROLLER_H
#define AGENTSMAPPINGCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>
#include <viewModel/agentinmappingvm.h>
#include <viewModel/mapbetweeniopvm.h>


/**
 * @brief The AgentsMappingController class defines the controller for agents mapping
 */
class AgentsMappingController : public QObject
{
    Q_OBJECT

    // List of all agents in mapping
    I2_QOBJECT_LISTMODEL(AgentInMappingVM, agentInMappingVMList)

    // List of all maps between agents
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allMapInMapping)

    // List of all partial maps between agents (input and input agent are fully defined, output and output agent are ghost element)
    // This list is not empty in the case of the presence of Ghost Agents in the mapping
    I2_QOBJECT_LISTMODEL(MapBetweenIOPVM, allPartialMapInMapping)

    // Flag indicating if our mapping is empty
    I2_QML_PROPERTY_READONLY(bool, isEmptyMapping)

    // Selected agent in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(AgentInMappingVM*, selectedAgent)

    // Selected map between agents in the mapping
    I2_QML_PROPERTY_DELETE_PROOF(MapBetweenIOPVM*, selectedMapBetweenIOP)


public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit AgentsMappingController(MasticModelManager* modelManager,
                                     QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsMappingController();


    /**
     * @brief Create a new Mapping
     */
    Q_INVOKABLE void createNewMapping();


    /**
     * @brief Remove the agent from the mapping and delete it
     * @param agent
     */
    Q_INVOKABLE void deleteAgentInMapping(AgentInMappingVM* agent);


    /**
     * @brief Remove a link between two agents from the mapping
     * @param link
     */
    Q_INVOKABLE void removeLinkBetweenTwoAgents(MapBetweenIOPVM* link);


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
     * @brief Slot when an identical agent model is added
     * @param newModel
     */
    void onIdenticalAgentModelAdded(AgentM* newModel);


    /**
     * @brief Slot when the flag "is Activated Mapping" changed
     * @param isActivatedMapping
     */
    void onIsActivatedMappingChanged(bool isActivatedMapping);


    /**
     * @brief Slot when a model of agent will be deleted
     * @param agent
     */
    void onAgentModelWillBeDeleted(AgentM* agent);


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


private Q_SLOTS:

    /**
     * @brief Slot when inside an agentInMappingVM, new inputsVM are created.
     *      Check if a map need to be created from the element mapping list in the model manager.
     *      A missing agent is substituted by a ghost agent with same name. > create a partial map.
     *      A missing output is substituted by a ghost output with same name. > create partial map.
     * @param currentAgentInMapping
     * @param inputsListAdded
     */
    //void _generateAllMapBetweenIopUsingNewlyAddedInputsVM(AgentInMappingVM* currentAgentInMapping, QList<InputVM*> inputsListAdded);


    /**
     * @brief Slot when inside an agentInMappingVM, new outputsVM are created.
     *      Check if a map need can be completed from internal partial maps list in the mapping controller.
     *      A missing agent is substituted by a ghost agent with same name. > create a partial map.
     *      A missing output is substituted by a ghost output with same name. > create partial map.
     * @param currentAgentInMapping
     * @param outputsListAdded
     */
    //void _completeAllPartialMapBetweenIopUsingNewlyOutputsVM(AgentInMappingVM* currentAgentInMapping, QList<OutputVM*> outputsListAdded);


    /**
     * @brief Slot when the list of "Agents in Mapping" changed
     */
    void _onAgentsInMappingChanged();


    /**
     * @brief Slot when some view models of inputs have been added to an agent in mapping
     * @param inputsListAdded
     */
    void _onInputsListAdded(QList<InputVM*> inputsListAdded);


    /**
     * @brief Slot when some view models of outputs have been added to an agent in mapping
     * @param outputsListAdded
     */
    void _onOutputsListAdded(QList<OutputVM*> outputsListAdded);


    /**
     * @brief Slot when some view models of inputs will be removed from an agent in mapping
     * @param inputsListWillBeRemoved
     */
    void _onInputsListWillBeRemoved(QList<InputVM*> inputsListWillBeRemoved);


    /**
     * @brief Slot when some view models of outputs will be removed from an agent in mapping
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
     * @brief Deletes all the mapBetweenIOPVM where agent in paramater is involved as agentFrom in the map.
     * @param agentInMapping
     */
    //void _deleteAllMappingMadeOnTargetAgent(AgentInMappingVM *agentInMapping);


    /**
     * @brief Check if the map between an agent output and an agent input already exist.
     * @param agentFrom
     * @param pointFrom
     * @param agentTo
     * @param pointTo
     * @return
     */
    //bool _checkIfMapBetweenIOPVMAlreadyExist(AgentInMappingVM* agentFrom, OutputVM *pointFrom, AgentInMappingVM* agentTo, InputVM *pointTo);


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
     * @brief Remove all the links with this agent
     * @param agent
     */
    void _removeAllLinksWithAgent(AgentInMappingVM* agent);


private:
    // Manager for the data model of MASTIC
    MasticModelManager* _modelManager;

    // Map from agent name to the (view model of) agent in mapping
    QHash<QString, AgentInMappingVM*> _mapFromNameToAgentInMapping;

    // Map from agentInMapping  name to a list of MapBetweenIOPVM where the agent is involved as agentFrom.
    QHash<QString, QList<MapBetweenIOPVM*>> _mapFromAgentNameToPartialMapBetweenIOPViewModelsList;

    // Previous list of agents in mapping
    QList<AgentInMappingVM*> _previousListOfAgentsInMapping;
};

QML_DECLARE_TYPE(AgentsMappingController)

#endif // AGENTSMAPPINGCONTROLLER_H
