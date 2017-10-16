/*
 *	ActionM
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

#include "actionm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionM::ActionM(QString name, QObject *parent) : QObject(parent),
    _name(name),
    _startTime(-1),
    _actionsPanelIndex(-1),
    _lineInTimeLine(-1),
    _validityDuration(0),
    _shallRevert(false),
    _revertWhenValidityIsOver(false),
    _revertAtTime(-1),
    _shallRearm(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionM::~ActionM()
{
    // Delete the list of conditions
    _conditionsList.deleteAllItems();

    // Delete the list of effects
    _effectsList.deleteAllItems();
}


