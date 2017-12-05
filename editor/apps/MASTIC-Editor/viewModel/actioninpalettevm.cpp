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
ActionInPaletteVM::ActionInPaletteVM(ActionM* actionM,
                                     int indexInPanel,
                                     QObject *parent) : QObject(parent),
    _modelM(actionM),
    _indexInPanel(indexInPanel)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionInPaletteVM::~ActionInPaletteVM()
{

}


/**
 * @brief Custom setter for property "modelM"
 */
void ActionInPaletteVM::setmodelM(ActionM* value)
{
    if (_modelM != value)
    {
        _modelM = value;

        // Reset status
        setstatus(ActionInPaletteState::DISABLE);

        Q_EMIT modelMChanged(value);
    }
}


