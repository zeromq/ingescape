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


    // Default characteristic "Name"
    CharacteristicM* characteristicName = new CharacteristicM(CHARACTERISTIC_NAME, CharacteristicValueTypes::TEXT, false, this);
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
 * @brief Add the characteristic to our experimentation
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
 * @brief Remove the characteristic from our experimentation
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

