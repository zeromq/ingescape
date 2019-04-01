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

#ifndef EXPERIMENTATIONSLISTCONTROLLER_H
#define EXPERIMENTATIONSLISTCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

#include <controller/ingescapemodelmanager.h>
#include <viewModel/experimentationsgroupvm.h>


/**
 * @brief The ExperimentationsListController class defines the controller to manage the list of experimentations
 */
class ExperimentationsListController : public QObject
{
    Q_OBJECT

    // List of all groups of experimentations
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(ExperimentationsGroupVM, allExperimentationsGroups)

    // Default group "Other"
    I2_QML_PROPERTY_READONLY(ExperimentationsGroupVM*, defaultGroupOther)

    // (Fake) group "New"
    I2_QML_PROPERTY_READONLY(ExperimentationsGroupVM*, newGroup)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit ExperimentationsListController(IngeScapeModelManager* modelManager,
                                            QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ExperimentationsListController();


    /**
     * @brief Create a new experimentation in new group (to create)
     * @param experimentationName
     * @param newExperimentationsGroupName
     */
    Q_INVOKABLE void createNewExperimentationInNewGroup(QString experimentationName, QString newExperimentationsGroupName);


    /**
     * @brief Create a new experimentation in a group
     * @param experimentationName
     * @param experimentationsGroup
     */
    Q_INVOKABLE void createNewExperimentationInGroup(QString experimentationName, ExperimentationsGroupVM* experimentationsGroup);


    /**
     * @brief Return true if the user can create an experimentations group with the name
     * Check if the name is not empty and if a group with the same name does not already exist
     * @param experimentationsGroupName
     * @return
     */
    Q_INVOKABLE bool canCreateExperimentationsGroupWithName(QString experimentationsGroupName);


    /**
     * @brief Open an experimentation of a group
     * @param experimentation
     * @param experimentationsGroup
     */
    Q_INVOKABLE void openExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup);


    /**
     * @brief Delete an experimentation of a group
     * @param experimentation
     * @param experimentationsGroup
     */
    Q_INVOKABLE void deleteExperimentationOfGroup(ExperimentationM* experimentation, ExperimentationsGroupVM* experimentationsGroup);


Q_SIGNALS:


public Q_SLOTS:


private:

    // Manager for the data model of our IngeScape measuring application
    IngeScapeModelManager* _modelManager;

};

QML_DECLARE_TYPE(ExperimentationsListController)

#endif // EXPERIMENTATIONSLISTCONTROLLER_H
