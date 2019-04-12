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

#include "taskscontroller.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
TasksController::TasksController(//IngeScapeModelManager* modelManager,
                                 QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr)
    //_modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Tasks Controller";

}


/**
 * @brief Destructor
 */
TasksController::~TasksController()
{
    qInfo() << "Delete Tasks Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    /*if (_modelManager != nullptr)
    {
        //disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
    }*/
}


/**
 * @brief Return true if the user can create a task with the name
 * Check if the name is not empty and if a task with the same name does not already exist
 * @param taskName
 * @return
 */
bool TasksController::canCreateTaskWithName(QString taskName)
{
    if (!taskName.isEmpty() && (_currentExperimentation != nullptr))
    {
        for (TaskM* task : _currentExperimentation->allTasks()->toList())
        {
            if ((task != nullptr) && (task->name() == taskName))
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
 * @brief Create a new task with an IngeScape platform file
 * @param taskName
 * @param platformFilePath
 */
void TasksController::createNewTaskWithIngeScapePlatformFile(QString taskName, QString platformFilePath)
{
    if (!taskName.isEmpty() && !platformFilePath.isEmpty() && (_currentExperimentation != nullptr))
    {
        qInfo() << "Create new task" << taskName << "with file" << platformFilePath;

        QUrl platformFileUrl = QUrl(platformFilePath);

        if (platformFileUrl.isValid())
        {
            // Create the new task
            TaskM* task = new TaskM(taskName);

            // FIXME: use the platformFileUrl

            // Add the task to the current experimentation
            _currentExperimentation->addTask(task);
        }
        else {
            qWarning() << "Failed to create the task" << taskName << "because the URL" << platformFilePath << "is wrong !";
        }
    }
}
