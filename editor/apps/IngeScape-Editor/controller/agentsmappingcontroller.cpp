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

#include "agentsmappingcontroller.h"

#include <QQmlEngine>
#include <QDebug>
#include <QFileDialog>


/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param directoryPath
 * @param parent
 */
AgentsMappingController::AgentsMappingController(IngeScapeModelManager* modelManager,
                                                 JsonHelper* jsonHelper,
                                                 QString directoryPath,
                                                 QObject *parent) : QObject(parent),
      _viewWidth(1920 - 320), // Full HD - Width of left panel
      _viewHeight(1080 - 100), // Full HD - Height of top & bottom bars of OS
      _isEmptyMapping(true),
      _selectedAgent(NULL),
      _selectedLink(NULL),
      _modelManager(modelManager),
      _jsonHelper(jsonHelper),
      _directoryPath(directoryPath)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Connect to signal "Count Changed" from the list of agents in mapping
    connect(&_allAgentsInMapping, &AbstractI2CustomItemListModel::countChanged, this, &AgentsMappingController::_onAgentsInMappingChanged);
}


/**
 * @brief Destructor
 */
AgentsMappingController::~AgentsMappingController()
{
    // Clean-up current selections
    setselectedAgent(NULL);
    setselectedLink(NULL);

    // DIS-connect from signal "Count Changed" from the list of agents in mapping
    disconnect(&_allAgentsInMapping, 0, this, 0);

    // Delete all links
    _allLinksInMapping.deleteAllItems();
    _hashFromAgentNameToListOfWaitingLinks.clear();

    // Delete all agents in mapping
    _previousListOfAgentsInMapping.clear();
    _allAgentsInMapping.deleteAllItems();

    // Reset pointers
    _modelManager = NULL;
    _jsonHelper = NULL;
}


/**
 * @brief Clear the current mapping
 */
void AgentsMappingController::clearMapping()
{
    qInfo() << "Clear the current mapping";

    // 1- First, DE-activate the mapping
    if (_modelManager != NULL) {
        _modelManager->setisMappingActivated(false);
    }

    // 2- Delete all links
    for (MapBetweenIOPVM* link : _allLinksInMapping.toList()) {
        _deleteLinkBetweenTwoAgents(link);
    }

    // 3- Delete all agents in mapping
    for (AgentInMappingVM* agent : _allAgentsInMapping.toList()) {
        deleteAgentInMapping(agent);
    }

    qInfo() << "The Mapping is empty !";
}


/**
 * @brief Remove the agent from the mapping and delete it
 * @param agent
 */
void AgentsMappingController::deleteAgentInMapping(AgentInMappingVM* agent)
{
    if (agent != NULL)
    {
        qInfo() << "Delete the agent" << agent->name() << "in the Mapping";

        // Unselect our agent if needed
        if (_selectedAgent == agent) {
            setselectedAgent(NULL);
        }

        // Remove from hash table
        _mapFromNameToAgentInMapping.remove(agent->name());

        // Remove all the links with this agent
        _removeAllLinksWithAgent(agent);

        // Remove this Agent In Mapping from the list to update view (QML)
        _allAgentsInMapping.remove(agent);

        // Free memory
        delete agent;
    }
}


/**
 * @brief Remove a link between two agents from the mapping
 * @param link
 */
void AgentsMappingController::removeLinkBetweenTwoAgents(MapBetweenIOPVM* link)
{
    if ((link != NULL) && (link->inputAgent() != NULL) && (link->input() != NULL) && (link->outputAgent() != NULL) && (link->output() != NULL))
    {
        qInfo() << "Remove the link between agents" << link->outputAgent()->name() << "and" << link->inputAgent()->name();

        // Mapping is activated
        if ((_modelManager != NULL) && _modelManager->isMappingActivated())
        {
            // Set to virtual to give a feedback to the user
            link->setisVirtual(true);

            // Emit signal "Command asked to agent about Mapping Input"
            Q_EMIT commandAskedToAgentAboutMappingInput(link->inputAgent()->peerIdsList(), "UNMAP", link->input()->name(), link->outputAgent()->name(), link->output()->name());
        }
        // Mapping is NOT activated
        else
        {
            // Remove temporary link (this temporary link will be removed when the user will activate the mapping)
            link->inputAgent()->removeTemporaryLink(link->input()->name(), link->outputAgent()->name(), link->output()->name());

            // Delete the link between two agents
            _deleteLinkBetweenTwoAgents(link);
        }
    }
}


/**
 * @brief Called when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param models
 * @param position
 */
void AgentsMappingController::dropAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* models, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
    if (agentInMapping == NULL)
    {
        I2CustomItemListModel<AgentM>* agentsList = dynamic_cast<I2CustomItemListModel<AgentM>*>(models);
        if (agentsList != NULL)
        {
            // Add new model(s) of agent to the current mapping at a specific position
            _addAgentModelsToMappingAtPosition(agentName, agentsList->toList(), position);

            agentInMapping = getAgentInMappingFromName(agentName);
            if (agentInMapping != NULL)
            {
                // Get the mapping currently edited (temporary until the user activate the mapping)
                AgentMappingM* temporaryMapping = agentInMapping->temporaryMapping();
                if (temporaryMapping != NULL)
                {
                    // Delete all "mapping elements" in this temporary mapping
                    temporaryMapping->mappingElements()->deleteAllItems();

                    // Mapping is already activated
                    if ((_modelManager != NULL) && _modelManager->isMappingActivated())
                    {
                        for (AgentM* model : agentsList->toList())
                        {
                            if (model != NULL)
                            {
                                // OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
                                _overWriteMappingOfAgentModel(model, temporaryMapping);
                            }
                        }
                    }
                }

                // Selects this new agent
                setselectedAgent(agentInMapping);
            }
        }
    }
}


/**
 * @brief Slot when a link from an output is dropped over an input on the current mapping (or when a link to an input is dropped over an output)
 * @param outputAgent
 * @param output
 * @param inputAgent
 * @param input
 */
void AgentsMappingController::dropLinkBetweenAgents(AgentInMappingVM* outputAgent, OutputVM* output, AgentInMappingVM* inputAgent, InputVM* input)
{
    if ((outputAgent != NULL) && (output != NULL) && (inputAgent != NULL) && (input != NULL))
    {
        // Check that the input can link to the output
        if (input->canLinkWith(output))
        {
            // Search if the same link already exists
            bool alreadyLinked = false;
            for (MapBetweenIOPVM* iterator : _allLinksInMapping.toList())
            {
                if ((iterator != NULL) && (iterator->outputAgent() == outputAgent) && (iterator->output() == output)
                        && (iterator->inputAgent() == inputAgent) && (iterator->input() == input))
                {
                    alreadyLinked = true;
                    break;
                }
            }

            // Check that the same link does not yet exist
            if (!alreadyLinked)
            {
                qInfo() << "QML asked to create the link between agents" << outputAgent->name() << "and" << inputAgent->name();

                // Mapping is activated
                if ((_modelManager != NULL) && _modelManager->isMappingActivated())
                {
                    // Create a new VIRTUAL link between agents
                    MapBetweenIOPVM* link = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, true, this);

                    // Add to the list
                    _allLinksInMapping.append(link);

                    // Emit signal "Command asked to agent about Mapping Input"
                    Q_EMIT commandAskedToAgentAboutMappingInput(inputAgent->peerIdsList(), "MAP", input->name(), outputAgent->name(), output->name());
                }
                // Mapping is NOT activated
                else
                {
                    // Add a temporary link (this temporary link will became a real link when the user will activate the mapping)
                    inputAgent->addTemporaryLink(input->name(), outputAgent->name(), output->name());

                    // Create a new link between agents
                    MapBetweenIOPVM* link = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, false, this);

                    // Add to the list
                    _allLinksInMapping.append(link);
                }
            }
            else {
                qWarning() << "The input" << input->name() << "(of agent" << inputAgent->name() << ") is already linked to output" << output->name() << "(of agent" << outputAgent->name() << ")";
            }
        }
        else {
            if ((output->firstModel() != NULL) && (input->firstModel() != NULL)) {
                qDebug() << "Can not link output" << output->name() << "with type" << AgentIOPValueTypes::staticEnumToString(output->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->name() << ")"
                         << "and input" << input->name() << "with type" << AgentIOPValueTypes::staticEnumToString(input->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->name() << ")";
            }
        }
    }
}


/**
 * @brief Get the agent in mapping from an agent name
 * @param name
 * @return
 */
AgentInMappingVM* AgentsMappingController::getAgentInMappingFromName(QString name)
{
    if (_mapFromNameToAgentInMapping.contains(name)) {
        return _mapFromNameToAgentInMapping.value(name);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Import a mapping of agents from a JSON array
 * @param jsonArrayOfAgentsMapping
 */
void AgentsMappingController::importMappingFromJson(QJsonArray jsonArrayOfAgentsMapping)
{
    if (_jsonHelper != NULL)
    {
        QList<ElementMappingM*> mappingElements;

        for (QJsonValue jsonValue : jsonArrayOfAgentsMapping)
        {
            if (jsonValue.isObject())
            {
                QJsonObject jsonAgent = jsonValue.toObject();

                // Get values for keys "agentName", "mapping" and "position"
                QJsonValue jsonName = jsonAgent.value("agentName");
                QJsonValue jsonMapping = jsonAgent.value("mapping");
                QJsonValue jsonPosition = jsonAgent.value("position");

                if (jsonName.isString() && jsonMapping.isObject() && jsonPosition.isString())
                {
                    QString agentName = jsonName.toString();

                    // Create the agent mapping from JSON
                    AgentMappingM* agentMapping = _jsonHelper->createModelOfAgentMappingFromJSON(agentName, jsonMapping.toObject());

                    // Position
                    QPointF position = QPointF();

                    QStringList positionStringList = jsonPosition.toString().split(", ");
                    if (positionStringList.count() == 2)
                    {
                        QString strX = positionStringList.at(0);
                        QString strY = positionStringList.at(1);

                        if (!strX.isEmpty() && !strY.isEmpty()) {
                            position = QPointF(strX.toFloat(), strY.toFloat());
                        }
                    }

                    // Get the list of models of agent from the name
                    QList<AgentM*> agentModelsList = _modelManager->getAgentModelsListFromName(agentName);

                    if (!position.isNull() && !agentModelsList.isEmpty())
                    {
                        qDebug() << "Position:" << position.x() << position.y() << "is defined for" << agentName << "with" << agentModelsList.count() << "models";

                        // Create a new Agent In Mapping
                        _addAgentModelsToMappingAtPosition(agentName, agentModelsList, position);

                        // Get the agent in mapping from the name
                        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
                        if (agentInMapping != NULL)
                        {
                            // Set the temporary mapping
                            if (agentMapping != NULL) {
                                agentInMapping->settemporaryMapping(agentMapping);
                            }

                            // Add the link elements
                            mappingElements.append(agentMapping->mappingElements()->toList());
                        }
                    }
                }
            }
        }

        // FIXME: Add links ?
        if (!mappingElements.isEmpty())
        {
            // Create all mapping links
            for (ElementMappingM* elementMapping : mappingElements)
            {
                if (elementMapping != NULL) {
                    onMapped(elementMapping);
                }
            }
        }
    }
}


/**
 * @brief Slot when a previous agent model is replaced by a new one strictly identical
 * @param previousModel
 * @param newModel
 */
void AgentsMappingController::onIdenticalAgentModelReplaced(AgentM* previousModel, AgentM* newModel)
{
    if ((previousModel != NULL) && (newModel != NULL))
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(newModel->name());
        if ((agentInMapping != NULL) && agentInMapping->models()->contains(previousModel))
        {
            // First add the new model before remove the previous model
            // (allows to prevent to have 0 model at a given moment and to prevent to emit signal in/out..putsListWillBeRemoved that remove links)
            agentInMapping->models()->append(newModel);
            agentInMapping->models()->remove(previousModel);

            // Our global mapping is activated and controlled
            if (_modelManager && _modelManager->isMappingActivated() && _modelManager->isMappingControlled())
            {
                // OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
                _overWriteMappingOfAgentModel(newModel, agentInMapping->temporaryMapping());
            }
        }
    }
}


/**
 * @brief Slot when the flag "is Mapping Activated" changed
 * @param isMappingActivated
 */
void AgentsMappingController::onIsMappingActivatedChanged(bool isMappingActivated)
{
    if ((_modelManager != NULL) && (_jsonHelper != NULL) && isMappingActivated)
    {
        // CONTROL
        if (_modelManager->isMappingControlled())
        {
            qDebug() << "Mapping Activated in mode CONTROL";

            // Apply all temporary mappings
            for (AgentInMappingVM* agent : _allAgentsInMapping.toList())
            {
                if ((agent != NULL) && (agent->temporaryMapping() != NULL))
                {
                    // Get the JSON of the agent mapping
                    QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(agent->temporaryMapping(), QJsonDocument::Compact);

                    QString command = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

                    // Emit signal "Command asked to agent"
                    Q_EMIT commandAskedToAgent(agent->peerIdsList(), command);
                }
            }
        }
        // OBSERVE
        else
        {
            qDebug() << "Mapping Activated in mode OBSERVE";

            // Get the map from agent name to list of active agents
            QHash<QString, QList<AgentM*>> mapFromAgentNameToActiveAgentsList = _modelManager->getMapFromAgentNameToActiveAgentsList();

            if (!mapFromAgentNameToActiveAgentsList.isEmpty())
            {
                double randomMax = (double)RAND_MAX;

                // Create all agents in mapping
                for (QString agentName : mapFromAgentNameToActiveAgentsList.keys())
                {
                    QList<AgentM*> activeAgentsList = mapFromAgentNameToActiveAgentsList.value(agentName);

                    // Get a random position in the current window
                    QPointF position = _getRandomPosition(randomMax);

                    //qDebug() << "Random position:" << position << "for agent" << agentName;

                    // Add new model(s) of agent to the current mapping
                    _addAgentModelsToMappingAtPosition(agentName, activeAgentsList, position);
                }

                // Create all links in mapping
                for (AgentInMappingVM* agent : _allAgentsInMapping.toList())
                {
                    if ((agent != NULL) && (agent->temporaryMapping() != NULL))
                    {
                        // Delete all "mapping elements" in the temporary mapping
                        agent->temporaryMapping()->mappingElements()->deleteAllItems();

                        for (AgentM* model : agent->models()->toList())
                        {
                            if ((model != NULL) && (model->mapping() != NULL))
                            {
                                for (ElementMappingM* mappingElement : model->mapping()->mappingElements()->toList())
                                {
                                    if (mappingElement != NULL)
                                    {
                                        // Simulate slot "on Mapped"
                                        onMapped(mappingElement);
                                    }
                                }
                            }
                        }
                    }
                }

                // Notify the QML to fit the view
                Q_EMIT fitToView();
            }
        }
    }
}


/**
 * @brief Slot when the flag "is Mapping Controlled" changed
 * @param isMappingControlled
 */
void AgentsMappingController::onIsMappingControlledChanged(bool isMappingControlled)
{
    qDebug() << "Mapping Ctrl Is Mapping Controlled Changed to" << isMappingControlled;
}


/**
 * @brief Slot when a model of agent will be deleted
 * @param agent
 */
void AgentsMappingController::onAgentModelWillBeDeleted(AgentM* agent)
{
    if (agent != NULL)
    {
        // Get the agent in mapping for the agent name
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agent->name());
        if (agentInMapping != NULL)
        {
            // Remove the model
            agentInMapping->models()->remove(agent);

            // If it was the last one...
            if (agentInMapping->models()->count() == 0)
            {
                // ...delete this agent in mapping
                deleteAgentInMapping(agentInMapping);
            }
        }
    }
}


/**
 * @brief Slot when an active agent has been defined
 * @param agent
 */
void AgentsMappingController::onActiveAgentDefined(AgentM* agent)
{
    if ((agent != NULL) && (_modelManager != NULL) && _modelManager->isMappingActivated())
    {
        QString agentName = agent->name();

        // CONTROL
        if (_modelManager->isMappingControlled())
        {
            //qDebug() << "CONTROL: Model of" << agentName << "is defined. CLEAR its MAPPING !";

            QStringList peerIdsList = QStringList(agent->peerId());

            // Get the agent in mapping for the agent name
            AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

            // The agent is not yet in the mapping...
            if (agentInMapping == NULL)
            {
                // Send the command "Clear Mapping" on the network to this agent
                Q_EMIT commandAskedToAgent(peerIdsList, command_ClearMapping);
            }
            // The agent is already in the mapping
            else
            {
                if (!agentInMapping->models()->contains(agent))
                {
                    agentInMapping->models()->append(agent);

                    // OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
                    _overWriteMappingOfAgentModel(agent, agentInMapping->temporaryMapping());
                }
            }
        }
        // OBSERVE
        else
        {
            //qDebug() << "OBSERVE: Model of" << agentName << "is defined. ADD in MAPPING view !" << agent;

            // Get the agent in mapping for the agent name
            AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

            // The agent is not yet in the mapping...
            if (agentInMapping == NULL)
            {
                QList<AgentM*> activeAgentsList;
                activeAgentsList.append(agent);

                double randomMax = (double)RAND_MAX;

                // Get a random position in the current window
                QPointF position = _getRandomPosition(randomMax);

                //qDebug() << "Random position:" << position << "for agent" << agentName;

                // Add new model(s) of agent to the current mapping
                _addAgentModelsToMappingAtPosition(agentName, activeAgentsList, position);
            }
            // The agent is already in the mapping
            else
            {
                if (!agentInMapping->models()->contains(agent)) {
                    agentInMapping->models()->append(agent);
                }
            }

            // If there are waiting links (where this agent is involved as "Output Agent")
            if (_hashFromAgentNameToListOfWaitingLinks.contains(agentName))
            {
                QList<ElementMappingM*> listOfWaitingLinks = _hashFromAgentNameToListOfWaitingLinks.value(agentName);
                for (ElementMappingM* mappingElement : listOfWaitingLinks)
                {
                    qDebug() << "Create waiting MAP..." << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();

                    // Create the link corresponding to the mapping element
                    onMapped(mappingElement);
                }
            }
        }
    }
}


/**
 * @brief Slot called when the mapping of an active agent has been defined
 * @param agent
 */
void AgentsMappingController::onActiveAgentMappingDefined(AgentM* agent)
{
    if ((agent != NULL) && (agent->mapping() != NULL) && (_modelManager != NULL) && _modelManager->isMappingActivated())
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agent->name());
        if ((agentInMapping != NULL) && (agentInMapping->temporaryMapping() != NULL))
        {
            QStringList idsOfRemovedMappingElements;
            for (QString idPreviousList : agentInMapping->temporaryMapping()->idsOfMappingElements())
            {
                if (!agent->mapping()->idsOfMappingElements().contains(idPreviousList)) {
                    idsOfRemovedMappingElements.append(idPreviousList);
                }
            }

            QStringList idsOfAddedMappingElements;
            for (QString idNewList : agent->mapping()->idsOfMappingElements())
            {
                if (!agentInMapping->temporaryMapping()->idsOfMappingElements().contains(idNewList)) {
                    idsOfAddedMappingElements.append(idNewList);
                }
            }

            // If there are some Removed mapping elements
            if (!idsOfRemovedMappingElements.isEmpty())
            {
                qDebug() << "unmapped" << idsOfRemovedMappingElements;

                for (ElementMappingM* mappingElement : agentInMapping->temporaryMapping()->mappingElements()->toList())
                {
                    if ((mappingElement != NULL) && idsOfRemovedMappingElements.contains(mappingElement->id()))
                    {
                        onUnmapped(mappingElement);
                    }
                }
            }
            // If there are some Added mapping elements
            if (!idsOfAddedMappingElements.isEmpty())
            {
                qDebug() << "mapped" << idsOfAddedMappingElements;

                for (ElementMappingM* mappingElement : agent->mapping()->mappingElements()->toList())
                {
                    if ((mappingElement != NULL) && idsOfAddedMappingElements.contains(mappingElement->id()))
                    {
                        onMapped(mappingElement);
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot when two agents are mapped
 * @param mappingElement
 */
void AgentsMappingController::onMapped(ElementMappingM* mappingElement)
{
    if (mappingElement != NULL)
    {
        //qDebug() << "MAPPED" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();

        // Try to get the virtual link which corresponds to the mapping element
        MapBetweenIOPVM* link = _getLinkFromMappingElement(mappingElement);

        if (link == NULL)
        {
            AgentInMappingVM* outputAgent = getAgentInMappingFromName(mappingElement->outputAgent());
            AgentInMappingVM* inputAgent = getAgentInMappingFromName(mappingElement->inputAgent());

            if ((outputAgent != NULL) && (inputAgent != NULL))
            {
                OutputVM* output = NULL;
                InputVM* input = NULL;

                // Get the list of view models of output from the output name
                QList<OutputVM*> outputsWithSameName = outputAgent->getOutputsListFromName(mappingElement->output());
                if (outputsWithSameName.count() == 1) {
                    output = outputsWithSameName.first();
                }
                else {
                    qWarning() << "There are" << outputsWithSameName.count() << "outputs with the same name" << mappingElement->output() << "."
                               << "We cannot choose and create the link" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();
                }

                // Get the list of view models of input from the input name
                QList<InputVM*> inputsWithSameName = inputAgent->getInputsListFromName(mappingElement->input());
                if (inputsWithSameName.count() == 1) {
                    input = inputsWithSameName.first();
                }
                else {
                    qWarning() << "There are" << inputsWithSameName.count() << "inputs with the same name" << mappingElement->input() << "."
                               << "We cannot choose and create the link" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();
                }

                if ((output != NULL) && (input != NULL))
                {
                    qInfo() << "MAPPED" << mappingElement->id();

                    // Create a new map between agents
                    link = new MapBetweenIOPVM(outputAgent, output, inputAgent, input, false, this);

                    // Add to the list
                    _allLinksInMapping.append(link);

                    qDebug() << "MAP has been created:" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();

                    // If there is a list of waiting links (where the agent is involved as "Output Agent")
                    if (_hashFromAgentNameToListOfWaitingLinks.contains(mappingElement->outputAgent()))
                    {
                        QList<ElementMappingM*> listOfWaitingLinks = _hashFromAgentNameToListOfWaitingLinks.value(mappingElement->outputAgent());
                        if (listOfWaitingLinks.contains(mappingElement))
                        {
                            // Remove the mapping element from the list because the corresponding link is really created
                            listOfWaitingLinks.removeOne(mappingElement);

                            // Update the hash table
                            _hashFromAgentNameToListOfWaitingLinks.insert(mappingElement->outputAgent(), listOfWaitingLinks);
                        }
                    }
                }
            }
            // Output agent is not yet in the mapping
            else if (inputAgent != NULL) // && (outputAgent == NULL)
            {
                QList<ElementMappingM*> listOfWaitingLinks;

                if (_hashFromAgentNameToListOfWaitingLinks.contains(mappingElement->outputAgent())) {
                    listOfWaitingLinks = _hashFromAgentNameToListOfWaitingLinks.value(mappingElement->outputAgent());
                }

                // Add the mapping element to the list to create the corresponding link later
                listOfWaitingLinks.append(mappingElement);

                // Update the hash table
                _hashFromAgentNameToListOfWaitingLinks.insert(mappingElement->outputAgent(), listOfWaitingLinks);

                qDebug() << "MAP will be created later:" << mappingElement->outputAgent() << "." << mappingElement->output() << "-->" << mappingElement->inputAgent() << "." << mappingElement->input();
            }
        }
        else
        {
            if (link->isVirtual())
            {
                qInfo() << "MAPPED" << mappingElement->id();

                // Update the flag if needed
                link->setisVirtual(false);
            }
        }

        if ((link != NULL) && (link->inputAgent() != NULL))
        {
            // Add the temporary link that correspond to this real link (if it does not yet exist)
            link->inputAgent()->addTemporaryLink(mappingElement->input(), mappingElement->outputAgent(), mappingElement->output());
        }
    }
}


/**
 * @brief Slot when two agents are unmapped
 * @param mappingElement
 */
void AgentsMappingController::onUnmapped(ElementMappingM* mappingElement)
{
    if (mappingElement != NULL)
    {
        qInfo() << "UN-mapped" << mappingElement->id();

        // Get the view model of link which corresponds to a mapping element
        MapBetweenIOPVM* link = _getLinkFromMappingElement(mappingElement);
        if (link != NULL)
        {
            if (link->inputAgent() != NULL) {
                // Remove the temporary link that correspond to this real link
                link->inputAgent()->removeTemporaryLink(mappingElement->input(), mappingElement->outputAgent(), mappingElement->output());
            }

            // Delete the link between two agents
            _deleteLinkBetweenTwoAgents(link);
        }
    }
}


/**
 * @brief Slot called when we receive the command highlight link from a recorder
 * @param parameters
 */
void AgentsMappingController::onHighlightLink(QStringList parameters)
{
    if (parameters.count() == 4)
    {
        QString inputAgentName = parameters.at(0);
        QString inputName = parameters.at(1);
        QString outputAgentName = parameters.at(2);
        QString outputName = parameters.at(3);

        // Get the view model of link which corresponds to these parameters
        MapBetweenIOPVM* link = NULL;

        for (MapBetweenIOPVM* iterator : _allLinksInMapping.toList())
        {
            if ((iterator != NULL)
                    && (iterator->outputAgent() != NULL) && (iterator->outputAgent()->name() == outputAgentName)
                    && (iterator->inputAgent() != NULL) && (iterator->inputAgent()->name() == inputAgentName)
                    && (iterator->output() != NULL) && (iterator->output()->name() == outputName)
                    && (iterator->input() != NULL) && (iterator->input()->name() == inputName))
            {
                link = iterator;
                break;
            }
        }

        if ((link != NULL) && (link->output() != NULL))
        {
            qDebug() << "Highlight the link between" << inputAgentName << "." << inputName << "and " << outputAgentName << "." << outputName;

            // Simulate that the current value of output model changed: allows to highlight the link
            link->output()->simulateCurrentValueOfModelChanged();
        }
    }
}


/**
 * @brief Slot when the list of "Agents in Mapping" changed
 */
void AgentsMappingController::_onAgentsInMappingChanged()
{
    // Update the flag "is Empty Mapping"
    if (_allAgentsInMapping.count() == 0) {
        setisEmptyMapping(true);
    }
    else {
        setisEmptyMapping(false);
    }

    QList<AgentInMappingVM*> newListOfAgentsInMapping = _allAgentsInMapping.toList();

    // Agent in Mapping added
    if (_previousListOfAgentsInMapping.count() < newListOfAgentsInMapping.count())
    {
        //qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping ADDED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : newListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !_previousListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "ADDED";

                // Emit the signal "Agent in Mapping Added"
                Q_EMIT agentInMappingAdded(agentInMapping);

                // Connect to signals from the new agent in mapping
                connect(agentInMapping, &AgentInMappingVM::inputsListHaveBeenAdded, this, &AgentsMappingController::_onInputsListHaveBeenAdded);
                connect(agentInMapping, &AgentInMappingVM::outputsListHaveBeenAdded, this, &AgentsMappingController::_onOutputsListHaveBeenAdded);
                connect(agentInMapping, &AgentInMappingVM::inputsListWillBeRemoved, this, &AgentsMappingController::_onInputsListWillBeRemoved);
                connect(agentInMapping, &AgentInMappingVM::outputsListWillBeRemoved, this, &AgentsMappingController::_onOutputsListWillBeRemoved);
            }
        }
    }
    // Agent in Mapping removed
    else if (_previousListOfAgentsInMapping.count() > newListOfAgentsInMapping.count())
    {
        //qDebug() << _previousListOfAgentsInMapping.count() << "--> Agent in Mapping REMOVED --> " << newListOfAgentsInMapping.count();

        for (AgentInMappingVM* agentInMapping : _previousListOfAgentsInMapping) {
            if ((agentInMapping != NULL) && !newListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "REMOVED";

                // Emit the signal "Agent in Mapping Removed"
                Q_EMIT agentInMappingRemoved(agentInMapping);

                // DIS-connect to signals from the previous agent in mapping
                disconnect(agentInMapping, &AgentInMappingVM::inputsListHaveBeenAdded, this, &AgentsMappingController::_onInputsListHaveBeenAdded);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListHaveBeenAdded, this, &AgentsMappingController::_onOutputsListHaveBeenAdded);
                disconnect(agentInMapping, &AgentInMappingVM::inputsListWillBeRemoved, this, &AgentsMappingController::_onInputsListWillBeRemoved);
                disconnect(agentInMapping, &AgentInMappingVM::outputsListWillBeRemoved, this, &AgentsMappingController::_onOutputsListWillBeRemoved);

            }
        }
    }

    _previousListOfAgentsInMapping = newListOfAgentsInMapping;
}


/**
 * @brief Slot called when some view models of inputs have been added to an agent in mapping
 * @param inputsListHaveBeenAdded
 */
void AgentsMappingController::_onInputsListHaveBeenAdded(QList<InputVM*> inputsListHaveBeenAdded)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && !inputsListHaveBeenAdded.isEmpty())
    {
        QStringList namesOfInputs;
        for (InputVM* input : inputsListHaveBeenAdded) {
            if (input != NULL) {
                namesOfInputs.append(input->name());
            }
        }

        if (agentInMapping->temporaryMapping() != NULL)
        {
            for (ElementMappingM* mappingElement : agentInMapping->temporaryMapping()->mappingElements()->toList())
            {
                if ((mappingElement != NULL) && namesOfInputs.contains(mappingElement->input()))
                {
                    // Try to create the link corresponding to the mapping element
                    onMapped(mappingElement);
                }
            }
        }
    }
}


/**
 * @brief Slot called when some view models of outputs have been added to an agent in mapping
 * @param outputsListHaveBeenAdded
 */
void AgentsMappingController::_onOutputsListHaveBeenAdded(QList<OutputVM*> outputsListHaveBeenAdded)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && !outputsListHaveBeenAdded.isEmpty())
    {
        QStringList namesOfOutputs;
        for (OutputVM* output : outputsListHaveBeenAdded) {
            if (output != NULL) {
                namesOfOutputs.append(output->name());
            }
        }

        for (AgentInMappingVM* iterator : _allAgentsInMapping.toList())
        {
            if ((iterator != NULL) && (iterator != agentInMapping) && (iterator->temporaryMapping() != NULL))
            {
                for (ElementMappingM* mappingElement : iterator->temporaryMapping()->mappingElements()->toList())
                {
                    if ((mappingElement != NULL) && (mappingElement->outputAgent() == agentInMapping->name()) && namesOfOutputs.contains(mappingElement->output()))
                    {
                        // Try to create the link corresponding to the mapping element
                        onMapped(mappingElement);
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot when some view models of inputs will be removed from an agent in mapping
 * @param inputsListWillBeRemoved
 */
void AgentsMappingController::_onInputsListWillBeRemoved(QList<InputVM*> inputsListWillBeRemoved)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && !inputsListWillBeRemoved.isEmpty())
    {
        //qDebug() << "_on Intputs List Will Be Removed from agent" << agentInMapping->name() << inputsListWillBeRemoved.count();

        for (MapBetweenIOPVM* link : _allLinksInMapping.toList())
        {
            if ((link != NULL) && (link->inputAgent() != NULL) && (link->inputAgent() == agentInMapping)
                    && (link->input() != NULL) && inputsListWillBeRemoved.contains(link->input()))
            {
                // Remove the link between two agents from the mapping
                //removeLinkBetweenTwoAgents(link);

                // Delete the link between two agents
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }
}


/**
 * @brief Slot when some view models of outputs will be removed from an agent in mapping
 * @param outputsListWillBeRemoved
 */
void AgentsMappingController::_onOutputsListWillBeRemoved(QList<OutputVM*> outputsListWillBeRemoved)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != NULL) && !outputsListWillBeRemoved.isEmpty())
    {
        //qDebug() << "_on Outputs List Will Be Removed from agent" << agentInMapping->name() << outputsListWillBeRemoved.count();

        for (MapBetweenIOPVM* link : _allLinksInMapping.toList())
        {
            if ((link != NULL) && (link->outputAgent() != NULL) && (link->outputAgent() == agentInMapping)
                    && (link->output() != NULL) && outputsListWillBeRemoved.contains(link->output()))
            {
                // Remove the link between two agents from the mapping
                //removeLinkBetweenTwoAgents(link);

                // Delete the link between two agents
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }
}


/**
 * @brief Add new model(s) of agent to the current mapping at a specific position
 * @param agentName
 * @param agentsList
 * @param position
 */
void AgentsMappingController::_addAgentModelsToMappingAtPosition(QString agentName, QList<AgentM*> agentsList, QPointF position)
{
    if (!agentName.isEmpty() && !agentsList.isEmpty())
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

        // The agent is defined, only add models of agent in the internal list of the agentInMappingVM
        if (agentInMapping != NULL)
        {   
            for (AgentM* model : agentsList)
            {
                if ((model != NULL) && !agentInMapping->models()->contains(model))
                {
                    agentInMapping->models()->append(model);

                    qInfo() << "The agent in mapping already exist, new agent model added to" << agentName;
                }
            }
        }
        // Instanciate a new view model of agent in mapping (and add models)
        else
        {
            // Create a new Agent In Mapping
            agentInMapping = new AgentInMappingVM(agentsList, position, this);

            // Add in the map list
            _mapFromNameToAgentInMapping.insert(agentName, agentInMapping);

            // Add this new Agent In Mapping VM in the list for the qml
            _allAgentsInMapping.append(agentInMapping);

            qInfo() << "A new agent mapping has been added:" << agentName;
        }
    }
}


/**
 * @brief Delete a link between two agents
 * @param link
 */
void AgentsMappingController::_deleteLinkBetweenTwoAgents(MapBetweenIOPVM* link)
{
    if (link != NULL)
    {
        // Unselect the link if it is the currently selected one
        if (_selectedLink == link) {
            setselectedLink(NULL);
        }

        // Remove from the list
        _allLinksInMapping.remove(link);

        // Free memory
        delete link;
    }
}


/**
 * @brief Get the view model of link which corresponds to a mapping element
 * @param mappingElement
 * @return
 */
MapBetweenIOPVM* AgentsMappingController::_getLinkFromMappingElement(ElementMappingM* mappingElement)
{
    MapBetweenIOPVM* link = NULL;

    if (mappingElement != NULL)
    {
        for (MapBetweenIOPVM* iterator : _allLinksInMapping.toList())
        {
            // FIXME: An agent in mapping can have several Inputs (or Outputs) with the same name but with different types
            // --> Instead, this method must return a list of MapBetweenIOPVM
            if ((iterator != NULL)
                    && (iterator->outputAgent() != NULL) && (iterator->outputAgent()->name() == mappingElement->outputAgent())
                    && (iterator->inputAgent() != NULL) && (iterator->inputAgent()->name() == mappingElement->inputAgent())
                    && (iterator->output() != NULL) && (iterator->output()->name() == mappingElement->output())
                    && (iterator->input() != NULL) && (iterator->input()->name() == mappingElement->input()))
            {
                link = iterator;
                break;
            }
        }
    }

    return link;
}


/**
 * @brief Remove all the links with this agent
 * @param agent
 */
void AgentsMappingController::_removeAllLinksWithAgent(AgentInMappingVM* agent)
{
    if ((agent != NULL) && (_modelManager != NULL))
    {
        for (MapBetweenIOPVM* link : _allLinksInMapping.toList())
        {
            if ( (link != NULL) && ((link->outputAgent() == agent) || (link->inputAgent() == agent)) )
            {
                // Remove a link between two agents from the mapping
                removeLinkBetweenTwoAgents(link);

                // Mapping is activated
                if (_modelManager->isMappingActivated())
                {
                    // We have to delete the link to clean our HMI (even if we do not have yet received the message "UNMAPPED")
                    _deleteLinkBetweenTwoAgents(link);
                }
            }
        }
    }
}


/**
 * @brief OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
 * @param agentModel
 * @param temporaryMapping
 */
void AgentsMappingController::_overWriteMappingOfAgentModel(AgentM* agentModel, AgentMappingM* temporaryMapping)
{
    // Model is ON
    if ((agentModel != NULL) && agentModel->isON() && (temporaryMapping != NULL) && (_jsonHelper != NULL)
            && (_modelManager != NULL) && _modelManager->isMappingActivated()  && _modelManager->isMappingControlled())
    {
        QStringList peerIdsList = QStringList(agentModel->peerId());

        // Get the JSON of the agent mapping
        QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(temporaryMapping, QJsonDocument::Compact);

        QString command = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

        // Emit signal "Command asked to agent"
        Q_EMIT commandAskedToAgent(peerIdsList, command);
    }
}


/**
 * @brief Get a random position in the current window
 * @param randomMax
 * @return
 */
QPointF AgentsMappingController::_getRandomPosition(double randomMax)
{
    double randomX = (double)qrand() / randomMax;
    double randomY = (double)qrand() / randomMax;

    // 5% + (random * 90% of the width)
    double x = 0.05 * _viewWidth + (0.90 * _viewWidth * randomX);

    // 5% + (random * 90% of the height)
    double y = 0.05 * _viewHeight + (0.90 * _viewHeight * randomY);

    return QPointF(x, y);
}
