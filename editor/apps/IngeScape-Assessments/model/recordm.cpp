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

#include "recordm.h"


/**
 * @brief Constructor
 * @param name
 * @param subject
 * @param task
 * @param startDateTime
 * @param parent
 */
RecordM::RecordM(QString uid,
                 QString name,
                 SubjectM* subject,
                 TaskM* task,
                 QDateTime startDateTime,
                 QObject *parent) : QObject(parent),
    _uid(uid),
    _name(name),
    _subject(subject),
    _task(task),
    _startDateTime(startDateTime),
    _endDateTime(QDateTime()),
    _duration(QDateTime())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if ((_subject != nullptr) && (_task != nullptr))
    {
        qInfo() << "New Model of Record" << _name << "(" << _uid << ") for subject" << _subject->name() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");
    }
}


/**
 * @brief Destructor
 */
RecordM::~RecordM()
{
    if ((_subject != nullptr) && (_task != nullptr))
    {
        qInfo() << "Delete Model of Record" << _name << "(" << _uid << ") for subject" << _subject->name() << "and task" << _task->name() << "at" << _startDateTime.toString("dd/MM/yyyy hh:mm:ss");

        setsubject(nullptr);
        settask(nullptr);
    }
}


/**
 * @brief Setter for property "End Date Time"
 * @param value
 */
void RecordM::setendDateTime(QDateTime value)
{
    if (_endDateTime != value)
    {
        _endDateTime = value;

        // Update the duration
        qint64 milliSeconds = _startDateTime.msecsTo(_endDateTime);
        QTime time = QTime(0, 0, 0, 0).addMSecs(static_cast<int>(milliSeconds));

        setduration(QDateTime(_startDateTime.date(), time));

        Q_EMIT endDateTimeChanged(value);
    }
}
