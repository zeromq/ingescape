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
ScenarioController::ScenarioController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ScenarioController::~ScenarioController()
{
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
void ScenarioController::openActionEditor(ActionVM* actionVM)
{
    // We check that or editor is not already opened
    if(_mapActionsEditorControllersFromActionVM.contains(actionVM) == false)
    {
        // Create an empty action if we create a new one
        if(actionVM == NULL)
        {
            ActionM* actionM = new ActionM("New action");
            actionVM = new ActionVM(actionM);
        }

        // Create action editor controller
        ActionEditorController* actionEditorC = new ActionEditorController(actionVM);

        // Add action into our opened actions
        _mapActionsEditorControllersFromActionVM.insert(actionVM,actionEditorC);

        // Add to list
        _openedActionsEditorsControllers.append(actionEditorC);
    }
}


/**
  * @brief Delete an action from the list
  * @param action view model
  */
void ScenarioController::deleteAction(ActionVM * actionVM)
{
    // Delete the popup if necessary
    if(_mapActionsEditorControllersFromActionVM.contains(actionVM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionVM);

        _mapActionsEditorControllersFromActionVM.remove(actionVM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }

    // Delete the action item
    if(_actionsList.contains(actionVM))
    {
        _actionsList.remove(actionVM);

        delete actionVM;
        actionVM = NULL;
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

    ActionVM* originalActionVM = actionEditorC->originalAction();

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
    ActionVM* actionVM = actionEditorC->originalAction();
    // Delete the popup if necessary
    if(actionVM != NULL && _mapActionsEditorControllersFromActionVM.contains(actionVM))
    {
        ActionEditorController* actionEditorC = _mapActionsEditorControllersFromActionVM.value(actionVM);

        _mapActionsEditorControllersFromActionVM.remove(actionVM);
        _openedActionsEditorsControllers.remove(actionEditorC);
    }
}

/**
  * @brief Delete action edition
  * @param action editor controller
  */
void ScenarioController::deleteActionEditor(ActionEditorController* actionEditorC)
{
    ActionVM* actionVM = actionEditorC->originalAction();

    // Delete the original action
    if(actionVM != NULL)
    {
        deleteAction(actionVM);
    }
}




