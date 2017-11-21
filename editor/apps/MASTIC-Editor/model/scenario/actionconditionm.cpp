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

    // Reset agent model to null
    setagentModel(NULL);
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
    }
}

/**
  * @brief Initialize the action condition. Make connections.
  */
void ActionConditionM::initialize()
{
    if(_agentModel != NULL)
    {
        connect(_agentModel, &AgentInMappingVM::isONChanged, this, &ActionConditionM::onAgentModelIsOnChange);
    }
}

/**
  * @brief Slot on IsON flag agent change
  */
void ActionConditionM::onAgentModelIsOnChange(bool isON)
{
    if((_comparison == ActionComparisonValueType::ON && isON)
                ||
       (_comparison == ActionComparisonValueType::OFF && isON))
    {
        setisValid(true);
    } else
    {
        setisValid(false);
    }
}

