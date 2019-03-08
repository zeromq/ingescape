/*
 *	IngeScape Measuring
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
     * @param parent
     */
    explicit ExperimentationM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationM();


Q_SIGNALS:


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(ExperimentationM)

#endif // EXPERIMENTATIONM_H
