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


static const QString prefix_Muted = "MUTED";
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

        connect(ingeScapeNetworkC, &IngeScapeNetworkController::shoutedMessageReceived,
                this, &NetworkController::_onShoutedMessageReceived);
        connect(ingeScapeNetworkC, &IngeScapeNetworkController::whisperedMessageReceived,
                this, &NetworkController::_onWhisperedMessageReceived);
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
        igs_result_t success = igs_busSendStringToAgent(peerIdOfExpe.toStdString().c_str(),
                                                        "%s=%s STATUS=%d",
                                                        command.toStdString().c_str(),
                                                        commandParameters.toStdString().c_str(),
                                                        status);

        qInfo() << "Send execution status" << status << "of command" << command << "with parameters" << commandParameters << "to expe" << peerIdOfExpe << "with success ?" << success;
    }
}


/**
 * @brief Slot called when a "Shouted" message has been received
 */
void NetworkController::_onShoutedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters)
{
    if (peer != nullptr)
    {
        QString peerId = peer->uid();

        // MUTED / UN-MUTED
        if (messageType == prefix_Muted)
        {
            QString isMuted = messageParameters.at(0);
            QString agentUid = messageParameters.at(1);

            if (isMuted == "1") {
                Q_EMIT isMutedFromAgentUpdated(peerId, agentUid, true);
            }
            else {
                Q_EMIT isMutedFromAgentUpdated(peerId, agentUid, false);
            }
        }
        // FROZEN / UN-FROZEN
        else if (messageType.startsWith(prefix_Frozen))
        {
            QString isFrozen = messageType.remove(0, prefix_Frozen.length());

            if (isFrozen == "1") {
                Q_EMIT isFrozenFromAgentUpdated(peerId, true);
            }
            else {
                Q_EMIT isFrozenFromAgentUpdated(peerId, false);
            }
        }
        // OUTPUT MUTED
        else if (messageType.startsWith(prefix_OutputMuted))
        {
            QString outputName = messageType.remove(0, prefix_OutputMuted.length());

            Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
        }
        // OUTPUT UN-MUTED
        else if (messageType.startsWith(prefix_OutputUnmuted))
        {
            QString outputName = messageType.remove(0, prefix_OutputUnmuted.length());

            Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
        }
        // STATE
        else if (messageType.startsWith(prefix_State))
        {
            QString stateName = messageType.remove(0, prefix_State.length());

            Q_EMIT agentStateChanged(peerId, stateName);
        }
        // LOG IN STREAM
        else if (messageType.startsWith(prefix_LogInStream))
        {
            QString hasLogInStream = messageType.remove(0, prefix_LogInStream.length());

            if (hasLogInStream == "1") {
                Q_EMIT agentHasLogInStream(peerId, true);
            }
            else {
                Q_EMIT agentHasLogInStream(peerId, false);
            }
        }
        // LOG IN FILE
        else if (messageType.startsWith(prefix_LogInFile))
        {
            QString hasLogInFile = messageType.remove(0, prefix_LogInFile.length());

            if (hasLogInFile == "1") {
                Q_EMIT agentHasLogInFile(peerId, true);
            }
            else {
                Q_EMIT agentHasLogInFile(peerId, false);
            }
        }
        // LOG FILE PATH
        else if (messageType.startsWith(prefix_LogFilePath))
        {
            QString logFilePath = messageType.remove(0, prefix_LogFilePath.length());

            Q_EMIT agentLogFilePath(peerId, logFilePath);
        }
        // DEFINITION FILE PATH
        else if (messageType.startsWith(prefix_DefinitionFilePath))
        {
            QString definitionFilePath = messageType.remove(0, prefix_DefinitionFilePath.length());

            Q_EMIT agentDefinitionFilePath(peerId, definitionFilePath);
        }
        // MAPPING FILE PATH
        else if (messageType.startsWith(prefix_MappingFilePath))
        {
            QString mappingFilePath = messageType.remove(0, prefix_MappingFilePath.length());

            Q_EMIT agentMappingFilePath(peerId, mappingFilePath);
        }
        // Unknown
        else
        {
            qWarning() << "Not yet managed SHOUTED message '" << messageType << "' with parameters" << messageParameters << "from peer" << peer->name() << "(" << peer->uid() << ")";
        }
    }
}


/**
 * @brief Slot called when "Whispered" message (with one part) has been received
 * @param peerId
 * @param peerName
 * @param message
 */
void NetworkController::_onWhisperedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters)
{
    if (peer != nullptr)
    {
        QString peerId = peer->uid();

        // MUTED / UN-MUTED
        if (messageType == prefix_Muted)
        {
            QString isMuted = messageParameters.at(0);
            QString agentUid = messageParameters.at(1);

            if (isMuted == "1") {
                Q_EMIT isMutedFromAgentUpdated(peerId, agentUid, true);
            }
            else {
                Q_EMIT isMutedFromAgentUpdated(peerId, agentUid, false);
            }
        }
        // FROZEN / UN-FROZEN
        else if (messageType.startsWith(prefix_Frozen))
        {
            QString isFrozen = messageType.remove(0, prefix_Frozen.length());

            if (isFrozen == "1") {
                Q_EMIT isFrozenFromAgentUpdated(peerId, true);
            }
            else {
                Q_EMIT isFrozenFromAgentUpdated(peerId, false);
            }
        }
        // OUTPUT MUTED
        else if (messageType.startsWith(prefix_OutputMuted))
        {
            QString outputName = messageType.remove(0, prefix_OutputMuted.length());

            Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
        }
        // OUTPUT UN-MUTED
        else if (messageType.startsWith(prefix_OutputUnmuted))
        {
            QString outputName = messageType.remove(0, prefix_OutputUnmuted.length());

            Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
        }
        // STATE
        else if (messageType.startsWith(prefix_State))
        {
            QString stateName = messageType.remove(0, prefix_State.length());

            Q_EMIT agentStateChanged(peerId, stateName);
        }
        // LOG IN STREAM
        else if (messageType.startsWith(prefix_LogInStream))
        {
            QString hasLogInStream = messageType.remove(0, prefix_LogInStream.length());

            if (hasLogInStream == "1") {
                Q_EMIT agentHasLogInStream(peerId, true);
            }
            else {
                Q_EMIT agentHasLogInStream(peerId, false);
            }
        }
        // LOG IN FILE
        else if (messageType.startsWith(prefix_LogInFile))
        {
            QString hasLogInFile = messageType.remove(0, prefix_LogInFile.length());

            if (hasLogInFile == "1") {
                Q_EMIT agentHasLogInFile(peerId, true);
            }
            else {
                Q_EMIT agentHasLogInFile(peerId, false);
            }
        }
        // LOG FILE PATH
        else if (messageType.startsWith(prefix_LogFilePath))
        {
            QString logFilePath = messageType.remove(0, prefix_LogFilePath.length());

            Q_EMIT agentLogFilePath(peerId, logFilePath);
        }
        // DEFINITION FILE PATH
        else if (messageType.startsWith(prefix_DefinitionFilePath))
        {
            QString definitionFilePath = messageType.remove(0, prefix_DefinitionFilePath.length());

            Q_EMIT agentDefinitionFilePath(peerId, definitionFilePath);
        }
        // MAPPING FILE PATH
        else if (messageType.startsWith(prefix_MappingFilePath))
        {
            QString mappingFilePath = messageType.remove(0, prefix_MappingFilePath.length());

            Q_EMIT agentMappingFilePath(peerId, mappingFilePath);
        }
        // The "Recorder app" Started to record
        else if (messageType.startsWith(prefix_RecordStarted))
        {
            qInfo() << prefix_RecordStarted;

            Q_EMIT recordStartedReceived();
        }
        // The "Recorder app" Stopped to record
        else if (messageType.startsWith(prefix_RecordStopped))
        {
            qInfo() << prefix_RecordStopped;

            Q_EMIT recordStoppedReceived();
        }
        // All records
        else if (messageType.startsWith(prefix_AllRecords))
        {
            messageType.remove(0, prefix_AllRecords.length());

            // Emit the signal "All records Received"
            Q_EMIT allRecordsReceived(messageType);
        }
        // Added record
        else if (messageType.startsWith(prefix_AddedRecord))
        {
            messageType.remove(0, prefix_AddedRecord.length());

            // Emit the signal "Added record received"
            Q_EMIT addedRecordReceived(messageType);
        }
        // Deleted Record
        else if (messageType.startsWith(prefix_DeletedRecord))
        {
            messageType.remove(0, prefix_DeletedRecord.length());

            Q_EMIT deletedRecordReceived(messageType);
        }
        // A replay has been loaded
        else if (messageType == prefix_ReplayLoaded)
        {
            qDebug() << prefix_ReplayLoaded;

            Q_EMIT replayLoadedReceived();
        }
        // A replay has been UN-loaded
        else if (messageType == prefix_ReplayUNloaded)
        {
            qDebug() << prefix_ReplayUNloaded;

            Q_EMIT replayUNloadedReceived();
        }
        // A replay has ended
        else if (messageType == prefix_ReplayEnded)
        {
            qDebug() << prefix_ReplayEnded;

            Q_EMIT replayEndedReceived();
        }
        // HIGHLIGHT LINK
        else if (messageType.startsWith(prefix_HighlightLink))
        {
            messageType.remove(0, prefix_HighlightLink.length());

            Q_EMIT highlightLink(messageType.split('|'));
        }
        // RUN (THIS) ACTION
        else if (messageType.startsWith(prefix_RunAction))
        {
            messageType.remove(0, prefix_RunAction.length());

            Q_EMIT runAction(messageType);
        }
        // LOAD PLATFORM FROM PATH
        else if (messageType.startsWith(prefix_LoadPlatformFile))
        {
            messageType.remove(0, prefix_LoadPlatformFile.length());

            Q_EMIT loadPlatformFileFromPath(messageType);
        }
        // Update the state of the TimeLine (Play/Pause/Stop)
        else if (messageType.startsWith(prefix_UpdateTimeLineState))
        {
            QString state = messageType.remove(0, prefix_UpdateTimeLineState.length());

            Q_EMIT updateTimeLineState(state);
        }
        // Update the state of the Record (Start/Stop)
        else if (messageType.startsWith(prefix_UpdateRecordState))
        {
            QString state = messageType.remove(0, prefix_UpdateRecordState.length());

            Q_EMIT updateRecordState(state);
        }
        // A replay is currently loading
        if (messageType == prefix_ReplayLoading)
        {
            // Check that there are still 3 others parts
            if (messageParameters.count() == 3)
            {
                qint64 deltaTimeFromTimeLineStart = messageParameters.at(0).toLongLong();
                QString jsonPlatform = messageParameters.at(1);
                QString jsonExecutedActions = messageParameters.at(2);

                // Emit the signal "Replay Loading received"
                Q_EMIT replayLoadingReceived(static_cast<int>(deltaTimeFromTimeLineStart), jsonPlatform, jsonExecutedActions);
            }
        }
        // A record has been exported
        else if (messageType == prefix_RecordExported)
        {
            Q_EMIT recordExported();
        }
        // Unknown
        else
        {
            qWarning() << "Not yet managed WHISPERED message '" << messageType << "' with parameters" << messageParameters << "from peer" << peer->name() << "(" << peer->uid() << ")";
        }
    }
}

