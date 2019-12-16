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

#include "networkcontroller.h"

#include <QDebug>
#include <QApplication>


static const QString prefix_Muted = "MUTED=";
static const QString prefix_CanBeFrozen = "CANBEFROZEN=";
static const QString prefix_Frozen = "FROZEN=";
static const QString prefix_OutputMuted = "OUTPUT_MUTED ";
static const QString prefix_OutputUnmuted = "OUTPUT_UNMUTED ";
static const QString prefix_State = "STATE=";
static const QString prefix_LogInStream = "LOG_IN_STREAM=";
static const QString prefix_LogInFile = "LOG_IN_FILE=";
static const QString prefix_LogFilePath = "LOG_FILE_PATH=";
static const QString prefix_DefinitionFilePath = "DEFINITION_FILE_PATH=";
static const QString prefix_MappingFilePath = "MAPPING_FILE_PATH=";

static const QString prefix_AllRecords = "RECORDS_LIST=";
static const QString prefix_DeletedRecord = "DELETED_RECORD=";
static const QString prefix_RecordExported = "RECORD_EXPORTED";
static const QString prefix_ReplayLoading = "REPLAY_LOADING=";
static const QString prefix_ReplayLoaded = "REPLAY_LOADED";
static const QString prefix_ReplayUNloaded = "REPLAY_UNLOADED";
static const QString prefix_ReplayEnded = "REPLAY_ENDED";

static const QString prefix_HighlightLink = "HIGHLIGHT_LINK=";
static const QString prefix_RunAction = "RUN_THIS_ACTION#";
static const QString prefix_LoadPlatformFile = "LOAD_PLATFORM_FROM_PATH=";
static const QString prefix_UpdateTimeLineState = "UPDATE_TIMELINE_STATE=";
static const QString prefix_UpdateRecordState = "UPDATE_RECORD_STATE=";


//--------------------------------------------------------------
//
// Network Controller
//
//--------------------------------------------------------------

/**
 * @brief Default constructor
 * @param parent
 */
NetworkController::NetworkController(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Add  header to declare ourselves as an editor
    igs_busAddServiceDescription("isEditor", "1");

    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC != nullptr)
    {
        // We don't see itself
        ingeScapeNetworkC->setnumberOfEditors(1);

        connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString)));
        connect(ingeScapeNetworkC, SIGNAL(shoutedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onShoutedMessageReceived(QString, QString, QString, QStringList)));

        connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString)));
        connect(ingeScapeNetworkC, SIGNAL(whisperedMessageReceived(QString, QString, QString, QStringList)),
                this, SLOT(_onWhisperedMessageReceived(QString, QString, QString, QStringList)));
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC != nullptr)
    {
        // DIS-connect from the IngeScape Network Controller
        disconnect(ingeScapeNetworkC, nullptr, this, nullptr);
    }
}


/**
 * @brief Send a command execution status to the expe
 * @param command
 * @param commandParameters
 * @param status
 */
void NetworkController::sendCommandExecutionStatusToExpe(QString peerIdOfExpe, QString command, QString commandParameters, int status)
{
    if (!peerIdOfExpe.isEmpty())
    {
        // Send the execution status of command with parameters to the peer id of the expe
        int success = igs_busSendStringToAgent(peerIdOfExpe.toStdString().c_str(),
                                               "%s=%s STATUS=%d",
                                               command.toStdString().c_str(),
                                               commandParameters.toStdString().c_str(),
                                               status);

        qInfo() << "Send execution status" << status << "of command" << command << "with parameters" << commandParameters << "to expe" << peerIdOfExpe << "with success ?" << success;
    }
}


/**
 * @brief Slot called when a "Shouted" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onShoutedMessageReceived(QString peerId, QString peerName, QString message)
{
    // MUTED / UN-MUTED
    if (message.startsWith(prefix_Muted))
    {
        QString isMuted = message.remove(0, prefix_Muted.length());

        if (isMuted == "1") {
            Q_EMIT isMutedFromAgentUpdated(peerId, true);
        }
        else {
            Q_EMIT isMutedFromAgentUpdated(peerId, false);
        }
    }
    // CAN BE FROZEN / CAN NOT BE FROZEN
    else if (message.startsWith(prefix_CanBeFrozen))
    {
        QString canBeFrozen = message.remove(0, prefix_CanBeFrozen.length());

        if (canBeFrozen == "1") {
            Q_EMIT canBeFrozenFromAgentUpdated(peerId, true);
        }
        else {
            Q_EMIT canBeFrozenFromAgentUpdated(peerId, false);
        }
    }
    // FROZEN / UN-FROZEN
    else if (message.startsWith(prefix_Frozen))
    {
        QString isFrozen = message.remove(0, prefix_Frozen.length());

        if (isFrozen == "1") {
            Q_EMIT isFrozenFromAgentUpdated(peerId, true);
        }
        else {
            Q_EMIT isFrozenFromAgentUpdated(peerId, false);
        }
    }
    // OUTPUT MUTED
    else if (message.startsWith(prefix_OutputMuted))
    {
        QString outputName = message.remove(0, prefix_OutputMuted.length());

        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
    }
    // OUTPUT UN-MUTED
    else if (message.startsWith(prefix_OutputUnmuted))
    {
        QString outputName = message.remove(0, prefix_OutputUnmuted.length());

        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
    }
    // STATE
    else if (message.startsWith(prefix_State))
    {
        QString stateName = message.remove(0, prefix_State.length());

        Q_EMIT agentStateChanged(peerId, stateName);
    }
    // LOG IN STREAM
    else if (message.startsWith(prefix_LogInStream))
    {
        QString hasLogInStream = message.remove(0, prefix_LogInStream.length());

        if (hasLogInStream == "1") {
            Q_EMIT agentHasLogInStream(peerId, true);
        }
        else {
            Q_EMIT agentHasLogInStream(peerId, false);
        }
    }
    // LOG IN FILE
    else if (message.startsWith(prefix_LogInFile))
    {
        QString hasLogInFile = message.remove(0, prefix_LogInFile.length());

        if (hasLogInFile == "1") {
            Q_EMIT agentHasLogInFile(peerId, true);
        }
        else {
            Q_EMIT agentHasLogInFile(peerId, false);
        }
    }
    // LOG FILE PATH
    else if (message.startsWith(prefix_LogFilePath))
    {
        QString logFilePath = message.remove(0, prefix_LogFilePath.length());

        Q_EMIT agentLogFilePath(peerId, logFilePath);
    }
    // DEFINITION FILE PATH
    else if (message.startsWith(prefix_DefinitionFilePath))
    {
        QString definitionFilePath = message.remove(0, prefix_DefinitionFilePath.length());

        Q_EMIT agentDefinitionFilePath(peerId, definitionFilePath);
    }
    // MAPPING FILE PATH
    else if (message.startsWith(prefix_MappingFilePath))
    {
        QString mappingFilePath = message.remove(0, prefix_MappingFilePath.length());

        Q_EMIT agentMappingFilePath(peerId, mappingFilePath);
    }
    // Unknown
    else
    {
        qDebug() << "Not yet managed SHOUTED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
    }
}


/**
 * @brief Slot called when a "Shouted" message (with several parts) has been received
 * @param peerId
 * @param peerName
 * @param messagePart1
 * @param messageOthersParts
 */
void NetworkController::_onShoutedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts)
{
    qDebug() << "Not yet managed SHOUTED message '" << messagePart1 << "+" << messageOthersParts << "' for agent" << peerName << "(" << peerId << ")";
}


/**
 * @brief Slot called when "Whispered" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, QString message)
{
    // MUTED / UN-MUTED
    if (message.startsWith(prefix_Muted))
    {
        QString isMuted = message.remove(0, prefix_Muted.length());

        if (isMuted == "1") {
            Q_EMIT isMutedFromAgentUpdated(peerId, true);
        }
        else {
            Q_EMIT isMutedFromAgentUpdated(peerId, false);
        }
    }
    // FROZEN / UN-FROZEN
    else if (message.startsWith(prefix_Frozen))
    {
        QString isFrozen = message.remove(0, prefix_Frozen.length());

        if (isFrozen == "1") {
            Q_EMIT isFrozenFromAgentUpdated(peerId, true);
        }
        else {
            Q_EMIT isFrozenFromAgentUpdated(peerId, false);
        }
    }
    // OUTPUT MUTED
    else if (message.startsWith(prefix_OutputMuted))
    {
        QString outputName = message.remove(0, prefix_OutputMuted.length());

        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
    }
    // OUTPUT UN-MUTED
    else if (message.startsWith(prefix_OutputUnmuted))
    {
        QString outputName = message.remove(0, prefix_OutputUnmuted.length());

        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
    }
    // STATE
    else if (message.startsWith(prefix_State))
    {
        QString stateName = message.remove(0, prefix_State.length());

        Q_EMIT agentStateChanged(peerId, stateName);
    }
    // LOG IN STREAM
    else if (message.startsWith(prefix_LogInStream))
    {
        QString hasLogInStream = message.remove(0, prefix_LogInStream.length());

        if (hasLogInStream == "1") {
            Q_EMIT agentHasLogInStream(peerId, true);
        }
        else {
            Q_EMIT agentHasLogInStream(peerId, false);
        }
    }
    // LOG IN FILE
    else if (message.startsWith(prefix_LogInFile))
    {
        QString hasLogInFile = message.remove(0, prefix_LogInFile.length());

        if (hasLogInFile == "1") {
            Q_EMIT agentHasLogInFile(peerId, true);
        }
        else {
            Q_EMIT agentHasLogInFile(peerId, false);
        }
    }
    // LOG FILE PATH
    else if (message.startsWith(prefix_LogFilePath))
    {
        QString logFilePath = message.remove(0, prefix_LogFilePath.length());

        Q_EMIT agentLogFilePath(peerId, logFilePath);
    }
    // DEFINITION FILE PATH
    else if (message.startsWith(prefix_DefinitionFilePath))
    {
        QString definitionFilePath = message.remove(0, prefix_DefinitionFilePath.length());

        Q_EMIT agentDefinitionFilePath(peerId, definitionFilePath);
    }
    // MAPPING FILE PATH
    else if (message.startsWith(prefix_MappingFilePath))
    {
        QString mappingFilePath = message.remove(0, prefix_MappingFilePath.length());

        Q_EMIT agentMappingFilePath(peerId, mappingFilePath);
    }
    // The "Recorder app" Started to record
    else if (message.startsWith(prefix_RecordStarted))
    {
        qInfo() << prefix_RecordStarted;

        Q_EMIT recordStartedReceived();
    }
    // The "Recorder app" Stopped to record
    else if (message.startsWith(prefix_RecordStopped))
    {
        qInfo() << prefix_RecordStopped;

        Q_EMIT recordStoppedReceived();
    }
    // All records
    else if (message.startsWith(prefix_AllRecords))
    {
        message.remove(0, prefix_AllRecords.length());

        // Emit the signal "All records Received"
        Q_EMIT allRecordsReceived(message);
    }
    // Added record
    else if (message.startsWith(prefix_AddedRecord))
    {
        message.remove(0, prefix_AddedRecord.length());

        // Emit the signal "Added record received"
        Q_EMIT addedRecordReceived(message);
    }
    // Deleted Record
    else if (message.startsWith(prefix_DeletedRecord))
    {
        message.remove(0, prefix_DeletedRecord.length());

        Q_EMIT deletedRecordReceived(message);
    }
    // A replay has been loaded
    else if (message == prefix_ReplayLoaded)
    {
        qDebug() << prefix_ReplayLoaded;

        Q_EMIT replayLoadedReceived();
    }
    // A replay has been UN-loaded
    else if (message == prefix_ReplayUNloaded)
    {
        qDebug() << prefix_ReplayUNloaded;

        Q_EMIT replayUNloadedReceived();
    }
    // A replay has ended
    else if (message == prefix_ReplayEnded)
    {
        qDebug() << prefix_ReplayEnded;

        Q_EMIT replayEndedReceived();
    }
    // HIGHLIGHT LINK
    else if (message.startsWith(prefix_HighlightLink))
    {
        message.remove(0, prefix_HighlightLink.length());

        Q_EMIT highlightLink(message.split('|'));
    }
    // RUN (THIS) ACTION
    else if (message.startsWith(prefix_RunAction))
    {
        message.remove(0, prefix_RunAction.length());

        Q_EMIT runAction(message);
    }
    // LOAD PLATFORM FROM PATH
    else if (message.startsWith(prefix_LoadPlatformFile))
    {
        message.remove(0, prefix_LoadPlatformFile.length());

        Q_EMIT loadPlatformFileFromPath(message);
    }
    // Update the state of the TimeLine (Play/Pause/Stop)
    else if (message.startsWith(prefix_UpdateTimeLineState))
    {
        QString state = message.remove(0, prefix_UpdateTimeLineState.length());

        Q_EMIT updateTimeLineState(state);
    }
    // Update the state of the Record (Start/Stop)
    else if (message.startsWith(prefix_UpdateRecordState))
    {
        QString state = message.remove(0, prefix_UpdateRecordState.length());

        Q_EMIT updateRecordState(state);
    }
    // A record has been exported
    else if (message == prefix_RecordExported)
    {
        Q_EMIT recordExported();
    }
    // Unknown
    else
    {
        qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
    }
}


/**
 * @brief Slot called when "Whispered" message (with several parts) has been received
 * @param peerId
 * @param peerName
 * @param messagePart1
 * @param messageOthersParts
 */
void NetworkController::_onWhisperedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts)
{
    // A replay is currently loading
    if (messagePart1 == prefix_ReplayLoading)
    {
        // Check that there are still 3 others parts
        if (messageOthersParts.count() == 3)
        {
            QString deltaTimeFromTimeLineStart = messageOthersParts.at(0);
            QString jsonPlatform = messageOthersParts.at(1);
            QString jsonExecutedActions = messageOthersParts.at(2);

            // Emit the signal "Replay Loading received"
            Q_EMIT replayLoadingReceived(deltaTimeFromTimeLineStart.toInt(), jsonPlatform, jsonExecutedActions);
        }
    }
    // Unknown
    else
    {
        qDebug() << "Not yet managed WHISPERED message '" << messagePart1 << "+" << messageOthersParts << "' for agent" << peerName << "(" << peerId << ")";
    }
}

