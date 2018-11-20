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

#include "inputvm.h"

/**
 * @brief Constructor
 * @param inputName
 * @param inputId
 * @param modelM
 * @param parent
 */
InputVM::InputVM(QString inputName,
                 QString inputId,
                 AgentIOPM* modelM,
                 QObject *parent) : AgentIOPVM(inputName,
                                               inputId,
                                               modelM,
                                               parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    //qInfo() << "New Input VM" << _name << "(" << _id << ")";

}


/**
 * @brief Destructor
 */
InputVM::~InputVM()
{
    //qInfo() << "Delete Input VM" << _name << "(" << _id << ")";

}
