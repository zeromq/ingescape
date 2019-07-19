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

#include <controller/assessmentsmodelmanager.h>

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
ExperimentationController::ExperimentationController(JsonHelper* jsonHelper,
                                                     QObject *parent) : QObject(parent),
    _recordC(nullptr),
    _currentExperimentation(nullptr),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    // Create the controller to manage a record of the current experimentation
    _recordC = new RecordController(_jsonHelper, this);
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

    if (_recordC != nullptr)
    {
        disconnect(_recordC);

        RecordController* temp = _recordC;
        setrecordC(nullptr);
        delete temp;
        temp = nullptr;
    }

    /*if (_modelManager != nullptr)
    {
        disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
    }*/

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
        _currentExperimentation = value;

        // Manage changes
        _onCurrentExperimentationChanged(_currentExperimentation);

        Q_EMIT currentExperimentationChanged(value);
    }
}


/**
 * @brief Create a new record for a subject and a task
 * @param recordName
 * @param subject
 * @param task
 */
void ExperimentationController::createNewRecordForSubjectAndTask(QString recordName, SubjectM* subject, TaskM* task)
{
    if (!recordName.isEmpty() && (subject != nullptr) && (task != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Create new record" << recordName << "for subject" << subject->displayedId() << "and task" << task->name();

        QDateTime now = QDateTime::currentDateTime();

        QString recordUID = now.toString("R-yyMMdd-hhmmss-zzz");

        // Create a new (experimentation) record setup
        RecordSetupM* recordSetup = new RecordSetupM(recordUID, recordName, subject, task, now, nullptr);

        // Add the record to the current experimentation
        _currentExperimentation->addRecordSetup(recordSetup);

        // Open this new record
        openRecordSetup(recordSetup);
    }
}


/**
 * @brief Open a record setup
 * @param record
 */
void ExperimentationController::openRecordSetup(RecordSetupM* recordSetup)
{
    if ((recordSetup != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Open the record" << recordSetup->name() << "of the experimentation" << _currentExperimentation->name();

        // Update the current record setup
        _recordC->setcurrentRecordSetup(recordSetup);
    }
}


/**
 * @brief Delete a record setup
 * @param record
 */
void ExperimentationController::deleteRecordSetup(RecordSetupM* recordSetup)
{
    if ((recordSetup != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Delete the record" << recordSetup->name() << "of the experimentation" << _currentExperimentation->name();

        // Remove the record from the current experimentation
        _currentExperimentation->removeRecordSetup(recordSetup);

        // Free memory
        delete recordSetup;
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

        // FIXME TODO: load data about this experimentation (subjects, tasks, ...)

        const char* query = "SELECT * FROM ingescape.task WHERE id_experimentation = ?;";

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(query, 1);
        cass_statement_bind_uuid(cassStatement, 0, currentExperimentation->getCassUuid());

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qDebug() << "Get all tasks succeeded";

            // Retrieve result set and iterate over the rows
            const CassResult* cassResult = cass_future_get_result(cassFuture);

            if (cassResult != nullptr)
            {
                CassIterator* cassIterator = cass_iterator_from_result(cassResult);

                while(cass_iterator_next(cassIterator))
                {
                    const CassRow* row = cass_iterator_get_row(cassIterator);
                    TaskM* task = TaskM::createTaskFromCassandraRow(row);
                    if (task != nullptr)
                    {
                        // Load variables
                        _retrieveIndependentVariableForTask(task);

                        // Load variables
                        _retrieveDependentVariableForTask(task);

                        // Add the task to the experimentation
                        _currentExperimentation->addTask(task);
                    }
                }

                cass_iterator_free(cassIterator);
            }
        }
        else {
            qCritical() << "Could not get all tasks for the current experiment from the database:" << cass_error_desc(cassError);
        }

        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
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
        const char* query = "SELECT * FROM ingescape.independent_var WHERE id_experimentation = ? AND id_task = ?;";

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(query, 2);
        cass_statement_bind_uuid(cassStatement, 0, task->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, task->getCassUuid());
        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qDebug() << "Get all independent variables for task" << task->name() << "succeeded";
        }
        else {
            qCritical() << "Could not get all independent variables for task" << task->name() << "from the database:" << cass_error_desc(cassError);
        }

        // Retrieve result set and iterate over the rows
        const CassResult* cassResult = cass_future_get_result(cassFuture);
        if (cassResult != nullptr)
        {
            CassIterator* cassIterator = cass_iterator_from_result(cassResult);

            while(cass_iterator_next(cassIterator))
            {
                const CassRow* row = cass_iterator_get_row(cassIterator);
                IndependentVariableM* independentVariable = IndependentVariableM::createIndependentVariableFromCassandraRow(row);
                if (independentVariable != nullptr)
                {
                    task->addIndependentVariable(independentVariable);
                }
            }

            cass_iterator_free(cassIterator);
        }

        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}

/**
 * @brief Retrieve all dependent variables from the Cassandra DB for the given task.
 * The task will be updated by this method.
 * @param task
 */
void ExperimentationController::_retrieveDependentVariableForTask(TaskM* task)
{
    if (AssessmentsModelManager::Instance() != nullptr)
    {
        const char* query = "SELECT * FROM ingescape.dependent_var WHERE id_experimentation = ? AND id_task = ?;";

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(query, 2);
        cass_statement_bind_uuid(cassStatement, 0, task->getExperimentationCassUuid());
        cass_statement_bind_uuid(cassStatement, 1, task->getCassUuid());
        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qDebug() << "Get all independent variables for task" << task->name() << "succeeded";
        }
        else {
            qCritical() << "Could not get all independent variables for task" << task->name() << "from the database:" << cass_error_desc(cassError);
        }

        // Retrieve result set and iterate over the rows
        const CassResult* cassResult = cass_future_get_result(cassFuture);
        if (cassResult != nullptr)
        {
            CassIterator* cassIterator = cass_iterator_from_result(cassResult);

            while(cass_iterator_next(cassIterator))
            {
                const CassRow* row = cass_iterator_get_row(cassIterator);
                DependentVariableM* dependentVariable = DependentVariableM::createDependentVariableFromCassandraRow(row);
                if (dependentVariable != nullptr)
                {
                    task->addDependentVariable(dependentVariable);
                }
            }

            cass_iterator_free(cassIterator);
        }

        cass_future_free(cassFuture);
        cass_statement_free(cassStatement);
    }
}
