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

#include "sessionm.h"

#include "controller/assessmentsmodelmanager.h"
#include "model/protocol/independentvariablevaluem.h"

/**
 * @brief Session table name
 */
const QString SessionM::table = "ingescape.task_instance";

/**
 * @brief Session table column names
 */
const QStringList SessionM::columnNames = {
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
 * @brief Session table primary keys IN ORDER
 */
const QStringList SessionM::primaryKeys = {
    "id_experimentation",
    "id_subject",
    "id_task",
    "id",
};

/**
 * @brief Constructor
 * @param experimentationUuid
 * @param cassUuid
 * @param name
 * @param comments
 * @param subjectUuid
 * @param protocolUuid
 * @param startDateTime
 * @param parent
 */
SessionM::SessionM(CassUuid experimentationUuid,
                             CassUuid cassUuid,
                             QString name,
                             QString comments,
                             CassUuid subjectUuid,
                             CassUuid protocolUuid,
                             QDateTime startDateTime,
                             QObject *parent) : QObject(parent),
    _name(name),
    _comments(comments),
    _subject(nullptr),
    _protocol(nullptr),
    _startDateTime(startDateTime),
    _endDateTime(startDateTime),  //FIXME Need a way to compute actual endDateTime.
    _duration(QTime()),           //FIXME Need a way to compute actual duration.
    _mapIndependentVariableValues(nullptr),
    _isRecorded(false),
    _experimentationCassUuid(experimentationUuid),
    _subjectUuid(subjectUuid),
    _protocolUuid(protocolUuid),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Session" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";

    // Create the "Qml Property Map" that allows to set key-value pairs that can be used in QML bindings
    _mapIndependentVariableValues = new QQmlPropertyMap(this);

    _recordsList.setSortProperty("startTimeInTimeline");

    // Connect to signal "Value Changed" from the "Qml Property Map"
    connect(_mapIndependentVariableValues, &QQmlPropertyMap::valueChanged, this, &SessionM::_onIndependentVariableValueChanged);

    // Connect to protocol change to reset the independent variables values
    connect(this, &SessionM::protocolChanged, [this](ProtocolM* protocol)
    {
        if (protocol != nullptr)
        {
            for (IndependentVariableM* independentVariable : protocol->independentVariables()->toList())
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
SessionM::~SessionM()
{
    if ((_subject != nullptr) && (_protocol != nullptr))
    {
        qInfo() << "Delete Model of Session" << _name << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ") for subject" << _subject->displayedId() << "and protocol" << _protocol->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

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
        setprotocol(nullptr);
    }
}

/**
 * @brief Custome _name setter that updates the DB entry with the new name
 * @param value
 */
void SessionM::setname(QString value)
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
void SessionM::setcomments(QString value)
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
void SessionM::setIndependentVariableValue(IndependentVariableM* indepVar, const QString& value)
{
    if (indepVar != nullptr)
    {
        _mapIndependentVariableValues->insert(indepVar->name(), value);

        // Call SLOT manually since valueChanged() signal is only emitted from QML
        _onIndependentVariableValueChanged(indepVar->name(), value);
    }
}


/**
 * @brief Static factory method to create a session from a CassandraDB record
 * @param row
 * @return
 */
SessionM* SessionM::createFromCassandraRow(const CassRow* row)
{
    SessionM* session = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUuid, subjectUuid, protocolUuid, sessionUuid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_subject"), &subjectUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id_task"), &protocolUuid);
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &sessionUuid);

        QString sessionName(AssessmentsModelManager::getStringValueFromColumnName(row, "name"));
        QString comments(AssessmentsModelManager::getStringValueFromColumnName(row, "comment"));
        QUrl platformUrl(AssessmentsModelManager::getStringValueFromColumnName(row, "platform_file"));

        QDateTime startDateTime(AssessmentsModelManager::getDateTimeFromColumnNames(row, "start_date", "start_time"));

        session = new SessionM(experimentationUuid,
                               sessionUuid,
                               sessionName,
                               comments,
                               subjectUuid,
                               protocolUuid,
                               startDateTime);
    }

    return session;
}

/**
 * @brief Delete the given session from Cassandra DB
 * @param session
 */
void SessionM::deleteSessionFromCassandra(const SessionM& session)
{
    if ((session.subject() != nullptr) && (session.protocol() != nullptr))
    {
        // Delete independent variable values linked to this session from DB
        AssessmentsModelManager::deleteEntry<IndependentVariableValueM>({ session.subject()->getExperimentationCassUuid(), session.getCassUuid() });

        // Delete the actual session from DB
        AssessmentsModelManager::deleteEntry<SessionM>({ session.subject()->getExperimentationCassUuid(), session.getSubjectCassUuid(), session.getProtocolCassUuid(), session.getCassUuid() });
    }
}

/**
 * @brief Create a CassStatement to insert a SessionM into the DB.
 * The statement contains the values from the given session.
 * Passed session must have a valid and unique UUID.
 * @param session
 * @return
 */
CassStatement* SessionM::createBoundInsertStatement(const SessionM& session)
{
    QString queryStr = "INSERT INTO " + SessionM::table + " (id, id_experimentation, id_subject, id_task, name, comment, start_date, start_time, end_date, end_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 10);
    cass_statement_bind_uuid  (cassStatement, 0, session.getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 1, session.protocol()->getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 2, session.subject()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 3, session.protocol()->getCassUuid());
    cass_statement_bind_string(cassStatement, 4, session.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 5, "");
    cass_statement_bind_uint32(cassStatement, 6, cass_date_from_epoch(session.startDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 7, cass_time_from_epoch(session.startDateTime().toTime_t()));
    cass_statement_bind_uint32(cassStatement, 8, cass_date_from_epoch(session.endDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 9, cass_time_from_epoch(session.endDateTime().toTime_t()));
    return cassStatement;
}

/**
 * @brief Create a CassStatement to update a SessionM into the DB.
 * The statement contains the values from the given session.
 * Passed session must have a valid and unique UUID.
 * @param session
 * @return
 */
CassStatement* SessionM::createBoundUpdateStatement(const SessionM& session)
{

    QString queryStr("UPDATE " + SessionM::table
                     + " SET name = ?, comment = ?, start_date = ?, start_time = ?, end_date = ?, end_time = ?"
                     + " WHERE id_experimentation = ? AND id_subject = ? AND id_task = ? AND id = ?;");
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 10);
    cass_statement_bind_string(cassStatement, 0, session.name().toStdString().c_str());
    cass_statement_bind_string(cassStatement, 1, session.comments().toStdString().c_str());
    cass_statement_bind_uint32(cassStatement, 2, cass_date_from_epoch(session.startDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 3, cass_time_from_epoch(session.startDateTime().toTime_t()));
    cass_statement_bind_uint32(cassStatement, 4, cass_date_from_epoch(session.endDateTime().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 5, cass_time_from_epoch(session.endDateTime().toTime_t()));
    cass_statement_bind_uuid  (cassStatement, 6, session.subject()->getExperimentationCassUuid());
    cass_statement_bind_uuid  (cassStatement, 7, session.subject()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 8, session.protocol()->getCassUuid());
    cass_statement_bind_uuid  (cassStatement, 9, session.getCassUuid());
    return cassStatement;
}


/**
 * @brief Slot called when a value of the Qml Property Map "map Independent Variable Values" changed
 * @param key
 * @param value
 */
void SessionM::_onIndependentVariableValueChanged(const QString& key, const QVariant& value)
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
void SessionM::setendDateTime(QDateTime value)
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
void SessionM::_printIndependentVariableValues()
{
    if ((_protocol != nullptr) && (_mapIndependentVariableValues != nullptr))
    {
        for (IndependentVariableM* independentVariable : _protocol->independentVariables()->toList())
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
