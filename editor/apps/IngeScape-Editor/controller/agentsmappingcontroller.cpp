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
      _selectedAgent(nullptr),
      _selectedLink(nullptr),
      _modelManager(modelManager),
      _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Connect to signal "Count Changed" from the list of all agents in mapping
    connect(&_allAgentsInMapping, &AbstractI2CustomItemListModel::countChanged, this, &AgentsMappingController::_onAllAgentsInMappingChanged);

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

    // Clear the current mapping
    clearMapping();

    // Delete all links
    _hashFromNameToLinkInMapping.clear();
    _hashFromAgentNameToListOfWaitingLinks.clear();
    _allLinksInMapping.deleteAllItems();

    // Delete all agents in mapping
    _hashFromNameToAgentInMapping.clear();
    _allAgentsInMapping.deleteAllItems();

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Clear the current mapping
 */
void AgentsMappingController::clearMapping()
{
    qInfo() << "Clear the current mapping";

    // 1- First, DE-activate the mapping
    if (_modelManager != nullptr) {
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
    if (agent != nullptr)
    {
        qInfo() << "Delete the agent" << agent->name() << "in the Mapping";

        // Unselect our agent if needed
        if (_selectedAgent == agent) {
            setselectedAgent(nullptr);
        }

        // DIS-connect to signals from this agent in mapping
        disconnect(agent, 0, this, 0);

        // Remove from the hash table
        _hashFromNameToAgentInMapping.remove(agent->name());

        // Remove all the links with this agent
        _removeAllLinksWithAgent(agent);

        // Remove from the list to update view (QML)
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
        if ((_modelManager != nullptr) && _modelManager->isMappingActivated() && link->inputAgent()->agentsGroupedByName()->isON())
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
            else
            {
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
 * @brief Called when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param position
 */
void AgentsMappingController::dropAgentNameToMappingAtPosition(QString agentName, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
    if ((agentInMapping == nullptr) && (_modelManager != nullptr))
    {
        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentName);
        if (agentsGroupedByName != nullptr)
        {
            // Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
            _createAgentInMappingAtPosition(agentsGroupedByName, position);

            agentInMapping = getAgentInMappingFromName(agentName);
            if (agentInMapping != nullptr)
            {
                // Get the mapping currently edited (temporary until the user activate the mapping)
                AgentMappingM* temporaryMapping = agentInMapping->temporaryMapping();
                if (temporaryMapping != nullptr)
                {
                    // Delete all "mapping elements" in this temporary mapping
                    temporaryMapping->mappingElements()->deleteAllItems();

                    // Mapping is already activated
                    if ((_modelManager != nullptr) && _modelManager->isMappingActivated())
                    {
                        for (AgentM* model : agentsGroupedByName->models()->toList())
                        {
                            if (model != nullptr)
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
            // Name with all names formatted: "outputAgent##output-->inputAgent##input"
            //QString linkName = QString("%1%2%3-->%4%2%5").arg(outputAgent->name(), SEPARATOR_AGENT_NAME_AND_IOP, linkOutput->name(), inputAgent->name(), linkInput->name());

            //LinkVM* link = getLinkInMappingFromName(linkName);

            // Search if the same link already exists
            bool alreadyLinked = false;
            for (LinkVM* iterator : _allLinksInMapping.toList())
            {
                if ((iterator != nullptr)
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
                if ((_modelManager != nullptr) && _modelManager->isMappingActivated() && inputAgent->agentsGroupedByName()->isON())
                {
                    // Add a temporary link (this temporary link will became a real link when the agent will send its mapping update)
                    inputAgent->addTemporaryLink(linkInput->name(), outputAgent->name(), linkOutput->name());

                    // Create a new VIRTUAL link between agents
                    LinkVM* link = new LinkVM(outputAgent, linkOutput, inputAgent, linkInput, true, this);

                    _allLinksInMapping.append(link);
                    _hashFromNameToLinkInMapping.insert(link->name(), link);

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

                    _allLinksInMapping.append(link);
                    _hashFromNameToLinkInMapping.insert(link->name(), link);

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
            if ((linkOutput->output()->firstModel() != nullptr) && (linkInput->input()->firstModel() != nullptr))
            {
                qDebug() << "Can not link output" << linkOutput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkOutput->output()->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->name() << ")"
                         << "and input" << linkInput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkInput->input()->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->name() << ")";
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
 * @brief Get the (view model of) link between agents in the global mapping from a link name
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
 * @brief Export the global mapping (of agents) into JSON
 * @return array of all agents and their mapping
 */
QJsonArray AgentsMappingController::exportGlobalMappingToJSON()
{
    QJsonArray jsonArray;

    if (_jsonHelper != nullptr)
    {
        for (AgentInMappingVM* agentInMapping : _allAgentsInMapping)
        {
            if ((agentInMapping != nullptr) && (agentInMapping->temporaryMapping() != nullptr))
            {
                QJsonObject jsonAgent;

                // Set the agent name
                jsonAgent.insert("agentName", agentInMapping->name());

                // Set the position
                QString position = QString("%1, %2").arg(QString::number(agentInMapping->position().x()), QString::number(agentInMapping->position().y()));
                jsonAgent.insert("position", position);

                // Set the mapping
                QJsonObject jsonMapping = _jsonHelper->exportAgentMappingToJson(agentInMapping->temporaryMapping());
                jsonAgent.insert("mapping", jsonMapping);

                jsonArray.append(jsonAgent);
            }
        }
    }
    return jsonArray;
}


/**
 * @brief Import the global mapping (of agents) from JSON
 * @param jsonArrayOfAgentsInMapping
 */
void AgentsMappingController::importMappingFromJson(QJsonArray jsonArrayOfAgentsInMapping)
{
    if (_jsonHelper != nullptr)
    {
        QList<ElementMappingM*> mappingElements;

        for (QJsonValue jsonValue : jsonArrayOfAgentsInMapping)
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

                    if ((agentsGroupedByName != nullptr) && (agentMapping != nullptr) && !position.isNull())
                    {
                        qDebug() << "Position:" << position.x() << position.y() << "is defined for" << agentName << "with" << agentsGroupedByName->models()->count() << "models";

                        // Create a new agent in the global mapping (with the "Agents Grouped by Name") at a specific position
                        _createAgentInMappingAtPosition(agentsGroupedByName, position);

                        // Get the agent in mapping from the name
                        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);
                        if (agentInMapping != nullptr)
                        {
                            // Set the temporary mapping
                            if (agentMapping != nullptr) {
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
                if (mappingElement != nullptr) {
                    onMapped(mappingElement);
                }
            }
        }
    }
}


/**
 * @brief Reset the modifications made while the mapping was UN-activated
 */
void AgentsMappingController::resetModificationsWhileMappingWasUNactivated()
{
    qDebug() << "Reset the modifications made while the mapping was UN-activated";

    for (QString linkName : _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.keys())
    {
        //qDebug() << "Remove added link" << linkName << "while the mapping was disconnected";

        ElementMappingM* mappingElement = _hashFromLinkNameToAddedLinkWhileMappingWasUNactivated.value(linkName);
        if (mappingElement != nullptr)
        {
            // Get the view model of link which corresponds to a mapping element
            LinkVM* link = getLinkInMappingFromName(mappingElement->name());
            if (link != nullptr)
            {
                // Delete this link (between two agents) to cancel the add
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }

    for (QString linkName : _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.keys())
    {
        //qDebug() << "Add removed link" << linkName << "while the mapping was disconnected";

        ElementMappingM* mappingElement = _hashFromLinkNameToRemovedLinkWhileMappingWasUNactivated.value(linkName);
        if (mappingElement != nullptr)
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


    // Update the global mapping with agents ON and their links
    _updateMappingWithAgentsONandLinks();
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
    if ((previousModel != nullptr) && (newModel != nullptr))
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(newModel->name());
        if ((agentInMapping != nullptr) && agentInMapping->models()->contains(previousModel))
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
 * @brief Slot called when the flag "is Mapping Activated" changed
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
                // Update the global mapping with agents ON and their links
                _updateMappingWithAgentsONandLinks();
            }
        }
    }
}


/**
 * @brief Slot called when the flag "is Mapping Controlled" changed
 * @param isMappingControlled
 */
void AgentsMappingController::onIsMappingControlledChanged(bool isMappingControlled)
{
    qDebug() << "AgentsMappingController: Is Mapping Controlled Changed to" << isMappingControlled;
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
                    // Get the (view model of) agents grouped for a name
                    AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentName);
                    if (agentsGroupedByName != nullptr)
                    {
                        double randomMax = (double)RAND_MAX;

                        // Get a random position in the current window
                        QPointF position = _getRandomPosition(randomMax);

                        //qDebug() << "Random position:" << position << "for agent" << agentName;

                        // Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
                        _createAgentInMappingAtPosition(agentsGroupedByName, position);
                    }
                }
                // The agent is already in the mapping
                else
                {
                    // Nothing to do...already added to AgentsGroupedByName
                }

                // If there are waiting links (where this agent is involved as "Output Agent")
                if (_hashFromAgentNameToListOfWaitingLinks.contains(agentName))
                {
                    QList<ElementMappingM*> listOfWaitingLinks = _hashFromAgentNameToListOfWaitingLinks.value(agentName);
                    for (ElementMappingM* mappingElement : listOfWaitingLinks)
                    {
                        qDebug() << "Create waiting MAP..." << mappingElement->name();

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

            // FIXME: Even if the definition is different, the agent in mapping will be modified !
            // What do we do with the mapping (links) of this agent ?
        }
    }
}


/**
 * @brief Slot called when the mapping of an active agent has been defined
 * @param agent
 */
void AgentsMappingController::onActiveAgentMappingDefined(AgentM* agent)
{
    if ((agent != nullptr) && (agent->mapping() != nullptr) && (_modelManager != nullptr) && _modelManager->isMappingActivated())
    {
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agent->name());
        if ((agentInMapping != nullptr) && (agentInMapping->temporaryMapping() != nullptr))
        {
            QStringList namesOfRemovedMappingElements;
            for (QString namePreviousList : agentInMapping->temporaryMapping()->namesOfMappingElements())
            {
                if (!agent->mapping()->namesOfMappingElements().contains(namePreviousList)) {
                    namesOfRemovedMappingElements.append(namePreviousList);
                }
            }

            QStringList namesOfAddedMappingElements;
            for (QString nameNewList : agent->mapping()->namesOfMappingElements())
            {
                if (!agentInMapping->temporaryMapping()->namesOfMappingElements().contains(nameNewList)) {
                    namesOfAddedMappingElements.append(nameNewList);
                }
            }

            // If there are some Removed mapping elements
            if (!namesOfRemovedMappingElements.isEmpty())
            {
                qDebug() << "unmapped" << namesOfRemovedMappingElements;

                for (ElementMappingM* mappingElement : agentInMapping->temporaryMapping()->mappingElements()->toList())
                {
                    if ((mappingElement != nullptr) && namesOfRemovedMappingElements.contains(mappingElement->name()))
                    {
                        onUnmapped(mappingElement);
                    }
                }
            }
            // If there are some Added mapping elements
            if (!namesOfAddedMappingElements.isEmpty())
            {
                qDebug() << "mapped" << namesOfAddedMappingElements;

                for (ElementMappingM* mappingElement : agent->mapping()->mappingElements()->toList())
                {
                    if ((mappingElement != nullptr) && namesOfAddedMappingElements.contains(mappingElement->name()))
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
    if (mappingElement != nullptr)
    {
        //qDebug() << "MAPPED" << mappingElement->name();

        // Get the (view model of) link between agents in the global mapping from a link name
        LinkVM* link = getLinkInMappingFromName(mappingElement->name());

        if (link == nullptr)
        {
            AgentInMappingVM* outputAgent = getAgentInMappingFromName(mappingElement->outputAgent());
            AgentInMappingVM* inputAgent = getAgentInMappingFromName(mappingElement->inputAgent());

            if ((outputAgent != nullptr) && (inputAgent != nullptr))
            {
                LinkOutputVM* linkOutput = nullptr;
                LinkInputVM* linkInput = nullptr;

                if (outputAgent->agentsGroupedByName() != nullptr)
                {
                    // Get the list of view models of link output from the output name
                    QList<LinkOutputVM*> linkOutputsWithSameName = outputAgent->getLinkOutputsListFromName(mappingElement->output());
                    if (linkOutputsWithSameName.count() == 1) {
                        linkOutput = linkOutputsWithSameName.at(0);
                    }
                    else {
                        qWarning() << "There are" << linkOutputsWithSameName.count() << "link outputs with the same name" << mappingElement->output() << "."
                                   << "We cannot choose and create the link" << mappingElement->name();
                    }
                }

                if (inputAgent->agentsGroupedByName() != nullptr)
                {
                    // Get the list of view models of input from the input name
                    QList<LinkInputVM*> linkInputsWithSameName = inputAgent->getLinkInputsListFromName(mappingElement->input());
                    if (linkInputsWithSameName.count() == 1) {
                        linkInput = linkInputsWithSameName.at(0);
                    }
                    else {
                        qWarning() << "There are" << linkInputsWithSameName.count() << "link inputs with the same name" << mappingElement->input() << "."
                                   << "We cannot choose and create the link" << mappingElement->name();
                    }
                }

                if ((linkOutput != nullptr) && (linkInput != nullptr))
                {
                    qInfo() << "MAPPED" << mappingElement->name();

                    // Create a new map between agents
                    link = new LinkVM(outputAgent, linkOutput, inputAgent, linkInput, false, this);

                    _allLinksInMapping.append(link);
                    _hashFromNameToLinkInMapping.insert(link->name(), link);

                    qDebug() << "MAP has been created:" << mappingElement->name();

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
            else if (inputAgent != nullptr) // && (outputAgent == nullptr)
            {
                QList<ElementMappingM*> listOfWaitingLinks;

                if (_hashFromAgentNameToListOfWaitingLinks.contains(mappingElement->outputAgent())) {
                    listOfWaitingLinks = _hashFromAgentNameToListOfWaitingLinks.value(mappingElement->outputAgent());
                }

                // Add the mapping element to the list to create the corresponding link later
                listOfWaitingLinks.append(mappingElement);

                // Update the hash table
                _hashFromAgentNameToListOfWaitingLinks.insert(mappingElement->outputAgent(), listOfWaitingLinks);

                qDebug() << "MAP will be created later:" << mappingElement->name();
            }
        }
        else
        {
            if (link->isVirtual())
            {
                qInfo() << "MAPPED" << mappingElement->name();

                // Update the flag if needed
                link->setisVirtual(false);
            }
        }

        if ((link != nullptr) && (link->inputAgent() != nullptr))
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
    if (mappingElement != nullptr)
    {
        //qInfo() << "UN-mapped" << mappingElement->name();

        // Get the (view model of) link between agents in the global mapping from a link name
        LinkVM* link = getLinkInMappingFromName(mappingElement->name());

        if (link != nullptr)
        {
            if (link->inputAgent() != nullptr)
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

        // Name with all names formatted: "outputAgent##output-->inputAgent##input"
        QString linkName = QString("%1%2%3-->%4%2%5").arg(outputAgentName, SEPARATOR_AGENT_NAME_AND_IOP, outputName, inputAgentName, inputName);

        // Get the (view model of) link between agents in the global mapping from a link name
        LinkVM* link = getLinkInMappingFromName(linkName);

        if ((link != nullptr) && (link->linkOutput() != nullptr))
        {
            qDebug() << "Highlight the link" << linkName;

            // Simulate that the current value of output model changed: allows to highlight the link
            link->linkOutput()->simulateCurrentValueOfModelChanged();
        }
    }
}


/**
 * @brief Slot called when the list of all "Agents in Mapping" changed
 */
void AgentsMappingController::_onAllAgentsInMappingChanged()
{
    // Update the flag "is Empty Mapping"
    if (_allAgentsInMapping.isEmpty()) {
        setisEmptyMapping(true);
    }
    else {
        setisEmptyMapping(false);
    }
}


/**
 * @brief Slot called when some view models of link inputs have been added to an agent in mapping
 * @param addedlinkInputs
 */
void AgentsMappingController::_onLinkInputsListHaveBeenAdded(QList<LinkInputVM*> addedlinkInputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !addedlinkInputs.isEmpty())
    {
        QStringList namesOfInputs;
        for (LinkInputVM* linkInput : addedlinkInputs)
        {
            if (linkInput != nullptr) {
                namesOfInputs.append(linkInput->name());
            }
        }

        if (agentInMapping->temporaryMapping() != nullptr)
        {
            for (ElementMappingM* mappingElement : agentInMapping->temporaryMapping()->mappingElements()->toList())
            {
                if ((mappingElement != nullptr) && namesOfInputs.contains(mappingElement->input()))
                {
                    // Try to create the link corresponding to the mapping element
                    onMapped(mappingElement);
                }
            }
        }
    }
}


/**
 * @brief Slot called when some view models of link outputs have been added to an agent in mapping
 * @param addedlinkOutputs
 */
void AgentsMappingController::_onLinkOutputsListHaveBeenAdded(QList<LinkOutputVM*> addedlinkOutputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !addedlinkOutputs.isEmpty())
    {
        QStringList namesOfOutputs;
        for (LinkOutputVM* linkOutput : addedlinkOutputs)
        {
            if (linkOutput != nullptr) {
                namesOfOutputs.append(linkOutput->name());
            }
        }

        for (AgentInMappingVM* iterator : _allAgentsInMapping.toList())
        {
            if ((iterator != nullptr) && (iterator != agentInMapping) && (iterator->temporaryMapping() != nullptr))
            {
                for (ElementMappingM* mappingElement : iterator->temporaryMapping()->mappingElements()->toList())
                {
                    if ((mappingElement != nullptr) && (mappingElement->outputAgent() == agentInMapping->name()) && namesOfOutputs.contains(mappingElement->output()))
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
 * @brief Slot called when some view models of link inputs will be removed from an agent in mapping
 * @param removedLinkInputs
 */
void AgentsMappingController::_onLinkInputsListWillBeRemoved(QList<LinkInputVM*> removedLinkInputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !removedLinkInputs.isEmpty())
    {
        //qDebug() << "_on Link Intputs List will be Removed from agent" << agentInMapping->name() << removedLinkInputs.count();

        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->inputAgent() != nullptr) && (link->inputAgent() == agentInMapping)
                    && (link->linkInput() != nullptr) && removedLinkInputs.contains(link->linkInput()))
            {
                // Delete the link between two agents
                _deleteLinkBetweenTwoAgents(link);
            }
        }
    }
}


/**
 * @brief Slot called when some view models of link outputs will be removed from an agent in mapping
 * @param removedLinkOutputs
 */
void AgentsMappingController::_onLinkOutputsListWillBeRemoved(QList<LinkOutputVM*> removedLinkOutputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !removedLinkOutputs.isEmpty())
    {
        //qDebug() << "_on Link Outputs List will be Removed from agent" << agentInMapping->name() << removedLinkOutputs.count();

        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->outputAgent() != nullptr) && (link->outputAgent() == agentInMapping)
                    && (link->linkOutput() != nullptr) && removedLinkOutputs.contains(link->linkOutput()))
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

            // Connect to signals from this new agent in mapping
            connect(agentInMapping, &AgentInMappingVM::linkInputsListHaveBeenAdded, this, &AgentsMappingController::_onLinkInputsListHaveBeenAdded);
            connect(agentInMapping, &AgentInMappingVM::linkOutputsListHaveBeenAdded, this, &AgentsMappingController::_onLinkOutputsListHaveBeenAdded);
            connect(agentInMapping, &AgentInMappingVM::linkInputsListWillBeRemoved, this, &AgentsMappingController::_onLinkInputsListWillBeRemoved);
            connect(agentInMapping, &AgentInMappingVM::linkOutputsListWillBeRemoved, this, &AgentsMappingController::_onLinkOutputsListWillBeRemoved);

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
 * @brief Delete a link between two agents
 * @param link
 */
void AgentsMappingController::_deleteLinkBetweenTwoAgents(LinkVM* link)
{
    if (link != nullptr)
    {
        qInfo() << "Delete the link" << link->name() << "in the Mapping";

        // Unselect our link if needed
        if (_selectedLink == link) {
            setselectedLink(nullptr);
        }

        // Remove from the hash table
        _hashFromNameToLinkInMapping.remove(link->name());

        // Remove from the list to update view (QML)
        _allLinksInMapping.remove(link);

        // Free memory
        delete link;
    }
}


/**
 * @brief Remove all the links with an agent
 * @param agent
 */
void AgentsMappingController::_removeAllLinksWithAgent(AgentInMappingVM* agent)
{
    if ((agent != nullptr) && !_allLinksInMapping.isEmpty())
    {
        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ( (link != nullptr) && ((link->outputAgent() == agent) || (link->inputAgent() == agent)) )
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
    if ((agentModel != nullptr) && agentModel->isON() && (temporaryMapping != nullptr) && (_jsonHelper != nullptr)
            && (_modelManager != nullptr) && _modelManager->isMappingActivated() && _modelManager->isMappingControlled())
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
 * @brief Update the global mapping with agents ON and their links
 */
void AgentsMappingController::_updateMappingWithAgentsONandLinks()
{
    if (_modelManager != nullptr)
    {
        double randomMax = (double)RAND_MAX;

        // Traverse the list of all "agents grouped by name"
        for (AgentsGroupedByNameVM* agentsGroupedByName : _modelManager->allAgentsGroupsByName()->toList())
        {
            // Only when the agent(s) is ON
            if ((agentsGroupedByName != nullptr) && agentsGroupedByName->isON() && !agentsGroupedByName->name().isEmpty())
            {
                AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentsGroupedByName->name());

                // This agent(s) is NOT yet in the global mapping
                if (agentInMapping == nullptr)
                {
                    // Get a random position in the current window
                    QPointF position = _getRandomPosition(randomMax);

                    //qDebug() << "Random position:" << position << "for agent" << agentsGroupedByName->name();

                    // Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position
                    _createAgentInMappingAtPosition(agentsGroupedByName, position);
                }
            }
        }


        // FIXME TODO: optimize to not go through the mapping of each model

        // Create all links between agents in the mapping
        // By looking the mapping of each (sub) model of the VM
        for (AgentInMappingVM* agentInMapping : _allAgentsInMapping.toList())
        {
            if ((agentInMapping != nullptr) && (agentInMapping->agentsGroupedByName() != nullptr) && (agentInMapping->temporaryMapping() != nullptr))
            {
                // First, delete all "mapping elements" in the temporary mapping
                agentInMapping->temporaryMapping()->mappingElements()->deleteAllItems();

                // Traverse through each model
                for (AgentM* model : agentInMapping->agentsGroupedByName()->models()->toList())
                {
                    if ((model != nullptr) && (model->mapping() != nullptr))
                    {
                        for (ElementMappingM* mappingElement : model->mapping()->mappingElements()->toList())
                        {
                            if (mappingElement != nullptr)
                            {
                                // Simulate the slot "on Mapped"
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
