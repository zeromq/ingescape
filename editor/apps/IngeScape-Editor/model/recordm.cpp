/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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
#include <model/enums.h>

/**
 * @brief Constructor
 * @param id
 * @param name
 * @param beginDate
 * @param endDate
 * @param parent
 */
RecordM::RecordM(QString id,
                 QString name,
                 QDateTime beginDate,
                 QDateTime endDate,
                 QObject *parent) : QObject(parent),
    _id(id),
    _name(name),
    _beginDateTime(beginDate),
    _endDateTime(endDate),
    _duration(QDateTime())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qint64 ms = _beginDateTime.msecsTo(_endDateTime);

    _duration = QDateTime(APPLICATION_START_DATE).addMSecs(ms);

    qInfo() << "New Model of Record" << _name << "(" << _id << ") with duration" << _duration.toString("dd/MM/yyyy HH:mm:ss");
}


/**
 * @brief Destructor
 */
RecordM::~RecordM()
{
    qInfo() << "Delete Model of Record" << _name << "(" << _id << ") ";
}
