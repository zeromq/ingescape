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
#define INTERVAL_ELAPSED_TIME 25

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
 * @brief Custom setter on is recording command for the scenario
 * @param is recording flag
 */
void RecordsSupervisionController::setisRecording(bool isRecording)
{
    if (_isRecording != isRecording)
    {
        _isRecording = isRecording;

        Q_EMIT isRecordingChanged(_isRecording);

        if (_isRecorderON && !_peerIdOfRecorder.isEmpty())
        {
            QStringList peerIdsList = QStringList(_peerIdOfRecorder);
            QString command = _isRecording ? "START_TO_RECORD" : "STOP_TO_RECORD";

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }

        // Update display of elapsed time
        if (isRecording) {
            _timerToDisplayTime.start();
        }
        else
        {
            _timerToDisplayTime.stop();
            setcurrentRecordTime(QTime(0, 0, 0, 0));
        }
    }
}


/**
 * @brief Delete the selected agent from the list
 */
void RecordsSupervisionController::deleteSelectedRecord()
{
    if ((_modelManager != NULL) && (_selectedRecord != NULL) && (_selectedRecord->modelM() != NULL))
    {
        qDebug() << "Delete the selected record " << _selectedRecord->modelM()->name();

        // Notify the recorder that he has to remove entry from db
        if (_isRecorderON && !_peerIdOfRecorder.isEmpty())
        {
            QStringList peerIdsList = QStringList(_peerIdOfRecorder);
            QString command = QString("DELETE_RECORD=%1").arg(_selectedRecord->modelM()->id());

            Q_EMIT commandAskedToAgent(peerIdsList, command);
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
    if (_isRecorderON && !_peerIdOfRecorder.isEmpty() && _mapFromRecordIdToViewModel.contains(recordId))
    {
        RecordVM* recordVM = _mapFromRecordIdToViewModel.value(recordId);
        if (recordVM != NULL)
        {
            QStringList peerIdsList = QStringList(_peerIdOfRecorder);
            QString command = "";

            if (startPlaying)
            {
                command = QString("PLAY_RECORD=%1").arg(recordId);
                setplayingRecord(recordVM);

                setisLoadingRecord(true);
            }
            else
            {
                command = QString("STOP_RECORD=%1").arg(recordId);
                setplayingRecord(NULL);
            }

            Q_EMIT commandAskedToAgent(peerIdsList, command);
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

        QStringList peerIdsList = QStringList(_peerIdOfRecorder);

        // Get all records
        Q_EMIT commandAskedToAgent(peerIdsList, "GET_RECORDS");
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
 * @brief Occurs when records from DB have been received
 * @param record in JSON format
 */
void RecordsSupervisionController::onNewRecordReceived(QString recordJSON)
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
void RecordsSupervisionController::onRecordDeleted(QString recordId)
{
    qDebug() << "onRecordDeleted" << recordId;

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
 * @brief Slot called when a record playing has ended
 */
void RecordsSupervisionController::onEndOfRecordReceived()
{
    if (_playingRecord != NULL)
    {
        setplayingRecord(NULL);
    }
    setisLoadingRecord(false);
}


/**
 * @brief Slot called when a record has been loaded
 */
void RecordsSupervisionController::onLoadedRecordReceived()
{
    setisLoadingRecord(false);
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


