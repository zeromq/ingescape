/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "actioninpalettevm.h"

#include <QDebug>


/**
 * @brief Constructor
 * @param actionM
 * @param indexInPanel
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
    if (_modelM != NULL) {
        setmodelM(NULL);
    }
}


