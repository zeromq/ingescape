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
#include "iopvalueeffectm.h"

/**
 * @brief Enum "AgentEffectValues" to string
 * @param value
 * @return
 */
QString AgentEffectValues::enumToString(int value)
{
    QString string = "Agent Effect Value";

    switch (value)
    {
    case AgentEffectValues::ON:
        string = "ON";
        break;

    case AgentEffectValues::OFF:
        string = "OFF";
        break;

    default:
        break;
    }

    return string;
}

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
    _agent(NULL),
    _agentEffectValue(AgentEffectValues::ON)
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
    setagent(NULL);
}

/**
* @brief Copy from another effect model
* @param effect to copy
*/
void ActionEffectM::copyFrom(ActionEffectM* effect)
{
    if (effect != NULL)
    {
        setagent(effect->agent());

        // FIXME TO TEST !?
        setagentEffectValue(effect->agentEffectValue());
    }
}

/**
 * @brief Called when our agent is destroyed
 * @param sender
 */
void ActionEffectM::_onAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagent(NULL);
}


/**
* @brief Custom setter for agent
* @param value
*/
void ActionEffectM::setagent(AgentInMappingVM* value)
{
    if(_agent != value)
    {
        if (_agent != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agent, &AgentInMappingVM::destroyed, this, &ActionEffectM::_onAgentDestroyed);
        }

        _agent = value;

        if (_agent != NULL)
        {
            // Subscribe to destruction
            connect(_agent, &AgentInMappingVM::destroyed, this, &ActionEffectM::_onAgentDestroyed);
        }

        Q_EMIT agentChanged(value);
    }
}


