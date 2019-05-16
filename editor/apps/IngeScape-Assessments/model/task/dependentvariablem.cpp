/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "dependentvariablem.h"

/**
 * @brief Constructor
 * @param parent
 */
DependentVariableM::DependentVariableM(QObject *parent) : QObject(parent),
    _name(""),
    _description(""),
    _agentName(""),
    _outputName("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Dependent Variable" << _name;
}


/**
 * @brief Destructor
 */
DependentVariableM::~DependentVariableM()
{
    qInfo() << "Delete Model of Dependent Variable" << _name;

}
