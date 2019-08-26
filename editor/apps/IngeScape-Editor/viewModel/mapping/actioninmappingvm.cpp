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
 * @brief Default width of the item when created
 */
const qreal ActionInMappingVM::DEFAULT_WIDTH = 220;


/**
 * @brief Constructor
 * @param uid
 * @param action
 * @param position
 * @param parent
 */
ActionInMappingVM::ActionInMappingVM(QString uid,
                                     ActionM* action,
                                     QPointF position,
                                     qreal width,
                                     QObject *parent) : ObjectInMappingVM(ObjectInMappingTypes::ACTION,
                                                                          position,
                                                                          width,
                                                                          parent),
    _action(action),
    _linkInput(nullptr),
    _linkOutput(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_action != nullptr)
    {
        _name = _action->name();
        _uid = uid;

        // Create the link input and the link output
        _linkInput = new LinkInputVM(nullptr, this);
        _linkOutput = new LinkOutputVM(nullptr, this);

        qInfo() << "New Action" << _name << "(" << _uid << ") in the global mapping";

        // Connect to signals from the model of action
        connect(_action, &ActionM::allEffectsHaveBeenExecuted, this, &ActionInMappingVM::_onAllEffectsHaveBeenExecuted);
        connect(_action, &ActionM::nameChanged, this, &ActionInMappingVM::_onActionNameChanged);
    }
}


/**
 * @brief Destructor
 */
ActionInMappingVM::~ActionInMappingVM()
{
    qInfo() << "Delete Action" << _name << "(" << _uid << ") in the global mapping";

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
        /*if (_action != nullptr) {
            qDebug() << "Activate (link) output for action" << _action->name();
        }*/

        // Activate the link
        _linkOutput->activate();
    }
}


/**
 * @brief Slot called when the name of the action changed
 * @param name
 */
void ActionInMappingVM::_onActionNameChanged(QString name)
{
    //qDebug() << "Action name changed from" << _name << "to" << name;

    setname(name);
}
