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

#include "agentiopm.h"

AgentIOPM::AgentIOPM(QObject *parent) : QObject(parent),
    _agentIOPType(AgentIOPTypes::PARAMETER),
    _name(""),
    _agentIOPValueType(AgentIOPValueTypes::STRING),
    _defaultValue(QVariant()),
    _displayableDefaultValue("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
AgentIOPM::~AgentIOPM()
{

}
