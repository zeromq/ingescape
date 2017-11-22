/*
 *	ActionConditionM
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

#include "actionconditionm.h"


#include <QDebug>
#include "iopvalueconditionm.h"

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
    _agentModel(NULL),
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
    if(_agentModel != NULL)
    {
        disconnect(_agentModel, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);
    }
}

/**
* @brief Custom setter for agent model
* @param agent model
*/
void ActionConditionM::setagentModel(AgentInMappingVM* agentModel)
{
    if(_agentModel != agentModel)
    {
        if(_agentModel != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agentModel, &AgentInMappingVM::destroyed, this, &ActionConditionM::_onAgentModelDestroyed);
        }
        setisValid(false);

        _agentModel = agentModel;

        if(_agentModel != NULL)
        {
            // Subscribe to destruction
            connect(_agentModel, &AgentInMappingVM::destroyed, this, &ActionConditionM::_onAgentModelDestroyed);
        }

        Q_EMIT agentModelChanged(agentModel);
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
        setagentModel(condition->agentModel());
        setcomparison(condition->comparison());
        setisValid(condition->isValid());
    }
}


/**
  * @brief Slot on IsON flag agent change
  */
void ActionConditionM::onAgentModelIsOnChange(bool isON)
{
    qDebug() << "onAgentModelIsOnChange : " << isON;
    if((_comparison == ActionComparisonValueType::ON && isON)
                ||
       (_comparison == ActionComparisonValueType::OFF && isON == false))
    {
        setisValid(true);
    } else
    {
        setisValid(false);
    }
}

/**
  * @brief Initialize the agent connections for the action condition
  */
void ActionConditionM::initializeConnections()
{
    if(_agentModel != NULL)
    {
        // Reset the connections
        resetConnections();

        // Make connection for the futur changes
        connect(_agentModel, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);

        // Initialize the action state with the current agent state
        onAgentModelIsOnChange(_agentModel->isON());
    }
}

/**
  * @brief Reset the agent connections for the action condition
  */
void ActionConditionM::resetConnections()
{
    if(_agentModel != NULL)
    {
        disconnect(_agentModel, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);
    }
}

/**
 * @brief Called when our agent model is destroyed
 * @param sender
 */
void ActionConditionM::_onAgentModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentModel(NULL);
}
