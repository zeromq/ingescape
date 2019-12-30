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
#include "model/protocol/independentvariablevaluem.h"

/**
 * @brief TaskInstance table name
 */
const QString TaskInstanceM::table = "ingescape.task_instance";

/**
 * @brief TaskInstance table column names
 */
const QStringList TaskInstanceM::columnNames = {
    "id_experimentation",
    "id_subject",
    "id_task",
    "id",
    "comment",
    "end_date",
    "end_time",
    "name",
    "start_date",
    "start_time",
};

/**
 * @brief TaskInstance table primary keys IN ORDER
 */
const QStringList TaskInstanceM::primaryKeys = {
    "id_experimentation",
    "id_subject",
    "id_task",
    "id",
};

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
                             CassUuid subjectUuid,
                             CassUuid taskUuid,
                             QDateTime startDateTime,
                             QObject *parent) : QObject(parent),
    _name(name),
    _comments(comments),
    _subject(nullptr),
    _task(nullptr),
    _startDateTime(startDateTime),
    _endDateTime(startDateTime),  //FIXME Need a way to compute actual endDateTime.
    _duration(QTime()),           //FIXME Need a way to compute actual duration.
    _mapIndependentVariableValues(nullptr),
    _isRecorded(false),
    _experimentationCassUuid(experimentationUuid),
    _subjectUuid(subjectUuid),
    _taskUuid(taskUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Session" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";

    // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
    _mapIndependentVariableValues = new QQmlPropertyMap(this);

    _recordsList.setSortProperty("startTimeInTimeline");

    // Connect to signal "Value Changed" from the "Qml Property Map"
    connect(_mapIndependentVariableValues, &QQmlPropertyMap::valueChanged, this, &TaskInstanceM::_onIndependentVariableValueChanged);

    // Connect to task change to reset the independent variables values
    connect(this, &TaskInstanceM::taskChanged, [this](ProtocolM* task) {
        if (task != nullptr) {
            for (IndependentVariableM* independentVariable : task->independentVariables()->toList())
            {
                if (independentVariable != nullptr)
                {
                    // Insert an (invalid) not initialized QVariant
                    _mapIndependentVariableValues->insert(independentVariable->name(), QVariant());
                    _mapIndependentVarByName.insert(independentVariable->name(), independentVariable);
                }
            }
        }
    });
}


/**
 * @brief Destructor
 */
TaskInstanceM::~TaskInstanceM()
{
    if ((_subject != nullptr) && (_task != nullptr))
    {
        qInfo() << "Delete Model of Session" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ") for subject" << _subject->displayedId() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

        // For debug purpose: Print the value of all independent variables
        _printIndependentVariableValues();

        // Clean-up independent variable map (by name). No deletion.
        _mapIndependentVarByName.clear();

        _recordsList.clear();

        // Free memory
        if (_mapIndependentVariableValues != nullptr)
        {

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
        AssessmentsModelManager::update(*this);

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
        AssessmentsModelManager::update(*this);

        // Notify change
        Q_EMIT commentsChanged(value);
    }
}

/**
 * @brief Set the value of the given independent variable into the QQmlPropertyMap
 * @param indepVar
 * @param value
 */
void TaskInstanceM::setIndependentVariableValue(IndependentVariableM* indepVar, const QString& value)
{
    if (indepVar != nullptr)
    {
        _mapIndependentVariableValues->insert(indepVar->name(), value);

        // Call SLOT manually since valueChanged() signal is only emitted from QML
        _onIndependentVariableValueChanged(indepVar->name(), value);
    }
}


/**
 * @brief Static factory method to create a task instance from a CassandraDB record
 * @param row
 * @return
 */
TaskInstanceM* TaskInstanceM::createFromCassandraRow(const CassRow* row)
{
    TaskInstanceM* taskInstance = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, subjectUuid, taskUuid, taskInstanceUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_subject"), &subjectUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &taskUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &taskInstanceUuid);

        QString sessionName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString comments(AssessmentsModelManager::getStringValueFromColumnName(row, "comment"));
        QUrl platformUrl(AssessmentsModelManager::getStringValueFromColumnName(row, "platform_file"));

        QDateTime startDateTime(AssessmentsModelManager::getDateTimeFromColumnNames(row, "start_date", "start_time"));

        taskInstance = new TaskInstanceM(experimentationUuid, taskInstanceUuid, sessionName, comments, subjectUuid, taskUuid, startDateTime);
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
        // Delete independent variable values linked to this task instance from DB
        AssessmentsModelManager::deleteEntry<IndependentVariableValueM>({ taskInstance.subject()->getExperimentationCassUuid(), taskInstance.getCassUuid() });

        // Delete the actual task instance from DB
        AssessmentsModelManager::deleteEntry<TaskInstanceM>({ taskInstance.subject()->getExperimentationCassUuid(), taskInstance.getSubjectCassUuid(), taskInstance.getTaskCassUuid(), taskInstance.getCassUuid() });
    }
}

/**
 * @brief Create a CassStatement to insert a TaskInstanceM into the DB.
 * The statement contains the values from the given taskInstance.
 * Passed taskInstance must have a valid and unique UUID.
 * @param taskInstance
 * @return
 */
CassStatement* TaskInstanceM::createBoundInsertStatement(const TaskInstanceM& taskInstance)
{
    QString queryStr = "INSERT INTO " + TaskInstanceM::table + " (id, id_experimentation, id_subject, id_task, name, comment, start_date, start_time, end_date, end_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 10);
    cass_statement_bind_uuid  (cassStatement, 0, taskInstance.getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, taskInstance.task()->getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 2, taskInstance.subject()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 3, taskInstance.task()->getCassUuid());
    cass_statement_bind_string(cassStatement, 4, taskInstance.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 5, "");
    cass_statement_bind_uint32(cassStatement, 6, cass_date_from_epoch(taskInstance.startDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 7, cass_time_from_epoch(taskInstance.startDateTime().toTime_t()));
    cass_statement_bind_uint32(cassStatement, 8, cass_date_from_epoch(taskInstance.endDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 9, cass_time_from_epoch(taskInstance.endDateTime().toTime_t()));
    return cassStatement;
}

/**
 * @brief Create a CassStatement to update a TaskInstanceM into the DB.
 * The statement contains the values from the given taskInstance.
 * Passed taskInstance must have a valid and unique UUID.
 * @param taskInstance
 * @return
 */
CassStatement* TaskInstanceM::createBoundUpdateStatement(const TaskInstanceM& taskInstance)
{

    QString queryStr("UPDATE " + TaskInstanceM::table
                     + " SET name = ?, comment = ?, start_date = ?, start_time = ?, end_date = ?, end_time = ?"
                     + " WHERE id_experimentation = ? AND id_subject = ? AND id_task = ? AND id = ?;");
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 10);
    cass_statement_bind_string(cassStatement, 0, taskInstance.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 1, taskInstance.comments().toStdString().c_str());
    cass_statement_bind_uint32(cassStatement, 2, cass_date_from_epoch(taskInstance.startDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 3, cass_time_from_epoch(taskInstance.startDateTime().toTime_t()));
    cass_statement_bind_uint32(cassStatement, 4, cass_date_from_epoch(taskInstance.endDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 5, cass_time_from_epoch(taskInstance.endDateTime().toTime_t()));
    cass_statement_bind_uuid  (cassStatement, 6, taskInstance.subject()->getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 7, taskInstance.subject()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 8, taskInstance.task()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 9, taskInstance.getCassUuid());
    return cassStatement;
}


/**
 * @brief Slot called when a value of the Qml Property Map "map Independent Variable Values" changed
 * @param key
 * @param value
 */
void TaskInstanceM::_onIndependentVariableValueChanged(const QString& key, const QVariant& value)
{
    IndependentVariableM* indepVar = _mapIndependentVarByName.value(key, nullptr);
    if (indepVar != nullptr)
    {
        AssessmentsModelManager::update(IndependentVariableValueM(_experimentationCassUuid, _cassUuid, indepVar->getCassUuid(), value.toString()));
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
