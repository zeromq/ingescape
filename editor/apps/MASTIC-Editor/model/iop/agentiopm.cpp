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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "agentiopm.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param parent
 */
AgentIOPM::AgentIOPM(QObject *parent) : AgentIOPM(AgentIOPTypes::PARAMETER,
                                                  "",
                                                  AgentIOPValueTypes::UNKNOWN,
                                                  parent)
{
}


/**
 * @brief Constructor
 * @param agentIOPType
 * @param name
 * @param agentIOPValueType
 * @param parent
 */
AgentIOPM::AgentIOPM(AgentIOPTypes::Value agentIOPType,
                     QString name,
                     AgentIOPValueTypes::Value agentIOPValueType,
                     QObject *parent) : QObject(parent),
    _agentIOPType(agentIOPType),
    _name(name),
    _agentIOPValueType(agentIOPValueType),
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


/**
 * @brief Setter for property "Default Value"
 * @param value
 */
void AgentIOPM::setdefaultValue(QVariant value)
{
    if (_defaultValue != value)
    {
        _defaultValue = value;

        // To String
        setdisplayableDefaultValue(_defaultValue.toString());

        Q_EMIT defaultValueChanged(value);
    }
}
