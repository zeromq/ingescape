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
TasksController::TasksController(AssessmentsModelManager* modelManager, /*JsonHelper* jsonHelper, */QObject *parent)
    : QObject(parent)
    , _currentExperimentation(nullptr)
    , _selectedTask(nullptr)
    , _modelManager(modelManager)
    //, _jsonHelper(jsonHelper)
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
    _modelManager = nullptr;
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
    //NOTE Oneliner
//    const QList<TaskM*>& taskList = _currentExperimentation->allTasks()->toList();
//    return !taskName.isEmpty() && (_currentExperimentation != nullptr)
//            && std::none_of(taskList.begin(), taskList.end(),
//                            [taskName](IndependentVariableM* task){
//        return (task != nullptr) && (task->name() == taskName);
//    });

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

        //TODO Remove dependent variables from DB

        // Remove independent_var from DB
        const char* query = "DELETE FROM ingescape.independent_var WHERE id_experimentation = ? AND id_task = ?;";
        CassStatement* cassStatement = cass_statement_new(query, 2);
        cass_statement_bind_uuid(cassStatement, 0, task->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, task->getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Independent variables for task" << task->name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the task" << task->name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

        // Remove task from DB
        query = "DELETE FROM ingescape.task WHERE id_experimentation = ? AND id = ?;";
        cassStatement = cass_statement_new(query, 2);
        cass_statement_bind_uuid(cassStatement, 0, task->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, task->getCassUuid());

        // Execute the query or bound statement
        cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
        cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Task" << task->name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the task" << task->name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

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
                    IndependentVariableM* newIndependentVariable = _insertIndependentVariableIntoDB(newTask->getExperimentationCassUuid()
                                                                                                    , newTask->getCassUuid()
                                                                                                    , independentVariable->name()
                                                                                                    , independentVariable->description()
                                                                                                    , independentVariable->valueType()
                                                                                                    , independentVariable->enumValues()
                                                                                                    );

                    if (newIndependentVariable != nullptr)
                    {
                        // Add the independent variable to the new task
                        newTask->addIndependentVariable(newIndependentVariable);
                    }
                }
            }

            // Copy each dependent variables
            for (DependentVariableM* dependentVariable : task->dependentVariables()->toList())
            {
                if (dependentVariable != nullptr)
                {
                    // Create the new dependent variable
                    DependentVariableM* newDependentVariable = _insertDependentVariableIntoDB(newTask->getExperimentationCassUuid()
                                                                                              , newTask->getCassUuid()
                                                                                              , dependentVariable->name()
                                                                                              , dependentVariable->description()
                                                                                              , dependentVariable->agentName()
                                                                                              , dependentVariable->outputName()
                                                                                              );

                    if (newDependentVariable != nullptr)
                    {
                        // Add the dependent variable to the new task
                        newTask->addDependentVariable(newDependentVariable);
                    }

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
    //NOTE Oneliner
//    const QList<IndependentVariableM*>& varList = _selectedTask->independentVariables()->toList();
//    return !independentVariableName.isEmpty() && (_selectedTask != nullptr)
//            && std::none_of(varList.begin(), varList.end(),
//                            [independentVariableName](IndependentVariableM* independentVariable){
//        return (independentVariable != nullptr) && (independentVariable->name() == independentVariableName);
//    });

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
    //NOTE Oneliner
//    const QList<IndependentVariableM*>& varList = _selectedTask->independentVariables()->toList();
//    return (independentVariableCurrentlyEdited != nullptr) && !independentVariableName.isEmpty() && (_selectedTask != nullptr)
//            && std::none_of(varList.begin(), varList.end(),
//                            [independentVariableName, independentVariableCurrentlyEdited](IndependentVariableM* independentVariable){
//        return (independentVariable != nullptr) && (independentVariable != independentVariableCurrentlyEdited) && (independentVariable->name() == independentVariableName);
//    });

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

        // Create and insert the new independent variable
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), independentVariableName, independentVariableDescription, independentVariableValueType, {});
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected task
            _selectedTask->addIndependentVariable(independentVariable);
        }
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
        IndependentVariableM* independentVariable = _insertIndependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), independentVariableName, independentVariableDescription, IndependentVariableValueTypes::INDEPENDENT_VARIABLE_ENUM, enumValues);
        if (independentVariable != nullptr)
        {
            // Add the independent variable to the selected task
            _selectedTask->addIndependentVariable(independentVariable);
        }
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
        // Remove independent variable from DB
        const char* query = "DELETE FROM ingescape.independent_var WHERE id_experimentation = ? AND id_task = ? AND id = ?;";
        CassStatement* cassStatement = cass_statement_new(query, 3);
        cass_statement_bind_uuid(cassStatement, 0, independentVariable->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, independentVariable->getTaskCassUuid());
        cass_statement_bind_uuid(cassStatement, 2, independentVariable->getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Independent variable" << independentVariable->name() << "has been successfully deleted from the DB";
        }
        else {
            qCritical() << "Could not delete the independent variable" << independentVariable->name() << "from the DB:" << cass_error_desc(cassError);
        }

        // Clean-up cassandra objects
        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);

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

        CassUuid dependentVarUuid;
        cass_uuid_gen_time(_modelManager->getCassUuidGen(), &dependentVarUuid);

        DependentVariableM* dependentVariable = _insertDependentVariableIntoDB(_selectedTask->getExperimentationCassUuid(), _selectedTask->getCassUuid(), "Dep. Var.", "", "", "");

        if (dependentVariable != nullptr)
        {
            // Add the dependent variable to the selected task
            _selectedTask->addDependentVariable(dependentVariable);
        }
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
        CassUuid taskUuid;
        cass_uuid_gen_time(_modelManager->getCassUuidGen(), &taskUuid);

        const char* query = "INSERT INTO ingescape.task (id_experimentation, id, name, platform_file) VALUES (?, ?, ?, ?);";
        CassStatement* cassStatement = cass_statement_new(query, 4);
        cass_statement_bind_uuid(cassStatement, 0, _currentExperimentation->getCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, taskUuid);
        cass_statement_bind_string(cassStatement, 2, taskName.toStdString().c_str());
        cass_statement_bind_string(cassStatement, 3, platformFileUrl.toString().toStdString().c_str());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Task" << taskName << "inserted into the DataBase";

            // Create the new task
            task = new TaskM(_currentExperimentation->getCassUuid(), taskUuid, taskName, platformFileUrl);

        }
        else {
            qCritical() << "Could not insert the task" << taskName << "into the DataBase:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);

        // Add the task to the current experimentation
        _currentExperimentation->addTask(task);

        // Select this new task
        setselectedTask(task);
    }
    else {
        qWarning() << "Cannot create new task because name is empty (" << task->name() << ") or group is null !";
    }

    return task;
}


/**
 * @brief Creates a new independent variable with the given parameters and insert it into the Cassandra DB
 * A nullptr is returned if the operation failed.
 * @param experimentationUuid
 * @param taskUuid
 * @param name
 * @param description
 * @param valueType
 * @param enumValues
 * @return
 */
IndependentVariableM* TasksController::_insertIndependentVariableIntoDB(CassUuid experimentationUuid, CassUuid taskUuid, const QString& variableName, const QString& variableDescription, IndependentVariableValueTypes::Value valueType, const QStringList& enumValues)
{
    IndependentVariableM* independentVariable = nullptr;

    if (!variableName.isEmpty())
    {
        CassUuid independentVarUuid;
        cass_uuid_gen_time(_modelManager->getCassUuidGen(), &independentVarUuid);

        const char* query = "INSERT INTO ingescape.independent_var (id_experimentation, id_task, id, name, description, value_type, enum_values) VALUES (?, ?, ?, ?, ?, ?, ?);";
        CassStatement* cassStatement = cass_statement_new(query, 7);
        cass_statement_bind_uuid  (cassStatement, 0, experimentationUuid);
        cass_statement_bind_uuid  (cassStatement, 1, taskUuid);
        cass_statement_bind_uuid  (cassStatement, 2, independentVarUuid);
        cass_statement_bind_string(cassStatement, 3, variableName.toStdString().c_str());
        cass_statement_bind_string(cassStatement, 4, variableDescription.toStdString().c_str());
        cass_statement_bind_int8  (cassStatement, 5, static_cast<int8_t>(valueType));
        cass_statement_bind_string(cassStatement, 6, enumValues.join(";").toStdString().c_str());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "Independent variable" << variableName << "inserted into the DB";

            // Create the new task
            independentVariable = new IndependentVariableM(experimentationUuid, taskUuid, independentVarUuid, variableName, variableDescription, valueType, enumValues);

        }
        else {
            qCritical() << "Could not insert the independent variable" << variableName << "into the DB:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);
    }

    return independentVariable;
}

DependentVariableM* TasksController::_insertDependentVariableIntoDB(CassUuid experimentationUuid, CassUuid taskUuid, const QString& name, const QString& description, const QString& agentName, const QString& outputName)
{
    DependentVariableM* dependentVariable = nullptr;

    CassUuid dependentVarUuid;
    cass_uuid_gen_time(_modelManager->getCassUuidGen(), &dependentVarUuid);

    const char* query = "INSERT INTO ingescape.dependent_var (id_experimentation, id_task, id, name, description, agent_name, output_name) VALUES (?, ?, ?, ?, ?, ?, ?);";
    CassStatement* cassStatement = cass_statement_new(query, 7);
    cass_statement_bind_uuid  (cassStatement, 0, experimentationUuid);
    cass_statement_bind_uuid  (cassStatement, 1, taskUuid);
    cass_statement_bind_uuid  (cassStatement, 2, dependentVarUuid);
    cass_statement_bind_string(cassStatement, 3, name.toStdString().c_str());
    cass_statement_bind_string(cassStatement, 4, description.toStdString().c_str());
    cass_statement_bind_string(cassStatement, 5, agentName.toStdString().c_str());
    cass_statement_bind_string(cassStatement, 6, outputName.toStdString().c_str());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qInfo() << "New dependent variable inserted into the DB";

        // Create the new task
        dependentVariable = new DependentVariableM(experimentationUuid, taskUuid, dependentVarUuid, name, description, agentName, outputName);

    }
    else {
        qCritical() << "Could not insert the new dependent variable into the DB:" << cass_error_desc(cassError);
    }

    cass_statement_free(cassStatement);
    cass_future_free(cassFuture);

    return dependentVariable;
}
