/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "abstractscenariocontroller.h"

#include <QDebug>
#include <misc/ingescapeutils.h>

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
AbstractScenarioController::AbstractScenarioController(IngeScapeModelManager* modelManager,
                                                       JsonHelper* jsonHelper,
                                                       QObject *parent) : QObject(parent),
    _selectedAction(nullptr),
    _selectedActionVMInTimeline(nullptr),
    _linesNumberInTimeLine(MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE),
    _isPlaying(false),
    _currentTime(QTime::fromMSecsSinceStartOfDay(0)),
    _nextActionToActivate(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper),
    _allActionNames(QStringList()),
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    // Configure our filtered list of "actionVM in timeline"
    _filteredListActionsInTimeLine.setSourceModel(&_actionsInTimeLine);


    // Add the first timeline line
    I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
    sortedListOfActionVM->setSortProperty("startTime");
    _hashFromLineIndexToSortedViewModelsOfAction.insert(0, sortedListOfActionVM);

    // Set the sort property for the actionVM in the time line
    _actionsInTimeLine.setSortProperty("startTime");

    // Set the sort property for the actionsVM to evaluate
    _listOfActionsToEvaluate.setSortProperty("startTime");

    // Set the sort property for the list of active actions
    _listOfActiveActions.setSortProperty("startTime");


    //
    // Init the timers
    //
    _timerToExecuteActions.setSingleShot(true);
    connect(&_timerToExecuteActions, &QTimer::timeout, this, &AbstractScenarioController::_onTimeout_ExecuteActions);

    _timerToRegularlyDelayActions.setInterval(INTERVAL_DELAY_ACTIONS);
    _timerToRegularlyDelayActions.setSingleShot(true);
    connect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &AbstractScenarioController::_onTimeout_DelayOrExecuteActions);
}


/**
 * @brief Destructor
 */
AbstractScenarioController::~AbstractScenarioController()
{
    disconnect(&_timerToExecuteActions, &QTimer::timeout, this, &AbstractScenarioController::_onTimeout_ExecuteActions);
    _timerToExecuteActions.stop();

    disconnect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &AbstractScenarioController::_onTimeout_DelayOrExecuteActions);
    _timerToRegularlyDelayActions.stop();

    // Clear the current scenario
    clearScenario();

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
  * @brief Import the scenario from JSON
  * @param jsonScenario
  */
void AbstractScenarioController::importScenarioFromJson(QJsonObject jsonScenario)
{
    if ((_modelManager != nullptr) && (_jsonHelper != nullptr))
    {
        // Get the hash table from a name to the group of agents with this name
        QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped = _modelManager->getHashTableFromNameToAgentsGrouped();

        // Create a model of scenario (actions in the list, in the palette and in the timeline) from JSON
        ScenarioM* scenarioToImport = _jsonHelper->createModelOfScenarioFromJSON(jsonScenario, hashFromNameToAgentsGrouped);
        if (scenarioToImport != nullptr)
        {
            // Append the list of actions
            if (!scenarioToImport->actionsList()->isEmpty())
            {
                // Add each action to our list
                for (ActionM* actionM : scenarioToImport->actionsList()->toList())
                {
                    if ((actionM != nullptr) && (_modelManager->getActionWithId(actionM->uid()) == nullptr))
                    {
                        // Add the action to the model manager
                        _modelManager->storeNewAction(actionM);

                        // Add the action to the list
                        _actionsList.append(actionM);

                        // Add action name
                        if (!_allActionNames.contains(actionM->name())) {
                            _allActionNames.append(actionM->name());
                        }
                    }
                }
            }

            // Append the list of actions in timeline
            if (!scenarioToImport->actionsInTimelineList()->isEmpty())
            {
                // Add each actionVM in to the right line of our timeline
                for (ActionVM* actionVM : scenarioToImport->actionsInTimelineList()->toList())
                {
                    if ((actionVM != nullptr) && (actionVM->modelM() != nullptr))
                    {
                        int actionId = actionVM->modelM()->uid();
                        int lineIndexInTimeLine = actionVM->lineInTimeLine();

                        // Add the new action VM to our hash table
                        QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);
                        listOfActionVM.append(actionVM);
                        _hashFromUidToViewModelsOfAction.insert(actionId, listOfActionVM);

                        // Get the "Sorted" list of view models of action with the index of the line (in the time line)
                        I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndexInTimeLine);

                        // Add the action VM to the line
                        if (sortedListOfActionVM != nullptr) {
                            sortedListOfActionVM->append(actionVM);
                        }
                        else
                        {
                            // Create a new list and add to the hash table
                            sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
                            sortedListOfActionVM->setSortProperty("startTime");
                            sortedListOfActionVM->append(actionVM);

                            _hashFromLineIndexToSortedViewModelsOfAction.insert(lineIndexInTimeLine, sortedListOfActionVM);
                        }

                        _actionsInTimeLine.append(actionVM);

                        // Increment the line number if necessary
                        if (_linesNumberInTimeLine < lineIndexInTimeLine + 2) {
                            setlinesNumberInTimeLine(lineIndexInTimeLine + 2);
                        }
                    }
                }
            }

            delete scenarioToImport;
        }
    }
}


/**
 * @brief Import the executed actions for a scenario from JSON
 * @param byteArrayOfJson
 */
void AbstractScenarioController::importExecutedActionsFromJson(QByteArray byteArrayOfJson)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
    if (jsonDocument.isObject())
    {
        QJsonObject jsonRoot = jsonDocument.object();
        if (jsonRoot.contains("Actions"))
        {
            QJsonValue jsonExecutedActions = jsonRoot.value("Actions");
            if (jsonExecutedActions.isArray())
            {
                for (QJsonValue iterator : jsonExecutedActions.toArray())
                {
                    QJsonObject jsonExecutedAction = iterator.toObject();

                    if (jsonExecutedAction.contains("id_action")
                            && jsonExecutedAction.contains("line_action")
                            && jsonExecutedAction.contains("time_action"))
                    {
                        int actionId = jsonExecutedAction.value("id_action").toInt();
                        int lineIndexInTimeLine = jsonExecutedAction.value("line_action").toInt();
                        int executionTime = jsonExecutedAction.value("time_action").toInt();

                        qDebug() << "Executed action" << actionId << "on line" << lineIndexInTimeLine << "at" << executionTime << "ms";

                        /*// Get the model of action with its (unique) id
                        ActionM* action = _modelManager->getActionWithId(actionId);
                        if (action != nullptr)
                        {

                        }*/

                        // Get the list of view models of action with its (unique) id
                        QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);
                        if (!listOfActionVM.isEmpty())
                        {
                            for (ActionVM* actionVM : listOfActionVM)
                            {
                                if ((actionVM != nullptr) && (actionVM->lineInTimeLine() == lineIndexInTimeLine))
                                {
                                    qDebug() << "Action VM found !";

                                    // FIXME TODO
                                    // Compare actionVM->startTime() and executionTime

                                    /*if (actionVM->executionsList()->count() == 1)
                                    {
                                        actionVM->executionsList()->at(0);
                                    }
                                    else
                                    {

                                    }*/

                                    // Create a new (view model of) action execution
                                    // startTime relative to our view model of action
                                    //void createActionExecution(int startTime);
                                    break;
                                }
                            }
                        }

                        // Get the "Sorted" list of view models of action with the index of the line (in the time line)
                        /*I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndexInTimeLine);
                        if (sortedListOfActionVM != nullptr)
                        {
                            // FIXME TODO importExecutedActionsFromJson
                        }*/
                    }
                }
            }
        }
    }
}


/**
  * @brief Remove an action from the list and delete it
  * @param action
  */
void AbstractScenarioController::deleteAction(ActionM* action)
{
    if ((action != nullptr) && (_modelManager != nullptr))
    {
        int actionId = action->uid();

        // Remove action from the timeline if exists
        QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);
        if (!listOfActionVM.isEmpty())
        {
            for (ActionVM* actionVM : listOfActionVM)
            {
                if (actionVM != nullptr) {
                    removeActionVMfromTimeLine(actionVM);
                }
            }
            _hashFromUidToViewModelsOfAction.remove(actionId);
        }

        // Unselect our action if needed
        if (_selectedAction == action) {
            setselectedAction(nullptr);
        }

        // Remove the action form the list
        _actionsList.remove(action);

        // Remove the name form the list
        //_allActionNames.removeAll(action->name());
        _allActionNames.removeOne(action->name());

        // Delete the model of action
        _modelManager->deleteAction(action);
    }
}


/**
 * @brief Remove an action VM from the time line and delete it
 * @param action view model
 */
void AbstractScenarioController::removeActionVMfromTimeLine(ActionVM* actionVM)
{
    if ((actionVM != nullptr) && (actionVM->modelM() != nullptr) && _actionsInTimeLine.contains(actionVM))
    {
        _actionsInTimeLine.remove(actionVM);

        int actionId = actionVM->modelM()->uid();

        // Get the list of view models of action with its (unique) id
        QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);

        if (!listOfActionVM.isEmpty())
        {
            listOfActionVM.removeOne(actionVM);
            if (listOfActionVM.isEmpty()) {
                _hashFromUidToViewModelsOfAction.remove(actionId);
            }
            else {
                _hashFromUidToViewModelsOfAction.insert(actionId, listOfActionVM);
            }

            // Remove the action VM from the timeline hash table by line index
            if (actionVM->lineInTimeLine() > -1)
            {
                int lineIndex = actionVM->lineInTimeLine();

                // Get the "Sorted" list of view models of action with the index of the line (in the time line)
                I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndex);
                if ((sortedListOfActionVM != nullptr) && sortedListOfActionVM->contains(actionVM))
                {
                    // Remove the view model of action
                    sortedListOfActionVM->remove(actionVM);

                    // Check if the list is empty to remove the line index
                    if (sortedListOfActionVM->isEmpty())
                    {
                        // We delete the last line, reduce the number of displayed lines in the limite of MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE
                        if ((_linesNumberInTimeLine > MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE) && (lineIndex + 2 == _linesNumberInTimeLine))
                        {
                            int nbOfDecrement = 1;

                            // Delete the last line, lets check if we can reduce the number of displayed lines
                            for (int i = lineIndex - 1; i >= 0; i--)
                            {
                                if (!_hashFromLineIndexToSortedViewModelsOfAction.contains(i)) {
                                    nbOfDecrement++;
                                }
                                else {
                                    break;
                                }
                            }

                            // Decrement the number of lines
                            if (_linesNumberInTimeLine - nbOfDecrement < MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE) {
                                setlinesNumberInTimeLine(MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE);
                            }
                            else {
                                setlinesNumberInTimeLine(_linesNumberInTimeLine - nbOfDecrement);
                            }
                        }

                        _hashFromLineIndexToSortedViewModelsOfAction.remove(lineIndex);
                        delete sortedListOfActionVM;
                    }
                }
            }
        }

        // Free memory
        delete actionVM;
    }
}


/**
 * @brief Play (or Resume) the timeline (current scenario)
 */
void AbstractScenarioController::playOrResumeTimeLine()
{
    // Udpate flag "is Playing"
    setisPlaying(true);

    Q_EMIT timeLineStateUpdated(PLAY);

    // Initialize actions and play (or resume) the scenario
    // (make connections for actions conditions and start the action evaluation timer)
    _initActionsAndPlayOrResumeScenario();
}


/**
 * @brief Pause the timeline (current scenario)
 */
void AbstractScenarioController::pauseTimeLine()
{
    // Udpate flag "is Playing"
    setisPlaying(false);

    Q_EMIT timeLineStateUpdated(PAUSE);

    // Pause the scenario and associated actions
    // (disconnect actions conditions and stop the action evaluation timer)
    _pauseScenarioAndActions();
}


/**
 * @brief Stop/Reset the timeline (current scenario)
 */
void AbstractScenarioController::stopTimeLine()
{
    // Udpate flag "is Playing"
    setisPlaying(false);

    Q_EMIT timeLineStateUpdated(RESET);

    // Pause the scenario and associated actions (disconnect actions conditions and stop the action evaluation timer)
    _pauseScenarioAndActions();

    // Reset current time (to 00:00:00.000)
    setcurrentTime(QTime::fromMSecsSinceStartOfDay(0));
}


/**
 * @brief Clear the current scenario
 * (clear the list of actions in the list, in the palette and in the timeline)
 */
void AbstractScenarioController::clearScenario()
{
    qInfo() << "Abstract Scenario Controller: Clear the current scenario";

    // Stop/Reset the timeline (current scenario)
    stopTimeLine();

    // Clean-up current selection
    setselectedAction(nullptr);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

    // Clean-up current selection
    setselectedActionVMInTimeline(nullptr);

    // Clear the list of actions
    _actionsList.clear();
    // NB: We have to call "_modelManager->deleteAllActions();" after our method "clearScenario()" to free memory

    // Clear names list
    _allActionNames.clear();
}


/**
 * @brief Execute all effects of the action
 * Activate (connect) the mapping if necessary
 * Notify the recorder that the action has been executed
 * @param action
 * @param lineInTimeLine
 */
void AbstractScenarioController::executeEffectsOfAction(ActionM* action, int lineInTimeLine)
{
    if ((action != nullptr) && !action->effectsList()->isEmpty())
    {
        // Active the mapping if needed
        if ((_modelManager != nullptr) && !_modelManager->isMappingConnected()) {
            _modelManager->setisMappingConnected(true);
        }

        QString commandAndParameters = QString("%1=%2|%3").arg(command_ExecutedAction,
                                                               QString::number(action->uid()),
                                                               QString::number(lineInTimeLine));

        // Emit the signal "Command asked to Recorder"
        Q_EMIT commandAskedToRecorder(commandAndParameters);

        // Execute all effects of the action
        _executeEffectsOfAction(action);
    }
}


/**
 * @brief Slot called when an action must be reverted
 * @param actionExecution
 */
void AbstractScenarioController::onRevertAction(ActionExecutionVM* actionExecution)
{
    ActionVM* actionVM = qobject_cast<ActionVM*>(sender());
    if ((actionVM != nullptr) && (actionExecution != nullptr))
    {
        int currentTimeInMilliSeconds = actionVM->startTime() + actionExecution->reverseTime();

        // Execute reverse effects of the action
        _executeReverseEffectsOfAction(actionExecution);

        // Notify the action that its reverse effects has been executed
        actionVM->reverseEffectsExecuted(currentTimeInMilliSeconds);

        // The revert action has been done after the end of the action validity
        if ((actionVM->endTime() >= 0) && (currentTimeInMilliSeconds >= actionVM->endTime()))
        {
            // ...we remove the current execution
            actionVM->setcurrentExecution(nullptr);

            // Remove from the list of "active" actions
            disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
            disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
        }
    }
}


/**
  * @brief Slot called when an action must be rearmed
  */
void AbstractScenarioController::onRearmAction()
{
    ActionVM* actionVM = qobject_cast<ActionVM*>(sender());
    if (actionVM != nullptr)
    {
        // Initialize the start time
        int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay() - actionVM->startTime();

        // Define the exact start time for the next action execution
        if (!actionVM->executionsList()->isEmpty())
        {
            ActionExecutionVM* lastActionExecution = actionVM->executionsList()->at(actionVM->executionsList()->count() -1);
            if (lastActionExecution != nullptr)
            {
                currentTimeInMilliSeconds = lastActionExecution->executionTime();

                if (lastActionExecution->shallRevert()) {
                    currentTimeInMilliSeconds += actionVM->modelM()->revertAfterTime();
                }
                if (actionVM->modelM()->shallRearm()) {
                    currentTimeInMilliSeconds += actionVM->modelM()->rearmAfterTime();
                }
            }
        }

        // Rearm the action
        actionVM->rearmCurrentActionExecution(currentTimeInMilliSeconds);

        // The rearm action has been done after the end of the action validity
        if ((actionVM->endTime() >= 0) && (currentTimeInMilliSeconds >= actionVM->endTime()))
        {
            // ...we remove the current execution
            actionVM->setcurrentExecution(nullptr);

            // Remove from the list of "active" actions
            disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
            disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
        }
    }
}


/**
 * @brief Slot called when the time line range changed
 * @param startTimeInMilliseconds
 * @param endTimeInMilliseconds
 */
void AbstractScenarioController::onTimeRangeChanged(int startTimeInMilliseconds, int endTimeInMilliseconds)
{
    _filteredListActionsInTimeLine.setTimeRange(startTimeInMilliseconds, endTimeInMilliseconds);
}


/**
 * @brief Slot called when we receive the command "run action" from a recorder
 * @param actionID
 */
void AbstractScenarioController::onRunAction(QString actionID)
{
    bool success = false;
    int id = actionID.toInt(&success);

    if (success && (_modelManager != nullptr))
    {
        // Get the model of action with this (unique) id
        ActionM* action = _modelManager->getActionWithId(id);
        if (action != nullptr)
        {
            // Execute all effects of the action
            _executeEffectsOfAction(action);
        }
    }
}


/**
 * @brief Slot called when an action has to be executed
 * @param action
 */
void AbstractScenarioController::onExecuteAction(ActionM* action)
{
    if (action != nullptr)
    {
        // Execute all effects of the action
        // Activate (connect) the mapping if necessary
        // Notify the recorder that the action has been executed
        executeEffectsOfAction(action, 0);

        // Execute all effects of the action
        //_executeEffectsOfAction(action);
    }
}


/**
 * @brief Called when our timer time out to handle the scenario and execute actions
 */
void AbstractScenarioController::_onTimeout_ExecuteActions()
{
    if (!_listOfActionsToEvaluate.isEmpty())
    {
        //qDebug() << "Timeout ExecuteActions" << _currentTime.toString();

        int currentTimeInMilliSeconds = 0;
        if (_nextActionToActivate != nullptr) {
            currentTimeInMilliSeconds = _nextActionToActivate->startTime();
        }
        ActionVM* tmpNextActionToActivate = nullptr;

        //qDebug() << "Time in ms of next action" << currentTimeInMilliSeconds;

        // Traverse the list of actions to evaluate
        for (ActionVM* actionVM : _listOfActionsToEvaluate.toList())
        {
            if ((actionVM != nullptr) && (actionVM->modelM() != nullptr))
            {
                // Current time is after the start time of the action
                if (actionVM->startTime() <= currentTimeInMilliSeconds)
                {
                    ActionExecutionVM* actionExecution = actionVM->currentExecution();

                    // Check if an action execution exists and has not already been executed
                    if ((actionExecution != nullptr) && !actionExecution->isExecuted())
                    {
                        if (actionVM->modelM()->isValid())
                        {
                            //qDebug() << "Execute action" << actionVM->modelM()->name();

                            // Execute action
                            _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);
                        }

                        // The action has a (validation) duration
                        if (actionVM->modelM()->validityDurationType() != ValidationDurationTypes::IMMEDIATE)
                        {
                            // Add to the list of active actions
                            _listOfActiveActions.append(actionVM);
                        }
                        else
                        {
                            // FIXME: Disconnect before removing from the list
                            if (!actionVM->modelM()->shallRevert() && !actionVM->modelM()->shallRearm())
                            {
                                disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
                                disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
                            }
                        }
                    }

                    // Remove from the list of actions to evaluate
                    _listOfActionsToEvaluate.remove(actionVM);
                }
                // Action is in the future
                else
                {
                    // Set the next action to launch
                    tmpNextActionToActivate = actionVM;

                    break;
                }
            }
        }

        if (tmpNextActionToActivate != nullptr)
        {
            // Set the next action to activate
            setnextActionToActivate(tmpNextActionToActivate);

            //int deltaMS = _nextActionToActivate->startTime() - currentTimeInMilliSeconds;
            //qDebug() << "Set NEXT action to" << _nextActionToActivate->modelM()->name() << "in" << deltaMS << "ms";

            // Set the timer to the next action
            _timerToExecuteActions.start(_nextActionToActivate->startTime() - currentTimeInMilliSeconds);
        }
        else {
            // Reset the next action to activate
            setnextActionToActivate(nullptr);
        }
    }
}


/**
 * @brief Called at each interval of our timer to delay actions when their conditions are not valid or execute them otherwise
 */
void AbstractScenarioController::_onTimeout_DelayOrExecuteActions()
{
    // Get the current time of the day
    int currentTimeOfDayInMS = QTime::currentTime().msecsSinceStartOfDay();

    // Update the current time of our scenario (from the beginning of our scenario)
    setcurrentTime(_currentTime.addMSecs(currentTimeOfDayInMS - _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut));

    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    //qDebug() << "_onTimeout_DelayOrExecuteActions" << currentTimeInMilliSeconds << "_listOfActiveActions" << _listOfActiveActions.count();

    // Traverse the list of active actions (the current time (is) was between start time and end time of these actions)
    for (ActionVM* actionVM : _listOfActiveActions.toList())
    {
        if ((actionVM != nullptr) && (actionVM->modelM() != nullptr))
        {
            ActionExecutionVM* actionExecution = actionVM->currentExecution();
            if (actionExecution != nullptr)
            {
                // Current time is after the start time of the action
                // the action has not finished or is forever
                if ( (actionVM->startTime() < currentTimeInMilliSeconds)
                     && ((actionVM->endTime() == -1) || (actionVM->endTime() > currentTimeInMilliSeconds)) )
                {
                    // Not already executed
                    if (!actionExecution->isExecuted())
                    {
                        // Delay the current execution of this action
                        if (!actionVM->modelM()->isValid())
                        {
                            //qDebug() << "Delay the action" << actionVM->modelM()->name();

                            actionVM->delayCurrentExecution(currentTimeInMilliSeconds);
                        }
                        else
                        {
                            qDebug() << "Execute the action" << actionVM->modelM()->name();

                            _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);
                        }
                    }
                }
                // Current time is after the end time of the action (or the action has no validity duration --> Immediate)
                else if ((actionVM->endTime() > -1) && (actionVM->endTime() <= currentTimeInMilliSeconds))
                {
                    if (!actionVM->timerToReverse()->isActive())
                    {
                        // the action has never been executed if there is only one Action Execution (the initial one) and its execution flag is false
                        if ((actionVM->executionsList()->count() <= 1) && !actionExecution->isExecuted()) {
                            actionExecution->setneverExecuted(true);
                        }
                        else {
                            actionExecution->setneverExecuted(false);
                        }

                        // If there is at least another execution for this action...
                        if ( (actionVM->executionsList()->count() > 1)
                             || (actionExecution->neverExecuted() && (actionVM->modelM()->validityDurationType() == ValidationDurationTypes::CUSTOM)) )
                        {
                            // ...we remove the current execution
                            actionVM->setcurrentExecution(nullptr);

                            // only if the execution has not be done
                            if (!actionExecution->isExecuted())
                            {
                                actionVM->executionsList()->remove(actionExecution);

                                // Print for DEBUG
                                //qDebug() << "_onTimeout_DelayOrExecuteActions: execution removed in" << actionVM->modelM()->name() << "(" << actionVM->executionsList()->count() << ")";

                                // Free memory
                                delete actionExecution;
                            }
                        }
                    }

                    if (!actionVM->timerToReverse()->isActive())
                    {
                        // Disconnect the revert action signal
                        disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
                    }
                    if (!actionVM->timerToRearm()->isActive())
                    {
                        // Disconnect the rearm action signal
                        disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
                    }

                    _listOfActiveActions.remove(actionVM);
                }
            }
        }
    }

    // Update the time of the day
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut = currentTimeOfDayInMS;

    //qDebug() << "_onTimeout_DelayOrExecuteActions:" << _currentTime << QTime::currentTime();

    if (_isPlaying) {
        _timerToRegularlyDelayActions.start(INTERVAL_DELAY_ACTIONS);
    }
}


/**
 * @brief Initialize actions and play (or resume) the scenario
 * - make connections for actions conditions
 * - start the action evaluation timer
 */
void AbstractScenarioController::_initActionsAndPlayOrResumeScenario()
{
    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    // Active the mapping if needed
    if ((_modelManager != nullptr) && !_modelManager->isMappingConnected()) {
        _modelManager->setisMappingConnected(true);
    }

    // Disconnect from signals
    for (ActionVM* actionVM : _listOfActionsToEvaluate.toList())
    {
        disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
        disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
    }

    // Reset lists and next action to activate
    _listOfActionsToEvaluate.clear();
    _listOfActiveActions.clear();
    setnextActionToActivate(nullptr);

    // "Future" actions previously added to the time line (on the first line) from the "palette" must be removed
    QList<ActionVM*> futureActionsOnFirstLineToRemove;

    ActionVM* tmpNextActionToActivate = nullptr;

    // Look for the current and futures actions
    for (ActionVM* actionVM : _actionsInTimeLine.toList())
    {
        // Ends in the future
        if ((actionVM != nullptr) && ((actionVM->endTime() > currentTimeInMilliSeconds) || (actionVM->endTime() == -1)) )
        {
            // First line is reserved for live insertions from the palette
            if (actionVM->lineInTimeLine() == 0)
            {
                // We have to remove this action that ends in the future
                futureActionsOnFirstLineToRemove.append(actionVM);
            }
            else
            {
                // Connect on the action revert signal
                connect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);
                connect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);

                // Initialize the action view model at a specific time.
                actionVM->resetDataFrom(currentTimeInMilliSeconds);

                // Current time is after the start time of the action
                if (actionVM->startTime() <= currentTimeInMilliSeconds)
                {
                    // Add to the list of active actions
                    _listOfActiveActions.append(actionVM);
                }
                else
                {
                    // Check the next action to activate
                    if ( (tmpNextActionToActivate == nullptr)
                         || ((tmpNextActionToActivate != nullptr) && (tmpNextActionToActivate->startTime() > actionVM->startTime())) )
                    {
                        tmpNextActionToActivate = actionVM;
                    }

                    // Add our action
                    _listOfActionsToEvaluate.append(actionVM);
                }
            }
        }
    }

    if (!futureActionsOnFirstLineToRemove.isEmpty())
    {
        // Remove each "future" action previously added to the time line (on the first line) from the "palette"
        for (ActionVM* actionVM : futureActionsOnFirstLineToRemove) {
            removeActionVMfromTimeLine(actionVM);
        }
    }

    if (tmpNextActionToActivate != nullptr) {
        setnextActionToActivate(tmpNextActionToActivate);
    }

    // Initialize the connections for conditions of all actions
    for (ActionM* actionM : _actionsList.toList())
    {
        if (actionM != nullptr) {
            actionM->initializeConditionsConnections();
        }
    }

    // Save the time of the day
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut = QTime::currentTime().msecsSinceStartOfDay();

    //qDebug() << "Play (or Resume) Scenario:" << _currentTime << QTime::currentTime();

    // Start timers
    // init the timer with the time of the next action execution
    if (_nextActionToActivate != nullptr) {
        _timerToExecuteActions.start(_nextActionToActivate->startTime() - currentTimeInMilliSeconds);
    }

    _timerToRegularlyDelayActions.start(INTERVAL_DELAY_ACTIONS);
}


/**
 * @brief Pause the scenario and associated actions
 * - disconnect actions conditions
 * - stop the action evaluation timer
 */
void AbstractScenarioController::_pauseScenarioAndActions()
{
    // Stop timers
    if (_timerToExecuteActions.isActive()) {
        _timerToExecuteActions.stop();
    }
    if (_timerToRegularlyDelayActions.isActive()) {
        _timerToRegularlyDelayActions.stop();
    }

    // De-active timers for revert and rearm
    for (ActionVM* actionVM : _listOfActiveActions.toList())
    {
        if (actionVM != nullptr)
        {
            // Disconnect revert action
            if (actionVM->timerToReverse()->isActive()) {
                actionVM->timerToReverse()->stop();
            }
            disconnect(actionVM, &ActionVM::revertAction, this, &AbstractScenarioController::onRevertAction);

            // Disconnect rearm action
            if (actionVM->timerToRearm()->isActive()) {
                actionVM->timerToRearm()->stop();
            }
            disconnect(actionVM, &ActionVM::rearmAction, this, &AbstractScenarioController::onRearmAction);
        }
    }

    // Reset the next action VM to activate
    setnextActionToActivate(nullptr);

    // Reset the connections for conditions of all actions
    for (ActionM* actionM : _actionsList.toList())
    {
        if (actionM != nullptr) {
            actionM->resetConditionsConnections();
        }
    }
}


/**
 * @brief Execute all effects of an action
 * @param action
 */
void AbstractScenarioController::_executeEffectsOfAction(ActionM* action)
{
    if ((action != nullptr) && !action->effectsList()->isEmpty())
    {
        // Execute the actions effects
        for (ActionEffectVM* effectVM : action->effectsList()->toList())
        {
            if ((effectVM != nullptr) && (effectVM->modelM() != nullptr))
            {
                // Get the pair with the agent and the command (with parameters) of the effect
                QPair<AgentsGroupedByNameVM*, QStringList> pairAgentAndCommandWithParameters = effectVM->modelM()->getAgentAndCommandWithParameters();

                // Execute the command for the agent
                _executeCommandForAgent(pairAgentAndCommandWithParameters.first, pairAgentAndCommandWithParameters.second);
            }
        }

        // Emit the signal "All Effects have been Executed"
        Q_EMIT action->allEffectsHaveBeenExecuted();
    }
}


/**
 * @brief Execute reverse effects of an action
 * @param actionExecution
 */
void AbstractScenarioController::_executeReverseEffectsOfAction(ActionExecutionVM* actionExecution)
{
    if ((_modelManager != nullptr) && (actionExecution != nullptr) && actionExecution->shallRevert())
    {
        // Get the list of pairs <agent name, reverse command (and parameters)>
        QList<QPair<QString, QStringList>> reverseCommandsForAgents = actionExecution->getReverseCommands();

        for (int i = 0; i < reverseCommandsForAgents.count(); i++)
        {
            QPair<QString, QStringList> pairAgentNameAndReverseCommand = reverseCommandsForAgents.at(i);

            AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(pairAgentNameAndReverseCommand.first);
            if (agentsGroupedByName != nullptr)
            {
                // Execute the (reverse) command for the agent
                _executeCommandForAgent(agentsGroupedByName, pairAgentNameAndReverseCommand.second);
            }
        }
    }
}


/**
 * @brief Execute a command for an agent
 * @param agentsGroupedByName
 * @param commandAndParameters
 */
void AbstractScenarioController::_executeCommandForAgent(AgentsGroupedByNameVM* agentsGroupedByName, QStringList commandAndParameters)
{
    if ((agentsGroupedByName != nullptr) && !commandAndParameters.isEmpty())
    {
        qInfo() << "Execute command" << commandAndParameters << "for agent" << agentsGroupedByName->name() << "(" << agentsGroupedByName->peerIdsList().count() << "peer ids)";

        QString command = commandAndParameters.at(0);

        // START
        if (command == command_StartAgent)
        {
            if (_modelManager != nullptr)
            {
                for (AgentM* model : agentsGroupedByName->models()->toList())
                {
                    // Check if the model has a hostname
                    if ((model != nullptr) && !model->hostname().isEmpty())
                    {
                        // Get the peer id of the Launcher on this host
                        QString peerIdOfLauncher = _modelManager->getPeerIdOfLauncherOnHost(model->hostname());
                        if (!peerIdOfLauncher.isEmpty())
                        {
                            Q_EMIT commandAskedToLauncher(peerIdOfLauncher, command, model->commandLine());
                        }
                    }
                }
            }
        }
        // STOP or MUTE/UN-MUTE or FREEZE/UN-FREEZE
        else if ( (command == command_StopAgent)
                  || (command == command_MuteAgent) || (command == command_UnmuteAgent)
                  || (command == command_FreezeAgent) || (command == command_UnfreezeAgent) )
        {
            // Emit signal "Command asked to agent"
            Q_EMIT commandAskedToAgent(agentsGroupedByName->peerIdsList(), command);
        }
        // MAP or UNMAP
        else if ((command == command_MapAgents) || (command == command_UnmapAgents))
        {
            if (commandAndParameters.count() == 4)
            {
                QString inputName = commandAndParameters.at(1);
                QString outputAgentName = commandAndParameters.at(2);
                QString outputName = commandAndParameters.at(3);

                // Emit signal "Command asked to agent about Mapping Input"
                Q_EMIT commandAskedToAgentAboutMappingInput(agentsGroupedByName->peerIdsList(), command, inputName, outputAgentName, outputName);
            }
            else {
                qCritical() << "Wrong number of parameters (" << commandAndParameters.count() << ") to map an input of agent" << agentsGroupedByName->name();
            }
        }
        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        else if (command.startsWith("SET_"))
        {
            if (commandAndParameters.count() == 3)
            {
                QString agentIOPName = commandAndParameters.at(1);
                QString value = commandAndParameters.at(2);

                // Emit signal "Command asked to agent about Setting Value"
                Q_EMIT commandAskedToAgentAboutSettingValue(agentsGroupedByName->peerIdsList(), command, agentIOPName, value);
            }
            else {
                qCritical() << "Wrong number of parameters (" << commandAndParameters.count() << ") to set a value to agent" << agentsGroupedByName->name();
            }
        }
    }
}


/**
 * @brief Exectute the action with the revert initialization if necessary
 * @param action view model
 * @param action execution view model
 * @param current time in ms
 */
void AbstractScenarioController::_executeAction(ActionVM* actionVM, ActionExecutionVM* actionExecution, int currentTimeInMilliSeconds)
{
    if ((actionVM != nullptr) && (actionVM->modelM() != nullptr) && (actionExecution != nullptr))
    {
        if (actionExecution->shallRevert()) {
            // Initialize the reverse command (and parameters) for each effect
            actionExecution->initReverseCommandsForEffects(actionVM->modelM()->effectsList()->toList());
        }

        // Execute all effects of the action
        executeEffectsOfAction(actionVM->modelM(), actionVM->lineInTimeLine());

        // Notify the action that its effects has been executed
        actionVM->effectsExecuted(currentTimeInMilliSeconds);
    }
}


/**
 * @brief Get the list of view models of action with its (unique) id
 * @param actionId
 * @return
 */
QList<ActionVM*> AbstractScenarioController::_getListOfActionVMwithId(int actionId)
{
    if (_hashFromUidToViewModelsOfAction.contains(actionId)) {
        return _hashFromUidToViewModelsOfAction.value(actionId);
    }
    else {
        return QList<ActionVM*>();
    }
}


/**
 * @brief Get the "Sorted" list of view models of action with the index of the line (in the time line)
 * @param index
 * @return
 */
I2CustomItemSortFilterListModel<ActionVM>* AbstractScenarioController::_getSortedListOfActionVMwithLineIndex(int index)
{
    if (_hashFromLineIndexToSortedViewModelsOfAction.contains(index)) {
        return _hashFromLineIndexToSortedViewModelsOfAction.value(index);
    }
    else {
        return nullptr;
    }
}
