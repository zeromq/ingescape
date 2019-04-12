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

#ifndef EXPERIMENTATIONM_H
#define EXPERIMENTATIONM_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <model/subject/characteristicm.h>
#include <model/subject/subjectm.h>
#include <model/task/taskm.h>
#include <model/recordm.h>


/**
 * @brief The ExperimentationM class defines a model of experimentation
 */
class ExperimentationM : public QObject
{
    Q_OBJECT

    // Name of our experimentation
    I2_QML_PROPERTY(QString, name)

    // Date of the creation of our experimentation
    I2_QML_PROPERTY(QDateTime, creationDate)

    // List of all characteristics of our experimentation
    I2_QOBJECT_LISTMODEL(CharacteristicM, allCharacteristics)

    // List of all subjects of our experimentation
    I2_QOBJECT_LISTMODEL(SubjectM, allSubjects)

    // List of all tasks of our experimentation
    I2_QOBJECT_LISTMODEL(TaskM, allTasks)

    // List of all records of our experimentation
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(RecordM, allRecords)


public:

    /**
     * @brief Constructor
     * @param name
     * @param creationDate
     * @param parent
     */
    explicit ExperimentationM(QString name,
                              QDateTime creationDate,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationM();


    /**
     * @brief Add a characteristic to our experimentation
     * @param characteristic
     */
    void addCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Remove a characteristic from our experimentation
     * @param characteristic
     */
    void removeCharacteristic(CharacteristicM* characteristic);


    /**
     * @brief Add a task to our experimentation
     * @param task
     */
    void addTask(TaskM* task);


    /**
     * @brief Remove a task from our experimentation
     * @param task
     */
    void removeTask(TaskM* task);



Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(ExperimentationM)

#endif // EXPERIMENTATIONM_H
