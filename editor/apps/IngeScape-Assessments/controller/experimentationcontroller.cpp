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

        // Create a new (experimentation) record setup
        RecordSetupM* recordSetup = _insertRecordSetupIntoDB(recordName, subject, task);

        if (recordSetup != nullptr)
        {
            // Add the record to the current experimentation
            _currentExperimentation->addRecordSetup(recordSetup);

            // Open this new record
            openRecordSetup(recordSetup);
        }
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

        _retrieveTasksForExperimentation(currentExperimentation);

        _retrieveSubjectsForExperimentation(currentExperimentation);
    }
}

/**
 * @brief Create and insert a new record setup into the DB.
 * A nullptr is returned if the record setup could not be created
 * @param recordName
 * @param subject
 * @param task
 * @return
 */
RecordSetupM* ExperimentationController::_insertRecordSetupIntoDB(const QString& recordName, SubjectM* subject, TaskM* task)
{
    RecordSetupM* recordSetup = nullptr;

    if (subject != nullptr && task != nullptr)
    {
        CassUuid recordUuid;
        cass_uuid_gen_time(AssessmentsModelManager::Instance()->getCassUuidGen(), &recordUuid);

        time_t now = std::time(nullptr);

        cass_uint32_t yearMonthDay = cass_date_from_epoch(now);
        cass_int64_t timeOfDay = cass_time_from_epoch(now);

        const char* query = "INSERT INTO ingescape.record_setup (id_experimentation, id_subject, id_task, id, name, start_date, start_time, end_date, end_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
        CassStatement* cassStatement = cass_statement_new(query, 9);
        cass_statement_bind_uuid  (cassStatement, 0, subject->getExperimentationCassUuid());
        cass_statement_bind_uuid  (cassStatement, 1, subject->getCassUuid());
        cass_statement_bind_uuid  (cassStatement, 2, task->getCassUuid());
        cass_statement_bind_uuid  (cassStatement, 3, recordUuid);
        cass_statement_bind_string(cassStatement, 4, recordName.toStdString().c_str());
        cass_statement_bind_uint32(cassStatement, 5, yearMonthDay);
        cass_statement_bind_int64 (cassStatement, 6, timeOfDay);
        cass_statement_bind_uint32(cassStatement, 7, yearMonthDay); //FIXME current date/time to have all values filled with something for test purposes.
        cass_statement_bind_int64 (cassStatement, 8, timeOfDay); //FIXME current date/time to have all values filled with something for test purposes.

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
        CassError cassError = cass_future_error_code(cassFuture);
        if (cassError == CASS_OK)
        {
            qInfo() << "New dependent variable inserted into the DB";

            // Create the new record setup
            recordSetup = new RecordSetupM(recordUuid, recordName, subject, task, QDateTime::currentDateTime());
        }
        else {
            qCritical() << "Could not insert the new dependent variable into the DB:" << cass_error_desc(cassError);
        }

        cass_statement_free(cassStatement);
        cass_future_free(cassFuture);
    }

    return recordSetup;
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


/**
 * @brief Retrieve all subjects from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveSubjectsForExperimentation(ExperimentationM* experimentation)
{
    const char* query = "SELECT * FROM ingescape.subject WHERE id_experimentation = ?;";

    // Creates the new query statement
    CassStatement* cassStatement = cass_statement_new(query, 1);
    cass_statement_bind_uuid(cassStatement, 0, experimentation->getCassUuid());

    // Execute the query or bound statement
    CassFuture* cassFuture = cass_session_execute(AssessmentsModelManager::Instance()->getCassSession(), cassStatement);
    CassError cassError = cass_future_error_code(cassFuture);
    if (cassError == CASS_OK)
    {
        qDebug() << "Get all subjects succeeded";

        // Retrieve result set and iterate over the rows
        const CassResult* cassResult = cass_future_get_result(cassFuture);

        if (cassResult != nullptr)
        {
            CassIterator* cassIterator = cass_iterator_from_result(cassResult);

            while(cass_iterator_next(cassIterator))
            {
                const CassRow* row = cass_iterator_get_row(cassIterator);
                SubjectM* task = SubjectM::createTaskFromCassandraRow(row);
                if (task != nullptr)
                {
                    // Add the subject to the experimentation
                    experimentation->addSubject(task);
                }
            }

            cass_iterator_free(cassIterator);
        }
    }
    else {
        qCritical() << "Could not get all subjects for the current experiment from the database:" << cass_error_desc(cassError);
    }

    cass_future_free(cassFuture);
    cass_statement_free(cassStatement);
}


/**
 * @brief Retrieve all tasks from the Cassandra DB for the given experimentaion.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveTasksForExperimentation(ExperimentationM* experimentation)
{
    const char* query = "SELECT * FROM ingescape.task WHERE id_experimentation = ?;";

    // Creates the new query statement
    CassStatement* cassStatement = cass_statement_new(query, 1);
    cass_statement_bind_uuid(cassStatement, 0, experimentation->getCassUuid());

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
                    experimentation->addTask(task);
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
