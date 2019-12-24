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
#include <controller/ingescapenetworkcontroller.h>
#include <controller/assessmentsmodelmanager.h>
#include "model/subject/characteristicvaluem.h"
#include "model/task/independentvariablevaluem.h"

/**
 * @brief Constructor
 * @param parent
 */
ExperimentationController::ExperimentationController(QObject *parent) : QObject(parent),
    _taskInstanceC(nullptr),
    _currentExperimentation(nullptr),
    _peerIdOfRecorder(""),
    _peerNameOfRecorder(""),
    _isRecorderON(false),
    _isRecording(false),
    _selectedSubjectIdListToFilter(QStringList()),
    _selectedProtocolNameListToFilter(QStringList()),
    _nextRecordToHandle(nullptr),
    _removeOtherRecordsWhileRecording(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Experimentation Controller";

    // Create the controller to manage a session of the current experimentation
    _taskInstanceC = new TaskInstanceController(this);

    // Sort list of our futur records by their start time in timeline
    _listFuturRecordsToHandle.setSortProperty("startTimeInTimeline");

    // Connect to signals from the controller of the scenario
    if (_taskInstanceC->scenarioC() != nullptr)
    {
        connect(_taskInstanceC->scenarioC(), &AbstractScenarioController::actionWillBeExecuted, this, &ExperimentationController::_onActionWillBeExecuted);
        connect(_taskInstanceC->scenarioC(), &AbstractScenarioController::timeLineStateUpdated, this, &ExperimentationController::_onTimeLineStateUpdated);
    }

    //
    // Init timer
    //
    _timerToHandleExistingRecords.setSingleShot(true);
    connect(&_timerToHandleExistingRecords, &QTimer::timeout, this, &ExperimentationController::_onTimeout_EncounterExistingRecords);
}


/**
 * @brief Destructor
 */
ExperimentationController::~ExperimentationController()
{
    qInfo() << "Delete Experimentation Controller";

    // First, delete the controller for session management
    if (_taskInstanceC != nullptr)
    {
        disconnect(_taskInstanceC);

        TaskInstanceController* temp = _taskInstanceC;
        settaskInstanceC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Then, we can reset the model of the current experimentation
    if (_currentExperimentation != nullptr)
    {
        setcurrentExperimentation(nullptr);
    }

    // Clear selected list to filter
    _selectedSubjectIdListToFilter.clear();
    _selectedProtocolNameListToFilter.clear();

    // Clear our list of records to handlex
    _listFuturRecordsToHandle.clear();

    // Disconnect our timer and stop it
    disconnect(&_timerToHandleExistingRecords, &QTimer::timeout, this, &ExperimentationController::_onTimeout_EncounterExistingRecords);
    _timerToHandleExistingRecords.stop();
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
 * @brief Create a new session for a subject and a protocol
 * @param subject
 * @param protocol
 */
void ExperimentationController::createNewSessionForSubjectAndProtocol(SubjectM* subject, TaskM* protocol)
{
    if ((subject != nullptr) && (protocol != nullptr) && (_currentExperimentation != nullptr))
    {
        QString sessionName = QString("%1 - %2 - %3").arg(protocol->name(), subject->displayedId(), QDate::currentDate().toString("dd/MM/yy"));

        qInfo() << "Create new session" << sessionName << "for subject" << subject->displayedId() << "and protocol" << protocol->name();

        // Create a new (experimentation) task instance
        TaskInstanceM* session = _insertTaskInstanceIntoDB(sessionName, subject, protocol);

        if (session != nullptr)
        {
            // Add the task insatnce to the current experimentation
            _currentExperimentation->addTaskInstance(session);

            // Open this new task instance
            openSession(session);
        }
    }
}


/**
 * @brief Open a session
 * @param session
 */
void ExperimentationController::openSession(TaskInstanceM* session)
{
    if ((session != nullptr) && (_currentExperimentation != nullptr))
    {
        qInfo() << "Open the session" << session->name() << "of the experimentation" << _currentExperimentation->name();

        if (session->recordsList()->isEmpty())
        {
            // Get the list of record
            QList<RecordAssessmentM*> recordList = AssessmentsModelManager::select<RecordAssessmentM>({session->getCassUuid() });
            session->recordsList()->append(recordList);

            igs_info("%i records on current task", session->recordsList()->count());
        }

        // If records list is empty when we load it from DB : session has never been recorded
        session->setisRecorded(!session->recordsList()->isEmpty());

        // Update the current task instance
        _taskInstanceC->setcurrentTaskInstance(session);
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
 * @brief Export the list of selected sessions
 */
void ExperimentationController::exportSelectedSessions()
{
    if ((_currentExperimentation != nullptr) && !_selectedSessions.isEmpty())
    {
        qInfo() << "Export" << _selectedSessions.count() << "selected sessions of" << _currentExperimentation->name() << "(to file)...";

        QStringList sessionIds;

        for (TaskInstanceM *session : _selectedSessions)
        {
            if (session != nullptr)
            {
                QString sessionUID = AssessmentsModelManager::cassUuidToQString(session->getCassUuid());
                sessionIds.append(sessionUID);
            }
        }

        if (_isRecorderON && !sessionIds.isEmpty())
        {
            QString message = QString("%1=%2").arg(command_ExportSessions, sessionIds.join('|'));

            //QString experimentationUID = AssessmentsModelManager::cassUuidToQString(_currentExperimentation->getCassUuid());
            //QString message = QString("%1=%2 [%3]").arg(command_ExportSessions, experimentationUID, sessionIds.join('|'));

            // Send the message "Export Sessions" to the recorder
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
        }
    }
}


/**
 * @brief Slot called when a recorder enter the network
 * @param peerId
 * @param peerName
 * @param ipAddress
 * @param hostname
 */
void ExperimentationController::onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qInfo() << "Recorder entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    if (!_isRecorderON && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfRecorder(peerId);
        setpeerNameOfRecorder(peerName);

        setisRecorderON(true);
    }
    else {
        qCritical() << "We are already connected to a recorder:" << _peerNameOfRecorder << "(" << _peerIdOfRecorder << ")";
    }
}


/**
 * @brief Slot called when a recorder quit the network
 * @param peerId
 * @param peerName
 */
void ExperimentationController::onRecorderExited(QString peerId, QString peerName)
{
    qInfo() << "Recorder exited (" << peerId << ")" << peerName;

    if (_isRecorderON && (_peerIdOfRecorder == peerId))
    {
        setpeerIdOfRecorder("");
        setpeerNameOfRecorder("");

        setisRecorderON(false);
    }
}


/**
 * @brief Slot called when the recorder has started the record
 */
void ExperimentationController::onRecordStartedReceived()
{
    if(_taskInstanceC != nullptr){
        setisRecording(true);
        _taskInstanceC->scenarioC()->playOrResumeTimeLine();

        //
        // Handle futur records
        //

        if ((_taskInstanceC->scenarioC() != nullptr) && (_nextRecordToHandle != nullptr))
        {
            int deltaTimeFromTimeLineStart = _taskInstanceC->scenarioC()->currentTime().msecsSinceStartOfDay();

            // N.B: timeout will apply user's choice (remove other records or stop record)
            if (_nextRecordToHandle->startTimeInTimeline() > deltaTimeFromTimeLineStart)
            {
                // Record to handle is beginning AFTER our current time
                _timerToHandleExistingRecords.start(_nextRecordToHandle->startTimeInTimeline() - deltaTimeFromTimeLineStart);
            }
            else {
                // Record to handle is beginning BEFORE our current time
                if (_removeOtherRecordsWhileRecording)
                {
                    _timerToHandleExistingRecords.start(0);
                }
                else {
                    qWarning() << "When user wants to keep its recorder already registered, next record to handle can't begin before our current time in our timeline !!";
                }
            }
        }
    }
}


/**
 * @brief Slot called when the recorder has stopped the record
 */
void ExperimentationController::onRecordStoppedReceived()
{
    qDebug() << "Record STOPPED" ;
    if(_taskInstanceC != nullptr){
        setisRecording(false);
        _taskInstanceC->scenarioC()->stopTimeLine();
    }
}


/**
 * @brief Slot called when a record is added : create a record and add it to the currentSession
 * @param model
 */
void ExperimentationController::onRecordAddedReceived(QString message){

    if (_taskInstanceC != nullptr && _taskInstanceC->currentTaskInstance() != nullptr){
        if (!message.isEmpty())
        {
            QByteArray byteArrayOfJson = message.toUtf8();

            QJsonDocument jsonAgentDefinition = QJsonDocument::fromJson(byteArrayOfJson);
            if (jsonAgentDefinition.isObject())
            {
                QJsonDocument jsonFileRoot = QJsonDocument::fromJson(byteArrayOfJson);
                QJsonValue recordsValue = jsonFileRoot.object().value("Records");

                if (recordsValue.isArray())
                {
                    for (QJsonValue jsonValue : recordsValue.toArray())
                    {
                        if (jsonValue.isObject())
                        {
                            QJsonObject jsonRecord = jsonValue.toObject();

                            QJsonValue jsonId = jsonRecord.value("id");
                            QJsonValue jsonName = jsonRecord.value("name_record");
                            QJsonValue jsonBeginDateTime = jsonRecord.value("time_beg");
                            QJsonValue jsonEndDateTime = jsonRecord.value("time_end");
                            QJsonValue jsonOffsetTimeline = jsonRecord.value("offset_timeline");

                            qDebug() << "JSON OFFSET " << jsonOffsetTimeline.toInt();

                            if (jsonName.isString() && jsonId.isString())
                            {
                                // Create record
                                RecordAssessmentM* record = new RecordAssessmentM(jsonId.toString(),
                                                              jsonName.toString(),
                                                              QDateTime::fromSecsSinceEpoch(static_cast<int>(jsonBeginDateTime.toDouble())),
                                                              QDateTime::fromSecsSinceEpoch(static_cast<int>(jsonEndDateTime.toDouble())),
                                                              jsonOffsetTimeline.toInt());

                                _taskInstanceC->currentTaskInstance()->recordsList()->append(record);

                                qInfo() << "Number of record " << _taskInstanceC->currentTaskInstance()->recordsList()->count();
                            }
                        }
                    }
                }
            }
        }
    }
};


/**
 * @brief Slot called when a record is deleted : delete a record and delete it from current session
 * @param message (id of the record)
 */
void ExperimentationController::onRecordDeletedReceived(QString message)
{
    if ((!message.isEmpty()) && (_currentExperimentation != nullptr) && (!_currentExperimentation->allTaskInstances()->isEmpty()))
    {
        for (TaskInstanceM* taskInstance : _currentExperimentation->allTaskInstances()->toList())
        {
            for (RecordAssessmentM* record : taskInstance->recordsList()->toList())
            {
                if (record->uid() == message)
                {
                    // Message is an unique ID, we can return from our function after remove record
                    _taskInstanceC->currentTaskInstance()->recordsList()->remove(record);
                    return;
                }
            }
        }
    }
};


/**
 * @brief Slot called when the current experimentation changed
 * @param currentExperimentation
 */
void ExperimentationController::_onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if (currentExperimentation != nullptr)
    {
        qDebug() << "_on Current Experimentation Changed" << currentExperimentation->name();

        // First, clear the list of selected sessions
        _selectedSessions.clear();

        _sessionFilteredList.setSourceModel(nullptr);

        _retrieveTasksForExperimentation(currentExperimentation);

        _retrieveSubjectsForExperimentation(currentExperimentation);

        _retrieveCharacteristicsForExperimentation(currentExperimentation);

        _retrieveCharacteristicValuesForSubjectsInExperimentation(currentExperimentation);

        _retrieveTaskInstancesForExperimentation(currentExperimentation);

        _retrieveIndependentVariableValuesForTaskInstancesInExperimentation(currentExperimentation);

        _sessionFilteredList.setSourceModel(_currentExperimentation->allTaskInstances());
    }
}


/**
 * @brief Slot called just before an action is performed
 * (the message "EXECUTED ACTION" must be sent on the network to the recorder)
 * @param message
 */
void ExperimentationController::_onActionWillBeExecuted(QString message)
{
    if (_isRecorderON)
    {
        // Send the message "EXECUTED ACTION" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
    }
}


/**
 * @brief Slot called when the state of the TimeLine updated
 * @param state
 */
void ExperimentationController::_onTimeLineStateUpdated(QString state)
{
    // Add the delta from the start time of the TimeLine
    int deltaTimeFromTimeLineStart = 0;

    if ((_taskInstanceC != nullptr) && (_taskInstanceC->scenarioC() != nullptr))
    {
        deltaTimeFromTimeLineStart = _taskInstanceC->scenarioC()->currentTime().msecsSinceStartOfDay();
    }

    // Notify the Recorder app
    if (_isRecorderON)
    {
        QString notificationAndParameters = QString("%1=%2|%3").arg(notif_TimeLineState, state, QString::number(deltaTimeFromTimeLineStart));

        // Send the message "TIMELINE STATE" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, notificationAndParameters);
    }
}


/**
 * @brief Called when our timer time out to handle existing record after our current time in our timeline
 */
void ExperimentationController::_onTimeout_EncounterExistingRecords() {
    if ((_nextRecordToHandle != nullptr) && (_isRecording))
    {
        if (_removeOtherRecordsWhileRecording) // User wants to remove other records encountered
        {
            // Notify the recorder that it has to remove entry from the data base
            QString message = QString("%1=%2").arg(command_DeleteRecord, _nextRecordToHandle->uid());
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);

            // Clean list of the record just deleted
            _listFuturRecordsToHandle.remove(_nextRecordToHandle);
            setnextRecordToHandle(nullptr);

            // Try to found next record that must be handled
            if ((_taskInstanceC != nullptr) && (_taskInstanceC->scenarioC() != nullptr) && !_listFuturRecordsToHandle.isEmpty())
            {
                int deltaTimeFromTimeLineStart = _taskInstanceC->scenarioC()->currentTime().msecsSinceStartOfDay();

                for (RecordAssessmentM* record : _listFuturRecordsToHandle.toList())
                {
                    if (record != nullptr)
                    {
                        if (record->endTimeInTimeline() >= deltaTimeFromTimeLineStart)
                        {
                            if (_nextRecordToHandle == nullptr) {
                                // Next record to handle found
                                setnextRecordToHandle(record);
                            }
                        }
                        else
                        {
                            // Clean list of records that are ended before our current time
                            _listFuturRecordsToHandle.remove(record);
                        }
                    }
                }

                // Relaunch timer to delete next recorder when current time achieved this record
                if (_nextRecordToHandle != nullptr)
                {
                    _timerToHandleExistingRecords.start(_nextRecordToHandle->startTimeInTimeline() - deltaTimeFromTimeLineStart);
                }
            }
        }
        else { // User wants to stop recording if we encounter another record already registered
            stopToRecord();
        }
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
                for (auto indepVarIterator = task->independentVariables()->begin() ; indepVarIterator != task->independentVariables()->end() ; ++indepVarIterator)
                {
                    IndependentVariableM* independentVar = *indepVarIterator;
                    if (independentVar != nullptr)
                    {
                        IndependentVariableValueM indepVarValue(subject->getExperimentationCassUuid(), taskInstance->getCassUuid(), independentVar->getCassUuid(), "");
                        AssessmentsModelManager::insert(indepVarValue);
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
    if (task != nullptr)
    {
        QList<IndependentVariableM*> indepVarList = AssessmentsModelManager::select<IndependentVariableM>({ task->getExperimentationCassUuid(), task->getCassUuid() });
        for (IndependentVariableM* independentVariable : indepVarList) {
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
    if (task != nullptr)
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
        QList<CharacteristicValueM*> characValueList = AssessmentsModelManager::select<CharacteristicValueM>({experimentation->getCassUuid()});
        for (CharacteristicValueM* characValue : characValueList)
        {
            if (characValue != nullptr)
            {
                SubjectM* subject = experimentation->getSubjectFromUID(characValue->subjectUuid);
                CharacteristicM* characteristic = experimentation->getCharacteristicFromUID(characValue->characteristicUuid);

                // Get characteristic value type
                if ((subject != nullptr) && (characteristic != nullptr))
                {
                    switch (characteristic->valueType()) {
                        case CharacteristicValueTypes::INTEGER:
                            subject->setCharacteristicValue(characteristic, characValue->valueString.toInt());
                            break;
                        case CharacteristicValueTypes::DOUBLE:
                            subject->setCharacteristicValue(characteristic, characValue->valueString.toDouble());
                            break;
                        case CharacteristicValueTypes::TEXT:
                            subject->setCharacteristicValue(characteristic, characValue->valueString);
                            break;
                        case CharacteristicValueTypes::CHARACTERISTIC_ENUM:
                            subject->setCharacteristicValue(characteristic, characValue->valueString);
                            break;
                        default: // UNKNOWN
                            break;
                    }
                }
            }
        }
        qDeleteAll(characValueList);
        characValueList.clear();
    }
}

/**
 * @brief Retrieve all independent variable values Cassandra DB for each task instance in the given experimentation.
 * The experimentation will be updated by this method
 * @param experimentation
 */
void ExperimentationController::_retrieveIndependentVariableValuesForTaskInstancesInExperimentation(ExperimentationM* experimentation)
{
    if (experimentation != nullptr)
    {
        QList<IndependentVariableValueM*> indepVarValueList = AssessmentsModelManager::select<IndependentVariableValueM>({ experimentation->getCassUuid() });
        for (IndependentVariableValueM* indepVarValue : indepVarValueList)
        {
            if (indepVarValue != nullptr)
            {
                TaskInstanceM* taskInstance = experimentation->getTaskInstanceFromUID(indepVarValue->taskInstanceUuid);
                IndependentVariableM* indepVar = taskInstance->task()->getIndependentVariableFromUuid(indepVarValue->independentVariableUuid);
                if ((taskInstance != nullptr) && (indepVar != nullptr))
                {
                    taskInstance->setIndependentVariableValue(indepVar, indepVarValue->valueString);
                }
            }
        }
        qDeleteAll(indepVarValueList);
        indepVarValueList.clear();
    }
}

bool ExperimentationController::isThereOneRecordAfterStartTime() {
    // Get start time of record in timeline
    int startTimeRecordInTimeline = _taskInstanceC->scenarioC()->currentTime().msecsSinceStartOfDay();

    // Test if there is an existing record after our start time
    for (RecordAssessmentM* record : _taskInstanceC->currentTaskInstance()->recordsList()->toList()) {
        if (record->endTimeInTimeline() >= startTimeRecordInTimeline) {
            return true;
        }
    }
    return false;
}


/**
 * @brief Method called when the user wants to start to record
 */
void ExperimentationController::startToRecord()
{
    if (!_isRecording && _isRecorderON && (_taskInstanceC != nullptr) && (_taskInstanceC->scenarioC() != nullptr))
    {
        // Current time in our timeline = beginning of user's new record
        int deltaTimeFromTimeLineStart = _taskInstanceC->scenarioC()->currentTime().msecsSinceStartOfDay();

        //
        // Handle our futur records
        //

        // Clean records list to handle other records while user is recording
        _listFuturRecordsToHandle.clear();
        setnextRecordToHandle(nullptr);

        // Update records list to handle and next record to handle
        if ((_taskInstanceC != nullptr) && (_taskInstanceC->currentTaskInstance()) && (!_taskInstanceC->currentTaskInstance()->recordsList()->isEmpty()))
        {
            for (RecordAssessmentM* record : _taskInstanceC->currentTaskInstance()->recordsList()->toList())
            {
                if ((record != nullptr) && (record->endTimeInTimeline() >= deltaTimeFromTimeLineStart))
                {
                    // Records are ending in the futur : we have to handle it
                    if (_nextRecordToHandle == nullptr)
                    {
                        // First record that start after our current time is next one to be handle
                        setnextRecordToHandle(record);
                    }
                    _listFuturRecordsToHandle.append(record);
                }
            }
        }

        if (!_removeOtherRecordsWhileRecording)
        {
            while ((_nextRecordToHandle != nullptr) && (_nextRecordToHandle->startTimeInTimeline() <= deltaTimeFromTimeLineStart))
            {
                // Record to handle is beginning BEFORE our current time and user wants to keep records already registered
                // We shift our current time in our timeline to the end of next record
                deltaTimeFromTimeLineStart = _nextRecordToHandle->endTimeInTimeline();
                QTime resetCurrentTime = QTime::fromMSecsSinceStartOfDay(0);
                _taskInstanceC->scenarioC()->setcurrentTime(resetCurrentTime.addMSecs(deltaTimeFromTimeLineStart));

                // Record end time is now before our current time, we can remove from our list to handle and set new next record to handle
                _listFuturRecordsToHandle.remove(_nextRecordToHandle);
                if (_listFuturRecordsToHandle.isEmpty())
                {
                    setnextRecordToHandle(nullptr);
                }
                else
                {
                    setnextRecordToHandle(_listFuturRecordsToHandle.at(0));
                }
            }
        }


        //
        // Start our record
        //

        TaskInstanceM* currentSession = _taskInstanceC->currentTaskInstance();
        if ((currentSession != nullptr) && (currentSession->task() != nullptr))
        {
            TaskM* task = _taskInstanceC->currentTaskInstance()->task();

            //QString currentPlatformName = task->platformFileName();

            // Get the JSON of the current platform
            QString platformFilePath = task->platformFileUrl().path();
#ifdef WIN64
    platformFilePath  = platformFilePath.remove(0,1);
#endif

            QFile jsonFile(platformFilePath);
            if (jsonFile.exists())
            {
                if (jsonFile.open(QIODevice::ReadOnly))
                {
                    QByteArray byteArrayOfJson = jsonFile.readAll();
                    jsonFile.close();

                    QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

                    if (!jsonDocument.isNull() && !jsonDocument.isEmpty())
                    {
                        QString jsonString = QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact));

                        // Add the delta from the start time of the TimeLine


                        QString recordName = QString("%1 (%2)").arg(currentSession->name(), _taskInstanceC->scenarioC()->currentTime().toString("hh:mm:ss.zzz"));

                        QString sessionUID = AssessmentsModelManager::cassUuidToQString(currentSession->getCassUuid());
                        QString experimentationUID = AssessmentsModelManager::cassUuidToQString(task->getExperimentationCassUuid());

                        QStringList message = {
                            command_StartRecord,
                            //currentPlatformName,
                            recordName,
                            QString::number(deltaTimeFromTimeLineStart),
                            jsonString,
                            sessionUID,
                            experimentationUID
                        };

                        // Send a ZMQ message in several parts to the recorder
                        IngeScapeNetworkController::instance()->sendZMQMessageToAgent(_peerIdOfRecorder, message);

                        // N.B: Wait for record started received (see ExperimentationController::onRecordStartedReceived)
                    }
                }
                else {
                    qCritical() << "Can not open file" << platformFilePath;
                }
            }
            else {
                qWarning() << "There is no file at" << platformFilePath;
            }
        }
    }
    else {
        qWarning() << "Recording is already launched or recorder is not launched";
    }
}


/**
 * @brief Method called when the user wants to stop to record
 */
void ExperimentationController::stopToRecord()
{
    if (_isRecording && _isRecorderON)
    {
        // Send the message "Stop Record" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, command_StopRecord);

    }
    else {
        qWarning() << "Recording is already stopped or recorder is not launched";
    }
}


/**
 * @brief Filter sessions list with one more subject
 * @param subjectId
 */
void ExperimentationController::addOneSubjectToFilterSessions(QString subjectId){
    QStringList temp = _selectedSubjectIdListToFilter;
    temp.append(subjectId);

    setselectedSubjectIdListToFilter(temp);

    _updateFilters();
}


/**
 * @brief Filter sessions list without one more subject
 * @param subjectId
 */
void ExperimentationController::removeOneSubjectToFilterSessions(QString subjectId){
    QStringList temp = _selectedSubjectIdListToFilter;
    temp.removeOne(subjectId);

    setselectedSubjectIdListToFilter(temp);

    _updateFilters();
}


/**
 * @brief Select sessions for every subject
 */
void ExperimentationController::addAllSubjectsToFilterSessions() {
    // Get all subjects displayed ids list
    QStringList tempSubjectIdList;
    for (SubjectM* subject : _currentExperimentation->allSubjects()->toList()) {
        tempSubjectIdList.append(subject->displayedId());
    }

    // Update selected subject ids list
    setselectedSubjectIdListToFilter(tempSubjectIdList);

    _updateFilters();
}


/**
 * @brief Filter sessions list without any subject
 */
void ExperimentationController::removeAllSubjectsToFilterSessions() {
    setselectedSubjectIdListToFilter(QStringList());

    _updateFilters();
}


/**
 * @brief Return true if the subject id is used to filter sessions list
 * @param subjectId
 * @return
 */
bool ExperimentationController::isSubjectFilterSessions(QString subjectId){
    return _selectedSubjectIdListToFilter.contains(subjectId);
}


/**
 * @brief Filter sessions list with one more subject
 * @param protocolName
 */
void ExperimentationController::addOneProtocolToFilterSessions(QString protocolName){
    QStringList temp = _selectedProtocolNameListToFilter;
    temp.append(protocolName);

    setselectedProtocolNameListToFilter(temp);

    _updateFilters();
}


/**
 * @brief Filter sessions list without one more protocol
 * @param protocolName
 */
void ExperimentationController::removeOneProtocolToFilterSessions(QString protocolName){
    QStringList temp = _selectedProtocolNameListToFilter;
    temp.removeOne(protocolName);

    setselectedProtocolNameListToFilter(temp);

    _updateFilters();
}


/**
 * @brief Filter sessions list with all protocols
 */
void ExperimentationController::addAllProtocolsToFilterSessions(){
    // Get all protocol names list
    QStringList tempProtocolNameList;
    for (TaskM* protocol : _currentExperimentation->allTasks()->toList()) {
        tempProtocolNameList.append(protocol->name());
    }

    // Update selected protocol names list
    setselectedProtocolNameListToFilter(tempProtocolNameList);

    _updateFilters();
}

/**
 * @brief Filter sessions list without any protocols
 */
void ExperimentationController::removeAllProtocolsToFilterSessions(){
    setselectedProtocolNameListToFilter(QStringList());

    _updateFilters();
}

/**
 * @brief Return true if the session is show for subject name
 * @param agentName
 * @return
 */
bool ExperimentationController::isProtocolFilterSessions(QString protocolName) {
    return _selectedProtocolNameListToFilter.contains(protocolName);
}

void ExperimentationController::_updateFilters() {
    // Update the list of subjects and protocol of the filter
    _sessionFilteredList.setselectedSubjectIdList(_selectedSubjectIdListToFilter);
    _sessionFilteredList.setselectedProtocolIdList(_selectedProtocolNameListToFilter);

    // Update the filter
    _sessionFilteredList.forceUpdate();
}

