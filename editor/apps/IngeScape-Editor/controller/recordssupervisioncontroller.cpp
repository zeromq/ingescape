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
 * @brief Default constructor
 * @param modelManager
 * @param parent
 */
RecordsSupervisionController::RecordsSupervisionController(IngeScapeModelManager* modelManager,
                                                           QObject *parent) : QObject(parent),
    _isRecorderON(false),
    _selectedRecord(NULL),
    _isRecording(false),
    _isLoadingRecord(false),
    _playingRecord(NULL),
    _currentRecordTime(QTime::fromMSecsSinceStartOfDay(0)),
    _modelManager(modelManager)
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

    _modelManager = NULL;
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
        if (isRecording)
        {
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
    if ((_modelManager != NULL) && (_selectedRecord != NULL) && (_selectedRecord->recordModel() != NULL))
    {
        qDebug() << "Delete the selected record " << _selectedRecord->recordModel()->name();

        // Notify the recorder that he has to remove entry from db
        if (_isRecorderON && !_peerIdOfRecorder.isEmpty())
        {
            QStringList peerIdsList = QStringList(_peerIdOfRecorder);
            QString command = QString("DELETE_RECORD=%1").arg(_selectedRecord->recordModel()->id());

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }

        // Remove it from the list
        _recordsList.remove(_selectedRecord);

        // Delete each model of record
        _modelManager->deleteRecordModel(_selectedRecord->recordModel());

        // Delete the view model of record
        RecordVM* temp = _selectedRecord;
        setselectedRecord(NULL);
        delete temp;
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
        //if (recordVM != NULL)

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
 * @brief Slot when the list of records model changes
 * @param records
 */
void RecordsSupervisionController::onRecordsListChanged(QList<RecordM*> records)
{
    QList<RecordVM*> recordsToDelete = _recordsList.toList();
    _recordsList.clear();

    for (RecordVM* vm : recordsToDelete)
    {
        if (vm != NULL) {
            _deleteRecordVM(vm);
        }
    }

    QList<RecordVM*> recordsToAdd;
    for (RecordM* model : records)
    {
        if (model != NULL)
        {
            RecordVM* vm = new RecordVM(model);
            recordsToAdd.append(vm);
            _mapFromRecordIdToViewModel.insert(model->id(), vm);
        }
    }

    _recordsList.append(recordsToAdd);
}


/**
 * @brief Slot when a new model of record has been added
 * @param record
 */
void RecordsSupervisionController::onRecordAdded(RecordM* model)
{
    if (model != NULL)
    {
        RecordVM* vm = new RecordVM(model);
        _recordsList.insert(0, vm);
        _mapFromRecordIdToViewModel.insert(model->id(), vm);
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
 * @brief Aims at deleting VM and model of a record
 * @param record
 */
void RecordsSupervisionController::_deleteRecordVM(RecordVM* record)
{
    if (record != NULL)
    {
        // Delete each model of record
        _modelManager->deleteRecordModel(record->recordModel());

        // Delete the view model of record
        delete record;
    }
}


