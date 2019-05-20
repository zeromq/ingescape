/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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
 * @param uid
 * @param action
 * @param position
 * @param parent
 */
ActionInMappingVM::ActionInMappingVM(int uid,
                                     ActionM* action,
                                     QPointF position,
                                     QObject *parent) : ObjectInMappingVM(ObjectInMappingTypes::ACTION,
                                                                          "",
                                                                          position,
                                                                          parent),
    _uid(uid),
    _action(action),
    _linkInput(nullptr),
    _linkOutput(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_action != nullptr)
    {
        _name = _action->name();

        // Create the link input and the link output
        _linkInput = new LinkInputVM(nullptr, this);
        _linkOutput = new LinkOutputVM(nullptr, this);

        qInfo() << "New Action" << _name << "(" << _uid << ") in the global mapping";

        // Connect to signals from the model of action
        connect(_action, &ActionM::allEffectsHaveBeenExecuted, this, &ActionInMappingVM::_onAllEffectsHaveBeenExecuted);
    }
}


/**
 * @brief Destructor
 */
ActionInMappingVM::~ActionInMappingVM()
{
    qInfo() << "Delete Action" << _name << "in the global mapping";

    if (_action != nullptr)
    {
        // DIS-connect to signals from the model of action
        disconnect(_action, nullptr, this, nullptr);

        // Deleted elsewhere
        setaction(nullptr);
    }

    // Free memory
    if (_linkInput != nullptr)
    {
        LinkInputVM* temp = _linkInput;
        setlinkInput(nullptr);
        delete temp;
    }

    // Free memory
    if (_linkOutput != nullptr)
    {
        LinkOutputVM* temp = _linkOutput;
        setlinkOutput(nullptr);
        delete temp;
    }
}


/**
 * @brief Slot called when all effects have been executed
 */
void ActionInMappingVM::_onAllEffectsHaveBeenExecuted()
{
    if (_linkOutput != nullptr)
    {
        if (_action != nullptr) {
            qDebug() << "Activate (link) output for action" << _action->name();
        }

        // Activate the link: allows to highlight the corresponding links
        _linkOutput->activate();
    }
}
