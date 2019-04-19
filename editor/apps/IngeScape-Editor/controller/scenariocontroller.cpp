/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "scenariocontroller.h"

#include <QDebug>
#include <QFileDialog>
#include <misc/ingescapeutils.h>

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
ScenarioController::ScenarioController(EditorModelManager* modelManager,
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
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    // Configure our filtered list of "actionVM in timeline"
    _filteredListActionsInTimeLine.setSourceModel(&_actionsInTimeLine);


    // Fill the list of all values of condition on agent (state)
    _allAgentConditionValues.fillWithAllEnumValues();

    // Fill the list of all types for values comparison
    _allValueComparisonTypes.fillWithAllEnumValues();

    // Fill with all values
    _agentEffectValuesList.fillWithAllEnumValues();
    _mappingEffectValuesList.fillWithAllEnumValues();

    // Fill general types
    _conditionsTypesList.fillWithAllEnumValues();
    _effectsTypesList.fillWithAllEnumValues();

    // Fill validity duration types list
    _validationDurationsTypesList.fillWithAllEnumValues();

    // Initialize the 9 items of the palette with NULL action
    for (int i = 0; i < 9; i++)
    {
        _actionsInPaletteList.append(new ActionInPaletteVM(nullptr, i));
    }

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
    connect(&_timerToExecuteActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_ExecuteActions);

    _timerToRegularlyDelayActions.setInterval(INTERVAL_DELAY_ACTIONS);
    _timerToRegularlyDelayActions.setSingleShot(true);
    connect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_DelayOrExecuteActions);
}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    disconnect(&_timerToExecuteActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_ExecuteActions);
    _timerToExecuteActions.stop();

    disconnect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_DelayOrExecuteActions);
    _timerToRegularlyDelayActions.stop();

    // Clear the current scenario
    clearScenario();

    // Delete actions VM from the palette
    _actionsInPaletteList.deleteAllItems();

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Custom setter on is playing command for the scenario
 * @param is playing flag
 */
void ScenarioController::setisPlaying(bool isPlaying)
{
    if (_isPlaying != isPlaying)
    {
        _isPlaying = isPlaying;

        // Start/Stop scenario according to the flag
        if (_isPlaying) {
            _startScenario();
        }
        else {
            _stopScenario();
        }

        Q_EMIT isPlayingChanged(_isPlaying);
    }
}


/**
  * @brief Import the scenario from JSON
  * @param jsonScenario
  */
void ScenarioController::importScenarioFromJson(QJsonObject jsonScenario)
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
                    if (actionM != nullptr)
                    {
                        // Add action into the list
                        _actionsList.append(actionM);

                        // Add action into the map
                        _mapActionsFromActionName.insert(actionM->name(), actionM);

                        if (!_hashFromUidToModelOfAction.contains(actionM->uid())) {
                            _hashFromUidToModelOfAction.insert(actionM->uid(), actionM);
                        }
                    }
                }
            }

            // Set the list of actions in palette
            if (!scenarioToImport->actionsInPaletteList()->isEmpty())
            {
                for (ActionInPaletteVM* actionInPalette : scenarioToImport->actionsInPaletteList()->toList())
                {
                    if ((actionInPalette != nullptr) && (actionInPalette->modelM() != nullptr)) {
                        setActionInPalette(actionInPalette->indexInPanel(), actionInPalette->modelM());
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
void ScenarioController::importExecutedActionsFromJson(QByteArray byteArrayOfJson)
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
                        ActionM* action = _getModelOfActionWithId(actionId);
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
  * @brief Check if an agent is used in the current scenario (actions, conditions, effects)
  * @param agentName
  */
bool ScenarioController::isAgentUsedInScenario(QString agentName)
{
    bool isUsed = false;

    for (ActionM* actionM : _actionsList.toList())
    {
        // Check the action conditions
        for (ActionConditionVM* conditionVM : actionM->conditionsList()->toList())
        {
            if ((conditionVM != nullptr) && (conditionVM->modelM() != nullptr) && (conditionVM->modelM()->agent() != nullptr)
                    && (conditionVM->modelM()->agent()->name() == agentName))
            {
                isUsed = true;
                break;
            }
        }

        // Check the action effects
        if (!isUsed)
        {
            for (ActionEffectVM* effectVM : actionM->effectsList()->toList())
            {
                if ((effectVM != nullptr) && (effectVM->modelM() != nullptr) && (effectVM->modelM()->agent() != nullptr)
                        && (effectVM->modelM()->agent()->name() == agentName))
                {
                    isUsed = true;
                    break;
                }
            }
        }
    }

    return isUsed;
}


/**
  * @brief Open the action editor with a model of action
  * @param action
  */
void ScenarioController::openActionEditorWithModel(ActionM* action)
{
    if (_modelManager != nullptr)
    {
        ActionEditorController* actionEditorC = nullptr;

        if (action != nullptr)
        {
            actionEditorC = _getActionEditorFromModelOfAction(action);

            // The corresponding editor is already opened
            if (actionEditorC != nullptr)
            {
                qDebug() << "The 'Action Editor' of" << action->name() << "is already opened...bring to front !";

                Q_EMIT actionEditorC->bringToFront();
            }
            else
            {
                // Set selected action
                setselectedAction(action);

                // Create an action editor
                actionEditorC = new ActionEditorController(_buildNewActionName(), action, _modelManager->allAgentsGroupsByName()->toList());

                _hashActionEditorControllerFromModelOfAction.insert(action, actionEditorC);

                // Add to the list of opened action editors
                _openedActionsEditorsControllers.append(actionEditorC);
            }
        }
        else
        {
            // Create an action editor
            actionEditorC = new ActionEditorController(_buildNewActionName(), nullptr, _modelManager->allAgentsGroupsByName()->toList());

            _hashActionEditorControllerFromModelOfAction.insert(actionEditorC->editedAction(), actionEditorC);

            // Add to the list of opened action editors
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }
}


/**
  * @brief Open the action editor with a view model of action
  * @param action
  */
void ScenarioController::openActionEditorWithViewModel(ActionVM* action)
{
    if ((_modelManager != nullptr) && (action != nullptr) && (action->modelM() != nullptr))
    {
        ActionEditorController* actionEditorC = _getActionEditorFromViewModelOfAction(action);

        // The corresponding editor is already opened
        if (actionEditorC != nullptr)
        {
            qDebug() << "The 'Action Editor' of" << action->modelM()->name() << "is already opened...bring to front !";

            Q_EMIT actionEditorC->bringToFront();
        }
        else
        {
            setselectedAction(action->modelM());

            // Create an action editor
            actionEditorC = new ActionEditorController(_buildNewActionName(), action->modelM(), _modelManager->allAgentsGroupsByName()->toList());

            _hashActionEditorControllerFromViewModelOfAction.insert(action, actionEditorC);

            // Set the original view model
            actionEditorC->setoriginalViewModel(action);

            // Create the temporary edited action view model
            ActionVM* temporaryActionVM = new ActionVM(nullptr, -1);
            temporaryActionVM->setstartTimeString(action->startTimeString());
            temporaryActionVM->setcolor(action->color());

            actionEditorC->seteditedViewModel(temporaryActionVM);

            // Add to the list of opened action editors
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }
}


/**
  * @brief Delete an action from the list
  * @param action
  */
void ScenarioController::deleteAction(ActionM* action)
{
    if (action != nullptr)
    {
        int actionId = action->uid();

        // Delete the popup if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromModelOfAction(action);
        if (actionEditorC != nullptr)
        {
            _hashActionEditorControllerFromModelOfAction.remove(action);

            // Remove from the list of opened action editors
            _openedActionsEditorsControllers.remove(actionEditorC);

            delete actionEditorC;
        }

        // Unselect our action if needed
        if (_selectedAction == action) {
            setselectedAction(nullptr);
        }

        // Remove action from the palette if exists
        for (int index = 0; index < _actionsInPaletteList.count(); index++)
        {
            ActionInPaletteVM* actionInPaletteVM = _actionsInPaletteList.at(index);

            if ((actionInPaletteVM != nullptr) && (actionInPaletteVM->modelM() != nullptr) && (actionInPaletteVM->modelM() == action)) {
                setActionInPalette(index, nullptr);
            }
        }

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

        // Delete the action item
        if (_actionsList.contains(action))
        {
            _actionsList.remove(action);

            _mapActionsFromActionName.remove(action->name());

            _hashFromUidToModelOfAction.remove(actionId);

            delete action;

            // Free the UID of the action model
            IngeScapeUtils::freeUIDofActionM(actionId);
        }
    }
}


/**
  * @brief Validate action editior
  * @param action editor controller
  */
void ScenarioController::validateActionEditor(ActionEditorController* actionEditorC)
{
    if (actionEditorC != nullptr)
    {
        // Validate modification
        actionEditorC->validateModification();

        ActionM* originalAction = actionEditorC->originalAction();
        if ((originalAction != nullptr) && !_actionsList.contains(originalAction))
        {
            // Insert into the list
            _actionsList.append(originalAction);

            // Insert into the map
            _mapActionsFromActionName.insert(originalAction->name(), originalAction);

            // Insert into the hash table
            _hashFromUidToModelOfAction.insert(originalAction->uid(), originalAction);
        }

        // Set selected action
        setselectedAction(originalAction);
    }
}


/**
  * @brief Close action editior
  * @param action editor controller
  */
void ScenarioController::closeActionEditor(ActionEditorController* actionEditorC)
{
    if (actionEditorC != nullptr)
    {
        // Remove action editor from view model of action
        if (actionEditorC->originalViewModel() != nullptr)
        {
            ActionEditorController* actionEditorToRemove = _getActionEditorFromViewModelOfAction(actionEditorC->originalViewModel());
            if (actionEditorToRemove != nullptr)
            {
                _hashActionEditorControllerFromViewModelOfAction.remove(actionEditorC->originalViewModel());

                // Remove from the list of opened action editors
                _openedActionsEditorsControllers.remove(actionEditorToRemove);

                delete actionEditorToRemove;
            }
        }
        // Remove action editor from model of action
        else
        {
            ActionM* action = nullptr;
            if (actionEditorC->originalAction() != nullptr) {
                action = actionEditorC->originalAction();
            }
            else {
                action = actionEditorC->editedAction();
            }
            if (action != nullptr)
            {
                ActionEditorController* actionEditorToRemove = _getActionEditorFromModelOfAction(action);
                if (actionEditorToRemove != nullptr)
                {
                    _hashActionEditorControllerFromModelOfAction.remove(action);

                    // Remove from the list of opened action editors
                    _openedActionsEditorsControllers.remove(actionEditorToRemove);

                    delete actionEditorToRemove;
                }
            }
        }
    }
}


/**
 * @brief Set an action into the palette at index
 * @param index where to insert the action
 * @param action to insert
 */
void ScenarioController::setActionInPalette(int index, ActionM* actionM)
{
    if (index < _actionsInPaletteList.count())
    {
        ActionInPaletteVM* actionInPalette = _actionsInPaletteList.at(index);
        if (actionInPalette != nullptr) {
            actionInPalette->setmodelM(actionM);
        }
    }
}


/**
 * @brief Add an action VM at the time in ms
 * @param action model
 * @param line index
 */
void ScenarioController::addActionVMAtTime(ActionM* actionM, int timeInMs, int lineIndex)
{
    if (actionM != nullptr)
    {
        int actionId = actionM->uid();

        ActionVM* actionVM = new ActionVM(actionM, timeInMs);

        // Insert the actionVM to the reight line number
        _insertActionVMIntoMapByLineNumber(actionVM, lineIndex);

        // If the action VM found a line to be insert in
        if (actionVM->lineInTimeLine() > -1)
        {
            // Add the new action VM to our hash table
            QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);
            listOfActionVM.append(actionVM);
            _hashFromUidToViewModelsOfAction.insert(actionId, listOfActionVM);

            // Add the action VM to the timeline
            _actionsInTimeLine.append(actionVM);

            // If scenario is playing we add the actionVM to the active ones
            if (_isPlaying)
            {
                // Initialize the action connections
                actionVM->modelM()->initializeConditionsConnections();

                // Connect the revert action
                connect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                connect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);

                // Add action to the ones to check
                _listOfActionsToEvaluate.append(actionVM);

                // The new action VM is the next one to activate, we change the next timer trigger
                if ( ((_nextActionToActivate == nullptr) && (timeInMs >= _currentTime.msecsSinceStartOfDay()))
                     || ((_nextActionToActivate != nullptr) && (timeInMs < _nextActionToActivate->startTime())) )
                {
                    // Stop the timer if necessary
                    if (_timerToExecuteActions.isActive()) {
                        _timerToExecuteActions.stop();
                    }

                    setnextActionToActivate(actionVM);
                    _timerToExecuteActions.start(actionVM->startTime() - _currentTime.msecsSinceStartOfDay());
                }
            }
        }
        else {
            delete actionVM;
            actionVM = nullptr;
        }
    }
}


/**
 * @brief Add an action VM at the current date time
 * @param action model
 */
void ScenarioController::addActionVMAtCurrentTime(ActionM* actionM)
{
    if (actionM != nullptr) {
        addActionVMAtTime(actionM, _currentTime.msecsSinceStartOfDay(), 0);
    }
}


/**
 * @brief Remove an action VM from the time line
 * @param action view model
 */
void ScenarioController::removeActionVMfromTimeLine(ActionVM* actionVM)
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


        // Delete the action editor if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromViewModelOfAction(actionVM);
        if (actionEditorC != nullptr)
        {
            _hashActionEditorControllerFromViewModelOfAction.remove(actionVM);

            // Remove from the list of opened action editors
            _openedActionsEditorsControllers.remove(actionEditorC);

            delete actionEditorC;
        }


        // Free memory
        delete actionVM;
    }
}


/**
 * @brief Initialize the connections for conditions of all actions
 */
void ScenarioController::initializeConditionsConnectionsOfAllActions()
{
    for (ActionM* actionM : _actionsList.toList())
    {
        if (actionM != nullptr) {
            actionM->initializeConditionsConnections();
        }
    }
}


/**
 * @brief Reset the connections for conditions of all actions
 */
void ScenarioController::resetConditionsConnectionsOfAllActions()
{
    for (ActionM* actionM : _actionsList.toList())
    {
        if (actionM != nullptr) {
            actionM->resetConditionsConnections();
        }
    }
}


/**
 * @brief Test if an item can be inserted into a line number
 * @param actionM to insert
 * @param time to insert
 * @param line index
 * @param optional dragged action VM when already in the time-line
 */
bool ScenarioController::canInsertActionVMTo(ActionM* actionMToInsert, int time, int lineIndex, ActionVM* draggedActionVM)
{
    // The dragged action is defined and is currently on the first line
    if ((draggedActionVM != nullptr) && (draggedActionVM->lineInTimeLine() == 0))
    {
        // Prevent modification on the first line because it is reserved for live insertions from the palette
        return false;
    }

    // Prevent insertion on the first line because it is reserved for live insertions from the palette
    if (lineIndex == 0) {
        return false;
    }

    // Get the "Sorted" list of view models of action with the index of the line (in the time line)
    I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndex);
    if (sortedListOfActionVM != nullptr)
    {
        int insertionStartTime = time;
        int insertionEndTime = time;

        // If we insert a FOREVER action
        if (actionMToInsert->validityDurationType() == ValidationDurationTypes::FOREVER) {
            insertionEndTime = -1;
        }
        // If we insert a CUSTOM temporal action
        else if (actionMToInsert->validityDurationType() == ValidationDurationTypes::CUSTOM)
        {
            insertionEndTime = time + actionMToInsert->validityDuration();
        }

        // Revert After Time > Validity Duration
        if ((insertionEndTime > -1)
                && actionMToInsert->shallRevertAfterTime() && (actionMToInsert->revertAfterTime() > actionMToInsert->validityDuration()))
        {
            insertionEndTime = time + actionMToInsert->revertAfterTime();
        }

        // Add some margin
        if (insertionStartTime == insertionEndTime) {
            insertionStartTime = time - MARGIN_FOR_ACTION_INSERTION_IN_MS;
            insertionEndTime = time + MARGIN_FOR_ACTION_INSERTION_IN_MS;
        }

        for (int indexAction = 0; indexAction < sortedListOfActionVM->count(); indexAction++)
        {
            ActionVM* actionVM = sortedListOfActionVM->at(indexAction);

            if ((actionVM != nullptr) && (actionVM->modelM() != nullptr)
                    && ((draggedActionVM == nullptr) || (draggedActionVM != actionVM)) )
            {
                //qDebug() << "Insert" << insertionStartTime << insertionEndTime << "inside" << actionVM->startTime() << actionVM->endTime();

                if ((actionVM->endTime() == -1) || (insertionEndTime == -1))
                {
                    // Both have no end time
                    if ( ((actionVM->endTime() == -1) && (insertionEndTime == -1))
                         ||
                         // Action starts before "Insertion END Time"
                         ((actionVM->endTime() == -1) && (actionVM->startTime() < insertionEndTime))
                         ||
                         // Action ends after "Insertion START Time"
                         ((insertionEndTime == -1) && (actionVM->endTime() > insertionStartTime)) )
                    {
                        return false;
                    }
                }
                else
                {
                    // Action is Inside
                    if ( ((actionVM->startTime() > insertionStartTime) && (actionVM->endTime() < insertionEndTime))
                            ||
                            // Action is around "Insertion START Time"
                            ((actionVM->startTime() < insertionStartTime) && (actionVM->endTime() > insertionStartTime))
                            ||
                            // Action is around "Insertion END Time"
                            ((actionVM->startTime() < insertionEndTime) && (actionVM->endTime() > insertionEndTime))
                            ||
                            // Action starts before and ends after
                            ((actionVM->startTime() < insertionStartTime) && (actionVM->endTime() > insertionEndTime)) )
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}


/**
 * @brief Execute all effects of the action
 * Activate (connect) the mapping if necessary
 * Notify the recorder that the action has been executed
 * @param action
 * @param lineInTimeLine
 */
void ScenarioController::executeEffectsOfAction(ActionM* action, int lineInTimeLine)
{
    if ((action != nullptr) && !action->effectsList()->isEmpty())
    {
        // Active the mapping if needed
        if ((_modelManager != nullptr) && !_modelManager->isMappingActivated()) {
            _modelManager->setisMappingActivated(true);
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
 * @brief Clear the current scenario
 * (clear the list of actions in the list, in the palette and in the timeline)
 */
void ScenarioController::clearScenario()
{
    qInfo() << "Clear the current scenario";

    _stopScenario();

    // Clean-up current selection
    setselectedAction(nullptr);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

    // Clean-up current selection
    setselectedActionVMInTimeline(nullptr);

    // Clear the lists of action editors
    _hashActionEditorControllerFromModelOfAction.clear();
    _hashActionEditorControllerFromViewModelOfAction.clear();
    _openedActionsEditorsControllers.deleteAllItems();

    // Delete all models of action
    _actionsList.deleteAllItems();

    // Clear map
    _mapActionsFromActionName.clear();
    _hashFromUidToModelOfAction.clear();

    // Reset actions in palette
    for (ActionInPaletteVM* actionInPalette : _actionsInPaletteList.toList())
    {
        if (actionInPalette != nullptr) {
            actionInPalette->setmodelM(nullptr);
        }
    }

    // Reset current time
    setcurrentTime(QTime::fromMSecsSinceStartOfDay(0));
}


/**
 * @brief Move an actionVM to a start time position in ms and a specific line number
 * @param action VM
 * @param time in milliseconds
 * @param line index
 */
void ScenarioController::moveActionVMAtTimeAndLine(ActionVM* actionVM, int timeInMilliseconds, int lineIndex)
{
    if ((actionVM != nullptr) && (actionVM->modelM() != nullptr) && (timeInMilliseconds >= 0) && (lineIndex >= 0))
    {
        bool canInsert = canInsertActionVMTo(actionVM->modelM(), timeInMilliseconds, lineIndex, actionVM);

        // Insert our item if possible
        if (canInsert)
        {
            // Reset connections
            // Connect the revert action
            if (actionVM->timerToReverse()->isActive()) {
                actionVM->timerToReverse()->stop();
            }
            disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
            disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);

            // FIXME: need to connect something later ?

            if (_listOfActionsToEvaluate.contains(actionVM)) {
                _listOfActionsToEvaluate.remove(actionVM);
            }
            if (_listOfActiveActions.contains(actionVM)) {
                _listOfActiveActions.remove(actionVM);
            }

            // Set the new start time
            int hours = timeInMilliseconds / NB_MILLI_SECONDS_IN_ONE_HOUR;
            int minutes = (timeInMilliseconds - hours * NB_MILLI_SECONDS_IN_ONE_HOUR) / NB_MILLI_SECONDS_IN_ONE_MINUTE;
            int seconds = (timeInMilliseconds - hours * NB_MILLI_SECONDS_IN_ONE_HOUR - minutes * NB_MILLI_SECONDS_IN_ONE_MINUTE) / 1000;
            int milliseconds = timeInMilliseconds % 1000;

            //actionVM->setstartTimeString(QString::number(hours).rightJustified(2, '0') + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0') + "." + QString::number(milliseconds).leftJustified(3, '0'));
            QString startTimeString = QString("%1:%2:%3.%4").arg(QString::number(hours).rightJustified(2, '0'),
                                                                 QString::number(minutes).rightJustified(2, '0'),
                                                                 QString::number(seconds).rightJustified(2, '0'),
                                                                 QString::number(milliseconds).leftJustified(3, '0'));
            actionVM->setstartTimeString(startTimeString);


            // If the line number has changed
            if (actionVM->lineInTimeLine() != lineIndex)
            {
                // Remove the actionVM from the previous line if different
                I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(actionVM->lineInTimeLine());
                if (sortedListOfActionVM != nullptr)
                {
                    sortedListOfActionVM->remove(actionVM);
                }

                // set the line number
                actionVM->setlineInTimeLine(lineIndex);

                // Add an extra line if inserted our actionVM at the last line
                if (lineIndex >= _linesNumberInTimeLine - 1) {
                    setlinesNumberInTimeLine(lineIndex + 2);
                }

                // Insert in the new line
                sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndex);
                if (sortedListOfActionVM != nullptr)
                {
                    // Insert the action
                    sortedListOfActionVM->append(actionVM);
                }
                else
                {
                    // Create a new list and add to the hash table
                    sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
                    sortedListOfActionVM->setSortProperty("startTime");
                    sortedListOfActionVM->append(actionVM);

                    _hashFromLineIndexToSortedViewModelsOfAction.insert(lineIndex, sortedListOfActionVM);
                }
            }
        }
    }
}


/**
 * @brief Check if a view model of an action is inserted in the timeline
 * @param actionM
 * @return
 */
bool ScenarioController::isInsertedInTimeLine(ActionM* actionM)
{
    if ((actionM != nullptr) && _hashFromUidToViewModelsOfAction.contains(actionM->uid()))
    {
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Slot called when an action must be reverted
 * @param actionExecution
 */
void ScenarioController::onRevertAction(ActionExecutionVM* actionExecution)
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
            disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
            disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
        }
    }
}


/**
  * @brief Slot called when an action must be rearmed
  */
void ScenarioController::onRearmAction()
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
            disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
            disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
        }
    }
}


/**
 * @brief Slot called when the time line range changed
 * @param startTimeInMilliseconds
 * @param endTimeInMilliseconds
 */
void ScenarioController::onTimeRangeChanged(int startTimeInMilliseconds, int endTimeInMilliseconds)
{
    _filteredListActionsInTimeLine.setTimeRange(startTimeInMilliseconds, endTimeInMilliseconds);
}


/**
 * @brief Slot called when we receive the command "run action" from a recorder
 * @param actionID
 */
void ScenarioController::onRunAction(QString actionID)
{
    bool success = false;
    int id = actionID.toInt(&success);

    if (success)
    {
        // Get the model of action with this (unique) id
        ActionM* action = _getModelOfActionWithId(id);
        if (action != nullptr)
        {
            // Execute all effects of the action
            _executeEffectsOfAction(action);
        }
    }
}


/**
 * @brief Called when our timer time out to handle the scenario and execute actions
 */
void ScenarioController::_onTimeout_ExecuteActions()
{
    if (!_listOfActionsToEvaluate.isEmpty())
    {
        //ActionVM* actionToExecute = _listOfActionsToEvaluate.at(0);

        int currentTimeInMilliSeconds = 0;
        if (_nextActionToActivate != nullptr) {
            currentTimeInMilliSeconds = _nextActionToActivate->startTime();
        }
        ActionVM* tmpNextActionToActivate = nullptr;


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
                        if (actionVM->modelM()->isValid()) {
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
                                disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                                disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
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
void ScenarioController::_onTimeout_DelayOrExecuteActions()
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
                        disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                    }
                    if (!actionVM->timerToRearm()->isActive())
                    {
                        // Disconnect the rearm action signal
                        disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
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
 * @brief Get a new action name
 */
QString ScenarioController::_buildNewActionName()
{
    //int index = _mapActionsFromActionName.count() + 1;

    int index = 0;

    // QString::arg(int a, int fieldWidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
    // The a argument is expressed in base base, which is 10 by default.
    // fieldWidth specifies the minimum amount of space that a is padded to and filled with the character fillChar.
    QString tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));

    while (_mapActionsFromActionName.contains(tmpName))
    {
        index++;
        tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));
    }

    return tmpName;
}


/**
 * @brief Insert an actionVM into our timeline
 * @param action view model
 * @return timeline line number
 */
void ScenarioController::_insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert, int lineNumberRef)
{
    if ((actionVMToInsert != nullptr) && (actionVMToInsert->modelM() != nullptr))
    {
        int insertionStartTime = actionVMToInsert->startTime();

        int lineNumber = 0;
        if (lineNumberRef != -1) {
            lineNumber = lineNumberRef;
        }

        while (lineNumber < _linesNumberInTimeLine)
        {
            bool canInsert = false;
            if (lineNumber == 0) {
                canInsert = true;
            }
            else {
                canInsert = canInsertActionVMTo(actionVMToInsert->modelM(), insertionStartTime, lineNumber);
            }

            // Insert our item if possible
            if (canInsert)
            {
                // set the line number
                actionVMToInsert->setlineInTimeLine(lineNumber);

                // Add an extra line if inserted our actionVM at the last line
                if (lineNumber >= _linesNumberInTimeLine - 1) {
                    setlinesNumberInTimeLine(lineNumber + 2);
                }

                // Get the "Sorted" list of view models of action with the index of the line (in the time line)
                I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineNumber);
                if (sortedListOfActionVM != nullptr)
                {
                    // Insert the action
                    sortedListOfActionVM->append(actionVMToInsert);
                    break;
                }
                else
                {
                    // Create a new list and add to the hash table
                    sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
                    sortedListOfActionVM->setSortProperty("startTime");
                    sortedListOfActionVM->append(actionVMToInsert);

                    _hashFromLineIndexToSortedViewModelsOfAction.insert(lineNumber, sortedListOfActionVM);
                    break;
                }
            }

            if (lineNumberRef != -1) {
                break;
            }
            else {
                lineNumber++;
            }

        }

        // If the action has not been inserted yet, we create a new line
        // only if we are not dropping at a busy position the actionVM
        if ((actionVMToInsert->lineInTimeLine() == -1) && (lineNumberRef == -1))
        {
            // Add an extra line if inserted our actionVM at the last line
            if (lineNumber >= _linesNumberInTimeLine -1) {
                setlinesNumberInTimeLine(lineNumber + 2);
            }

            // Create the new line number
            actionVMToInsert->setlineInTimeLine(lineNumber);

            // Create a new list and add to the hash table
            I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
            sortedListOfActionVM->setSortProperty("startTime");
            sortedListOfActionVM->append(actionVMToInsert);

            _hashFromLineIndexToSortedViewModelsOfAction.insert(lineNumber, sortedListOfActionVM);
        }
    }
}


/**
 * @brief Start the scenario by
 *        making connections for the actions conditions
 *        starting the action evaluation timer
 */
void ScenarioController::_startScenario()
{
    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    // Active the mapping if needed
    if ((_modelManager != nullptr) && !_modelManager->isMappingActivated()) {
        _modelManager->setisMappingActivated(true);
    }

    // Disconnect from signals
    for (ActionVM* actionVM : _listOfActionsToEvaluate.toList())
    {
        disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
        disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
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
                connect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                connect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);

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
    initializeConditionsConnectionsOfAllActions();

    // Save the time of the day
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut = QTime::currentTime().msecsSinceStartOfDay();

    //qDebug() << "_startScenario:" << _currentTime << QTime::currentTime();

    // Start timers
    // init the timer with the time of the next action execution
    if (_nextActionToActivate != nullptr) {
        _timerToExecuteActions.start(_nextActionToActivate->startTime() - currentTimeInMilliSeconds);
    }

    _timerToRegularlyDelayActions.start(INTERVAL_DELAY_ACTIONS);
}


/**
 * @brief Stop the scenario by
 *        disconnecting the actions conditions
 *        stoping the action evaluation timer
 */
void ScenarioController::_stopScenario()
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
            disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);

            // Disconnect rearm action
            if (actionVM->timerToRearm()->isActive()) {
                actionVM->timerToRearm()->stop();
            }
            disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
        }
    }

    // Reset the next action VM to activate
    setnextActionToActivate(nullptr);

    // Reset the connections for conditions of all actions
    resetConditionsConnectionsOfAllActions();
}


/**
 * @brief Execute all effects of an action
 * @param action
 */
void ScenarioController::_executeEffectsOfAction(ActionM* action)
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
    }
}


/**
 * @brief Execute reverse effects of an action
 * @param actionExecution
 */
void ScenarioController::_executeReverseEffectsOfAction(ActionExecutionVM* actionExecution)
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
void ScenarioController::_executeCommandForAgent(AgentsGroupedByNameVM* agentsGroupedByName, QStringList commandAndParameters)
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
void ScenarioController::_executeAction(ActionVM* actionVM, ActionExecutionVM* actionExecution, int currentTimeInMilliSeconds)
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
 * @brief Get the model of action with its (unique) id
 * @param actionId
 * @return
 */
ActionM* ScenarioController::_getModelOfActionWithId(int actionId)
{
    if (_hashFromUidToModelOfAction.contains(actionId)) {
        return _hashFromUidToModelOfAction.value(actionId);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the list of view models of action with its (unique) id
 * @param actionId
 * @return
 */
QList<ActionVM*> ScenarioController::_getListOfActionVMwithId(int actionId)
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
I2CustomItemSortFilterListModel<ActionVM>* ScenarioController::_getSortedListOfActionVMwithLineIndex(int index)
{
    if (_hashFromLineIndexToSortedViewModelsOfAction.contains(index)) {
        return _hashFromLineIndexToSortedViewModelsOfAction.value(index);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the "Action Editor" from a model of action
 * @return
 */
ActionEditorController* ScenarioController::_getActionEditorFromModelOfAction(ActionM* action)
{
    if (_hashActionEditorControllerFromModelOfAction.contains(action)) {
        return _hashActionEditorControllerFromModelOfAction.value(action);
    }
    else {
        return nullptr;
    }
}


/**
 * @brief Get the "Action Editor" from a view model of action
 * @return
 */
ActionEditorController* ScenarioController::_getActionEditorFromViewModelOfAction(ActionVM* action)
{
    if (_hashActionEditorControllerFromViewModelOfAction.contains(action)) {
        return _hashActionEditorControllerFromViewModelOfAction.value(action);
    }
    else {
        return nullptr;
    }
}
