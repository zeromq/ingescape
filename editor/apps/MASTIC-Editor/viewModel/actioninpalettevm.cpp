/*
 *	ActionInPaletteVM
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

#include "actioninpalettevm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// ActionInPaletteVM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param action model
 * @param parent
 */
ActionInPaletteVM::ActionInPaletteVM(ActionM* actionM, QObject *parent) : QObject(parent),
    _actionModel(actionM)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionInPaletteVM::~ActionInPaletteVM()
{
    setactionModel(NULL);
}

/**
 * @brief Custom setter on action model
 */
void ActionInPaletteVM::setactionModel(ActionM* actionM)
{
    if(_actionModel != actionM)
    {
        _actionModel = actionM;

        // Reset status
        setstatus(ActionInPaletteState::DISABLE);

        emit actionModelChanged(actionM);
    }
}


