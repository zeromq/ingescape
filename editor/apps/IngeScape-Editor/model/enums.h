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


/**
  * Types of log
  */
I2_ENUM(LogTypes, TRACE, DEBUG, INFO, WARNING, ERROR, FATAL)


static const QString SEPARATOR_AGENT_NAME_AND_IOP = QString("##");
static const QString SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE = QString("::");


static const QString command_LoadDefinition = "LOAD_THIS_DEFINITION#";
static const QString command_LoadMapping = "LOAD_THIS_MAPPING#";
static const QString command_ClearMapping = "CLEAR_MAPPING";

static const QString command_StartAgent = "RUN";
static const QString command_StopAgent = "STOP";
static const QString command_MuteAgent = "MUTE_ALL";
static const QString command_UnmuteAgent = "UNMUTE_ALL";
static const QString command_MuteAgentOutput = "MUTE";
static const QString command_UnmuteAgentOutput = "UNMUTE";
static const QString command_FreezeAgent = "FREEZE";
static const QString command_UnfreezeAgent = "UNFREEZE";

static const QString command_EnableLogStream = "ENABLE_LOG_STREAM";
static const QString command_DisableLogStream = "DISABLE_LOG_STREAM";
static const QString command_EnableLogFile = "ENABLE_LOG_FILE";
static const QString command_DisableLogFile = "DISABLE_LOG_FILE";
static const QString command_SetLogPath = "SET_LOG_PATH";
static const QString command_SetDefinitionPath = "SET_DEFINITION_PATH";
static const QString command_SetMappingPath = "SET_MAPPING_PATH";
static const QString command_SaveDefinitionToPath = "SAVE_DEFINITION_TO_PATH";
static const QString command_SaveMappingToPath = "SAVE_MAPPING_TO_PATH";


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
