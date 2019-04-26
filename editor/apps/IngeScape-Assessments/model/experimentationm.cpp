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

/**
 * @brief Constructor
 * @param name
 * @param creationDate
 * @param parent
 */
ExperimentationM::ExperimentationM(QString name,
                                   QDateTime creationDate,
                                   QObject *parent) : QObject(parent),
    _name(name),
    _creationDate(creationDate)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Experimentation" << _name << "(" << _creationDate.toString("dd/MM/yy hh:mm:ss") << ")";


    // Records are sorted on their start date/time (chronological order)
    _allRecords.setSortProperty("startDateTime");


    // Characteristic "Subject Name"
    CharacteristicM* characteristicName = new CharacteristicM(CHARACTERISTIC_SUBJECT_NAME, CharacteristicValueTypes::TEXT, true, this);
    _allCharacteristics.append(characteristicName);


    //
    // FIXME for tests
    //
    for (int i = 0; i < 3; i++)
    {
        QString taskName = QString("Task for test %1").arg(i + 1);

        TaskM* task = new TaskM(nullptr);
        task->setname(taskName);

        _allTasks.append(task);
    }
}


/**
 * @brief Destructor
 */
ExperimentationM::~ExperimentationM()
{
    qInfo() << "Delete Model of Experimentation" << _name;

    // Delete all characteristics of our experimentation
    _allCharacteristics.deleteAllItems();

    // Delete all subjects of our experimentation
    _allSubjects.deleteAllItems();

    // Delete all tasks of our experimentation
    _allTasks.deleteAllItems();

    // Delete all records of our experimentation
    _allRecords.deleteAllItems();

}


/**
 * @brief Add a characteristic to our experimentation
 * @param characteristic
 */
void ExperimentationM::addCharacteristic(CharacteristicM* characteristic)
{
    if (characteristic != nullptr)
    {
        // Add to the list
        _allCharacteristics.append(characteristic);

        // Add this characteristic for all existing subjects
        for (SubjectM* subject : _allSubjects)
        {
            if (subject != nullptr)
            {
                subject->addCharacteristic(characteristic);
            }
        }
    }
}


/**
 * @brief Remove a characteristic from our experimentation
 * @param characteristic
 */
void ExperimentationM::removeCharacteristic(CharacteristicM* characteristic)
{
    if (characteristic != nullptr)
    {
        // Remove from the list
        _allCharacteristics.remove(characteristic);

        // Remove this characteristic for all existing subjects
        for (SubjectM* subject : _allSubjects)
        {
            if (subject != nullptr)
            {
                subject->removeCharacteristic(characteristic);
            }
        }
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
 * @brief Add a record to our experimentation
 * @param record
 */
void ExperimentationM::addRecord(ExperimentationRecordM* record)
{
    if (record != nullptr)
    {
        // Add to the list
        _allRecords.append(record);

        if (record->subject() != nullptr)
        {
            qDebug() << "Subject:" << record->subject()->name() << "(" << record->subject()->uid() << ")";

            for (CharacteristicM* characteristic : _allCharacteristics)
            {
                if ((characteristic != nullptr) && record->subject()->mapCharacteristicValues()->contains(characteristic->name()))
                {
                    qDebug() << characteristic->name() << ":" << record->subject()->mapCharacteristicValues()->value(characteristic->name());
                }
            }
        }
    }
}


/**
 * @brief Remove a record from our experimentation
 * @param record
 */
void ExperimentationM::removeRecord(ExperimentationRecordM* record)
{
    if (record != nullptr)
    {
        // Remove from the list
        _allRecords.remove(record);
    }
}

