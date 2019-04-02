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

#include "experimentationcontroller.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
ExperimentationController::ExperimentationController(IngeScapeModelManager* modelManager,
                                                     QObject *parent) : QObject(parent),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    if (_modelManager != nullptr)
    {
        connect(_modelManager, &IngeScapeModelManager::currentExperimentationChanged, this, &ExperimentationController::onCurrentExperimentationChanged);
    }
}


/**
 * @brief Destructor
 */
ExperimentationController::~ExperimentationController()
{
    qInfo() << "Delete Experimentation Controller";

    if (_modelManager != nullptr)
    {
        disconnect(_modelManager, nullptr, this, nullptr);

        _modelManager = nullptr;
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
    if (!recordName.isEmpty() && (subject != nullptr) && (task != nullptr)
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        qInfo() << "Create new record" << recordName << "for subject" << subject->name() << "and task" << task->name();

        // Create the new record
        RecordM* record = new RecordM(recordName, subject, task, nullptr);

        // Add the record to the experimentation
        _modelManager->currentExperimentation()->allRecords()->append(record);
    }
}


/**
 * @brief Open a record
 * @param record
 */
void ExperimentationController::openRecord(RecordM* record)
{
    if ((record != nullptr)
                    && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        qInfo() << "Open the record" << record->name() << "of the experimentation" << _modelManager->currentExperimentation()->name();

        // FIXME TODO: openRecord
    }
}


/**
 * @brief Delete a record
 * @param record
 */
void ExperimentationController::deleteRecord(RecordM* record)
{
    if ((record != nullptr)
            && (_modelManager != nullptr) && (_modelManager->currentExperimentation() != nullptr))
    {
        qInfo() << "Delete the record" << record->name() << "of the experimentation" << _modelManager->currentExperimentation()->name();

        // Remove from the experimentation
        _modelManager->currentExperimentation()->allRecords()->remove(record);

        // Free memory
        delete record;
    }
}


/**
 * @brief Slot called when the current experimentation changed
 * @param currentExperimentation
 */
void ExperimentationController::onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if (currentExperimentation != nullptr)
    {
        qDebug() << "onCurrentExperimentationChanged" << currentExperimentation->name();

        // FIXME TODO: load data about this experimentation (subjects, tasks, ...)
    }
}
