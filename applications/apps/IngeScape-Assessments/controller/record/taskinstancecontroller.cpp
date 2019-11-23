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

#include "taskinstancecontroller.h"

#include <controller/assessmentsmodelmanager.h>

/**
 * @brief Constructor
 * @param parent
 */
TaskInstanceController::TaskInstanceController(QObject *parent) : QObject(parent),
    _timeLineC(nullptr),
    _scenarioC(nullptr),
    _currentTaskInstance(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";

    // Create the controller to manage the time-line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the controller for scenario management
    _scenarioC = new AbstractScenarioController(this);

    // List of agents present in current Platform
    _agentsGroupedByNameInCurrentPlatform.setSourceModel(IngeScapeModelManager::instance()->allAgentsGroupsByName());

    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &AbstractScenarioController::onTimeRangeChanged);
}


/**
 * @brief Destructor
 */
TaskInstanceController::~TaskInstanceController()
{
    qInfo() << "Delete Record Controller";


    // Reset the model of the current task instance
    if (_currentTaskInstance != nullptr)
    {
        setcurrentTaskInstance(nullptr);
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
}


/**
 * @brief Setter for property "Current Task Instance"
 * @param value
 */
void TaskInstanceController::setcurrentTaskInstance(TaskInstanceM *value)
{
    if (_currentTaskInstance != value)
    {
        TaskInstanceM *previousTaskInstance = _currentTaskInstance;

        _currentTaskInstance = value;

        // Manage changes
        _oncurrentTaskInstanceChanged(previousTaskInstance, _currentTaskInstance);

        Q_EMIT currentTaskInstanceChanged(value);
    }
}


/**
 * @brief Slot called when the current task instance changed
 * @param previousTaskInstance
 * @param currentTaskInstance
 */
void TaskInstanceController::_oncurrentTaskInstanceChanged(TaskInstanceM* previousTaskInstance, TaskInstanceM* currentTaskInstance)
{
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if ((ingeScapeModelManager != nullptr) && (_scenarioC != nullptr))
    {
        //
        // Clean the previous session
        //
        if (previousTaskInstance != nullptr)
        {
            // Clear the previous scenario
            _scenarioC->clearScenario();

            // Delete all published values
            ingeScapeModelManager->deleteAllPublishedValues();

            // Delete all (models of) actions
            ingeScapeModelManager->deleteAllActions();

            // Delete agents OFF
            QStringList namesListOfAgentsON = ingeScapeModelManager->deleteAgentsOFF();
            qDebug() << "Remaining agents ON:" << namesListOfAgentsON;
        }


        //
        // Manage the new (current) session
        //
        if ((currentTaskInstance != nullptr) && (currentTaskInstance->task() != nullptr))
        {
            if (currentTaskInstance->task()->platformFileUrl().isValid())
            {
                QString platformFilePath = currentTaskInstance->task()->platformFileUrl().path();

                QFile jsonFile(platformFilePath);
                if (jsonFile.exists())
                {
                    if (jsonFile.open(QIODevice::ReadOnly))
                    {
                        QByteArray byteArrayOfJson = jsonFile.readAll();
                        jsonFile.close();

                        QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);
                        if (jsonDocument.isObject())
                        {
                            QJsonObject jsonRoot = jsonDocument.object();

                            // Version
                            QString versionJsonPlatform = "";
                            if (jsonRoot.contains("version"))
                            {
                                versionJsonPlatform = jsonRoot.value("version").toString();

                                qDebug() << "Version of JSON platform is" << versionJsonPlatform;
                            }
                            else {
                                qDebug() << "UNDEFINED version of JSON platform";
                            }

                            // Import the agents list from JSON
                            if (jsonRoot.contains("agents"))
                            {
                                ingeScapeModelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);
                            }

                            // Import the scenario from JSON
                            if (jsonRoot.contains("scenario"))
                            {
                                _scenarioC->importScenarioFromJson(jsonRoot.value("scenario").toObject());
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

                // Update the filter
                _agentsGroupedByNameInCurrentPlatform.setcurrentProtocol(currentTaskInstance->task());
                _agentsGroupedByNameInCurrentPlatform.forceUpdate();
            }
            else {
                qWarning() << "The URL of platform" << currentTaskInstance->task()->platformFileUrl() << "is not valid";
            }
        }
    }
}


/**
 * @brief Adds the given URLs as attachements for this record
 * @param urlList
 */
void TaskInstanceController::addNewAttachements(const QList<QUrl>& urlList)
{
    for (QUrl url : urlList)
    {
        qDebug() << "New attachement:" << url.toString();
    }
}

