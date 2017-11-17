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
    _agentIOPValueType(AgentIOPValueTypes::UNKNOWN),
    _agentIOPValueTypeGroup(AgentIOPValueTypeGroups::UNKNOWN),
    _defaultValue(QVariant()),
    _displayableDefaultValue(""),
    _currentValue(QVariant()),
    _displayableCurrentValue("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Call the setter to update the corresponding group
    setagentIOPValueType(agentIOPValueType);

    // Create the unique identifier with name and value type
    _id = QString("%1%2%3").arg(_name, SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE, AgentIOPValueTypes::staticEnumToString(_agentIOPValueType));
}


/**
 * @brief Destructor
 */
AgentIOPM::~AgentIOPM()
{

}


void AgentIOPM::setagentIOPValueType(AgentIOPValueTypes::Value value)
{
    if (_agentIOPValueType != value)
    {
        _agentIOPValueType = value;

        // Get the corresponding group
        AgentIOPValueTypeGroups::Value iopValueTypeGroup = Enums::getGroupForAgentIOPValueType(_agentIOPValueType);

        // Update it
        setagentIOPValueTypeGroup(iopValueTypeGroup);

        Q_EMIT agentIOPValueTypeChanged(value);
    }
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

        // Get a displayable value: convert a variant into a string (in function of the value type)
        setdisplayableDefaultValue(Enums::getDisplayableValue(_agentIOPValueType, _defaultValue));

        // Set the current value
        setcurrentValue(_defaultValue);

        Q_EMIT defaultValueChanged(value);
    }
}


/**
 * @brief Setter for property "Current Value"
 * @param value
 */
void AgentIOPM::setcurrentValue(QVariant value)
{
    if (_currentValue != value) {
        _currentValue = value;

        // Get a displayable value: convert a variant into a string (in function of the value type)
        setdisplayableCurrentValue(Enums::getDisplayableValue(_agentIOPValueType, _currentValue));
    }

    // Emit the signal even if the value has not changed to show the animation
    Q_EMIT currentValueChanged(value);
}
