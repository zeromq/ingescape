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

#include "recordm.h"

/**
 * @brief Constructor
 * @param parent
 */
RecordM::RecordM(QObject *parent) : QObject(parent),
    _name(),
    _subject(nullptr),
    _task(nullptr),
    _startDateTime(QDateTime()),
    _endDateTime(QDateTime())
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
