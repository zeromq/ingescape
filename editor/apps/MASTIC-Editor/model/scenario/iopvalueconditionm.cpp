/*
 *	IOPValueConditionM
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

#include "iopvalueconditionm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// IOPValueConditionM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
IOPValueConditionM::IOPValueConditionM(QObject *parent) : ActionConditionM(parent),
    _agentIOP(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
IOPValueConditionM::~IOPValueConditionM()
{
    // reset agent IOP pointer
    setagentIOP(NULL);
}


