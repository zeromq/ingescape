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

#include "independentvariableenumm.h"

/**
 * @brief Constructor
 * @param parent
 */
IndependentVariableEnumM::IndependentVariableEnumM(QObject *parent) : QObject(parent),
    _name(""),
    _values(QStringList())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Enum for Independent Variable" << _name;
}


/**
 * @brief Destructor
 */
IndependentVariableEnumM::~IndependentVariableEnumM()
{
    qInfo() << "Delete Model of Enum for Independent Variable" << _name;

}
