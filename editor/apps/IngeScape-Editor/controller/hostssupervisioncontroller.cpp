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
HostsSupervisionController::HostsSupervisionController(IngeScapeModelManager* modelManager,
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
            // Create a view model for this model of host
            hostVM = new HostVM(host, this);

            connect(hostVM, &HostVM::commandAskedToAgent, this, &HostsSupervisionController::commandAskedToAgent);
            connect(hostVM, &HostVM::commandAskedToLauncher, this, &HostsSupervisionController::commandAskedToLauncher);

            _hashFromNameToHost.insert(hostName, hostVM);

            _hostsList.append(hostVM);

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
            disconnect(hostVM, nullptr, this, nullptr);

            _hashFromNameToHost.remove(hostName);

            if (_selectedHost == hostVM) {
                // Clean-up current selection
                setselectedHost(nullptr);
            }

            // Remove from the sorted list of hosts
            _hostsList.remove(hostVM);

            // Free memory
            delete hostVM;
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

            if (hostVM->name() != HOSTNAME_NOT_DEFINED)
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
 * @brief Get the view model of host with a name
 * @param hostName
 * @return
 */
HostVM* HostsSupervisionController::_getHostWithName(QString hostName)
{
    if (_hashFromNameToHost.contains(hostName)) {
        return _hashFromNameToHost.value(hostName);
    }
    else {
        return nullptr;
    }
}
