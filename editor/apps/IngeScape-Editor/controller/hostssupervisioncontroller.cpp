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
    _mapFromNameToHost.clear();

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
 * @brief Slot when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelCreated(HostM* host)
{
    if (host != NULL)
    {
        HostVM* newHostVM = new HostVM(host, this);

        _hostsList.append(newHostVM);

        if (!_mapFromNameToHost.contains(host->name())) {
            _mapFromNameToHost.insert(host->name(), newHostVM);
        }

        connect(newHostVM, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

        // Associate host with existing agents if necessary
        for (AgentM* agent : _allAgents)
        {
            if ((agent != NULL) && (agent->address() == host->ipAddress()) && !newHostVM->listOfAgents()->contains(agent))
            {
                qDebug() << "Add agent " << agent->name() << " to host " << newHostVM->name();

                newHostVM->listOfAgents()->append(agent);
            }
        }
    }
}


/**
 * @brief Slot when a model of host will be removed
 * @param host
 */
void HostsSupervisionController::onHostModelWillBeRemoved(HostM* host)
{
    if (host != NULL)
    {
        if (_mapFromNameToHost.contains(host->name()))
        {
            HostVM* hostToRemove = _mapFromNameToHost.value(host->name());
            if (hostToRemove != NULL)
            {
                disconnect(hostToRemove, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

                if (_hostsList.contains(hostToRemove)) {
                    _hostsList.remove(hostToRemove);
                }

                if (_selectedHost == hostToRemove) {
                    // Clean-up current selection
                    setselectedHost(NULL);
                }

                // Free memory
                delete hostToRemove;
            }

            _mapFromNameToHost.remove(host->name());
        }
    }
}


/**
 * @brief Slot when a new model of agent has been created
 * @param agent
 */
void HostsSupervisionController::onAgentModelCreated(AgentM* agent)
{
    if (agent != NULL)
    {
        if (!_allAgents.contains(agent)) {
            _allAgents.append(agent);
        }

        // Try to get the involved hostVM with agent's host name
        for (HostVM* host : _hostsList.toList())
        {
            if ( (host != NULL) && (host->modelM() != NULL)
                &&
                (host->modelM()->ipAddress() == agent->address())
                &&
                !host->listOfAgents()->contains(agent) )
            {
                host->listOfAgents()->append(agent);

                qDebug() << "Add agent " << agent->name() << " to host " << host->name();

                break;
            }
        }
    }
}


/**
 * @brief Slot when a model of agent will be deleted
 * @param agent
 */
void HostsSupervisionController::onAgentModelWillBeDeleted(AgentM* agent)
{
    if (agent != NULL)
    {
        if (_allAgents.contains(agent)) {
            _allAgents.removeOne(agent);
        }

        // Try to get the involved hostVM with agent's host name
        for (HostVM* host : _hostsList.toList())
        {
            if ((host != NULL) && host->listOfAgents()->contains(agent))
            {
                host->listOfAgents()->remove(agent);

                qDebug() << "Remove agent " << agent->name() << " from host " << host->modelM()->name();

                break;
            }
        }
    }
}

