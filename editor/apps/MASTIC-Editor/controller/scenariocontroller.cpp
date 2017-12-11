/*
 *	ScenarioController
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

#include "scenariocontroller.h"

#include <QDebug>
#include <QFileDialog>

/**
 * @brief Constructor
 * @param modelManager
 * @param scenariosPath Path of files with scenarios
 * @param parent
 */
ScenarioController::ScenarioController(MasticModelManager* modelManager,
                                       QString scenariosPath,
                                       QObject *parent) : QObject(parent),
    _selectedAction(NULL),
    _selectedActionVMInTimeline(NULL),
    _linesNumberInTimeLine(MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE),
    _isPlaying(false),
    _currentTime(QTime::fromMSecsSinceStartOfDay(0)),
    _nextActionVMToActive(NULL),
    _modelManager(modelManager),
    _scenariosDirectoryPath(scenariosPath),
    _jsonHelper(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set the agent in mapping list sort by name property
    _agentsInMappingList.setSortProperty("name");

    // Fill state comparisons types list
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::ON);
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::OFF);

    // Fill value comparisons types list
    _comparisonsValuesTypesList.fillWithAllEnumValues();
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::ON);
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::OFF);

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

    QDate today = QDate::currentDate();
    _scenariosDefaultFilePath = QString("%1scenarios_%2.json").arg(_scenariosDirectoryPath, today.toString("ddMMyy"));

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);

    // Add the first timeline line
    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
    actionVMSortedList->setSortProperty("startTime");
    // Add into our map
    _mapActionsVMsInTimelineFromLineIndex.insert(0,actionVMSortedList);

    // Set the sort property for the actionVM in the time line
    _actionsInTimeLine.setSortProperty("startTime");

    // Set the sort property for the actionsVM to evaluate
    _actionsVMToEvaluateVMList.setSortProperty("startTime");

    // Set the sort property for the active actionsVM
    _activeActionsVMList.setSortProperty("startTime");


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
    _stopScenario();

    disconnect(&_timerToExecuteActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_ExecuteActions);
    disconnect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_DelayOrExecuteActions);

    // Clean-up current selection
    setselectedAction(NULL);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

    // Clean-up current selection
    setselectedActionVMInTimeline(NULL);

    // Delete actions VM from the palette
    _actionsInPaletteList.deleteAllItems();

    // Clear the list of editor opened
    _mapActionsEditorControllersFromActionM.clear();
    _mapActionsEditorControllersFromActionVM.clear();
    _openedActionsEditorsControllers.deleteAllItems();

    // Delete actions Vm List
    _actionsList.deleteAllItems();

    // Clear map
    _mapActionsFromActionName.clear();

    _modelManager = NULL;

    // Delete json helper
    if(_jsonHelper != NULL)
    {
        delete _jsonHelper;
        _jsonHelper = NULL;
    }
}


/**
  * @brief Open the action editor
  * @param action view model
  */
void ScenarioController::openActionEditor(ActionM* actionM)
{
    // We check that or editor is not already opened
    if (!_mapActionsEditorControllersFromActionM.contains(actionM))
    {
        // Create an empty action if we create a new one
        if(actionM != NULL)
        {
            setselectedAction(actionM);

            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionM,agentsInMappingList());

            // Add action into our opened actions
            _mapActionsEditorControllersFromActionM.insert(actionM,actionEditorC);

            // Add to list
            _openedActionsEditorsControllers.append(actionEditorC);
        }
        // Set selected action
        else
        {
            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionM ,agentsInMappingList());

            // Add action into our opened actions
            _mapActionsEditorControllersFromActionM.insert(actionEditorC->editedAction(),actionEditorC);

            // Add to list
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }
}


/**
  * @brief Open the action editor
  * @param action view model
  */
void ScenarioController::openActionEditorFromActionVM(ActionVM* actionVM)
{
    if (actionVM != NULL && actionVM->modelM() != NULL)
    {
        // We check that or editor is not already opened
        if (!_mapActionsEditorControllersFromActionVM.contains(actionVM))
        {
            setselectedAction(actionVM->modelM());

            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionVM->modelM(),agentsInMappingList());

            // Set the original view model
            actionEditorC->setoriginalViewModel(actionVM);

            // Create the temporary edited action view model
            ActionVM * temporaryActionVM = new ActionVM(NULL,-1);
            temporaryActionVM->setstartTimeString(actionVM->startTimeString());
            temporaryActionVM->setcolor(actionVM->color());
            actionEditorC->seteditedViewModel(temporaryActionVM);

            // Add action into our opened actions
            _mapActionsEditorControllersFromActionVM.insert(actionVM,actionEditorC);

            // Add to list
            _openedActionsEditorsControllers.append(actionEditorC);
        }
    }

}


/**
  * @brief Delete an action from the list
  * @param action view model
  */
void ScenarioController::deleteAction(ActionM * actionM)
{
    // Delete the popup if necessary
    if(actionM != NULL && _mapActionsEditorControllersFromActionM.contains(actionM))
    {
        _mapActionsEditorControllersFromActionM.remove(actionM);
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionM.value(actionM);
        if(actionEditorC != NULL)
        {
            _openedActionsEditorsControllers.remove(actionEditorC);
            delete actionEditorC;
            actionEditorC = NULL;
        }
    }

    // Unselect our action if needed
    if (_selectedAction == actionM) {
        setselectedAction(NULL);
    }

    // Remove action from the palette if exists
    for (int index = 0; index < _actionsInPaletteList.count(); ++index)
    {
        ActionInPaletteVM* actionInPaletteVM = _actionsInPaletteList.at(index);
        if (actionInPaletteVM->modelM() != NULL && actionInPaletteVM->modelM() == actionM)
        {
            setActionInPalette(index,NULL);
        }
    }

    // Remove action from the timeline if exists
    if(_mapActionsVMsInTimelineFromActionModel.contains(actionM))
    {
        QList<ActionVM*> actionVMList = _mapActionsVMsInTimelineFromActionModel.value(actionM);

        foreach (ActionVM* actionVM, actionVMList)
        {
            removeActionVMFromTimeLine(actionVM);
        }
    }

    // Delete the action item
    if(_actionsList.contains(actionM))
    {
        _actionsList.remove(actionM);

        _mapActionsFromActionName.remove(actionM->name());

        delete actionM;
        actionM = NULL;
    }


}

/**
  * @brief Valide action edition
  * @param action editor controller
  */
void ScenarioController::valideActionEditor(ActionEditorController* actionEditorC)
{
    // Valide modification
    actionEditorC->validateModification();

    ActionM* originalActionM = actionEditorC->originalAction();

    // We check that or editor is not already opened
    if (!_actionsList.contains(originalActionM))
    {
        // Insert in to the list
        _actionsList.append(originalActionM);

        // Insert into the map
        _mapActionsFromActionName.insert(originalActionM->name(),originalActionM);
    }

    // Set selected action
    setselectedAction(originalActionM);
}

/**
  * @brief Close action edition
  * @param action editor controller
  */
void ScenarioController::closeActionEditor(ActionEditorController* actionEditorC)
{
    // Remove action editor controller from action model
    if(actionEditorC->originalViewModel() == NULL)
    {
        ActionM* actionM = actionEditorC->originalAction() != NULL ? actionEditorC->originalAction() : actionEditorC->editedAction();
        // Delete the popup if necessary
        if(actionM != NULL && _mapActionsEditorControllersFromActionM.contains(actionM))
        {
            ActionEditorController* actionEditorToRemove = _mapActionsEditorControllersFromActionM.value(actionM);

            _mapActionsEditorControllersFromActionM.remove(actionM);
            _openedActionsEditorsControllers.remove(actionEditorToRemove);

            delete actionEditorToRemove;
            actionEditorToRemove = NULL;
        }
    }
    // Remove action editor controller from action view model
    else
    {
        // Delete the popup if necessary
        if(_mapActionsEditorControllersFromActionVM.contains(actionEditorC->originalViewModel()))
        {
            ActionEditorController* actionEditorToRemove = _mapActionsEditorControllersFromActionVM.value(actionEditorC->originalViewModel());

            _mapActionsEditorControllersFromActionVM.remove(actionEditorToRemove->originalViewModel());
            _openedActionsEditorsControllers.remove(actionEditorToRemove);

            delete actionEditorToRemove;
            actionEditorToRemove = NULL;
        }
    }
}


/**
  * @brief slot on agent added in mapping
  */
void ScenarioController::onAgentInMappingAdded(AgentInMappingVM * agentAdded)
{
    if (!_agentsInMappingList.contains(agentAdded)) {
        _agentsInMappingList.append(agentAdded);
    }
}


/**
  * @brief slot on agent removed in mapping
  */
void ScenarioController::onAgentInMappingRemoved(AgentInMappingVM * agentRemoved)
{
    if (_agentsInMappingList.contains(agentRemoved)) {
        _agentsInMappingList.remove(agentRemoved);
    }
}


/**
 * @brief Get a new action name
 */
QString ScenarioController::_buildNewActionName()
{
    // Remove the effect
    int index = _mapActionsFromActionName.count()+1;
    QString tmpName = "Action_"+QString("%1").arg(index, 3,10, QChar('0'));

    while(_mapActionsFromActionName.contains(tmpName))
    {
        index++;
        tmpName = "Action_"+QString("%1").arg(index, 3, 10, QChar('0'));
    }

    return tmpName;
}


/**
 * @brief Set an action into the palette at index
 * @param index where to insert the action
 * @param action to insert
 */
void ScenarioController::setActionInPalette(int index, ActionM* actionM)
{
    // Set action in palette
    if (index < _actionsInPaletteList.count())
    {
        _actionsInPaletteList.at(index)->setmodelM(actionM);
    }
}


/**
 * @brief Import a scenario a file (actions, palette, timeline actions )
 */
void ScenarioController::importScenarioFromFile()
{
    // "File Dialog" to get the files (paths) to open
    QString scenarioFilePath = QFileDialog::getOpenFileName(NULL,
                                                                "Open scenario",
                                                                _scenariosDirectoryPath,
                                                                "JSON (*.json)");

    // Import the scenario from JSON file
    _importScenarioFromFile(scenarioFilePath);
}

/**
 * @brief Import the scenario from JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_importScenarioFromFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Import the scenario from JSON file" << scenarioFilePath;

        QFile jsonFile(scenarioFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                // Initialize agents lists from JSON file
                scenario_import_actions_lists_t * scenarioToImport = _jsonHelper->initActionsList(byteArrayOfJson, _agentsInMappingList.toList());
                if(scenarioToImport != NULL)
                {
                    // Append the list of actions
                    if(scenarioToImport->actionsInTableList.count() > 0)
                    {
                        // Add each actions to out list
                        foreach (ActionM* actionM, scenarioToImport->actionsInTableList)
                        {
                            // Add action into the list
                            _actionsList.append(actionM);

                            // Add action into the map
                            _mapActionsFromActionName.insert(actionM->name(),actionM);
                        }
                    }

                    // Set the list of actions in palette
                    if(scenarioToImport->actionsInPaletteList.count() > 0)
                    {
                        foreach (ActionInPaletteVM* actionInPalette, scenarioToImport->actionsInPaletteList)
                        {
                            if(actionInPalette->modelM() != NULL)
                            {
                                setActionInPalette(actionInPalette->indexInPanel(), actionInPalette->modelM());
                            }

                            delete actionInPalette;
                            actionInPalette = NULL;
                        }
                    }

                    // Append the list of actions in timeline
                    if(scenarioToImport->actionsInTimelineList.count() > 0)
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
                            if(_linesNumberInTimeLine < lineNumber+1)
                            {
                                setlinesNumberInTimeLine(lineNumber+1);
                            }
                        }
                    }

                    delete scenarioToImport;
                    scenarioToImport = NULL;
                }

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
 * @brief Export a scenario to the default file (actions, palette, timeline actions)
 */
void ScenarioController::exportScenarioToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString scenarioFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save scenario",
                                                              _scenariosDirectoryPath,
                                                              "JSON (*.json)");

    if(!scenarioFilePath.isEmpty()) {
        // Export the scenario to JSON file
        _exportScenarioToFile(scenarioFilePath);
    }
}

/**
 * @brief Export the scenario to JSON file
 * @param scenarioFilePath
 */
void ScenarioController::_exportScenarioToFile(QString scenarioFilePath)
{
    if (!scenarioFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Save the scenario to JSON file" << scenarioFilePath;

        // Export the scenario
        QByteArray byteArrayOfJson = _jsonHelper->exportScenario(_actionsList.toList(),_actionsInPaletteList.toList(),_actionsInTimeLine.toList());

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
            if(_mapActionsVMsInTimelineFromActionModel.contains(actionM)) {
                actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionM);
            }
            actionsVMsList.append(actionVM);
            _mapActionsVMsInTimelineFromActionModel.insert(actionM, actionsVMsList);

            // Add the action VM to the timeline
            _actionsInTimeLine.append(actionVM);

            // If scenario is playing we add the actionVM to the active ones
            if(_isPlaying)
            {
                // Initialize the action connections
                actionVM->modelM()->initializeConditionsConnections();

                // Connect the revert action
                connect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);

                // Add action to the ones to check
                _actionsVMToEvaluateVMList.append(actionVM);

                // The new actionVM is the next one to active, we change the next timer trigger
                if(         (_nextActionVMToActive == NULL && timeInMs >= _currentTime.msecsSinceStartOfDay())
                        ||  (_nextActionVMToActive != NULL && timeInMs < _nextActionVMToActive->startTime()))
                {
                    // Stop the timer if necessary
                    if(_timerToExecuteActions.isActive())
                    {
                        _timerToExecuteActions.stop();
                    }

                    setnextActionVMToActive(actionVM);
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
void ScenarioController::removeActionVMFromTimeLine(ActionVM * actionVM)
{
    if(actionVM != NULL && _actionsInTimeLine.contains(actionVM))
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

        // Delete action view model editor if exists
        if(_mapActionsEditorControllersFromActionVM.contains(actionVM))
        {
            _mapActionsEditorControllersFromActionVM.remove(actionVM);
            ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionVM);
            if(actionEditorC != NULL)
            {
                _openedActionsEditorsControllers.remove(actionEditorC);
                delete actionEditorC;
                actionEditorC = NULL;
            }

        }

        delete actionVM;
        actionVM = NULL;
    }
}

/**
 * @brief Make conditions connections
 */
void ScenarioController::conditionsConnect()
{
    // COnnect all actions from the list
    foreach (ActionM * actionM, _actionsList.toList())
    {
        actionM->initializeConditionsConnections();
    }
}

/**
 * @brief Conditions disconnections
 */
void ScenarioController::conditionsDisconnect()
{
    // COnnect all actions from the list
    foreach (ActionM * actionM, _actionsList.toList())
    {
        actionM->resetConditionsConnections();
    }
}


/**
 * @brief Insert an actionVM into our timeline
 * @param action view model
 * @return timeline line number
 */
void ScenarioController::_insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert, int lineNumberRef)
{
    int insertionStartTime = actionVMToInsert->startTime();

    int lineNumber = lineNumberRef != -1 ? lineNumberRef : 0;
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
            else {
                // Create a new list
                I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
                actionVMSortedList->setSortProperty("startTime");
                actionVMSortedList->append(actionVMToInsert);

                // Set the line number
                actionVMToInsert->setlineInTimeLine(lineNumber);

                // Add into our map
                _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber,actionVMSortedList);

                // Add an extra line if inserted our actionVM at the last line
                if(lineNumber >= _linesNumberInTimeLine -1)
                {
                    setlinesNumberInTimeLine(lineNumber+2);
                }

                break;
            }
        }

        if(lineNumberRef != -1)
        {
            break;
        }
        else {
            lineNumber++;
        }

    }

    // If the action has not been inserted yet, we create a new line
    // only if we are not dropping at a busy position the actionVM
    if(actionVMToInsert->lineInTimeLine() == -1 && lineNumberRef == -1)
    {
        // Add an extra line if inserted our actionVM at the last line
        if(lineNumber >= _linesNumberInTimeLine -1)
        {
            setlinesNumberInTimeLine(lineNumber+2);
        }

        // Create the new line number
        actionVMToInsert->setlineInTimeLine(lineNumber);

        // Create a new list
        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
        actionVMSortedList->setSortProperty("startTime");
        actionVMSortedList->append(actionVMToInsert);

        // Add into our map
        _mapActionsVMsInTimelineFromLineIndex.insert(lineNumber,actionVMSortedList);
    }
}


/**
 * @brief Test if an item can be inserted into a line number
 * @param actionM to insert
 * @param time into insert
 * @param line number
 */
bool ScenarioController::canInsertActionVMTo(ActionM* actionMToInsert, int time, int lineNumber)
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

                if(actionVM->modelM() != NULL)
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
                        if(actionMToInsert->validityDurationType() == ValidationDurationType::FOREVER)
                        {
                            // Try with the next line
                            canInsert = false;
                            break;
                        }
                        // If we insert a custom temporal action, we compute the end time
                        else if(actionMToInsert->validityDurationType() == ValidationDurationType::CUSTOM)
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
            if (!reachPosition && (previousActionVM != NULL))
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
    if ((action != NULL) && (action->effectsList()->count() > 0))
    {
        // Active the mapping if needed
        if ((_modelManager != NULL) && !_modelManager->isActivatedMapping()) {
            _modelManager->setisActivatedMapping(true);
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
 * @brief Called when our timer time out to handle the scenario and execute actions
 */
void ScenarioController::_onTimeout_ExecuteActions()
{
    if(_actionsVMToEvaluateVMList.count() > 0)
    {
        QList<ActionVM *> actionsVMToRemove;
        ActionVM * actionToExecute = _actionsVMToEvaluateVMList.at(0);
        ActionVM * nextActionToExecute = NULL;

        int currentTimeInMilliSeconds = actionToExecute->startTime();

        // Traverse the list of active actions
        for (int indexAction = 0; indexAction < _actionsVMToEvaluateVMList.count(); ++indexAction)
        {
            ActionVM* actionVM = _actionsVMToEvaluateVMList.at(indexAction);
            if(actionVM != NULL)
            {
                // Current time is after the start time of action
                if ((actionVM != NULL) && (actionVM->modelM() != NULL) && (actionVM->startTime() <= currentTimeInMilliSeconds))
                {
                    ActionExecutionVM* actionExecution = actionVM->currentExecution();

                    // Check if an action execution exists and has not already been executed
                    if ((actionExecution != NULL) && !actionExecution->isExecuted())
                    {
                        if (actionVM->modelM()->isValid()
                            // And the action has no validation duration
                            && actionVM->modelM()->validityDurationType() == ValidationDurationType::IMMEDIATE)
                        {
                            // Execute action
                            _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);

                            // Disconnect before removing from the list
                            disconnect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                        }
                        else if (actionVM->modelM()->validityDurationType() != ValidationDurationType::IMMEDIATE)
                        {
                            if (actionVM->modelM()->isValid())
                            {
                                // Execute action
                                _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);
                            }

                            // Add the action VM to the active list
                            _activeActionsVMList.append(actionVM);
                        }
                    }

                    // Remove from the list of "active" actions
                    actionsVMToRemove.append(actionVM);
                }
                // Mission are in the future
                else {

                    // Set the next action to launch
                    nextActionToExecute = actionVM;

                    break;
                }
            }
        }

        // Remove processed actions VM
        if(actionsVMToRemove.count() > 0)
        {
            foreach(ActionVM* actionRemoved, actionsVMToRemove)
            {
                _actionsVMToEvaluateVMList.remove(actionRemoved);
            }
        }

        // Set the next action timer
        if(nextActionToExecute != NULL)
        {
            // Set the next action VM to active
            setnextActionVMToActive(nextActionToExecute);

            // Set the timer to the next action
            _timerToExecuteActions.start(nextActionToExecute->startTime() - currentTimeInMilliSeconds);

        }
        else {
            // Reset the next action VM to active
            setnextActionVMToActive(NULL);
        }
    }
 }


/**
 * @brief Called at each interval of our timer to delay actions when their conditions are not valid or execute them otherwise
 */
void ScenarioController::_onTimeout_DelayOrExecuteActions()
{
    // Move the currenttime
    int currentTimeOfDay = QTime::currentTime().msecsSinceStartOfDay();

    setcurrentTime(_currentTime.addMSecs(currentTimeOfDay - _scenarioStartingTimeInMs));

    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    // Traverse the list of active actions
    foreach (ActionVM* actionVM, _activeActionsVMList.toList())
    {
        if(actionVM != NULL)
        {
            ActionExecutionVM* actionExecution = actionVM->currentExecution();

            // Current time is after the start time of action
            if ((actionExecution != NULL) && (actionVM->modelM() != NULL) && (actionVM->startTime() < currentTimeInMilliSeconds)
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
            // Current time is after the end time of action (or the action has no validity duration --> Immediate)
            else if(actionVM->endTime() <= currentTimeInMilliSeconds)
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
                            || (actionExecution->neverExecuted() && (actionVM->modelM()->validityDurationType() == ValidationDurationType::CUSTOM)) )
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
                    // Remove from the list of "active" actions
                    disconnect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);
                }
                _activeActionsVMList.remove(actionVM);
            }
        }
    }

    // Save our scenario start
    _scenarioStartingTimeInMs = currentTimeOfDay;

    if (_isPlaying) {
        _timerToRegularlyDelayActions.start(INTERVAL_DELAY_ACTIONS);
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
    if ((_modelManager != NULL) && !_modelManager->isActivatedMapping()) {
        _modelManager->setisActivatedMapping(true);
    }

    // Set the list of Actions to process at currentTime
    foreach (ActionVM* actionVM, _actionsVMToEvaluateVMList.toList())
    {
        disconnect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);
    }
    _actionsVMToEvaluateVMList.clear();
    _activeActionsVMList.clear();
    setnextActionVMToActive(NULL);

    // Look for the current and futures actions
    ActionVM * nextActionToLaunch = NULL;
    foreach (ActionVM* actionVM , _actionsInTimeLine.toList())
    {
        if ((actionVM->endTime() > currentTimeInMilliSeconds) || (actionVM->endTime() == -1))
        {
            // Connect on the action revert signal
            connect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);

            // Initialize the action view model at a specific time.
            actionVM->resetDataFrom(currentTimeInMilliSeconds);

            if(actionVM->startTime() <= currentTimeInMilliSeconds)
            {
                // Add our action
                _activeActionsVMList.append(actionVM);
            } else {
                // Check the next mission to launch
                if(nextActionToLaunch == NULL || nextActionToLaunch->startTime() > actionVM->startTime())
                {
                    nextActionToLaunch = actionVM;
                }

                // Add our action
                _actionsVMToEvaluateVMList.append(actionVM);
            }
        }
    }

    if (nextActionToLaunch != NULL)
    {
        setnextActionVMToActive(nextActionToLaunch);
    }

    // Connect actions conditions
    conditionsConnect();

    // Save our scenario start
    _scenarioStartingTimeInMs = QTime::currentTime().msecsSinceStartOfDay();

    // Start timers
    // init the timer with the time of the next action execution
    if(_nextActionVMToActive != NULL)
    {
        _timerToExecuteActions.start(_nextActionVMToActive->startTime() - _currentTime.msecsSinceStartOfDay());
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
    if(_timerToExecuteActions.isActive())
    {
        _timerToExecuteActions.stop();
    }
    if(_timerToRegularlyDelayActions.isActive())
    {
        _timerToRegularlyDelayActions.stop();
    }

    // Desactive revert timers
    foreach (ActionVM* actionVM, _activeActionsVMList.toList())
    {
        if(actionVM->timerToReverse()->isActive())
        {
            actionVM->timerToReverse()->stop();
        }
        disconnect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);
    }

    // Reset the next action VM to active
    setnextActionVMToActive(NULL);

    // Disconnect actions conditions
    conditionsDisconnect();


}


/**
 * @brief Get the agent in mapping from an agent name
 * @param agentName
 * @return
 */
AgentInMappingVM* ScenarioController::_getAgentInMappingFromName(QString agentName)
{
    /*if (_mapFromNameToAgentInMapping.contains(name)) {
        return _mapFromNameToAgentInMapping.value(name);
    }
    else {
        return NULL;
    }*/

    foreach (AgentInMappingVM* agent, _agentsInMappingList.toList()) {
        if ((agent != NULL) && (agent->name() == agentName)) {
            return agent;
        }
    }
    return NULL;
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

        // RUN
        if (command == "RUN")
        {
            foreach (AgentM* model, agent->models()->toList())
            {
                // Check if the model has a hostname
                if ((model != NULL) && !model->hostname().isEmpty())
                {
                    // Emit signal "Command asked to agent"
                    Q_EMIT commandAskedToLauncher(command, model->hostname(), model->commandLine());
                }
            }
        }
        // STOP
        else if (command == "STOP")
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
 * @brief Set the current time in milliseconds
 * @param current time in milliseconds
 */
void ScenarioController::updateCurrentTimeInMs(int currentTimeInMs)
{
    if(currentTimeInMs > 0)
    {
        setcurrentTime(QTime::fromMSecsSinceStartOfDay(currentTimeInMs));
    }
    else {
        setcurrentTime(QTime::fromMSecsSinceStartOfDay(0));
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
    if(actionVM != NULL && actionExecution != NULL)
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
  * @brief slot on the action reversion
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
        if(actionVM->endTime() >= 0 && currentTimeInMilliSeconds >= actionVM->endTime())
        {
            // ...we remove the current execution
            actionVM->setcurrentExecution(NULL);

            // Remove from the list of "active" actions
            disconnect(actionVM,&ActionVM::revertAction, this, &ScenarioController::onRevertAction);
        }
    }
}
