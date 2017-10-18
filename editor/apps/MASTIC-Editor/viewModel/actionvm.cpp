/*
 *	ActionVM
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

#include "actionvm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionVM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionVM::ActionVM(ActionM *actionModel, QObject *parent) : QObject(parent),
    _actionModel(actionModel),
    _actionsPanelIndex(-1),
    _lineInTimeLine(-1),
    _startDateTime(QDateTime::currentDateTime())
{
    if(_actionModel != NULL && _actionModel->startTime() >= 0)
    {
        _startDateTime.setTime(QTime::fromMSecsSinceStartOfDay(_actionModel->startTime()*1000));
    }
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{
    if(_actionModel != NULL)
    {
        delete _actionModel;
        _actionModel = NULL;
    }
}


