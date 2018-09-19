/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "scenariomarkerm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ScenarioMarkerM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ScenarioMarkerM::ScenarioMarkerM(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ScenarioMarkerM::~ScenarioMarkerM()
{
}


