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
    _startTimeString("00:00:00.000"),
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
            int hours = startTime / 3600000;
            int minutes = (startTime - hours*3600000)/60000 ;
            int seconds = (startTime - hours*3600000 - minutes*60000) / 1000;
            int milliseconds = startTime%1000;

            _startTimeString = QString::number(hours) + ":" + QString::number(minutes) + ":" + QString::number(seconds) + "." + QString::number(milliseconds);
        }

        // Update valid flag
        setisValid(model->isValid());

        // Set the action model
        setactionModel(model);

        // Create the first (view model of) action execution
        _createActionExecution(0);
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
 * @brief Setter for property "Start Time String"
 * @param value
 */
void ActionVM::setstartTimeString(QString value)
{
    if (_startTimeString != value)
    {
        _startTimeString = value;

        if (!_startTimeString.isEmpty())
        {
            bool successSeconds = false;
            bool successMinutes = false;
            bool successHours = false;
            bool successMilliSeconds = false;

            int hours = 0;
            int minutes = 0;
            int seconds = 0;
            int milliSeconds = 0;

            // Split start time HH:MM:SS
            QStringList splittedTime = _startTimeString.split(':');
            if (splittedTime.count() == 3)
            {
                hours = splittedTime.at(0).toInt(&successHours);
                minutes = splittedTime.at(1).toInt(&successMinutes);

                // Try to split SS.zzz
                QStringList splittedTimeSeconds = splittedTime.at(2).split('.');
                if (splittedTimeSeconds.count() == 2)
                {
                    seconds = splittedTimeSeconds.at(0).toInt(&successSeconds);
                    milliSeconds = splittedTimeSeconds.at(1).leftJustified(3, '0').toInt(&successMilliSeconds);
                }
                else {
                    seconds = splittedTime.at(2).toInt(&successSeconds);
                }
            }

            if(successHours && successMinutes && successSeconds)
            {
                int totalMilliseconds = hours*3600000 + minutes*60000 + seconds*1000;
                if(successMilliSeconds)
                {
                    totalMilliseconds += milliSeconds;
                    setstartTime(totalMilliseconds);
                    qDebug() << "Start Time =" << _startTime;
                } else {
                    setstartTime(-1);
                    if (_actionModel != NULL) {
                        qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _actionModel->name();
                    }
                }
            } else {
                setstartTime(-1);
                if (_actionModel != NULL) {
                    qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _actionModel->name();
                }
            }
        }
        else {
            setstartTime(-1);
            if (_actionModel != NULL) {
                qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _actionModel->name();
            }
        }

        // Compute the new endtime
        _computeEndTime();

        emit startTimeStringChanged(value);
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

            // Endtime reevaluation disconnection
            disconnect(_actionModel, &ActionM::validityDurationChanged, this, &ActionVM::_onValidityDurationChange);
            disconnect(_actionModel, &ActionM::validityDurationTypeChanged, this, &ActionVM::_onValidityDurationChange);
        }

        _actionModel = actionM;

        if(_actionModel != NULL)
        {
            connect(_actionModel, &ActionM::isValidChanged, this, &ActionVM::onActionIsValidChange);

            // Endtime reevaluation connection
            connect(_actionModel, &ActionM::validityDurationChanged, this, &ActionVM::_onValidityDurationChange);
            connect(_actionModel, &ActionM::validityDurationTypeChanged, this, &ActionVM::_onValidityDurationChange);
        }

        // Compute the new endtime
        _computeEndTime();

        Q_EMIT actionModelChanged(actionM);
    }
}


/**
 * @brief Notify our action that its effects has been executed
 * @param currentTimeInMilliSeconds
 */
void ActionVM::effectsExecuted(int currentTimeInMilliSeconds)
{
    if (_currentExecution != NULL)
    {
        _currentExecution->setisExecuted(true);

        // Shall revert
        if (_currentExecution->shallRevert())
        {
            // Update flag "Is Waiting Revert"
            _currentExecution->setisWaitingRevert(true);
        }
        // No reverse
        else
        {
            setcurrentExecution(NULL);

            // Shall rearm
            if (_actionModel->shallRearm())
            {
                // Create a new (view model of) action execution
                _createActionExecution(currentTimeInMilliSeconds - _startTime);
            }
        }
    }
}


/**
 * @brief Notify our action that its reverse effects has been executed
 * @param currentTimeInMilliSeconds
 */
void ActionVM::reverseEffectsExecuted(int currentTimeInMilliSeconds)
{
    if (_currentExecution != NULL)
    {
        setcurrentExecution(NULL);

        // Shall rearm
        if (_actionModel->shallRearm())
        {
            // Create a new (view model of) action execution
            _createActionExecution(currentTimeInMilliSeconds - _startTime);
        }
    }
}


/**
 * @brief Delay the current execution of our action
 * @param currentTimeInMilliSeconds
 */
void ActionVM::delayCurrentExecution(int currentTimeInMilliSeconds)
{
    if (_currentExecution != NULL)
    {
        // Add 1 ms
        _currentExecution->setexecutionTime(currentTimeInMilliSeconds - _startTime + 1);

        if ((_actionModel != NULL) && _actionModel->shallRevert() && _actionModel->shallRevertAfterTime())
        {
            // FIXME TODO: check reverseTime is not after validityDuration

            if (_actionModel->revertAfterTime() > -1) {
                _currentExecution->setreverseTime(currentTimeInMilliSeconds - _startTime + _actionModel->revertAfterTime());
            }
            else {
                qWarning() << "Action" << _actionModel->name() << "Shall revert after time but 'revert after time' is not defined";
            }
        }
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
 * @brief Slot on the validity duration change
 * @param validity duration
 */
void ActionVM::_onValidityDurationChange()
{
    _computeEndTime();
}

/**
 * @brief Compute the endTime according to the action model and its type
 */
void ActionVM::_computeEndTime()
{
    int endTime = _startTime;
    if(_actionModel != NULL)
    {
        int itemDurationTime = 0;
        if(_actionModel->validityDurationType() == ValidationDurationType::FOREVER)
        {
            endTime = -1;
        }
        else if(_actionModel->validityDurationType() == ValidationDurationType::CUSTOM)
        {
            itemDurationTime = _actionModel->validityDuration();
        }

        // Compare with the time before revert if selected
        if(_actionModel->shallRevertAfterTime() && _actionModel->revertAfterTime() > itemDurationTime)
        {
            itemDurationTime = _actionModel->revertAfterTime();
        }

        // If not forever, add duration
        if(endTime != -1)
        {
            endTime += itemDurationTime;
        }
    }

    // Set the new value
    setendTime(endTime);
}


/**
 * @brief Create a new (view model of) action execution
 * @param relative to our view model of action
 */
void ActionVM::_createActionExecution(int startTime)
{
    if (_actionModel != NULL)
    {
        // No revert by default: reverse time = start time
        int reverseTime = startTime;

        if (_actionModel->shallRevert())
        {
            // Shall revert when validity is over
            if (_actionModel->shallRevertWhenValidityIsOver())
            {
                if (_actionModel->validityDuration() > -1) {
                    reverseTime = startTime + _actionModel->validityDuration();
                }
                else {
                    qWarning() << "Action" << _actionModel->name() << "Shall revert when validity is over but 'validity duration' is not defined";
                }
            }
            // Shall revert after time
            else if (_actionModel->shallRevertAfterTime())
            {
                if (_actionModel->revertAfterTime() > -1) {
                    reverseTime = startTime + _actionModel->revertAfterTime();
                }
                else {
                    qWarning() << "Action" << _actionModel->name() << "Shall revert after time but 'revert after time' is not defined";
                }
            }
        }

        // Create a new (view model of) action execution
        ActionExecutionVM* actionExecution = actionExecution = new ActionExecutionVM(_actionModel->shallRevert(), startTime, reverseTime, this);

        // Add to the list
        _executionsList.append(actionExecution);

        // Set as current
        setcurrentExecution(actionExecution);
    }
}

