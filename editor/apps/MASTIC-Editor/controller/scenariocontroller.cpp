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
    _linesNumberInTimeLine(1),
    _isPlayingScenario(false),
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
}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    // Clean-up current selection
    setselectedAction(NULL);

    // Delete actions VM from the timeline
    _actionsInTimeLine.deleteAllItems();

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

            // Set view model
            actionEditorC->setviewModel(actionVM);

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
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionM.value(actionM);

        _mapActionsEditorControllersFromActionM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
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

    ActionM* originalActionVM = actionEditorC->originalAction();

    //FIXME Initialize connections
    //originalActionVM->initializeConditionsConnections();

    // We check that or editor is not already opened
    if(_actionsList.contains(originalActionVM) == false)
    {
        // Insert in to the list
        _actionsList.append(originalActionVM);

        // Insert into the map
        _mapActionsFromActionName.insert(originalActionVM->name(),originalActionVM);
    }

    // Set selected action
    setselectedAction(originalActionVM);
}

/**
  * @brief Close action edition
  * @param action editor controller
  */
void ScenarioController::closeActionEditor(ActionEditorController* actionEditorC)
{
    ActionM* actionM = actionEditorC->originalAction() != NULL ? actionEditorC->originalAction() : actionEditorC->editedAction();
    // Delete the popup if necessary
    if(actionM != NULL && _mapActionsEditorControllersFromActionM.contains(actionM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionM.value(actionM);

        _mapActionsEditorControllersFromActionM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
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
    int index = 1;
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
                    //FIXME
//                    foreach (ActionM* actionM, scenarioToImport.first.first)
//                    {
//                        actionM->initializeConditionsConnections();
//                    }

                    _actionsList.append(scenarioToImport.first.first);
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
                    _actionsInTimeLine.append(scenarioToImport.second);
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
 */
void ScenarioController::addActionVMAtTime(ActionM * actionModel, int timeInMs)
{
    if(actionModel != NULL)
    {
        ActionVM * actionVM = new ActionVM(actionModel,timeInMs);

        // Insert the actionVM to the reight line number
        _insertActionVMIntoMapByLineNumber(actionVM);

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
        // FIXME - get time from the current time of the timeline
        int timeInMs = QDateTime::currentDateTime().time().msecsSinceStartOfDay();

        addActionVMAtTime(actionModel,timeInMs);
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
                        if(actionVMSortedList->count() == 0 && lineNumber == _linesNumberInTimeLine-1)
                        {
                            _mapActionsVMsInTimelineFromLineIndex.remove(lineNumber);
                            delete actionVMSortedList;
                            actionVMSortedList = NULL;

                            // Decrement the number of lines
                           setlinesNumberInTimeLine(_linesNumberInTimeLine-1);
                        }
                    }
                }
            }
        }

        // Delete action view model editor if exists
        if(_mapActionsEditorControllersFromActionVM.contains(actionVM))
        {
            ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionVM);

            _mapActionsEditorControllersFromActionVM.remove(actionVM);
            _openedActionsEditorsControllers.remove(actionEditorC);
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
void ScenarioController::_insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert)
{
    int insertionStartTime = actionVMToInsert->startTime();

    for (int lineNumber = 0; lineNumber < _linesNumberInTimeLine; ++lineNumber)
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
        }
    }

    // If the action has not been inserted yet, we create a new line
    if(actionVMToInsert->lineInTimeLine() == -1)
    {
        // Create the new line number
        int newLineNumber = _linesNumberInTimeLine;
        setlinesNumberInTimeLine(_linesNumberInTimeLine+1);
        actionVMToInsert->setlineInTimeLine(newLineNumber);

        // Create a new list
        I2CustomItemSortFilterListModel<ActionVM>* actionVMSortedList = new I2CustomItemSortFilterListModel<ActionVM>();
        actionVMSortedList->setSortProperty("startTime");
        actionVMSortedList->append(actionVMToInsert);

        // Add into our map
        _mapActionsVMsInTimelineFromLineIndex.insert(newLineNumber,actionVMSortedList);
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
            foreach (ActionVM * actionVM, actionVMSortedList->toList())
            {
                if(actionVM->actionModel() != NULL)
                {
                    if(time < actionVM->startTime())
                    {
                        reachPosition = true;

                        // We reach the current action VM
                        // Check with the previous actionM
                        if(previousActionVM != NULL && previousActionVM->actionModel() != NULL)
                        {
                            int prevEndTime = previousActionVM->startTime();
                            // We skip that line since the action have a forever validity
                            if(previousActionVM->actionModel()->validityDurationType() == ValidationDurationType::FOREVER)
                            {
                                // Try with the next line
                                canInsert = false;
                                break;
                            } else if(previousActionVM->actionModel()->validityDurationType() == ValidationDurationType::CUSTOM)
                            {
                                prevEndTime += previousActionVM->actionModel()->validityDuration();
                            }

                            // If the previous action ends after the beginning of the new one, we skip it
                            if(prevEndTime >= time)
                            {
                                canInsert = false;
                                break;
                            }
                        }

                        int insertionEndTime = time;
                        if(actionMToInsert->validityDurationType() == ValidationDurationType::FOREVER)
                        {
                            // Try with the next line
                            canInsert = false;
                            break;
                        } else if(actionMToInsert->validityDurationType() == ValidationDurationType::CUSTOM)
                        {
                            insertionEndTime += actionMToInsert->validityDuration();
                        }

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
                int prevEndTime = previousActionVM->startTime();
                // We skip that line since the action have a forever validity
                if(previousActionVM->actionModel()->validityDurationType() == ValidationDurationType::FOREVER)
                {
                    // Try with the next line
                    canInsert = false;
                } else if(previousActionVM->actionModel()->validityDurationType() == ValidationDurationType::CUSTOM) {
                    prevEndTime += previousActionVM->actionModel()->validityDuration();
                }

                // If the previous action ends after the beginning of the new one, we skip it
                if(prevEndTime >= time)
                {
                    canInsert = false;
                }
            }
        }
    }

    return canInsert;
}


/**
 * @brief Custom setter on is playing command for the scenario
 * @param is playing flag
 */
void ScenarioController::setisPlayingScenario(bool isPlaying)
{
    if(_isPlayingScenario != isPlaying)
    {
        _isPlayingScenario = isPlaying;

        // Connect/disconnect conditions connections
        if(_isPlayingScenario == false)
        {
            conditionsDisconnect();
        } else {
            conditionsConnect();
        }

        Q_EMIT isPlayingScenarioChanged(_isPlayingScenario);
    }
}
