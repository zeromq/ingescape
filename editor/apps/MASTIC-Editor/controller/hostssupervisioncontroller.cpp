/*
 *	MASTIC Editor
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

#include "HostsSupervisionController.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
HostsSupervisionController::HostsSupervisionController(MasticLauncherManager* masticLauncherManager,
                                                         QObject *parent) : QObject(parent),
    _selectedHost(NULL),
    _masticLauncherManager(masticLauncherManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
HostsSupervisionController::~HostsSupervisionController()
{
    // Clean-up current selection
//    setselectedAgent(NULL);

    _mapFromHostModelToViewModel.clear();

    // Delete all VM of agents
    _hostsList.deleteAllItems();

    _masticLauncherManager = NULL;
}

/**
 * @brief Slot when a new model of host has been created
 * @param host
 */
void HostsSupervisionController::onHostModelCreated(HostM* hostModel)
{
    if(hostModel != NULL)
    {
        HostVM* newHost = new HostVM(hostModel, this);
        _hostsList.append(newHost);
        _mapFromHostModelToViewModel.insert(hostModel, newHost);

        connect(newHost, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);
    }
}

/**
 * @brief Slot when a model of host will be removed
 * @param host
 */
void HostsSupervisionController::onHostModelWillBeRemoved(HostM* hostModel)
{
    if(hostModel != NULL)
    {
        if(_mapFromHostModelToViewModel.contains(hostModel))
        {
            HostVM* hostToRemove = _mapFromHostModelToViewModel.value(hostModel);

            disconnect(hostToRemove, &HostVM::commandAskedToHost, this, &HostsSupervisionController::commandAskedToHost);

            if(_hostsList.contains(hostToRemove))
                _hostsList.remove(hostToRemove);

            _mapFromHostModelToViewModel.remove(hostModel);
        }
    }
}

