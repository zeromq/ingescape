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
    _uid(AssessmentsModelManager::cassUuidToQString(cassUuid)),
    _name(name),
    _groupName(groupeName),
    _creationDate(creationDate),
    _cassUuid(cassUuid)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Experimentation" << _name << "created" << _creationDate.toString("dd/MM/yy hh:mm:ss") << "(" << _uid << ")";

    // Record setups are sorted on their start date/time (chronological order)
    _allRecordSetups.setSortProperty("startDateTime");
}


/**
 * @brief Destructor
 */
ExperimentationM::~ExperimentationM()
{
    qInfo() << "Delete Model of Experimentation" << _name << "created" << _creationDate.toString("dd/MM/yy hh:mm:ss") << "(" << _uid << ")";

    clearData();
}


/**
 * @brief Clear the attribute lists
 */
void ExperimentationM::clearData()
{
    // Delete all record setups of our experimentation
    _allRecordSetups.deleteAllItems();

    // Delete all characteristics of our experimentation
    _allCharacteristics.deleteAllItems();

    // Delete all subjects of our experimentation
    _allSubjects.deleteAllItems();

    // Delete all tasks of our experimentation
    _allTasks.deleteAllItems();
}


/**
 * @brief Get the unique identifier in Cassandra Data Base
 * @return
 */
CassUuid ExperimentationM::getCassUuid()
{
    return _cassUuid;
}


/**
 * @brief Add a characteristic to our experimentation
 * @param characteristic
 */
void ExperimentationM::addCharacteristic(CharacteristicM* characteristic)
{
    if ((characteristic != nullptr) && !_hashFromUIDtoCharacteristic.contains(characteristic->uid()))
    {
        // Add to the hash
        _hashFromUIDtoCharacteristic.insert(characteristic->uid(), characteristic);

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
    if ((characteristic != nullptr) && _hashFromUIDtoCharacteristic.contains(characteristic->uid()))
    {
        // Remove from the hash
        _hashFromUIDtoCharacteristic.remove(characteristic->uid());

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
    }
}


/**
 * @brief Add a task to our experimentation
 * @param task
 */
void ExperimentationM::addTask(TaskM* task)
{
    if (task != nullptr)
    {
        // Add to the list
        _allTasks.append(task);
    }
}


/**
 * @brief Remove a task from our experimentation
 * @param task
 */
void ExperimentationM::removeTask(TaskM* task)
{
    if (task != nullptr)
    {
        // Remove from the list
        _allTasks.remove(task);
    }
}


/**
 * @brief Add a record setup to our experimentation
 * @param record
 */
void ExperimentationM::addRecordSetup(RecordSetupM* recordSetup)
{
    if (recordSetup != nullptr)
    {
        // Add to the list
        _allRecordSetups.append(recordSetup);

        if (recordSetup->subject() != nullptr)
        {
            qDebug() << "Subject:" << recordSetup->subject()->displayedId();

            for (CharacteristicM* characteristic : _allCharacteristics)
            {
                if ((characteristic != nullptr) && recordSetup->subject()->mapCharacteristicValues()->contains(characteristic->name()))
                {
                    qDebug() << characteristic->name() << ":" << recordSetup->subject()->mapCharacteristicValues()->value(characteristic->name());
                }
            }
        }
    }
}


/**
 * @brief Remove a record setup from our experimentation
 * @param record
 */
void ExperimentationM::removeRecordSetup(RecordSetupM* recordSetup)
{
    if (recordSetup != nullptr)
    {
        // Remove from the list
        _allRecordSetups.remove(recordSetup);
    }
}


/**
 * @brief Get a characteristic from its UID
 * @param uid
 * @return
 */
CharacteristicM* ExperimentationM::getCharacteristicFromUID(const QString& uid)
{
    return _hashFromUIDtoCharacteristic.value(uid, nullptr);
}

/**
 * @brief Get a task from its UID
 * @param uid
 * @return
 */
SubjectM* ExperimentationM::getSubjectFromUID(const QString& uid)
{
    auto subjectIt = std::find_if(_allSubjects.begin(), _allSubjects.end(), [uid](SubjectM* subject) { return (subject != nullptr) && (subject->uid() == uid); });
    return (subjectIt != _allSubjects.end()) ? *subjectIt : nullptr;
}


/**
 * @brief Get a task from its UID
 * @param uid
 * @return
 */
TaskM* ExperimentationM::getTaskFromUID(const QString& uid)
{
    auto taskIt = std::find_if(_allTasks.begin(), _allTasks.end(), [uid](TaskM* task) { return (task != nullptr) && (task->uid() == uid); });
    return (taskIt != _allTasks.end()) ? *taskIt : nullptr;
}


/**
 * @brief Static factory method to create an experiment from a CassandraDB record
 * @param row
 * @return
 */
ExperimentationM* ExperimentationM::createExperimentationFromCassandraRow(const CassRow* row)
{
    ExperimentationM* experimentation = nullptr;

    if (row != nullptr)
    {
        CassUuid experimentationUid;
        cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &experimentationUid);

        const char *chrExperimentationName = "";
        size_t nameLength;
        cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrExperimentationName, &nameLength);
        QString experimentationName = QString::fromUtf8(chrExperimentationName, static_cast<int>(nameLength));

        const char *chrGroupName = "";
        size_t groupNameLength;
        cass_value_get_string(cass_row_get_column_by_name(row, "group_name"), &chrGroupName, &groupNameLength);
        QString groupName = QString::fromUtf8(chrGroupName, static_cast<int>(groupNameLength));

        cass_uint32_t creationDate;
        cass_value_get_uint32(cass_row_get_column_by_name(row, "creation_date"), &creationDate);

        cass_int64_t creationTime;
        cass_value_get_int64(cass_row_get_column_by_name(row, "creation_time"), &creationTime);

        time_t secCreationDateTime = cass_date_time_to_epoch(creationDate, creationTime);
        QDateTime creationDateTime;
        creationDateTime.setSecsSinceEpoch(secCreationDateTime);

        experimentation = new ExperimentationM(experimentationUid, experimentationName, groupName, creationDateTime, nullptr);
    }

    return experimentation;
}

