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
#include <model/taskinstancem.h>

#include "cassandra.h"


/**
 * @brief The ExperimentationM class defines a model of experimentation
 */
class ExperimentationM : public QObject
{
    Q_OBJECT

    // Name of our experimentation
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, name)

    // Group name to which the experimentation belongs
    I2_QML_PROPERTY(QString, groupName)

    // Date of the creation of our experimentation
    I2_QML_PROPERTY(QDateTime, creationDate)

    // List of all characteristics of our experimentation
    I2_QOBJECT_LISTMODEL(CharacteristicM, allCharacteristics)

    // List of all subjects of our experimentation
    I2_QOBJECT_LISTMODEL(SubjectM, allSubjects)

    // List of all tasks of our experimentation
    I2_QOBJECT_LISTMODEL(TaskM, allTasks)

    // List of all task instances of our experimentation
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(TaskInstanceM, allTaskInstances)


public:

    /**
     * @brief Constructor
     * @param name
     * @param creationDate
     * @param parent
     */
    explicit ExperimentationM(CassUuid cassUuid,
                              QString name,
                              QString groupeName,
                              QDateTime creationDate,
                              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationM();


    /**
     * @brief Experimentation table name
     */
    static const QString table;

    /**
     * @brief Experimentation table column names
     */
    static const QStringList columnNames;

    /**
     * @brief Experimentation table primary keys IN ORDER
     */
    static const QStringList primaryKeys;


    /**
     * @brief Clear the attribute lists
     */
    void clearData();


    /**
     * @brief Get the unique identifier in Cassandra Data Base
     * @return
     */
    CassUuid getCassUuid() const;


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
     * @brief Add a subject to our experimentation
     * @param subject
     */
    void addSubject(SubjectM* subject);


    /**
     * @brief Remove a subject from our experimentation
     * @param subject
     */
    void removeSubject(SubjectM* subject);


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


    /**
     * @brief Add a task instance to our experimentation
     * @param taskInstance
     */
    void addTaskInstance(TaskInstanceM* taskInstance);


    /**
     * @brief Remove a task instance from our experimentation
     * @param taskInstance
     */
    void removeTaskInstance(TaskInstanceM* taskInstance);


    /**
     * @brief Remove task instances related to the given subject
     * @param subject
     */
    void removeTaskInstanceRelatedToSubject(SubjectM* subject);


    /**
     * @brief Remove task instances related to the given task
     * @param task
     */
    void removeTaskInstanceRelatedToTask(TaskM* task);


    /**
     * @brief Get a characteristic from its UUID
     * @param cassUuid
     * @return
     */
    CharacteristicM* getCharacteristicFromUID(const CassUuid& cassUuid);

    /**
     * @brief Get a task from its UUID
     * @param cassUuid
     * @return
     */
    SubjectM* getSubjectFromUID(const CassUuid& cassUuid);


    /**
     * @brief Get a task from its UUID
     * @param cassUuid
     * @return
     */
    TaskM* getTaskFromUID(const CassUuid& cassUuid);


    /**
     * @brief Static factory method to create an experiment from a CassandraDB record
     * @param row
     * @return
     */
    static ExperimentationM* createFromCassandraRow(const CassRow* row);

    /**
     * @brief Delete the given experimentation from Cassandra DB
     * Also deletes its associated taks, subjects, characteristics and characteristics values
     * @param experimentation
     */
    static void deleteExperimentationFromCassandra(const ExperimentationM& experimentation);


    /**
     * @brief Create a CassStatement to insert an ExperimentationM into the DB.
     * The statement contains the values from the given experimentation.
     * Passed experimentation must have a valid and unique UUID.
     * @param experimentation
     * @return
     */
    static CassStatement* createBoundInsertStatement(const ExperimentationM& experimentation);


private: // Methods
    /**
     * @brief Delete all tasks associated with the given experimentation
     * @param experimentation
     */
    static void _deleteAllTasksForExperimentation(const ExperimentationM& experimentation);
    /**
     * @brief Delete all subjects associated with the given experimentation
     * @param experimentation
     */
    static void _deleteAllSubjectsForExperimentation(const ExperimentationM& experimentation);
    /**
     * @brief Delete all characteristics associated with the given experimentation
     * @param experimentation
     */
    static void _deleteAllCharacteristicsForExperimentation(const ExperimentationM& experimentation);
    /**
     * @brief Delete all task instances with the given experimentation
     * @param experimentation
     */
    static void _deleteAllTaskInstancesForExperimentation(const ExperimentationM& experimentation);


private:
    // Unique identifier in Cassandra Data Base
    CassUuid _cassUuid;

    // Hash table from an UID to a characteristic
    QHash<CassUuid, CharacteristicM*> _hashFromUIDtoCharacteristic;

};

QML_DECLARE_TYPE(ExperimentationM)

#endif // EXPERIMENTATIONM_H
