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

#include "experimentationslistcontroller.h"

/**
 * @brief Constructor
 * @param parent
 */
ExperimentationsListController::ExperimentationsListController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentations List Controller";
}


/**
 * @brief Destructor
 */
ExperimentationsListController::~ExperimentationsListController()
{
    qInfo() << "Delete Experimentations List Controller";

}
