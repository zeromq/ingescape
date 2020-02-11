/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "recordssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>
#include <controller/ingescapenetworkcontroller.h>


// Interval in milli-seconds to display current record elapsed time
#define INTERVAL_ELAPSED_TIME 250


/**
 * @brief Constructor
 * @param parent
 */
RecordsSupervisionController::RecordsSupervisionController(QObject *parent) : QObject(parent),
    _peerIdOfRecorder(""),
    _peerNameOfRecorder(""),
    _isRecorderON(false),
    _selectedRecord(nullptr),
    _isRecording(false),
    //_isRecordingTimeLine(false),
    _replayState(ReplayStates::UNLOADED),
    _currentReplay(nullptr),
    _currentRecordTime(QDateTime(QDate::currentDate())),
    _recordStartTime(QDateTime(QDate::currentDate()))
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Records are sorted on their end date & time
    _recordsList.setSortProperty("endDateTime");

    // Init the timer
    _timerToDisplayTime.setInterval(INTERVAL_ELAPSED_TIME);
    connect(&_timerToDisplayTime, &QTimer::timeout, this, &RecordsSupervisionController::_onTimeout_DisplayTime);
}


/**
 * @brief Destructor
 */
RecordsSupervisionController::~RecordsSupervisionController()
{
    // Clean-up current selection
    setselectedRecord(nullptr);

    setcurrentReplay(nullptr);

    _hashFromRecordIdToViewModel.clear();

    disconnect(&_timerToDisplayTime, &QTimer::timeout, this, &RecordsSupervisionController::_onTimeout_DisplayTime);

    // Delete all VM of host
    _recordsList.deleteAllItems();
}


/**
 * @brief Setter for property "Selected Record"
 * @param value
 */
void RecordsSupervisionController::setselectedRecord(RecordVM *value)
{
    if (_selectedRecord != value)
    {
        RecordVM* previousSelectedRecord = _selectedRecord;

        _selectedRecord = value;

        if ((previousSelectedRecord != nullptr) && (_selectedRecord == nullptr))
        {
            if (previousSelectedRecord->modelM() != nullptr) {
                qDebug() << "UN-load the previous selected record" << previousSelectedRecord->modelM()->uid();
            }

            // UN-load the previous selected record
            unloadRecord();
        }
        else if (_selectedRecord != nullptr)
        {
            if (_selectedRecord->modelM() != nullptr) {
                qDebug() << "Load the new selected record" << _selectedRecord->modelM()->uid();
            }

            // Load the new selected record
            loadRecord(_selectedRecord->modelM()->uid());
        }

        Q_EMIT selectedRecordChanged(value);
    }
}


/**
 * @brief Start/Stop to record (optionaly with the actions in the timeline)
 * @param withTimeLine flag indicating if the actions in the timeline must be recorded
 */
void RecordsSupervisionController::startOrStopToRecord(bool withTimeLine)
{
    Q_UNUSED(withTimeLine)

    if (_isRecorderON)
    {
        // Stop to record
        if (_isRecording)
        {
            // Update the flag
            //setisRecordingTimeLine(false);

            // Send the message "Stop Record" to the recorder
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, command_StopRecord);
        }
        // Start to record
        else
        {
            // Update flags
            //setisRecordingTimeLine(withTimeLine);

            Q_EMIT startToRecord();
        }
    }
}


/**
 * @brief Delete a record from the list
 * @param record
 */
void RecordsSupervisionController::deleteRecord(RecordVM* record)
{
    if ((record != nullptr) && (record->modelM() != nullptr))
    {
        qInfo() << "Ask to delete the record " << record->modelM()->name();

        // Notify the recorder that it has to remove entry from the data base
        if (_isRecorderON)
        {
            QString message = QString("%1=%2").arg(command_DeleteRecord, record->modelM()->uid());

            // Send the message "Delete Record" to the recorder
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
        }
    }
}


/**
 * @brief Load a record
 * @param recordId
 */
void RecordsSupervisionController::loadRecord(QString recordId)
{
    if (_isRecorderON && _hashFromRecordIdToViewModel.contains(recordId))
    {
        RecordVM* recordVM = _hashFromRecordIdToViewModel.value(recordId);
        if (recordVM != nullptr)
        {
            // To be sure, init the current state of the replay
            setreplayState(ReplayStates::UNLOADED);

            // Set the current replay (loading record)
            setcurrentReplay(recordVM);

            QString message = QString("%1=%2").arg(command_LoadReplay, recordId);

            // Send the message "Load Replay" to the recorder
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
        }
    }
}


/**
 * @brief UN-load the current loaded record (replay)
 */
void RecordsSupervisionController::unloadRecord()
{
    if (_isRecorderON && (_currentReplay != nullptr) && (_currentReplay->modelM() != nullptr))
    {
        QString message = QString("%1=%2").arg(command_UNloadReplay, _currentReplay->modelM()->uid());

        // Send the message "UN-load Replay" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
    }
}


/**
 * @brief Start or Resume the current loaded record (replay)
 * @param isStart
 */
void RecordsSupervisionController::startOrResumeReplay(bool isStart)
{
    if (_isRecorderON && (_currentReplay != nullptr) && (_currentReplay->modelM() != nullptr))
    {
        QString message = "";

        if (isStart)
        {
            // Update the current state of the replay
            setreplayState(ReplayStates::PLAYING);

            message = QString("%1=%2").arg(command_StartReplay, _currentReplay->modelM()->uid());
        }
        else
        {
            // Update the current state of the replay
            setreplayState(ReplayStates::RESUMING);

            message = QString("%1=%2").arg(command_UNpauseReplay, _currentReplay->modelM()->uid());
        }

        // Send the message "Start Replay" or "UN-pause Replay" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
    }
}


/**
 * @brief Stop or Pause the current loaded record (replay)
 * @param isStop
 */
void RecordsSupervisionController::stopOrPauseReplay(bool isStop)
{
    if (_isRecorderON && (_currentReplay != nullptr) && (_currentReplay->modelM() != nullptr))
    {
        QString message = "";

        if (isStop)
        {
            // Update the current state of the replay
            //setreplayState(ReplayStates::LOADED);

            message = QString("%1=%2").arg(command_StopReplay, _currentReplay->modelM()->uid());
        }
        else
        {
            // Update the current state of the replay
            setreplayState(ReplayStates::PAUSED);

            message = QString("%1=%2").arg(command_PauseReplay, _currentReplay->modelM()->uid());
        }

        // Send the message "Stop Replay" or "Pause Replay" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
    }
}


/**
 * @brief Export a record
 * @param recordId
 */
void RecordsSupervisionController::exportRecord(QString recordId)
{
    if (_isRecorderON && !recordId.isEmpty())
    {
        QString message = QString("%1=%2").arg(command_ExportRecord, recordId);

        // Send the message "Export Record" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, message);
    }
}


/**
 * @brief Get the name of the current replay
 * @return
 */
QString RecordsSupervisionController::getCurrentReplayName()
{
    if ((_currentReplay != nullptr) && (_currentReplay->modelM() != nullptr)) {
        return _currentReplay->modelM()->name();
    }
    else {
        return "";
    }
}


/**
 * @brief Slot called when a recorder enter the network
 * @param peerId
 * @param peerName
 * @param ipAddress
 * @param hostname
 */
void RecordsSupervisionController::onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qInfo() << "Recorder entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    if (!_isRecorderON && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfRecorder(peerId);
        setpeerNameOfRecorder(peerName);

        setisRecorderON(true);

        qDebug() << "New recorder on the network, get all its records...";

        // Send the message "Get Records" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerIdOfRecorder, "GET_RECORDS");
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
void RecordsSupervisionController::onRecorderExited(QString peerId, QString peerName)
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
 * @brief Occurs when records from DB have been received
 * @param list of records in JSON format
 */
void RecordsSupervisionController::onAllRecordsReceived(QString recordsJSON)
{
    if (!_hashFromRecordIdToModel.isEmpty())
    {
        QList<RecordM*> copy = _hashFromRecordIdToModel.values();
        _hashFromRecordIdToModel.clear();

        for (RecordM* model : copy)
        {
            if (model != nullptr)
            {
                // Delete the view model of record with the model
                _deleteRecordVMwithModel(model);

                // Free memory
                delete model;
            }
        }
    }

    if (!recordsJSON.isEmpty())
    {
        QByteArray byteArrayOfJson = recordsJSON.toUtf8();
        QList<RecordM*> recordsList = _createRecordsListFromJSON(byteArrayOfJson);

        if (!recordsList.isEmpty())
        {
            for (RecordM* record : recordsList)
            {
                if ((record != nullptr) && !_hashFromRecordIdToModel.contains(record->uid()))
                {
                    _hashFromRecordIdToModel.insert(record->uid(), record);

                    // Create a view model of record with this model
                    _createRecordVMwithModel(record);
                }
            }
        }
    }
}


/**
 * @brief Slot called when the "Recorder app" started to record
 */
void RecordsSupervisionController::onRecordStartedReceived()
{
    // Update the flag
    setisRecording(true);

    // Initialize the time when user started to record
    _recordStartTime = QDateTime::currentDateTime();

    // Start the timer for feedback
    _timerToDisplayTime.start();
}


/**
 * @brief Slot called when the "Recorder app" stopped to record
 */
void RecordsSupervisionController::onRecordStoppedReceived()
{
    // Update the flag
    setisRecording(false);

    // Stop the timer for feedback
    _timerToDisplayTime.stop();

    // Reset the record time
    setcurrentRecordTime(QDateTime(QDate::currentDate()));
}


/**
 * @brief Slot called when a new record has been added (into the DB)
 * @param record in JSON format
 */
void RecordsSupervisionController::onAddedRecord(QString recordJSON)
{
    //qDebug() << "onAddedRecord" << recordJSON;

    if (!recordJSON.isEmpty())
    {
        QByteArray byteArrayOfJson = recordJSON.toUtf8();
        QList<RecordM*> recordsList = _createRecordsListFromJSON(byteArrayOfJson);

        if (recordsList.count() == 1)
        {
            RecordM* newRecord = recordsList.at(0);

            if ((newRecord != nullptr) && !_hashFromRecordIdToModel.contains(newRecord->uid()))
            {
                _hashFromRecordIdToModel.insert(newRecord->uid(), newRecord);

                // Create a view model of record with this model
                _createRecordVMwithModel(newRecord);
            }
        }
    }
}


/**
 * @brief Slot called when a record has been deleted
 * @param recordId
 */
void RecordsSupervisionController::onDeletedRecord(QString recordId)
{
    qInfo() << "Deleted Record" << recordId;

    if (_hashFromRecordIdToModel.contains(recordId))
    {
        RecordM* model = _hashFromRecordIdToModel.value(recordId);

        _hashFromRecordIdToModel.remove(recordId);

        if (model != nullptr)
        {
            // Delete the view model of record with the model
            _deleteRecordVMwithModel(model);

            // Free memory
            delete model;
        }
    }
}


/**
 * @brief Slot called when a replay has been loaded
 */
void RecordsSupervisionController::onReplayLoaded()
{
    // Update the current state of the replay
    setreplayState(ReplayStates::LOADED);
}


/**
 * @brief Slot called when a replay has been UN-loaded
 */
void RecordsSupervisionController::onReplayUNloaded()
{
    // Update the current state of the replay
    setreplayState(ReplayStates::UNLOADED);

    // Reset the current replay (loaded record)
    if (_currentReplay != nullptr) {
        setcurrentReplay(nullptr);
    }
}


/**
 * @brief Slot called when a replay has ended
 */
void RecordsSupervisionController::onReplayEnded()
{
    // Update the current state of the replay
    setreplayState(ReplayStates::LOADED);
}


/**
 * @brief Slot called when a record has been exported
 */
void RecordsSupervisionController::onRecordExported()
{
    qInfo() << "The record has been recorded";
}


/**
 * @brief Called at each interval of our timer to display elapsed time
 */
void RecordsSupervisionController::_onTimeout_DisplayTime()
{
    qint64 recordDurationInMilliseconds = _recordStartTime.msecsTo(QDateTime::currentDateTime());

    setcurrentRecordTime(QDateTime(APPLICATION_START_DATE).addMSecs(recordDurationInMilliseconds));
}


/**
 * @brief Create a model of record from JSON data
 * @param byteArrayOfJson
 * @return
 */
QList<RecordM*> RecordsSupervisionController::_createRecordsListFromJSON(QByteArray byteArrayOfJson)
{
    QList<RecordM*> recordsList;

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

                    if (jsonName.isString() && jsonId.isString())
                    {
                        // Create record
                        RecordM* record = new RecordM(jsonId.toString(),
                                                      jsonName.toString(),
                                                      QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(jsonBeginDateTime.toDouble())),
                                                      QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(jsonEndDateTime.toDouble())));

                        recordsList.append(record);
                    }
                }
            }
        }
    }
    return recordsList;
}



/**
 * @brief Create a view model of record with a model
 * @param model
 */
void RecordsSupervisionController::_createRecordVMwithModel(RecordM* model)
{
    if ((model != nullptr) && !_hashFromRecordIdToViewModel.contains(model->uid()))
    {
        RecordVM* vm = new RecordVM(model);

        _hashFromRecordIdToViewModel.insert(model->uid(), vm);

        // Add to the displayed list
        _recordsList.append(vm);
    }
}


/**
 * @brief Delete a view model of record with its model
 * @param model
 */
void RecordsSupervisionController::_deleteRecordVMwithModel(RecordM* model)
{
    if ((model != nullptr) && _hashFromRecordIdToViewModel.contains(model->uid()))
    {
        RecordVM* vm = _hashFromRecordIdToViewModel.value(model->uid());
        if (vm != nullptr)
        {
            _hashFromRecordIdToViewModel.remove(model->uid());

            if (_currentReplay == vm)
            {
                setcurrentReplay(nullptr);

                // Reset the current state of the replay
                setreplayState(ReplayStates::UNLOADED);
            }

            if (_selectedRecord == vm) {
                setselectedRecord(nullptr);
            }

            // Remove from the displayed list
            _recordsList.remove(vm);

            // Free memory
            delete vm;
        }
    }
}
