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
    _isRecorderON(false),
    _selectedRecord(NULL),
    _isRecording(false),
    _isRecordingTimeLine(false),
    _isLoadingRecord(false),
    _playingRecord(NULL),
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
    setselectedRecord(NULL);

    _mapFromRecordIdToViewModel.clear();

    disconnect(&_timerToDisplayTime, &QTimer::timeout, this, &RecordsSupervisionController::_onTimeout_DisplayTime);

    // Delete all VM of host
    _recordsList.deleteAllItems();

    // Reset pointers
    _modelManager = NULL;
    _jsonHelper = NULL;
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

                Q_EMIT commandAskedToRecorder(command_StopToRecord);
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
    if ((_modelManager != NULL) && (record != NULL) && (record->modelM() != NULL))
    {
        qInfo() << "Ask to delete the record " << record->modelM()->name();

        // Notify the recorder that it has to remove entry from the data base
        if (_isRecorderON)
        {
            QString commandAndParameters = QString("%1=%2").arg(command_DeleteRecord, record->modelM()->id());

            Q_EMIT commandAskedToRecorder(commandAndParameters);
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
    if (_isRecorderON && _mapFromRecordIdToViewModel.contains(recordId))
    {
        RecordVM* recordVM = _mapFromRecordIdToViewModel.value(recordId);
        if (recordVM != NULL)
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
                setplayingRecord(NULL);
            }

            Q_EMIT commandAskedToRecorder(commandAndParameters);
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

    if (!_isRecorderON)
    {
        _peerIdOfRecorder = peerId;
        _peerNameOfRecorder = peerName;

        setisRecorderON(true);

        qDebug() << "New recorder on the network, get all its records...";

        // Get all records
        Q_EMIT commandAskedToRecorder("GET_RECORDS");
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
        _peerIdOfRecorder = "";
        _peerNameOfRecorder = "";

        setisRecorderON(false);
    }
}


/**
 * @brief Occurs when records from DB have been received
 * @param list of records in JSON format
 */
void RecordsSupervisionController::onAllRecordsReceived(QString recordsJSON)
{
    if (!_mapFromRecordIdToModel.isEmpty())
    {
        QList<RecordM*> copy = _mapFromRecordIdToModel.values();
        _mapFromRecordIdToModel.clear();

        for (RecordM* model : copy)
        {
            if (model != NULL)
            {
                // Delete the view model of record with the model
                _deleteRecordVMwithModel(model);

                // Free memory
                delete model;
            }
        }
    }

    if (!recordsJSON.isEmpty() && (_jsonHelper != NULL))
    {
        QByteArray byteArrayOfJson = recordsJSON.toUtf8();
        QList<RecordM*> recordsList = _jsonHelper->createRecordModelList(byteArrayOfJson);

        if (!recordsList.isEmpty())
        {
            for (RecordM* record : recordsList)
            {
                if ((record != NULL) && !_mapFromRecordIdToModel.contains(record->id()))
                {
                    _mapFromRecordIdToModel.insert(record->id(), record);

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
    if (!recordJSON.isEmpty() && (_jsonHelper != NULL))
    {
        QByteArray byteArrayOfJson = recordJSON.toUtf8();
        QList<RecordM*> recordsList = _jsonHelper->createRecordModelList(byteArrayOfJson);

        if (recordsList.count() == 1)
        {
            RecordM* newRecord = recordsList.at(0);

            if ((newRecord != NULL) && !_mapFromRecordIdToModel.contains(newRecord->id()))
            {
                _mapFromRecordIdToModel.insert(newRecord->id(), newRecord);

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

    if (_mapFromRecordIdToModel.contains(recordId))
    {
        RecordM* model = _mapFromRecordIdToModel.value(recordId);

        _mapFromRecordIdToModel.remove(recordId);

        if (model != NULL)
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

    if (_playingRecord != NULL) {
        setplayingRecord(NULL);
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
    if ((model != NULL) && !_mapFromRecordIdToViewModel.contains(model->id()))
    {
        RecordVM* vm = new RecordVM(model);

        _mapFromRecordIdToViewModel.insert(model->id(), vm);

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
    if ((model != NULL) && _mapFromRecordIdToViewModel.contains(model->id()))
    {
        RecordVM* vm = _mapFromRecordIdToViewModel.value(model->id());
        if (vm != NULL)
        {
            _mapFromRecordIdToViewModel.remove(model->id());

            if (_playingRecord != NULL) {
                setplayingRecord(NULL);
            }
            if (_selectedRecord == vm) {
                setselectedRecord(NULL);
            }

            // Remove from the displayed list
            _recordsList.remove(vm);

            // Free memory
            delete vm;
        }
    }
}


