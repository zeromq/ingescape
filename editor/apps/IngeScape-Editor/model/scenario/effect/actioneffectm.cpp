/*
 *	ActionEffectM
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

#include "actioneffectm.h"

#include <QDebug>


//--------------------------------------------------------------
//
// ActionEffectM
//
//--------------------------------------------------------------

/**
 * @brief Default constructor
 * @param parent
 */
ActionEffectM::ActionEffectM(QObject *parent) : QObject(parent),
    _agent(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionEffectM::~ActionEffectM()
{
    // Reset agent to null
    setagent(nullptr);
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void ActionEffectM::copyFrom(ActionEffectM* effect)
{
    if (effect != nullptr) {
        setagent(effect->agent());
    }
}


/**
* @brief Custom setter for agent
* @param value
*/
void ActionEffectM::setagent(AgentsGroupedByNameVM* value)
{
    if (_agent != value)
    {
        if (_agent != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_agent, &AgentsGroupedByNameVM::destroyed, this, &ActionEffectM::_onAgentDestroyed);
        }

        _agent = value;

        if (_agent != nullptr)
        {
            // Subscribe to destruction
            connect(_agent, &AgentsGroupedByNameVM::destroyed, this, &ActionEffectM::_onAgentDestroyed);
        }

        Q_EMIT agentChanged(value);
    }
}


/**
 * @brief FIXME custom event instead: Called when our agent is destroyed
 * @param sender
 */
void ActionEffectM::_onAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagent(nullptr);

    Q_EMIT askForDestruction();
}
