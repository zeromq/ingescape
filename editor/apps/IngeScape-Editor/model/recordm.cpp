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

/**
 * @brief Constructor
 * @param name
 * @param peerId
 * @param ipAddress
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
    _endDateTime(endDate)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
RecordM::~RecordM()
{
    qInfo() << "Delete Model of Record" << _name << "(" << _id << ") ";
}
