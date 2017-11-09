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
    _comparison(ActionComparisonValueType::ON)
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
* @brief Custom setter on the agent model
* @param agent
*/
void ActionConditionM::setagentModel(AgentM* agentM)
{
    if(_agentModel != agentM)
    {
        _agentModel = agentM;

        IOPValueConditionM* iopCondition = dynamic_cast<IOPValueConditionM*>(this);
        if(iopCondition != NULL)
        {
            if(_agentModel == NULL)
            {
                iopCondition->agentIopList()->clear();
            } else if(_agentModel->definition() != NULL ){
                iopCondition->agentIopList()->append(_agentModel->definition()->inputsList());
                iopCondition->agentIopList()->append(_agentModel->definition()->outputsList());
                iopCondition->agentIopList()->append(_agentModel->definition()->parametersList());
            }

        }

        emit agentModelChanged(agentM);
    }
}


