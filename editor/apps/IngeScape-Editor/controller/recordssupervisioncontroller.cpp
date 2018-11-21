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

// Interval in milli-seconds to display current record elapsed time
#define INTERVAL_ELAPSED_TIME 50


/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
RecordsSupervisionController::RecordsSupervisionController(IngeScapeModelManager* modelManager,
                                                           JsonHelper* jsonHelper,
                                                           QObject *parent) : QObject(parent),
    _peerIdOfRecorder(""),
    _peerNameOfRecorder(""),
    _isRecorderON(false),
    _selectedRecord(nullptr),
    _isRecording(false),
    _isRecordingTimeLine(false),
    _isLoadingRecord(false),
    _playingRecord(nullptr),
    _currentRecordTime(QTime::fromMSecsSinceStartOfDay(0)),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // init display timer
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

    _hashFromRecordIdToViewModel.clear();

    disconnect(&_timerToDisplayTime, &QTimer::timeout, this, &RecordsSupervisionController::_onTimeout_DisplayTime);

    // Delete all VM of host
    _recordsList.deleteAllItems();

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Start/Stop to record (optionaly with the actions in the timeline)
 * @param isStart flag indicating if we start to record or if we stop to record
 * @param withTimeLine flag indicating if the actions in the timeline must be recorded
 */
void RecordsSupervisionController::startOrStopToRecord(bool isStart, bool withTimeLine)
{
    if (_isRecorderON)
    {
        // Start to record
        if (isStart)
        {
            if (!_isRecording)
            {
                // Update flags
                setisRecording(true);
                setisRecordingTimeLine(withTimeLine);

                // Start the timer for feedback
                _timerToDisplayTime.start();

                Q_EMIT startToRecord();
            }
        }
        // Stop to record
        else
        {
            if (_isRecording)
            {
                // Update the flag
                setisRecording(false);
                setisRecordingTimeLine(false);

                // Stop the timer for feedback
                _timerToDisplayTime.stop();
                setcurrentRecordTime(QTime(0, 0, 0, 0));

                Q_EMIT commandAskedToRecorder(_peerIdOfRecorder, command_StopToRecord);
            }
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
            QString commandAndParameters = QString("%1=%2").arg(command_DeleteRecord, record->modelM()->id());

            Q_EMIT commandAskedToRecorder(_peerIdOfRecorder, commandAndParameters);
        }
    }
}


/**
 * @brief Controls the selected record from the list
 * @param recordId
 * @param startPlaying
 */
void RecordsSupervisionController::controlRecord(QString recordId, bool startPlaying)
{
    if (_isRecorderON && _hashFromRecordIdToViewModel.contains(recordId))
    {
        RecordVM* recordVM = _hashFromRecordIdToViewModel.value(recordId);
        if (recordVM != nullptr)
        {
            QString commandAndParameters = "";

            if (startPlaying)
            {
                commandAndParameters = QString("%1=%2").arg(command_PlayTheRecord, recordId);
                setplayingRecord(recordVM);
            }
            else
            {
                commandAndParameters = QString("%1=%2").arg(command_StopTheRecord, recordId);
                setplayingRecord(nullptr);
            }

            Q_EMIT commandAskedToRecorder(_peerIdOfRecorder, commandAndParameters);
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
void RecordsSupervisionController::onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qDebug() << "Recorder Entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    if (!_isRecorderON && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfRecorder(peerId);
        setpeerNameOfRecorder(peerName);

        setisRecorderON(true);

        qDebug() << "New recorder on the network, get all its records...";

        // Get all records
        Q_EMIT commandAskedToRecorder(_peerIdOfRecorder, "GET_RECORDS");
    }
}


/**
 * @brief Slot called when a recorder quit the network
 * @param peerId
 * @param peerName
 */
void RecordsSupervisionController::onRecorderExited(QString peerId, QString peerName)
{
    qDebug() << "Recorder Exited (" << peerId << ")" << peerName;

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

    if (!recordsJSON.isEmpty() && (_jsonHelper != nullptr))
    {
        QByteArray byteArrayOfJson = recordsJSON.toUtf8();
        QList<RecordM*> recordsList = _jsonHelper->createRecordModelList(byteArrayOfJson);

        if (!recordsList.isEmpty())
        {
            for (RecordM* record : recordsList)
            {
                if ((record != nullptr) && !_hashFromRecordIdToModel.contains(record->id()))
                {
                    _hashFromRecordIdToModel.insert(record->id(), record);

                    // Create a view model of record with this model
                    _createRecordVMwithModel(record);
                }
            }
        }
    }
}


/**
 * @brief Slot called when a new record has been added (into the DB)
 * @param record in JSON format
 */
void RecordsSupervisionController::onAddedRecord(QString recordJSON)
{
    if (!recordJSON.isEmpty() && (_jsonHelper != nullptr))
    {
        QByteArray byteArrayOfJson = recordJSON.toUtf8();
        QList<RecordM*> recordsList = _jsonHelper->createRecordModelList(byteArrayOfJson);

        if (recordsList.count() == 1)
        {
            RecordM* newRecord = recordsList.at(0);

            if ((newRecord != nullptr) && !_hashFromRecordIdToModel.contains(newRecord->id()))
            {
                _hashFromRecordIdToModel.insert(newRecord->id(), newRecord);

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
    qDebug() << "on Deleted Record" << recordId;

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
 * @brief Slot called when a record is loading
 * @param deltaTimeFromTimeLine
 * @param jsonPlatform
 * @param jsonExecutedActions
 */
void RecordsSupervisionController::onLoadingRecord(int deltaTimeFromTimeLine, QString jsonPlatform, QString jsonExecutedActions)
{
    Q_UNUSED(deltaTimeFromTimeLine)
    Q_UNUSED(jsonPlatform)
    Q_UNUSED(jsonExecutedActions)

    setisLoadingRecord(true);
}


/**
 * @brief Slot called when a record has been loaded
 */
void RecordsSupervisionController::onLoadedRecord()
{
    setisLoadingRecord(false);
}


/**
 * @brief Slot called when a record playing has ended
 */
void RecordsSupervisionController::onEndOfRecord()
{
    setisLoadingRecord(false);

    if (_playingRecord != nullptr) {
        setplayingRecord(nullptr);
    }
}


/**
 * @brief Called at each interval of our timer to display elapsed time
 */
void RecordsSupervisionController::_onTimeout_DisplayTime()
{
    setcurrentRecordTime(_currentRecordTime.addMSecs(INTERVAL_ELAPSED_TIME));
}


/**
 * @brief Create a view model of record with a model
 * @param model
 */
void RecordsSupervisionController::_createRecordVMwithModel(RecordM* model)
{
    if ((model != nullptr) && !_hashFromRecordIdToViewModel.contains(model->id()))
    {
        RecordVM* vm = new RecordVM(model);

        _hashFromRecordIdToViewModel.insert(model->id(), vm);

        // Insert in the displayed list
        _recordsList.insert(0, vm);
    }
}


/**
 * @brief Delete a view model of record with its model
 * @param model
 */
void RecordsSupervisionController::_deleteRecordVMwithModel(RecordM* model)
{
    if ((model != nullptr) && _hashFromRecordIdToViewModel.contains(model->id()))
    {
        RecordVM* vm = _hashFromRecordIdToViewModel.value(model->id());
        if (vm != nullptr)
        {
            _hashFromRecordIdToViewModel.remove(model->id());

            if (_playingRecord != nullptr) {
                setplayingRecord(nullptr);
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
