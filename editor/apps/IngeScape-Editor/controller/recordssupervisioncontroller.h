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

    // Reference on recorder agent
    I2_QML_PROPERTY(AgentM*, recorderAgent)

    // Sorted list of Record
    I2_QOBJECT_LISTMODEL(RecordVM, recordsList)

    // Selected Record in the Records list
    I2_QML_PROPERTY_DELETE_PROOF(RecordVM*, selectedRecord)

    // Indicating if a recorder agent is currently recording
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isRecording)

    // Stores the id of the currently played record
    I2_QML_PROPERTY(QString, playingRecordId)

    // Current elapsed time of our record
    I2_QML_PROPERTY(QTime, currentRecordTime)


public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit RecordsSupervisionController(IngeScapeModelManager* modelManager, QObject *parent = nullptr);


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
     */
    Q_INVOKABLE void controlRecord(QString recordId, bool startPlaying);

Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


public Q_SLOTS:


    /**
     * @brief Slot when the list of records model changes
     * @param records
     */
    void onRecordsListChanged(QList<RecordM*> records);

    /**
     * @brief Slot when a new model of record has been added
     * @param records
     */
    void onRecordAdded(RecordM* record);

    /**
     * @brief Slot when a new model of agent has been created
     * @param agent
     */
    void onAgentModelCreated(AgentM* model);


private Q_SLOTS:

    /**
     * @brief Called at each interval of our timer to display elapsed time
     */
    void _onTimeout_DisplayTime();

private:

    /**
     * @brief Aims at deleting VM and model of a record
     * @param record
     */
    void _deleteRecordVM(RecordVM* record);

private:

    // Manager for the data model of INGESCAPE
    IngeScapeModelManager* _modelManager;

    // Map from record name to a list of view models of record
    QHash<RecordM*, RecordVM*> _mapFromRecordModelToViewModel;

    // Timer to rdisplay elapsed time on current record
    QTimer _timerToDisplayTime;
};

QML_DECLARE_TYPE(RecordsSupervisionController)

#endif // RECORDSSUPERVISIONCONTROLLER_H
