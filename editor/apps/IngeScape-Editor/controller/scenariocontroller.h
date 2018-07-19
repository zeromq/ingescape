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

#ifndef SCENARIOCONTROLLER_H
#define SCENARIOCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include <controller/ingescapemodelmanager.h>
#include <controller/actioneditorcontroller.h>
#include "viewModel/agentinmappingvm.h"
#include "viewModel/scenario/actionvm.h"
#include "viewModel/scenario/actioninpalettevm.h"
#include "sortFilter/abstracttimerangefilter.h"


// Margin in milliseconds to insert an action following another
#define MARGIN_FOR_ACTION_INSERTION_IN_MS 2000

// Minimum lines displayed into the timeline by default
#define MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE 6

// Interval in milli-seconds to regularly delay actions (when their conditions are not valid)
#define INTERVAL_DELAY_ACTIONS 25


/**
 * @brief The ScenarioController class defines the controller of our scenario edition
 */
class ScenarioController: public QObject
{
    Q_OBJECT

    // List of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

    // List of opened action editors
    I2_QOBJECT_LISTMODEL(ActionEditorController, openedActionsEditorsControllers)

    // Selected action
    I2_QML_PROPERTY_DELETE_PROOF(ActionM*, selectedAction)

    // List of all values of condition on agent (state)
    I2_ENUM_LISTMODEL(AgentConditionValues, allAgentConditionValues)

    // List of all types for values comparison
    I2_ENUM_LISTMODEL(ValueComparisonTypes, allValueComparisonTypes)

    // List of validity duration type
    I2_ENUM_LISTMODEL(ValidationDurationTypes, validationDurationsTypesList)

    // --- List of values about effect on agent
    I2_ENUM_LISTMODEL(AgentEffectValues, agentEffectValuesList)
    // --- List of effects links type
    I2_ENUM_LISTMODEL(MappingEffectValues, mappingEffectValuesList)

    // --- List of conditions type
    I2_ENUM_LISTMODEL(ActionConditionTypes, conditionsTypesList)
    // --- List of effects type
    I2_ENUM_LISTMODEL(ActionEffectTypes, effectsTypesList)

    // --- agents list in mapping
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentInMappingVM, agentsInMappingList)

    // List of actions in palette
    I2_QOBJECT_LISTMODEL(ActionInPaletteVM, actionsInPaletteList)

    // List of actions in timeline
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, actionsInTimeLine)

    // List of "actionVM in timeline" filtered with a given time range
    Q_PROPERTY(AbstractTimeRangeFilter* filteredListActionsInTimeLine READ filteredListActionsInTimeLine CONSTANT)

    // Selected action VM in timeline
    I2_QML_PROPERTY_DELETE_PROOF(ActionVM*, selectedActionVMInTimeline)

    // Number of lines in our timeline
    I2_QML_PROPERTY(int, linesNumberInTimeLine)

    // Flag indicating if our scenario is currently playing
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isPlaying)

    // Current time (from the beginning of our scenario)
    I2_QML_PROPERTY(QTime, currentTime)

    // List of (future) actions to evaluate at each timeout of our timer
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, listOfActionsToEvaluate)

    // List of active actions (the current time is between start time and end time of these actions)
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, listOfActiveActions)

    // Next action view model to activate
    I2_QML_PROPERTY(ActionVM*, nextActionToActivate)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param scenariosPath
     * @param parent
     */
    explicit ScenarioController(IngeScapeModelManager* modelManager,
                                JsonHelper* jsonHelper,
                                QString scenariosPath,
                                QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ScenarioController();


    /**
    * @brief Get our filtered list of "time ticks"
    * @return
    */
    AbstractTimeRangeFilter* filteredListActionsInTimeLine()
    {
        return &_filteredListActionsInTimeLine;
    }


    /**
      * @brief Import the scenario lists structure from the json byte content
      * @param byteArrayOfJson
      */
    void importScenarioFromJson(QByteArray byteArrayOfJson);


    /**
      * @brief Check if an agent is defined into tha actions (conditions and effects)
      * @param agent name
      */
    bool isAgentDefinedInActions(QString agentName);


    /**
      * @brief Open the action editor with a model of action
      * @param action
      */
    Q_INVOKABLE void openActionEditorWithModel(ActionM* action);


    /**
      * @brief Open the action editor with a view model of action
      * @param action
      */
    Q_INVOKABLE void openActionEditorWithViewModel(ActionVM* action);


    /**
      * @brief Delete an action from the list
      * @param action
      */
    Q_INVOKABLE void deleteAction(ActionM* action);


    /**
      * @brief Validate action editior
      * @param action editor controller
      */
    Q_INVOKABLE void validateActionEditor(ActionEditorController* actionEditorC);


    /**
      * @brief Close action editior
      * @param action editor controller
      */
    Q_INVOKABLE void closeActionEditor(ActionEditorController* actionEditorC);


    /**
     * @brief Set an action into the palette at index
     * @param index where to insert the action
     * @param action to insert
     */
    Q_INVOKABLE void setActionInPalette(int index, ActionM* actionM);


    /**
     * @brief Add an action VM at the time in ms
     * @param action model
     * @param line number
     */
    Q_INVOKABLE void addActionVMAtTime(ActionM* actionM, int timeInMs, int lineNumber = -1);


    /**
     * @brief Add an action VM at the current date time
     * @param action model
     */
    Q_INVOKABLE void addActionVMAtCurrentTime(ActionM* actionM);


    /**
     * @brief Remove an action VM from the time line
     * @param action view model
     */
    Q_INVOKABLE void removeActionVMFromTimeLine(ActionVM * actionVM);


    /**
     * @brief Initialize the connections for conditions of all actions
     */
    Q_INVOKABLE void initializeConditionsConnectionsOfAllActions();


    /**
     * @brief Reset the connections for conditions of all actions
     */
    Q_INVOKABLE void resetConditionsConnectionsOfAllActions();


    /**
     * @brief Test if an item can be inserted into a line number
     * @param actionM to insert
     * @param time into insert
     * @param line number
     * @param optional excluded actionVM from the check
     */
    Q_INVOKABLE bool canInsertActionVMTo(ActionM *actionMToInsert, int time, int lineNumber, ActionVM* excludedActionVM = NULL);


    /**
     * @brief Execute all effects of the action
     * @param action
     */
    Q_INVOKABLE void executeEffectsOfAction(ActionM* action);


    /**
     * @brief Clear the list of actions in the table / palette / timeline
     */
    Q_INVOKABLE void clearScenario();


    /**
     * @brief Move an actionVM to a start time position in ms and a specific line number
     * @param action VM
     * @param time in milliseconds
     * @param line number
     */
    Q_INVOKABLE void moveActionVMAtTimeAndLine(ActionVM* actionVM, int timeInMilliseconds, int lineNumber);


    /**
     * @brief Can delete an action from the list
     *        Check dependencies in the timeline
     * @param action to delete
     * @return can delete response
     */
    Q_INVOKABLE bool canDeleteActionFromList(ActionM* actionM);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToLauncher(QString command, QString hostname, QString commandLine);


    /**
     * @brief Signal emitted when a command must be sent on the network to a recorder
     * @param commandAndParameters
     */
    void commandAskedToRecorder(QString commandAndParameters);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Emitted when a command must be sent on the network to an agent about setting a value to one of its Input/Output/Parameter
     * @param peerIdsList
     * @param command
     * @param agentIOPName
     * @param value
     */
    void commandAskedToAgentAboutSettingValue(QStringList peerIdsList, QString command, QString agentIOPName, QString value);


    /**
     * @brief Emitted when a command must be sent on the network to an agent about mapping one of its input
     * @param peerIdsList
     * @param command
     * @param inputName
     * @param outputAgentName
     * @param outputName
     */
    void commandAskedToAgentAboutMappingInput(QStringList peerIdsList, QString command, QString inputName, QString outputAgentName, QString outputName);


public Q_SLOTS:

    /**
      * @brief Slot called when an agent is added in the mapping
      */
    void onAgentInMappingAdded(AgentInMappingVM* agentAdded);


    /**
      * @brief Slot called when an agent is removed from the mapping
      */
    void onAgentInMappingRemoved(AgentInMappingVM* agentRemoved);


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



private Q_SLOTS:

    /**
     * @brief Called when our timer time out to handle the scenario and execute actions
     */
    void _onTimeout_ExecuteActions();


    /**
     * @brief Called at each interval of our timer to delay actions when their conditions are not valid or execute them otherwise
     */
    void _onTimeout_DelayOrExecuteActions();


private:

    /**
     * @brief Get a new action name
     */
    QString _buildNewActionName();


    /**
     * @brief Open the scenario from JSON file
     * @param scenarioFilePath
     */
    void _openScenarioFromFile(QString scenarioFilePath);


    /**
     * @brief Save the scenario to JSON file
     * @param scenarioFilePath
     */
    void _saveScenarioToFile(QString scenarioFilePath);


    /**
     * @brief Insert an actionVM into our timeline
     * @param action view model
     * @param line number
     */
    void _insertActionVMIntoMapByLineNumber(ActionVM* actionVMToInsert, int lineNumberRef);


    /**
     * @brief Start the scenario by
     *        making connections for the actions conditions
     *        starting the action evaluation timer
     */
    void _startScenario();


    /**
     * @brief Stop the scenario by
     *        disconnecting the actions conditions
     *        stoping the action evaluation timer
     */
    void _stopScenario();


    /**
     * @brief Get the agent in mapping from an agent name
     * @param agentName
     * @return
     */
    AgentInMappingVM* _getAgentInMappingFromName(QString agentName);


    /**
     * @brief Execute reverse effects of an action
     * @param actionExecution
     */
    void _executeReverseEffectsOfAction(ActionExecutionVM* actionExecution);


    /**
     * @brief Execute a command for an agent
     * @param agent
     * @param commandAndParameters
     */
    void _executeCommandForAgent(AgentInMappingVM* agent, QStringList commandAndParameters);


    /**
     * @brief Exectute the action with the revert initialization if necessary
     * @param action view model
     * @param action execution view model
     * @param current time in ms
     */
    void _executeAction(ActionVM* actionVM, ActionExecutionVM* actionExecution, int currentTimeInMilliSeconds);


    /**
     * @brief Get the "Action Editor" from a model of action
     * @return
     */
    ActionEditorController* _getActionEditorFromModelOfAction(ActionM* action);


    /**
     * @brief Get the "Action Editor" from a view model of action
     * @return
     */
    ActionEditorController* _getActionEditorFromViewModelOfAction(ActionVM* action);


private:

    // Manager for the data model of INGESCAPE
    IngeScapeModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Path to the directory containing JSON files to save scenarios
    QString _scenariosDirectoryPath;

    // Hash table of action editor controller from a model of action
    QHash<ActionM*, ActionEditorController*> _hashActionEditorControllerFromModelOfAction;

    // Hash table of action editor controller from a view model of action
    QHash<ActionVM*, ActionEditorController*> _hashActionEditorControllerFromViewModelOfAction;

    // Map of actions model from the action name
    QHash<QString, ActionM*> _mapActionsFromActionName;

    // Hash table from UID to the corresponding model of action
    QHash<int, ActionM*> _hashFromUidToModelOfAction;

    // Map of actions VM in the timeline from the action model
    QHash<ActionM*, QList<ActionVM*> > _mapActionsVMsInTimelineFromActionModel;

    // Map of actions VM in the timeline from the line index
    QHash<int, I2CustomItemSortFilterListModel<ActionVM>* > _mapActionsVMsInTimelineFromLineIndex;

    // Timer to handle the scenario and execute actions
    QTimer _timerToExecuteActions;

    // Timer to regularly delay actions (when their conditions are not valid)
    QTimer _timerToRegularlyDelayActions;

    // Time of the day in milli-seconds when user starts the scenario, then at the last timeout (of the timer which regularly delay actions)
    int _timeOfDayInMS_WhenStartScenario_ThenAtLastTimeOut;

    // List of actionVM in timeline filtered with a given time range in milliseconds
    AbstractTimeRangeFilter _filteredListActionsInTimeLine;

    // Map from agent name to the (view model of) agent in mapping
    QHash<QString, AgentInMappingVM*> _mapFromNameToAgentInMapping;

};

QML_DECLARE_TYPE(ScenarioController)

#endif // SCENARIOCONTROLLER_H
