/*
 *	IngeScape Editor
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
    _uid(""),
    _defaultValue(QVariant()),
    _displayableDefaultValue(""),
    _currentValue(QVariant()),
    _displayableCurrentValue("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Call the setter to update the corresponding group
    setagentIOPValueType(agentIOPValueType);

    // Create the unique identifier with name and value type: "Name::ValueType"
    _uid = QString("%1%2%3").arg(_name, SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE, AgentIOPValueTypes::staticEnumToString(_agentIOPValueType));
}


/**
 * @brief Destructor
 */
AgentIOPM::~AgentIOPM()
{

}


/**
 * @brief Setter for property "Agent IOP Value Type"
 * @param value
 */
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

        Q_EMIT defaultValueChanged(value);

        // Get a displayable value: convert a variant into a string (in function of the value type)
        //setdisplayableDefaultValue(Enums::getDisplayableValue(_agentIOPValueType, _defaultValue));

        setdisplayableDefaultValue(_defaultValue.toString());

        // Set the current value
        setcurrentValue(_defaultValue);
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
        //setdisplayableCurrentValue(Enums::getDisplayableValue(_agentIOPValueType, _currentValue));

        setdisplayableCurrentValue(_currentValue.toString());
    }

    // Emit the signal even if the value has not changed to show the animation
    Q_EMIT currentValueChanged(value);
}


/**
 * @brief Get the name and the value type of an agent I/O/P from its id
 * @param uid
 * @return
 */
QPair<QString, AgentIOPValueTypes::Value> AgentIOPM::getNameAndValueTypeFromId(QString uid)
{
    QString name = "";
    AgentIOPValueTypes::Value valueType = AgentIOPValueTypes::UNKNOWN;

    if (uid.contains(SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE))
    {
        QStringList nameAndValueType = uid.split(SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE);
        if (nameAndValueType.count() == 2)
        {
            name = nameAndValueType.at(0);

            int nValueType = AgentIOPValueTypes::staticEnumFromKey(nameAndValueType.at(1));
            if (nValueType > -1)
            {
                valueType = static_cast<AgentIOPValueTypes::Value>(nValueType);
            }
        }
    }
    return QPair<QString, AgentIOPValueTypes::Value>(name, valueType);
}
