
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
#include "controller/actioneditorcontroller.h"
#include "masticmodelmanager.h"


/**
 * @brief The ScenarioController class defines the main controller of our application
 */
class ScenarioController: public QObject
{
    Q_OBJECT

    // Filtered Sorted list of actions
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ActionM, actionsList)

    // Sorted list of ations by start time
    I2_QOBJECT_LISTMODEL(ActionEditorController, openedActionsEditorsControllers)

    // Selected action
    I2_QML_PROPERTY_DELETE_PROOF(ActionM*, selectedAction)

    // Used for action edition
    // --- List of comparisons values types
    I2_ENUM_LISTMODEL(ActionComparisonValueType, comparisonsValuesTypesList)
    // --- List of comparisons state types
    I2_ENUM_LISTMODEL(ActionComparisonValueType, comparisonsStatesTypesList)
    // --- List of validity duration type
    I2_ENUM_LISTMODEL(ValidationDurationType, validationDurationsTypesList)
    // --- List of effects states type
    I2_ENUM_LISTMODEL(ActionEffectValueType, effectsStatesTypesList)
    // --- List of effects links type
    I2_ENUM_LISTMODEL(ActionEffectValueType, effectsLinksTypesList)

    // --- List of conditions type
    I2_ENUM_LISTMODEL(ActionConditionType, conditionsTypesList)
    // --- List of effects type
    I2_ENUM_LISTMODEL(ActionEffectType, effectsTypesList)


    // --- agents list in mapping
    I2_QOBJECT_LISTMODEL(AgentInMappingVM, agentsInMappingList)



public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit ScenarioController(QObject *parent = 0);


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

Q_SIGNALS:


public Q_SLOTS:

    /**
      * @brief slot on agents in mapping list count change
      */
    void onAgentsInMappingListCountChange();

private :
    /**
     * @brief Get a new action name
     */
    QString _buildNewActionName();

protected:

    QHash<ActionM*, ActionEditorController*> _mapActionsEditorControllersFromActionVM;

    QHash<QString, ActionM*> _mapActionsFromActionName;
};

QML_DECLARE_TYPE(ScenarioController)

#endif // SCENARIOCONTROLLER_H
