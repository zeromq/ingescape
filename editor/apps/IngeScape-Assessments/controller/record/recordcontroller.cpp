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
    _selectedAction(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Record Controller";

    // Create the controller to manage the time-line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(_modelManager, _jsonHelper, this);


    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    //connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::onTimeRangeChanged);
}


/**
 * @brief Destructor
 */
RecordController::~RecordController()
{
    qInfo() << "Delete Record Controller";

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

        ScenarioController* temp = _scenarioC;
        setscenarioC(nullptr);
        delete temp;
        temp = nullptr;
    }


    // Reset the model of the current record
    if (_currentRecord != nullptr)
    {
        setcurrentRecord(nullptr);
    }

    // Free memory
    _actionsList.deleteAllItems();

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
                            // Append the list of actions
                            if (!scenarioToImport->actionsList()->isEmpty())
                            {
                                // Add actions into our list
                                _actionsList.append(scenarioToImport->actionsList()->toList());
                            }

                            // Append the list of actions in timeline
                            /*if (!scenarioToImport->actionsInTimelineList()->isEmpty())
                            {
                                // Add each actionVM in to the right line of our timeline
                                for (ActionVM* actionVM : scenarioToImport->actionsInTimelineList()->toList())
                                {
                                    if ((actionVM != nullptr) && (actionVM->modelM() != nullptr))
                                    {
                                        int actionId = actionVM->modelM()->uid();
                                        int lineIndexInTimeLine = actionVM->lineInTimeLine();

                                        // Add the new action VM to our hash table
                                        QList<ActionVM*> listOfActionVM = _getListOfActionVMwithId(actionId);
                                        listOfActionVM.append(actionVM);
                                        _hashFromUidToViewModelsOfAction.insert(actionId, listOfActionVM);

                                        // Get the "Sorted" list of view models of action with the index of the line (in the time line)
                                        I2CustomItemSortFilterListModel<ActionVM>* sortedListOfActionVM = _getSortedListOfActionVMwithLineIndex(lineIndexInTimeLine);

                                        // Add the action VM to the line
                                        if (sortedListOfActionVM != nullptr) {
                                            sortedListOfActionVM->append(actionVM);
                                        }
                                        else
                                        {
                                            // Create a new list and add to the hash table
                                            sortedListOfActionVM = new I2CustomItemSortFilterListModel<ActionVM>();
                                            sortedListOfActionVM->setSortProperty("startTime");
                                            sortedListOfActionVM->append(actionVM);

                                            _hashFromLineIndexToSortedViewModelsOfAction.insert(lineIndexInTimeLine, sortedListOfActionVM);
                                        }

                                        _actionsInTimeLine.append(actionVM);

                                        // Increment the line number if necessary
                                        if (_linesNumberInTimeLine < lineIndexInTimeLine + 2) {
                                            setlinesNumberInTimeLine(lineIndexInTimeLine + 2);
                                        }
                                    }
                                }
                            }*/

                            // Free memory
                            delete scenarioToImport;
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

