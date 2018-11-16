/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "linkinputvm.h"


/**
 * @brief Constructor
 * @param input
 * @param parent
 */
LinkInputVM::LinkInputVM(InputVM* input,
                         QObject *parent) : QObject(parent),
    _input(input),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_input != nullptr) {
        _name = _input->name();
    }
}


/**
 * @brief Destructor
 */
LinkInputVM::~LinkInputVM()
{
    if (_input != nullptr) {
        setinput(nullptr);
    }
}
