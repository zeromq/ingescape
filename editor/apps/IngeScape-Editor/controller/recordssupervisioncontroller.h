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

    // Flag indicating is there is a recorder with state ON
    I2_QML_PROPERTY_READONLY(bool, isRecorderON)

    // Sorted list of Record
    I2_QOBJECT_LISTMODEL(RecordVM, recordsList)

    // Selected Record in the Records list
    I2_QML_PROPERTY_DELETE_PROOF(RecordVM*, selectedRecord)

    // Indicating if a recorder agent is currently recording
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isRecording)

    // Indicating if a record is being loaded
    I2_QML_PROPERTY(bool, isLoadingRecord)

    // Stores the currently played record
    I2_QML_PROPERTY(RecordVM*, playingRecord)

    // Current elapsed time of our record
    I2_QML_PROPERTY(QTime, currentRecordTime)


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
     * @brief Delete the selected record from the list
     */
    Q_INVOKABLE void deleteSelectedRecord();


    /**
     * @brief Controls the selected record from the list
     * @param recordId
     * @param startPlaying
     */
    Q_INVOKABLE void controlRecord(QString recordId, bool startPlaying);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to the recorder
     * @param commandAndParameters
     */
    void commandAskedToRecorder(QString commandAndParameters);


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

    // Map from record id to a model of record
    QHash<QString, RecordM*> _mapFromRecordIdToModel;

    // Map from record id to a view model of record
    QHash<QString, RecordVM*> _mapFromRecordIdToViewModel;

    // Timer to rdisplay elapsed time on current record
    QTimer _timerToDisplayTime;

    // Peer id of the recorder
    QString _peerIdOfRecorder;

    // Peer name of the recorder
    QString _peerNameOfRecorder;

};

QML_DECLARE_TYPE(RecordsSupervisionController)

#endif // RECORDSSUPERVISIONCONTROLLER_H
