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
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
HostsSupervisionController::HostsSupervisionController(IngeScapeLauncherManager* ingescapeLauncherManager,
                                                         QObject *parent) : QObject(parent),
    _selectedHost(NULL),
    _ingescapeLauncherManager(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    setingescapeLauncherManager(ingescapeLauncherManager);
}


/**
 * @brief Destructor
 */
HostsSupervisionController::~HostsSupervisionController()
{
    // Clean-up current selection
    setselectedHost(NULL);

    _mapFromHostModelToViewModel.clear();

    // Delete all VM of host
    _hostsList.deleteAllItems();

    // Clean reference to IngeScape launcher manager
    setingescapeLauncherManager(NULL);
}


/**
 * @brief Slot when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelCreated(HostM* hostModel)
{
    if (hostModel != NULL)
    {
        HostVM* newHost = new HostVM(hostModel, this);
        _hostsList.append(newHost);
        _mapFromHostModelToViewModel.insert(hostModel, newHost);

        connect(newHost, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

        // associate host with existing agents if necessary
        for (AgentM* agent : _agentsList)
        {
            if (
                (agent != NULL)
                &&
                (newHost->hostModel() != NULL)
                &&
                (newHost->hostModel()->ipAddress() == agent->address())
                &&
                (newHost->listOfAgents() != NULL)
                &&
                !newHost->listOfAgents()->contains(agent)
               )
            {
                qDebug() << "Add agent " << agent->name() << " to host " << newHost->hostModel()->name();

                newHost->listOfAgents()->append(agent);
            }
        }
    }
}


/**
 * @brief Slot when a model of host will be removed
 * @param host
 */
void HostsSupervisionController::onHostModelWillBeRemoved(HostM* hostModel)
{
    if (hostModel != NULL)
    {
        if (_mapFromHostModelToViewModel.contains(hostModel))
        {
            HostVM* hostToRemove = _mapFromHostModelToViewModel.value(hostModel);

            disconnect(hostToRemove, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

            if (_hostsList.contains(hostToRemove)) {
                _hostsList.remove(hostToRemove);
            }

            _mapFromHostModelToViewModel.remove(hostModel);
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
        if (!_agentsList.contains(agent)) {
            _agentsList.append(agent);
        }

        // try to get the involved hostVM with agent's host name
        for (HostVM* host : _hostsList)
        {
            if ( (host != NULL) && (host->hostModel() != NULL)
                &&
                (host->hostModel()->ipAddress() == agent->address())
                &&
                !host->listOfAgents()->contains(agent)
               )
            {
                host->listOfAgents()->append(agent);
                qDebug() << "Add agent " << agent->name() << " to host " << host->hostModel()->name();
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
        if (_agentsList.contains(agent)) {
            _agentsList.removeOne(agent);
        }

        // try to get the involved hostVM with agent's host name
        for (HostVM* host : _hostsList)
        {
            if ((host != NULL) && host->listOfAgents()->contains(agent))
            {
                host->listOfAgents()->remove(agent);
                qDebug() << "Remove agent " << agent->name() << " from host " << host->hostModel()->name();
                break;
            }
        }
    }
}

