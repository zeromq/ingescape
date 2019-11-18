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

#include <memory>


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

static const QString prefix_RecordStarted = "RECORD_STARTED";
static const QString prefix_RecordStopped = "RECORD_STOPPED";
static const QString prefix_AllRecords = "RECORDS_LIST=";
static const QString prefix_AddedRecord = "ADDED_RECORD=";
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
NetworkController::NetworkController(QObject *parent) : IngeScapeNetworkController(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Add  header to declare ourselves as an editor
    igs_busAddServiceDescription("isEditor", "1");

    // We don't see itself
    setnumberOfEditors(1);
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Mother class is automatically called
    //IngeScapeNetworkController::~IngeScapeNetworkController();
}


/**
 * @brief Manage a "Shouted" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void NetworkController::manageShoutedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{
    QString message = zmsg_popstr(zMessage);

    // MUTED / UN-MUTED
    if (message.startsWith(prefix_Muted))
    {
        QString isMuted = message.remove(0, prefix_Muted.length());

        // Emit the signal "is Muted from Agent Updated"
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

        // Emit the signal "can be Frozen from Agent Updated"
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

        // Emit the signal "is Frozen from Agent Updated"
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

        // Emit the signal "is Muted from OUTPUT of Agent Updated"
        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
    }
    // OUTPUT UN-MUTED
    else if (message.startsWith(prefix_OutputUnmuted))
    {
        QString outputName = message.remove(0, prefix_OutputUnmuted.length());

        // Emit the signal "is Muted from OUTPUT of Agent Updated"
        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
    }
    // STATE
    else if (message.startsWith(prefix_State))
    {
        QString stateName = message.remove(0, prefix_State.length());

        // Emit the signal "State changed"
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
 * @brief Manage a "Whispered" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void NetworkController::manageWhisperedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{
    std::unique_ptr<char> zmsg_str(zmsg_popstr(zMessage));
    QString message(zmsg_str.get());

    // An agent DEFINITION has been received
    if (message.startsWith(prefix_Definition))
    {
        QString definitionJSON = message.remove(0, prefix_Definition.length());

        Q_EMIT definitionReceived(peerId, peerName, definitionJSON);
    }
    // An agent MAPPING has been received
    else if (message.startsWith(prefix_Mapping))
    {
        QString mappingJSON = message.remove(0, prefix_Mapping.length());

        Q_EMIT mappingReceived(peerId, peerName, mappingJSON);
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
    // A replay is currently loading
    else if (message == prefix_ReplayLoading)
    {
        qDebug() << prefix_ReplayLoading << zmsg_size(zMessage) << "frames";

        // Check that there are still 3 frames
        if (zmsg_size(zMessage) == 3)
        {
            QString deltaTimeFromTimeLineStart = zmsg_popstr(zMessage);
            QString jsonPlatform = zmsg_popstr(zMessage);
            QString jsonExecutedActions = zmsg_popstr(zMessage);

            // Emit the signal "Replay Loading received"
            Q_EMIT replayLoadingReceived(deltaTimeFromTimeLineStart.toInt(), jsonPlatform, jsonExecutedActions);
        }
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
    // MUTED / UN-MUTED
    else if (message.startsWith(prefix_Muted))
    {
        QString isMuted = message.remove(0, prefix_Muted.length());

        // Emit the signal "is Muted from Agent Updated"
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

        // Emit the signal "is Frozen from Agent Updated"
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

        // Emit the signal "is Muted from OUTPUT of Agent Updated"
        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
    }
    // OUTPUT UN-MUTED
    else if (message.startsWith(prefix_OutputUnmuted))
    {
        QString outputName = message.remove(0, prefix_OutputUnmuted.length());

        // Emit the signal "is Muted from OUTPUT of Agent Updated"
        Q_EMIT isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
    }
    // STATE
    else if (message.startsWith(prefix_State))
    {
        QString stateName = message.remove(0, prefix_State.length());

        // Emit the signal "State changed"
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
 * @brief Send a command, parameters and the content of a JSON file to the recorder
 * @param peerIdOfRecorder
 * @param commandAndParameters
 */
void NetworkController::sendCommandWithJsonToRecorder(QString peerIdOfRecorder, QStringList commandAndParameters)
{
    if (!peerIdOfRecorder.isEmpty() && !commandAndParameters.isEmpty())
    {
        // Create ZMQ message
        zmsg_t* msg = zmsg_new();

        for (QString string : commandAndParameters)
        {
            // Add a frame with STRING
            //zframe_t* frameString = zframe_new(string.toStdString().c_str(), string.length() + 1);
            //zmsg_append(msg, &frameString);

            zmsg_addstr(msg, string.toStdString().c_str());
        }

        //int framesNumber = zmsg_size(msg);

        // Send ZMQ message to the recorder
        int success = igs_busSendZMQMsgToAgent(peerIdOfRecorder.toStdString().c_str(), &msg);

        // Do not print the JSON file content
        commandAndParameters.removeLast();
        qInfo() << "Send command, parameters and the content of a JSON file" << commandAndParameters << "to recorder" << peerIdOfRecorder << "with success ?" << success;

        zmsg_destroy(&msg);
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
 * @brief Send a (string) message to a peer id
 * @param peerId
 * @param message
 */
void NetworkController::sendMessageToPeerId(QString peerId, QString message)
{
    if (!peerId.isEmpty())
    {
        // Send the message to the peer id
        int success = igs_busSendStringToAgent(peerId.toStdString().c_str(),
                                               "%s",
                                               message.toStdString().c_str());

        qInfo() << "Send message" << message << "to peer" << peerId << "with success ?" << success;
    }
}


/**
 * @brief Slot called when a command must be sent on the network to a launcher
 * @param peerIdOfLauncher
 * @param command
 * @param commandLine
 */
void NetworkController::onCommandAskedToLauncher(QString peerIdOfLauncher, QString command, QString commandLine)
{
    if (!peerIdOfLauncher.isEmpty())
    {
        // Send the command with command line to the peer id of the launcher
        int success = igs_busSendStringToAgent(peerIdOfLauncher.toStdString().c_str(),
                                               "%s %s",
                                               command.toStdString().c_str(),
                                               commandLine.toStdString().c_str());

        qInfo() << "Send command" << command << "with command line" << commandLine << "to launcher" << peerIdOfLauncher << "with success ?" << success;
    }
}


/**
 * @brief Slot when a command must be sent on the network to an agent
 * @param peerIdsList
 * @param command
 */
void NetworkController::onCommandAskedToAgent(QStringList peerIdsList, QString command)
{
    if (!command.isEmpty() && (peerIdsList.count() > 0))
    {
        for (QString peerId : peerIdsList)
        {
            // Send the command to a peer id of agent
            int success = igs_busSendStringToAgent(peerId.toStdString().c_str(), "%s", command.toStdString().c_str());

            qInfo() << "Send command" << command << "for agent" << peerId << "with success ?" << success;
        }
    }
}


/**
 * @brief Slot when a command must be sent on the network to an agent about one of its output
 * @param peerIdsList
 * @param command
 * @param outputName
 */
void NetworkController::onCommandAskedToAgentAboutOutput(QStringList peerIdsList, QString command, QString outputName)
{
    if (!command.isEmpty() && !outputName.isEmpty() && (peerIdsList.count() > 0))
    {
        for (QString peerId : peerIdsList)
        {
            // Send the command to a peer id of agent
            int success = igs_busSendStringToAgent(peerId.toStdString().c_str(), "%s %s",
                                                   command.toStdString().c_str(),
                                                   outputName.toStdString().c_str());

            qInfo() << "Send command" << command << "for agent" << peerId << "and output" << outputName << "with success ?" << success;
        }
    }
}


/**
 * @brief Slot when a command must be sent on the network to an agent about setting a value to one of its Input/Output/Parameter
 * @param peerIdsList
 * @param command
 * @param agentIOPName
 * @param value
 */
void NetworkController::onCommandAskedToAgentAboutSettingValue(QStringList peerIdsList, QString command, QString agentIOPName, QString value)
{
    if (!command.isEmpty() && !agentIOPName.isEmpty() && !value.isEmpty() && (peerIdsList.count() > 0))
    {
        for (QString peerId : peerIdsList)
        {
            // Send the command to a peer id of agent
            int success = igs_busSendStringToAgent(peerId.toStdString().c_str(), "%s %s %s",
                                                   command.toStdString().c_str(),
                                                   agentIOPName.toStdString().c_str(),
                                                   value.toStdString().c_str());

            qInfo() << "Send command" << command << "for agent" << peerId << "and I/O/P" << agentIOPName << "about setting value" << value << "with success ?" << success;
        }
    }
}


/**
 * @brief Slot when a command must be sent on the network to an agent about mapping one of its input
 * @param peerIdsList
 * @param command
 * @param inputName
 * @param outputAgentName
 * @param outputName
 */
void NetworkController::onCommandAskedToAgentAboutMappingInput(QStringList peerIdsList, QString command, QString inputName, QString outputAgentName, QString outputName)
{
    for (QString peerId : peerIdsList)
    {
        // Send the command to a peer id of agent
        int success = igs_busSendStringToAgent(peerId.toStdString().c_str(), "%s %s %s %s",
                                                command.toStdString().c_str(),
                                                inputName.toStdString().c_str(),
                                                outputAgentName.toStdString().c_str(),
                                                outputName.toStdString().c_str());

        qInfo() << "Send command" << command << "for agent" << peerId << "and input" << inputName << "about mapping on agent" << outputAgentName << "and output" << outputName << "with success ?" << success;
    }
}

