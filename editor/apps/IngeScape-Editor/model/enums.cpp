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
 *
 */

#include "enums.h"

/**
 * @brief Enum "LogTypes" to string
 * @param value
 * @return
 */
QString LogTypes::enumToString(int value)
{
    switch (value)
    {
    case LogTypes::IGS_LOG_TRACE:
        return tr("TRACE");

    case LogTypes::IGS_LOG_DEBUG:
        return tr("DEBUG");

    case LogTypes::IGS_LOG_INFO:
        return tr("INFO");

    case LogTypes::IGS_LOG_WARNING:
        return tr("WARNING");

    case LogTypes::IGS_LOG_ERROR:
        return tr("ERROR");

    case LogTypes::IGS_LOG_FATAL:
        return tr("FATAL");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
//  Enums
//
//--------------------------------------------------------------


/**
 * @brief Constructor
 * @param parent
 */
Enums::Enums(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Get the group for an Agent Input/Output/Parameter Value Type
 * @param valueType
 * @return
 */
AgentIOPValueTypeGroups::Value Enums::getGroupForAgentIOPValueType(AgentIOPValueTypes::Value valueType)
{
    AgentIOPValueTypeGroups::Value agentIOPValueTypeGroup = AgentIOPValueTypeGroups::UNKNOWN;

    switch (valueType)
    {
    case AgentIOPValueTypes::INTEGER:
    case AgentIOPValueTypes::DOUBLE:
    case AgentIOPValueTypes::BOOL:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::NUMBER;
        break;

    case AgentIOPValueTypes::STRING:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::STRING;
        break;

    case AgentIOPValueTypes::IMPULSION:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::IMPULSION;
        break;

    case AgentIOPValueTypes::DATA:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::DATA;
        break;

    case AgentIOPValueTypes::MIXED:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::MIXED;
        break;

    case AgentIOPValueTypes::UNKNOWN:
        agentIOPValueTypeGroup = AgentIOPValueTypeGroups::UNKNOWN;
        break;

    default:
        break;
    }

    return agentIOPValueTypeGroup;
}


/**
 * @brief Get a displayable value: convert a variant into a string (in function of the value type)
 * @param valueType
 * @param value
 * @return
 */
QString Enums::getDisplayableValue(AgentIOPValueTypes::Value valueType, QVariant value)
{
    Q_UNUSED(valueType)

    // To String
    return value.toString();

    /*QString displayableValue = "";

    switch (valueType)
    {
    case AgentIOPValueTypes::INTEGER:
        break;

    case AgentIOPValueTypes::DOUBLE:
        break;

    case AgentIOPValueTypes::STRING:
        break;

    case AgentIOPValueTypes::BOOL:
        break;

    case AgentIOPValueTypes::IMPULSION:
        break;

    case AgentIOPValueTypes::DATA:
        break;

    default:
        break;
    }

    return displayableValue;*/
}

QString Enums::getDisplayableValueFromInteger(int value)
{
    return QString::number(value);
}

QString Enums::getDisplayableValueFromDouble(double value)
{
    return QString::number(value);
}

QString Enums::getDisplayableValueFromString(QString value)
{
    return value;
}

QString Enums::getDisplayableValueFromBool(bool value)
{
    if (value) {
        return "true";
    }
    else {
        return "false";
    }
}

QString Enums::getDisplayableValueFromData(QByteArray value)
{
    return QString::fromLocal8Bit(value);
    //return QString::fromUtf8(value);
}


/*int Enums::getIntegerFromValue(QVariant value)
{

}

double Enums::getDoubleFromValue(QVariant value)
{

}

QString Enums::getStringFromValue(QVariant value)
{

}

bool Enums::getBoolFromValue(QVariant value)
{

}

QByteArray Enums::getDataFromValue(QVariant value)
{

}*/

