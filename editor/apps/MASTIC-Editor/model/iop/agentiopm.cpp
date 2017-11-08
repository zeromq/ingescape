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
    _agentIOPValueType(AgentIOPValueTypes::UNKNOWN),
    _agentIOPValueTypeGroup(AgentIOPValueTypeGroups::UNKNOWN),
    _defaultValue(QVariant()),
    _displayableDefaultValue("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Call the setter to update the corresponding group
    setagentIOPValueType(agentIOPValueType);

    // Create the identifier with name and value type
    _id = QString("%1::%2").arg(_name, AgentIOPValueTypes::staticEnumToString(_agentIOPValueType));
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

        // Update the corresponding group
        switch (_agentIOPValueType)
        {
        case AgentIOPValueTypes::INTEGER:
        case AgentIOPValueTypes::DOUBLE:
        case AgentIOPValueTypes::BOOL:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::NUMBER);
            break;

        case AgentIOPValueTypes::STRING:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::STRING);
            break;

        case AgentIOPValueTypes::IMPULSION:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::IMPULSION);
            break;

        case AgentIOPValueTypes::DATA:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::DATA);
            break;

        case AgentIOPValueTypes::MIXED:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::MIXED);
            break;

        case AgentIOPValueTypes::UNKNOWN:
            setagentIOPValueTypeGroup(AgentIOPValueTypeGroups::UNKNOWN);
            break;

        default:
            break;
        }

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

        // To String
        setdisplayableDefaultValue(_defaultValue.toString());

        Q_EMIT defaultValueChanged(value);
    }
}
