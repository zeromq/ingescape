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
RecordM::RecordM(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of record" << _name << "(" << _id << ")";
}


/**
 * @brief Destructor
 */
RecordM::~RecordM()
{
    qInfo() << "Delete Model of Record" << _name << "(" << _id << ") ";
}
