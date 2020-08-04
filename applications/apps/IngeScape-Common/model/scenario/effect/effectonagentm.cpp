/*
 *	IngeScape Editor
 *
 *  Copyright Â© 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "effectonagentm.h"

/**
 * @brief Enum "AgentEffectValues" to string
 * @param value
 * @return
 */
QString AgentEffectValues::enumToString(int value)
{
    switch (value)
    {
    case AgentEffectValues::ON:
        return tr("ON");

    case AgentEffectValues::OFF:
        return tr("OFF");

    case AgentEffectValues::MUTE:
        return tr("MUTE");

    case AgentEffectValues::UNMUTE:
        return tr("UNMUTE");

    case AgentEffectValues::FREEZE:
        return tr("FREEZE");

    case AgentEffectValues::UNFREEZE:
        return tr("UNFREEZE");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// EffectOnAgentM
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
EffectOnAgentM::EffectOnAgentM(QObject *parent) : ActionEffectM(parent),
    _agentEffectValue(AgentEffectValues::ON)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
  * @brief Destructor
  */
EffectOnAgentM::~EffectOnAgentM()
{

}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void EffectOnAgentM::copyFrom(ActionEffectM* effect)
{
    // Call our mother class
    ActionEffectM::copyFrom(effect);

    EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(effect);
    if (effectOnAgent != nullptr)
    {
        setagentEffectValue(effectOnAgent->agentEffectValue());
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentsGroupedByNameVM*, QStringList> EffectOnAgentM::getAgentAndCommandWithParameters()
{
    QPair<AgentsGroupedByNameVM*, QStringList> pairAgentAndCommandWithParameters;

    if (_agent != nullptr)
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        switch (_agentEffectValue)
        {
        case AgentEffectValues::ON: {
            commandAndParameters << command_StartPeer;
            break;
        }
        case AgentEffectValues::OFF: {
            commandAndParameters << command_StopPeer;
            break;
        }
        case AgentEffectValues::MUTE: {
            commandAndParameters << command_MuteAgent;
            break;
        }
        case AgentEffectValues::UNMUTE: {
            commandAndParameters << command_UnmuteAgent;
            break;
        }
        case AgentEffectValues::FREEZE: {
            commandAndParameters << command_FreezeAgent;
            break;
        }
        case AgentEffectValues::UNFREEZE: {
            commandAndParameters << command_UnfreezeAgent;
            break;
        }
        default:
            break;
        }

        pairAgentAndCommandWithParameters.second = commandAndParameters;
    }

    return pairAgentAndCommandWithParameters;
}


/**
 * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
 * @return
 */
QPair<QString, QStringList> EffectOnAgentM::getAgentNameAndReverseCommandWithParameters()
{
    QPair<QString, QStringList> pairAgentNameAndReverseCommand;

    if (_agent != nullptr)
    {
        pairAgentNameAndReverseCommand.first = _agent->name();

        QStringList reverseCommandAndParameters;

        switch (_agentEffectValue)
        {
        case AgentEffectValues::ON: {
            reverseCommandAndParameters << command_StopPeer;
            break;
        }
        case AgentEffectValues::OFF: {
            reverseCommandAndParameters << command_StartPeer;
            break;
        }
        case AgentEffectValues::MUTE: {
            reverseCommandAndParameters << command_UnmuteAgent;
            break;
        }
        case AgentEffectValues::UNMUTE: {
            reverseCommandAndParameters << command_MuteAgent;
            break;
        }
        case AgentEffectValues::FREEZE: {
            reverseCommandAndParameters << command_UnfreezeAgent;
            break;
        }
        case AgentEffectValues::UNFREEZE: {
            reverseCommandAndParameters << command_FreezeAgent;
            break;
        }
        default:
            break;
        }

        pairAgentNameAndReverseCommand.second = reverseCommandAndParameters;
    }

    return pairAgentNameAndReverseCommand;
}
