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



//--------------------------------------------------------------
//
// ScenarioController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param scenarios files path
 * @param parent
 */
ScenarioController::ScenarioController(QString scenariosPath, QObject *parent) : QObject(parent),
    _selectedAction(NULL),
    _selectedActionVMInTimeline(NULL),
    _linesNumberInTimeLine(MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE),
    _isPlaying(false),
    _currentTime(QTime::fromMSecsSinceStartOfDay(0)),
    _scenariosDirectoryPath(scenariosPath)
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

    // Set the sort property for the active actionsVM
    _activeActionsVMList.setSortProperty("startTime");


    //
    // Init the timers
    //
    connect(&_timerToExecuteActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_ExecuteActions);

    _timerToRegularlyDelayActions.setInterval(INTERVAL_DELAY_ACTIONS);
    connect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_DelayActions);
}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    _stopScenario();

    disconnect(&_timerToExecuteActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_ExecuteActions);
    disconnect(&_timerToRegularlyDelayActions, &QTimer::timeout, this, &ScenarioController::_onTimeout_DelayActions);

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
}


/**
  * @brief Open the action editor
  * @param action view model
  */
void ScenarioController::openActionEditor(ActionM* actionM)
{
    // We check that or editor is not already opened
    if(_mapActionsEditorControllersFromActionM.contains(actionM) == false)
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
    if(actionVM != NULL && actionVM->actionModel() != NULL)
    {
        // We check that or editor is not already opened
        if(_mapActionsEditorControllersFromActionVM.contains(actionVM) == false)
        {
            setselectedAction(actionVM->actionModel());

            // Create action editor controller
            ActionEditorController* actionEditorC = new ActionEditorController(_buildNewActionName(), actionVM->actionModel(),agentsInMappingList());

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
        if(actionInPaletteVM->actionModel() != NULL && actionInPaletteVM->actionModel() == actionM)
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
    if(_actionsList.contains(originalActionM) == false)
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
    if(_agentsInMappingList.contains(agentAdded) == false)
    {
        _agentsInMappingList.append(agentAdded);
    }
}

/**
  * @brief slot on agent removed in mapping
  */
void ScenarioController::onAgentInMappingRemoved(AgentInMappingVM * agentRemoved)
{
    if(_agentsInMappingList.contains(agentRemoved) == true)
    {
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
    if(index < _actionsInPaletteList.count())
    {
        _actionsInPaletteList.at(index)->setactionModel(actionM);
    }
}

/**
 * @brief Import a scenario a file (actions, palette, timeline actions )
 */
void ScenarioController::importScenarioFromFile()
{
    // "File Dialog" to get the files (paths) to open
    QString scenarioFilePath = QFileDialog::getOpenFileName(NULL,
                                                                "Importer un fichier scénario",
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

                // Initialize agents list from JSON file
                QPair< QPair< QList<ActionM*>, QList<ActionInPaletteVM*> > , QList<ActionVM*> > scenarioToImport = _jsonHelper->initActionsList(byteArrayOfJson, _agentsInMappingList.toList());

                // Append the list of actions
                if(scenarioToImport.first.first.count() > 0)
                {
                    // Add each actions to out list
                    foreach (ActionM* actionM, scenarioToImport.first.first)
                    {
                        // Add action into the list
                        _actionsList.append(actionM);

                        // Add action into the map
                        _mapActionsFromActionName.insert(actionM->name(),actionM);
                    }
                }

                // Set the list of actions in palette
                if(scenarioToImport.first.second.count() > 0)
                {
                    foreach (ActionInPaletteVM* actionInPalette, scenarioToImport.first.second)
                    {
                        if(actionInPalette->actionModel() != NULL)
                        {
                            setActionInPalette(actionInPalette->indexInPanel(),actionInPalette->actionModel());
                        }

                        delete actionInPalette;
                        actionInPalette = NULL;
                    }
                }

                // Append the list of actions in timeline
                if(scenarioToImport.second.count() > 0)
                {
                    // Add each actionVM in to the right line of our timeline
                    foreach (ActionVM* actionVM, scenarioToImport.second)
                    {
                        int lineNumber = actionVM->lineInTimeLine();

                        // Increment actionVM into the line number
                        if(_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber) == true)
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
                        if(_mapActionsVMsInTimelineFromActionModel.contains(actionVM->actionModel()) == true)
                        {
                            actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionVM->actionModel());
                        }
                        actionsVMsList.append(actionVM);
                        _mapActionsVMsInTimelineFromActionModel.insert(actionVM->actionModel(),actionsVMsList);

                        _actionsInTimeLine.append(actionVM);

                        // Increment the line number if necessary
                        if(_linesNumberInTimeLine < lineNumber+1)
                        {
                            setlinesNumberInTimeLine(lineNumber+1);
                        }
                    }
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
                                                              "Sauvegarder dans un fichier JSON e scenario",
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
        qInfo() << "Export the scenario to JSON file" << scenarioFilePath;

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
void ScenarioController::addActionVMAtTime(ActionM * actionModel, int timeInMs, int lineNumber)
{
    if(actionModel != NULL)
    {
        ActionVM * actionVM = new ActionVM(actionModel,timeInMs);

        // Insert the actionVM to the reight line number
        _insertActionVMIntoMapByLineNumber(actionVM, lineNumber);

        // If the action VM found a line to be insert in
        if(actionVM->lineInTimeLine() != -1)
        {
            // Add the new action VM to our map
            QList<ActionVM*> actionsVMsList;
            if(_mapActionsVMsInTimelineFromActionModel.contains(actionModel) == true)
            {
                actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionModel);
            }
            actionsVMsList.append(actionVM);
            _mapActionsVMsInTimelineFromActionModel.insert(actionModel,actionsVMsList);

            // Add the action VM to the timeline
            _actionsInTimeLine.append(actionVM);

            // If scenario is playing we add the actionVM to the active ones
            if(_isPlaying)
            {
                _activeActionsVMList.append(actionVM);
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
void ScenarioController::addActionVMAtCurrentTime(ActionM * actionModel)
{
    if(actionModel != NULL)
    {
        addActionVMAtTime(actionModel,_currentTime.msecsSinceStartOfDay());
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
        if(actionVM->actionModel() != NULL &&
                _mapActionsVMsInTimelineFromActionModel.contains(actionVM->actionModel()))
        {
            QList<ActionVM*> actionsVMsList = _mapActionsVMsInTimelineFromActionModel.value(actionVM->actionModel());
            actionsVMsList.removeAll(actionVM);
            _mapActionsVMsInTimelineFromActionModel.insert(actionVM->actionModel(),actionsVMsList);

            // Remove the action VM from the timeline map by line number
            if(actionVM->lineInTimeLine() != -1)
            {
                int lineNumber = actionVM->lineInTimeLine();
                if(_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber) == true)
                {
                    I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                    if(actionVMSortedList != NULL && actionVMSortedList->contains(actionVM) == true)
                    {
                        // Remove item
                        actionVMSortedList->remove(actionVM);

                        // Check if the list is empty to remove the line
                        if(actionVMSortedList->count() == 0)
                        {
                            // We delete the last line, redice the number of display line in the limite of MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE
                            if(_linesNumberInTimeLine > MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE && lineNumber+1 == _linesNumberInTimeLine)
                            {
                                int nbOfDecrement = 1;

                                // Delete the last line, lets check if we can reduce the number of displayed lines
                                for (int lineTmp = lineNumber-1; lineTmp >= 0 ; --lineTmp)
                                {
                                    if(_mapActionsVMsInTimelineFromLineIndex.contains(lineTmp) == false)
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
    foreach (ActionVM * actionVM, _actionsInTimeLine.toList())
    {
        if(actionVM->actionModel() != NULL && actionVM->actionModel()->isConnected() == false)
        {
            actionVM->actionModel()->initializeConditionsConnections();
        }
    }
}

/**
 * @brief Conditions disconnections
 */
void ScenarioController::conditionsDisconnect()
{
    foreach (ActionVM * actionVM, _actionsInTimeLine.toList())
    {
        if(actionVM->actionModel() != NULL && actionVM->actionModel()->isConnected())
        {
            actionVM->actionModel()->resetConditionsConnections();
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
    int insertionStartTime = actionVMToInsert->startTime();

    int lineNumber = lineNumberRef != -1 ? lineNumberRef : 0;
    while (lineNumber < _linesNumberInTimeLine)
    {
        bool canInsert = canInsertActionVMTo(actionVMToInsert->actionModel(), insertionStartTime,lineNumber);
        // Insert our item if possible
        if(canInsert == true)
        {
            if(_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber) == true)
            {
                I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
                if(actionVMSortedList != NULL)
                {
                    // set the line number
                    actionVMToInsert->setlineInTimeLine(lineNumber);

                    // Insert the action
                    actionVMSortedList->append(actionVMToInsert);

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
        if(lineNumber >= _linesNumberInTimeLine)
        {
            setlinesNumberInTimeLine(_linesNumberInTimeLine+1);
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

    if(_mapActionsVMsInTimelineFromLineIndex.contains(lineNumber) == true)
    {
        bool reachPosition = false;
        ActionVM * previousActionVM = NULL;

        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = _mapActionsVMsInTimelineFromLineIndex.value(lineNumber);
        if(actionVMSortedList != NULL)
        {
            for (int indexAction = 0; indexAction < actionVMSortedList->count(); ++indexAction)
            {
                ActionVM * actionVM = actionVMSortedList->at(indexAction);

                if(actionVM->actionModel() != NULL)
                {
                    if(time < actionVM->startTime())
                    {
                        reachPosition = true;

                        // We reach the current action VM
                        // Check with the previous actionM
                        if(previousActionVM != NULL && previousActionVM->actionModel() != NULL)
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
            if(reachPosition == false && previousActionVM != NULL)
            {
                // If the previous action ends after the beginning of the new one, we skip it
                if(previousActionVM->endTime()+MARGIN_FOR_ACTION_INSERTION_IN_MS >= time || previousActionVM->endTime() == -1)
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
        qDebug() << "Execute" << action->effectsList()->count() << "effects of action" << action->name();

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

        // Start/stop scenario according to the flag
        if(_isPlaying == false)
        {
            _stopScenario();
        }
        else
        {
            _startScenario();
        }

        Q_EMIT isPlayingChanged(_isPlaying);
    }
}


/**
 * @brief Called at each interval of our timer to update the current state of each zones that have at least one loudspeakers line
 */
/*void ScenarioController::_onTimeout_EvaluateActions()
{
    // Move the currenttime
    int currentTimeOfDay = QTime::currentTime().msecsSinceStartOfDay();
    setcurrentTime(_currentTime.addMSecs(currentTimeOfDay - _scenarioStartingTimeInMs));

    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    // Evaluate the list of actions
    foreach (ActionVM* actionVM, _activeActionsVMList.toList())
    {
        if ((actionVM != NULL) && (actionVM->actionModel() != NULL))
        {
            // View model of action is in the PRESENT OR in the PAST
            if (actionVM->startTime() <= currentTimeInMilliSeconds)
            {
                // FIXME TODO: if the action VM has no duration

                // View model of action is in the PRESENT
                if ((actionVM->endTime() == -1) || (actionVM->endTime() >= currentTimeInMilliSeconds))
                {
                    ActionExecutionVM* actionExecution = actionVM->currentExecution();
                    if (actionExecution != NULL)
                    {
                        // Not already executed
                        if (!actionExecution->isExecuted())
                        {
                            // All conditions are met (or there is NO condition on this action)...
                            if (actionVM->isValid())
                            {
                                _executeAction(actionVM,actionExecution,currentTimeInMilliSeconds);

                            }
                            // There is at least one condition which is not respected
                            else
                            {
                                // Delay the current execution of its action
                                actionVM->delayCurrentExecution(currentTimeInMilliSeconds);
                            }
                        }
                        // Action is already executed and shall revert
                        else if (actionExecution->shallRevert())
                        {
                            // TODO
                            if (actionExecution->reverseTime() <= currentTimeInMilliSeconds)
                            {
                                // Execute reverse effects of the action
                                _executeReverseEffectsOfAction(actionExecution);

                                // Notify the action that its reverse effects has been executed
                                actionVM->reverseEffectsExecuted(currentTimeInMilliSeconds);
                            }
                        }
                    }
                }
                // View model of action is in the PAST
                else
                {
                    ActionExecutionVM* actionExecution = actionVM->currentExecution();
                    if (actionExecution != NULL)
                    {
                        // FIXME TODO

                        // Remove the current execution
                        actionVM->setcurrentExecution(NULL);
                        actionVM->executionsList()->remove(actionExecution);
                        delete actionExecution;
                    }

                    // Remove from the list of "active" actions
                    _activeActionsVMList.remove(actionVM);
                }
            }
            // View model of action is in the FUTURE
            else // (actionVM->startTime() > currentTimeInMilliSeconds)
            {
                // Exit loop foreach
                break;
            }
        }
    }


    // Save our scenario start
    _scenarioStartingTimeInMs = currentTimeOfDay;
}*/


/**
 * @brief Called when our timer time out to handle the scenario and execute actions
 */
void ScenarioController::_onTimeout_ExecuteActions()
{
    qDebug() << "Timeout Execute Actions";
}


/**
 * @brief Called at each interval of our timer to delay actions (when their conditions are not valid)
 */
void ScenarioController::_onTimeout_DelayActions()
{
    //qDebug() << "Timeout Delay Actions" << _activeActionsVMList.count();

    // Move the currenttime
    int currentTimeOfDay = QTime::currentTime().msecsSinceStartOfDay();

    setcurrentTime(_currentTime.addMSecs(currentTimeOfDay - _scenarioStartingTimeInMs));

    int currentTimeInMilliSeconds = _currentTime.msecsSinceStartOfDay();

    // Traverse the list of active actions
    foreach (ActionVM* actionVM, _activeActionsVMList.toList())
    {
        // Current time is after the start time of action
        if ((actionVM != NULL) && (actionVM->actionModel() != NULL) && (actionVM->startTime() < currentTimeInMilliSeconds))
        {
            // Action has no end or end is in the future
            if ( ((actionVM->endTime() == -1) || (currentTimeInMilliSeconds < actionVM->endTime()))
                 // And the action has a validity duration
                 && (actionVM->actionModel()->validityDurationType() != ValidationDurationType::IMMEDIATE) )
            {
                ActionExecutionVM* actionExecution = actionVM->currentExecution();

                // Not already executed
                if ((actionExecution != NULL) && !actionExecution->isExecuted())
                {
                    if(actionVM->isValid() == false)
                    {
                        // Delay the current execution of this action
                        actionVM->delayCurrentExecution(currentTimeInMilliSeconds);
                    } else {
                        _executeAction(actionVM, actionExecution, currentTimeInMilliSeconds);
                    }
                }
            }
            // Current time is after the end time of action (or the action has no validity duration --> Immediate)
            else
            {
                ActionExecutionVM* actionExecution = actionVM->currentExecution();
                if (actionExecution != NULL)
                {
                    actionExecution->setneverExecuted(true);

                    // If there is at least another execution for this action...
                    if (actionVM->executionsList()->count() > 1)
                    {
                        // ...we remove the current execution
                        actionVM->setcurrentExecution(NULL);
                        actionVM->executionsList()->remove(actionExecution);

                        // Free memory
                        delete actionExecution;
                    }
                }

                // Remove from the list of "active" actions
                _activeActionsVMList.remove(actionVM);
            }
        }
    }

    // Save our scenario start
    _scenarioStartingTimeInMs = currentTimeOfDay;
}


/**
 * @brief Start the scenario by
 *        making connections for the actions conditions
 *        starting the action evaluation timer
 */
void ScenarioController::_startScenario()
{
    // Set the list of Actions to process at currentTime
    _activeActionsVMList.clear();

    // Inverse exploration since we add the future actions first
    QList<ActionVM*> actionListToAdd;
    for (int index = _actionsInTimeLine.count()-1; index >= 0; --index)
    {
        ActionVM* actionVM = _actionsInTimeLine.at(index);
        if ((actionVM->endTime() > _currentTime.msecsSinceStartOfDay()) || (actionVM->endTime() == -1))
        {
            actionListToAdd.append(actionVM);
        }
        else {
            break;
        }
    }
    if (actionListToAdd.count() > 0)
    {
        _activeActionsVMList.append(actionListToAdd);
    }

    // Connect actions conditions
    conditionsConnect();

    // Save our scenario start
    _scenarioStartingTimeInMs = QTime::currentTime().msecsSinceStartOfDay();

    // Start timers
    // FIXME TODO: init the timer with the time of the next action execution
    //_timerToExecuteActions.start();
    _timerToRegularlyDelayActions.start();
}


/**
 * @brief Stop the scenario by
 *        disconnecting the actions conditions
 *        stoping the action evaluation timer
 */
void ScenarioController::_stopScenario()
{
    // Disconnect actions conditions
    conditionsDisconnect();

    // Stop timers
    //_timerToExecuteActions.stop();
    _timerToRegularlyDelayActions.stop();
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
        QStringList peerIdsList = agent->getPeerIdsList();

        qInfo() << "Execute command" << commandAndParameters << "for agent" << agent->name() << "(" << peerIdsList.count() << "peer ids)";

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
            Q_EMIT commandAskedToAgent(peerIdsList, command);
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
                Q_EMIT commandAskedToAgentAboutMappingInput(peerIdsList, command, inputName, outputAgentName, outputName);
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
                Q_EMIT commandAskedToAgentAboutSettingValue(peerIdsList, command, agentIOPName, value);
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
            actionExecution->initReverseCommandsForEffects(actionVM->actionModel()->effectsList()->toList());
        }

        // Execute all effects of the action
        executeEffectsOfAction(actionVM->actionModel());

        // Notify the action that its effects has been executed
        actionVM->effectsExecuted(currentTimeInMilliSeconds);
    }
}
