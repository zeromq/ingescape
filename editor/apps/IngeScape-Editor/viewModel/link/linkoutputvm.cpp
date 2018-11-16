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

#include "linkoutputvm.h"


/**
 * @brief Constructor
 * @param output
 * @param parent
 */
LinkOutputVM::LinkOutputVM(OutputVM* output,
                           QObject *parent) : QObject(parent),
    _output(output),
    _position(QPointF())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_output != nullptr) {
        _name = _output->name();
    }
}


/**
 * @brief Destructor
 */
LinkOutputVM::~LinkOutputVM()
{
    if (_output != nullptr) {
        setoutput(nullptr);
    }
}
