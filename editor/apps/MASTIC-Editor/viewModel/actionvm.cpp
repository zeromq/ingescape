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
 * @brief Constructor
 * @param model
 * @param startTime
 * @param parent
 */
ActionVM::ActionVM(ActionM* model,
                   int startTime,
                   QObject *parent) : QObject(parent),
    _actionModel(NULL),
    _startTime(startTime),
    _startTimeString("0.0"),
    _lineInTimeLine(-1),
    _isValid(false),
    _currentExecution(NULL),
    _endTime(startTime)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != NULL)
    {
        if (startTime >= 0)
        {
            _startTimeString = QString::number((int)startTime / 1000) + "." + QString::number((int)startTime % 1000);
        }

        // Update valid flag
        setisValid(model->isValid());

        // Set the action model
        setactionModel(model);

        // No revert by default: reverse time = start time
        int reverseTime = _startTime;

        if (_actionModel->shallRevert())
        {
            // Shall revert when validity is over
            if (_actionModel->shallRevertWhenValidityIsOver()) {
                if (_actionModel->validityDuration() > -1) {
                    reverseTime = _startTime + _actionModel->validityDuration();
                }
                else {
                    qWarning() << "Action" << _actionModel->name() << "Shall revert when validity is over but 'validity duration' is not defined";
                }
            }
            // Shall revert after time
            else if (_actionModel->shallRevertAfterTime()) {
                if (_actionModel->revertAfterTime() > -1) {
                    reverseTime = _startTime + _actionModel->revertAfterTime();
                }
                else {
                    qWarning() << "Action" << _actionModel->name() << "Shall revert after time but 'revert after time' is not defined";
                }
            }
        }

        // Create the first view model of execution for our action
        ActionExecutionVM* actionExecution = actionExecution = new ActionExecutionVM(true, _startTime, reverseTime, this);

        _executionsList.append(actionExecution);
        setcurrentExecution(actionExecution);
    }
}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{
    // Reset current execution
    setcurrentExecution(NULL);

    // Delete all executions
    _executionsList.deleteAllItems();

    // Reset model of action
    setactionModel(NULL);
}


/**
 * @brief Copy from another action view model
 * @param action VM to copy
 */
void ActionVM::copyFrom(ActionVM* actionVM)
{
    if(actionVM != NULL)
    {
        ActionM* originalModel = actionVM->actionModel();

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
            disconnect(_actionModel, &ActionM::isValidChanged, this, &ActionVM::onActionIsValidChange);
        }

        _actionModel = actionM;

        if(_actionModel != NULL)
        {
            connect(_actionModel, &ActionM::isValidChanged, this, &ActionVM::onActionIsValidChange);
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




