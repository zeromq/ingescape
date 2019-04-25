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

#include "recordcontroller.h"

/**
 * @brief Constructor
 * @param parent
 */
RecordController::RecordController(QObject *parent) : QObject(parent),
    _currentRecord(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";
}


/**
 * @brief Destructor
 */
RecordController::~RecordController()
{
    qInfo() << "Delete Record Controller";

}
