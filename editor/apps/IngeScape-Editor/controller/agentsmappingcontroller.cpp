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
                                                 QObject *parent) : QObject(parent),
      _viewWidth(1920 - 320), // Full HD - Width of left panel
      _viewHeight(1080 - 100), // Full HD - Height of top & bottom bars of OS
      _isEmptyMapping(true),
      _selectedAgent(NULL),
      _selectedLink(NULL),
      _modelManager(modelManager),
      _jsonHelper(jsonHelper)
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
    setselectedAgent(nullptr);
    setselectedLink(nullptr);

    // DIS-connect from signal "Count Changed" from the list of agents in mapping
    disconnect(&_allAgentsInMapping, 0, this, 0);

    // Delete all links
    _hashFromNameToLinkInMapping.clear();
    _hashFromAgentNameToListOfWaitingLinks.clear();
    _allLinksInMapping.deleteAllItems();

    // Delete all agents in mapping
    _previousListOfAgentsInMapping.clear();
    _hashFromNameToAgentInMapping.clear();
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
    for (LinkVM* link : _allLinksInMapping.toList()) {
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
            setselectedAgent(nullptr);
        }

        // Remove from hash table
        _hashFromNameToAgentInMapping.remove(agent->name());

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
 * @return true if the link has been deleted during the call of our method
 */
bool AgentsMappingController::removeLinkBetweenTwoAgents(LinkVM* link)
{
    bool linkHasBeenDeleted = false;

    if ((link != nullptr)
            && (link->inputAgent() != nullptr) && (link->inputAgent()->agentsGroupedByName() != nullptr) && (link->linkInput() != nullptr)
            && (link->outputAgent() != nullptr) && (link->outputAgent()->agentsGroupedByName() != nullptr) && (link->linkOutput() != nullptr))
    {
        qInfo() << "Remove the link between agents" << link->outputAgent()->name() << "and" << link->inputAgent()->name();

        // Mapping is activated
        // AND the input agent is ON
        if ((_modelManager != NULL) && _modelManager->isMappingActivated() && link->inputAgent()->agentsGroupedByName()->isON())
        {
            // Set to virtual to give a feedback to the user
            link->setisVirtual(true);

            // Emit signal "Command asked to agent about Mapping Input"
            Q_EMIT commandAskedToAgentAboutMappingInput(link->inputAgent()->agentsGroupedByName()->peerIdsList(),
                                                        command_UnmapAgents,
                                                        link->linkInput()->name(),
                                                        link->outputAgent()->name(),
                                                        link->linkOutput()->name());
        }
        // Mapping is NOT activated
        // OR the input agent is OFF
        else
        {
            // Remove temporary link (this temporary link will be removed when the user will activate the mapping)
            link->inputAgent()->removeTemporaryLink(link->linkInput()->name(), link->outputAgent()->name(), link->linkOutput()->name());

            qDebug() << "Remove TEMPORARY link" << link->name();

            // This link has been added while the mapping was UN-activated, just cancel the add
            if (_hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.contains(link->name()))
            {
                ElementMappingM* mappingElement = _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.value(link->name());
                _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.remove(link->name());
                delete mappingElement;
            }
            // Add this link to the hash
            else {
                ElementMappingM* mappingElement = new ElementMappingM(link->inputAgent()->name(),
                                                                      link->linkInput()->name(),
                                                                      link->outputAgent()->name(),
                                                                      link->linkOutput()->name());
                _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.insert(link->name(), mappingElement);
            }

            // Delete the link between two agents
            _deleteLinkBetweenTwoAgents(link);

            linkHasBeenDeleted = true;
        }
    }

    return linkHasBeenDeleted;
}


/**
 * @brief FIXME REPAIR: Called when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param models
 * @param position
 */
/*void AgentsMappingController::dropAgentToMappingAtPosition(QString agentName, AbstractI2CustomItemListModel* models, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
    if (agentInMapping == nullptr)
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
}*/


/**
 * @brief Slot called when a link from an output is dropped over an input on the current mapping
 * Or when a link to an input is dropped over an output
 * @param outputAgent
 * @param linkOutput
 * @param inputAgent
 * @param linkInput
 */
void AgentsMappingController::dropLinkBetweenAgents(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput)
{
    if ((outputAgent != nullptr) && (outputAgent->agentsGroupedByName() != nullptr) && (linkOutput != nullptr) && (linkOutput->output() != nullptr)
            && (inputAgent != nullptr) && (inputAgent->agentsGroupedByName() != nullptr) && (linkInput != nullptr) && (linkInput->input() != nullptr))
    {
        // Check that the input can link to the output
        if (linkInput->canLinkWith(linkOutput))
        {
            // If an Input (or an Output) have the same name used for 2 different types --> test pointers instead of names
            //LinkVM* link = _getLinkFromNames(outputAgent->name(), output->name(), inputAgent->name(), input->name());

            // Search if the same link already exists
            bool alreadyLinked = false;
            for (LinkVM* iterator : _allLinksInMapping.toList())
            {
                if ((iterator != NULL)
                        && (iterator->outputAgent() == outputAgent) && (iterator->linkOutput() == linkOutput)
                        && (iterator->inputAgent() == inputAgent) && (iterator->linkInput() == linkInput))
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
                // AND the input agent is ON
                if ((_modelManager != NULL) && _modelManager->isMappingActivated() && inputAgent->agentsGroupedByName()->isON())
                {
                    // Add a temporary link (this temporary link will became a real link when the agent will send its mapping update)
                    inputAgent->addTemporaryLink(linkInput->name(), outputAgent->name(), linkOutput->name());

                    // Create a new VIRTUAL link between agents
                    LinkVM* link = new LinkVM(outputAgent, linkOutput, inputAgent, linkInput, true, this);

                    // Add to the list
                    _allLinksInMapping.append(link);

                    // Emit signal "Command asked to agent about Mapping Input"
                    Q_EMIT commandAskedToAgentAboutMappingInput(inputAgent->agentsGroupedByName()->peerIdsList(),
                                                                command_MapAgents,
                                                                linkInput->name(),
                                                                outputAgent->name(),
                                                                linkOutput->name());
                }
                // Mapping is NOT activated
                // OR the input agent is OFF
                else
                {
                    // Add a temporary link (this temporary link will became a real link when the user will activate the mapping)
                    inputAgent->addTemporaryLink(linkInput->name(), outputAgent->name(), linkOutput->name());

                    // Create a new link between agents
                    LinkVM* link = new LinkVM(outputAgent, linkOutput, inputAgent, linkInput, false, this);

                    // Add to the list
                    _allLinksInMapping.append(link);

                    qDebug() << "Add TEMPORARY link" << link->name();

                    // This link has been removed while the mapping was UN-activated, just cancel the suppression
                    if (_hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.contains(link->name()))
                    {
                        ElementMappingM* mappingElement = _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.value(link->name());
                        _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.remove(link->name());
                        delete mappingElement;
                    }
                    // Add this link to the hash
                    else {
                        ElementMappingM* mappingElement = new ElementMappingM(inputAgent->name(), linkInput->name(), outputAgent->name(), linkOutput->name());
                        _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.insert(link->name(), mappingElement);
                    }
                }
            }
            else {
                qWarning() << "The input" << linkInput->name() << "(of agent" << inputAgent->name() << ") is already linked to output" << linkOutput->name() << "(of agent" << outputAgent->name() << ")";
            }
        }
        else
        {
            if ((output->firstModel() != NULL) && (input->firstModel() != NULL)) {
                qDebug() << "Can not link output" << linkOutput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkOutput->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->name() << ")"
                         << "and input" << linkInput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkInput->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->name() << ")";
            }
        }
    }
}


/**
 * @brief Get the (view model of) agent in the global mapping from an agent name
 * @param name
 * @return
 */
AgentInMappingVM* AgentsMappingController::getAgentInMappingFromName(QString name)
{
    if (_hashFromNameToAgentInMapping.contains(name)) {
        return _hashFromNameToAgentInMapping.value(name);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the (view model of) link between agents in the global mapping from an agent name
 * @param name
 * @return
 */
LinkVM* AgentsMappingController::getLinkInMappingFromName(QString name)
{
    if (_hashFromNameToLinkInMapping.contains(name)) {
        return _hashFromNameToLinkInMapping.value(name);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Import a mapping of agents from a JSON array
 * @param jsonArrayOfAgentsMapping
 */
void AgentsMappingController::importMappingFromJson(QJsonArray jsonArrayOfAgentsMapping)
{
    // FIXME REPAIR: importMappingFromJson
    Q_UNUSED(jsonArrayOfAgentsMapping)

    /*if (_jsonHelper != NULL)
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

                    // Get the (view model of) agents grouped for this name
                    AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentName);

                    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->models()->isEmpty() && !position.isNull())
                    {
                        qDebug() << "Position:" << position.x() << position.y() << "is defined for" << agentName << "with" << agentsGroupedByName->models()->count() << "models";

                        // FIXME TODO: use directly the AgentsGroupedByNameVM instead of agentsGroupedByName->models()->toList()

                        // Create a new Agent In Mapping
                        _addAgentModelsToMappingAtPosition(agentName, agentsGroupedByName->models()->toList(), position);

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
            for (ElementMappingM* mappingElement : mappingElements)
            {
                if (mappingElement != NULL) {
                    onMapped(mappingElement);
                }
            }
        }
    }*/
}


/**
 * @brief Reset the modifications made while the mapping was UN-activated
 */
void AgentsMappingController::resetModificationsWhileMappingWasUNactivated()
{
    qDebug() << "Reset the modifications made while the mapping was UN-activated";

    for (QString linkId : _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.keys())
    {
        //qDebug() << "Remove added link" << linkId << "while the mapping was disconnected";

        ElementMappingM* mappingElement = _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.value(linkId);
        if (mappingElement != NULL)
        {
            // Get the view model of link which corresponds to a mapping element
            LinkVM* link = _getLinkFromMappingElement(mappingElement);
            if (link != NULL)
            {
                // Delete this link (between two agents) to cancel the add
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }

    for (QString linkId : _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.keys())
    {
        //qDebug() << "Add removed link" << linkId << "while the mapping was disconnected";

        ElementMappingM* mappingElement = _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.value(linkId);
        if (mappingElement != NULL)
        {
            // Make a copy
            ElementMappingM* copy = new ElementMappingM(mappingElement->inputAgent(), mappingElement->input(), mappingElement->outputAgent(), mappingElement->output());

            // Simulate slot "onMapped" to create a link (between two agents) to cancel the remove
            onMapped(copy);

            // FIXME: we cannot delete this mapping element because it could be added to _hashFromAgentNameToListOfWaitingLinks in the slot "onMapped"
            //delete copy;
        }
    }

    // Clear modifications made while the mapping was UN-activated
    qDeleteAll(_hashFromLinkNameToAddedLinkWhileMappingWasUNactivated);
    _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.clear();

    qDeleteAll(_hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated);
    _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.clear();


    // Update the (global) mapping with all models of agents and links
    _updateMappingWithModelsOfAgentsAndLinks();
}


// FIXME Usefull to conect to signal "identicalAgentModelWillBeReplaced" from AgentsGroupedByNameVM and to call "_overWriteMappingOfAgentModel" ?
// Because "_overWriteMappingOfAgentModel" is already called in AgentsMappingController::onActiveAgentDefined
/**
 * @brief Slot when a previous agent model is replaced by a new one strictly identical
 * @param previousModel
 * @param newModel
 */
/*void AgentsMappingController::onIdenticalAgentModelReplaced(AgentM* previousModel, AgentM* newModel)
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
}*/


/**
 * @brief Slot when the flag "is Mapping Activated" changed
 * @param isMappingActivated
 */
void AgentsMappingController::onIsMappingActivatedChanged(bool isMappingActivated)
{
    if ((_modelManager != nullptr) && (_jsonHelper != nullptr) && isMappingActivated)
    {
        // CONTROL
        if (_modelManager->isMappingControlled())
        {
            qDebug() << "Mapping Activated in mode CONTROL";

            // Clear modifications made while the mapping was UN-activated
            qDeleteAll(_hashFromLinkNameToAddedLinkWhileMappingWasUNactivated);
            _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.clear();

            qDeleteAll(_hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated);
            _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.clear();


            // Apply all temporary mappings
            for (AgentInMappingVM* agentInMapping : _allAgentsInMapping.toList())
            {
                if ((agentInMapping != nullptr) && (agentInMapping->agentsGroupedByName() != nullptr) && (agentInMapping->temporaryMapping() != nullptr))
                {
                    // Get the JSON of the agent mapping
                    QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(agentInMapping->temporaryMapping(), QJsonDocument::Compact);

                    QString command = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

                    // Emit signal "Command asked to agent"
                    Q_EMIT commandAskedToAgent(agentInMapping->agentsGroupedByName()->peerIdsList(), command);
                }
            }
        }
        // OBSERVE
        else
        {
            qDebug() << "Mapping Activated in mode OBSERVE";

            // There were modifications in the mapping while the mapping was UN-activated
            if (!_hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.isEmpty() || !_hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.isEmpty())
            {
                qDebug() << "There were modifications in the mapping while the mapping was UN-activated. Force to CONTROL ?\n"
                         << _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.keys() << "to ADD\n"
                         << _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.keys() << "to REMOVE";

                // The mapping has been modified but will be lost if the user stay in mode OBSERVE
                Q_EMIT modificationsOnLinksWhileMappingUnactivated();
            }
            else
            {
                // Update the (global) mapping with all models of agents and links
                _updateMappingWithModelsOfAgentsAndLinks();
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
 * @brief Slot called when a view model of agents grouped by name will be deleted
 * @param agentsGroupedByName
 */
void AgentsMappingController::onAgentsGroupedByNameWillBeDeleted(AgentsGroupedByNameVM* agentsGroupedByName)
{
    if (agentsGroupedByName != nullptr)
    {
        // Get the agent in mapping for the agent name
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentsGroupedByName->name());
        if (agentInMapping != nullptr)
        {
            // Delete this agent in the mapping
            deleteAgentInMapping(agentInMapping);
        }
    }
}


/**
 * @brief Slot when an active agent has been defined
 * @param agent
 */
void AgentsMappingController::onActiveAgentDefined(AgentM* agent)
{
    if ((agent != nullptr) && (_modelManager != nullptr))
    {
        QString agentName = agent->name();

        // Get the agent in mapping for the agent name
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

        // The mapping is activated
        if (_modelManager->isMappingActivated())
        {
            // CONTROL
            if (_modelManager->isMappingControlled())
            {
                QStringList peerIdsList = QStringList(agent->peerId());

                // The agent is not yet in the mapping...
                if (agentInMapping == nullptr)
                {
                    qDebug() << "CONTROL: Model of" << agentName << "is defined. CLEAR its MAPPING !";

                    // Send the command "Clear Mapping" on the network to this agent
                    Q_EMIT commandAskedToAgent(peerIdsList, command_ClearMapping);
                }
                // The agent is already in the mapping
                else
                {
                    /* FIXME Useless
                    if (!agentInMapping->models()->contains(agent))
                    {
                        agentInMapping->models()->append(agent);

                        // OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
                        _overWriteMappingOfAgentModel(agent, agentInMapping->temporaryMapping());
                    }*/

                    // OverWrite the mapping of the model of agent (with the mapping currently edited in the agent in mapping)
                    _overWriteMappingOfAgentModel(agent, agentInMapping->temporaryMapping());
                }
            }
            // OBSERVE
            else
            {
                //qDebug() << "OBSERVE: Model of" << agentName << "is defined. ADD in MAPPING view !" << agent;

                // The agent is not yet in the mapping...
                if (agentInMapping == nullptr)
                {
                    //QList<AgentM*> activeAgentsList;
                    //activeAgentsList.append(agent);

                    // Get the (view model of) agents grouped for a name
                    AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentName);
                    if (agentsGroupedByName != nullptr)
                    {
                        double randomMax = (double)RAND_MAX;

                        // Get a random position in the current window
                        QPointF position = _getRandomPosition(randomMax);

                        //qDebug() << "Random position:" << position << "for agent" << agentName;

                        // Add new model(s) of agent to the current mapping
                        //_addAgentModelsToMappingAtPosition(agentName, activeAgentsList, position);

                        // Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
                        _createAgentInMappingAtPosition(agentsGroupedByName, position);
                    }
                }
                // The agent is already in the mapping
                else
                {
                    // Nothing to do...already added to AgentsGroupedByName
                    /*if (!agentInMapping->models()->contains(agent)) {
                        agentInMapping->models()->append(agent);
                    }*/
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
        // The mapping is NOT activated
        else
        {
            // Nothing to do...already added to AgentsGroupedByName
            /*if ((agentInMapping != nullptr) && !agentInMapping->models()->contains(agent))
            {
                // FIXME: Even if the definition is different, the agent in mapping will be modified !
                // What do we do with the mapping (links) of this agent ?
                agentInMapping->models()->append(agent);
            }*/
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
        LinkVM* link = _getLinkFromMappingElement(mappingElement);

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
                    link = new LinkVM(outputAgent, output, inputAgent, input, false, this);

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
            //bool hasBeenAdded = link->inputAgent()->addTemporaryLink(mappingElement->input(), mappingElement->outputAgent(), mappingElement->output());
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
        LinkVM* link = _getLinkFromMappingElement(mappingElement);
        if (link != NULL)
        {
            if (link->inputAgent() != NULL)
            {
                // Remove the temporary link that correspond to this real link
                //bool hasBeenRemoved = link->inputAgent()->removeTemporaryLink(mappingElement->input(), mappingElement->outputAgent(), mappingElement->output());
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
        LinkVM* link = _getLinkFromNames(outputAgentName, outputName, inputAgentName, inputName);

        if ((link != nullptr) && (link->linkOutput() != nullptr))
        {
            qDebug() << "Highlight the link between" << inputAgentName << "." << inputName << "and " << outputAgentName << "." << outputName;

            // Simulate that the current value of output model changed: allows to highlight the link
            link->linkOutput()->simulateCurrentValueOfModelChanged();
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

        for (AgentInMappingVM* agentInMapping : newListOfAgentsInMapping)
        {
            if ((agentInMapping != NULL) && !_previousListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "ADDED";

                // Emit the signal "Agent in Mapping Added"
                //Q_EMIT agentInMappingAdded(agentInMapping);

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

        for (AgentInMappingVM* agentInMapping : _previousListOfAgentsInMapping)
        {
            if ((agentInMapping != NULL) && !newListOfAgentsInMapping.contains(agentInMapping))
            {
                qDebug() << "Agents Mapping Controller: Agent in mapping" << agentInMapping->name() << "REMOVED";

                // Emit the signal "Agent in Mapping Removed"
                //Q_EMIT agentInMappingRemoved(agentInMapping);

                // DIS-connect to signals from the previous agent in mapping
                disconnect(agentInMapping, 0, this, 0);
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
        for (InputVM* input : inputsListHaveBeenAdded)
        {
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
        for (OutputVM* output : outputsListHaveBeenAdded)
        {
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
    if ((agentInMapping != nullptr) && !inputsListWillBeRemoved.isEmpty())
    {
        //qDebug() << "_on Intputs List Will Be Removed from agent" << agentInMapping->name() << inputsListWillBeRemoved.count();

        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->inputAgent() != nullptr) && (link->inputAgent() == agentInMapping)
                    && (link->input() != nullptr) && inputsListWillBeRemoved.contains(link->input()))
            {
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
    if ((agentInMapping != nullptr) && !outputsListWillBeRemoved.isEmpty())
    {
        //qDebug() << "_on Outputs List Will Be Removed from agent" << agentInMapping->name() << outputsListWillBeRemoved.count();

        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->outputAgent() != nullptr) && (link->outputAgent() == agentInMapping)
                    && (link->output() != NULL) && outputsListWillBeRemoved.contains(link->output()))
            {
                // Delete the link between two agents
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }
}


/**
 * @brief Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
 * @param agentsGroupedByName
 * @param position
 */
void AgentsMappingController::_createAgentInMappingAtPosition(AgentsGroupedByNameVM* agentsGroupedByName, QPointF position)
{
    if (agentsGroupedByName != nullptr)
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentsGroupedByName->name());

        // Check that there is NOT yet an agent in the global mapping for this name
        if (agentInMapping == nullptr)
        {
            // Create a new view model of agent in the global Mapping
            agentInMapping = new AgentInMappingVM(agentsGroupedByName, position, this);

            // Add in the hash table
            _hashFromNameToAgentInMapping.insert(agentsGroupedByName->name(), agentInMapping);

            // Add in the list with all agents (for QML)
            _allAgentsInMapping.append(agentInMapping);

            qInfo() << "The agent" << agentsGroupedByName->name() << "has been added to the global mapping";
        }
        else {
            qCritical() << "There is already an agent in the global mapping named" << agentsGroupedByName->name();
        }
    }
}


/**
 * @brief Add new model(s) of agent to the current mapping at a specific position
 * @param agentName
 * @param agentsList
 * @param position
 */
/*void AgentsMappingController::_addAgentModelsToMappingAtPosition(QString agentName, QList<AgentM*> agentsList, QPointF position)
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
            _hashFromNameToAgentInMapping.insert(agentName, agentInMapping);

            // Add this new Agent In Mapping VM in the list for the qml
            _allAgentsInMapping.append(agentInMapping);

            qInfo() << "A new agent mapping has been added:" << agentName;
        }
    }
}*/


/**
 * @brief Delete a link between two agents
 * @param link
 */
void AgentsMappingController::_deleteLinkBetweenTwoAgents(LinkVM* link)
{
    if (link != NULL)
    {
        // Unselect the link if it is the currently selected one
        if (_selectedLink == link) {
            setselectedLink(nullptr);
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
/*LinkVM* AgentsMappingController::_getLinkFromMappingElement(ElementMappingM* mappingElement)
{
    LinkVM* link = nullptr;

    if (mappingElement != nullptr)
    {
        for (LinkVM* iterator : _allLinksInMapping.toList())
        {
            // FIXME: An agent in mapping can have several Inputs (or Outputs) with the same name but with different types
            // --> Instead, this method must return a list of LinkVM
            if ((iterator != nullptr)
                    && (iterator->outputAgent() != nullptr) && (iterator->outputAgent()->name() == mappingElement->outputAgent())
                    && (iterator->inputAgent() != nullptr) && (iterator->inputAgent()->name() == mappingElement->inputAgent())
                    && (iterator->linkOutput() != nullptr) && (iterator->linkOutput()->name() == mappingElement->output())
                    && (iterator->linkInput() != nullptr) && (iterator->linkInput()->name() == mappingElement->input()))
            {
                link = iterator;
                break;
            }
        }
    }
    return link;
}*/


/**
 * @brief Get the view model of link which corresponds to names
 * @param outputAgentName
 * @param outputName
 * @param inputAgentName
 * @param inputName
 * @return
 */
/*LinkVM* AgentsMappingController::_getLinkFromNames(QString outputAgentName, QString outputName, QString inputAgentName, QString inputName)
{
    LinkVM* link = NULL;

    for (LinkVM* iterator : _allLinksInMapping.toList())
    {
        // FIXME: An agent in mapping can have several Inputs (or Outputs) with the same name but with different types
        // --> Instead, this method must return a list of LinkVM
        if ((iterator != NULL)
                && (iterator->outputAgent() != NULL) && (iterator->outputAgent()->name() == outputAgentName)
                && (iterator->inputAgent() != NULL) && (iterator->inputAgent()->name() == inputAgentName)
                && (iterator->linkOutput() != NULL) && (iterator->linkOutput()->name() == outputName)
                && (iterator->linkInput() != NULL) && (iterator->linkInput()->name() == inputName))
        {
            link = iterator;
            break;
        }
    }
    return link;
}¨*/


/**
 * @brief Remove all the links with this agent
 * @param agent
 */
void AgentsMappingController::_removeAllLinksWithAgent(AgentInMappingVM* agent)
{
    if ((agent != NULL) && (_modelManager != NULL))
    {
        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ( (link != NULL) && ((link->outputAgent() == agent) || (link->inputAgent() == agent)) )
            {
                // Remove a link between two agents from the mapping
                bool linkHasBeenDeleted = removeLinkBetweenTwoAgents(link);

                if (!linkHasBeenDeleted)
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
    // AND
    // Global mapping is activated and controlled !
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
 * @brief FIXME REPAIR: Update the (global) mapping with all models of agents and links
 */
void AgentsMappingController::_updateMappingWithModelsOfAgentsAndLinks()
{
    if (_modelManager != NULL)
    {
        // FIXME: use AgentsGroupedByNameVM instead of the function "getMapFromAgentNameToActiveAgentsList"

        /*// Traverse the list of all "agents grouped by name"
        for (AgentsGroupedByNameVM* agentsGroupedByName : _modelManager->allAgentsGroupedByName())
        {
            // Only when the agent(s) is ON
            if ((agentsGroupedByName != nullptr) && agentsGroupedByName->isON())
            {

            }
        }*/

        // FIXME REPAIR: _updateMappingWithModelsOfAgentsAndLinks

        /*// Get the map from agent name to list of active agents
        QHash<QString, QList<AgentM*>> mapFromAgentNameToActiveAgentsList = _modelManager->getMapFromAgentNameToActiveAgentsList();

        if (!mapFromAgentNameToActiveAgentsList.isEmpty())
        {
            double randomMax = (double)RAND_MAX;

            // For each name of agent, we get all models of active agent (ON) and:
            // - create the corresponding agent in the mapping (VM) if not yet added
            // - or add eventually new model to existing agent in the mapping (VM)
            for (QString agentName : mapFromAgentNameToActiveAgentsList.keys())
            {
                QList<AgentM*> activeAgentsList = mapFromAgentNameToActiveAgentsList.value(agentName);

                // Get a random position in the current window
                QPointF position = _getRandomPosition(randomMax);

                //qDebug() << "Random position:" << position << "for agent" << agentName;

                // Add new model(s) of agent to the current mapping
                _addAgentModelsToMappingAtPosition(agentName, activeAgentsList, position);

                // Add the "Agents Grouped by Name" to the current mapping at a specific position
                //_addAgentsGroupedByNameToMappingAtPosition(agentsGroupedByName, position);
            }

            // Create all links between agents in the mapping
            // By looking the mapping of each (sub) model of the VM
            for (AgentInMappingVM* agent : _allAgentsInMapping.toList())
            {
                if ((agent != NULL) && (agent->temporaryMapping() != NULL))
                {
                    // First, delete all "mapping elements" in the temporary mapping
                    agent->temporaryMapping()->mappingElements()->deleteAllItems();

                    // Traverse through each model
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
        }*/
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
