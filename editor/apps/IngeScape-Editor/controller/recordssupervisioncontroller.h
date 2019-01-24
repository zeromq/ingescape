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

#include <controller/ingescapemodelmanager.h>
#include <viewModel/recordvm.h>
#include <model/recordm.h>
#include <model/agentm.h>


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

    // Sorted list of Record
    I2_QOBJECT_LISTMODEL(RecordVM, recordsList)

    // Selected Record in the Records list
    I2_QML_PROPERTY_DELETE_PROOF(RecordVM*, selectedRecord)

    // Flag indicating if a recorder is currently recording
    I2_QML_PROPERTY(bool, isRecording)

    // Flag indicating if the actions in the timeline are currently recorded
    I2_QML_PROPERTY(bool, isRecordingTimeLine)

    // Indicating if a record is being loaded
    I2_QML_PROPERTY(bool, isLoadingRecord)

    // Stores the currently played record
    I2_QML_PROPERTY(RecordVM*, playingRecord)

    // Current elapsed time of our record
    //I2_QML_PROPERTY(QTime, currentRecordTime)
    I2_QML_PROPERTY(QDateTime, currentRecordTime)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param jsonHelper
     * @param parent
     */
    explicit RecordsSupervisionController(IngeScapeModelManager* modelManager,
                                          JsonHelper* jsonHelper,
                                          QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~RecordsSupervisionController();


    /**
     * @brief Start/Stop to record (optionaly with the actions in the timeline)
     * @param isStart flag indicating if we start to record or if we stop to record
     * @param withTimeLine flag indicating if the actions in the timeline must be recorded
     */
    Q_INVOKABLE void startOrStopToRecord(bool isStart, bool withTimeLine = false);


    /**
     * @brief Delete a record from the list
     * @param record
     */
    Q_INVOKABLE void deleteRecord(RecordVM* record);


    /**
     * @brief Controls the selected record from the list
     * @param recordId
     * @param startPlaying
     */
    Q_INVOKABLE void controlRecord(QString recordId, bool startPlaying);


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
     * @brief Slot called when a record is loading
     * @param deltaTimeFromTimeLine
     * @param jsonPlatform
     * @param jsonExecutedActions
     */
    void onLoadingRecord(int deltaTimeFromTimeLine, QString jsonPlatform, QString jsonExecutedActions);


    /**
     * @brief Slot called when a record has been loaded
     */
    void onLoadedRecord();


    /**
     * @brief Slot called when a record playing has ended
     */
    void onEndOfRecord();


private Q_SLOTS:

    /**
     * @brief Called at each interval of our timer to display elapsed time
     */
    void _onTimeout_DisplayTime();


private:

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
    IngeScapeModelManager* _modelManager;

    // Helper to manage JSON files
    JsonHelper* _jsonHelper;

    // Hash table from record id to a model of record
    QHash<QString, RecordM*> _hashFromRecordIdToModel;

    // Hash table from record id to a view model of record
    QHash<QString, RecordVM*> _hashFromRecordIdToViewModel;

    // Timer to rdisplay elapsed time on current record
    QTimer _timerToDisplayTime;

};

QML_DECLARE_TYPE(RecordsSupervisionController)

#endif // RECORDSSUPERVISIONCONTROLLER_H
