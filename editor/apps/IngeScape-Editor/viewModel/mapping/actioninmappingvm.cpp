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

#include "actioninmappingvm.h"

/**
 * @brief Constructor
 * @param action
 * @param position
 * @param parent
 */
ActionInMappingVM::ActionInMappingVM(ActionM* action,
                                     QPointF position,
                                     QObject *parent) : ObjectInMappingVM("",
                                                                          position,
                                                                          parent),
    _action(action)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_action != nullptr)
    {
        _name = _action->name();

        qInfo() << "New Action" << _name << "in the global mapping";
    }
}


/**
 * @brief Destructor
 */
ActionInMappingVM::~ActionInMappingVM()
{
    qInfo() << "Delete Action" << _name << "in the global mapping";
}
