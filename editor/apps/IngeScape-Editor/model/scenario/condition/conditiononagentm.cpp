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

#include "conditiononagentm.h"


/**
 * @brief Enum "AgentConditionValues" to string
 * @param value
 * @return
 */
QString AgentConditionValues::enumToString(int value)
{
    switch (value)
    {
    case AgentConditionValues::ON:
        return tr("ON");

    case AgentConditionValues::OFF:
        return tr("OFF");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// ConditionOnAgentM
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
ConditionOnAgentM::ConditionOnAgentM(QObject *parent) : ActionConditionM(parent),
    _agentConditionValue(AgentConditionValues::ON)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
ConditionOnAgentM::~ConditionOnAgentM()
{

}


/**
* @brief Copy from another condition model
* @param condition to copy
*/
void ConditionOnAgentM::copyFrom(ActionConditionM* condition)
{
    // Call our mother class
    ActionConditionM::copyFrom(condition);

    ConditionOnAgentM* conditionOnAgent = qobject_cast<ConditionOnAgentM*>(condition);
    if (conditionOnAgent != nullptr)
    {
        setagentConditionValue(conditionOnAgent->agentConditionValue());
    }
}


/**
 * @brief Setter for property "Agent"
 * @param agent
 */
void ConditionOnAgentM::setagent(AgentsGroupedByNameVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentsGroupedByNameVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionConditionM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (_agent != nullptr) {
            _onAgentIsOnChanged(_agent->isON());
        }
    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void ConditionOnAgentM::initializeConnections()
{
    // Call our mother class
    ActionConditionM::initializeConnections();
}


/**
  * @brief Reset the agent connections for the action condition
  */
void ConditionOnAgentM::resetConnections()
{
    // Call our mother class
    ActionConditionM::resetConnections();
}


/**
 * @brief Slot called when the flag "is ON" of the agent changed
 * @param isON
 */
void ConditionOnAgentM::_onAgentIsOnChanged(bool isON)
{
    if ( ((_agentConditionValue == AgentConditionValues::ON) && isON)
         ||
         ((_agentConditionValue == AgentConditionValues::OFF) && !isON) )
    {
        // Update the flag "is Valid"
        setisValid(true);
    }
    else
    {
        // Update the flag "is Valid"
        setisValid(false);
    }
}


