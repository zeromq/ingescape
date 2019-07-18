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

#include "exportcontroller.h"

/*extern "C" {
#include <cqlexporter.h>
}*/


/**
 * @brief Constructor
 * @param parent
 */
ExportController::ExportController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Export Controller";
}


/**
 * @brief Destructor
 */
ExportController::~ExportController()
{
    qInfo() << "Delete Export Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr) {
        setcurrentExperimentation(nullptr);
    }
}


/**
 * @brief Export the current experimentation
 */
void ExportController::exportExperimentation()
{
    if (_currentExperimentation != nullptr)
    {
        qDebug() << "Export the experimentation" << _currentExperimentation->name();

        //_currentExperimentation->getCassUuid();
    }
}
