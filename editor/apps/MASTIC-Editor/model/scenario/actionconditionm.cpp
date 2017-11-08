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


/**
 * @brief Comparison type for an action
 * @param value
 * @return
 */
QString ComparisonType::enumToString(int value)
{
    QString string = "Comparison type";

    switch (value) {
    case ComparisonType::SUPERIOR_TO:
        string = ">";
        break;

    case ComparisonType::INFERIOR_TO:
        string = "<";
        break;

    case ComparisonType::EQUAL_TO:
        string = "=";
        break;

    case ComparisonType::ON:
        string = "On";
        break;

    case ComparisonType::OFF:
        string = "Off";
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
    _comparison(ComparisonType::EQUAL_TO)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

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


