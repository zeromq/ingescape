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

#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>


/**
  * Types:
  * - Agent Input
  * - Agent Output
  * - Agent Parameter
  */
I2_ENUM(AgentIOPTypes, INPUT, OUTPUT, PARAMETER)


/**
  * Types of the value of an Agent Input / Output / Parameter
  */
I2_ENUM(AgentIOPValueTypes, INTEGER = 1, DOUBLE, STRING, BOOL, IMPULSION, DATA, MIXED, UNKNOWN)


/**
  * Groups for types of the value of an Agent Input / Output / Parameter
  */
I2_ENUM(AgentIOPValueTypeGroups, NUMBER, STRING, IMPULSION, DATA, MIXED, UNKNOWN)


static const QString SEPARATOR_AGENT_NAME_AND_IOP = QString("##");
static const QString SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE = QString("::");


/**
 * @brief The Enums class is a helper for general enumerations
 */
class Enums : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit Enums(QObject *parent = nullptr);


    /**
     * @brief Get the group for an Agent Input/Output/Parameter Value Type
     * @param valueType
     * @return
     */
    static AgentIOPValueTypeGroups::Value getGroupForAgentIOPValueType(AgentIOPValueTypes::Value valueType);


    /**
     * @brief Get a displayable value: convert a variant into a string (in function of the value type)
     * @param valueType
     * @param value
     * @return
     */
    static QString getDisplayableValue(AgentIOPValueTypes::Value valueType, QVariant value);

    static QString getDisplayableValueFromInteger(int value);

    static QString getDisplayableValueFromDouble(double value);

    static QString getDisplayableValueFromString(QString value);

    static QString getDisplayableValueFromBool(bool value);

    static QString getDisplayableValueFromData(QByteArray value);


    /*static int getIntegerFromValue(QVariant value);

    static double getDoubleFromValue(QVariant value);

    static QString getStringFromValue(QVariant value);

    static bool getBoolFromValue(QVariant value);

    static QByteArray getDataFromValue(QVariant value);*/

};

#endif // ENUMS_H
