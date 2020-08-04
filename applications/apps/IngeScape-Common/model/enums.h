/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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

#ifdef INGESCAPE_FROM_PRI
#include "ingescape.h"
#else
#include <ingescape/ingescape.h>
#endif


//--------------------------------
//
// Enums
//
//--------------------------------

/**
  * Types of IngeScape elements on the network
  */
I2_ENUM(IngeScapeTypes, UNKNOWN, AGENT, LAUNCHER, RECORDER, EDITOR, ASSESSMENTS, EXPE)


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
I2_ENUM_CUSTOM(LogTypes,
               IGS_APPS_LOG_TRACE = IGS_LOG_TRACE,
               IGS_APPS_LOG_DEBUG = IGS_LOG_DEBUG,
               IGS_APPS_LOG_INFO = IGS_LOG_INFO,
               IGS_APPS_LOG_WARNING = IGS_LOG_WARN,
               IGS_APPS_LOG_ERROR = IGS_LOG_ERROR,
               IGS_APPS_LOG_FATAL = IGS_LOG_FATAL)


//--------------------------------
//
// Commands
//
//--------------------------------

static const QString SEPARATOR_AGENT_NAME_AND_IOP = QString("##");
static const QString SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE = QString("::");
static const QString SEPARATOR_LINK_OUTPUT_AND_LINK_INPUT = QString("-->");
static const QString ACTION_LINK_INPUT_NAME = QString("IN");
static const QString ACTION_LINK_OUTPUT_NAME = QString("OUT");
static const QString HOSTNAME_NOT_DEFINED = QString("HOSTNAME NOT DEFINED");

static const QString PLAY = QString("PLAY");
static const QString PAUSE = QString("PAUSE");
static const QString START = QString("START");
static const QString STOP = QString("STOP");
static const QString RESET = QString("RESET");


static const QString command_LoadDefinition = "LOAD_THIS_DEFINITION#";
static const QString command_LoadMapping = "LOAD_THIS_MAPPING#";
static const QString command_ClearMapping = "CLEAR_MAPPING";

static const QString command_StartPeer = "RUN";
static const QString command_StopPeer = "STOP_PEER";

static const QString command_MuteAgent = "MUTE_ALL";
static const QString command_UnmuteAgent = "UNMUTE_ALL";
static const QString command_MuteAgentOutput = "MUTE";
static const QString command_UnmuteAgentOutput = "UNMUTE";

static const QString command_FreezeAgent = "FREEZE";
static const QString command_UnfreezeAgent = "UNFREEZE";

static const QString command_MapAgents = "MAP";
static const QString command_UnmapAgents = "UNMAP";

static const QString command_ExecutedAction = "EXECUTED_ACTION";

static const QString command_StartRecord = "START_RECORD";
static const QString command_StopRecord = "STOP_RECORD";
static const QString command_DeleteRecord = "DELETE_RECORD";
static const QString command_ExportRecord = "EXPORT_RECORD";
static const QString command_ExportSessions = "EXPORT_SESSIONS";
static const QString command_LoadReplay = "LOAD_REPLAY";
static const QString command_UNloadReplay = "UNLOAD_REPLAY";
static const QString command_StartReplay = "START_REPLAY";
static const QString command_StopReplay = "STOP_REPLAY";
static const QString command_PauseReplay = "PAUSE_REPLAY";
static const QString command_UNpauseReplay = "UNPAUSE_REPLAY";

static const QString prefix_RecordStarted = "RECORD_STARTED";
static const QString prefix_RecordStopped = "RECORD_STOPPED";
static const QString prefix_AddedRecord = "ADDED_RECORD=";
static const QString prefix_DeletedRecord = "DELETED_RECORD=";
static const QString prefix_RecordExported = "RECORD_EXPORTED";

static const QString command_LoadPlatformFile = "LOAD_PLATFORM_FROM_PATH";
static const QString command_UpdateTimeLineState = "UPDATE_TIMELINE_STATE";
static const QString command_UpdateRecordState = "UPDATE_RECORD_STATE";


//--------------------------------
//
// Notifications
//
//--------------------------------

static const QString notif_TimeLineState = "TIMELINE_STATE";



//--------------------------------
//
// I2_QML_PROPERTY_QTime
//
//--------------------------------

// Date date of our application
static const QDate APPLICATION_START_DATE = QDate::currentDate();


/*!
 * Define a QTime and a QDateTime: Manage a date in addition to the time
 * to prevent a delta in hours between JS (QML) and C++.
 * The QDateTime is readable/writable from QML.
 * The date used is the current date.
 */
#define I2_QML_PROPERTY_QTime(name) \
Q_PROPERTY (QDateTime name READ qmlGet##name WRITE qmlSet##name NOTIFY name##Changed) \
public: \
    QDateTime qmlGet##name () const { \
        return QDateTime(_##name##_Date, _##name); \
    } \
    QTime name () const { \
        return _##name; \
    } \
    virtual bool qmlSet##name (QDateTime value) { \
        bool hasChanged = false; \
        if (_##name != value.time()) { \
            _##name = value.time(); \
            _##name##_Date = value.date(); \
            hasChanged = true; \
            Q_EMIT name##Changed(value); \
        } \
        return hasChanged; \
    } \
    void set##name(QTime value) { \
        if (_##name != value) { \
            _##name = value; \
            Q_EMIT name##Changed(QDateTime(_##name##_Date, _##name)); \
        } \
    } \
Q_SIGNALS: \
    void name##Changed (QDateTime value); \
protected: \
    QTime _##name; \
    QDate _##name##_Date = APPLICATION_START_DATE;



//--------------------------------
//
// Class "Enums"
//
//--------------------------------


/**
 * @brief The Enums class is a helper for general enumerations and constants
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

QML_DECLARE_TYPE(Enums)

#endif // ENUMS_H
