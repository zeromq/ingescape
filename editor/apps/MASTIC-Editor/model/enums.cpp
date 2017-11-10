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

#include "enums.h"

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
