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

#include "experimentationslistcontroller.h"

/**
 * @brief Constructor
 * @param parent
 */
ExperimentationsListController::ExperimentationsListController(AssessmentsModelManager* modelManager,
                                                               QObject *parent) : QObject(parent),
    _defaultGroupOther(nullptr),
    _newGroup(nullptr),
    _modelManager(modelManager)
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
    int nbExpes = 1;

    for (int i = 0; i < 3; i++)
    {
        QString groupName = QString("Group for test %1").arg(i + 1);

        ExperimentationsGroupVM* group = new ExperimentationsGroupVM(groupName, nullptr);

        _allExperimentationsGroups.append(group);

        for (int j = 0; j < 2; j++)
        {
            QString expeName = QString("Expe for test %1").arg(nbExpes);

            ExperimentationM* experimentation = new ExperimentationM(expeName, QDateTime::currentDateTime(), nullptr);

            group->experimentations()->append(experimentation);

            nbExpes++;
        }
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

    if (_modelManager != nullptr)
    {
        _modelManager = nullptr;
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


/**
 * @brief Open an experimentation of a group
 * @param experimentation
 */
void ExperimentationsListController::openExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup)
{
    if ((experimentation != nullptr) && (experimentationsGroup != nullptr))
    {
        qInfo() << "Open the experimentation" << experimentation->name() << "of the group" << experimentationsGroup->name();

        // Update the model manager
        if (_modelManager != nullptr)
        {
            _modelManager->setcurrentExperimentation(experimentation);
            _modelManager->setcurrentExperimentationsGroup(experimentationsGroup);
        }
    }
}


/**
 * @brief Delete an experimentation of a group
 * @param experimentation
 * @param experimentationsGroup
 */
void ExperimentationsListController::deleteExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup)
{
    if ((experimentation != nullptr) && (experimentationsGroup != nullptr))
    {
        qInfo() << "Delete the experimentation" << experimentation->name() << "of the group" << experimentationsGroup->name();

        // Remove from the group
        experimentationsGroup->experimentations()->remove(experimentation);

        // Free memory
        delete experimentation;
    }
}
