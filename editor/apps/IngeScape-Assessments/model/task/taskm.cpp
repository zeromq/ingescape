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

#include "taskm.h"

/**
 * @brief Constructor
 * @param name
 * @param parent
 */
TaskM::TaskM(QString name,
             QObject *parent) : QObject(parent),
    _name(name),
    _platformFileUrl(QUrl())
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Model of Task" << _name;

}


/**
 * @brief Destructor
 */
TaskM::~TaskM()
{
    qInfo() << "Delete Model of Task" << _name;

}


/**
 * @brief Setter for property "Platform File Url"
 * @param value
 */
void TaskM::setplatformFileUrl(QUrl value)
{
    if (_platformFileUrl != value)
    {
        _platformFileUrl = value;

        // Update file name
        if (_platformFileUrl.isValid()) {
            setplatformFileName(_platformFileUrl.fileName());
        }
        else {
            setplatformFileName("");
        }

        Q_EMIT platformFileUrlChanged(value);
    }
}
