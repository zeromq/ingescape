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

    _mapFromHostModelToViewModel.clear();

    _allAgents.clear();

    // Delete all VM of host
    _hostsList.deleteAllItems();
}


/**
 * @brief Slot when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelCreated(HostM* host)
{
    if (host != NULL)
    {
        HostVM* newHost = new HostVM(host, this);

        _hostsList.append(newHost);
        _mapFromHostModelToViewModel.insert(host, newHost);

        connect(newHost, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

        // associate host with existing agents if necessary
        for (AgentM* agent : _allAgents)
        {
            if ( (agent != NULL) && (newHost->modelM() != NULL)
                &&
                (newHost->modelM()->ipAddress() == agent->address())
                &&
                !newHost->listOfAgents()->contains(agent) )
            {
                qDebug() << "Add agent " << agent->name() << " to host " << newHost->modelM()->name();

                newHost->listOfAgents()->append(agent);
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
        if (_mapFromHostModelToViewModel.contains(host))
        {
            HostVM* hostToRemove = _mapFromHostModelToViewModel.value(host);

            disconnect(hostToRemove, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

            if (_hostsList.contains(hostToRemove)) {
                _hostsList.remove(hostToRemove);
            }

            _mapFromHostModelToViewModel.remove(host);
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

        // try to get the involved hostVM with agent's host name
        for (HostVM* host : _hostsList.toList())
        {
            if ( (host != NULL) && (host->modelM() != NULL)
                &&
                (host->modelM()->ipAddress() == agent->address())
                &&
                !host->listOfAgents()->contains(agent) )
            {
                host->listOfAgents()->append(agent);
                qDebug() << "Add agent " << agent->name() << " to host " << host->modelM()->name();
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

        // try to get the involved hostVM with agent's host name
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

