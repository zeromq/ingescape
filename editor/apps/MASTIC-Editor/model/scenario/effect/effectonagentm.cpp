/*
 *	MASTIC Editor
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
    // Call mother class
    ActionEffectM::copyFrom(effect);

    EffectOnAgentM* effectOnAgent = qobject_cast<EffectOnAgentM*>(effect);
    if (effectOnAgent != NULL)
    {
        setagentEffectValue(effectOnAgent->agentEffectValue());
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentInMappingVM*, QStringList> EffectOnAgentM::getAgentAndCommandWithParameters()
{
    QPair<AgentInMappingVM*, QStringList> pairAgentAndCommandWithParameters;

    if (_agent != NULL)
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        switch (_agentEffectValue)
        {
        case AgentEffectValues::ON: {
            commandAndParameters << "RUN";
            break;
        }
        case AgentEffectValues::OFF: {
            commandAndParameters << "DIE";
            break;
        }
        default:
            break;
        }

        pairAgentAndCommandWithParameters.second = commandAndParameters;
    }

    return pairAgentAndCommandWithParameters;
}
