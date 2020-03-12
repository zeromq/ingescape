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
 *      Chloé Roumieu      <roumieu@ingenuity.io
 */

#include "agentsmappingcontroller.h"

#include <QQmlEngine>
#include <QDebug>
#include <QFileDialog>
#include <model/actionmappingm.h>
#include <controller/ingescapenetworkcontroller.h>


AgentsMappingController::AgentsMappingController(EditorModelManager* modelManager,
                                                 QObject *parent) : QObject(parent),
      _viewWidth(1920 - 320), // Full HD - Width of left panel
      _viewHeight(1080 - 100), // Full HD - Height of top & bottom bars of OS
      _xSpawnZoneOffset(0),
      _ySpawnZoneOffset(0),
      _isEmptyMapping(true),
      _selectedAgent(nullptr),
      _selectedAction(nullptr),
      _selectedLink(nullptr),
      _isLoadedView(false),
      _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Connect to signal "Count Changed" from the list of all agents in mapping
    connect(&_allAgentsInMapping, &AbstractI2CustomItemListModel::countChanged, this, &AgentsMappingController::_onAllAgentsInMappingChanged);

}

AgentsMappingController::~AgentsMappingController()
{
    // Clean-up current selections
    setselectedAgent(nullptr);
    setselectedAction(nullptr);
    setselectedLink(nullptr);

    // DIS-connect from signal "Count Changed" from the list of agents in mapping
    disconnect(&_allAgentsInMapping, nullptr, this, nullptr);

    // Clear the current mapping
    clearMapping();

    // Delete all links
    _hashFromIdToLinkInMapping.clear();
    _hashFromNameToListOfLinksInMapping.clear();
    _hashFromOutputAgentNameToListOfWaitingMappingElements.clear();
    _allLinksInMapping.deleteAllItems();

    // Delete all agents in the mapping
    _hashFromNameToAgentInMapping.clear();
    _allAgentsInMapping.deleteAllItems();

    // Delete all actions in the mapping
    _hashFromUidToActionInMapping.clear();
    _allActionsInMapping.deleteAllItems();

    // Reset pointers
    _modelManager = nullptr;
}


void AgentsMappingController::setisLoadedView(bool value)
{
    if (_isLoadedView != value)
    {
        _isLoadedView = value;
        Q_EMIT isLoadedViewChanged(value);
    }
}

void AgentsMappingController::setselectedAgent(AgentsGroupedByNameVM *value)
{
    if (_selectedAgent != value)
    {
        if (_selectedAgent != nullptr)
        {
            _selectedAgent->setisSelected(false);
        }
        _selectedAgent = value;

        if (_selectedAgent != nullptr)
        {
            _selectedAgent->setisSelected(true);
        }
        Q_EMIT selectedAgentChanged(value);
    }
}


/**
 * @brief Clear the current mapping
 */
void AgentsMappingController::clearMapping()
{
    qInfo() << "Clear the current mapping";

    // Clear the hash table from "output agent name" to a list of waiting mapping elements (where the agent is involved as "output agent")
    _hashFromOutputAgentNameToListOfWaitingMappingElements.clear();

    // 1- Delete all links
    for (LinkVM* link : _allLinksInMapping.toList()) {
        _deleteLinkBetweenTwoObjectsInMapping(link);
    }

    // 2- Delete all agents in mapping
    for (AgentInMappingVM* agent : _allAgentsInMapping.toList()) {
        deleteAgentInMapping(agent);
    }

    // 3- Delete all actions in mapping
    for (ActionInMappingVM* action : _allActionsInMapping.toList()) {
        deleteActionInMapping(action);
    }

//    qInfo() << "The Mapping is empty !";
}


/**
 * @brief Remove the agent from the mapping and delete the view model
 * @param agent
 */
void AgentsMappingController::deleteAgentInMapping(AgentInMappingVM* agent)
{
    if (agent != nullptr)
    {
        //qDebug() << "Delete the agent" << agent->name() << "in the Mapping";

        // Unselect our agent if needed
        if (_selectedAgent == agent->agentsGroupedByName())
        {
            setselectedAgent(nullptr);
        }

        // DIS-connect to signals from this agent in mapping
        disconnect(agent, nullptr, this, nullptr);

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
 * @brief Remove the action from the mapping and delete the view model
 * @param action
 */
void AgentsMappingController::deleteActionInMapping(ActionInMappingVM* action)
{
    if (action != nullptr)
    {
        QString actionId = action->uid();

        //qDebug() << "Delete the action" << action->name() << "(" << actionId << ") in the Mapping";

        // Unselect our action if needed
        if (_selectedAction == action) {
            setselectedAction(nullptr);
        }

        // DIS-connect to signals from this action in mapping
        disconnect(action, nullptr, this, nullptr);

        // Remove from the hash table
        _hashFromUidToActionInMapping.remove(actionId);

        // Remove all the links with this action
        //_removeAllLinksWithAction(action);
        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ( (link != nullptr) && ((link->outputObject() == action) || (link->inputObject() == action)) )
            {
                // Delete the link between two objects in the mapping
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }

        // Remove from the list to update view (QML)
        _allActionsInMapping.remove(action);

        // Free memory
        delete action;

        // Free the UID of the action model
        IngeScapeUtils::freeUIDofActionInMappingVM(actionId);
    }
}


/**
 * @brief Remove a link between two objects in the mapping
 * @param link
 */
void AgentsMappingController::removeLinkBetweenTwoObjectsInMapping(LinkVM* link)
{
    if ((link != nullptr)
            && (link->inputObject() != nullptr) && (link->linkInput() != nullptr)
            && (link->outputObject() != nullptr) && (link->linkOutput() != nullptr))
    {
        // 2 agents
        if ((link->inputObject()->type() == ObjectInMappingTypes::AGENT) && (link->outputObject()->type() == ObjectInMappingTypes::AGENT))
        {
            AgentInMappingVM* inputAgent = qobject_cast<AgentInMappingVM*>(link->inputObject());
            AgentInMappingVM* outputAgent = qobject_cast<AgentInMappingVM*>(link->outputObject());

            if ((inputAgent != nullptr) && (outputAgent != nullptr))
            {
                qDebug() << "Remove link between 2 AGENTS";

                // Remove the link between two agents from the mapping
                _removeLinkBetweenTwoAgents(link);
            }
        }
        // 2 actions
        else if ((link->inputObject()->type() == ObjectInMappingTypes::ACTION) && (link->outputObject()->type() == ObjectInMappingTypes::ACTION))
        {
            ActionInMappingVM* inputAction = qobject_cast<ActionInMappingVM*>(link->inputObject());
            ActionInMappingVM* outputAction = qobject_cast<ActionInMappingVM*>(link->outputObject());

            if ((inputAction != nullptr) && (outputAction != nullptr))
            {
                qDebug() << "Remove link between 2 ACTIONS";

                // Delete the link between two agents in the mapping
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }
        // From Action to Agent
        else if ((link->outputObject()->type() == ObjectInMappingTypes::ACTION) && (link->inputObject()->type() == ObjectInMappingTypes::AGENT))
        {
            AgentInMappingVM* inputAgent = qobject_cast<AgentInMappingVM*>(link->inputObject());
            ActionInMappingVM* outputAction = qobject_cast<ActionInMappingVM*>(link->outputObject());

            if ((inputAgent != nullptr) && (outputAction != nullptr))
            {
                qDebug() << "Remove link from ACTION to AGENT";

                // Delete the link between two agents in the mapping
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }
        // From Agent to Action
        else if ((link->outputObject()->type() == ObjectInMappingTypes::AGENT) && (link->inputObject()->type() == ObjectInMappingTypes::ACTION))
        {
            ActionInMappingVM* inputAction = qobject_cast<ActionInMappingVM*>(link->inputObject());
            AgentInMappingVM* outputAgent = qobject_cast<AgentInMappingVM*>(link->outputObject());

            if ((inputAction != nullptr) && (outputAgent != nullptr))
            {
                qDebug() << "Remove link from AGENT to ACTION";

                // Delete the link between two agents in the mapping
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }
    }
}


/**
 * @brief Called when an agent from the list is dropped on the current mapping at a position
 * @param agentName
 * @param position
 */
void AgentsMappingController::dropAgentNameToMappingAtPosition(const QString& agentName, QPointF position)
{
    // Check that there is NOT yet an agent in the current mapping for this name
    AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentName);

    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

    if ((agentInMapping == nullptr) && (ingeScapeModelManager != nullptr))
    {
        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = ingeScapeModelManager->getAgentsGroupedForName(agentName);
        if (agentsGroupedByName != nullptr)
        {
            // Create a new agent in the global mapping (with an "Agents Grouped by Name") at a specific position with the default width
            agentInMapping = _createAgentInMappingAtPosition(agentsGroupedByName, position, AgentInMappingVM::DEFAULT_WIDTH);

            if (agentInMapping != nullptr)
            {
                // The global mapping is activated
                if (IngeScapeNetworkController::instance()->isStarted())
                {
                    // Link the agent in the global mapping on its inputs (add all missing links TO the agent)
                    _linkAgentOnInputs(agentInMapping);
                }

                // Link the agent in the global mapping on its outputs (add all missing links FROM the agent)
                _linkAgentOnOutputs(agentInMapping);

                // Selects this new agent
                setselectedAgent(agentsGroupedByName);
            }
        }
    }
}


/**
 * @brief Called when an action from the list is dropped on the current mapping at a position
 * @param action
 * @param position
 * @return
 */
void AgentsMappingController::dropActionToMappingAtPosition(ActionM* action, QPointF position)
{
    if (action != nullptr)
    {
        qDebug() << "Drop action" << action->name() << "at" << position;

        // Get an UID for our new view model of action in mapping
        QString uid = IngeScapeUtils::getUIDforNewActionInMappingVM();

        // Create a new view model of action in mapping
        ActionInMappingVM* actionInMapping = _createActionInMappingAtPosition(uid, action, position, ActionInMappingVM::DEFAULT_WIDTH);

        if (actionInMapping != nullptr)
        {
            // Selects this new action
            setselectedAction(actionInMapping);
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
void AgentsMappingController::dropLinkBetweenTwoAgents(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput)
{
    if ((outputAgent != nullptr) && (outputAgent->agentsGroupedByName() != nullptr) && (linkOutput != nullptr) && (linkOutput->output() != nullptr)
            && (inputAgent != nullptr) && (inputAgent->agentsGroupedByName() != nullptr) && (linkInput != nullptr) && (linkInput->input() != nullptr))
    {
        // Check that the input can link to the output
        if (linkInput->canLinkWith(linkOutput))
        {
            // Get the link id (with format "outputAgent##output::outputType-->inputAgent##input::inputType") from agent names and Input/Output ids
            QString linkId = LinkVM::getLinkIdFromObjectIdsAndIOids(outputAgent->uid(), linkOutput->uid(), inputAgent->uid(), linkInput->uid());

            LinkVM* link = getLinkInMappingFromId(linkId);

            // Check that the link does NOT exist
            if (link == nullptr)
            {
                // Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
                QString linkName = MappingElementM::getLinkNameFromNamesList(outputAgent->name(), linkOutput->name(), inputAgent->name(), linkInput->name());

                if (inputAgent->agentsGroupedByName()->isON())
                {
                    // The input agent is ON (and so our igs Editor agent is started)

                    // Create a new TEMPORARY link between the two agents
                    link = _createLinkBetweenTwoObjectsInMapping(linkName,
                                                                 outputAgent,
                                                                 linkOutput,
                                                                 inputAgent,
                                                                 linkInput,
                                                                 nullptr,
                                                                 true);

                    if ((link != nullptr) && !_hashFromLinkIdToAddedLink_WaitingReply.contains(link->uid()))
                    {
                        // Insert in the hash table the "(unique) link id" and the link for which we are waiting a reply to the request "add"
                        _hashFromLinkIdToAddedLink_WaitingReply.insert(link->uid(), link);
                    }
                    else {
                        qWarning() << "The request" << command_MapAgents << "has already been sent to add the link" << link->uid();
                    }

                    QStringList message = {
                        command_MapAgents,
                        linkInput->name(),
                        outputAgent->name(),
                        linkOutput->name()
                    };

                    // Send the message "MAP" to the list of agents
                    IngeScapeNetworkController::instance()->sendStringMessageToAgents(inputAgent->agentsGroupedByName()->peerIdsList(), message);
                }
                else
                {
                    // Our igs Editor agent is stopped OR the input agent is OFF

                    // Create a new (REAL) link between the two agents
                    link = _createLinkBetweenTwoObjectsInMapping(linkName,
                                                                 outputAgent,
                                                                 linkOutput,
                                                                 inputAgent,
                                                                 linkInput,
                                                                 nullptr,
                                                                 false);

                    if (link != nullptr)
                    {
                        // Our igs Editor agent is Started OR our agent is off...
                        MappingElementVM* mappingElement = inputAgent->getRemovedMappingElementFromLinkId_WhileAgentWasOFF(linkId);
                        if (mappingElement != nullptr)
                        {
                            // This link has been removed while the agent was OFF : just cancel the remove of the link while the agent is OFF
                            link->setmappingElement(mappingElement); // Update the view model of mapping element which have been cleared
                            inputAgent->cancelRemoveLink_WhileAgentWasOFF(linkId);
                        }
                        else
                        {
                            // Add the link while the agent is OFF
                            inputAgent->addLink_WhileAgentIsOFF(link->uid(), linkInput->name(), outputAgent->name(), linkOutput->name());
                        }
                    }
                }
            }
            else {
                qWarning() << "The link" << link->uid() << "already exist";
            }
        }
        else
        {
            if ((linkOutput->output()->firstModel() != nullptr) && (linkInput->input()->firstModel() != nullptr))
            {
                qWarning() << "Can not link output" << linkOutput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkOutput->output()->firstModel()->agentIOPValueType()) << "(of agent" << outputAgent->name() << ")"
                           << "and input" << linkInput->name() << "with type" << AgentIOPValueTypes::staticEnumToString(linkInput->input()->firstModel()->agentIOPValueType()) << "(of agent" << inputAgent->name() << ")";
            }
        }
    }
}


/**
 * @brief Slot called when a link from an output is dropped over an input on the current mapping
 * Or when a link to an input is dropped over an output
 * @param outputAction
 * @param linkOutput
 * @param inputAction
 * @param linkInput
 */
void AgentsMappingController::dropLinkBetweenTwoActions(ActionInMappingVM* outputAction, LinkOutputVM* linkOutput, ActionInMappingVM* inputAction, LinkInputVM* linkInput)
{
    if ((outputAction != nullptr) && (outputAction->action() != nullptr) && (linkOutput != nullptr)
            && (inputAction != nullptr) && (inputAction->action() != nullptr) && (linkInput != nullptr))
    {
        qDebug() << "drop Link from action" << outputAction->name() << "." << linkOutput->uid() << "to action" << inputAction->name() << "." << linkInput->uid();

        // Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
        QString linkName = MappingElementM::getLinkNameFromNamesList(outputAction->name(), linkOutput->name(), inputAction->name(), linkInput->name());

        // Create a link between two actions in the mapping
        LinkVM* link = _createLinkBetweenTwoObjectsInMapping(linkName,
                                                             outputAction,
                                                             linkOutput,
                                                             inputAction,
                                                             linkInput);

        if (link != nullptr)
        {
            // Connect to the signal "Activate Input of Object in Mapping" from the link
            connect(link, &LinkVM::activateInputOfObjectInMapping, this, &AgentsMappingController::_onActivateInputOfActionInMapping);
        }
    }
}


/**
 * @brief Slot called when a link from an output is dropped over an input on the current mapping
 * Or when a link to an input is dropped over an output
 * @param outputAction
 * @param linkOutput
 * @param inputAgent
 * @param linkInput
 */
void AgentsMappingController::dropLinkFromActionToAgent(ActionInMappingVM* outputAction, LinkOutputVM* linkOutput, AgentInMappingVM* inputAgent, LinkInputVM* linkInput)
{
    if ((outputAction != nullptr) && (outputAction->action() != nullptr) && (linkOutput != nullptr)
            && (inputAgent != nullptr) && (inputAgent->agentsGroupedByName() != nullptr) && (linkInput != nullptr) && (linkInput->input() != nullptr))
    {
        qDebug() << "drop Link from action" << outputAction->name() << "." << linkOutput->uid() << "to agent" << inputAgent->name() << "." << linkInput->uid();

        // Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
        QString linkName = MappingElementM::getLinkNameFromNamesList(outputAction->name(), linkOutput->name(), inputAgent->name(), linkInput->name());

        // Create a link between from action to agent in the mapping
        LinkVM* link = _createLinkBetweenTwoObjectsInMapping(linkName,
                                                             outputAction,
                                                             linkOutput,
                                                             inputAgent,
                                                             linkInput);

        if (link != nullptr)
        {
            // Connect to the signal "Activate Input of Object in Mapping" from the link
            connect(link, &LinkVM::activateInputOfObjectInMapping, this, &AgentsMappingController::_onWriteOnInputOfAgentInMapping);
        }
    }
}


/**
 * @brief Slot called when a link from an output is dropped over an input on the current mapping
 * Or when a link to an input is dropped over an output
 * @param outputAgent
 * @param linkOutput
 * @param inputAction
 * @param linkInput
 */
void AgentsMappingController::dropLinkFromAgentToAction(AgentInMappingVM* outputAgent, LinkOutputVM* linkOutput, ActionInMappingVM* inputAction, LinkInputVM* linkInput)
{
    if ((outputAgent != nullptr) && (outputAgent->agentsGroupedByName() != nullptr) && (linkOutput != nullptr) && (linkOutput->output() != nullptr)
            && (inputAction != nullptr) && (inputAction->action() != nullptr) && (linkInput != nullptr))
    {
        qDebug() << "drop Link from agent" << outputAgent->name() << "." << linkOutput->uid() << "to action" << inputAction->name() << "." << linkInput->uid();

        // Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
        QString linkName = MappingElementM::getLinkNameFromNamesList(outputAgent->name(), linkOutput->name(), inputAction->name(), linkInput->name());

        // Create a link between from agent to action in the mapping
        LinkVM* link = _createLinkBetweenTwoObjectsInMapping(linkName,
                                                             outputAgent,
                                                             linkOutput,
                                                             inputAction,
                                                             linkInput);

        if (link != nullptr)
        {
            // Connect to the signal "Activate Input of Object in Mapping" from the link
            connect(link, &LinkVM::activateInputOfObjectInMapping, this, &AgentsMappingController::_onActivateInputOfActionInMapping);
        }
    }
}


/**
 * @brief Get the (view model of) agent in the global mapping from an agent name
 * @param name
 * @return
 */
AgentInMappingVM* AgentsMappingController::getAgentInMappingFromName(const QString& name)
{
    return _hashFromNameToAgentInMapping.value(name, nullptr);
}


/**
 * @brief Get the (view model of) action in the global mapping from a unique id
 * @param uid
 * @return
 */
ActionInMappingVM* AgentsMappingController::getActionInMappingFromUid(const QString& uid)
{
    return _hashFromUidToActionInMapping.value(uid, nullptr);
}


/**
 * @brief Get the list of (view model of) links between agents in the global mapping from a link name
 * @param linkName
 * @return
 */
QList<LinkVM*> AgentsMappingController::getLinksInMappingFromName(const QString& linkName)
{
    return _hashFromNameToListOfLinksInMapping.value(linkName, QList<LinkVM*>());
}


/**
 * @brief Get the (view model of) link between agents in the global mapping from a link id
 * @param linkId
 * @return
 */
LinkVM* AgentsMappingController::getLinkInMappingFromId(const QString& linkId)
{
    return _hashFromIdToLinkInMapping.value(linkId, nullptr);
}


/**
 * @brief Check if an action has been inserted in the global mapping
 * @param actionM
 * @return
 */
bool AgentsMappingController::isActionInsertedInMapping(ActionM* actionM)
{
    auto isActionM = [actionM](ActionInMappingVM* actionInMapping) {
        return (actionInMapping != nullptr) && (actionInMapping->action() == actionM);
    };

    return (actionM != nullptr) && std::any_of(_allActionsInMapping.begin(), _allActionsInMapping.end(), isActionM);
}


/**
 * @brief Export the global mapping (of agents) into JSON
 * @return array of all agents and their mapping
 */
QJsonArray AgentsMappingController::exportGlobalMappingToJSON()
{
    QJsonArray jsonArray;

    // Copy the list of links
    QList<LinkVM*> linksList = _allLinksInMapping.toList();

    // List of agents and their mapping
    for (AgentInMappingVM* agentInMapping : _allAgentsInMapping)
    {
        if ((agentInMapping != nullptr) && (agentInMapping->agentsGroupedByName() != nullptr)
                && (agentInMapping->agentsGroupedByName()->currentMapping() != nullptr))
        {
            QJsonObject jsonAgent;

            // Set the agent name
            jsonAgent.insert("agentName", agentInMapping->name());

            // Set the position
            QString position = QString("%1, %2").arg(QString::number(agentInMapping->position().x()), QString::number(agentInMapping->position().y()));
            jsonAgent.insert("position", position);

            // Set the width
            jsonAgent.insert("width", agentInMapping->width());


            //
            // Set the mapping
            //
            QJsonObject jsonMapping = QJsonObject();

            if (IngeScapeNetworkController::instance()->isStarted())
            {

            }

            if (agentInMapping->agentsGroupedByName()->isON())
            {
                // The agent is ON (and our igs Editor agent is started) : export the current mapping
                jsonMapping = JsonHelper::exportAgentMappingToJson(agentInMapping->agentsGroupedByName()->currentMapping());
            }
            else
            {
                // Our igs Editor agent is NOT activated OR the agent is OFF
                if (agentInMapping->hadLinksAdded_WhileAgentWasOFF() || agentInMapping->hadLinksRemoved_WhileAgentWasOFF())
                {
                    // Get the list of all added mapping elements while the global mapping was UN-activated
                    QList<MappingElementM*> addedMappingElements = agentInMapping->getAddedMappingElements_WhileAgentWasOFF();

                    // Get the list of all names of removed mapping elements while the global mapping was UN-activated
                    QStringList namesOfRemovedMappingElements = agentInMapping->getNamesOfRemovedMappingElements_WhileAgentWasOFF();

                    // Export the current mapping with changes (applied while the global mapping was UN-activated) into a JSON objec
                    jsonMapping = JsonHelper::exportAgentMappingWithChangesToJson(agentInMapping->agentsGroupedByName()->currentMapping(),
                                                                                  addedMappingElements,
                                                                                  namesOfRemovedMappingElements);
                }
                else
                {
                    // Export the current mapping
                    jsonMapping = JsonHelper::exportAgentMappingToJson(agentInMapping->agentsGroupedByName()->currentMapping());
                }
            }
            jsonAgent.insert("mapping", jsonMapping);


            //
            // Set the actions mapping
            //
            QJsonArray jsonActionsMapping = QJsonArray();

            // Make a copy because the list can be modified inside loop "for"
            QList<LinkVM*> copy = QList<LinkVM*>(linksList);
            for (LinkVM* link : copy)
            {
                if ((link != nullptr) && (link->inputObject() != nullptr) && (link->inputObject() == agentInMapping)
                        && (link->outputObject() != nullptr))
                {
                    QJsonObject jsonMappingElement;

                    jsonMappingElement.insert("input_name", link->linkInput()->name());

                    jsonMappingElement.insert("output_name", link->linkOutput()->name());

                    // From Action to Agent
                    if (link->outputObject()->type() == ObjectInMappingTypes::ACTION)
                    {
                        jsonMappingElement.insert("output_action_name", link->outputObject()->name());

                        ActionInMappingVM* outputAction = qobject_cast<ActionInMappingVM*>(link->outputObject());
                        if ((outputAction != nullptr) && (outputAction->action() != nullptr))
                        {
                            jsonMappingElement.insert("output_actionInMapping_id", outputAction->uid());
                            //jsonMappingElement.insert("output_action_id", outputAction->action()->uid());
                        }
                    }
                    // From Agent to Agent
                    //else if (link->outputObject()->type() == ObjectInMappingTypes::AGENT)
                    //{
                    //}

                    jsonActionsMapping.append(jsonMappingElement);

                    // Remove from the list to optimize next traversals
                    linksList.removeOne(link);
                }
            }

            jsonAgent.insert("actions_mapping", jsonActionsMapping);

            jsonArray.append(jsonAgent);
        }
    }

    // List of actions and their mapping
    for (ActionInMappingVM* actionInMapping : _allActionsInMapping)
    {
        if ((actionInMapping != nullptr) && (actionInMapping->action() != nullptr))
        {
            QJsonObject jsonAction;

            // Set the action name
            jsonAction.insert("action_name", actionInMapping->name());
            //jsonAction.insert("action_name", actionInMapping->action()->name());

            // Set the action id and the unique id of the action in the mapping
            jsonAction.insert("action_id", actionInMapping->action()->uid());
            jsonAction.insert("actionInMapping_id", actionInMapping->uid());

            // Set the position
            QString position = QString("%1, %2").arg(QString::number(actionInMapping->position().x()), QString::number(actionInMapping->position().y()));
            jsonAction.insert("position", position);

            // Set the width
            jsonAction.insert("width", actionInMapping->width());


            //
            // Set the actions mapping
            //
            QJsonArray jsonActionsMapping = QJsonArray();

            // Make a copy because the list can be modified inside loop "for"
            QList<LinkVM*> copy = QList<LinkVM*>(linksList);
            for (LinkVM* link : copy)
            {
                if ((link != nullptr) && (link->inputObject() != nullptr) && (link->inputObject() == actionInMapping)
                        && (link->outputObject() != nullptr))
                {
                    QJsonObject jsonMappingElement;

                    jsonMappingElement.insert("input_name", link->linkInput()->name());

                    jsonMappingElement.insert("output_name", link->linkOutput()->name());

                    // From Agent to Action
                    if (link->outputObject()->type() == ObjectInMappingTypes::AGENT)
                    {
                        jsonMappingElement.insert("output_agent_name", link->outputObject()->name());
                    }
                    // From Action to Action
                    else if (link->outputObject()->type() == ObjectInMappingTypes::ACTION)
                    {
                        jsonMappingElement.insert("output_action_name", link->outputObject()->name());

                        ActionInMappingVM* outputAction = qobject_cast<ActionInMappingVM*>(link->outputObject());
                        if ((outputAction != nullptr) && (outputAction->action() != nullptr))
                        {
                            jsonMappingElement.insert("output_actionInMapping_id", outputAction->uid());
                            //jsonMappingElement.insert("output_action_id", outputAction->action()->uid());
                        }
                    }

                    jsonActionsMapping.append(jsonMappingElement);

                    // Remove from the list to optimize next traversals
                    linksList.removeOne(link);
                }
            }

            jsonAction.insert("actions_mapping", jsonActionsMapping);

            jsonArray.append(jsonAction);
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
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

    if (ingeScapeModelManager != nullptr)
    {
        QList<QPair<AgentInMappingVM*, AgentMappingM*>> listOfAgentsAndMappingToAgents;
        QList<QPair<AgentInMappingVM*, ActionMappingM*>> listOfAgentsAndMappingToActions;
        QList<QPair<ActionInMappingVM*, ActionMappingM*>> listOfActionsAndMappingToAgentsAndActions;

        for (QJsonValue jsonValue : jsonArrayOfAgentsInMapping)
        {
            if (jsonValue.isObject())
            {
                QJsonObject jsonObjectInMapping = jsonValue.toObject();

                //
                // Position
                //
                // Get the value for key "position"
                QJsonValue jsonPosition = jsonObjectInMapping.value("position");

                QPointF position = QPointF();

                if (jsonPosition.isString())
                {
                    QStringList positionStringList = jsonPosition.toString().split(", ");
                    if (positionStringList.count() == 2)
                    {
                        QString strX = positionStringList.at(0);
                        QString strY = positionStringList.at(1);

                        if (!strX.isEmpty() && !strY.isEmpty()) {
                            position = QPointF(static_cast<qreal>(strX.toFloat()), static_cast<qreal>(strY.toFloat()));
                        }
                    }
                }


                //
                // Actions Mapping
                //
                // Get the value for key "actions_mapping"
                QJsonValue jsonActionsMapping = jsonObjectInMapping.value("actions_mapping");


                //
                // Agent
                //
                if (jsonObjectInMapping.contains("agentName"))
                {
                    // Get values for key "agentName" and "mapping"
                    QJsonValue jsonName = jsonObjectInMapping.value("agentName");
                    QJsonValue jsonMapping = jsonObjectInMapping.value("mapping");

                    // Get the value for key "width"
                    QJsonValue jsonWidth = jsonObjectInMapping.value("width");
                    qreal width = jsonWidth.toDouble(AgentInMappingVM::DEFAULT_WIDTH);

                    if (jsonName.isString() && jsonMapping.isObject())
                    {
                        QString agentName = jsonName.toString();

                        // Create the agent mapping from JSON
                        AgentMappingM* agentMapping = JsonHelper::createModelOfAgentMappingFromJSON(agentName, jsonMapping.toObject());


                        //
                        // Actions Mapping
                        //
                        ActionMappingM* actionMapping = nullptr;

                        if (jsonActionsMapping.isArray())
                        {
                            QStringList mappingIdsList_FromActionToAgent = QStringList();

                            // Traverse the list of mapping elements
                            for (QJsonValue jsonIterator : jsonActionsMapping.toArray())
                            {
                                QJsonObject jsonMappingElement = jsonIterator.toObject();

                                // Action
                                if (jsonMappingElement.contains("output_action_name"))
                                {
                                    QJsonValue jsonInputName = jsonMappingElement.value("input_name");

                                    QJsonValue jsonOutputActionName = jsonMappingElement.value("output_action_name");
                                    QJsonValue jsonOutputName = jsonMappingElement.value("output_name");

                                    //QJsonValue jsonOutputActionUID = jsonMappingElement.value("output_action_id");
                                    QJsonValue jsonOutputActionInMappingUID = jsonMappingElement.value("output_actionInMapping_id");

                                    //qDebug() << "Link Action" << jsonOutputActionName.toString() << "(" << jsonOutputActionInMappingUID.toInt() << ")" << "to Agent" << agentName << "." << jsonInputName.toString();

                                    // outputObjectInMapping##output-->inputObjectInMapping##input
                                    QString mappingId_FromActionToAgent = ActionMappingM::getMappingIdFromUIDsList(jsonOutputActionInMappingUID.toString(),
                                                                                                                   jsonOutputName.toString(),
                                                                                                                   agentName,
                                                                                                                   jsonInputName.toString());

                                    mappingIdsList_FromActionToAgent.append(mappingId_FromActionToAgent);
                                }
                            }

                            if (!mappingIdsList_FromActionToAgent.isEmpty())
                            {
                                actionMapping = new ActionMappingM(this);
                                actionMapping->setmappingIdsList_FromActionToAgent(mappingIdsList_FromActionToAgent);
                            }
                        }


                        // Get the (view model of) agents grouped for this name
                        AgentsGroupedByNameVM* agentsGroupedByName = ingeScapeModelManager->getAgentsGroupedForName(agentName);

                        if ((agentsGroupedByName != nullptr) && !position.isNull())
                        {
                            //qDebug() << "Position:" << position.x() << position.y() << "is defined for" << agentName << "with" << agentsGroupedByName->models()->count() << "models";

                            // Create a new agent in the global mapping (with the "Agents Grouped by Name") at a specific position
                            AgentInMappingVM* agentInMapping = _createAgentInMappingAtPosition(agentsGroupedByName, position, width);


                            // If there are some mapping elements, save the pair [agent, its mapping] in the list
                            // We will create the corresponding links when all agents would have been added to the global mapping
                            if ((agentInMapping != nullptr) && (agentMapping != nullptr) && !agentMapping->mappingElements()->isEmpty())
                            {
                                QPair<AgentInMappingVM*, AgentMappingM*> pair = QPair<AgentInMappingVM*, AgentMappingM*>(agentInMapping, agentMapping);

                                listOfAgentsAndMappingToAgents.append(pair);
                            }


                            // If there are some mapping to actions, save the pair [agent, action mapping] in the list
                            // We will create the corresponding links when all agents and all actions would have been added to the global mapping
                            if ((agentInMapping != nullptr) && (actionMapping != nullptr))
                            {
                                QPair<AgentInMappingVM*, ActionMappingM*> pair = QPair<AgentInMappingVM*, ActionMappingM*>(agentInMapping, actionMapping);

                                listOfAgentsAndMappingToActions.append(pair);
                            }
                        }

                    }
                }
                //
                // Action
                //
                else if (jsonObjectInMapping.contains("action_name"))
                {
                    // Get value for key "action_name"
                    QJsonValue jsonActionName = jsonObjectInMapping.value("action_name");

                    QJsonValue jsonActionUID = jsonObjectInMapping.value("action_id");
                    QJsonValue jsonActionInMappingUID = jsonObjectInMapping.value("actionInMapping_id");

                    // Get the value for key "width"
                    QJsonValue jsonWidth = jsonObjectInMapping.value("width");
                    qreal width = jsonWidth.toDouble(ActionInMappingVM::DEFAULT_WIDTH);

                    if (jsonActionName.isString() && jsonActionUID.isDouble() && jsonActionInMappingUID.isString())
                    {
                        QString actionName = jsonActionName.toString();
                        int actionUID = jsonActionUID.toInt();
                        QString actionInMappingUID = jsonActionInMappingUID.toString();

                        //
                        // Actions Mapping
                        //
                        ActionMappingM* actionMapping = nullptr;

                        if (jsonActionsMapping.isArray())
                        {
                            QStringList mappingIdsList_FromAgentToAction = QStringList();
                            QStringList uidsListOfOutputActionsInMapping = QStringList();

                            // Traverse the list of mapping elements
                            for (QJsonValue jsonIterator : jsonActionsMapping.toArray())
                            {
                                QJsonObject jsonMappingElement = jsonIterator.toObject();

                                // Agent
                                if (jsonMappingElement.contains("output_agent_name"))
                                {
                                    QJsonValue jsonOutputAgentName = jsonMappingElement.value("output_agent_name");
                                    QJsonValue jsonOutputName = jsonMappingElement.value("output_name");

                                    QJsonValue jsonInputName = jsonMappingElement.value("input_name");

                                    //qDebug() << "Link Agent" << jsonOutputAgentName.toString() << "." << jsonOutputName.toString() << "to Action" << actionName;

                                    // outputObjectInMapping##output-->inputObjectInMapping##input
                                    QString mappingId_FromAgentToAction = ActionMappingM::getMappingIdFromUIDsList(jsonOutputAgentName.toString(),
                                                                                                                   jsonOutputName.toString(),
                                                                                                                   actionInMappingUID,
                                                                                                                   jsonInputName.toString());

                                    mappingIdsList_FromAgentToAction.append(mappingId_FromAgentToAction);
                                }
                                // Action
                                else if (jsonMappingElement.contains("output_action_name"))
                                {
                                    QJsonValue jsonOutputActionName = jsonMappingElement.value("output_action_name");
                                    //QJsonValue jsonOutputName = jsonMappingElement.value("output_name");

                                    //QJsonValue jsonOutputActionUID = jsonMappingElement.value("output_action_id");
                                    QJsonValue jsonOutputActionInMappingUID = jsonMappingElement.value("output_actionInMapping_id");

                                    //qDebug() << "Link Action" << jsonOutputActionName.toString() << "(" << jsonOutputActionInMappingUID.toInt() << ")" << "to Action" << actionName;

                                    uidsListOfOutputActionsInMapping.append(jsonOutputActionInMappingUID.toString());
                                }
                                else {
                                    qCritical() << "The JSON mapping element for action" << actionName << "is bad formatted !";
                                }
                            }
                            // If at least, one list is not empty
                            if (!mappingIdsList_FromAgentToAction.isEmpty() || !uidsListOfOutputActionsInMapping.isEmpty())
                            {
                                actionMapping = new ActionMappingM(this);
                                actionMapping->setmappingIdsList_FromAgentToAction(mappingIdsList_FromAgentToAction);
                                actionMapping->setuidsListOfOutputActionsInMapping(uidsListOfOutputActionsInMapping);
                            }
                        }

                        // Get the (model of) action for this name
                        ActionM* action = ingeScapeModelManager->getActionWithId(actionUID);

                        if ((action != nullptr) && !position.isNull())
                        {
                            qDebug() << "Position:" << position.x() << position.y() << "is defined for" << actionName << "with" << action;

                            // Get an UID for our new view model of action in mapping
                            IngeScapeUtils::bookUIDforActionInMappingVM(actionInMappingUID);

                            // Create a new action in the global mapping with a unique id, with a model of action and at a specific position
                            ActionInMappingVM* actionInMapping = _createActionInMappingAtPosition(actionInMappingUID, action, position, width);


                            // If there are some mapping to agents and actions, save the pair [action, action mapping] in the list
                            // We will create the corresponding links when all agents and all actions would have been added to the global mapping
                            if ((actionInMapping != nullptr) && (actionMapping != nullptr))
                            {
                                QPair<ActionInMappingVM*, ActionMappingM*> pair = QPair<ActionInMappingVM*, ActionMappingM*>(actionInMapping, actionMapping);

                                listOfActionsAndMappingToAgentsAndActions.append(pair);
                            }
                        }
                    }
                }
            }
        }


        //
        // All agents have been been added to the global mapping, we can create their links...
        //
        for (QPair<AgentInMappingVM*, AgentMappingM*> pair : listOfAgentsAndMappingToAgents)
        {
            AgentInMappingVM* inputAgent = pair.first;
            AgentMappingM* agentMapping = pair.second;

            if ((agentMapping != nullptr) && (inputAgent != nullptr))
            {
                for (MappingElementM* mappingElement : agentMapping->mappingElements()->toList())
                {
                    if (mappingElement != nullptr)
                    {
                        // Get the output agent in the global mapping from the output agent name
                        AgentInMappingVM* outputAgent = getAgentInMappingFromName(mappingElement->outputAgent());
                        if (outputAgent != nullptr)
                        {
                            // Get the link input and the link output
                            LinkInputVM* linkInput = _getAloneLinkInputFromName(inputAgent, mappingElement->input(), mappingElement->name());
                            LinkOutputVM* linkOutput = _getAloneLinkOutputFromName(outputAgent, mappingElement->output(), mappingElement->name());

                            if ((linkInput != nullptr) && (linkOutput != nullptr))
                            {
                                // Simulate a drop of the link between the two agents
                                dropLinkBetweenTwoAgents(outputAgent, linkOutput, inputAgent, linkInput);
                            }
                        }
                    }
                }
            }
        }


        //
        // All agents and all actions have been been added to the global mapping, we can create their links...
        //
        for (QPair<AgentInMappingVM*, ActionMappingM*> pair : listOfAgentsAndMappingToActions)
        {
            AgentInMappingVM* inputAgent = pair.first;
            ActionMappingM* actionMapping = pair.second;

            if ((actionMapping != nullptr) && (inputAgent != nullptr))
            {
                for (QString mappingId_FromActionToAgent : actionMapping->mappingIdsList_FromActionToAgent())
                {
                    // Get the list of UIDs (each parts of a mapping) from the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
                    QStringList uidsList = ActionMappingM::getUIDsListFromMappingId(mappingId_FromActionToAgent);
                    if (uidsList.count() == 4)
                    {
                        QString outputObjectInMappingUID = uidsList.at(0);
                        QString outputUID = uidsList.at(1);
                        QString inputObjectInMappingUID = uidsList.at(2);
                        QString inputUID = uidsList.at(3);

                        // Get the (view model of) action in the global mapping from this unique id
                        ActionInMappingVM* outputAction = getActionInMappingFromUid(outputObjectInMappingUID);

                        // Get the link input
                        LinkInputVM* linkInput = _getAloneLinkInputFromName(inputAgent, inputUID, mappingId_FromActionToAgent);

                        if ((outputAction != nullptr) && (outputAction->linkOutput() != nullptr) && (linkInput != nullptr))
                        {
                            // Simulate a drop of a link between these 2 actions
                            dropLinkFromActionToAgent(outputAction, outputAction->linkOutput(), inputAgent, linkInput);
                        }
                    }
                }
            }

            // Free memory
            delete actionMapping;
        }
        listOfAgentsAndMappingToActions.clear();


        //
        // All agents and all actions have been been added to the global mapping, we can create their links...
        //
        for (QPair<ActionInMappingVM*, ActionMappingM*> pair : listOfActionsAndMappingToAgentsAndActions)
        {
            ActionInMappingVM* inputAction = pair.first;
            ActionMappingM* actionMapping = pair.second;

            if ((actionMapping != nullptr) && (inputAction != nullptr) && (inputAction->linkInput() != nullptr))
            {
                for (QString mappingId_FromAgentToAction : actionMapping->mappingIdsList_FromAgentToAction())
                {
                    // Get the list of names (each parts of a mapping) from the mapping id (with format "outputObjectInMapping##output-->inputObjectInMapping##input")
                    QStringList uidsList = ActionMappingM::getUIDsListFromMappingId(mappingId_FromAgentToAction);
                    if (uidsList.count() == 4)
                    {
                        QString outputObjectInMappingUID = uidsList.at(0);
                        QString outputUID = uidsList.at(1);
                        QString inputObjectInMappingUID = uidsList.at(2);
                        QString inputUID = uidsList.at(3);

                        // Get the output agent in the global mapping from the output agent name
                        AgentInMappingVM* outputAgent = getAgentInMappingFromName(outputObjectInMappingUID);
                        if (outputAgent != nullptr)
                        {
                            // Get the link output
                            LinkOutputVM* linkOutput = _getAloneLinkOutputFromName(outputAgent, outputUID, mappingId_FromAgentToAction);
                            if (linkOutput != nullptr)
                            {
                                // Simulate a drop of the link from the agent to the action
                                dropLinkFromAgentToAction(outputAgent, linkOutput, inputAction, inputAction->linkInput());
                            }
                        }
                    }
                }

                for (QString uidOfOutputActionInMapping : actionMapping->uidsListOfOutputActionsInMapping())
                {
                    ActionInMappingVM* outputAction = getActionInMappingFromUid(uidOfOutputActionInMapping);
                    if ((outputAction != nullptr) && (outputAction->linkOutput() != nullptr))
                    {
                        // Simulate a drop of a link between these 2 actions
                        dropLinkBetweenTwoActions(outputAction, outputAction->linkOutput(), inputAction, inputAction->linkInput());
                    }
                }
            }

            // Free memory
            delete actionMapping;
        }
        listOfActionsAndMappingToAgentsAndActions.clear();
    }
}


/**
 * @brief Slot called when a new view model of agents grouped by name has been created
 * @param agentsGroupedByName
 */
void AgentsMappingController::onAgentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName)
{
    if (agentsGroupedByName != nullptr)
    {
        // Connect to signals from this new agents grouped by name
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::isONChanged, this, &AgentsMappingController::_onAgentIsONChanged);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::agentModelONhasBeenAdded, this, &AgentsMappingController::_onAgentModelONhasBeenAdded);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::mappingElementsHaveBeenAdded, this, &AgentsMappingController::_onMappingElementsHaveBeenAdded);
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::mappingElementsWillBeRemoved, this, &AgentsMappingController::_onMappingElementsWillBeRemoved);

        // Get a random position in the current window
        double randomMax = static_cast<double>(RAND_MAX);
        QPointF position = _getRandomPosition(randomMax);
        qreal width = AgentInMappingVM::DEFAULT_WIDTH;

        // Add new agent in our mapping
        _createAgentInMappingAtPosition(agentsGroupedByName, position, width);
        Q_EMIT fitToView(); // Notify the QML to fit the view
    }
}


/**
 * @brief Slot called when a view model of agents grouped by name will be deleted
 * @param agentsGroupedByName
 */
void AgentsMappingController::onAgentsGroupedByNameWillBeDeleted(AgentsGroupedByNameVM* agentsGroupedByName)
{
    if (agentsGroupedByName != nullptr)
    {
        // DIS-connect to signals from this old agents grouped by name
        disconnect(agentsGroupedByName, nullptr, this, nullptr);

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
 * @brief Slot called when a model of action will be deleted
 * @param action
 */
void AgentsMappingController::onActionModelWillBeDeleted(ActionM* action)
{
    if (action != nullptr)
    {
        // Make a copy of the list
        for (ActionInMappingVM* actionInMapping : _allActionsInMapping.toList())
        {
            if ((actionInMapping != nullptr) && (actionInMapping->action() == action))
            {
                // Remove the action from the mapping and delete the view model
                deleteActionInMapping(actionInMapping);
            }
        }
    }
}


/**
 * @brief Slot called when we receive the command "highlight link" from a recorder
 * @param parameters
 */
void AgentsMappingController::onHighlightLink(const QStringList& parameters)
{
    if (parameters.count() == 4)
    {
        QString inputAgentName = parameters.at(0);
        QString inputName = parameters.at(1);
        QString outputAgentName = parameters.at(2);
        QString outputName = parameters.at(3);

        // Get the link name (with format "outputAgent##output-->inputAgent##input") from the list of names (each parts of a mapping element)
        QString linkName = MappingElementM::getLinkNameFromNamesList(outputAgentName, outputName, inputAgentName, inputName);

        // Get the list of links between agents in the global mapping from a link name
        QList<LinkVM*> linksWithSameName = getLinksInMappingFromName(linkName);

        if (linksWithSameName.count() == 1)
        {
            LinkVM* link = linksWithSameName.at(0);
            if ((link != nullptr) && (link->linkOutput() != nullptr))
            {
                qDebug() << "Highlight the link" << linkName;

                // Activate the link: allows to highlight the corresponding links
                link->linkOutput()->activate();
            }
        }
    }
}


/**
 * @brief Slot called when the list of all "Agents in Mapping" changed
 */
void AgentsMappingController::_onAllAgentsInMappingChanged()
{
    // Update the flag "is Empty Mapping"
    setisEmptyMapping(_allAgentsInMapping.isEmpty());
}


/**
 * @brief Slot called when the flag "is ON" of an agent(s grouped by name) changed (only happens when we are online)
 * It means that our agent was ALREADY in our platform
 */
void AgentsMappingController::_onAgentIsONChanged(bool isON)
{
    if (isON && (_modelManager != nullptr))
    {
        AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
        AgentInMappingVM* agentInMapping = getAgentInMappingFromName(agentsGroupedByName->name());
        if (agentInMapping != nullptr)
        {
            if (_modelManager->isMappingControlled())
            {
                // We IMPOSE the mapping of agent that connect on our network
                // Send the message "LOAD THIS MAPPING" to this agent with our current mapping
                QString jsonOfMapping = _getJSONofMappingOfAgentInGlobalMapping(agentInMapping);
                QString message = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);
                IngeScapeNetworkController::instance()->sendStringMessageToAgents(agentsGroupedByName->peerIdsList(), message); // FIXME: JSON can be too big for a string
            }
            else
            {
                // We KEEP the mapping of agent that connect on our network
                // We cancel all added and removed link while agent was OFF
                if (agentInMapping->hadLinksAdded_WhileAgentWasOFF())
                {
                    // Get the list of all added link Ids while the agent was OFF
                    QList<QString> addedLinkIds = agentInMapping->getAddedLinkIds_WhileAgentWasOFF();
                    for (QString linkId : addedLinkIds)
                    {
                        qDebug() << "Remove added link" << linkId << "while the agent was OFF";
                        LinkVM* link = getLinkInMappingFromId(linkId);
                        if (link != nullptr)
                        {
                            _deleteLinkBetweenTwoObjectsInMapping(link);
                        }
                    }
                    agentInMapping->cancelAllAddedLinks_WhileAgentWasOFF();
                }

                if (agentInMapping->hadLinksRemoved_WhileAgentWasOFF())
                {
                    // Get the list of all (view models of) removed mapping elements while the global mapping was UN-activated
                    QList<MappingElementVM*> removedMappingElements = agentInMapping->getRemovedMappingElements_WhileAgentWasOFF();
                    for (MappingElementVM* mappingElement : removedMappingElements)
                    {
                        if (mappingElement != nullptr)
                        {
                            qDebug() << "Add removed link" << mappingElement->name() << "while the agent was OFF";
                            _linkAgentOnInputFromMappingElement(agentInMapping, mappingElement);
                        }
                    }
                    agentInMapping->cancelAllRemovedLinks_WhileAgentWasOFF();
                }
            }
        }
    }
}


/**
 * @brief Slot called when a model of agent "ON" has been added to an agent(s grouped by name) (only happens when we are online)
 * It means that our agent was NOT in our platform
 */
void AgentsMappingController::_onAgentModelONhasBeenAdded(AgentM* model)
{
    // Model of Agent ON
    if ((model != nullptr) && model->isON() && !model->name().isEmpty() && !model->peerId().isEmpty() && (_modelManager != nullptr))
    {
        if (_modelManager->isMappingControlled())
        {
            // We IMPOSE the mapping of agent that connect on our network
            // Send the message "CLEAR MAPPING" to this agent
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(model->peerId(), command_ClearMapping);
        }
        else
        {
            QString agentName = model->name();
            AgentsGroupedByNameVM* agentsGroupedByName = IngeScapeModelManager::instance()->getAgentsGroupedForName(agentName);
            if (agentsGroupedByName != nullptr)
            {
                // Get a random position in the current window
                double randomMax = static_cast<double>(RAND_MAX);
                QPointF position = _getRandomPosition(randomMax);
                qreal width = AgentInMappingVM::DEFAULT_WIDTH;

                // Add new agent in our mapping & link the agent in the global mapping
                // on its inputs (add all missing links TO the agent) and on its outputs (add all missing links FROM the agent)
                AgentInMappingVM* agentInMapping = _createAgentInMappingAtPosition(agentsGroupedByName, position, width);
                if (agentInMapping != nullptr)
                {
                    _linkAgentOnInputs(agentInMapping);
                    _linkAgentOnOutputs(agentInMapping);
                }
                Q_EMIT fitToView(); // Notify the QML to fit the view
            }
        }
    }
}


/**
 * @brief Slot called when some view models of mapping elements have been added to an agent(s grouped by name)
 * @param newMappingElements
 */
void AgentsMappingController::_onMappingElementsHaveBeenAdded(QList<MappingElementVM*> newMappingElements)
{
    AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty() && !newMappingElements.isEmpty())
    {
        // Get the input agent in the global mapping from the agent name (outside loop "for")
        AgentInMappingVM* inputAgent = getAgentInMappingFromName(agentsGroupedByName->name());
        if (inputAgent != nullptr)
        {
            for (MappingElementVM* mappingElement : newMappingElements)
            {
                if ((mappingElement != nullptr) && (mappingElement->firstModel() != nullptr))
                {
                    qInfo() << mappingElement->name() << "MAPPED";

                    if (IngeScapeNetworkController::instance()->isStarted()) // TODO VERIFY !
                    {
                        // Our agent Editor is started
                        // Link the agent on its input from the mapping element (add a missing link TO the agent)
                        _linkAgentOnInputFromMappingElement(inputAgent, mappingElement);
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot called when some view models of mapping elements will be removed from an agent(s grouped by name)
 * @param oldMappingElements
 */
void AgentsMappingController::_onMappingElementsWillBeRemoved(QList<MappingElementVM*> oldMappingElements)
{
    //AgentsGroupedByNameVM* agentsGroupedByName = qobject_cast<AgentsGroupedByNameVM*>(sender());
    //if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty() && !oldMappingElements.isEmpty())
    if (!oldMappingElements.isEmpty())
    {
        for (MappingElementVM* mappingElement : oldMappingElements)
        {
            // firstModel is NULL, we cannot use it
            //if ((mappingElement != nullptr) && !mappingElement->name().isEmpty() && (mappingElement->firstModel() != nullptr))
            if ((mappingElement != nullptr) && !mappingElement->name().isEmpty())
            {
                QString linkName = mappingElement->name();

                //qDebug() << "on Mapping Elements will be Removed" << linkName;
                qInfo() << linkName << "UN-mapped";

                // Get the list of links between agents in the global mapping from a link name
                QList<LinkVM*> linksWithSameName = getLinksInMappingFromName(linkName);

                // The link is NOT in the global mapping
                if (linksWithSameName.isEmpty())
                {
                    // Get the list of names (each parts of a mapping element) from the link name (with format "outputAgent##output-->inputAgent##input")
                    QStringList namesList = MappingElementM::getNamesListFromLinkName(linkName);
                    if (namesList.count() == 4)
                    {
                        QString outputAgentName = namesList.at(0);
                        //QString outputName = namesList.at(1);
                        QString inputAgentName = namesList.at(2);
                        //QString inputName = namesList.at(3);

                        AgentInMappingVM* inputAgent = getAgentInMappingFromName(inputAgentName);
                        if (inputAgent != nullptr)
                        {
                            // Get the list of all names of removed mapping elements while the global mapping was UN-activated
                            QStringList namesOfRemovedMappingElements = inputAgent->getNamesOfRemovedMappingElements_WhileAgentWasOFF();
                            if (namesOfRemovedMappingElements.contains(linkName))
                            {
                                // The link has been removed when our agent was OFF
                                qDebug() << "There is still the corresponding removed Mapping Element" << linkName << "while the Mapping was UN-activated";
                                inputAgent->mappingElementRemoved_CorrespondingLinkRemovedWhileAgentWasOFF(mappingElement);
                            }
                        }

                        // Remove the "Waiting Mapping Element" on the output agent name
                        _removeWaitingMappingElementOnOutputAgent(outputAgentName, mappingElement);
                    }

                }
                // The link is in the global mapping
                else if (linksWithSameName.count() == 1)
                {
                    LinkVM* link = linksWithSameName.at(0);
                    if (link != nullptr)
                    {
                        if (IngeScapeNetworkController::instance()->isStarted())
                        {
                            // Our agent Editor is started
                            if (link->isTemporary())
                            {
                                qDebug() << "The link" << link->uid() << "from TEMPORARY to DELETED";

                                // Reset the flag
                                //link->setisTemporary(false);

                                if (_hashFromLinkIdToRemovedLink_WaitingReply.contains(link->uid()))
                                {
                                    // Remove from the hash table the "(unique) link id" and the link for which we are waiting a reply to the request "remove"
                                    _hashFromLinkIdToRemovedLink_WaitingReply.remove(link->uid());
                                }
                                else {
                                    qCritical() << "The 'Temporary' link" << link->uid() << "was not in the hash table 'Removed Link Waiting Reply'";
                                }
                            }

                            // Delete the link between two agents in the mapping
                            _deleteLinkBetweenTwoObjectsInMapping(link);
                        }
                    }
                }
                // There are several links with this name in the global mapping
                else {
                    qWarning() << "There are" << linksWithSameName.count() << "links with the same name" << linkName << ". We cannot choose and remove the link";
                }
            }
        }
    }
}


/**
 * @brief Slot called when some view models of link inputs have been added to an agent in mapping
 * @param addedlinkInputs
 */
void AgentsMappingController::_onLinkInputsListHaveBeenAdded(const QList<LinkInputVM*>& addedlinkInputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !addedlinkInputs.isEmpty())
    {
        // Link the agent in the global mapping on its inputs (add all missing links TO the agent)
        _linkAgentOnInputs(agentInMapping);
    }
}


/**
 * @brief Slot called when some view models of link outputs have been added to an agent in mapping
 * @param addedlinkOutputs
 */
void AgentsMappingController::_onLinkOutputsListHaveBeenAdded(const QList<LinkOutputVM*>& addedlinkOutputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !addedlinkOutputs.isEmpty())
    {
        // Link the agent in the global mapping on its outputs (add all missing links FROM the agent)
        _linkAgentOnOutputs(agentInMapping);
    }
}


/**
 * @brief Slot called when some view models of link inputs will be removed from an agent in mapping
 * @param removedLinkInputs
 */
void AgentsMappingController::_onLinkInputsListWillBeRemoved(const QList<LinkInputVM*>& removedLinkInputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !removedLinkInputs.isEmpty())
    {
        // Traverse the list of all links
        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->inputObject() != nullptr) && (link->linkInput() != nullptr))
            {
                AgentInMappingVM* inputAgent = qobject_cast<AgentInMappingVM*>(link->inputObject());
                if ((inputAgent != nullptr) && (inputAgent == agentInMapping) && removedLinkInputs.contains(link->linkInput()))
                {
                    // Delete the link between two agents in the mapping
                    _deleteLinkBetweenTwoObjectsInMapping(link);
                }
            }
        }
    }
}


/**
 * @brief Slot called when some view models of link outputs will be removed from an agent in mapping
 * @param removedLinkOutputs
 */
void AgentsMappingController::_onLinkOutputsListWillBeRemoved(const QList<LinkOutputVM*>& removedLinkOutputs)
{
    AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(sender());
    if ((agentInMapping != nullptr) && !removedLinkOutputs.isEmpty())
    {
        qDebug() << "_on Link Outputs List will be Removed from agent" << agentInMapping->name() << removedLinkOutputs.count();

        // Traverse the list of all links
        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ((link != nullptr) && (link->outputObject() != nullptr) && (link->linkOutput() != nullptr))
            {
                AgentInMappingVM* outputAgent = qobject_cast<AgentInMappingVM*>(link->outputObject());
                if ((outputAgent != nullptr) && (outputAgent == agentInMapping) && removedLinkOutputs.contains(link->linkOutput()))
                {
                    if (link->mappingElement() != nullptr)
                    {
                        // Add a "Waiting Mapping Element" on the output agent (name)
                        _addWaitingMappingElementOnOutputAgent(link->outputObject()->name(), link->mappingElement());
                    }

                    // Delete the link between two agents in the mapping
                    _deleteLinkBetweenTwoObjectsInMapping(link);
                }
            }
        }
    }
}


/**
 * @brief Slot called when an output has been activated, so we have to activate an input (of an action in the global mapping)
 * @param inputObject
 * @param linkInput
 */
void AgentsMappingController::_onActivateInputOfActionInMapping(ObjectInMappingVM* inputObject, LinkInputVM* linkInput)
{
    Q_UNUSED(linkInput)

    if (inputObject != nullptr)
    {
        ActionInMappingVM* actionInMapping = qobject_cast<ActionInMappingVM*>(inputObject);
        if ((actionInMapping != nullptr) && (actionInMapping->action() != nullptr))
        {
            qDebug() << "Execute (effects of) Action in Mapping" << actionInMapping->action()->name();

            // Emit the signal "Execute Action"
            Q_EMIT executeAction(actionInMapping->action());
        }
    }
}


/**
 * @brief Slot called when an output has been activated, so we have to write on an input (of an input agent in the global mapping)
 * @param inputObject
 * @param linkInput
 */
void AgentsMappingController::_onWriteOnInputOfAgentInMapping(ObjectInMappingVM* inputObject, LinkInputVM* linkInput)
{
    if ((inputObject != nullptr) && (linkInput != nullptr) && (linkInput->input() != nullptr))
    {
        AgentInMappingVM* agentInMapping = qobject_cast<AgentInMappingVM*>(inputObject);
        if ((agentInMapping != nullptr) && (agentInMapping->agentsGroupedByName() != nullptr))
        {
            qDebug() << "Write on Input" << linkInput->input()->name() << "of Agent in Mapping" << agentInMapping->agentsGroupedByName()->name();

            QStringList message = {
                "SET_INPUT",
                linkInput->input()->name(),
                "0"
            };

            // Send the message "SET INPUT" to the list of agents
            IngeScapeNetworkController::instance()->sendStringMessageToAgents(agentInMapping->agentsGroupedByName()->peerIdsList(), message);
        }
    }
}


/**
 * @brief Create a new agent in the global mapping with an "Agents Grouped by Name" and at a specific position
 */
AgentInMappingVM* AgentsMappingController::_createAgentInMappingAtPosition(AgentsGroupedByNameVM* agentsGroupedByName, QPointF position, qreal width)
{
    AgentInMappingVM* agentInMapping = nullptr;
    if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty())
    {
        // Check that there is NOT yet an agent in the global mapping for this name
        agentInMapping = getAgentInMappingFromName(agentsGroupedByName->name());
        if (agentInMapping == nullptr)
        {
            // Create a new view model of agent in the global Mapping
            agentInMapping = new AgentInMappingVM(agentsGroupedByName, position, width, this);

            // Connect to signals from this new agent in mapping
            connect(agentInMapping, &AgentInMappingVM::linkInputsListHaveBeenAdded, this, &AgentsMappingController::_onLinkInputsListHaveBeenAdded);
            connect(agentInMapping, &AgentInMappingVM::linkOutputsListHaveBeenAdded, this, &AgentsMappingController::_onLinkOutputsListHaveBeenAdded);
            connect(agentInMapping, &AgentInMappingVM::linkInputsListWillBeRemoved, this, &AgentsMappingController::_onLinkInputsListWillBeRemoved);
            connect(agentInMapping, &AgentInMappingVM::linkOutputsListWillBeRemoved, this, &AgentsMappingController::_onLinkOutputsListWillBeRemoved);

            _hashFromNameToAgentInMapping.insert(agentsGroupedByName->name(), agentInMapping);
            _allAgentsInMapping.append(agentInMapping); // for QML
        }
        else {
            qCritical() << "The agent" << agentsGroupedByName->name() << "is already in the global mapping";
        }
    }
    return agentInMapping;
}


/**
 * @brief Create a new action in the global mapping with a unique id, with a model of action and at a specific position
 */
ActionInMappingVM* AgentsMappingController::_createActionInMappingAtPosition(QString uid, ActionM* action, QPointF position, qreal width)
{
    ActionInMappingVM* actionInMapping = nullptr;
    if (action != nullptr)
    {
        // Check that there is NOT yet an action in the global mapping for this uid
        actionInMapping = getActionInMappingFromUid(uid);
        if (actionInMapping == nullptr)
        {
            // Create a new view model of action in mapping
            actionInMapping = new ActionInMappingVM(uid, action, position, width);


            _hashFromUidToActionInMapping.insert(actionInMapping->uid(), actionInMapping);
            _allActionsInMapping.append(actionInMapping); // for QML
        }
        else {
            qCritical() << "The action" << action->name() << "with uid" << uid << "is already in the global mapping";
        }
    }
    return actionInMapping;
}


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
LinkVM* AgentsMappingController::_createLinkBetweenTwoObjectsInMapping(const QString& linkName,
                                                                       ObjectInMappingVM* outputObject,
                                                                       LinkOutputVM* linkOutput,
                                                                       ObjectInMappingVM* inputObject,
                                                                       LinkInputVM* linkInput,
                                                                       MappingElementVM* mappingElement,
                                                                       bool isTemporary)
{
    LinkVM* link = nullptr;

    if ((outputObject != nullptr) && (linkOutput != nullptr)
            && (inputObject != nullptr) && (linkInput != nullptr))
    {
        // Get the link id (with format "outputObject##output::outputType-->inputObject##input::inputType") from object names and Input/Output ids
        QString linkId = LinkVM::getLinkIdFromObjectIdsAndIOids(outputObject->uid(), linkOutput->uid(), inputObject->uid(), linkInput->uid());

        link = getLinkInMappingFromId(linkId);

        // Check that there is NOT yet a link in the global mapping for this id
        if (link == nullptr)
        {
            // Create a new link between two agents
            link = new LinkVM(linkName,
                              mappingElement,
                              outputObject,
                              linkOutput,
                              inputObject,
                              linkInput,
                              isTemporary,
                              this);

            // Add to the list to update the view (QML)
            _allLinksInMapping.append(link);

            // Add to the hash table
            _hashFromIdToLinkInMapping.insert(link->uid(), link);

            // Update the list of links between agents in the global mapping for this link name
            QList<LinkVM*> linksWithSameName = getLinksInMappingFromName(linkName);
            linksWithSameName.append(link);
            _hashFromNameToListOfLinksInMapping.insert(linkName, linksWithSameName);
        }
        else {
            qWarning() << "The link" << linkId << "is already in the global mapping !";
        }
    }

    return link;
}


/**
 * @brief Remove a link between two agents from the mapping
 * @param link
 */
void AgentsMappingController::_removeLinkBetweenTwoAgents(LinkVM* link)
{
    if ((link != nullptr)
            && (link->inputObject() != nullptr) && (link->linkInput() != nullptr)
            && (link->outputObject() != nullptr) && (link->linkOutput() != nullptr))
    {
        AgentInMappingVM* inputAgent = qobject_cast<AgentInMappingVM*>(link->inputObject());

        if ((inputAgent != nullptr) && (inputAgent->agentsGroupedByName() != nullptr))
        {
            qInfo() << "Remove the link between agents" << link->outputObject()->name() << "and" << link->inputObject()->name();

            if (inputAgent->agentsGroupedByName()->isON())
            {
                // The input agent is ON (and our igs Editor agent is started)
                if (!_hashFromLinkIdToRemovedLink_WaitingReply.contains(link->uid()))
                {
                    // Insert in the hash table the "(unique) link id" and the link for which we are waiting a reply to the request "remove"
                    _hashFromLinkIdToRemovedLink_WaitingReply.insert(link->uid(), link);

                    // Update the flag to give a feedback to the user
                    link->setisTemporary(true);
                }
                else {
                    qWarning() << "The request" << command_UnmapAgents << "has already been sent to remove the link" << link->uid();
                }

                QStringList message = {
                    command_UnmapAgents,
                    link->linkInput()->name(),
                    link->outputObject()->name(),
                    link->linkOutput()->name()
                };

                // Send the message "UNMAP" to the list of agents
                IngeScapeNetworkController::instance()->sendStringMessageToAgents(inputAgent->agentsGroupedByName()->peerIdsList(), message);
            }
            else
            {
                // Our igs Editor agent is Started OR our agent is off...
                MappingElementM* mappingElement = inputAgent->getAddedMappingElementFromLinkId_WhileAgentWasOFF(link->uid());
                if (mappingElement != nullptr)
                {
                    // This link has been added while the agent was off : just cancel the add of the link while the the agent is OFF
                    inputAgent->cancelAddLink_WhileAgentIsOFF(link->uid());
                }
                else
                {
                    inputAgent->removeLink_WhileAgentIsOFF(link->uid(), link->mappingElement());
                }
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }
    }
}


/**
 * @brief Delete a link between two objects in the mapping
 * @param link
 */
void AgentsMappingController::_deleteLinkBetweenTwoObjectsInMapping(LinkVM* link)
{
    if (link != nullptr)
    {
        qInfo() << "Delete the link" << link->name() << "in the Mapping";

        // Unselect our link if needed
        if (_selectedLink == link) {
            setselectedLink(nullptr);
        }

        // DIS-connect to signals from the link
        disconnect(link, nullptr, this, nullptr);

        // Remove from the hash table
        _hashFromIdToLinkInMapping.remove(link->uid());

        // Update the list of links between agents in the global mapping for this link name
        QList<LinkVM*> linksWithSameName = getLinksInMappingFromName(link->name());
        linksWithSameName.removeOne(link);
        _hashFromNameToListOfLinksInMapping.insert(link->name(), linksWithSameName);

        // Remove from the list to update the view (QML)
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
    if ((agent != nullptr) && (agent->agentsGroupedByName() != nullptr))
    {
        qDebug() << "Remove all Links with agent" << agent->name();

        for (LinkVM* link : _allLinksInMapping.toList())
        {
            if ( (link != nullptr) && ((link->outputObject() == agent) || (link->inputObject() == agent)) )
            {
                if (link->outputObject() == agent)
                {
                    // Add a "Waiting Mapping Element" on the output agent (name)
                   _addWaitingMappingElementOnOutputAgent(link->outputObject()->name(), link->mappingElement());
                }

                // Delete the link between two objects in the mapping
                _deleteLinkBetweenTwoObjectsInMapping(link);
            }
        }

        // Our igs Editor agent is started AND the agent is ON
        if (agent->agentsGroupedByName()->isON())
        {
            // Send the message "Clear Mapping" to these agents
            IngeScapeNetworkController::instance()->sendStringMessageToAgents(agent->agentsGroupedByName()->peerIdsList(), command_ClearMapping);
        }
    }
}


/**
 * @brief Link an agent (in the global mapping) on its inputs (add all missing links TO an agent)
 */
void AgentsMappingController::_linkAgentOnInputs(AgentInMappingVM* agentInMapping)
{
    if ((agentInMapping != nullptr) && !agentInMapping->name().isEmpty()
            && (agentInMapping->agentsGroupedByName() != nullptr))
    {
        // Traverse the list of all mapping mapping elements which this agent is the input agent
        for (MappingElementVM* mappingElement : agentInMapping->agentsGroupedByName()->allMappingElements()->toList())
        {
            if (mappingElement != nullptr)
            {
                // Link the agent on its input from the mapping element (add a missing link TO the agent)
                _linkAgentOnInputFromMappingElement(agentInMapping, mappingElement);
            }
        }
    }
}


/**
 * @brief Link an agent (in the global mapping) on its input from a mapping element (add a missing link TO an agent)
 */
void AgentsMappingController::_linkAgentOnInputFromMappingElement(AgentInMappingVM* inputAgent, MappingElementVM* mappingElement)
{
    if ((inputAgent != nullptr) && !inputAgent->name().isEmpty()
            && (mappingElement != nullptr) && (mappingElement->firstModel() != nullptr) && !mappingElement->name().isEmpty()
            && (inputAgent->name() == mappingElement->firstModel()->inputAgent()))
    {
        QString linkName = mappingElement->name();

        // Get the list of links between agents in the global mapping from a link name
        QList<LinkVM*> linksWithSameName = getLinksInMappingFromName(linkName);

        if (linksWithSameName.isEmpty())
        {
            // The link is NOT in the global mapping
            //qDebug() << "Try to create link" << linkName << "TO agent" << inputAgent->name() << "involved as 'INPUT' agent";

            // Get the (view model of) agent in the global mapping from the output agent name
            AgentInMappingVM* outputAgent = getAgentInMappingFromName(mappingElement->firstModel()->outputAgent());
            if (outputAgent == nullptr)
            {
                // The Output agent is NOT in the mapping
                _addWaitingMappingElementOnOutputAgent(mappingElement->firstModel()->outputAgent(), mappingElement);
            }
            else
            {
                // The Output agent is in the mapping
                LinkOutputVM* linkOutput = nullptr;

                // Get the list of view models of link output from the output name
                QList<LinkOutputVM*> linkOutputsWithSameName = outputAgent->getLinkOutputsListFromName(mappingElement->firstModel()->output());


                if (linkOutputsWithSameName.isEmpty())
                {
                    // The output does NOT exist
                    _addWaitingMappingElementOnOutputAgent(mappingElement->firstModel()->outputAgent(), mappingElement);
                }
                else if (linkOutputsWithSameName.count() == 1)
                {
                    // The output exist
                    linkOutput = linkOutputsWithSameName.at(0);
                    if (linkOutput != nullptr)
                    {
                        // Get the link input from a name if there is only one input for this name
                        LinkInputVM* linkInput = _getAloneLinkInputFromName(inputAgent, mappingElement->firstModel()->input(), linkName);
                        if (linkInput != nullptr)
                        {
                            // Create a new REAL link between the two agents
                            _createLinkBetweenTwoObjectsInMapping(linkName, outputAgent, linkOutput, inputAgent, linkInput, mappingElement);

                            // Remove eventually the corresponding "Waiting Mapping Element" on the output agent name
                            _removeWaitingMappingElementOnOutputAgent(outputAgent->name(), mappingElement);
                        }
                    }
                }
                else {
                    // There are several outputs with the same name
                    qWarning() << "There are" << linkOutputsWithSameName.count() << "link outputs with the same name" << mappingElement->firstModel()->output() << "."
                               << "We cannot choose and create the link" << linkName;
                }
            }
        }
        else if (linksWithSameName.count() == 1)
        {
            // The link is in the global mapping
            LinkVM* link = linksWithSameName.at(0);
            if (link != nullptr)
            {
                if (link->mappingElement() == nullptr)
                {
                    // The link was Virtual (agents were not linked on the network), it is a real one now !
                    qDebug() << "The link" << link->uid() << "from VIRTUAL to REAL";

                    link->setmappingElement(mappingElement);

                    if (link->isTemporary())
                    {
                        qDebug() << "The link" << link->uid() << "from TEMPORARY to REAL";
                        link->setisTemporary(false);
                        if (_hashFromLinkIdToAddedLink_WaitingReply.contains(link->uid()))
                        {
                            // Remove from the hash table the "(unique) link id" and the link for which we are waiting a reply to the request "add"
                            _hashFromLinkIdToAddedLink_WaitingReply.remove(link->uid());
                        }
                        else
                        {
                            qCritical() << "The 'Temporary' link" << link->uid() << "was not in the hash table 'Added Link Waiting Reply'";
                        }
                    }
                    else
                    {
                        if (link->inputObject() != nullptr)
                        {
                            // The link has been added when the agent was OFF
                            AgentInMappingVM* inputAgentFromLink = qobject_cast<AgentInMappingVM*>(link->inputObject());
                            if ((inputAgentFromLink != nullptr) && inputAgentFromLink->getAddedMappingElementFromLinkId_WhileAgentWasOFF(link->uid()))
                            {
                                qDebug() << "There is still the corresponding added Mapping Element" << link->uid() << "while the Agent was OFF";
                                inputAgentFromLink->cancelAddLink_WhileAgentIsOFF(link->uid());
                            }
                        }
                    }
                }
                else {
                    qWarning() << "The link" << link->uid() << "already exist";
                }
            }
        }
        // There are several links with this name in the global mapping
        else {
            qWarning() << "There are" << linksWithSameName.count() << "links with the same name" << linkName << ". We cannot choose the link";
        }
    }
}


/**
 * @brief Link an agent (in the global mapping) on its outputs (add all missing links FROM an agent)
 */
void AgentsMappingController::_linkAgentOnOutputs(AgentInMappingVM* agentInMapping)
{
    if ((agentInMapping != nullptr) && !agentInMapping->name().isEmpty())
    {
        QString agentName = agentInMapping->name();

        // Get the list of "Waiting Mapping Elements" on this Output agent name (where the agent is involved as "Output Agent")
        QList<MappingElementVM*> listOfWaitingMappingElements = _getWaitingMappingElementsOnOutputAgent(agentName);

        if (!listOfWaitingMappingElements.isEmpty())
        {
            for (MappingElementVM* waitingMappingElement : listOfWaitingMappingElements)
            {
                if ((waitingMappingElement != nullptr) && !waitingMappingElement->name().isEmpty() && (waitingMappingElement->firstModel() != nullptr))
                {
                    QString linkName = waitingMappingElement->name();
                    qDebug() << "Try to create WAITING link" << linkName << "from agent" << agentName << "involved as 'OUTPUT' agent";

                    // Get the input agent in the global mapping from the input agent name
                    AgentInMappingVM* inputAgent = getAgentInMappingFromName(waitingMappingElement->firstModel()->inputAgent());

                    if (inputAgent != nullptr)
                    {
                        // Get the link input from a name if there is only one input for this name
                        LinkInputVM* linkInput = _getAloneLinkInputFromName(inputAgent, waitingMappingElement->firstModel()->input(), linkName);

                        // Get the link output from a name if there is only one output for this name
                        LinkOutputVM* linkOutput = _getAloneLinkOutputFromName(agentInMapping, waitingMappingElement->firstModel()->output(), linkName);

                        if ((linkOutput != nullptr) && (linkInput != nullptr))
                        {
                            // Create a new REAL link between the two agents
                            _createLinkBetweenTwoObjectsInMapping(linkName, agentInMapping, linkOutput, inputAgent, linkInput, waitingMappingElement);

                            // Remove the corresponding "Waiting Mapping Element" on the output agent name
                            _removeWaitingMappingElementOnOutputAgent(agentName, waitingMappingElement);
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Get a random position in the current window
 * @param randomMax
 * @return
 */
QPointF AgentsMappingController::_getRandomPosition(double randomMax)
{
    double randomX = static_cast<double>(qrand()) / randomMax;
    double randomY = static_cast<double>(qrand()) / randomMax;

    // 5% + (random * 90% of the width) + the spawn zone's offset to follow the viewport
    double x = _xSpawnZoneOffset + (0.05 * _viewWidth) + (0.85 * _viewWidth * randomX);

    // 5% + (random * 90% of the height) + the spawn zone's offset to follow the viewport
    double y = _ySpawnZoneOffset + (0.05 * _viewHeight) + (0.85 * _viewHeight * randomY);

    return QPointF(x, y);
}


/**
 * @brief Get the list of "Waiting Mapping Elements" on an output agent (name)
 * @param outputAgentName
 * @return
 */
QList<MappingElementVM*> AgentsMappingController::_getWaitingMappingElementsOnOutputAgent(const QString& outputAgentName)
{
    return _hashFromOutputAgentNameToListOfWaitingMappingElements.value(outputAgentName, QList<MappingElementVM*>());
}


/**
 * @brief Add a "Waiting Mapping Element" on an output agent (name)
 * @param outputAgentName
 * @param waitingMappingElement
 */
void AgentsMappingController::_addWaitingMappingElementOnOutputAgent(const QString& outputAgentName, MappingElementVM* waitingMappingElement)
{
    if (!outputAgentName.isEmpty() && (waitingMappingElement != nullptr))
    {
        // Get the list of "Waiting Mapping Elements" on this output agent name (where the agent is involved as "Output Agent")
        QList<MappingElementVM*> listOfWaitingMappingElements = _getWaitingMappingElementsOnOutputAgent(outputAgentName);

        if (!listOfWaitingMappingElements.contains(waitingMappingElement))
        {
            // Add the "Waiting" mapping element to the list
            listOfWaitingMappingElements.append(waitingMappingElement);

            // Update the hash table
            _hashFromOutputAgentNameToListOfWaitingMappingElements.insert(outputAgentName, listOfWaitingMappingElements);

            qDebug() << waitingMappingElement->name() << "added to 'Waiting List' on" << outputAgentName;
        }
    }
}


/**
 * @brief Remove a "Waiting Mapping Element" on an output agent (name)
 * @param outputAgentName
 * @param waitingMappingElement
 */
void AgentsMappingController::_removeWaitingMappingElementOnOutputAgent(const QString& outputAgentName, MappingElementVM* waitingMappingElement)
{
    if (!outputAgentName.isEmpty() && (waitingMappingElement != nullptr))
    {
        // Get the list of "Waiting Mapping Elements" on this output agent name (where the agent is involved as "Output Agent")
        QList<MappingElementVM*> listOfWaitingMappingElements = _getWaitingMappingElementsOnOutputAgent(outputAgentName);

        if (listOfWaitingMappingElements.contains(waitingMappingElement))
        {
            // Remove the "Waiting" mapping element from the list
            listOfWaitingMappingElements.removeOne(waitingMappingElement);

            // Update the hash table
            _hashFromOutputAgentNameToListOfWaitingMappingElements.insert(outputAgentName, listOfWaitingMappingElements);

            qDebug() << waitingMappingElement->name() << "removed from 'Waiting List' on" << outputAgentName;
        }
    }
}


/**
 * @brief Helper to get the link input from a name if there is only one input for this name
 * If there are several inputs with the same name (but different value type), we return NULL
 * @param agent
 * @param inputName
 * @param linkName
 * @return
 */
LinkInputVM* AgentsMappingController::_getAloneLinkInputFromName(AgentInMappingVM* agent, const QString& inputName, const QString& linkName)
{
    LinkInputVM* linkInput = nullptr;

    if ((agent != nullptr) && !inputName.isEmpty())
    {
        // Get the list of view models of link input from the input name
        QList<LinkInputVM*> linkInputsWithSameName = agent->getLinkInputsListFromName(inputName);

        if (linkInputsWithSameName.count() == 1) {
            linkInput = linkInputsWithSameName.at(0);
        }
        else if (linkInputsWithSameName.count() > 1) {
            qWarning() << "There are" << linkInputsWithSameName.count() << "link inputs with the same name" << inputName << "."
                       << "We cannot choose and create the link" << linkName;
        }
    }
    return linkInput;
}


/**
 * @brief Helper to get the link output from a name if there is only one output for this name
 * If there are several outputs with the same name (but different value type), we return NULL
 * @param agent
 * @param outputName
 * @param linkName
 * @return
 */
LinkOutputVM* AgentsMappingController::_getAloneLinkOutputFromName(AgentInMappingVM* agent, const QString& outputName, const QString& linkName)
{
    LinkOutputVM* linkOutput = nullptr;

    if ((agent != nullptr) && !outputName.isEmpty())
    {
        // Get the list of view models of link output from the output name
        QList<LinkOutputVM*> linkOutputsWithSameName = agent->getLinkOutputsListFromName(outputName);

        if (linkOutputsWithSameName.count() == 1) {
            linkOutput = linkOutputsWithSameName.at(0);
        }
        else if (linkOutputsWithSameName.count() > 1) {
            qWarning() << "There are" << linkOutputsWithSameName.count() << "link outputs with the same name" << outputName << "."
                       << "We cannot choose and create the link" << linkName;
        }
    }
    return linkOutput;
}


/**
 * @brief Get the JSON of the mapping of an agent as displayed in the global mapping
 * @param agentInMapping
 * @return
 */
QString AgentsMappingController::_getJSONofMappingOfAgentInGlobalMapping(AgentInMappingVM* agentInMapping)
{
    QString jsonOfMapping;

    if ((agentInMapping != nullptr) && (agentInMapping->agentsGroupedByName() != nullptr) && (agentInMapping->agentsGroupedByName()->currentMapping() != nullptr))
    {
        if (agentInMapping->hadLinksAdded_WhileAgentWasOFF() || agentInMapping->hadLinksRemoved_WhileAgentWasOFF())
        {
            // Get the list of all added mapping elements while the global mapping was UN-activated
            QList<MappingElementM*> addedMappingElements = agentInMapping->getAddedMappingElements_WhileAgentWasOFF();

            // Get the list of all names of removed mapping elements while the global mapping was UN-activated
            QStringList namesOfRemovedMappingElements = agentInMapping->getNamesOfRemovedMappingElements_WhileAgentWasOFF();

            // Get the JSON of the current mapping of the agent with changes applied while the mapping was UN-activated
            jsonOfMapping = JsonHelper::getJsonOfAgentMappingWithChanges(agentInMapping->agentsGroupedByName()->currentMapping(),
                                                                         addedMappingElements,
                                                                         namesOfRemovedMappingElements,
                                                                         QJsonDocument::Compact);
        }
        else
        {
            // Get the JSON of the current mapping of the agent
            jsonOfMapping = JsonHelper::getJsonOfAgentMapping(agentInMapping->agentsGroupedByName()->currentMapping(),
                                                              QJsonDocument::Compact);
        }
    }
    return jsonOfMapping;
}
