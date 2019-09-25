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

#include "hostssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Constructor
 * @param parent
 */
HostsSupervisionController::HostsSupervisionController(EditorModelManager* modelManager,
                                                       QObject *parent) : QObject(parent),
    _selectedHost(nullptr),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Hosts are sorted on their name (alphabetical order)
    _hostsList.setSortProperty("name");

}


/**
 * @brief Destructor
 */
HostsSupervisionController::~HostsSupervisionController()
{
    // Clean-up current selection
    setselectedHost(nullptr);

    // Clear the hash table
    _hashFromNameToHost.clear();

    // Free memory
    _hostsList.deleteAllItems();

    // Clear the list of agents
    _allAgents.clear();

    // Reset pointers
    _modelManager = nullptr;
}


/**
 * @brief Remove a model of agent from a host
 * @param agent
 * @param host
 */
void HostsSupervisionController::removeAgentModelFromHost(AgentM* agent, HostVM* host)
{
    if ((host != nullptr) && (agent != nullptr) && (_modelManager != nullptr))
    {
        if (host->agentsList()->contains(agent))
        {
            host->agentsList()->remove(agent);

            qDebug() << "Remove agent" << agent->name() << "from host" << host->name();
        }

        AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agent->name());
        if (agentsGroupedByName != nullptr)
        {
            // Remove a model of agent from its host
            agentsGroupedByName->removeAgentModelFromHost(agent);
        }
    }
}


/**
 * @brief Delete hosts OFF
 */
void HostsSupervisionController::deleteHostsOFF()
{
    for (HostVM* host : _hostsList.toList())
    {
        if ((host != nullptr) && !host->isON() && (host->name() != HOSTNAME_NOT_DEFINED))
        {
            deleteHost(host);
        }
    }
}


/**
 * @brief Delete a view model of host
 * @param host
 */
void HostsSupervisionController::deleteHost(HostVM* host)
{
    if (host != nullptr)
    {
        disconnect(host, nullptr, this, nullptr);

        if (_selectedHost == host)
        {
            // Clean-up current selection
            setselectedHost(nullptr);
        }

        if (!host->name().isEmpty() && _hashFromNameToHost.contains(host->name())) {
            _hashFromNameToHost.remove(host->name());
        }

        // Remove from the sorted list of hosts
        _hostsList.remove(host);

        // Free memory
        delete host;
    }
}


/**
 * @brief Slot called when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelHasBeenCreated(HostM* host)
{
    if (host != nullptr)
    {
        QString hostName = host->name();

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(hostName);

        if (hostVM == nullptr)
        {
            // Create a view model of host with a name and a model
            hostVM = _createViewModelOfHost(hostName, host);

            // Associate host with existing agents if necessary
            for (AgentM* agent : _allAgents)
            {
                if ((agent != nullptr) && (agent->hostname() == hostName) && !hostVM->agentsList()->contains(agent))
                {
                    qDebug() << "Add (existing) agent" << agent->name() << "to new host" << hostName;

                    hostVM->agentsList()->append(agent);
                }
            }
        }
        else
        {
            if (hostVM->modelM() == nullptr)
            {
                // Set the model of host
                hostVM->setmodelM(host);

                // FIXME: Do something for agents ?
            }
            else {
                qCritical() << "A model of host already exist for the view model" << hostName;
            }
        }
    }
}


/**
 * @brief Slot called when a model of host will be deleted
 * @param host
 */
void HostsSupervisionController::onHostModelWillBeDeleted(HostM* host)
{
    if (host != nullptr)
    {
        QString hostName = host->name();

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(hostName);

        if (hostVM != nullptr)
        {
            if (hostVM->modelM() == host)
            {
                // Simply, remove the model of host from this view model
                hostVM->setmodelM(nullptr);
            }
            else {
                qCritical() << "The model of host does not correspond to the view model" << hostName;
            }
        }
    }
}


/**
 * @brief Slot called when a previous host has been parsed (in JSON file)
 * @param hostName
 * @param ipAddress
 */
void HostsSupervisionController::onPreviousHostParsed(QString hostName, QString ipAddress)
{
    if (!hostName.isEmpty() && !ipAddress.isEmpty())
    {
        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(hostName);

        if (hostVM == nullptr)
        {
            // Create a view model of host with a name
            hostVM = _createViewModelOfHost(hostName, nullptr);
        }
    }
}


/**
 * @brief Slot called when a new model of agent has been created
 * @param agent
 */
void HostsSupervisionController::onAgentModelHasBeenCreated(AgentM* agent)
{
    if ((agent != nullptr) && !agent->hostname().isEmpty())
    {
        _allAgents.append(agent);

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(agent->hostname());

        if ((hostVM != nullptr) && !hostVM->agentsList()->contains(agent))
        {
            // Add this agent to the host
            hostVM->agentsList()->append(agent);

            if (hostVM->isON() && (hostVM->name() != HOSTNAME_NOT_DEFINED))
            {
                // This agent can be restarted
                agent->setcanBeRestarted(true);
            }

            qDebug() << "Add agent" << agent->name() << "to host" << hostVM->name();
        }
    }
}


/**
 * @brief Slot called when a model of agent will be deleted
 * @param agent
 */
void HostsSupervisionController::onAgentModelWillBeDeleted(AgentM* agent)
{
    if ((agent != nullptr) && !agent->hostname().isEmpty())
    {
        _allAgents.removeOne(agent);

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(agent->hostname());

        if ((hostVM != nullptr) && hostVM->agentsList()->contains(agent))
        {
            // Remove this agent from the host
            hostVM->agentsList()->remove(agent);

            qDebug() << "Remove agent" << agent->name() << "from host" << hostVM->name();
        }
    }
}


/**
 * @brief Slot called when the name of a view model of host changed
 * @param newName
 */
/*void HostsSupervisionController::_onHostNameChanged(QString newName)
{
    HostVM* host = qobject_cast<HostVM*>(sender());
    if (host != nullptr)
    {
        QString previousName = "";

        for (QString key : _hashFromNameToHost.keys())
        {
            HostVM* iterator = _hashFromNameToHost.value(key);
            if ((iterator != nullptr) && (iterator == host))
            {
                previousName = key;
                break;
            }
        }

        if (!previousName.isEmpty())
        {
            _hashFromNameToHost.remove(previousName);
            _hashFromNameToHost.insert(newName, host);
        }
    }
}*/


/**
 * @brief Get the view model of host with a name
 * @param hostName
 * @return
 */
HostVM* HostsSupervisionController::_getHostWithName(QString hostName)
{
    return _hashFromNameToHost.value(hostName, nullptr);
}


/**
 * @brief Create a view model of host with a name and a model (optional)
 * @param hostName
 * @param model
 * @return
 */
HostVM* HostsSupervisionController::_createViewModelOfHost(QString hostName, HostM* model)
{
    HostVM* host = nullptr;

    if (!hostName.isEmpty() && !_hashFromNameToHost.contains(hostName))
    {
        // Create a view model for this model of host
        host = new HostVM(hostName, model, this);

        // Connect to signals
        //connect(host, &HostVM::nameChanged, this, &HostsSupervisionController::_onHostNameChanged);
        connect(host, &HostVM::commandAskedToAgent, this, &HostsSupervisionController::commandAskedToAgent);
        connect(host, &HostVM::commandAskedToLauncher, this, &HostsSupervisionController::commandAskedToLauncher);

        _hashFromNameToHost.insert(hostName, host);

        _hostsList.append(host);
    }
    return host;
}

