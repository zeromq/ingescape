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
            actionM = new ActionM("New action");
        }

        // Create action editor controller
        ActionEditorController* actionEditorC = new ActionEditorController(actionM);

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

    // Delete the popup if necessary
    if(_mapActionsEditorControllersFromActionVM.contains(actionM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionM);

        _mapActionsEditorControllersFromActionVM.remove(actionM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }

    // Delete the action item
    if(_actionsList.contains(actionM))
    {
        _actionsList.remove(actionM);

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
        _actionsList.append(originalActionVM);
    }

    if(_mapActionsEditorControllersFromActionVM.contains(originalActionVM))
    {
        _mapActionsEditorControllersFromActionVM.remove(originalActionVM);
    }
    _openedActionsEditorsControllers.remove(actionEditorC);
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
  * @brief Delete action edition
  * @param action editor controller
  */
void ScenarioController::deleteActionEditor(ActionEditorController* actionEditorC)
{
    ActionM* actionM = actionEditorC->originalAction();

    // Delete the original action
    if(actionM != NULL)
    {
        deleteAction(actionM);
    }
}




