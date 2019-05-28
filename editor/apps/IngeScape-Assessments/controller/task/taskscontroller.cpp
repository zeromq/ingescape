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
 * @param jsonHelper
 * @param parent
 */
TasksController::TasksController(//AssessmentsModelManager* modelManager,
                                 //JsonHelper* jsonHelper,
                                 QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr),
    _selectedTask(nullptr)
    //_modelManager(modelManager),
    //_jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Tasks Controller";

    // Fill without type "UNKNOWN"
    _allIndependentVariableValueTypes.fillWithAllEnumValues();
    _allIndependentVariableValueTypes.removeEnumValue(IndependentVariableValueTypes::UNKNOWN);

}


/**
 * @brief Destructor
 */
TasksController::~TasksController()
{
    qInfo() << "Delete Tasks Controller";

    if (_selectedTask != nullptr) {
        setselectedTask(nullptr);
    }

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    // Reset pointers
    //_modelManager = nullptr;
    //_jsonHelper = nullptr;
}


/**
 * @brief Setter for property "Selected Task"
 * @param value
 */
/*void TasksController::setselectedTask(TaskM *value)
{
    if (_selectedTask != value)
    {
        // Previous selected task was defined
        if (_selectedTask != nullptr)
        {

        }

        // Update the selected task
        _selectedTask = value;

        // New selected task is defined
        if (_selectedTask != nullptr)
        {

        }

        Q_EMIT selectedTaskChanged(value);
    }
}*/


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
 * @brief Create a new task with an IngeScape platform file path
 * @param taskName
 * @param platformFilePath
 */
void TasksController::createNewTaskWithIngeScapePlatformFilePath(QString taskName, QString platformFilePath)
{
    if (!taskName.isEmpty() && !platformFilePath.isEmpty())
    {
        qInfo() << "Create new task" << taskName << "with file" << platformFilePath;

        QUrl platformFileUrl = QUrl(platformFilePath);

        if (platformFileUrl.isValid())
        {
            // Create a new task with an IngeScape platform file URL
            _createNewTaskWithIngeScapePlatformFileUrl(taskName, platformFileUrl);
        }
        else {
            qWarning() << "Failed to create the task" << taskName << "because the path" << platformFilePath << "is wrong !";
        }
    }
}


/**
 * @brief Delete a task
 * @param task
 */
void TasksController::deleteTask(TaskM* task)
{
    if ((task != nullptr) && (_currentExperimentation != nullptr))
    {
        if (task == _selectedTask) {
            setselectedTask(nullptr);
        }

        // Remove the task from the current experimentation
        _currentExperimentation->removeTask(task);

        // Free memory
        delete task;
    }
}


/**
 * @brief Duplicate a task
 * @param task
 */
void TasksController::duplicateTask(TaskM* task)
{
    if (task != nullptr)
    {
        QString taskName = QString("%1_copy").arg(task->name());

        // Create a new task with an IngeScape platform file URL
        TaskM* newTask = _createNewTaskWithIngeScapePlatformFileUrl(taskName, task->platformFileUrl());

        if (newTask != nullptr)
        {
            // Copy each independent variables
            for (IndependentVariableM* independentVariable : task->independentVariables()->toList())
            {
                if (independentVariable != nullptr)
                {
                    // Create the new independent variable
                    IndependentVariableM* newIndependentVariable = new IndependentVariableM(independentVariable->name(),
                                                                                            independentVariable->description(),
                                                                                            independentVariable->valueType());

                    // Add the independent variable to the new task
                    newTask->addIndependentVariable(newIndependentVariable);
                }
            }

            // Copy each dependent variables
            for (DependentVariableM* dependentVariable : task->dependentVariables()->toList())
            {
                if (dependentVariable != nullptr)
                {
                    // Create the new dependent variable
                    DependentVariableM* newDependentVariable = new DependentVariableM();

                    newDependentVariable->setname(dependentVariable->name());
                    newDependentVariable->setdescription(dependentVariable->description());
                    newDependentVariable->setagentName(dependentVariable->agentName());
                    newDependentVariable->setoutputName(dependentVariable->outputName());

                    // Add the dependent variable to the new task
                    newTask->addDependentVariable(newDependentVariable);
                }
            }
        }
    }
}


/**
 * @brief Return true if the user can create an independent variable with the name
 * Check if the name is not empty and if a independent variable with the same name does not already exist
 * @param independentVariableName
 * @return
 */
bool TasksController::canCreateIndependentVariableWithName(QString independentVariableName)
{
    if (!independentVariableName.isEmpty() && (_selectedTask != nullptr))
    {
        for (IndependentVariableM* independentVariable : _selectedTask->independentVariables()->toList())
        {
            if ((independentVariable != nullptr) && (independentVariable->name() == independentVariableName))
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
 * @brief Return true if the user can edit an independent variable with the name
 * Check if the name is not empty and if a independent variable with the same name does not already exist
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @return
 */
bool TasksController::canEditIndependentVariableWithName(IndependentVariableM* independentVariableCurrentlyEdited, QString independentVariableName)
{
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (_selectedTask != nullptr))
    {
        for (IndependentVariableM* independentVariable : _selectedTask->independentVariables()->toList())
        {
            if ((independentVariable != nullptr) && (independentVariable != independentVariableCurrentlyEdited) && (independentVariable->name() == independentVariableName))
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
 * @brief Create a new independent variable
 * @param independentVariableName
 * @param independentVariableDescription
 * @param nIndependentVariableValueType
 */
void TasksController::createNewIndependentVariable(QString independentVariableName,
                                                   QString independentVariableDescription,
                                                   int nIndependentVariableValueType)
{
    if (!independentVariableName.isEmpty() && (nIndependentVariableValueType > -1) && (_selectedTask != nullptr))
    {
        IndependentVariableValueTypes::Value independentVariableValueType = static_cast<IndependentVariableValueTypes::Value>(nIndependentVariableValueType);

        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(independentVariableValueType);

        // Create the new independent variable
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, independentVariableValueType);

        // Add the independent variable to the selected task
        _selectedTask->addIndependentVariable(independentVariable);
    }
}


/**
 * @brief Create a new independent variable of type enum
 * @param independentVariableName
 * @param independentVariableDescription
 * @param enumValues
 */
void TasksController::createNewIndependentVariableEnum(QString independentVariableName,
                                                       QString independentVariableDescription,
                                                       QStringList enumValues)
{
    if (!independentVariableName.isEmpty() && !enumValues.isEmpty() && (_selectedTask != nullptr))
    {
        qInfo() << "Create new independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM) << "with values:" << enumValues;

        // Create the new independent variable
        IndependentVariableM* independentVariable = new IndependentVariableM(independentVariableName, independentVariableDescription, IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
        independentVariable->setenumValues(enumValues);

        // Add the independent variable to the selected task
        _selectedTask->addIndependentVariable(independentVariable);
    }
}


/**
 * @brief Save the modifications of the Independent Variable currently edited
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @param independentVariableDescription
 * @param nIndependentVariableValueType
 */
void TasksController::saveModificationsOfIndependentVariable(IndependentVariableM* independentVariableCurrentlyEdited,
                                                             QString independentVariableName,
                                                             QString independentVariableDescription,
                                                             int nIndependentVariableValueType)
{
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (nIndependentVariableValueType > -1) && (_selectedTask != nullptr))
    {
        IndependentVariableValueTypes::Value independentVariableValueType = static_cast<IndependentVariableValueTypes::Value>(nIndependentVariableValueType);

        qInfo() << "Edit independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(independentVariableValueType);

        independentVariableCurrentlyEdited->setname(independentVariableName);
        independentVariableCurrentlyEdited->setdescription(independentVariableDescription);
        independentVariableCurrentlyEdited->setvalueType(independentVariableValueType);
        independentVariableCurrentlyEdited->setenumValues(QStringList());
    }
}


/**
 * @brief Save the modifications of the Independent Variable (of type enum) currently edited
 * @param independentVariableCurrentlyEdited
 * @param independentVariableName
 * @param independentVariableDescription
 * @param enumValues
 */
void TasksController::saveModificationsOfIndependentVariableEnum(IndependentVariableM* independentVariableCurrentlyEdited,
                                                                 QString independentVariableName,
                                                                 QString independentVariableDescription,
                                                                 QStringList enumValues)
{
    if ((independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && !enumValues.isEmpty() && (_selectedTask != nullptr))
    {
        qInfo() << "Edit independent variable" << independentVariableName << "of type" << IndependentVariableValueTypes::staticEnumToString(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM) << "with values:" << enumValues;

        independentVariableCurrentlyEdited->setname(independentVariableName);
        independentVariableCurrentlyEdited->setdescription(independentVariableDescription);
        independentVariableCurrentlyEdited->setvalueType(IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM);
        independentVariableCurrentlyEdited->setenumValues(enumValues);
    }
}


/**
 * @brief Delete an independent variable
 * @param independentVariable
 */
void TasksController::deleteIndependentVariable(IndependentVariableM* independentVariable)
{
    if ((independentVariable != nullptr) && (_selectedTask != nullptr))
    {
        // Remove the independent variable from the selected task
        _selectedTask->removeIndependentVariable(independentVariable);

        // Free memory
        delete independentVariable;
    }
}


/**
 * @brief Create a new dependent variable
 */
void TasksController::createNewDependentVariable()
{
    if (_selectedTask != nullptr)
    {
        qDebug() << "Create a new dependent variable";

        DependentVariableM* dependentVariable = new DependentVariableM();

        dependentVariable->setname("Dep. Var.");
        //dependentVariable->setdescription("");
        //dependentVariable->setagentName("");
        //dependentVariable->setoutputName("");

        // Add the dependent variable to the selected task
        _selectedTask->addDependentVariable(dependentVariable);
    }
}


/**
 * @brief Delete an dependent variable
 * @param dependentVariable
 */
void TasksController::deleteDependentVariable(DependentVariableM* dependentVariable)
{
    if ((dependentVariable != nullptr) && (_selectedTask != nullptr))
    {
        // Remove the dependent variable from the selected task
        _selectedTask->removeDependentVariable(dependentVariable);

        // Free memory
        delete dependentVariable;
    }
}


/**
 * @brief Create a new task with an IngeScape platform file URL
 * @param taskName
 * @param platformFileUrl
 * @return
 */
TaskM* TasksController::_createNewTaskWithIngeScapePlatformFileUrl(QString taskName, QUrl platformFileUrl)
{
    TaskM* task = nullptr;

    if (!taskName.isEmpty() && platformFileUrl.isValid() && (_currentExperimentation != nullptr))
    {
        // Create the new task
        task = new TaskM(taskName);

        // Set the URL of the IngeScape platform file (JSON)
        task->setplatformFileUrl(platformFileUrl);

        // Add the task to the current experimentation
        _currentExperimentation->addTask(task);

        // Select this new task
        setselectedTask(task);
    }

    return task;
}
