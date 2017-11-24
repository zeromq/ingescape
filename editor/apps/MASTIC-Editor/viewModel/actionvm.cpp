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
    _actionModel(NULL),
    _startTime(startTime),
   _startTimeString("0.0"),
    _lineInTimeLine(-1),
    _isValid(false),
    _currentActionVM(NULL),
    _endTime(startTime)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if(actionModel != NULL)
    {
        if(startTime >= 0)
        {
            _startTimeString = QString::number((int)startTime/1000) + "." + QString::number((int)startTime%1000);
        }

        // Update valid flag
        setisValid(actionModel->isValid());

        // Set the action model
        setactionModel(actionModel);
    }
}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{

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
        }
        else {
            setactionModel(NULL);
        }

        // Copy the view model attributes
        setcolor(actionVM->color());
        setstartTime(actionVM->startTime());
        setendTime(actionVM->endTime());
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
            QStringList splittedTime = stringDateTime.split('.');
            if(splittedTime.count() == 2)
            {
                setstartTime(QString(splittedTime.at(0)).toInt()*1000 + QString(splittedTime.at(1)).toInt());
            }
        }
        else
        {
            setstartTime(-1);
        }

        // Compute the new endtime
        _computeEndTime();

        emit startTimeStringChanged(stringDateTime);
    }
}

/**
 * @brief Custom setter on the action model
 * @param action model
 */
void ActionVM::setactionModel(ActionM * actionM)
{
    if(_actionModel != actionM)
    {
        if(_actionModel != NULL)
        {
            disconnect(_actionModel,&ActionM::isValidChanged,this, &ActionVM::onActionIsValidChange);
        }

        _actionModel = actionM;

        if(_actionModel != NULL)
        {
            connect(_actionModel,&ActionM::isValidChanged,this, &ActionVM::onActionIsValidChange);
        }
        else {
            setisValid(true);
            setendTime(_startTime);
        }

        // Compute the new endtime
        _computeEndTime();

        Q_EMIT actionModelChanged(actionM);
    }
}

/**
 * @brief Slot on the is valid flag change on the action Model
 * @param is valid flag
 */
void ActionVM::onActionIsValidChange(bool isValid)
{
    setisValid(isValid);
}

/**
 * @brief Compute the endTime according to the action model and its type
 */
void ActionVM::_computeEndTime()
{
    int endTime = _startTime;
    if(_actionModel != NULL)
    {
        if(_actionModel->validityDurationType() == ValidationDurationType::FOREVER)
        {
            endTime = -1;
        }
        else if(_actionModel->validityDurationType() == ValidationDurationType::CUSTOM)
        {
            endTime += _actionModel->validityDuration();
        }
    }

    // Set the new value
    setendTime(endTime);
}




