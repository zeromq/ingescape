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


