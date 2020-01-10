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

#include "actionassessmentm.h"

#include "controller/assessmentsmodelmanager.h"

/**
 * @brief Event table name
 */
const QString ActionAssessmentM::table = "ingescape.action";


/**
 * @brief Event table column names
 */
const QStringList ActionAssessmentM::columnNames = {
    "id_record",
    "time",
    "id_action",
    "line_tl"
};


/**
 * @brief Event table primary keys IN ORDER
 */
const QStringList ActionAssessmentM::primaryKeys = {
    "id_record",
    "time"
};


/**
 * @brief Constructor
 */
ActionAssessmentM::ActionAssessmentM(CassUuid recordCassUuid,
                           CassUuid timeUuid,
                           int actionId,
                           int timelineLine,
                           QObject *parent) : QObject(parent),
    _actionId(actionId),
    _timelineLine(timelineLine),
    _recordCassUuid(recordCassUuid),
    _timeUuid(timeUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Action with action id "  << _actionId << " and line " << _timelineLine << " in timeline";
}


/**
 * @brief Destructor
 */
ActionAssessmentM::~ActionAssessmentM() {
    qInfo() << "Delete Model of Action";
}


/**
 * @brief Static factory method to create an event from a CassandraDB record
 * @param row
 * @return
 */
ActionAssessmentM* ActionAssessmentM::createFromCassandraRow(const CassRow* row) {
    ActionAssessmentM* action = nullptr;

    if (row != nullptr)
    {
        CassUuid recordCassUuid, timeUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_record"), &recordCassUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "time"), &timeUuid);

        int actionId =  AssessmentsModelManager::getInt32ValueFromColumnName(row, "id_action");
        int timelineLine =  AssessmentsModelManager::getInt32ValueFromColumnName(row, "line_tl");

        action = new ActionAssessmentM(recordCassUuid, timeUuid, actionId, timelineLine);
    }

    return action;
}
