/*
 *	ActionVM
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
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
ActionVM::ActionVM(ActionM *actionModel, int startTime, QObject *parent) : QObject(parent),
    _actionModel(actionModel),
    _startTimeInSec(startTime),
    _startTimeString("00:00:00"),
    _lineInTimeLine(-1)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if(_actionModel != NULL)
    {
        if(startTime >= 0)
        {
            _startTimeString = QString::number((int)startTime/3600) + ":" + QString::number((int)startTime/60) + ":" + QString::number((int)startTime%60);
        }
    }
}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{
    if (_actionModel != NULL)
    {
        ActionM* temp = _actionModel;
        setactionModel(NULL);
        delete temp;
    }
}


/**
 * @brief Copy from another action view model
 * @param action VM to copy
 */
void ActionVM::copyFrom(ActionVM* actionVM)
{
    if(actionVM != NULL)
    {
        ActionM* originalModel =  actionVM->actionModel();

        // Copy the model
        if(originalModel != NULL)
        {
            ActionM* model = new ActionM(originalModel->name());
            model->copyFrom(originalModel);

            setactionModel(model);
        } else {
            setactionModel(NULL);
        }

        // Copy the view model attributes
        setcolor(actionVM->color());
        setstartTimeInSec(actionVM->startTimeInSec());
        setlineInTimeLine(actionVM->lineInTimeLine());
        setstartTimeString(actionVM->startTimeString());
    }
}

/**
 * @brief Set the date time in string format
 * @param action VM to copy
 */
void ActionVM::setstartTimeString(QString stringDateTime)
{
    if(_startTimeString.compare(stringDateTime) != 0)
    {
        _startTimeString = stringDateTime;

        // Update the start date time
        if(stringDateTime.isEmpty() == false)
        {
            QStringList splittedTime = stringDateTime.split(':');
            if(splittedTime.count() == 3)
            {
                setstartTimeInSec(QString(splittedTime.at(0)).toInt()*3600 + QString(splittedTime.at(1)).toInt()*60 + + QString(splittedTime.at(2)).toInt());
            }
        }
        else
        {
            setstartTimeInSec(-1);
        }

        emit startTimeStringChanged(stringDateTime);
    }
}




