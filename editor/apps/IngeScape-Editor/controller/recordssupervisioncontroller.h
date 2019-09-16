/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *
 */

#ifndef RECORDSSUPERVISIONCONTROLLER_H
#define RECORDSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/editormodelmanager.h>
#include <viewModel/recordvm.h>
#include <model/recordm.h>


/**
  * States for the replay
  */
I2_ENUM(ReplayStates, UNLOADED, LOADING, LOADED, PLAYING, PAUSED, RESUMING)


/**
 * @brief The RecordsSupervisionController class defines the controller for Records supervision
 */
class RecordsSupervisionController : public QObject
{
    Q_OBJECT

    // Peer id of the recorder
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfRecorder)

    // Peer name of the recorder
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfRecorder)

    // Flag indicating is there is a recorder with state ON
    I2_QML_PROPERTY_READONLY(bool, isRecorderON)

    // List of records
    I2_QOBJECT_LISTMODEL(RecordVM, recordsList)

    // Selected Record in the Records list
    //I2_QML_PROPERTY_DELETE_PROOF(RecordVM*, selectedRecord)
    I2_QML_PROPERTY_CUSTOM_SETTER(RecordVM*, selectedRecord)

    // Flag indicating if the recorder is currently recording
    I2_QML_PROPERTY(bool, isRecording)

    // Flag indicating if the actions in the timeline are currently recorded
    //I2_QML_PROPERTY(bool, isRecordingTimeLine)

    // Current state of the replay
    I2_QML_PROPERTY(ReplayStates::Value, replayState)

    // Current replay (loaded record)
    I2_QML_PROPERTY(RecordVM*, currentReplay)

    // Current elapsed time of our record
    I2_QML_PROPERTY(QDateTime, currentRecordTime)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit RecordsSupervisionController(EditorModelManager* modelManager,
                                          QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordsSupervisionController();


    /**
     * @brief Start/Stop to record (optionaly with the actions in the timeline)
     * @param withTimeLine flag indicating if the actions in the timeline must be recorded
     */
    Q_INVOKABLE void startOrStopToRecord(bool withTimeLine = false);


    /**
     * @brief Delete a record from the list
     * @param record
     */
    Q_INVOKABLE void deleteRecord(RecordVM* record);


    /**
     * @brief Load a record
     * @param recordId
     */
    Q_INVOKABLE void loadRecord(QString recordId);


    /**
     * @brief UN-load the current loaded record (replay)
     */
    Q_INVOKABLE void unloadRecord();


    /**
     * @brief Start or Resume the current loaded record (replay)
     * @param isStart
     */
    Q_INVOKABLE void startOrResumeReplay(bool isStart);


    /**
     * @brief Stop or Pause the current loaded record (replay)
     * @param isStop
     */
    Q_INVOKABLE void stopOrPauseReplay(bool isStop);


    /**
     * @brief Export a record
     * @param recordId
     */
    Q_INVOKABLE void exportRecord(QString recordId);


    /**
     * @brief Get the name of the current replay
     * @return
     */
    QString getCurrentReplayName();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to the recorder
     * @param peerIdOfRecorder
     * @param commandAndParameters
     */
    void commandAskedToRecorder(QString peerIdOfRecorder, QString commandAndParameters);


    /**
     * @brief Signal emitted when the user wants to start to record
     */
    void startToRecord();


public Q_SLOTS:

    /**
     * @brief Slot called when a recorder enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Slot called when a recorder quit the network
     * @param peerId
     * @param peerName
     */
    void onRecorderExited(QString peerId, QString peerName);


    /**
     * @brief Slot called when all records of DB have been received
     * @param list of records in JSON format
     */
    void onAllRecordsReceived(QString records);


    /**
     * @brief Slot called when the "Recorder app" started to record
     */
    void onRecordStartedReceived();


    /**
     * @brief Slot called when the "Recorder app" stopped to record
     */
    void onRecordStoppedReceived();


    /**
     * @brief Slot called when a new record has been added (into the DB)
     * @param record in JSON format
     */
    void onAddedRecord(QString record);


    /**
     * @brief Slot called when a record has been deleted
     * @param recordId
     */
    void onDeletedRecord(QString recordId);


    /**
     * @brief Slot called when a replay has been loaded
     */
    void onReplayLoaded();


    /**
     * @brief Slot called when a replay has been UN-loaded
     */
    void onReplayUNloaded();


    /**
     * @brief Slot called when a replay has ended
     */
    void onReplayEnded();


    /**
     * @brief Slot called when a record has been exported
     */
    void onRecordExported();


private Q_SLOTS:

    /**
     * @brief Called at each interval of our timer to display elapsed time
     */
    void _onTimeout_DisplayTime();


private:

    /**
     * @brief Create a model of record from JSON data
     * @param byteArrayOfJson
     * @return
     */
    QList<RecordM*> _createRecordsListFromJSON(QByteArray byteArrayOfJson);


    /**
     * @brief Create a view model of record with a model
     * @param model
     */
    void _createRecordVMwithModel(RecordM* model);


    /**
     * @brief Delete a view model of record with its model
     * @param model
     */
    void _deleteRecordVMwithModel(RecordM* model);


private:

    // Manager for the data model of INGESCAPE
    EditorModelManager* _modelManager;

    // Hash table from record id to a model of record
    QHash<QString, RecordM*> _hashFromRecordIdToModel;

    // Hash table from record id to a view model of record
    QHash<QString, RecordVM*> _hashFromRecordIdToViewModel;

    // Timer to display elapsed time on current record
    QTimer _timerToDisplayTime;

    // Time when user started to record
    QDateTime _recordStartTime;

};

QML_DECLARE_TYPE(RecordsSupervisionController)

#endif // RECORDSSUPERVISIONCONTROLLER_H
