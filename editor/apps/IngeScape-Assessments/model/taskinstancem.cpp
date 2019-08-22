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

#include "taskinstancem.h"

#include "controller/assessmentsmodelmanager.h"

// TaskInstance table name
const QString TaskInstanceM::table = "ingescape.task_instance";

/**
 * @brief Constructor
 * @param name
 * @param subject
 * @param task
 * @param startDateTime
 * @param parent
 */
TaskInstanceM::TaskInstanceM(CassUuid experimentationUuid,
                             CassUuid cassUuid,
                             QString name,
                             QString comments,
                             SubjectM* subject,
                             TaskM* task,
                             QDateTime startDateTime,
                             QObject *parent) : QObject(parent),
    _name(name),
    _comments(comments),
    _subject(subject),
    _task(task),
    _startDateTime(startDateTime),
    _endDateTime(QDateTime()),
    _duration(QTime()),
    _mapIndependentVariableValues(nullptr),
    _experimentationCassUuid(experimentationUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if ((subject != nullptr) && (task != nullptr))
    {
        qInfo() << "New Model of Record" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ") for subject" << _subject->displayedId() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

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
        connect(_mapIndependentVariableValues, &QQmlPropertyMap::valueChanged, this, &TaskInstanceM::_onIndependentVariableValueChanged);
    }
}


/**
 * @brief Destructor
 */
TaskInstanceM::~TaskInstanceM()
{
    if ((_subject != nullptr) && (_task != nullptr))
    {
        qInfo() << "Delete Model of Record" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ") for subject" << _subject->displayedId() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

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
 * @brief Custome _name setter that updates the DB entry with the new name
 * @param value
 */
void TaskInstanceM::setname(QString value)
{
    if (value != _name)
    {
        // Assign value
        _name = value;

        // Update DB entry
        _updateDBEntry();

        // Notify change
        Q_EMIT nameChanged(value);
    }
}

/**
 * @brief Custome _comments setter that updates the DB entry with the new user comments
 * @param value
 */
void TaskInstanceM::setcomments(QString value)
{
    if (value != _comments)
    {
        // Assign value
        _comments = value;

        // Update DB entry
        _updateDBEntry();

        // Notify change
        Q_EMIT commentsChanged(value);
    }
}

/**
 * @brief Set the value of the given independent variable into the QQmlPropertyMap
 * @param indeVar
 * @param value
 */
void TaskInstanceM::setIndependentVariableValue(IndependentVariableM* indeVar, const QString& value)
{
    if (indeVar != nullptr)
    {
        _mapIndependentVariableValues->insert(indeVar->name(), value);

        // Call SLOT manually since valueChanged() signal is only emitted from QML
        _onIndependentVariableValueChanged(indeVar->name(), value);
    }
}


/**
 * @brief Static factory method to create a task instance from a CassandraDB record
 * @param row
 * @return
 */
TaskInstanceM* TaskInstanceM::createTaskInstanceFromCassandraRow(const CassRow* row, SubjectM* subject, TaskM* task)
{
    TaskInstanceM* taskInstance = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, subjectUuid, taskUuid, taskInstanceUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_subject"), &subjectUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &taskInstanceUuid);

        QString taskName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString comments(AssessmentsModelManager::getStringValueFromColumnName(row, "comment"));
        QUrl platformUrl(AssessmentsModelManager::getStringValueFromColumnName(row, "platform_file"));

        QDateTime startDateTime(AssessmentsModelManager::getDateTimeFromColumnNames(row, "start_date", "start_time"));

        taskInstance = new TaskInstanceM(experimentationUuid, taskInstanceUuid, taskName, comments, subject, task, startDateTime);
    }

    return taskInstance;
}

/**
 * @brief Delete the given task instance from Cassandra DB
 * @param experimentation
 */
void TaskInstanceM::deleteTaskInstanceFromCassandra(const TaskInstanceM& taskInstance)
{
    if ((taskInstance.subject() != nullptr) && (taskInstance.task() != nullptr))
    {
        QString queryStr = "DELETE FROM " + IndependentVariableValueM::table + " WHERE id_experimentation = ? AND id_task_instance = ?;";
        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 2);
        cass_statement_bind_uuid(cassStatement, 0, taskInstance.subject()->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, taskInstance.getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "IndependentVar values for task instance" << taskInstance.name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the independentVar values for task instance" << taskInstance.name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

        queryStr = "DELETE FROM " + TaskInstanceM::table + " WHERE id_experimentation = ? AND id_subject = ? AND id_task = ? AND id = ?;";
        cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 4);
        cass_statement_bind_uuid(cassStatement, 0, taskInstance.subject()->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, taskInstance.subject()->getCassUuid());
        cass_statement_bind_uuid(cassStatement, 2, taskInstance.task()->getCassUuid());
        cass_statement_bind_uuid(cassStatement, 3, taskInstance.getCassUuid());

        // Execute the query or bound statement
        cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Task instance" << taskInstance.name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the task instance" << taskInstance.name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}


void TaskInstanceM::_onIndependentVariableValueChanged(const QString& key, const QVariant& value)
{
    IndependentVariableM* indeVar = _mapIndependentVarByName.value(key, nullptr);
    if (indeVar != nullptr)
    {
        QString queryStr = "UPDATE " + IndependentVariableValueM::table + " SET independent_var_value = ? WHERE id_experimentation = ? AND id_task_instance = ? AND id_independent_var = ?;";
        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 4);
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
void TaskInstanceM::setendDateTime(QDateTime value)
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
void TaskInstanceM::_printIndependentVariableValues()
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

/**
 * @brief Update the DB entry corresponding
 */
void TaskInstanceM::_updateDBEntry()
{
    if ((_task != nullptr) && (_subject != nullptr))
    {
        time_t startDateTime(_startDateTime.toTime_t());
        cass_uint32_t startYearMonthDay = cass_date_from_epoch(startDateTime);
        cass_int64_t startTimeOfDay = cass_time_from_epoch(startDateTime);
        time_t endDateTime(_startDateTime.toTime_t());
        cass_uint32_t endYearMonthDay = cass_date_from_epoch(endDateTime);
        cass_int64_t endTimeOfDay = cass_time_from_epoch(endDateTime);

        QString queryStr("UPDATE " + TaskInstanceM::table
                         + " SET name = ?, comment = ?, start_date = ?, start_time = ?, end_date = ?, end_time = ?"
                         + " WHERE id_experimentation = ? AND id_subject = ? AND id_task = ? AND id = ?;");

        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 10);
        cass_statement_bind_string(cassStatement, 0, _name.toStdString().c_str());
        cass_statement_bind_string(cassStatement, 1, _comments.toStdString().c_str());
        cass_statement_bind_uint32(cassStatement, 2, startYearMonthDay);
        cass_statement_bind_int64 (cassStatement, 3, startTimeOfDay);
        cass_statement_bind_uint32(cassStatement, 4, endYearMonthDay);
        cass_statement_bind_int64 (cassStatement, 5, endTimeOfDay);
        cass_statement_bind_uuid  (cassStatement, 6, _subject->getExperimentationCassUuid());
        cass_statement_bind_uuid  (cassStatement, 7, _subject->getCassUuid());
        cass_statement_bind_uuid  (cassStatement, 8, _task->getCassUuid());
        cass_statement_bind_uuid  (cassStatement, 9, getCassUuid());

        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError != CASS_OK)
        {
            qCritical() << "Unable to update the TaskInstance" << _name << cass_error_desc(cassError);
        }

    }
}
