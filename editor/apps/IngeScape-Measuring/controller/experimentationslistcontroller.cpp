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
ExperimentationsListController::ExperimentationsListController(QObject *parent) : QObject(parent),
    _defaultGroupOther(nullptr),
    _newGroup(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentations List Controller";

    // Groups are sorted on their name (alphabetical order)
    _allExperimentationsGroups.setSortProperty("name");


    // Create the default group "Other"
    _defaultGroupOther = new ExperimentationsGroupVM(tr("Others"), nullptr);

    _allExperimentationsGroups.append(_defaultGroupOther);


    // Create the (fake) group "New"
    _newGroup = new ExperimentationsGroupVM(tr("New Group"), nullptr);


    //
    // FIXME for tests
    //
    for (int i = 1; i < 3; i++)
    {
        QString groupName = QString("Group for test %1").arg(i);

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

    _allExperimentationsGroups.clear();
    //_allExperimentationsGroups.deleteAllItems();

    if (_defaultGroupOther != nullptr)
    {
        ExperimentationsGroupVM* temp = _defaultGroupOther;
        setdefaultGroupOther(nullptr);
        delete temp;
    }

    if (_newGroup != nullptr)
    {
        ExperimentationsGroupVM* temp = _newGroup;
        setnewGroup(nullptr);
        delete temp;
    }
}


/**
 * @brief Create a new experimentation in new group (to create)
 * @param experimentationName
 * @param newExperimentationsGroupName
 */
void ExperimentationsListController::createNewExperimentationInNewGroup(QString experimentationName, QString newExperimentationsGroupName)
{
    if (!experimentationName.isEmpty() && !newExperimentationsGroupName.isEmpty())
    {
        // Create a new experimentations group
        ExperimentationsGroupVM* newExperimentationsGroup = new ExperimentationsGroupVM(newExperimentationsGroupName, nullptr);

        _allExperimentationsGroups.append(newExperimentationsGroup);

        // Create a new experimentation in this new group
        createNewExperimentationInGroup(experimentationName, newExperimentationsGroup);
    }
}


/**
 * @brief Create a new experimentation in a group
 * @param experimentationName
 * @param experimentationsGroup
 */
void ExperimentationsListController::createNewExperimentationInGroup(QString experimentationName, ExperimentationsGroupVM* experimentationsGroup)
{
    if (!experimentationName.isEmpty() && (experimentationsGroup != nullptr))
    {
        // Create the new experimentation
        ExperimentationM* experimentation = new ExperimentationM(experimentationName, QDateTime::currentDateTime(), nullptr);

        // Add to the group
        experimentationsGroup->experimentations()->append(experimentation);
    }
    else {
        qWarning() << "Cannot create new experimentation because name is empty (" << experimentationName << ") or group is null !";
    }
}


/**
 * @brief Return true if the user can create an experimentations group with the name
 * Check if the name is not empty and if a group with the same name does not already exist
 * @param experimentationsGroupName
 * @return
 */
bool ExperimentationsListController::canCreateExperimentationsGroupWithName(QString experimentationsGroupName)
{
    if (!experimentationsGroupName.isEmpty())
    {
        for (ExperimentationsGroupVM* experimentationsGroup : _allExperimentationsGroups.toList())
        {
            if ((experimentationsGroup != nullptr) && (experimentationsGroup->name() == experimentationsGroupName))
            {
                return false;
            }
        }
        return true;
    }
    else {
        return false;
    }
}
