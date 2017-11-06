/*
 *	ActionEditorController
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

#include "actioneditorcontroller.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionEditorController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionEditorController::ActionEditorController(ActionVM * originalAction, QObject *parent) : QObject(parent),
    _originalAction(originalAction),
    _editedAction(NULL)
{
    if(_originalAction != NULL)
    {
        _editedAction = new ActionVM(NULL, this);
        _editedAction->copyFrom(_originalAction);
    }
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionEditorController::~ActionEditorController()
{
    setoriginalAction(NULL);

    if(_editedAction != NULL)
    {
        ActionVM* tmp = _editedAction;
        seteditedAction(NULL);
        delete tmp;
        tmp = NULL;
    }
}

/**
 * @brief Valide the edition/creation
 */
void ActionEditorController::validateModification()
{
    if(_originalAction == NULL)
    {
        setoriginalAction(_editedAction);
        seteditedAction(NULL);
    }
    else {
        _originalAction->copyFrom(_editedAction);
    }
}


