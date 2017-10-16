/*
 *	ScenarioM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *
 */

#include "scenariom.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ScenarioM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ScenarioM::ScenarioM(QString name, QObject *parent) : QObject(parent),
    _name(name),
    _currentTime(QDateTime())
    //_globalMapping(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ScenarioM::~ScenarioM()
{
    // Reset global mapping
    //setglobalMapping(NULL);

    // Delete list of markers
    _markersList.deleteAllItems();

    // Delete list of actions
    _actionsList.deleteAllItems();
}


