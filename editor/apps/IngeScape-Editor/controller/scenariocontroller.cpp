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

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param scenariosPath
 * @param parent
 */
ScenarioController::ScenarioController(IngeScapeModelManager* modelManager,
                                       JsonHelper* jsonHelper,
                                       QString scenariosPath,
                                       QObject *parent) : QObject(parent),
    _selectedAction(NULL),
    _selectedActionVMInTimeline(NULL),
    _linesNumberInTimeLine(MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE),
    _isPlaying(false),
    _currentTime(QTime::fromMSecsSinceStartOfDay(0)),
    _nextActionToActivate(NULL),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper),
    _scenariosDirectoryPath(scenariosPath),
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Configure our filtered list of "actionVM in timeline"
    _filteredListActionsInTimeLine.setSourceModel(&_actionsInTimeLine);

    // Set the agent in mapping list sort by name property
    _agentsInMappingList.setSortProperty("name");

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
        _actionsInPaletteList.append(new ActionInPaletteVM(NULL, i));
    }

    // Add the first timeline line
    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
    actionVMSortedList->setSortProperty("startTime");
    // Add into our map
    _mapActionsVMsInTimelineFromLineIndex.insert(0,actionVMSortedList);

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

    // Clear list and hash table of agents in mapping
    _agentsInMappingList.clear();
    _mapFromNameToAgentInMapping.clear();

    // Clear all scenario and delete objects
    clearScenario();

    // Delete actions VM from the palette
    _actionsInPaletteList.deleteAllItems();

    // Reset pointers
    _modelManager = NULL;
    _jsonHelper = NULL;
}


/**
 * @brief Custom setter on is playing command for the scenario
 * @param is playing flag
 */
void ScenarioController::setisPlaying(bool isPlaying)
{
    if(_isPlaying != isPlaying)
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
  * @brief Import the scenario lists structure from the json byte content
  * @param byteArrayOfJson
  */
void ScenarioController::importScenarioFromJson(QByteArray byteArrayOfJson)
{
    if (_jsonHelper != NULL)
    {
        // Initialize agents lists from JSON file
        scenario_import_actions_lists_t * scenarioToImport = _jsonHelper->initActionsList(byteArrayOfJson, _agentsInMappingList.toList());
        if(scenarioToImport != NULL)
        {
            // Append the list of actions
            if (!scenarioToImport->actionsInTableList.isEmpty())
            {
                // Add each actions to out list
                foreach (ActionM* actionM, scenarioToImport->actionsInTableList)
                {
                    // Add action into the list
                    _actionsList.append(actionM);

                    // Add action into the map
                    _mapActionsFromActionName.insert(actionM->name(), actionM);
                }
            }

            // Set the list of actions in palette
            if (!scenarioToImport->actionsInPaletteList.isEmpty())
            {
                foreach (ActionInPaletteVM* actionInPalette, scenarioToImport->actionsInPaletteList)
                {
                    if (actionInPalette->modelM() != NULL)
                    {
                        setActionInPalette(actionInPalette->indexInPanel(), actionInPalette->modelM());
                    }

                    delete actionInPalette;
                    actionInPalette = NULL;
                }
            }

            // Append the list of actions in timeline
            if (!scenarioToImport->actionsInTimelineList.isEmpty())
            {
                // Add each actionVM in to the right line of our timeline
                foreach (ActionVM* actionVM, scenarioToImport->actionsInTimelineList)
                {
                    int lineNumber = actionVM->lineInTimeLine();

                    // Increment actionVM into the line number
                    if (_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber))
                    {
                        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                        if(actionVMSortedList != NULL)
                        {
                            // Insert the action
                            actionVMSortedList->append(actionVM);
                        }
                    }
                    else {
                        // Create a new list
                        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
                        actionVMSortedList->setSortProperty("startTime");
                        actionVMSortedList->append(actionVM);

                        // Add into our map
                        _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber,actionVMSortedList);
                    }

                    // Add the new action VM to our map
                    QList<ActionVM*> actionsVMsList;
                    if (_mapActionsVMsInTimelineFromActionModel.contains(actionVM->modelM())) {
                        actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionVM->modelM());
                    }
                    actionsVMsList.append(actionVM);
                    _mapActionsVMsInTimelineFromActionModel.insert(actionVM->modelM(), actionsVMsList);

                    _actionsInTimeLine.append(actionVM);

                    // Increment the line number if necessary
                    if (_linesNumberInTimeLine < lineNumber + 2)
                    {
                        setlinesNumberInTimeLine(lineNumber + 2);
                    }
                }
            }

            delete scenarioToImport;
            scenarioToImport = NULL;
        }
    }
}


/**
  * @brief Check if an agent is defined into tha actions (conditions and effects)
  * @param agent name
  */
bool ScenarioController::isAgentDefinedInActions(QString agentName)
{
    bool exists = false;

    foreach (ActionM* actionM, _actionsList.toList())
    {
        // Check the action conditions
        foreach (ActionConditionVM* conditionVM, actionM->conditionsList()->toList())
        {
            if ((conditionVM->modelM() != NULL) && (conditionVM->modelM()->agent() != NULL)
                    && (conditionVM->modelM()->agent()->name() == agentName))
            {
                exists = true;
                break;
            }
        }

        // Check the action effects
        if (!exists)
        {
            foreach (ActionEffectVM* effectVM, actionM->effectsList()->toList())
            {
                if ((effectVM->modelM() != NULL) && (effectVM->modelM()->agent() != NULL)
                        && (effectVM->modelM()->agent()->name() == agentName))
                {
                    exists = true;
                    break;
                }
            }
        }
    }

    return exists;
}


/**
  * @brief Open the action editor with a model of action
  * @param action
  */
void ScenarioController::openActionEditorWithModel(ActionM* action)
{
    if (action != NULL)
    {
        ActionEditorController* actionEditorC = _getActionEditorFromModelOfAction(action);

        // The corresponding editor is already opened
        if (actionEditorC != NULL)
        {
            qDebug() << "The 'Action Editor' of" << action->name() << "is already opened...bring to front !";

            Q_EMIT actionEditorC->bringToFront();
        }
        else
        {
            // Set selected action
            setselectedAction(action);

            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), action, agentsInMappingList());

            _hashActionEditorControllerFromModelOfAction.insert(action, actionEditorC);

            // Add to the list of opened action editors
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }
    else
    {
        // Create action editor controller
        ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), NULL, agentsInMappingList());

        _hashActionEditorControllerFromModelOfAction.insert(actionEditorC->editedAction(), actionEditorC);

        // Add to the list of opened action editors
        _openedActionsEditorsControllers.append(actionEditorC);
    }
}


/**
  * @brief Open the action editor with a view model of action
  * @param action
  */
void ScenarioController::openActionEditorWithViewModel(ActionVM* action)
{
    if ((action != NULL) && (action->modelM() != NULL))
    {
        ActionEditorController* actionEditorC = _getActionEditorFromViewModelOfAction(action);

        // The corresponding editor is already opened
        if (actionEditorC != NULL)
        {
            qDebug() << "The 'Action Editor' of" << action->modelM()->name() << "is already opened...bring to front !";

            Q_EMIT actionEditorC->bringToFront();
        }
        else
        {
            setselectedAction(action->modelM());

            // Create action editor controller
            actionEditorC = new ActionEditorController(_buildNewActionName(), action->modelM(), agentsInMappingList());

            _hashActionEditorControllerFromViewModelOfAction.insert(action, actionEditorC);

            // Set the original view model
            actionEditorC->setoriginalViewModel(action);

            // Create the temporary edited action view model
            ActionVM* temporaryActionVM = new ActionVM(NULL, -1);
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
    if (action != NULL)
    {
        // Delete the popup if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromModelOfAction(action);
        if (actionEditorC != NULL)
        {
            _hashActionEditorControllerFromModelOfAction.remove(action);

            // Remove from the list of opened action editors
            _openedActionsEditorsControllers.remove(actionEditorC);

            delete actionEditorC;
        }

        // Unselect our action if needed
        if (_selectedAction == action) {
            setselectedAction(NULL);
        }

        // Remove action from the palette if exists
        for (int index = 0; index < _actionsInPaletteList.count(); index++)
        {
            ActionInPaletteVM* actionInPaletteVM = _actionsInPaletteList.at(index);
            if ((actionInPaletteVM->modelM() != NULL) && (actionInPaletteVM->modelM() == action)) {
                setActionInPalette(index, NULL);
            }
        }

        // Remove action from the timeline if exists
        if (_mapActionsVMsInTimelineFromActionModel.contains(action))
        {
            QList<ActionVM*> actionVMList = _mapActionsVMsInTimelineFromActionModel.value(action);
            foreach (ActionVM* actionVM, actionVMList)
            {
                removeActionVMFromTimeLine(actionVM);
            }
        }

        // Delete the action item
        if (_actionsList.contains(action))
        {
            _actionsList.remove(action);

            _mapActionsFromActionName.remove(action->name());

            delete action;
            action = NULL;
        }
    }
}


/**
  * @brief Validate action editior
  * @param action editor controller
  */
void ScenarioController::validateActionEditor(ActionEditorController* actionEditorC)
{
    if (actionEditorC != NULL)
    {
        // Validate modification
        actionEditorC->validateModification();

        ActionM* originalAction = actionEditorC->originalAction();
        if ((originalAction != NULL) && !_actionsList.contains(originalAction))
        {
            // Insert into the list
            _actionsList.append(originalAction);

            // Insert into the map
            _mapActionsFromActionName.insert(originalAction->name(), originalAction);
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
    if (actionEditorC != NULL)
    {
        // Remove action editor from view model of action
        if (actionEditorC->originalViewModel() != NULL)
        {
            ActionEditorController* actionEditorToRemove = _getActionEditorFromViewModelOfAction(actionEditorC->originalViewModel());
            if (actionEditorToRemove != NULL)
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
            ActionM* action = NULL;
            if (actionEditorC->originalAction() != NULL) {
                action = actionEditorC->originalAction();
            }
            else {
                action = actionEditorC->editedAction();
            }
            if (action != NULL)
            {
                ActionEditorController* actionEditorToRemove = _getActionEditorFromModelOfAction(action);
                if (actionEditorToRemove != NULL)
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
        if (actionInPalette != NULL) {
            actionInPalette->setmodelM(actionM);
        }
    }
}


/**
 * @brief Add an action VM at the time in ms
 * @param action model
 * @param line number
 */
void ScenarioController::addActionVMAtTime(ActionM* actionM, int timeInMs, int lineNumber)
{
    if (actionM != NULL)
    {
        ActionVM * actionVM = new ActionVM(actionM, timeInMs);

        // Insert the actionVM to the reight line number
        _insertActionVMIntoMapByLineNumber(actionVM, lineNumber);

        // If the action VM found a line to be insert in
        if (actionVM->lineInTimeLine() != -1)
        {
            // Add the new action VM to our map
            QList<ActionVM*> actionsVMsList;
            if (_mapActionsVMsInTimelineFromActionModel.contains(actionM)) {
                actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionM);
            }
            actionsVMsList.append(actionVM);
            _mapActionsVMsInTimelineFromActionModel.insert(actionM, actionsVMsList);

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
                if ( ((_nextActionToActivate == NULL) && (timeInMs >= _currentTime.msecsSinceStartOfDay()))
                     || ((_nextActionToActivate != NULL) && (timeInMs < _nextActionToActivate->startTime())) )
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
            actionVM = NULL;
        }
    }
}


/**
 * @brief Add an action VM at the current date time
 * @param action model
 */
void ScenarioController::addActionVMAtCurrentTime(ActionM* actionM)
{
    if (actionM != NULL) {
        addActionVMAtTime(actionM, _currentTime.msecsSinceStartOfDay());
    }
}


/**
 * @brief Remove an action VM from the time line
 * @param action view model
 */
void ScenarioController::removeActionVMFromTimeLine(ActionVM* actionVM)
{
    if ((actionVM != NULL) && _actionsInTimeLine.contains(actionVM))
    {
        _actionsInTimeLine.remove(actionVM);

        // Delete action vm from timeline
        if(actionVM->modelM() != NULL && _mapActionsVMsInTimelineFromActionModel.contains(actionVM->modelM()))
        {
            QList<ActionVM*> actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionVM->modelM());
            actionsVMsList.removeAll(actionVM);
            _mapActionsVMsInTimelineFromActionModel.insert(actionVM->modelM(),actionsVMsList);

            // Remove the action VM from the timeline map by line number
            if(actionVM->lineInTimeLine() != -1)
            {
                int lineNumber = actionVM->lineInTimeLine();
                if(_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber))
                {
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                    if ((actionVMSortedList != NULL) && actionVMSortedList->contains(actionVM))
                    {
                        // Remove item
                        actionVMSortedList->remove(actionVM);

                        // Check if the list is empty to remove the line
                        if(actionVMSortedList->count() == 0)
                        {
                            // We delete the last line, redice the number of display line in the limite of MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE
                            if(_linesNumberInTimeLine > MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE && lineNumber+2 == _linesNumberInTimeLine)
                            {
                                int nbOfDecrement = 1;

                                // Delete the last line, lets check if we can reduce the number of displayed lines
                                for (int lineTmp = lineNumber-1; lineTmp >= 0 ; --lineTmp)
                                {
                                    if (!_mapActionsVMsInTimelineFromLineIndex.contains(lineTmp))
                                    {
                                        nbOfDecrement++;
                                    }
                                    else {
                                        break;
                                    }
                                }

                                // Decrement the number of lines
                                setlinesNumberInTimeLine(_linesNumberInTimeLine-nbOfDecrement < MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE ? MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE : _linesNumberInTimeLine-nbOfDecrement);
                            }

                            _mapActionsVMsInTimelineFromLineIndex.remove(lineNumber);
                            delete actionVMSortedList;
                            actionVMSortedList = NULL;
                        }
                    }
                }
            }
        }


        // Delete the action editor if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromViewModelOfAction(actionVM);
        if (actionEditorC != NULL)
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
        if (actionM != NULL) {
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
        if (actionM != NULL) {
            actionM->resetConditionsConnections();
        }
    }
}


/**
 * @brief Test if an item can be inserted into a line number
 * @param actionM to insert
 * @param time into insert
 * @param line number
 * @param optional excluded actionVM from the check
 */
bool ScenarioController::canInsertActionVMTo(ActionM* actionMToInsert, int time, int lineNumber, ActionVM* excludedActionVM)
{
    bool canInsert = true;

    if (_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber))
    {
        bool reachPosition = false;
        ActionVM * previousActionVM = NULL;

        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
        if(actionVMSortedList != NULL)
        {
            for (int indexAction = 0; indexAction < actionVMSortedList->count(); ++indexAction)
            {
                ActionVM * actionVM = actionVMSortedList->at(indexAction);

                if(actionVM->modelM() != NULL && (excludedActionVM == NULL || excludedActionVM != actionVM))
                {
                    if(time < actionVM->startTime())
                    {
                        reachPosition = true;

                        // We reach the current action VM
                        // Check with the previous actionM
                        if(previousActionVM != NULL && previousActionVM->modelM() != NULL)
                        {
                            // If the previous action ends after the beginning of the new one, we skip it
                            if(previousActionVM->endTime()+MARGIN_FOR_ACTION_INSERTION_IN_MS >= time || previousActionVM->endTime() == -1)
                            {
                                canInsert = false;
                                break;
                            }
                        }

                        int insertionEndTime = time + MARGIN_FOR_ACTION_INSERTION_IN_MS;
                        int itemDurationTime = 0;
                        // If we insert a forever item and an item exists in the future, we cannot insert
                        if (actionMToInsert->validityDurationType() == ValidationDurationTypes::FOREVER)
                        {
                            // Try with the next line
                            canInsert = false;
                            break;
                        }
                        // If we insert a custom temporal action, we compute the end time
                        else if(actionMToInsert->validityDurationType() == ValidationDurationTypes::CUSTOM)
                        {
                            itemDurationTime = actionMToInsert->validityDuration();
                        }
                        // Compare with the time before revert if selected
                        if(actionMToInsert->shallRevertAfterTime()  && actionMToInsert->revertAfterTime() > itemDurationTime)
                        {
                            itemDurationTime = actionMToInsert->revertAfterTime();
                        }
                        insertionEndTime += itemDurationTime;

                        // If the next action starts after the end of the new one, we skip it
                        if(insertionEndTime >= actionVM->startTime())
                        {
                            canInsert = false;
                            break;
                        }

                        break;
                    }
                }

                previousActionVM = actionVM;
            }

            // If we didn't reach the position, we test with the previous action
            if (!reachPosition && (previousActionVM != NULL) && (excludedActionVM == NULL || excludedActionVM != previousActionVM))
            {
                // If the previous action ends after the beginning of the new one, we skip it
                if ((previousActionVM->endTime() + MARGIN_FOR_ACTION_INSERTION_IN_MS >= time) || (previousActionVM->endTime() == -1))
                {
                    canInsert = false;
                }
            }
        }
    }

    return canInsert;
}


/**
 * @brief Execute all effects of the action
 * @param action
 */
void ScenarioController::executeEffectsOfAction(ActionM* action)
{
    if ((action != NULL) && !action->effectsList()->isEmpty())
    {
        // Active the mapping if needed
        if ((_modelManager != NULL) && !_modelManager->isMappingActivated()) {
            _modelManager->setisMappingActivated(true);
        }

        // Execute the actions effects
        foreach (ActionEffectVM* effectVM, action->effectsList()->toList())
        {
            if ((effectVM != NULL) && (effectVM->modelM() != NULL))
            {
                // Get the pair with the agent and the command (with parameters) of the effect
                QPair<AgentInMappingVM*, QStringList> pairAgentAndCommandWithParameters = effectVM->modelM()->getAgentAndCommandWithParameters();

                // Execute the command for the agent
                _executeCommandForAgent(pairAgentAndCommandWithParameters.first, pairAgentAndCommandWithParameters.second);
            }
        }
    }
}


/**
 * @brief Clear the list of actions in the table / palette / timeline
 */
void ScenarioController::clearScenario()
{
    _stopScenario();

    // Clean-up current selection
    setselectedAction(NULL);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

    // Clean-up current selection
    setselectedActionVMInTimeline(NULL);

    // Clear the lists of action editors
    _hashActionEditorControllerFromModelOfAction.clear();
    _hashActionEditorControllerFromViewModelOfAction.clear();
    _openedActionsEditorsControllers.deleteAllItems();

    // Delete all models of action
    _actionsList.deleteAllItems();

    // Clear map
    _mapActionsFromActionName.clear();

    // Reset actions in palette
    foreach (ActionInPaletteVM* actionInPalette, _actionsInPaletteList.toList())
    {
        actionInPalette->setmodelM(NULL);
    }

    // Reset current time
    setcurrentTime(QTime::fromMSecsSinceStartOfDay(0));

    //qDebug() << "clearScenario:" << _currentTime << QTime::currentTime();
}


/**
 * @brief Move an actionVM to a start time position in ms and a specific line number
 * @param action VM
 * @param time in milliseconds
 * @param line number
 */
void ScenarioController::moveActionVMAtTimeAndLine(ActionVM* actionVM, int timeInMilliseconds, int lineNumber)
{
    if ((actionVM != NULL) && (timeInMilliseconds >= 0) && (lineNumber >= 0))
    {
        bool canInsert = canInsertActionVMTo(actionVM->modelM(), timeInMilliseconds, lineNumber,actionVM);
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
            actionVM->setstartTimeString(QString::number(hours).rightJustified(2, '0') + ":" + QString::number(minutes).rightJustified(2, '0') + ":" + QString::number(seconds).rightJustified(2, '0') + "." + QString::number(milliseconds).leftJustified(3, '0'));

            // If the line number has changed
            if (actionVM->lineInTimeLine() != lineNumber)
            {
                // Remove the actionVM from the previous line if different
                if (_mapActionsVMsInTimelineFromLineIndex.contains(actionVM->lineInTimeLine()))
                {
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(actionVM->lineInTimeLine());
                    if (actionVMSortedList != NULL) {
                        actionVMSortedList->remove(actionVM);
                    }
                }

                // Insert in the right line
                if (_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber))
                {
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                    if(actionVMSortedList != NULL)
                    {
                        // set the line number
                        actionVM->setlineInTimeLine(lineNumber);

                        // Insert the action
                        actionVMSortedList->append(actionVM);

                        // Add an extra line if inserted our actionVM at the last line
                        if (lineNumber >= _linesNumberInTimeLine - 1) {
                            setlinesNumberInTimeLine(lineNumber + 2);
                        }
                    }
                }
                else
                {
                    // Create a new list
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
                    actionVMSortedList->setSortProperty("startTime");
                    actionVMSortedList->append(actionVM);

                    // Set the line number
                    actionVM->setlineInTimeLine(lineNumber);

                    // Add into our map
                    _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber,actionVMSortedList);

                    // Add an extra line if inserted our actionVM at the last line
                    if (lineNumber >= _linesNumberInTimeLine - 1) {
                        setlinesNumberInTimeLine(lineNumber + 2);
                    }
                }
            }
        }
    }
}


/**
 * @brief Can delete an action from the list
 *        Check dependencies in the timeline
 * @param action to delete
 * @return can delete response
 */
bool ScenarioController::canDeleteActionFromList(ActionM* actionM)
{
    bool canBeDeleted = true;

    if (actionM != NULL) {
        canBeDeleted = !_mapActionsVMsInTimelineFromActionModel.contains(actionM);
    }

    return canBeDeleted;
}


/**
  * @brief Slot called when an agent is added in the mapping
  */
void ScenarioController::onAgentInMappingAdded(AgentInMappingVM* agentAdded)
{
    if ((agentAdded != NULL) && !_mapFromNameToAgentInMapping.contains(agentAdded->name()))
    {
        _mapFromNameToAgentInMapping.insert(agentAdded->name(), agentAdded);
        _agentsInMappingList.append(agentAdded);
    }
}


/**
  * @brief Slot called when an agent is removed from the mapping
  */
void ScenarioController::onAgentInMappingRemoved(AgentInMappingVM* agentRemoved)
{
    if ((agentRemoved != NULL) && _mapFromNameToAgentInMapping.contains(agentRemoved->name()))
    {
        _mapFromNameToAgentInMapping.remove(agentRemoved->name());
        _agentsInMappingList.remove(agentRemoved);
    }
}


/**
 * @brief Slot called when an action must be reverted
 * @param actionExecution
 */
void ScenarioController::onRevertAction(ActionExecutionVM* actionExecution)
{
    ActionVM* actionVM = qobject_cast<ActionVM*>(sender());
    if ((actionVM != NULL) && (actionExecution != NULL))
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
            actionVM->setcurrentExecution(NULL);

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
    if (actionVM != NULL)
    {
        // Initialize the start time
        int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay() - actionVM->startTime();

        // Define the exact start time for the next action execution
        if (!actionVM->executionsList()->isEmpty())
        {
            ActionExecutionVM* lastActionExecution = actionVM->executionsList()->at(actionVM->executionsList()->count() -1);
            if (lastActionExecution != NULL)
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
            actionVM->setcurrentExecution(NULL);

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
    _filteredListActionsInTimeLine.setTimeRange(startTimeInMilliseconds,endTimeInMilliseconds);
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
        if (_nextActionToActivate != NULL) {
            currentTimeInMilliSeconds = _nextActionToActivate->startTime();
        }
        ActionVM* tmpNextActionToActivate = NULL;


        // Traverse the list of actions to evaluate
        for (ActionVM* actionVM : _listOfActionsToEvaluate.toList())
        {
            if ((actionVM != NULL) && (actionVM->modelM() != NULL))
            {
                // Current time is after the start time of the action
                if (actionVM->startTime() <= currentTimeInMilliSeconds)
                {
                    ActionExecutionVM* actionExecution = actionVM->currentExecution();

                    // Check if an action execution exists and has not already been executed
                    if ((actionExecution != NULL) && !actionExecution->isExecuted())
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

        if (tmpNextActionToActivate != NULL)
        {
            // Set the next action to activate
            setnextActionToActivate(tmpNextActionToActivate);

            // Set the timer to the next action
            _timerToExecuteActions.start(_nextActionToActivate->startTime() - currentTimeInMilliSeconds);
        }
        else {
            // Reset the next action to activate
            setnextActionToActivate(NULL);
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

    // Traverse the list of active actions (the current time (is) was between start time and end time of these actions)
    foreach (ActionVM* actionVM, _listOfActiveActions.toList())
    {
        if ((actionVM != NULL) && (actionVM->modelM() != NULL))
        {
            ActionExecutionVM* actionExecution = actionVM->currentExecution();

            // Current time is after the start time of the action
            if ((actionExecution != NULL) && (actionVM->startTime() < currentTimeInMilliSeconds)
                    // the action has not finished or is forever
                    && (actionVM->endTime() == -1 || actionVM->endTime() > currentTimeInMilliSeconds))
            {

                // Not already executed
                if ((actionExecution != NULL) && !actionExecution->isExecuted())
                {
                    // Delay the current execution of this action
                    if (!actionVM->modelM()->isValid())
                    {
                        actionVM->delayCurrentExecution(currentTimeInMilliSeconds);
                    }
                    else {
                        _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);
                    }
                }
            }
            // Current time is after the end time of the action (or the action has no validity duration --> Immediate)
            else if (actionVM->endTime() <= currentTimeInMilliSeconds)
            {
                if ((actionExecution != NULL) && !actionVM->timerToReverse()->isActive())
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
                        actionVM->setcurrentExecution(NULL);

                        // only if the execution has not be done
                        if (!actionExecution->isExecuted())
                        {
                            actionVM->executionsList()->remove(actionExecution);

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
    int index = _mapActionsFromActionName.count() + 1;

    // QString::arg(int a, int fieldWidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
    // The a argument is expressed in base base, which is 10 by default.
    // fieldWidth specifies the minimum amount of space that a is padded to and filled with the character fillChar.
    QString tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));

    while(_mapActionsFromActionName.contains(tmpName))
    {
        index++;
        tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));
    }

    return tmpName;
}


/**
 * @brief Open the scenario from JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_openScenarioFromFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Open the scenario from JSON file" << scenarioFilePath;

        QFile jsonFile(scenarioFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                importScenarioFromJson(byteArrayOfJson);
            }
            else {
                qCritical() << "Can not open file" << scenarioFilePath;
            }
        }
        else {
            qWarning() << "There is no file" << scenarioFilePath;
        }
    }
}


/**
 * @brief Save the scenario to JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_saveScenarioToFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Save the scenario to JSON file" << scenarioFilePath;

        // Save the scenario
        QJsonObject scenarioJsonObject = _jsonHelper->exportScenario(_actionsList.toList(),_actionsInPaletteList.toList(),_actionsInTimeLine.toList());

        // Conversion into byteArray
        QByteArray byteArrayOfJson = QJsonDocument(scenarioJsonObject).toJson();

        QFile jsonFile(scenarioFilePath);
        if (jsonFile.open(QIODevice::WriteOnly))
        {
            jsonFile.write(byteArrayOfJson);
            jsonFile.close();
        }
        else {
            qCritical() << "Can not open file" << scenarioFilePath;
        }
    }
}


/**
 * @brief Insert an actionVM into our timeline
 * @param action view model
 * @return timeline line number
 */
void ScenarioController::_insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert, int lineNumberRef)
{
    if (actionVMToInsert != NULL)
    {
        int insertionStartTime = actionVMToInsert->startTime();

        int lineNumber = 0;
        if (lineNumberRef != -1) {
            lineNumber = lineNumberRef;
        }

        while (lineNumber < _linesNumberInTimeLine)
        {
            bool canInsert = canInsertActionVMTo(actionVMToInsert->modelM(), insertionStartTime, lineNumber);
            // Insert our item if possible
            if (canInsert)
            {
                if (_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber))
                {
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                    if(actionVMSortedList != NULL)
                    {
                        // set the line number
                        actionVMToInsert->setlineInTimeLine(lineNumber);

                        // Insert the action
                        actionVMSortedList->append(actionVMToInsert);

                        // Add an extra line if inserted our actionVM at the last line
                        if(lineNumber >= _linesNumberInTimeLine -1)
                        {
                            setlinesNumberInTimeLine(lineNumber+2);
                        }

                        break;
                    }
                }
                else
                {
                    // Create a new list
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
                    actionVMSortedList->setSortProperty("startTime");
                    actionVMSortedList->append(actionVMToInsert);

                    // Set the line number
                    actionVMToInsert->setlineInTimeLine(lineNumber);

                    // Add into our map
                    _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber,actionVMSortedList);

                    // Add an extra line if inserted our actionVM at the last line
                    if (lineNumber >= _linesNumberInTimeLine -1)
                    {
                        setlinesNumberInTimeLine(lineNumber+2);
                    }

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

            // Create a new list
            I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
            actionVMSortedList->setSortProperty("startTime");
            actionVMSortedList->append(actionVMToInsert);

            // Add into our map
            _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber, actionVMSortedList);
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
    if ((_modelManager != NULL) && !_modelManager->isMappingActivated()) {
        _modelManager->setisMappingActivated(true);
    }

    // Disconnect from signals
    foreach (ActionVM* actionVM, _listOfActionsToEvaluate.toList())
    {
        disconnect(actionVM, &ActionVM::revertAction, this, &ScenarioController::onRevertAction);
        disconnect(actionVM, &ActionVM::rearmAction, this, &ScenarioController::onRearmAction);
    }

    // Reset lists and next action to activate
    _listOfActionsToEvaluate.clear();
    _listOfActiveActions.clear();
    setnextActionToActivate(NULL);

    // Look for the current and futures actions
    ActionVM* tmpNextActionToActivate = NULL;
    for (ActionVM* actionVM : _actionsInTimeLine.toList())
    {
        if ((actionVM != NULL) && ((actionVM->endTime() > currentTimeInMilliSeconds) || (actionVM->endTime() == -1)) )
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
                if ( (tmpNextActionToActivate == NULL)
                     || ((tmpNextActionToActivate != NULL) && (tmpNextActionToActivate->startTime() > actionVM->startTime())) )
                {
                    tmpNextActionToActivate = actionVM;
                }

                // Add our action
                _listOfActionsToEvaluate.append(actionVM);
            }
        }
    }

    if (tmpNextActionToActivate != NULL) {
        setnextActionToActivate(tmpNextActionToActivate);
    }

    // Initialize the connections for conditions of all actions
    initializeConditionsConnectionsOfAllActions();

    // Save the time of the day
    _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut = QTime::currentTime().msecsSinceStartOfDay();

    //qDebug() << "_startScenario:" << _currentTime << QTime::currentTime();

    // Start timers
    // init the timer with the time of the next action execution
    if (_nextActionToActivate != NULL) {
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
    foreach (ActionVM* actionVM, _listOfActiveActions.toList())
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

    // Reset the next action VM to activate
    setnextActionToActivate(NULL);

    // Reset the connections for conditions of all actions
    resetConditionsConnectionsOfAllActions();
}


/**
 * @brief Get the agent in mapping from an agent name
 * @param agentName
 * @return
 */
AgentInMappingVM* ScenarioController::_getAgentInMappingFromName(QString agentName)
{
    if (_mapFromNameToAgentInMapping.contains(agentName)) {
        return _mapFromNameToAgentInMapping.value(agentName);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Execute reverse effects of an action
 * @param actionExecution
 */
void ScenarioController::_executeReverseEffectsOfAction(ActionExecutionVM* actionExecution)
{
    if ((actionExecution != NULL) && actionExecution->shallRevert())
    {
        // Get the list of pairs <agent name, reverse command (and parameters)>
        QList<QPair<QString, QStringList>> reverseCommandsForAgents = actionExecution->getReverseCommands();

        for (int i = 0; i < reverseCommandsForAgents.count(); i++)
        {
            QPair<QString, QStringList> pairAgentNameAndReverseCommand = reverseCommandsForAgents.at(i);

            AgentInMappingVM* agent = _getAgentInMappingFromName(pairAgentNameAndReverseCommand.first);
            if (agent != NULL)
            {
                // Execute the (reverse) command for the agent
                _executeCommandForAgent(agent, pairAgentNameAndReverseCommand.second);
            }
        }
    }
}


/**
 * @brief Execute a command for an agent
 * @param agent
 * @param commandAndParameters
 */
void ScenarioController::_executeCommandForAgent(AgentInMappingVM* agent, QStringList commandAndParameters)
{
    if ((agent != NULL) && (commandAndParameters.count() > 0))
    {
        qInfo() << "Execute command" << commandAndParameters << "for agent" << agent->name() << "(" << agent->peerIdsList().count() << "peer ids)";

        QString command = commandAndParameters.at(0);

        // START
        if (command == command_StartAgent)
        {
            foreach (AgentM* model, agent->models()->toList())
            {
                // Check if the model has a hostname
                if ((model != NULL) && !model->hostname().isEmpty())
                {
                    // Emit signal "Command asked to launcher"
                    Q_EMIT commandAskedToLauncher(command, model->hostname(), model->commandLine());
                }
            }
        }
        // STOP or MUTE/UN-MUTE or FREEZE/UN-FREEZE
        else if ( (command == command_StopAgent)
                  || (command == command_MuteAgent) || (command == command_UnmuteAgent)
                  || (command == command_FreezeAgent) || (command == command_UnfreezeAgent) )
        {
            // Emit signal "Command asked to agent"
            Q_EMIT commandAskedToAgent(agent->peerIdsList(), command);
        }
        // MAP or UNMAP
        else if ((command == "MAP") || (command == "UNMAP"))
        {
            if (commandAndParameters.count() == 4)
            {
                QString inputName = commandAndParameters.at(1);
                QString outputAgentName = commandAndParameters.at(2);
                QString outputName = commandAndParameters.at(3);

                // Emit signal "Command asked to agent about Mapping Input"
                Q_EMIT commandAskedToAgentAboutMappingInput(agent->peerIdsList(), command, inputName, outputAgentName, outputName);
            }
            else {
                qCritical() << "Wrong number of parameters (" << commandAndParameters.count() << ") to map an input of agent" << agent->name();
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
                Q_EMIT commandAskedToAgentAboutSettingValue(agent->peerIdsList(), command, agentIOPName, value);
            }
            else {
                qCritical() << "Wrong number of parameters (" << commandAndParameters.count() << ") to set a value to agent" << agent->name();
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
    if ((actionVM != NULL) && (actionExecution != NULL))
    {
        if (actionExecution->shallRevert()) {
            // Initialize the reverse command (and parameters) for each effect
            actionExecution->initReverseCommandsForEffects(actionVM->modelM()->effectsList()->toList());
        }

        // Execute all effects of the action
        executeEffectsOfAction(actionVM->modelM());

        // Notify the action that its effects has been executed
        actionVM->effectsExecuted(currentTimeInMilliSeconds);
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
        return NULL;
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
        return NULL;
    }
}
