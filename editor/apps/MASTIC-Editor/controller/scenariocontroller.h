
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

#ifndef SCENARIOCONTROLLER_H
#define SCENARIOCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include "viewModel/actionvm.h"
#include "viewModel/agentinmappingvm.h"
#include "viewModel/actioninpalettevm.h"
#include "controller/actioneditorcontroller.h"
#include "masticmodelmanager.h"


// Margin in milliseconds to insert an action following another
#define MARGIN_FOR_ACTION_INSERTION_IN_MS 2000

// Minimum lines displayed into the timeline by default
#define MINIMUM_DISPLAYED_LINES_NUMBER_IN_TIMELINE 7

// Interval in milli-seconds to evaluate each actionVM conditions
#define INTERVAL_EVALUATION_ACTIONS 500

/**
 * @brief The ScenarioController class defines the main controller of our scenario edition
 */
class ScenarioController: public QObject
{
    Q_OBJECT

    // Filtered Sorted list of actions
    I2_QOBJECT_LISTMODEL(ActionM, actionsList)

    // Sorted list of ations by start time
    I2_QOBJECT_LISTMODEL(ActionEditorController, openedActionsEditorsControllers)

    // Selected action
    I2_QML_PROPERTY_DELETE_PROOF(ActionM*, selectedAction)

    // Used for action edition
    // --- List of comparisons values types
    I2_ENUM_LISTMODEL(ActionComparisonValueType, comparisonsValuesTypesList)
    // --- List of comparisons state types
    I2_ENUM_LISTMODEL(ActionComparisonValueType, comparisonsAgentsTypesList)
    // --- List of validity duration type
    I2_ENUM_LISTMODEL(ValidationDurationType, validationDurationsTypesList)

    // --- List of values about effect on agent
    I2_ENUM_LISTMODEL(AgentEffectValues, agentEffectValuesList)
    // --- List of effects links type
    I2_ENUM_LISTMODEL(MappingEffectValues, mappingEffectValuesList)

    // --- List of conditions type
    I2_ENUM_LISTMODEL(ActionConditionType, conditionsTypesList)
    // --- List of effects type
    I2_ENUM_LISTMODEL(ActionEffectTypes, effectsTypesList)

    // --- agents list in mapping
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentInMappingVM, agentsInMappingList)


    // List of actions in palette
    I2_QOBJECT_LISTMODEL(ActionInPaletteVM, actionsInPaletteList)

    // List of actions in timeline
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, actionsInTimeLine)

    // Number of line in our timeline
    I2_QML_PROPERTY(int, linesNumberInTimeLine)

    // Is playing scenario flag
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isPlaying)

    // Current time (from the beginning of our scenario)
    I2_QML_PROPERTY_READONLY(QTime, currentTime)

    // List of actionsVM to evaluate each timeout of our timer linked to our scenario
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionVM, activeActionsVMList)

public:

    /**
     * @brief Default constructor
     * @param scenarios files path
     * @param parent
     */
    explicit ScenarioController(QString scenariosPath, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~ScenarioController();

    /**
      * @brief Open the action editor
      * @param action model
      */
    Q_INVOKABLE void openActionEditor(ActionM* actionM);

    /**
      * @brief Open the action editor
      * @param action view model
      */
    Q_INVOKABLE void openActionEditorFromActionVM(ActionVM* actionVM);

    /**
      * @brief Delete an action from the list
      * @param action model
      */
    Q_INVOKABLE void deleteAction(ActionM * actionM);

    /**
      * @brief Valide action edition
      * @param action editor controller
      */
    Q_INVOKABLE void valideActionEditor(ActionEditorController* actionEditorC);

    /**
      * @brief Close action edition
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
     * @brief Import a scenario a file (actions, palette, timeline actions )
     */
    Q_INVOKABLE void importScenarioFromFile();

    /**
     * @brief Export a scenario to a selected file (actions, palette, timeline actions)
     */
    Q_INVOKABLE void exportScenarioToSelectedFile();

    /**
     * @brief Add an action VM at the time in ms
     * @param action model
     * @param line number
     */
    Q_INVOKABLE void addActionVMAtTime(ActionM * actionModel, int timeInMs, int lineNumber = -1);

    /**
     * @brief Add an action VM at the current date time
     * @param action model
     */
    Q_INVOKABLE void addActionVMAtCurrentTime(ActionM * actionModel);

    /**
     * @brief Remove an action VM from the time line
     * @param action view model
     */
    Q_INVOKABLE void removeActionVMFromTimeLine(ActionVM * actionVM);

    /**
     * @brief Make conditions connections
     */
    Q_INVOKABLE void conditionsConnect();

    /**
     * @brief Conditions disconnections
     */
    Q_INVOKABLE void conditionsDisconnect();

    /**
     * @brief Test if an item can be inserted into a line number
     * @param actionM to insert
     * @param time into insert
     * @param line number
     */
    Q_INVOKABLE bool canInsertActionVMTo(ActionM *actionMToInsert, int time, int lineNumber);


    /**
     * @brief Execute all effects of the action
     * @param action
     */
    Q_INVOKABLE void executeEffectsOfAction(ActionM* action);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToLauncher(QString command, QString hostname, QString commandLine);


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
      * @brief slot on agent added in mapping
      */
    void onAgentInMappingAdded(AgentInMappingVM* agentAdded);


    /**
      * @brief slot on agent removed in mapping
      */
    void onAgentInMappingRemoved(AgentInMappingVM* agentRemoved);


private Q_SLOTS:

    /**
     * @brief Called at each interval of our timer to update the current state of each zones that have at least one loudspeakers line
     */
    void _onTimeout_EvaluateActions();


private :
    /**
     * @brief Get a new action name
     */
    QString _buildNewActionName();

    /**
     * @brief Import the scenario from JSON file
     * @param scenarioFilePath
     */
    void _importScenarioFromFile(QString scenarioFilePath);

    /**
     * @brief Export the scenario to JSON file
     * @param scenarioFilePath
     */
    void _exportScenarioToFile(QString scenarioFilePath);

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
     * @brief Execute a list of commands for agents
     * @param commandsForAgents
     */
    void _executeCommandsForAgents(QList<QPair<QString, QStringList>> commandsForAgents);


protected:

    // Path to the directory containing JSON files to save scenarios
    QString _scenariosDirectoryPath;
    QString _scenariosDefaultFilePath;

    // Helper to manage JSON definitions of agents
    JsonHelper* _jsonHelper;

    // Map of actions editors controllers from the actions model
    QHash<ActionM*, ActionEditorController*> _mapActionsEditorControllersFromActionM;

    // Map of actions editors controllers from the actions view model
    QHash<ActionVM*, ActionEditorController*> _mapActionsEditorControllersFromActionVM;

    // Map of actions model from the action name
    QHash<QString, ActionM*> _mapActionsFromActionName;

    // Map of actions VM in the timeline from the action model
    QHash<ActionM*, QList<ActionVM*> > _mapActionsVMsInTimelineFromActionModel;

    // Map of actions VM in the timeline from the line index
    QHash<int, I2CustomItemSortFilterListModel<ActionVM>* > _mapActionsVMsInTimelineFromLineIndex;

    // Main timer to handle the scenario and evaluate actions
    QTimer _timerToEvaluateActions;

private:
    // Time in milliseconds of our scenario start
    int _scenarioStartingTimeInMs;

};

QML_DECLARE_TYPE(ScenarioController)

#endif // SCENARIOCONTROLLER_H
