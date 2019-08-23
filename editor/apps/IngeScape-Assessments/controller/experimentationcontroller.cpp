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

#include "experimentationcontroller.h"

#include <ctime>

#include <controller/assessmentsmodelmanager.h>
#include "model/subject/characteristicvaluem.h"
#include "model/task/independentvariablevaluem.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
ExperimentationController::ExperimentationController(JsonHelper* jsonHelper,
                                                     QObject *parent) : QObject(parent),
    _taskInstanceC(nullptr),
    _currentExperimentation(nullptr),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    // Create the controller to manage a record of the current experimentation
    _taskInstanceC = new TaskInstanceController(_jsonHelper, this);
}


/**
 * @brief Destructor
 */
ExperimentationController::~ExperimentationController()
{
    qInfo() << "Delete Experimentation Controller";

    // Reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    if (_taskInstanceC != nullptr)
    {
        disconnect(_taskInstanceC);

        TaskInstanceController* temp = _taskInstanceC;
        settaskInstanceC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Reset pointers
    _jsonHelper = nullptr;
}


/**
 * @brief Setter for property "Current Experimentation"
 * @param value
 */
void ExperimentationController::setcurrentExperimentation(ExperimentationM *value)
{
    if (_currentExperimentation != value)
    {
        if (_currentExperimentation != nullptr)
        {
            // Unload any previous experimentation
            _currentExperimentation->clearData();
        }

        _currentExperimentation = value;

        // Manage changes
        _onCurrentExperimentationChanged(_currentExperimentation);

        Q_EMIT currentExperimentationChanged(value);
    }
}


/**
 * @brief Create a new task instance for a subject and a task
 * @param recordName
 * @param subject
 * @param task
 */
void ExperimentationController::createNewTaskInstanceForSubjectAndTask(QString taskInstanceName, SubjectM* subject, TaskM* task)
{
    if (!taskInstanceName.isEmpty() && (subject != nullptr) && (task != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Create new record" << taskInstanceName << "for subject" << subject->displayedId() << "and task" << task->name();

        // Create a new (experimentation) task instance
        TaskInstanceM* taskInstance = _insertTaskInstanceIntoDB(taskInstanceName, subject, task);

        if (taskInstance != nullptr)
        {
            // Add the task insatnce to the current experimentation
            _currentExperimentation->addTaskInstance(taskInstance);

            // Open this new task instance
            openTaskInstance(taskInstance);
        }
    }
}


/**
 * @brief Open a task instance
 * @param record
 */
void ExperimentationController::openTaskInstance(TaskInstanceM* taskInstance)
{
    if ((taskInstance != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Open the record" << taskInstance->name() << "of the experimentation" << _currentExperimentation->name();

        // Update the current task instance
        _taskInstanceC->setcurrentTaskInstance(taskInstance);
    }
}


/**
 * @brief Delete a task instance
 * @param record
 */
void ExperimentationController::deleteTaskInstance(TaskInstanceM* taskInstance)
{
    if ((taskInstance != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Delete the record" << taskInstance->name() << "of the experimentation" << _currentExperimentation->name();

        // Delete task instance from DB
        TaskInstanceM::deleteTaskInstanceFromCassandra(*taskInstance);

        // Remove the task instance from the current experimentation
        _currentExperimentation->removeTaskInstance(taskInstance);

        // Free memory
        delete taskInstance;
    }
}


/**
 * @brief Slot called when the current experimentation changed
 * @param currentExperimentation
 */
void ExperimentationController::_onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if (currentExperimentation != nullptr && AssessmentsModelManager::Instance() != nullptr)
    {
        qDebug() << "_on Current Experimentation Changed" << currentExperimentation->name();

        _retrieveTasksForExperimentation(currentExperimentation);

        _retrieveSubjectsForExperimentation(currentExperimentation);

        _retrieveCharacteristicsForExperimentation(currentExperimentation);

        _retrieveCharacteristicValuesForSubjectsInExperimentation(currentExperimentation);

        _retrieveTaskInstancesForExperimentation(currentExperimentation);
    }
}

/**
 * @brief Create and insert a new task instance into the DB.
 * A nullptr is returned if the task instance could not be created
 * @param recordName
 * @param subject
 * @param task
 * @return
 */
TaskInstanceM* ExperimentationController::_insertTaskInstanceIntoDB(const QString& taskInstanceName, SubjectM* subject, TaskM* task)
{
    TaskInstanceM* taskInstance = nullptr;

    if ((_currentExperimentation != nullptr) && (subject != nullptr) && (task != nullptr))
    {
        // Create the new task instance
        taskInstance = new TaskInstanceM(_currentExperimentation->getCassUuid(), AssessmentsModelManager::genCassUuid(), taskInstanceName, "", subject->getCassUuid(), task->getCassUuid(), QDateTime::currentDateTime());
        if (taskInstance != nullptr)
        {
            taskInstance->settask(task);
            taskInstance->setsubject(subject);

            if (AssessmentsModelManager::insert(*taskInstance))
            {
                for (auto indeVarIt = task->independentVariables()->begin() ; indeVarIt != task->independentVariables()->end() ; ++indeVarIt)
                {
                    IndependentVariableM* independentVar = *indeVarIt;
                    if (independentVar != nullptr)
                    {
                        // Insert an instance of every independent variable for this task instance into DB
                        QString innerQueryString = "INSERT INTO " + IndependentVariableValueM::table + " (id_experimentation, id_task_instance, id_independent_var, independent_var_value) VALUES (?, ?, ?, ?);";
                        CassStatement* innerCassStatement = cass_statement_new(innerQueryString.toStdString().c_str(), 4);
                        cass_statement_bind_uuid  (innerCassStatement, 0, subject->getExperimentationCassUuid());
                        cass_statement_bind_uuid  (innerCassStatement, 1, taskInstance->getCassUuid());
                        cass_statement_bind_uuid  (innerCassStatement, 2, independentVar->getCassUuid());
                        cass_statement_bind_string(innerCassStatement, 3, "");

                        // Execute the query or bound statement
                        CassFuture* innerCassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), innerCassStatement);
                        CassError innerCassError = cass_future_error_code(innerCassFuture);
                        if (innerCassError == CASS_OK)
                        {
                            qInfo() << "New independent value for task_instance inserted into the DB";
                        }
                        else {
                            qCritical() << "Could not insert the new independent value for task_instance" << taskInstanceName << "into the DB:" << cass_error_desc(innerCassError);
                        }

                        cass_statement_free(innerCassStatement);
                        cass_future_free(innerCassFuture);
                    }
                }
            }
        }
    }

    return taskInstance;
}

/**
 * @brief Retrieve all independent variables from the Cassandra DB for the given task.
 * The task will be updated by this method
 * @param task
 */
void ExperimentationController::_retrieveIndependentVariableForTask(TaskM* task)
{
    if (AssessmentsModelManager::Instance() != nullptr)
    {
        QList<IndependentVariableM*> indeVarList = AssessmentsModelManager::select<IndependentVariableM>({ task->getExperimentationCassUuid(), task->getCassUuid() });
        for (IndependentVariableM* independentVariable : indeVarList) {
            task->addIndependentVariable(independentVariable);
        }
    }
}

/**
 * @brief Retrieve all dependent variables from the Cassandra DB for the given task.
 * The task will be updated by this method.
 * @param task
 */
void ExperimentationController::_retrieveDependentVariableForTask(TaskM* task)
{
    if ((task != nullptr) && (AssessmentsModelManager::Instance() != nullptr))
    {
        QList<DependentVariableM*> depVarList = AssessmentsModelManager::select<DependentVariableM>({ task->getExperimentationCassUuid(), task->getCassUuid() });
        for (DependentVariableM* dependentVariable : depVarList) {
            task->addDependentVariable(dependentVariable);
        }
    }
}


/**
 * @brief Retrieve all subjects from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveSubjectsForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<SubjectM*> subejctList = AssessmentsModelManager::select<SubjectM>({ experimentation->getCassUuid() });
        for (SubjectM* subject : subejctList) {
            experimentation->addSubject(subject);
        }
    }
}


/**
 * @brief Retrieve all characteristics from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveCharacteristicsForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<CharacteristicM*> characteristicList = AssessmentsModelManager::select<CharacteristicM>({ experimentation->getCassUuid() });
        for (CharacteristicM* characteristic : characteristicList) {
            experimentation->addCharacteristic(characteristic);
        }
    }
}


/**
 * @brief Retrieve all tasks from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveTasksForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<TaskM*> taskList = AssessmentsModelManager::select<TaskM>({ experimentation->getCassUuid() });
        for (TaskM* task : taskList) {
            // Independent variables
            _retrieveIndependentVariableForTask(task);

            // Dependent variables
            _retrieveDependentVariableForTask(task);

            // Add the task to the experimentation
            experimentation->addTask(task);
        }
    }
}

/**
 * @brief Retrieve all task instances from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveTaskInstancesForExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<TaskInstanceM*> taskInstanceList = AssessmentsModelManager::select<TaskInstanceM>({ experimentation->getCassUuid() });
        for (TaskInstanceM* taskInstance : taskInstanceList) {
            experimentation->addTaskInstance(taskInstance);

            // Set pointers to Task & Subject
            taskInstance->settask(experimentation->getTaskFromUID(taskInstance->getTaskCassUuid()));
            taskInstance->setsubject(experimentation->getSubjectFromUID(taskInstance->getSubjectCassUuid()));

            // Retrieve independent variables values for the task instance
            _retrieveIndependentVariableValuesForTaskInstance(taskInstance);
        }
    }
}

/**
 * @brief Retrieve all characteristic values from the Cassandra DB for each subjects in the given experimentation
 * The subjects will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveCharacteristicValuesForSubjectsInExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        for (auto subjectIt = experimentation->allSubjects()->begin() ; subjectIt != experimentation->allSubjects()->end() ; ++subjectIt)
        {
            SubjectM* subject = *subjectIt;
            if (subject != nullptr)
            {
                QString queryStr = "SELECT * FROM " + CharacteristicValueM::table + " WHERE id_experimentation = ? AND id_subject = ?;";
                // Creates the new query statement
                CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 2);
                cass_statement_bind_uuid(cassStatement, 0, experimentation->getCassUuid());
                cass_statement_bind_uuid(cassStatement, 1, subject->getCassUuid());

                // Execute the query or bound statement
                CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
                CassError cassError = cass_future_error_code(cassFuture);
                if (cassError == CASS_OK)
                {
                    qDebug() << "Get all characteristics values for subject" << subject->displayedId() << "succeeded";

                    // Retrieve result set and iterate over the rows
                    const CassResult* cassResult = cass_future_get_result(cassFuture);

                    if (cassResult != nullptr)
                    {
                        CassIterator* cassIterator = cass_iterator_from_result(cassResult);

                        while(cass_iterator_next(cassIterator))
                        {
                            const CassRow* row = cass_iterator_get_row(cassIterator);

                            // Get characteristic id
                            CassUuid characteristicUuid;
                            cass_value_get_uuid(cass_row_get_column_by_name(row, "id_characteristic"), &characteristicUuid);

                            // Get characteristic value as a string
                            QString valueString = AssessmentsModelManager::getStringValueFromColumnName(row, "characteristic_value");

                            // Get characteristic value type
                            CharacteristicM* characteristic = _currentExperimentation->getCharacteristicFromUID(characteristicUuid);
                            if (characteristic != nullptr)
                            {
                                switch (characteristic->valueType()) {
                                    case CharacteristicValueTypes::INTEGER:
                                        subject->setCharacteristicValue(characteristic, valueString.toInt());
                                        break;
                                    case CharacteristicValueTypes::DOUBLE:
                                        subject->setCharacteristicValue(characteristic, valueString.toDouble());
                                        break;
                                    case CharacteristicValueTypes::TEXT:
                                        subject->setCharacteristicValue(characteristic, valueString);
                                        break;
                                    case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                                        subject->setCharacteristicValue(characteristic, valueString);
                                        break;
                                    default: // UNKNOWN
                                        break;
                                }
                            }

                        }

                        cass_iterator_free(cassIterator);
                    }
                }
                else {
                    qCritical() << "Could not get all characteristic values for the subjects of experiment" << experimentation->name() << "from the database:" << cass_error_desc(cassError);
                }

                cass_future_free(cassFuture);
                cass_statement_free(cassStatement);
            }
        }
    }
}

/**
 * @brief Retrieve all independent variable values Cassandra DB for the given task instance.
 * The task instance will be updated by this method
 * @param taskInstance
 */
void ExperimentationController::_retrieveIndependentVariableValuesForTaskInstance(TaskInstanceM* taskInstance)
{
    if ((taskInstance != nullptr) && (taskInstance->task() != nullptr)) {

        QString queryStr = "SELECT * FROM " + IndependentVariableValueM::table + " WHERE id_experimentation = ? AND id_task_instance = ?;";
        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(queryStr.toStdString().c_str(), 2);
        cass_statement_bind_uuid(cassStatement, 0, taskInstance->task()->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, taskInstance->getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qDebug() << "Get all independent variables values for task instance" << taskInstance->name() << "succeeded";

            // Retrieve result set and iterate over the rows
            const CassResult* cassResult = cass_future_get_result(cassFuture);

            if (cassResult != nullptr)
            {
                CassIterator* cassIterator = cass_iterator_from_result(cassResult);

                while(cass_iterator_next(cassIterator))
                {
                    const CassRow* row = cass_iterator_get_row(cassIterator);

                    // Get independent variable uuid
                    CassUuid indeVarUuid;
                    cass_value_get_uuid(cass_row_get_column_by_name(row, "id_independent_var"), &indeVarUuid);

                    // Get corresponding independent variable
                    IndependentVariableM* indeVar = taskInstance->task()->getIndependentVariableFromUuid(indeVarUuid);
                    if (indeVar != nullptr)
                    {
                        // Get value as a string
                        QString valueString = AssessmentsModelManager::getStringValueFromColumnName(row, "independent_var_value");

                        taskInstance->setIndependentVariableValue(indeVar, valueString);
                    }
                }

                cass_iterator_free(cassIterator);
            }
        }
        else {
            qCritical() << "Could not get all independent variable values for the task instance" << taskInstance->name() << "from the database:" << cass_error_desc(cassError);
        }

        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}
