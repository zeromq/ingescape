/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "recordsetupm.h"

#include "controller/assessmentsmodelmanager.h"

// RecordSetup table name
const QString RecordSetupM::table = "ingescape.task_instance";

/**
 * @brief Constructor
 * @param name
 * @param subject
 * @param task
 * @param startDateTime
 * @param parent
 */
RecordSetupM::RecordSetupM(CassUuid experimentationUuid,
                           CassUuid cassUuid,
                           QString name,
                           SubjectM* subject,
                           TaskM* task,
                           QDateTime startDateTime,
                           QObject *parent) : QObject(parent),
    _uid(AssessmentsModelManager::cassUuidToQString(cassUuid)),
    _name(name),
    _subject(subject),
    _task(task),
    _startDateTime(startDateTime),
    _endDateTime(QDateTime()),
    //_duration(QDateTime())
    _duration(QTime()),
    _mapIndependentVariableValues(nullptr),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if ((subject != nullptr) && (task != nullptr))
    {
        qInfo() << "New Model of Record" << _name << "(" << _uid << ") for subject" << _subject->displayedId() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

        // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
        _mapIndependentVariableValues = new QQmlPropertyMap(this);

        for (IndependentVariableM* independentVariable : _task->independentVariables()->toList())
        {
            if (independentVariable != nullptr)
            {
                // Insert an (invalid) not initialized QVariant
                _mapIndependentVariableValues->insert(independentVariable->name(), QVariant());
                _mapIndependentVarByName.insert(independentVariable->name(), independentVariable);
            }
        }

        // Connect to signal "Value Changed" fro the "Qml Property Map"
        connect(_mapIndependentVariableValues, &QQmlPropertyMap::valueChanged, this, &RecordSetupM::_onIndependentVariableValueChanged);
    }
}


/**
 * @brief Destructor
 */
RecordSetupM::~RecordSetupM()
{
    if ((_subject != nullptr) && (_task != nullptr))
    {
        qInfo() << "Delete Model of Record" << _name << "(" << _uid << ") for subject" << _subject->displayedId() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

        // For debug purpose: Print the value of all independent variables
        _printIndependentVariableValues();

        // Clean-up independent variable map (by name). No deletion.
        _mapIndependentVarByName.clear();

        // Free memory
        if (_mapIndependentVariableValues != nullptr)
        {
            /*// Clear each value
            for (QString key : _mapIndependentVariableValues->keys())
            {
                _mapIndependentVariableValues->clear(key);
            }*/

            QQmlPropertyMap* temp = _mapIndependentVariableValues;
            setmapIndependentVariableValues(nullptr);
            delete temp;
        }

        // Reset pointers
        setsubject(nullptr);
        settask(nullptr);
    }
}


/**
 * @brief Static factory method to create a record setup from a CassandraDB record
 * @param row
 * @return
 */
//NOTE Same note as CharacteristicM::_deleteCharacteristicValuesForCharacteristic
RecordSetupM* RecordSetupM::createRecordSetupFromCassandraRow(const CassRow* row, SubjectM* subject, TaskM* task)
{
    RecordSetupM* recordSetup = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, subjectUuid, taskUuid, recordUuid, recordSetupUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_subject"), &subjectUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_records"), &recordUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &recordSetupUuid);

        const char *chrTaskName = "";
        size_t nameLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrTaskName, &nameLength);
        QString taskName = QString::fromUtf8(chrTaskName, static_cast<int>(nameLength));

        const char *chrPlatformUrl = "";
        size_t platformUrlLength = 0;
        cass_value_get_string(cass_row_get_column_by_name(row, "platform_file"), &chrPlatformUrl, &platformUrlLength);
        QUrl platformUrl(QString::fromUtf8(chrPlatformUrl, static_cast<int>(platformUrlLength)));

        cass_uint32_t yearMonthDay;
        cass_value_get_uint32(cass_row_get_column_by_name(row, "start_date"), &yearMonthDay);
        cass_int64_t timeOfDay;
        cass_value_get_int64(cass_row_get_column_by_name(row, "start_time"), &timeOfDay);

        /* Convert 'date' and 'time' to Epoch time */
        time_t time = static_cast<time_t>(cass_date_time_to_epoch(yearMonthDay, timeOfDay));

        recordSetup = new RecordSetupM(experimentationUuid, recordSetupUuid, taskName, subject, task, QDateTime::fromTime_t(static_cast<uint>(time)));
    }

    return recordSetup;
}

/**
 * @brief Delete the given record setup from Cassandra DB
 * @param experimentation
 */
void RecordSetupM::deleteRecordSetupFromCassandra(const RecordSetupM& recordSetup)
{
    if ((recordSetup.subject() != nullptr) && (recordSetup.task() != nullptr))
    {
        //TODO Clean-up associations if any ?

        // Delete actual experimentation,

        //FIXME Hard coded record UUID for test purposes
        CassUuid recordUuid;
        cass_uuid_from_string("052c42a0-ad26-11e9-bd79-c9fd40f1d28a", &recordUuid);

        QString queryString = "DELETE FROM " + RecordSetupM::table + " WHERE id_experimentation = ? AND id_subject = ? AND id_task = ? AND id_records = ? AND id = ?;";
        const char* query = queryString.toStdString().c_str();
        CassStatement* cassStatement = cass_statement_new(query, 5);
        cass_statement_bind_uuid(cassStatement, 0, recordSetup.subject()->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, recordSetup.subject()->getCassUuid());
        cass_statement_bind_uuid(cassStatement, 2, recordSetup.task()->getCassUuid());
        cass_statement_bind_uuid(cassStatement, 3, recordUuid);
        cass_statement_bind_uuid(cassStatement, 4, recordSetup.getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Experimentation" << recordSetup.name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the experimentation" << recordSetup.name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}


void RecordSetupM::_onIndependentVariableValueChanged(const QString& key, const QVariant& value)
{
    IndependentVariableM* indeVar = _mapIndependentVarByName.value(key, nullptr);
    if (indeVar != nullptr)
    {
        const char* query = "UPDATE ingescape.independent_var_value_of_record_setup SET independent_var_value = ? WHERE id_experimentation = ? AND id_task_instance = ? AND id_independent_var = ?;";
        CassStatement* cassStatement = cass_statement_new(query, 4);
        cass_statement_bind_string(cassStatement, 0, value.toString().toStdString().c_str());
        cass_statement_bind_uuid  (cassStatement, 1, _experimentationCassUuid);
        cass_statement_bind_uuid  (cassStatement, 2, _cassUuid);
        cass_statement_bind_uuid  (cassStatement, 3, indeVar->getCassUuid());
        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError != CASS_OK)
        {
            qCritical() << "Could not update the value of independent variable" << indeVar->name() << "for record_setup" << name();
        }
    }
    else {
        qCritical() << "Unknown independent variable" << key;
    }
}


/**
 * @brief Setter for property "End Date Time"
 * @param value
 */
void RecordSetupM::setendDateTime(QDateTime value)
{
    if (_endDateTime != value)
    {
        _endDateTime = value;

        // Update the duration
        qint64 milliSeconds = _startDateTime.msecsTo(_endDateTime);
        QTime time = QTime(0, 0, 0, 0).addMSecs(static_cast<int>(milliSeconds));

        //setduration(QDateTime(_startDateTime.date(), time));
        setduration(time);

        Q_EMIT endDateTimeChanged(value);
    }
}


/**
 * @brief For debug purpose: Print the value of all independent variables
 */
void RecordSetupM::_printIndependentVariableValues()
{
    if ((_task != nullptr) && (_mapIndependentVariableValues != nullptr))
    {
        for (IndependentVariableM* independentVariable : _task->independentVariables()->toList())
        {
            if ((independentVariable != nullptr) && _mapIndependentVariableValues->contains(independentVariable->name()))
            {
                QVariant var = _mapIndependentVariableValues->value(independentVariable->name());

                // Check validity
                if (var.isValid()) {
                    qDebug() << "Independent Variable:" << independentVariable->name() << "(" << IndependentVariableValueTypes::staticEnumToString(independentVariable->valueType()) << ") --> value:" << var;
                }
                else {
                    qDebug() << "Independent Variable:" << independentVariable->name() << "(" << IndependentVariableValueTypes::staticEnumToString(independentVariable->valueType()) << ") --> value: UNDEFINED";
                }
            }
        }
    }
}
