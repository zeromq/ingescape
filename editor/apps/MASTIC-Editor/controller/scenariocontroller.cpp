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
    _openedActionsEditors.clear();

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
    if(_openedActionsEditors.contains(actionVM) == false)
    {
        // Create an empty action if we create a new one
        if(actionVM == NULL)
        {
            ActionM* actionM = new ActionM("New action",this);
            actionVM = new ActionVM(actionM,this);

            _actionsList.append(actionVM);
        }

        // Add action into our opened actions
        _openedActionsEditors.append(actionVM);
    }
}


