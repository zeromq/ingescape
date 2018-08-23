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
 * @brief Reset the list of agents
 */
void HostsSupervisionController::reset()
{
    // Clear the list of agents
    _allAgents.clear();
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

            connect(hostVM, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

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
            disconnect(hostVM, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

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
        if (!_allAgents.contains(agent)) {
            _allAgents.append(agent);
        }

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
        if (_allAgents.contains(agent)) {
            _allAgents.removeOne(agent);
        }

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
 * @brief Get the view model of host with an IP address
 * @param ipAddress
 * @return
 */
/*HostVM* HostsSupervisionController::_getHostWithAddress(QString ipAddress)
{
    if (_hashFromAddressToHostVM.contains(ipAddress)) {
        return _hashFromAddressToHostVM.value(ipAddress);
    }
    else {
        return NULL;
    }
}*/


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

