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
 *
 */

#include "scenariom.h"

#include <QDebug>

/**
 * @brief Constructor
 * @param parent
 */
ScenarioM::ScenarioM(QObject *parent) : QObject(parent),
    _name("")
    //_currentTime(QDateTime())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ScenarioM::~ScenarioM()
{
    // Do not delete models of action (used in the scenario controller)
    _actionsList.clear();

    // The 9 view models of actions ("in palette") are created in the constructor of "ScenarioController"
    // The view models of actions ("in palette") stored in this list are used only to store temporarily their model of actions
    // So, we can delete all these "temporary" view models of actions ("in palette")
    _actionsInPaletteList.deleteAllItems();

    // Do not delete view models of action (used in the scenario controller)
    _actionsInTimelineList.clear();
}


