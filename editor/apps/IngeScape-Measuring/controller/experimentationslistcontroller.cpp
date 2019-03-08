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

    // Groups are sorted on their name (alphabetical order)
    _allExperimentationsGroups.setSortProperty("name");


    //
    // FIXME for tests
    //
    ExperimentationsGroupVM *experimentationsGroup = new ExperimentationsGroupVM("Other", nullptr);

    _allExperimentationsGroups.append(experimentationsGroup);


    for (int i = 1; i < 4; i++)
    {
        QString groupName = QString("Group %1").arg(i);

        ExperimentationsGroupVM *group = new ExperimentationsGroupVM(groupName, nullptr);

        _allExperimentationsGroups.append(group);
    }

}


/**
 * @brief Destructor
 */
ExperimentationsListController::~ExperimentationsListController()
{
    qInfo() << "Delete Experimentations List Controller";

}
