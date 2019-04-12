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

#ifndef TASKSCONTROLLER_H
#define TASKSCONTROLLER_H

#include <QObject>
#include <I2PropertyHelpers.h>

//#include <controller/ingescapemodelmanager.h>
#include <model/experimentationm.h>


/**
 * @brief The TasksController class defines the controller to manage the tasks of the current experimentation
 */
class TasksController : public QObject
{
    Q_OBJECT

    // Model of the current experimentation
    I2_QML_PROPERTY_READONLY(ExperimentationM*, currentExperimentation)


public:

    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit TasksController(//IngeScapeModelManager* modelManager,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~TasksController();


    /**
     * @brief Return true if the user can create a task with the name
     * Check if the name is not empty and if a task with the same name does not already exist
     * @param taskName
     * @return
     */
    Q_INVOKABLE bool canCreateTaskWithName(QString taskName);


    /**
     * @brief Create a new task with an IngeScape platform file
     * @param taskName
     * @param platformFilePath
     */
    Q_INVOKABLE void createNewTaskWithIngeScapePlatformFile(QString taskName, QString platformFilePath);


Q_SIGNALS:


public Q_SLOTS:


private:

    // Manager for the data model of our IngeScape Assessments application
    //IngeScapeModelManager* _modelManager;

};

QML_DECLARE_TYPE(TasksController)

#endif // TASKSCONTROLLER_H
