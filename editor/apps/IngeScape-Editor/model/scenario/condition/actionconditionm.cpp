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

#include <QDebug>


/**
 * @brief Enum "ActionComparisonTypes" to string
 * @param value
 * @return
 */
QString ActionComparisonTypes::enumToString(int value)
{
    switch (value)
    {
    case ActionComparisonTypes::SUPERIOR_TO:
        return tr(">");

    case ActionComparisonTypes::INFERIOR_TO:
        return tr("<");

    case ActionComparisonTypes::EQUAL_TO:
        return tr("=");

    case ActionComparisonTypes::ON:
        return tr("ON");

    case ActionComparisonTypes::OFF:
        return tr("OFF");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// ActionConditionM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionConditionM::ActionConditionM(QObject *parent) : QObject(parent),
    _agent(NULL),
    _comparison(ActionComparisonTypes::ON),
    _isValid(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set the condition comparision type
    setcomparison(ActionComparisonTypes::ON);
}


/**
 * @brief Destructor
 */
ActionConditionM::~ActionConditionM()
{
    // Disconnect the agent model
    if (_agent != NULL)
    {
        disconnect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::_onAgentModelIsOnChanged);
    }
}


/**
* @brief Custom setter for agent
* @param agent
*/
void ActionConditionM::setagent(AgentInMappingVM* value)
{
    if (_agent != value)
    {
        if (_agent != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agent, &AgentInMappingVM::destroyed, this, &ActionConditionM::_onAgentDestroyed);
        }
        setisValid(false);

        _agent = value;

        if (_agent != NULL)
        {
            // Subscribe to destruction
            connect(_agent, &AgentInMappingVM::destroyed, this, &ActionConditionM::_onAgentDestroyed);
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
    if (condition != NULL)
    {
        setagent(condition->agent());
        setcomparison(condition->comparison());
        setisValid(condition->isValid());
    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void ActionConditionM::initializeConnections()
{
    if (_agent != NULL)
    {
        // Reset the connections
        resetConnections();

        // Make connection for the futur changes
        connect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::_onAgentModelIsOnChanged);

        // Initialize the action state with the current agent state
        _onAgentModelIsOnChanged(_agent->isON());
    }
}


/**
  * @brief Reset the agent connections for the action condition
  */
void ActionConditionM::resetConnections()
{
    if (_agent != NULL)
    {
        disconnect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::_onAgentModelIsOnChanged);
    }
}


/**
  * @brief Slot called when the flag "is ON" of an agent changed
  */
void ActionConditionM::_onAgentModelIsOnChanged(bool isON)
{
    if ( ((_comparison == ActionComparisonTypes::ON) && isON)
         ||
         ((_comparison == ActionComparisonTypes::OFF) && !isON) )
    {
        setisValid(true);
    }
    else {
        setisValid(false);
    }
}


/**
 * @brief Called when our agent model is destroyed
 * @param sender
 */
void ActionConditionM::_onAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagent(NULL);

    Q_EMIT askForDestruction();
}
