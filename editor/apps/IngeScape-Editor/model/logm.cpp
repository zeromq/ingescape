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

#include "logm.h"

/**
 * @brief Constructor
 * @param logDateTime
 * @param logType
 * @param logContent
 * @param parent
 */
LogM::LogM(QDateTime logDateTime,
           LogTypes::Value logType,
           QString logContent,
           QObject *parent) : QObject(parent),
    _logDateTime(logDateTime),
    _logType(logType),
    _logContent(logContent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
LogM::~LogM()
{

}
