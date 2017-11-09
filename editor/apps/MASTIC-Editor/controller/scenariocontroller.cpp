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



//--------------------------------------------------------------
//
// ScenarioController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ScenarioController::ScenarioController(QObject *parent) : QObject(parent),
    _selectedAction(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Fill state comparisons types list
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::ON);
    _comparisonsAgentsTypesList.appendEnumValue(ActionComparisonValueType::OFF);

    // Fill value comparisons types list
    _comparisonsValuesTypesList.fillWithAllEnumValues();
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::ON);
    _comparisonsValuesTypesList.removeEnumValue(ActionComparisonValueType::OFF);

    // Fill value effects types list
    _effectsStatesTypesList.appendEnumValue(ActionEffectValueType::ON);
    _effectsStatesTypesList.appendEnumValue(ActionEffectValueType::OFF);

    // Fill link effects types list
    _effectsLinksTypesList.appendEnumValue(ActionEffectValueType::ENABLE);
    _effectsLinksTypesList.appendEnumValue(ActionEffectValueType::DISABLE);

    // Fill general types
    _conditionsTypesList.fillWithAllEnumValues();
    _effectsTypesList.fillWithAllEnumValues();

    // Fill validity duration types list
    _validationDurationsTypesList.fillWithAllEnumValues();
}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
    // Clean-up current selection
    setselectedAction(NULL);

    // Clear the list of editor opened
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
    if(_mapActionsEditorControllersFromActionVM.contains(actionM) == false)
    {
        // Create an empty action if we create a new one
        if(actionM == NULL)
        {
            actionM = new ActionM(_buildNewActionName());
        }
        // Set selected action
        else
        {
            setselectedAction(actionM);
        }

        // Create action editor controller
        ActionEditorController* actionEditorC = new ActionEditorController(actionM,agentsInMappingList());

        // Add action into our opened actions
        _mapActionsEditorControllersFromActionVM.insert(actionM,actionEditorC);

        // Add to list
        _openedActionsEditorsControllers.append(actionEditorC);
    }
}


/**
  * @brief Delete an action from the list
  * @param action view model
  */
void ScenarioController::deleteAction(ActionM * actionM)
{
    // Unselect our action if needed
    if (_selectedAction == actionM) {
        setselectedAction(NULL);
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
    ActionM* actionM = actionEditorC->originalAction();
    // Delete the popup if necessary
    if(actionM != NULL && _mapActionsEditorControllersFromActionVM.contains(actionM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionM);

        _mapActionsEditorControllersFromActionVM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }
}

/**
  * @brief slot on agents in mapping list count change
  */
void ScenarioController::onAgentsInMappingListCountChange()
{
    I2CustomItemListModel<AgentInMappingVM> * agentInMappingList = dynamic_cast<I2CustomItemListModel<AgentInMappingVM> *>(sender());
    if (agentInMappingList != NULL)
    {
        // Reset the agents in mapping list
        _agentsInMappingList.clear();

        // Add the new list of agents
        _agentsInMappingList.append(agentInMappingList->toList());
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


