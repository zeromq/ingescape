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

#include "scenariocontroller.h"

#include <QDebug>
#include <QFileDialog>

/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
ScenarioController::ScenarioController(IngeScapeModelManager* modelManager,
                                       QObject *parent) : AbstractScenarioController (modelManager,
                                                                                      parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


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
}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    // Delete actions VM from the palette
    _actionsInPaletteList.deleteAllItems();

    // Mother class is automatically called
    //AbstractScenarioController::~AbstractScenarioController();
}


/**
  * @brief Import the scenario from JSON
  * @param jsonScenario
  */
void ScenarioController::importScenarioFromJson(QJsonObject jsonScenario)
{
    // FIXME: Separate in 2 methods
    // - importScenarioFromJson
    // - and initPalette(ScenarioM* scenarioToImport)

    // Call our mother class
    AbstractScenarioController::importScenarioFromJson(jsonScenario);

    if (_modelManager != nullptr)
    {
        // Get the hash table from a name to the group of agents with this name
        QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped = _modelManager->getHashTableFromNameToAgentsGrouped();

        // Create a model of scenario (actions in the list, in the palette and in the timeline) from JSON
        ScenarioM* scenarioToImport = JsonHelper::createModelOfScenarioFromJSON(jsonScenario, hashFromNameToAgentsGrouped);
        if (scenarioToImport != nullptr)
        {
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

            delete scenarioToImport;
        }
    }
}


/**
  * @brief Remove an action from the list and delete it
  * @param action
  */
void ScenarioController::deleteAction(ActionM* action)
{
    if (action != nullptr)
    {
        // Remove action from the palette if exists
        for (int index = 0; index < _actionsInPaletteList.count(); index++)
        {
            ActionInPaletteVM* actionInPaletteVM = _actionsInPaletteList.at(index);

            if ((actionInPaletteVM != nullptr) && (actionInPaletteVM->modelM() != nullptr) && (actionInPaletteVM->modelM() == action)) {
                setActionInPalette(index, nullptr);
            }
        }

        // Delete action editor if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromModelOfAction(action);
        if (actionEditorC != nullptr)
        {
            _hashActionEditorControllerFromModelOfAction.remove(action);

            // Remove from the list of opened action editors
            _openedActionsEditorsControllers.remove(actionEditorC);

            delete actionEditorC;
        }

        // Call our mother class
        AbstractScenarioController::deleteAction(action);
    }
}


/**
 * @brief Remove an action VM from the time line and delete it
 * @param action view model
 */
void ScenarioController::removeActionVMfromTimeLine(ActionVM* actionVM)
{
    if ((actionVM != nullptr))
    {
        // Delete the action editor if necessary
        ActionEditorController* actionEditorC = _getActionEditorFromViewModelOfAction(actionVM);
        if (actionEditorC != nullptr)
        {
            _hashActionEditorControllerFromViewModelOfAction.remove(actionVM);

            // Remove from the list of opened action editors
            _openedActionsEditorsControllers.remove(actionEditorC);

            delete actionEditorC;
        }

        // Call our mother class
        AbstractScenarioController::removeActionVMfromTimeLine(actionVM);
    }
}


/**
 * @brief Clear the current scenario
 * (clear the list of actions in the list, in the palette and in the timeline)
 */
void ScenarioController::clearScenario()
{
    qInfo() << "Scenario Controller: Clear the current scenario";

    // Clear the lists of action editors
    _hashActionEditorControllerFromModelOfAction.clear();
    _hashActionEditorControllerFromViewModelOfAction.clear();
    _openedActionsEditorsControllers.deleteAllItems();

    // Reset actions in palette
    for (ActionInPaletteVM* actionInPalette : _actionsInPaletteList.toList())
    {
        if (actionInPalette != nullptr) {
            actionInPalette->setmodelM(nullptr);
        }
    }

    // Call our mother class
    AbstractScenarioController::clearScenario();
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
  * @brief Open the action editor with a duplicate model of action
  * @param action to duplicate
  */
void ScenarioController::openActionEditorToDuplicateModel(ActionM* action)
{
    if ((action != nullptr) && (_modelManager != nullptr))
    {
        // Create an action editor
        ActionEditorController* actionEditorC = new ActionEditorController(_buildDuplicateActionName(action->name()), action, _modelManager->allAgentsGroupsByName()->toList(), true);

        _hashActionEditorControllerFromModelOfAction.insert(actionEditorC->editedAction(), actionEditorC);

        // Add to the list of opened action editors
        _openedActionsEditorsControllers.append(actionEditorC);
    }
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
                actionEditorC = new ActionEditorController(_buildNewActionName(), action, _modelManager->allAgentsGroupsByName()->toList(), false);

                _hashActionEditorControllerFromModelOfAction.insert(action, actionEditorC);

                // Add to the list of opened action editors
                _openedActionsEditorsControllers.append(actionEditorC);
            }
        }
        else
        {
            // Create an action editor
            actionEditorC = new ActionEditorController(_buildNewActionName(), nullptr, _modelManager->allAgentsGroupsByName()->toList(), false);

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
            actionEditorC = new ActionEditorController(_buildNewActionName(), action->modelM(), _modelManager->allAgentsGroupsByName()->toList(), false);

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
  * @brief Validate action editior
  * @param action editor controller
  */
void ScenarioController::validateActionEditor(ActionEditorController* actionEditorC)
{
    if ((actionEditorC != nullptr) && (_modelManager != nullptr))
    {
        // Validate modification
        actionEditorC->validateModification();

        ActionM* originalAction = actionEditorC->originalAction();
        if ((originalAction != nullptr) && (_modelManager->getActionWithId(originalAction->uid()) == nullptr))
        {
            // Add the action to the model manager
            _modelManager->storeNewAction(originalAction);

            // Add the action to the list
            _actionsList.append(originalAction);

            // Add action name
            if (!_allActionNames.contains(originalAction->name())) {
                _allActionNames.append(originalAction->name());
            }
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
 * @brief Set a model of action into the palette at index
 * @param index where to insert the action
 * @param actionM model of action to insert
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
 * @brief Check if a view model of an action has been inserted in the timeline
 * @param actionM
 * @return
 */
bool ScenarioController::isActionInsertedInTimeLine(ActionM* actionM)
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
 * @brief Get a new action name
 */
QString ScenarioController::_buildNewActionName()
{
    int index = 0;

    // QString::arg(int a, int fieldWidth = 0, int base = 10, QChar fillChar = QLatin1Char(' '))
    // The a argument is expressed in base base, which is 10 by default.
    // fieldWidth specifies the minimum amount of space that a is padded to and filled with the character fillChar.
    QString tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));

    while (_allActionNames.contains(tmpName))
    {
        index++;
        tmpName = QString("Action_%1").arg(index, 3, 10, QChar('0'));
    }

    return tmpName;
}

/**
 * @brief Get a name for a new copied action
 * @param the duplicate action name
 */
QString ScenarioController::_buildDuplicateActionName(QString actionName)
{
    int index = 1;

    // Clean name if it contains "_copy_XXX" at the end
    QString originalName;
    QStringList nameSplitted = actionName.split('_');
    int length = nameSplitted.length();
    if (length == 1) {
        originalName = actionName;
    }
    else {
        if (nameSplitted.at(length-1) == "copy") {
            nameSplitted.removeAt(length-1);
        }
        else if (nameSplitted.at(length-2) == "copy") {
            nameSplitted.removeAt(length-1);
            nameSplitted.removeAt(length-2);
        }
        originalName = nameSplitted.join("");
    }

    // Create new name with "_copy_XXX" at the end
    QString tmpName = QString(originalName + "_copy");
    while (_allActionNames.contains(tmpName))
    {
        index++;
        tmpName = originalName + "_copy_" + QString::number(index);
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
 * @brief Get the "Action Editor" from a model of action
 * @return
 */
ActionEditorController* ScenarioController::_getActionEditorFromModelOfAction(ActionM* action)
{
    return _hashActionEditorControllerFromModelOfAction.value(action, nullptr);
}


/**
 * @brief Get the "Action Editor" from a view model of action
 * @return
 */
ActionEditorController* ScenarioController::_getActionEditorFromViewModelOfAction(ActionVM* action)
{
    return _hashActionEditorControllerFromViewModelOfAction.value(action, nullptr);
}
