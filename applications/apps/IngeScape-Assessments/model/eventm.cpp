/*
 *	IngeScape Assessments
 *
 *  Copyright © 2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu   <roumieu@ingenuity.io>
 */

#include "eventm.h"

#include "controller/assessmentsmodelmanager.h"

/**
 * @brief Event table name
 */
const QString EventM::table = "ingescape.event";


/**
 * @brief Event table column names
 */
const QStringList EventM::columnNames = {
    "id_record",
    "time",
    "agent",
    "output",
    "time_of_day",
    "type",
    "year_month_day"
};


/**
 * @brief Event table primary keys IN ORDER
 */
const QStringList EventM::primaryKeys = {
    "id_record",
    "time"
};


/**
 * @brief Constructor
 */
EventM::EventM(CassUuid recordCassUuid,
               CassUuid timeUuid,
               QString agent,
               QString output,
               QDateTime executionDateTime,
               int type,
               QObject *parent) : QObject(parent),
    _agent(agent),
    _output(output),
    _executionDateTime(executionDateTime),
    _type(type),
    _recordCassUuid(recordCassUuid),
    _timeUuid(timeUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Event at " << _executionDateTime << " of type " << _type;
}


/**
 * @brief Destructor
 */
EventM::~EventM() {
    qInfo() << "Delete Model of Event";
}


/**
 * @brief Static factory method to create an event from a CassandraDB record
 * @param row
 * @return
 */
EventM* EventM::createFromCassandraRow(const CassRow* row) {
    EventM* event = nullptr;

    if (row != nullptr)
    {
        CassUuid recordCassUuid, timeUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_record"), &recordCassUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "time"), &timeUuid);

        QString agent = AssessmentsModelManager::getStringValueFromColumnName(row, "agent");
        QString output = AssessmentsModelManager::getStringValueFromColumnName(row, "output");

        QDateTime executionDateTime(AssessmentsModelManager::getDateTimeFromColumnNames(row, "year_month_day", "time_of_day"));

        int type =  AssessmentsModelManager::getTinyIntValueFromColumnName(row, "type");

        event = new EventM(recordCassUuid, timeUuid, agent, output, executionDateTime, type);
    }

    return event;
}
