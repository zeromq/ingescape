/*
 *	IngeScape Editor
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

#include "actionconditionm.h"

/**
 * @brief Default constructor
 * @param parent
 */
ActionConditionM::ActionConditionM(QObject *parent) : QObject(parent),
    _agent(nullptr),
    _isValid(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
ActionConditionM::~ActionConditionM()
{
    // Reset the agent connections
    resetConnections();

    // Reset agent
    setagent(nullptr);
}


/**
* @brief Custom setter for agent
* @param agent
*/
void ActionConditionM::setagent(AgentsGroupedByNameVM* value)
{
    if (_agent != value)
    {
        if (_agent != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_agent, &AgentsGroupedByNameVM::destroyed, this, &ActionConditionM::_onAgentDestroyed);
        }

        // Reset
        setisValid(false);

        _agent = value;

        if (_agent != nullptr)
        {
            // Subscribe to destruction
            connect(_agent, &AgentsGroupedByNameVM::destroyed, this, &ActionConditionM::_onAgentDestroyed);
        }

        Q_EMIT agentChanged(value);
    }
}


/**
* @brief Copy from another condition model
* @param condition to copy
*/
void ActionConditionM::copyFrom(ActionConditionM* condition)
{
    if (condition != nullptr)
    {
        setagent(condition->agent());
        setisValid(condition->isValid());
    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void ActionConditionM::initializeConnections()
{
    if (_agent != nullptr)
    {
        // Reset the agent connections
        resetConnections();

        // Connect to signal emitted when the flag "is ON" changed
        connect(_agent, &AgentsGroupedByNameVM::isONChanged, this, &ActionConditionM::_onAgentIsOnChanged);

        // Force the call of the slot
        _onAgentIsOnChanged(_agent->isON());
    }
}


/**
  * @brief Reset the agent connections for the action condition
  */
void ActionConditionM::resetConnections()
{
    if (_agent != nullptr)
    {
        // DIS-connect to signal emitted when the flag "is ON" changed
        disconnect(_agent, &AgentsGroupedByNameVM::isONChanged, this, &ActionConditionM::_onAgentIsOnChanged);
    }
}


/**
 * @brief Slot called when the flag "is ON" of the agent changed
 * @param isON
 */
void ActionConditionM::_onAgentIsOnChanged(bool isON)
{
    Q_UNUSED(isON)
}


/**
 * @brief FIXME custom event instead: Called when our agent model is destroyed
 * @param sender
 */
void ActionConditionM::_onAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    // Reset our agent
    setagent(nullptr);

    Q_EMIT askForDestruction();
}
