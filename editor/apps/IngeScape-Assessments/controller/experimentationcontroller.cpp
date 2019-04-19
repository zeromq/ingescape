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
 * @param parent
 */
ExperimentationController::ExperimentationController(//IngeScapeModelManager* modelManager,
                                                     QObject *parent) : QObject(parent),
    _currentExperimentation(nullptr)
    //_modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

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

    /*if (_modelManager != nullptr)
    {
        disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
    }*/
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
        qInfo() << "Create new record" << recordName << "for subject" << subject->name() << "and task" << task->name();

        QDateTime now = QDateTime::currentDateTime();

        QString recordUID = now.toString("R-yyMMdd-hhmmss-zzz");

        // Create a new (experimentation) record
        ExperimentationRecordM* record = new ExperimentationRecordM(recordUID, recordName, subject, task, now, nullptr);

        // Add the record to the current experimentation
        _currentExperimentation->addRecord(record);
    }
}


/**
 * @brief Open a record
 * @param record
 */
void ExperimentationController::openRecord(ExperimentationRecordM* record)
{
    if ((record != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Open the record" << record->name() << "of the experimentation" << _currentExperimentation->name();

        // FIXME TODO: openRecord
    }
}


/**
 * @brief Delete a record
 * @param record
 */
void ExperimentationController::deleteRecord(ExperimentationRecordM* record)
{
    if ((record != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Delete the record" << record->name() << "of the experimentation" << _currentExperimentation->name();

        // Remove the record from the current experimentation
        _currentExperimentation->removeRecord(record);

        // Free memory
        delete record;
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
    }
}
