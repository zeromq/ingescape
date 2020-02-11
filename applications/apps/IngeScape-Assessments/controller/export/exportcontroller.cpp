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
#include <controller/assessmentsmodelmanager.h>
#include <misc/ingescapeutils.h>


/**
 * @brief Constructor
 * @param parent
 */
ExportController::ExportController(QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _exportsDirectoryPath("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Export Controller";

    // Init the path to the directory containing CSV files about exports
    _exportsDirectoryPath = IngeScapeUtils::getExportsPath();

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

