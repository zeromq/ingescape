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
RecordsSupervisionController::RecordsSupervisionController(IngeScapeModelManager* modelManager, QObject *parent) : QObject(parent),
    _recorderAgent(NULL),
    _selectedRecord(NULL),
    _isRecording(false),
    _playingRecordId(""),
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

    _mapFromRecordModelToViewModel.clear();

    disconnect(&_timerToDisplayTime, &QTimer::timeout, this, &RecordsSupervisionController::_onTimeout_DisplayTime);

    // Delete all VM of host
    _recordsList.deleteAllItems();

    _modelManager = NULL;
}

/**
 * @brief Slot when the list of records model changes
 * @param records
 */
void RecordsSupervisionController::onRecordsListChanged(QList<RecordM*> newRecords)
{
    QList<RecordVM*> recordsToDelete = _recordsList.toList();
    _recordsList.clear();

    foreach (RecordVM* vm, recordsToDelete) {
        _deleteRecordVM(vm);
    }

    QList<RecordVM*> recordsToAdd;
    foreach (RecordM* model, newRecords) {
        RecordVM* vm = new RecordVM(model);
        recordsToAdd.append(vm);
    }

    _recordsList.append(recordsToAdd);
}


/**
 * @brief Slot when a new model of record has been added
 * @param record
 */
void RecordsSupervisionController::onRecordAdded(RecordM* model)
{
    RecordVM* vm = new RecordVM(model);
    _recordsList.insert(0, vm);
}


/**
 * @brief Slot when a new model of agent has been created
 * @param agent
 */
void RecordsSupervisionController::onAgentModelCreated(AgentM* model)
{
    if (model != NULL && model->isRecorder() && model != _recorderAgent)
    {
        setrecorderAgent(model);

        // Retrieve all records
        Q_EMIT commandAskedToAgent(_recorderAgent->peerId().split(","), "GET_RECORDS");
        qDebug() << "New recorder on the network, get all its records";
    }
}

/**
 * @brief Delete the selected agent from the list
 */
void RecordsSupervisionController::deleteSelectedRecord()
{
    if ((_modelManager != NULL) && (_selectedRecord != NULL))
    {
        qDebug() << "Delete _selectedRecord " << _selectedRecord->recordModel()->name();

        // Notify the recorder that he has to remove entry from db
        if(_recorderAgent != NULL)
        {
            QString idToRemove = _selectedRecord->recordModel()->id();
            Q_EMIT commandAskedToAgent(_recorderAgent->peerId().split(","), QString("DELETE_RECORD#%1").arg(idToRemove));
        }

        // Remove it from the list
        _recordsList.remove(_selectedRecord);

        // Delete each model of record
        _modelManager->deleteRecordModel(_selectedRecord->recordModel());

        // Delete the view model of record
        delete _selectedRecord;

        setselectedRecord(NULL);
    }
}


/**
 * @brief Delete the selected agent from the list
 */
void RecordsSupervisionController::controlRecord(QString recordId, bool startPlaying)
{
    if(_recorderAgent != NULL)
    {
        QString command = startPlaying? "PLAY_RECORD#" : "PAUSE_RECORD#";
        setplayingRecordId(startPlaying ? recordId : "");
        Q_EMIT commandAskedToAgent(_recorderAgent->peerId().split(","), QString("%1%2").arg(command).arg(recordId));
    }
}


/**
 * @brief Custom setter on is recording command for the scenario
 * @param is recording flag
 */
void RecordsSupervisionController::setisRecording(bool isRecording)
{
    if(_isRecording != isRecording)
    {
        _isRecording = isRecording;

        Q_EMIT isRecordingChanged(_isRecording);

        if(_recorderAgent != NULL)
        {
            QString command = _isRecording? "START_RECORD" : "STOP_RECORD";
            Q_EMIT commandAskedToAgent(_recorderAgent->peerId().split(","), command);
        }

        // Update display of elapsed time
        if(isRecording)
        {
            _timerToDisplayTime.start();
        }
        else
        {
            _timerToDisplayTime.stop();
            setcurrentRecordTime(QTime::fromMSecsSinceStartOfDay(0));
        }
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
 * @brief Aims at deleting VM and model of a record
 * @param record
 */
void RecordsSupervisionController::_deleteRecordVM(RecordVM* record)
{
    // Delete each model of record
    _modelManager->deleteRecordModel(record->recordModel());

    // Delete the view model of record
    delete record;
}


