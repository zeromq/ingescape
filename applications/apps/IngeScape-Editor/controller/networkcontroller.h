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


/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController: public IngeScapeNetworkController
{
    Q_OBJECT


public:

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    static NetworkController* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
     * @brief Constructor
     * @param parent
     */
    explicit NetworkController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~NetworkController() Q_DECL_OVERRIDE;


    /**
     * @brief Manage a "Shouted" message
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    void manageShoutedMessage(QString peerId, QString peerName, zmsg_t* zMessage) Q_DECL_OVERRIDE;


    /**
     * @brief Manage a "Whispered" message
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    void manageWhisperedMessage(QString peerId, QString peerName, zmsg_t* zMessage) Q_DECL_OVERRIDE;


    /**
     * @brief Send a command, parameters and the content of a JSON file to the recorder
     * @param peerIdOfRecorder
     * @param commandAndParameters
     */
    void sendCommandWithJsonToRecorder(QString peerIdOfRecorder, QStringList commandAndParameters);


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


    /**
     * @brief Signal emitted when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void isMutedFromAgentUpdated(QString peerId, bool isMuted);


    /**
     * @brief Signal emitted when the flag "can be Frozen" from an agent updated
     * @param peerId
     * @param canBeFrozen
     */
    void canBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen);


    /**
     * @brief Signal emitted when the flag "is Frozen" from an agent updated
     * @param peerId
     * @param isFrozen
     */
    void isFrozenFromAgentUpdated(QString peerId, bool isFrozen);


    /**
     * @brief Signal emitted when the flag "is Muted" from an output of agent updated
     * @param peerId
     * @param isMuted
     * @param outputName
     */
    void isMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName);


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

    /**
     * @brief Slot called when a command must be sent on the network to a launcher
     * @param peerIdOfLauncher
     * @param command
     * @param commandLine
     */
    void onCommandAskedToLauncher(QString peerIdOfLauncher, QString command, QString commandLine);


    /**
     * @brief Slot called when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void onCommandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about one of its output
     * @param peerIdsList
     * @param command
     * @param outputName
     */
    void onCommandAskedToAgentAboutOutput(QStringList peerIdsList, QString command, QString outputName);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about setting a value to one of its Input/Output/Parameter
     * @param peerIdsList
     * @param command
     * @param agentIOPName
     * @param value
     */
    void onCommandAskedToAgentAboutSettingValue(QStringList peerIdsList, QString command, QString agentIOPName, QString value);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about mapping one of its input
     * @param peerIdsList
     * @param command
     * @param inputName
     * @param outputAgentName
     * @param outputName
     */
    void onCommandAskedToAgentAboutMappingInput(QStringList peerIdsList, QString command, QString inputName, QString outputAgentName, QString outputName);


private:

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
