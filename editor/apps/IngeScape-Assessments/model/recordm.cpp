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
 * @param parent
 */
/*RecordM::RecordM(QObject *parent) : QObject(parent),
    _name(),
    _subject(nullptr),
    _task(nullptr),
    _startDateTime(QDateTime()),
    _endDateTime(QDateTime())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Record" << _name;

}*/


/**
 * @brief Constructor
 * @param name
 * @param subject
 * @param task
 * @param parent
 */
RecordM::RecordM(QString name,
                 SubjectM* subject,
                 TaskM* task,
                 QObject *parent) : QObject(parent),
    _name(name),
    _subject(subject),
    _task(task),
    _startDateTime(QDateTime()),
    _endDateTime(QDateTime()),
    _duration(QDateTime())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Record" << _name;
}


/**
 * @brief Destructor
 */
RecordM::~RecordM()
{
    qInfo() << "Delete Model of Record" << _name;

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
