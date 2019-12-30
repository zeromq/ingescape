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

#include "experimentationm.h"
#include <misc/ingescapeutils.h>

#include "controller/assessmentsmodelmanager.h"
#include "model/subject/characteristicvaluem.h"
#include "model/protocol/independentvariablevaluem.h"

/**
 * @brief Experimentation table name
 */
const QString ExperimentationM::table = "ingescape.experimentation";

/**
 * @brief Experimentation table column names
 */
const QStringList ExperimentationM::columnNames = {
    "id",
    "creation_date",
    "creation_time",
    "group_name",
    "name",
};

/**
 * @brief Experimentation table primary keys IN ORDER
 */
const QStringList ExperimentationM::primaryKeys = {
    "id",
};

/**
 * @brief Constructor
 * @param name
 * @param creationDate
 * @param parent
 */
ExperimentationM::ExperimentationM(CassUuid cassUuid,
                                   QString name,
                                   QString groupeName,
                                   QDateTime creationDate,
                                   QObject *parent) : QObject(parent),
    _name(name),
    _groupName(groupeName),
    _creationDate(creationDate),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Experimentation" << _name << "created" << _creationDate.toString("dd/MM/yy hh:mm:ss") << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";

    // Task instances are sorted on their start date/time (chronological order)
    _allSessions.setSortProperty("startDateTime");
}


/**
 * @brief Destructor
 */
ExperimentationM::~ExperimentationM()
{
    qInfo() << "Delete Model of Experimentation" << _name << "created" << _creationDate.toString("dd/MM/yy hh:mm:ss") << "(" << AssessmentsModelManager::cassUuidToQString(_cassUuid) << ")";

    clearData();
}


/**
 * @brief Clear the attribute lists
 */
void ExperimentationM::clearData()
{
    // Delete all task instances of our experimentation
    _allSessions.deleteAllItems();

    // Delete all characteristics of our experimentation
    _hashFromUIDtoCharacteristic.clear();
    _allCharacteristics.deleteAllItems();

    // Delete all subjects of our experimentation
    _allSubjects.deleteAllItems();

    // Delete all protocols of our experimentation
    _allProtocols.deleteAllItems();
}


void ExperimentationM::setname(QString value)
{
    if (_name != value)
    {
        _name = value;

        AssessmentsModelManager::update(*this);

        Q_EMIT nameChanged(value);
    }
}


/**
 * @brief Get the unique identifier in Cassandra Data Base
 * @return
 */
CassUuid ExperimentationM::getCassUuid() const
{
    return _cassUuid;
}


/**
 * @brief Add a characteristic to our experimentation
 * @param characteristic
 */
void ExperimentationM::addCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && !_hashFromUIDtoCharacteristic.contains(characteristic->getCassUuid()))
    {
        // Add to the hash
        _hashFromUIDtoCharacteristic.insert(characteristic->getCassUuid(), characteristic);

        // Add to the list
        _allCharacteristics.append(characteristic);
    }
}


/**
 * @brief Remove a characteristic from our experimentation
 * @param characteristic
 */
void ExperimentationM::removeCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && _hashFromUIDtoCharacteristic.contains(characteristic->getCassUuid()))
    {
        // Remove from the hash
        _hashFromUIDtoCharacteristic.remove(characteristic->getCassUuid());

        // Remove from the list
        _allCharacteristics.remove(characteristic);
    }
}


/**
 * @brief Add a subject to our experimentation
 * @param subject
 */
void ExperimentationM::addSubject(SubjectM* subject)
{
    if (subject != nullptr)
    {
        // Add to the list
        _allSubjects.append(subject);
    }
}


/**
 * @brief Remove a subject from our experimentation
 * @param subject
 */
void ExperimentationM::removeSubject(SubjectM* subject)
{
    if (subject != nullptr)
    {
        // Remove from the list
        _allSubjects.remove(subject);

        // Remove related sessions
        removeSessionsRelatedToSubject(subject);
    }
}


/**
 * @brief Add a protocol to our experimentation
 * @param protocol
 */
void ExperimentationM::addProtocol(ProtocolM* protocol)
{
    if (protocol != nullptr)
    {
        // Add to the list
        _allProtocols.append(protocol);
    }
}


/**
 * @brief Remove a protocol from our experimentation
 * @param protocol
 */
void ExperimentationM::removeProtocol(ProtocolM* protocol)
{
    if (protocol != nullptr)
    {
        // Remove from the list
        _allProtocols.remove(protocol);

        // Remove related sessions
        removeSessionsRelatedToProtocol(protocol);
    }
}


/**
 * @brief Add a session to our experimentation
 * @param session
 */
void ExperimentationM::addSession(SessionM* session)
{
    if (session != nullptr)
    {
        // Add to the list
        _allSessions.append(session);
    }
}


/**
 * @brief Remove a session from our experimentation
 * @param session
 */
void ExperimentationM::removeSession(SessionM* session)
{
    if (session != nullptr)
    {
        // Remove from the list
        _allSessions.remove(session);
    }
}


/**
 * @brief Remove sessions related to the given subject
 * @param subject
 */
void ExperimentationM::removeSessionsRelatedToSubject(SubjectM* subject)
{
    const QList<SessionM*> sessionList = _allSessions.toList();
    auto sessionIt = sessionList.begin();
    while (sessionIt != sessionList.end())
    {
        // Looking for a task instance related to the given subject
        sessionIt = std::find_if(sessionIt, sessionList.end(), [subject](SessionM* session){
                return (session != nullptr) && (session->subject() == subject);
        });

        if (sessionIt != sessionList.end())
        {
            // We found a session with given subject
            removeSession(*sessionIt);
            ++sessionIt;
        }
    }
}


/**
 * @brief Remove sessions related to the given protocol
 * @param protocol
 */
void ExperimentationM::removeSessionsRelatedToProtocol(ProtocolM* protocol)
{
    const QList<SessionM*> sessionList = _allSessions.toList();
    auto sessionIt = sessionList.begin();
    while (sessionIt != sessionList.end())
    {
        // Looking for a task instance related to the given subject
        sessionIt = std::find_if(sessionIt, sessionList.end(), [protocol](SessionM* session) {
                return (session != nullptr) && (session->protocol() == protocol);
        });

        if (sessionIt != sessionList.end())
        {
            // We found a session with given protocol
            removeSession(*sessionIt);
            ++sessionIt;
        }
    }
}


/**
 * @brief Get a characteristic from its UUID
 * @param cassUuid
 * @return
 */
CharacteristicM* ExperimentationM::getCharacteristicFromUID(const CassUuid& cassUuid)
{
    return _hashFromUIDtoCharacteristic.value(cassUuid, nullptr);
}

/**
 * @brief Get a task from its UUID
 * @param cassUuid
 * @return
 */
SubjectM* ExperimentationM::getSubjectFromUID(const CassUuid& cassUuid)
{
    auto subjectIt = std::find_if(_allSubjects.begin(), _allSubjects.end(), [cassUuid](SubjectM* subject) { return (subject != nullptr) && (subject->getCassUuid() == cassUuid); });
    return (subjectIt != _allSubjects.end()) ? *subjectIt : nullptr;
}


/**
 * @brief Get a protocol from its UUID
 * @param cassUuid
 * @return
 */
ProtocolM* ExperimentationM::getProtocolFromUID(const CassUuid& cassUuid)
{
    auto taskIt = std::find_if(_allProtocols.begin(), _allProtocols.end(), [cassUuid](ProtocolM* protocol) { return (protocol != nullptr) && (protocol->getCassUuid() == cassUuid); });
    return (taskIt != _allProtocols.end()) ? *taskIt : nullptr;
}


/**
 * @brief Get a session from its UUID
 * @param cassUuid
 * @return
 */
SessionM* ExperimentationM::getSessionFromUID(const CassUuid& cassUuid)
{
    // FIXME: why the copy ?
    const QList<SessionM*> sessionsList = _allSessions.toList();
    auto sessionIt = std::find_if(sessionsList.begin(), sessionsList.end(), [cassUuid](SessionM* session) { return (session != nullptr) && (session->getCassUuid() == cassUuid); });
    return (sessionIt != sessionsList.end()) ? *sessionIt : nullptr;
}


/**
 * @brief Static factory method to create an experiment from a CassandraDB record
 * @param row
 * @return
 */
ExperimentationM* ExperimentationM::createFromCassandraRow(const CassRow* row)
{
    ExperimentationM* experimentation = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &experimentationUid);

        QString experimentationName = AssessmentsModelManager::getStringValueFromColumnName(row, "name");
        QString groupName = AssessmentsModelManager::getStringValueFromColumnName(row, "group_name");
        QDateTime creationDateTime(AssessmentsModelManager::getDateTimeFromColumnNames(row, "creation_date", "creation_time"));

        experimentation = new ExperimentationM(experimentationUid, experimentationName, groupName, creationDateTime, nullptr);
    }

    return experimentation;
}

/**
 * @brief Delete the given experimentation from Cassandra DB
 * Also deletes its associated taks, subjects, characteristics and characteristics values
 * @param experimentation
 */
void ExperimentationM::deleteExperimentationFromCassandra(const ExperimentationM& experimentation)
{
    // Delete experimentations associations
    _deleteAllProtocolsForExperimentation(experimentation);
    _deleteAllSubjectsForExperimentation(experimentation);
    _deleteAllCharacteristicsForExperimentation(experimentation);
    _deleteAllSessionsForExperimentation(experimentation);

    // Delete actual experimentation
    AssessmentsModelManager::deleteEntry<ExperimentationM>({ experimentation.getCassUuid() });
}


/**
 * @brief Create a CassStatement to insert an ExperimentationM into the DB.
 * The statement contains the values from the given experimentation.
 * Passed experimentation must have a valid and unique UUID.
 * @param experimentation
 * @return
 */
CassStatement* ExperimentationM::createBoundInsertStatement(const ExperimentationM& experimentation)
{
    // Create the query
    QString query = "INSERT INTO " + ExperimentationM::table + " (id, name, creation_date, creation_time, group_name) VALUES (?, ?, ?, ?, ?);";

    // Creates the new query statement
    CassStatement* cassStatement = cass_statement_new(query.toStdString().c_str(), 5);
    cass_statement_bind_uuid  (cassStatement, 0, experimentation.getCassUuid());
    cass_statement_bind_string(cassStatement, 1, experimentation.name().toStdString().c_str());
    cass_statement_bind_uint32(cassStatement, 2, cass_date_from_epoch(experimentation.creationDate().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 3, cass_time_from_epoch(experimentation.creationDate().toTime_t()));
    cass_statement_bind_string(cassStatement, 4, experimentation.groupName().toStdString().c_str());

    return cassStatement;
}

/**
 * @brief Create a CassStatement to update an ExperimentationM into the DB.
 * The statement contains the values from the given experimentation.
 * Passed experimentation must have a valid and unique UUID.
 * @param experimentation
 * @return
 */
CassStatement* ExperimentationM::createBoundUpdateStatement(const ExperimentationM& experimentation)
{
    QString queryStr = "UPDATE " + ExperimentationM::table + " SET name = ?, creation_date = ?, creation_time = ?, group_name = ? WHERE id = ?;";
    CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 5);
    cass_statement_bind_string(cassStatement, 0, experimentation.name().toStdString().c_str());
    cass_statement_bind_uint32(cassStatement, 1, cass_date_from_epoch(experimentation.creationDate().toTime_t()));
    cass_statement_bind_int64 (cassStatement, 2, cass_time_from_epoch(experimentation.creationDate().toTime_t()));
    cass_statement_bind_string(cassStatement, 3, experimentation.groupName().toStdString().c_str());
    cass_statement_bind_uuid  (cassStatement, 4, experimentation.getCassUuid());
    return cassStatement;
}

/**
 * @brief Delete all protocols associated with the given experimentation
 * @param experimentation
 */
void ExperimentationM::_deleteAllProtocolsForExperimentation(const ExperimentationM& experimentation)
{
    // Delete all tasks associations
    AssessmentsModelManager::deleteEntry<IndependentVariableM>({ experimentation.getCassUuid() });
    AssessmentsModelManager::deleteEntry<IndependentVariableValueM>({ experimentation.getCassUuid() });
    AssessmentsModelManager::deleteEntry<DependentVariableM>({ experimentation.getCassUuid() });

    // Delete all tasks
    AssessmentsModelManager::deleteEntry<ProtocolM>({ experimentation.getCassUuid() });
}

/**
 * @brief Delete all subjects associated with the given experimentation
 * @param experimentation
 */
void ExperimentationM::_deleteAllSubjectsForExperimentation(const ExperimentationM& experimentation)
{
    // Delete all subjects associations
    AssessmentsModelManager::deleteEntry<CharacteristicValueM>({ experimentation.getCassUuid() });

    // Delete all subjects
    AssessmentsModelManager::deleteEntry<SubjectM>({ experimentation.getCassUuid() });
}

/**
 * @brief Delete all characteristics associated with the given experimentation
 * @param experimentation
 */
void ExperimentationM::_deleteAllCharacteristicsForExperimentation(const ExperimentationM& experimentation)
{
    // Characteristics values already deleted with subject deletion

    // Delete all characteristics
    AssessmentsModelManager::deleteEntry<CharacteristicM>({ experimentation.getCassUuid() });
}

/**
 * @brief Delete all sessions with the given experimentation
 * @param experimentation
 */
void ExperimentationM::_deleteAllSessionsForExperimentation(const ExperimentationM& experimentation)
{
    // Independent variable values already deleted with task deletion

    // Delete all task_instance
    AssessmentsModelManager::deleteEntry<SessionM>({ experimentation.getCassUuid() });
}

