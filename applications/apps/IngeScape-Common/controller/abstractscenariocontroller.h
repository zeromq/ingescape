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

#ifndef ABSTRACT_SCENARIO_CONTROLLER_H
#define ABSTRACT_SCENARIO_CONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include <controller/ingescapemodelmanager.h>
#include <viewModel/agentsgroupedbynamevm.h>
#include <viewModel/scenario/actionvm.h>
#include <viewModel/scenario/actioninpalettevm.h>
#include <sortFilter/abstracttimerangefilter.h>


// Margin in milliseconds to insert an action following another
#define MARGIN_FOR_ACTION_INSERTION_IN_MS 2000

// Minimum lines displayed into the timeline by default
#define MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE 6

// Interval in milli-seconds to regularly delay actions (when their conditions are not valid)
#define INTERVAL_DELAY_ACTIONS 25


/**
 * @brief The AbstractScenarioController class is the base class for scenario management
 */
class AbstractScenarioController: public QObject
{
    Q_OBJECT

    // List of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

    // Selected action
    I2_QML_PROPERTY_DELETE_PROOF(ActionM*, selectedAction)

    // List of actions in timeline
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, actionsInTimeLine)

    // List of "actionVM in timeline" filtered with a given time range
    Q_PROPERTY(AbstractTimeRangeFilter* filteredListActionsInTimeLine READ filteredListActionsInTimeLine CONSTANT)

    // Selected action VM in timeline
    I2_QML_PROPERTY_DELETE_PROOF(ActionVM*, selectedActionVMInTimeline)

    // Number of lines in our timeline
    I2_QML_PROPERTY(int, linesNumberInTimeLine)

    // Flag indicating if our timeline (scenario) is currently playing
    I2_QML_PROPERTY_READONLY(bool, isPlaying)

    // Current time (from the beginning of our scenario)
    // Define a QTime and a QDateTime: Manage a date in addition to the time to prevent a delta in hours between JS (QML) and C++
    I2_QML_PROPERTY_QTime(currentTime)

    // List of (future) actions to evaluate at each timeout of our timer
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, listOfActionsToEvaluate)

    // List of active actions (the current time is between start time and end time of these actions)
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, listOfActiveActions)

    // Next action view model to activate
    I2_QML_PROPERTY(ActionVM*, nextActionToActivate)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit AbstractScenarioController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~AbstractScenarioController();


    /**
    * @brief Get our filtered list of "time ticks"
    * @return
    */
    AbstractTimeRangeFilter* filteredListActionsInTimeLine()
    {
        return &_filteredListActionsInTimeLine;
    }


    /**
      * @brief Import the scenario from JSON
      * @param jsonScenario
      */
    virtual void importScenarioFromJson(QJsonObject jsonScenario);


    /**
     * @brief Import the executed actions for a scenario from JSON
     * @param byteArrayOfJson
     */
    void importExecutedActionsFromJson(int deltaTimeFromTimeLineStart, QByteArray byteArrayOfJson);

    /**

     * @brief Import an executed action in our timeline : create a new action view model
     * ONLY if no action view model already exists for the actionID
     * at lineIndexInTimeline and executionTime
     */
    ActionVM* addExecutedActionToTimeline(int actionId, int lineIndexInTimeLine, int executionTime);


    /**
      * @brief Remove an action from the list and delete it
      * @param action
      */
    virtual Q_INVOKABLE void deleteAction(ActionM* action);


    /**
     * @brief Test if an item can be inserted into a line number
     * @param actionM to insert
     * @param time to insert
     * @param line index
     * @param optional dragged action VM when already in the time-line
     */
    Q_INVOKABLE bool canInsertActionVMTo(ActionM *actionMToInsert, int time, int lineIndex, ActionVM* draggedActionVM = nullptr);


    /**
     * @brief Add an action VM at the current date time
     * @param action model
     */
    Q_INVOKABLE void addActionVMAtCurrentTime(ActionM* actionM);


    /**
     * @brief Add an action VM at the time in ms
     * @param action model
     * @param line index
     */
    Q_INVOKABLE void addActionVMAtTime(ActionM* actionM, int timeInMs, int lineIndex);


    /**
     * @brief Remove an action VM from the time line and delete it
     * @param action view model
     */
    virtual Q_INVOKABLE void removeActionVMfromTimeLine(ActionVM* actionVM);


    /**
     * @brief Play (or Resume) the timeline (current scenario)
     */
    Q_INVOKABLE void playOrResumeTimeLine();


    /**
     * @brief Pause the timeline (current scenario)
     */
    Q_INVOKABLE void pauseTimeLine();


    /**
     * @brief Stop/Reset the timeline (current scenario)
     */
    Q_INVOKABLE void stopTimeLine();


    /**
     * @brief Clear the current scenario
     * (clear the list of actions in the list, in the palette and in the timeline)
     */
    virtual Q_INVOKABLE void clearScenario();


    /**
     * @brief Execute all effects of the action
     * Activate (connect) the mapping if necessary
     * Notify the recorder that the action has been executed
     * @param action
     * @param lineInTimeLine
     */
    Q_INVOKABLE void executeEffectsOfAction(ActionM* action, int lineInTimeLine = 0);


Q_SIGNALS:

    /**
     * @brief Signal emitted just before an action is performed
     * (the message "EXECUTED ACTION" must be sent on the network to the recorder)
     * @param message
     */
    void actionWillBeExecuted(QString message);


    /**
     * @brief Signal emitted when the state of the TimeLine updated
     * @param state
     */
    void timeLineStateUpdated(QString state);


public Q_SLOTS:

    /**
     * @brief Slot called when an action must be reverted
     * @param actionExecution
     */
    void onRevertAction(ActionExecutionVM* actionExecution);


    /**
     * @brief Slot called when an action must be rearmed
     */
    void onRearmAction();


    /**
     * @brief Slot called when the time line range changed
     * @param startTimeInMilliseconds
     * @param endTimeInMilliseconds
     */
    void onTimeRangeChanged(int startTimeInMilliseconds, int endTimeInMilliseconds);


    /**
     * @brief Slot called when we receive the command "run action" from a recorder
     * @param actionID
     */
    void onRunAction(QString actionID);


    /**
     * @brief Slot called when an action has to be executed
     * @param action
     */
    void onExecuteAction(ActionM* action);


protected Q_SLOTS:

    /**
     * @brief Called when our timer time out to handle the scenario and execute actions
     */
    void _onTimeout_ExecuteActions();


    /**
     * @brief Called at each interval of our timer to delay actions when their conditions are not valid or execute them otherwise
     */
    void _onTimeout_DelayOrExecuteActions();


protected:

    /**
     * @brief Initialize actions and play (or resume) the scenario
     * - make connections for actions conditions
     * - start the action evaluation timer
     */
    void _initActionsAndPlayOrResumeScenario();


    /**
     * @brief Pause the scenario and associated actions
     * - disconnect actions conditions
     * - stop the action evaluation timer
     */
    void _pauseScenarioAndActions();


    /**
     * @brief Execute all effects of an action
     * @param action
     */
    void _executeEffectsOfAction(ActionM* action);


    /**
     * @brief Execute reverse effects of an action
     * @param actionExecution
     */
    void _executeReverseEffectsOfAction(ActionExecutionVM* actionExecution);


    /**
     * @brief Execute a command for an agent
     * @param agentsGroupedByName
     * @param commandAndParameters
     */
    void _executeCommandForAgent(AgentsGroupedByNameVM* agentsGroupedByName, QStringList commandAndParameters);


    /**
     * @brief Exectute the action with the revert initialization if necessary
     * @param action view model
     * @param action execution view model
     * @param current time in ms
     */
    void _executeAction(ActionVM* actionVM, ActionExecutionVM* actionExecution, int currentTimeInMilliSeconds);


    /**
     * @brief Get the list of view models of action with its (unique) id
     * @param actionId
     * @return
     */
    QList<ActionVM*> _getListOfActionVMwithId(int actionId);


    /**
     * @brief Get the "Sorted" list of view models of action with the index of the line (in the time line)
     * @param index
     * @return
     */
    I2CustomItemSortFilterListModel<ActionVM>* _getSortedListOfActionVMwithLineIndex(int index);


protected:

    // List of all action names
    QStringList _allActionNames;

    // Hash table from action UID to the corresponding list of view models of action (in the timeline)
    QHash<int, QList<ActionVM*> > _hashFromUidToViewModelsOfAction;

    // Hash table from line index (in the timeline) to the corresponding list of view models of action
    QHash<int, I2CustomItemSortFilterListModel<ActionVM>* > _hashFromLineIndexToSortedViewModelsOfAction;

    // Timer to handle the scenario and execute actions
    QTimer _timerToExecuteActions;

    // Timer to regularly delay actions (when their conditions are not valid)
    QTimer _timerToRegularlyDelayActions;

    // Time of the day in milli-seconds when user starts the scenario, then at the last timeout (of the timer which regularly delay actions)
    int _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut;

    // List of actionVM in timeline filtered with a given time range in milliseconds
    AbstractTimeRangeFilter _filteredListActionsInTimeLine;


private:

    /**
     * @brief Insert an actionVM into our timeline
     * @param action view model
     * @param line number
     */
    void _insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert, int lineNumberRef);

    /**
     * @brief Clear the current timeline
     * (clear the list of actions in the timeline)
     */
    void _clearTimeline();
};

QML_DECLARE_TYPE(AbstractScenarioController)

#endif // ABSTRACT_SCENARIO_CONTROLLER_H
