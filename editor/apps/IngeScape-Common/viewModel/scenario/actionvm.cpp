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

#include "actionvm.h"

#include <QDebug>


/**
 * @brief Constructor
 * @param model
 * @param startTime
 * @param parent
 */
ActionVM::ActionVM(ActionM* model,
                   int startTime,
                   QObject *parent) : QObject(parent),
    _modelM(nullptr),
    _startTime(startTime),
    _startTimeString("00:00:00.000"),
    _lineInTimeLine(-1),
    _areAllConditionsValid(false),
    _currentExecution(nullptr),
    _endTime(startTime)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (model != nullptr)
    {
        if (startTime >= 0)
        {
            int hours = startTime / NB_MILLI_SECONDS_IN_ONE_HOUR;
            int minutes = (startTime - hours * NB_MILLI_SECONDS_IN_ONE_HOUR) / NB_MILLI_SECONDS_IN_ONE_MINUTE;
            int seconds = (startTime - hours * NB_MILLI_SECONDS_IN_ONE_HOUR - minutes * NB_MILLI_SECONDS_IN_ONE_MINUTE) / 1000;
            int milliseconds = startTime % 1000;

            _startTimeString = QString::number(hours).rightJustified(2, '0') + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0') + "." + QString::number(milliseconds).leftJustified(3, '0');
        }

        // Update flag
        setareAllConditionsValid(model->isValid());

        // Set the action model
        setmodelM(model);

        // Create the first (view model of) action execution
        _createActionExecution(0);
    }

    //
    // Init the revert timer
    //
    _timerToReverse = new QTimer();
    _timerToReverse->setSingleShot(true);
    connect(_timerToReverse, &QTimer::timeout, this, &ActionVM::_onTimeout_ReserseAction);

    //
    // Init the rearm timer
    //
    _timerToRearm = new QTimer();
    _timerToRearm->setSingleShot(true);
    connect(_timerToRearm, &QTimer::timeout, this, &ActionVM::_onTimeout_RearmAction);

}


/**
 * @brief Destructor
 */
ActionVM::~ActionVM()
{
    // Reset current execution
    setcurrentExecution(nullptr);

    // Delete all executions
    _executionsList.deleteAllItems();

    // Reset model of action
    if (_modelM != nullptr) {
        setmodelM(nullptr);
    }

    // Remove revert timer
    if (_timerToReverse != nullptr)
    {
        disconnect(_timerToReverse, nullptr, this, nullptr);
        _timerToReverse->stop();
        delete _timerToReverse;
        _timerToReverse = nullptr;
    }

    // Remove rearm timer
    if (_timerToRearm != nullptr)
    {
        disconnect(_timerToRearm, nullptr, this, nullptr);
        _timerToRearm->stop();
        delete _timerToRearm;
        _timerToRearm = nullptr;
    }
}


/**
 * @brief Setter for property "Model"
 * @param value
 */
void ActionVM::setmodelM(ActionM* value)
{
    if (_modelM != value)
    {
        if (_modelM != nullptr)
        {
            disconnect(_modelM, &ActionM::isValidChanged, this, &ActionVM::_onIsValidChangedInModel);

            // End time re-evaluation disconnection
            disconnect(_modelM, &ActionM::validityDurationChanged, this, &ActionVM::_onValidityDurationChanged);
            disconnect(_modelM, &ActionM::validityDurationTypeChanged, this, &ActionVM::_onValidityDurationChanged);
        }

        _modelM = value;

        if (_modelM != nullptr)
        {
            connect(_modelM, &ActionM::isValidChanged, this, &ActionVM::_onIsValidChangedInModel);

            // End time re-evaluation connection
            connect(_modelM, &ActionM::validityDurationChanged, this, &ActionVM::_onValidityDurationChanged);
            connect(_modelM, &ActionM::validityDurationTypeChanged, this, &ActionVM::_onValidityDurationChanged);
        }

        // Compute the new end time
        _computeEndTime();

        Q_EMIT modelMChanged(value);
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

            if (successHours && successMinutes && successSeconds)
            {
                int totalMilliseconds = (hours * NB_MILLI_SECONDS_IN_ONE_HOUR) + (minutes * NB_MILLI_SECONDS_IN_ONE_MINUTE) + (seconds * 1000);
                if (successMilliSeconds)
                {
                    totalMilliseconds += milliSeconds;
                    setstartTime(totalMilliseconds);
                    //qDebug() << "Start Time =" << _startTime;
                }
                else {
                    setstartTime(-1);
                    if (_modelM != nullptr) {
                        qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _modelM->name();
                    }
                }
            }
            else {
                setstartTime(-1);
                if (_modelM != nullptr) {
                    qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _modelM->name();
                }
            }
        }
        else {
            setstartTime(-1);
            if (_modelM != nullptr) {
                qCritical() << "Wrong 'Start Time':" << _startTimeString << "for action" << _modelM->name();
            }
        }

        // Compute the new endtime
        _computeEndTime();

        Q_EMIT startTimeStringChanged(value);
    }
}


/**
 * @brief Notify our action that its effects has been executed
 * @param currentTimeInMilliSeconds
 */
void ActionVM::effectsExecuted(int currentTimeInMilliSeconds)
{
    if (_currentExecution != nullptr)
    {
        _currentExecution->setisExecuted(true);

        // Shall revert
        if (_modelM->shallRevertAfterTime() && (_modelM->revertAfterTime() > 0))
        {
            // Launch timer for revert
            _timerToReverse->start(_modelM->revertAfterTime());
        }
        else if (_modelM->shallRevertWhenValidityIsOver())
        {
            // Launch timer for revert
            _timerToReverse->start(_endTime - currentTimeInMilliSeconds);
        }
        // No reverse
        else
        {
            setcurrentExecution(nullptr);

            // Shall rearm
            if (_modelM->shallRearm())
            {
                _timerToRearm->start(_modelM->rearmAfterTime());
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
    if (_currentExecution != nullptr)
    {
        setcurrentExecution(nullptr);

        // Shall rearm
        if (_modelM->shallRearm() && ((currentTimeInMilliSeconds < _endTime) || (_endTime == -1)) )
        {
            _timerToRearm->start(_modelM->rearmAfterTime());
        }
    }
}


/**
 * @brief Rearm our action
 * @param currentTimeInMilliSeconds
 */
void ActionVM::rearmCurrentActionExecution(int currentTimeInMilliSeconds)
{
    qDebug() << "Rearm current action execution at" << currentTimeInMilliSeconds;

    // Create a new (view model of) action execution
    _createActionExecution(currentTimeInMilliSeconds);
}


/**
 * @brief Delay the current execution of our action
 * @param currentTimeInMilliSeconds
 */
void ActionVM::delayCurrentExecution(int currentTimeInMilliSeconds)
{
    if (_currentExecution != nullptr)
    {
        // Add 1 ms
        _currentExecution->setexecutionTime(currentTimeInMilliSeconds - _startTime + 1);

        if ((_modelM != nullptr) && _modelM->shallRevert() && _modelM->shallRevertAfterTime())
        {
            // FIXME TODO: check reverseTime is not after validityDuration

            if (_modelM->revertAfterTime() > -1) {
                _currentExecution->setreverseTime(currentTimeInMilliSeconds - _startTime + _modelM->revertAfterTime());
            }
            else {
                qWarning() << "Action" << _modelM->name() << "Shall revert after time but 'revert after time' is not defined";
            }
        }
    }
}



/**
  * @brief Initialize the action view model at a specific time
  * @param time when to initialize the action VM
  */
void ActionVM::resetDataFrom(int time)
{
    if (_modelM != nullptr)
    {
        // Update the conditions validation flag
        if (_modelM->isConnected())
        {
            setareAllConditionsValid(_modelM->isValid());
        }
        else {
            setareAllConditionsValid(false);
        }

        // Reset the current action execution
        setcurrentExecution(nullptr);

        // Get the relative time to the action view model
        int relativeTime = time - _startTime;

        // Delete each action execution which is in the FUTURE
        for (ActionExecutionVM* actionExecution : _executionsList.toList())
        {
            // This action execution is in the future
            if ((actionExecution != nullptr)
                    && ( (actionExecution->executionTime() >= relativeTime) || (actionExecution->shallRevert() && (actionExecution->reverseTime() >= relativeTime)) ))
            {
                _executionsList.remove(actionExecution);

                // Free memory
                delete actionExecution;
            }
        }

        // If the action is empty or the rearm is asked, we can create a new action execution
        if (_executionsList.isEmpty() || _modelM->shallRearm())
        {
            // Create the first action execution
            if (time <= _startTime)
            {
                // Create the first (view model of) action execution
                _createActionExecution(0);
            }
            else {
                // Create one default action execution in the current validation duration
                _createActionExecution(relativeTime);
            }
        }
    }
}


/**
 * @brief Slot when the flag "is valid" changed in the model of action
 * @param isValid
 */
void ActionVM::_onIsValidChangedInModel(bool isValid)
{
    setareAllConditionsValid(isValid);

    // FIXME TODO: connect to areAllConditionsValidChanged(); to execute the corresponding action
}


/**
 * @brief Slot called when the validity duration changed and when the type of the validity duration changed
 */
void ActionVM::_onValidityDurationChanged()
{
    _computeEndTime();
}


/**
 * @brief Called when our timer time out to handle the action reversion
 */
void ActionVM::_onTimeout_ReserseAction()
{
    if (_currentExecution != nullptr)
    {
        //qDebug() << "Timeout: emit 'Revert Action'";

        // Emit the signal to send the action reversion
        Q_EMIT revertAction(_currentExecution);
    }
}


/**
 * @brief Called when our timer time out to handle the action rearm
 */
void ActionVM::_onTimeout_RearmAction()
{
    //qDebug() << "Timeout: emit 'Rearm Action'";

    // Emit the signal to send the action rearm
    Q_EMIT rearmAction();
}


/**
 * @brief Compute the endTime according to the action model and its type
 */
void ActionVM::_computeEndTime()
{
    int endTime = _startTime;
    if (_modelM != nullptr)
    {
        int itemDurationTime = 0;
        if (_modelM->validityDurationType() == ValidationDurationTypes::FOREVER) {
            endTime = -1;
        }
        else if (_modelM->validityDurationType() == ValidationDurationTypes::CUSTOM) {
            itemDurationTime = _modelM->validityDuration();
        }

        // Compare with the time before revert if selected
        if (_modelM->shallRevertAfterTime() && (_modelM->revertAfterTime() > itemDurationTime)) {
            itemDurationTime = _modelM->revertAfterTime();
        }

        // If not forever, add duration
        if (endTime != -1) {
            endTime += itemDurationTime;
        }
    }

    // Set the new value
    setendTime(endTime);
}


/**
 * @brief Create a new (view model of) action execution
 * @param startTime relative to our view model of action
 */
void ActionVM::_createActionExecution(int startTime)
{
    if (_modelM != nullptr)
    {
        // No revert by default: reverse time = start time
        int reverseTime = startTime;

        if (_modelM->shallRevert())
        {
            // Shall revert when validity is over
            if (_modelM->shallRevertWhenValidityIsOver())
            {
                if (_modelM->validityDuration() > -1) {
                    reverseTime = startTime + _modelM->validityDuration();
                }
                else {
                    qWarning() << "Action" << _modelM->name() << "Shall revert when validity is over but 'validity duration' is not defined";
                }
            }
            // Shall revert after time
            else if (_modelM->shallRevertAfterTime())
            {
                if (_modelM->revertAfterTime() > -1) {
                    reverseTime = startTime + _modelM->revertAfterTime();
                }
                else {
                    qWarning() << "Action" << _modelM->name() << "Shall revert after time but 'revert after time' is not defined";
                }
            }
        }

        // Create a new (view model of) action execution
        ActionExecutionVM* actionExecution = new ActionExecutionVM(_modelM->shallRevert(), startTime, reverseTime, this);

        _executionsList.append(actionExecution);

        //qDebug() << "_createActionExecution: action" << _modelM->name() << "has" << _executionsList.count() << "executions";

        // Set as current
        setcurrentExecution(actionExecution);
    }
}

