/*
 *	MASTIC Editor
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
 * @brief Comparison type for an action
 * @param value
 * @return
 */
QString ActionComparisonValueType::enumToString(int value)
{
    QString string = "Comparison type";

    switch (value) {
    case ActionComparisonValueType::SUPERIOR_TO:
        string = ">";
        break;

    case ActionComparisonValueType::INFERIOR_TO:
        string = "<";
        break;

    case ActionComparisonValueType::EQUAL_TO:
        string = "=";
        break;

    case ActionComparisonValueType::ON:
        string = "ON";
        break;

    case ActionComparisonValueType::OFF:
        string = "OFF";
        break;

    default:
        break;
    }

    return string;
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
    _comparison(ActionComparisonValueType::ON),
    _isValid(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set the condition comparision type
    setcomparison(ActionComparisonValueType::ON);
}


/**
 * @brief Destructor
 */
ActionConditionM::~ActionConditionM()
{
    // Disconnect the agent model
    if(_agent != NULL)
    {
        disconnect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);
    }
}

/**
* @brief Custom setter for agent
* @param agent
*/
void ActionConditionM::setagent(AgentInMappingVM* value)
{
    if(_agent != value)
    {
        if(_agent != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agent, &AgentInMappingVM::destroyed, this, &ActionConditionM::_onAgentDestroyed);
        }
        setisValid(false);

        _agent = value;

        if(_agent != NULL)
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
    if(condition != NULL)
    {
        setagent(condition->agent());
        setcomparison(condition->comparison());
        setisValid(condition->isValid());
    }
}


/**
  * @brief Slot on IsON flag agent change
  */
void ActionConditionM::onAgentModelIsOnChange(bool isON)
{
    if ((_comparison == ActionComparisonValueType::ON && isON)
            ||
            (_comparison == ActionComparisonValueType::OFF && !isON)) {
        setisValid(true);
    }
    else {
        setisValid(false);
    }
}

/**
  * @brief Initialize the agent connections for the action condition
  */
void ActionConditionM::initializeConnections()
{
    if(_agent != NULL)
    {
        // Reset the connections
        resetConnections();

        // Make connection for the futur changes
        connect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);

        // Initialize the action state with the current agent state
        onAgentModelIsOnChange(_agent->isON());
    }
}

/**
  * @brief Reset the agent connections for the action condition
  */
void ActionConditionM::resetConnections()
{
    if(_agent != NULL)
    {
        disconnect(_agent, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);
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
