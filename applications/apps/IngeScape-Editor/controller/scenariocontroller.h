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

#ifndef SCENARIO_CONTROLLER_H
#define SCENARIO_CONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include <controller/abstractscenariocontroller.h>
#include <controller/actioneditorcontroller.h>


/**
 * @brief The ScenarioController class defines the controller for scenario management
 */
class ScenarioController: public AbstractScenarioController
{
    Q_OBJECT

    // List of opened action editors
    I2_QOBJECT_LISTMODEL(ActionEditorController, openedActionsEditorsControllers)

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

    // List of actions in palette
    I2_QOBJECT_LISTMODEL(ActionInPaletteVM, actionsInPaletteList)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit ScenarioController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~ScenarioController();


    /**
      * @brief Import the scenario from JSON
      * @param jsonScenario
      */
    void importScenarioFromJson(QJsonObject jsonScenario) Q_DECL_OVERRIDE;


    /**
      * @brief Delete an action from the list
      * @param action
      */
    Q_INVOKABLE void deleteAction(ActionM* action) Q_DECL_OVERRIDE;


    /**
     * @brief Remove an action VM from the time line
     * @param action view model
     */
    Q_INVOKABLE void removeActionVMfromTimeLine(ActionVM* actionVM) Q_DECL_OVERRIDE;


    /**
     * @brief Clear the current scenario
     * (clear the list of actions in the list, in the palette and in the timeline)
     */
    Q_INVOKABLE void clearScenario() Q_DECL_OVERRIDE;


    /**
      * @brief Check if an agent is used in the current scenario (actions, conditions, effects)
      * @param agent name
      */
    bool isAgentUsedInScenario(QString agentName);


    /**
     * @brief Open the action editor with a duplicate model of action
     * @param The duplicated action
     */
    Q_INVOKABLE void openActionEditorToDuplicateModel(ActionM* action);


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
     * @brief Set a model of action into the palette at index
     * @param index where to insert the action
     * @param actionM model of action to insert
     */
    Q_INVOKABLE void setActionInPalette(int index, ActionM* actionM);


    /**
     * @brief Move an actionVM to a start time position in ms and a specific line number
     * @param action VM
     * @param time in milliseconds
     * @param line index
     */
    Q_INVOKABLE void moveActionVMAtTimeAndLine(ActionVM* actionVM, int timeInMilliseconds, int lineIndex);


    /**
     * @brief Check if a view model of an action has been inserted in the timeline
     * @param actionM
     * @return
     */
    Q_INVOKABLE bool isActionInsertedInTimeLine(ActionM* actionM);


Q_SIGNALS:


public Q_SLOTS:


private:

    /**
     * @brief Get a new action name
     */
    QString _buildNewActionName();


    /**
     * @brief Get a name for a new copied action
     * @param the duplicate action name
     */
    QString _buildDuplicateActionName(QString actionName);


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

    // Hash table of action editor controller from a model of action
    QHash<ActionM*, ActionEditorController*> _hashActionEditorControllerFromModelOfAction;

    // Hash table of action editor controller from a view model of action
    QHash<ActionVM*, ActionEditorController*> _hashActionEditorControllerFromViewModelOfAction;

};

QML_DECLARE_TYPE(ScenarioController)

#endif // SCENARIO_CONTROLLER_H
