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


    //
    // FIXME for tests
    //
    for (int i = 0; i < 3; i++)
    {
        QString subjectName = QString("Subject for test %1").arg(i + 1);
        QString taskName = QString("Task for test %1").arg(i + 1);

        SubjectM* subject = new SubjectM(nullptr);
        subject->setname(subjectName);

        _allSubjects.append(subject);

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

}

