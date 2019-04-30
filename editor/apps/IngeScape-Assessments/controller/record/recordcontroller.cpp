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

#include "recordcontroller.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
RecordController::RecordController(AssessmentsModelManager* modelManager,
                                   JsonHelper* jsonHelper,
                                   QObject *parent) : QObject(parent),
    _currentRecord(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";
}


/**
 * @brief Destructor
 */
RecordController::~RecordController()
{
    qInfo() << "Delete Record Controller";

    // Reset the model of the current record
    if (_currentRecord != nullptr)
    {
        setcurrentRecord(nullptr);
    }

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Setter for property "Current Record"
 * @param value
 */
void RecordController::setcurrentRecord(ExperimentationRecordM *value)
{
    if (_currentRecord != value)
    {
        ExperimentationRecordM *previousRecord = _currentRecord;

        _currentRecord = value;

        // Manage changes
        _onCurrentRecordChanged(previousRecord, _currentRecord);

        Q_EMIT currentRecordChanged(value);
    }
}


/**
 * @brief Slot called when the current record changed
 * @param currentRecord
 * @param previousRecord
 */
void RecordController::_onCurrentRecordChanged(ExperimentationRecordM* previousRecord, ExperimentationRecordM* currentRecord)
{
    // TODO clean previous record
    if (previousRecord != nullptr)
    {

    }

    // Manage the new (current) record
    if ((currentRecord != nullptr) && (currentRecord->task() != nullptr) && (_modelManager != nullptr) && (_jsonHelper != nullptr))
    {
        if (currentRecord->task()->platformFileUrl().isValid())
        {
            QString platformFilePath = currentRecord->task()->platformFileUrl().path();

            QFile jsonFile(platformFilePath);
            if (jsonFile.exists())
            {
                if (jsonFile.open(QIODevice::ReadOnly))
                {
                    QByteArray byteArrayOfJson = jsonFile.readAll();
                    jsonFile.close();

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

                    QJsonObject jsonRoot = jsonDocument.object();

                    // List of agents
                    if (jsonRoot.contains("scenario"))
                    {
                        // Get the hash table from a name to the group of agents with this name
                        QHash<QString, AgentsGroupedByNameVM*> hashFromNameToAgentsGrouped = _modelManager->getHashTableFromNameToAgentsGrouped();

                        // Create a model of scenario (actions in the list, in the palette and in the timeline) from JSON
                        ScenarioM* scenarioToImport = _jsonHelper->createModelOfScenarioFromJSON(jsonRoot.value("scenario").toObject(), hashFromNameToAgentsGrouped);
                        if (scenarioToImport != nullptr)
                        {
                            // FIXME TODO: scenarioToImport
                        }
                    }
                }
                else {
                    qCritical() << "Can not open file" << platformFilePath;
                }
            }
            else {
                qWarning() << "There is no file" << platformFilePath;
            }
        }
        else {
            qWarning() << "The URL of platform" << currentRecord->task()->platformFileUrl() << "is not valid";
        }
    }
}

