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
    _timeLineC(nullptr),
    _scenarioC(nullptr),
    _currentRecord(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";

    // Create the controller to manage the time-line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the controller for scenario management
    _scenarioC = new AbstractScenarioController(_modelManager, _jsonHelper, this);


    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &AbstractScenarioController::onTimeRangeChanged);
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


    //
    // Clean-up sub-controllers
    //

    if (_timeLineC != nullptr)
    {
        //disconnect(_timeLineC);

        AbstractTimeActionslineScenarioViewController* temp = _timeLineC;
        settimeLineC(nullptr);
        delete temp;
    }

    if (_scenarioC != nullptr)
    {
        //disconnect(_scenarioC);

        AbstractScenarioController* temp = _scenarioC;
        setscenarioC(nullptr);
        delete temp;
        temp = nullptr;
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
    if (_scenarioC != nullptr)
    {
        // Clean the previous record
        if (previousRecord != nullptr)
        {
            // Clear the previous scenario
            _scenarioC->clearScenario();
        }

        // Manage the new (current) record
        if ((currentRecord != nullptr) && (currentRecord->task() != nullptr))
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

                        // Import the scenario from JSON
                        if (jsonRoot.contains("scenario"))
                        {
                            _scenarioC->importScenarioFromJson(jsonRoot.value("scenario").toObject());
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
}

