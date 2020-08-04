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
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <controller/ingescapenetworkcontroller.h>

extern "C" {
#include <czmq.h>
}


/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit NetworkController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~NetworkController();


    /**
     * @brief Send a command execution status to the expe
     * @param command
     * @param commandParameters
     * @param status
     */
    void sendCommandExecutionStatusToExpe(QString peerIdOfExpe, QString command, QString commandParameters, int status);


Q_SIGNALS:

    /**
     * @brief Signal emitted when the "Recorder app" started to record
     */
    void recordStartedReceived();


    /**
     * @brief Signal emitted when the "Recorder app" stopped to record
     */
    void recordStoppedReceived();


    /**
     * @brief Signal emitted when all records from DB have been received
     * @param recordsJSON
     */
    void allRecordsReceived(QString recordsJSON);


    /**
     * @brief Signal emitted when a new record has been added
     * @param recordJSON
     */
    void addedRecordReceived(QString recordJSON);


    /**
     * @brief Signal emitted when a record has been deleted
     * @param recordId
     */
    void deletedRecordReceived(QString recordId);


    /**
     * @brief Signal emitted when a replay is currently loading
     * @param deltaTimeFromTimeLineStart
     * @param jsonPlatform
     * @param jsonExecutedActions
     */
    void replayLoadingReceived(int deltaTimeFromTimeLineStart, QString jsonPlatform, QString jsonExecutedActions);


    /**
     * @brief Signal emitted when a replay has been loaded
     */
    void replayLoadedReceived();


    /**
     * @brief Signal emitted when a replay has been UN-loaded
     */
    void replayUNloadedReceived();


    /**
     * @brief Signal emitted when a replay has ended
     */
    void replayEndedReceived();


    // Signal emitted when the flag "is Muted" from an agent/agent output has been updated
    void isMutedFromAgentUpdated(QString peerId, QString agentUid, bool isMuted);
    void isMutedFromOutputOfAgentUpdated(QString peerId, QString agentUid, bool isMuted, QString outputName);


    // Signal emitted when the flag "is Frozen" from an agent has been updated
    void isFrozenFromAgentUpdated(QString peerId, QString agentUid, bool isFrozen);


    /**
     * @brief Signal emitted when the state of an agent changes
     * @param peerId
     * @param stateName
     */
    void agentStateChanged(QString peerId, QString stateName);


    /**
     * @brief Signal emitted when we receive the flag "Log In Stream" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void agentHasLogInStream(QString peerId, bool hasLogInStream);


    /**
     * @brief Signal emitted when we receive the flag "Log In File" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void agentHasLogInFile(QString peerId, bool hasLogInFile);


    /**
     * @brief Signal emitted when we receive the path of "Log File" for an agent
     * @param peerId
     * @param logFilePath
     */
    void agentLogFilePath(QString peerId, QString logFilePath);


    /**
     * @brief Signal emitted when we receive the path of "Definition File" for an agent
     * @param peerId
     * @param definitionFilePath
     */
    void agentDefinitionFilePath(QString peerId, QString definitionFilePath);


    /**
     * @brief Signal emitted when we receive the path of "Mapping File" for an agent
     * @param peerId
     * @param mappingFilePath
     */
    void agentMappingFilePath(QString peerId, QString mappingFilePath);


    /**
     * @brief Signal emitted when we receive the command "highlight link" from a recorder
     * @param parameters
     */
    void highlightLink(QStringList parameters);


    /**
     * @brief Signal emitted when we receive the command "run action" from a recorder
     * @param actionID
     */
    void runAction(QString actionID);


    /**
     * @brief Signal emitted when we receive the command "Load Platform File From Path"
     * @param platformFilePath
     */
    void loadPlatformFileFromPath(QString platformFilePath);


    /**
     * @brief Signal emitted when we receive the command "Update TimeLine State"
     * @param state
     */
    void updateTimeLineState(QString state);


    /**
     * @brief Signal emitted when we receive the command "Update Record State"
     * @param state
     */
    void updateRecordState(QString state);


    /**
     * @brief Signal emitted when a record has been exported
     */
    void recordExported();


public Q_SLOTS:


private Q_SLOTS:

    // FIXME error: invalid application of 'sizeof' to an incomplete type '_zmsg_t'
    // Q_STATIC_ASSERT_X(sizeof(T), "Type argument of Q_DECLARE_METATYPE(T*) must be fully defined");
    //void _onShoutedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);
    //void _onWhisperedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);


    /**
     * @brief Slot called when a "Shouted" message has been received
     */
    //void _onShoutedMessageReceived(PeerM* peer, QString message);
    void _onShoutedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters);


    /**
     * @brief Slot called when a "Whispered" message has been received
     */
    //void _onWhisperedMessageReceived(PeerM* peer, QString message);
    void _onWhisperedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters);

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
