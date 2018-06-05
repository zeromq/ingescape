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
 * @brief Constructor
 * @param parent
 */
ConditionOnAgentM::ConditionOnAgentM(QObject *parent) : ActionConditionM(parent)
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
    if (conditionOnAgent != NULL)
    {
        // TODO
    }
}


/**
 * @brief Setter for property "Agent"
 * @param agent
 */
void ConditionOnAgentM::setagent(AgentInMappingVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentInMappingVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionConditionM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {

    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void ConditionOnAgentM::initializeConnections()
{
    if (_agent != NULL)
    {
        // Call our mother class
        ActionConditionM::initializeConnections();

    }
}


/**
  * @brief Reset the agent connections for the action condition
  */
void ConditionOnAgentM::resetConnections()
{
    if (_agent != NULL)
    {
        // Call our mother class
        ActionConditionM::resetConnections();

    }
}


/**
  * @brief Slot called when the flag "is ON" of an agent changed
  */
void ConditionOnAgentM::_onAgentModelIsOnChanged(bool isON)
{
    Q_UNUSED(isON)
}


