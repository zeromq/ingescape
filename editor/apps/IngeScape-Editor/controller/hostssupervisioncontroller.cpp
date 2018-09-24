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
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
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
HostsSupervisionController::HostsSupervisionController(QObject *parent) : QObject(parent),
    _selectedHost(NULL)
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
    setselectedHost(NULL);

    // Clear the hash table
    _hashFromNameToHost.clear();

    // Free memory
    _hostsList.deleteAllItems();

    // Clear the list of agents
    _allAgents.clear();
}


/**
 * @brief Remove each UN-active agent (agent with state OFF) from the global list with all agents
 */
void HostsSupervisionController::removeUNactiveAgents()
{
    QList<AgentM*> copy = QList<AgentM*>(_allAgents);

    // Remove each model with state OFF
    for (AgentM* model : copy)
    {
        if ((model != NULL) && !model->isON()) {
            _allAgents.removeOne(model);
        }
    }
}


/**
 * @brief Slot called when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelCreated(HostM* host)
{
    if (host != NULL)
    {
        QString hostname = host->name();

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(hostname);
        if (hostVM == NULL)
        {
            // Create a view model for this model of host
            hostVM = new HostVM(host, this);

            connect(hostVM, &HostVM::commandAskedToAgent, this, &HostsSupervisionController::commandAskedToAgent);
            connect(hostVM, &HostVM::commandAskedToLauncher, this, &HostsSupervisionController::commandAskedToLauncher);

            _hashFromNameToHost.insert(hostname, hostVM);

            // Add to the sorted list of hosts
            _hostsList.append(hostVM);

            // Associate host with existing agents if necessary
            for (AgentM* agent : _allAgents)
            {
                if ((agent != NULL) && (agent->hostname() == hostname) && !hostVM->agentsList()->contains(agent))
                {
                    qDebug() << "Add (existing) agent" << agent->name() << "to new host" << hostname;

                    hostVM->agentsList()->append(agent);
                }
            }
        }
    }
}


/**
 * @brief Slot called when a model of host will be removed
 * @param host
 */
void HostsSupervisionController::onHostModelWillBeRemoved(HostM* host)
{
    if (host != NULL)
    {
        QString hostname = host->name();

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(hostname);
        if (hostVM != NULL)
        {
            disconnect(hostVM, 0, this, 0);

            _hashFromNameToHost.remove(hostname);

            if (_selectedHost == hostVM) {
                // Clean-up current selection
                setselectedHost(NULL);
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
void HostsSupervisionController::onAgentModelCreated(AgentM* agent)
{
    if (agent != NULL)
    {
        // Connect to signals from this new agent
        connect(agent, &AgentM::networkDataWillBeCleared, this, &HostsSupervisionController::_onNetworkDataOfAgentWillBeCleared);

        _allAgents.append(agent);

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(agent->hostname());

        if ((hostVM != NULL) && !hostVM->agentsList()->contains(agent))
        {
            // Add this agent to the host
            hostVM->agentsList()->append(agent);

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
    if (agent != NULL)
    {
        // DIS-connect to signals from the agent
        disconnect(agent, 0, this, 0);

        _allAgents.removeOne(agent);

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(agent->hostname());

        if ((hostVM != NULL) && hostVM->agentsList()->contains(agent))
        {
            // Remove this agent from the host
            hostVM->agentsList()->remove(agent);

            qDebug() << "Remove agent" << agent->name() << "from host" << hostVM->name();
        }
    }
}


/**
 * @brief Slot called when the network data (of an agent) will be cleared
 * @param peerId
 */
void HostsSupervisionController::_onNetworkDataOfAgentWillBeCleared(QString peerId)
{
    Q_UNUSED(peerId)

    AgentM* agent = qobject_cast<AgentM*>(sender());
    if (agent != NULL)
    {
        //qDebug() << "Hosts Supervision: on Network Data of agent" << agent->name() << "will be cleared" << agent->hostname() << "(" << agent->peerId() << ")";

        // Get the view model of host with its name
        HostVM* hostVM = _getHostWithName(agent->hostname());

        if ((hostVM != NULL) && hostVM->agentsList()->contains(agent))
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
        return NULL;
    }
}

