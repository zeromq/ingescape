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

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
ExperimentationController::ExperimentationController(AssessmentsModelManager* modelManager,
                                                     JsonHelper* jsonHelper,
                                                     QObject *parent) : QObject(parent),
    _recordC(nullptr),
    _currentExperimentation(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    // Create the controller to manage a record of the current experimentation
    _recordC = new RecordController(_modelManager, _jsonHelper, this);
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
    _modelManager = nullptr;
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
    if (currentExperimentation != nullptr)
    {
        qDebug() << "_on Current Experimentation Changed" << currentExperimentation->name();

        // FIXME TODO: load data about this experimentation (subjects, tasks, ...)

        const char* query = "SELECT * FROM ingescape.task;";

        // Creates the new query statement
        CassStatement* cassStatement = cass_statement_new(query, 0);

        // Execute the query or bound statement
        CassFuture* cassFuture = cass_session_execute(_modelManager->getCassSession(), cassStatement);
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

                    CassUuid taskUuid, experimentationUuid;
                    cass_value_get_uuid(cass_row_get_column_by_name(row, "id"), &taskUuid);
                    cass_value_get_uuid(cass_row_get_column_by_name(row, "id_experimentation"), &experimentationUuid);

                    const char *chrTaskName = "";
                    size_t nameLength = 0;
                    cass_value_get_string(cass_row_get_column_by_name(row, "name"), &chrTaskName, &nameLength);
                    QString taskName = QString::fromUtf8(chrTaskName, static_cast<int>(nameLength));

                    const char *chrPlatformUrl = "";
                    size_t platformUrlLength = 0;
                    cass_value_get_string(cass_row_get_column_by_name(row, "platform_file"), &chrPlatformUrl, &platformUrlLength);
                    QUrl platformUrl(QString::fromUtf8(chrPlatformUrl, static_cast<int>(platformUrlLength)));

                    TaskM* task = new TaskM(experimentationUuid, taskUuid, taskName, platformUrl);
                    if (task != nullptr)
                    {
                        // Load variables
                        query = "SELECT * FROM ingescape.independent_var WHERE id_experimentation = ? AND id_task = ?;";

                        // Creates the new query statement
                        CassStatement* indeVarCassStatement = cass_statement_new(query, 2);
                        cass_statement_bind_uuid(indeVarCassStatement, 0, experimentationUuid);
                        cass_statement_bind_uuid(indeVarCassStatement, 1, taskUuid);
                        // Execute the query or bound statement
                        CassFuture* indeVarCassFuture = cass_session_execute(_modelManager->getCassSession(), indeVarCassStatement);
                        CassError indeVarCassError = cass_future_error_code(indeVarCassFuture);
                        if (indeVarCassError == CASS_OK)
                        {
                            qDebug() << "Get all independent variables for task" << task->name() << "succeeded";
                        }
                        else {
                            qCritical() << "Could not get all independent variables for task" << task->name() << "from the database:" << cass_error_desc(indeVarCassError);
                        }

                        // Retrieve result set and iterate over the rows
                        const CassResult* indeVarCassResult = cass_future_get_result(indeVarCassFuture);
                        if (indeVarCassResult != nullptr)
                        {
                            CassIterator* indeVarCassIterator = cass_iterator_from_result(indeVarCassResult);

                            while(cass_iterator_next(indeVarCassIterator))
                            {
                                const CassRow* indeVarRow = cass_iterator_get_row(indeVarCassIterator);
                                IndependentVariableM* independentVariable = IndependentVariableM::createIndependentVariableFromCassandraRow(indeVarRow);
                                if (independentVariable != nullptr)
                                {
                                    task->addIndependentVariable(independentVariable);
                                }
                            }

                            cass_iterator_free(indeVarCassIterator);
                        }

                        cass_future_free(indeVarCassFuture);
                        cass_statement_free(indeVarCassStatement);

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
