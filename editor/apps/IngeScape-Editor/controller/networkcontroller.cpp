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
static const QString prefix_AddedRecord = "ADD_RECORD=";
static const QString prefix_DeletedRecord = "DELETED_RECORD=";
static const QString prefix_LoadingRecord = "REPLAY_LOADING=";
static const QString prefix_LoadedRecord = "REPLAY_LOADED";
static const QString prefix_UNloadedRecord = "REPLAY_UNLOADED";
static const QString prefix_EndedRecord = "REPLAY_ENDED";
static const QString prefix_HighlightLink = "HIGHLIGHT_LINK=";
static const QString prefix_RunAction = "RUN_THIS_ACTION#";

static const QString prefix_LoadPlatformFile = "LOAD_PLATFORM_FROM_PATH=";
static const QString prefix_UpdateTimeLineState = "UPDATE_TIMELINE_STATE=";


/**
 * @brief Callback for Observing Inputs of our agent "IngeScape Editor"
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param myData
 */
void onObserveInputCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData)
{
    Q_UNUSED(value)
    Q_UNUSED(valueSize)

    NetworkController* networkController = (NetworkController*)myData;
    if (networkController != nullptr)
    {
        if (iopType == IGS_INPUT_T)
        {
            QString inputName = name;

            QStringList agentNameAndIOP = inputName.split(SEPARATOR_AGENT_NAME_AND_IOP);
            if (agentNameAndIOP.count() == 2)
            {
                QString outputAgentName = agentNameAndIOP.at(0);
                QString outputId = agentNameAndIOP.at(1);

                AgentIOPValueTypes::Value agentIOPValueType = static_cast<AgentIOPValueTypes::Value>(valueType);
                QVariant currentValue = QVariant();
                bool isValid = false;

                switch (valueType)
                {
                // INTEGER
                case IGS_INTEGER_T: {
                    int newValue = igs_readInputAsInt(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                // DOUBLE
                case IGS_DOUBLE_T: {
                    double newValue = igs_readInputAsDouble(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                // STRING
                case IGS_STRING_T: {
                    QString newValue = igs_readInputAsString(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                // BOOL
                case IGS_BOOL_T: {
                    bool newValue = igs_readInputAsBool(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                // IMPULSION
                case IGS_IMPULSION_T: {
                    isValid = true;

                    //qDebug() << "New IMPULSION received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                // DATA
                case IGS_DATA_T: {
                    // On peut utiliser directement value plutôt que de re-générer un tableau de bytes ??
                    // On stocke dans un dossier le media (eg video, son, image) et on log le path et le start time ??
                    //void* data = NULL;
                    //int result = igs_readInputAsData(name, &data, &valueSize);
                    //if (result == 1) {
                        // data must be a char* to have automatic conversion
                        //QByteArray newValue = QByteArray(data, valueSize);
                        //currentValue = QVariant(newValue);
                        //isValid = true;

                        //qDebug() << "New DATA with size" << valueSize << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    //}
                    //else {
                    //    qCritical() << "Can NOT read input" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType) << "(DATA of size:" << valueSize << ")";
                    //}

                    isValid = true;

                    break;
                }
                default: {
                    break;
                }
                }

                if (isValid)
                {
                    PublishedValueM* publishedValue = new PublishedValueM(QDateTime::currentDateTime(),
                                                                          outputAgentName,
                                                                          outputId,
                                                                          agentIOPValueType,
                                                                          currentValue);

                    // Emit the signal "Value Published"
                    Q_EMIT networkController->valuePublished(publishedValue);
                }
                else {
                    qCritical() << "Can NOT read input" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                }
            }
        }
    }
}


//--------------------------------------------------------------
//
// Network Controller
//
//--------------------------------------------------------------

/**
 * @brief Default constructor
 * @param parent
 */
NetworkController::NetworkController(QObject *parent) : IngeScapeNetworkController(parent),
    _agentEditor(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Add  header to declare ourselves as an editor
    igs_busAddServiceDescription("isEditor", "1");

    // We don't see itself
    setnumberOfEditors(1);

    // Create the model of our agent "IngeScape Editor"
    _agentEditor = new AgentM(_igsAgentApplicationName);

    QString definitionDescription = QString("Definition of %1 made by %2").arg(_igsAgentApplicationName, QApplication::instance()->organizationName());
    DefinitionM* agentDefinition = new DefinitionM(_igsAgentApplicationName, QApplication::instance()->applicationVersion(), definitionDescription);

    _agentEditor->setdefinition(agentDefinition);
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Delete the model of our agent "IngeScape Editor"
    if (_agentEditor != nullptr)
    {
        AgentM* temp = _agentEditor;
        setagentEditor(nullptr);
        delete temp;
    }

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
    QString message = zmsg_popstr(zMessage);

    // Definition
    if (message.startsWith(prefix_Definition))
    {
        message.remove(0, prefix_Definition.length());

        // Emit the signal "Definition Received"
        Q_EMIT definitionReceived(peerId, peerName, message);
    }
    // Mapping
    else if (message.startsWith(prefix_Mapping))
    {
        message.remove(0, prefix_Mapping.length());

        // Emit the signal "Mapping Received"
        Q_EMIT mappingReceived(peerId, peerName, message);
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

        // Emit the signal "Deleted record received"
        Q_EMIT deletedRecordReceived(message);
    }
    // Loading record
    else if (message == prefix_LoadingRecord)
    {
        qDebug() << prefix_LoadingRecord << zmsg_size(zMessage) << "frames";

        // Check that there are still 3 frames
        if (zmsg_size(zMessage) == 3)
        {
            QString deltaTimeFromTimeLine = zmsg_popstr(zMessage);
            QString jsonPlatform = zmsg_popstr(zMessage);
            QString jsonExecutedActions = zmsg_popstr(zMessage);

            // Emit the signal "Loading record received"
            Q_EMIT loadingRecordReceived(deltaTimeFromTimeLine.toInt(), jsonPlatform, jsonExecutedActions);
        }
    }
    // Loaded record
    else if (message == prefix_LoadedRecord)
    {
        // Emit the signal "Loaded record received"
        Q_EMIT loadedRecordReceived();
    }
    // UN-loaded record
    else if (message == prefix_UNloadedRecord)
    {
        // Emit the signal "UN-loaded record received"
        Q_EMIT unloadedRecordReceived();
    }
    // End of record
    else if (message == prefix_EndedRecord)
    {
        // Emit the signal "End of record Received"
        Q_EMIT endOfRecordReceived();
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
        message.remove(0, prefix_UpdateTimeLineState.length());

        Q_EMIT updateTimeLineState(message);
    }
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
 * @brief Slot called when a command must be sent on the network to a recorder
 * @param peerIdOfRecorder
 * @param commandAndParameters
 */
void NetworkController::onCommandAskedToRecorder(QString peerIdOfRecorder, QString commandAndParameters)
{
    if (!peerIdOfRecorder.isEmpty() && !commandAndParameters.isEmpty())
    {
        // Send the command (and parameters) to the peer id of the recorder
        int success = igs_busSendStringToAgent(peerIdOfRecorder.toStdString().c_str(), "%s", commandAndParameters.toStdString().c_str());

        qInfo() << "Send command (and parameters)" << commandAndParameters << "to recorder" << peerIdOfRecorder << "with success ?" << success;
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


/**
 * @brief Slot called when the flag "is Mapping Activated" changed
 * @param isMappingConnected
 */
void NetworkController::onIsMappingConnectedChanged(bool isMappingConnected)
{
    if ((_agentEditor != nullptr) && (_agentEditor->definition() != nullptr))
    {
        for (AgentIOPM* input : _agentEditor->definition()->inputsList()->toList())
        {
            if (input != nullptr)
            {
                QString inputName = input->name();

                QStringList agentNameAndIOP = inputName.split(SEPARATOR_AGENT_NAME_AND_IOP);
                if (agentNameAndIOP.count() == 2)
                {
                    QString outputAgentName = agentNameAndIOP.at(0);
                    QString outputId = agentNameAndIOP.at(1);

                    // Get the name and the value type of the output from its id
                    QPair<QString, AgentIOPValueTypes::Value> pair = AgentIOPM::getNameAndValueTypeFromId(outputId);

                    if (!pair.first.isEmpty() && (pair.second != AgentIOPValueTypes::UNKNOWN))
                    {
                        QString outputName = pair.first;
                        //AgentIOPValueTypes::Value valueType = pair.second;

                        // Mapping Activated (Connected)
                        if (isMappingConnected)
                        {
                            // Add mapping between our input and this output
                            unsigned long id = igs_addMappingEntry(inputName.toStdString().c_str(), outputAgentName.toStdString().c_str(), outputName.toStdString().c_str());

                            if (id > 0) {
                                //qDebug() << "Mapping added between output" << outputName << "of agent" << outputAgentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "(id" << id << ")";
                            }
                            else {
                                qCritical() << "Can NOT add mapping between output" << outputName << "of agent" << outputAgentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "Error code:" << id;
                            }
                        }
                        // Mapping DE-activated (DIS-connected)
                        else
                        {
                            // Remove mapping between our input and this output
                            int resultRemoveMappingEntry = igs_removeMappingEntryWithName(inputName.toStdString().c_str(), outputAgentName.toStdString().c_str(), outputName.toStdString().c_str());

                            if (resultRemoveMappingEntry == 1)
                            {
                                //qDebug() << "Mapping removed between output" << outputName << "of agent" << outputAgentName << "and input" << inputName << "of agent" << _igsAgentApplicationName;
                            }
                            else {
                                qCritical() << "Can NOT remove mapping between output" << outputName << "of agent" << outputAgentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "Error code:" << resultRemoveMappingEntry;
                            }
                        }
                    }
                }
            }
        }
    }
}


/**
 * @brief Slot called when inputs must be added to our application for a list of agent outputs
 * @param agentName
 * @param newOutputsIds
 * @param isMappingConnected
 */
void NetworkController::onAddInputsToOurApplicationForAgentOutputs(QString agentName, QStringList newOutputsIds, bool isMappingConnected)
{
    if ((_agentEditor != nullptr) && (_agentEditor->definition() != nullptr) && !newOutputsIds.isEmpty())
    {
        for (QString outputId : newOutputsIds)
        {
            // Get the name and the value type of the output from its id
            QPair<QString, AgentIOPValueTypes::Value> pair = AgentIOPM::getNameAndValueTypeFromId(outputId);

            if (!pair.first.isEmpty() && (pair.second != AgentIOPValueTypes::UNKNOWN))
            {
                QString outputName = pair.first;
                AgentIOPValueTypes::Value valueType = pair.second;

                QString inputName = QString("%1%2%3").arg(agentName, SEPARATOR_AGENT_NAME_AND_IOP, outputId);

                int resultCreateInput = 0;

                switch (valueType)
                {
                case AgentIOPValueTypes::INTEGER: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_INTEGER_T, nullptr, 0);
                    break;
                }
                case AgentIOPValueTypes::DOUBLE: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_DOUBLE_T, nullptr, 0);
                    break;
                }
                case AgentIOPValueTypes::STRING: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_STRING_T, nullptr, 0);
                    break;
                }
                case AgentIOPValueTypes::BOOL: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_BOOL_T, nullptr, 0);
                    break;
                }
                case AgentIOPValueTypes::IMPULSION: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_IMPULSION_T, nullptr, 0);
                    break;
                }
                case AgentIOPValueTypes::DATA: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_DATA_T, nullptr, 0);
                    break;
                }
                default: {
                    qCritical() << "Wrong type for the value of output" << outputName << "of agent" << agentName;
                    break;
                }
                }

                if (resultCreateInput == 1)
                {
                    qDebug() << "Input" << inputName << "created on agent" << _igsAgentApplicationName << "with value type" << AgentIOPValueTypes::staticEnumToString(valueType);

                    // Create a new model of input
                    AgentIOPM* input = new AgentIOPM(AgentIOPTypes::INPUT, inputName, valueType);

                    // Add the input to the definition of our agent "IngeScape Editor"
                    _agentEditor->definition()->inputsList()->append(input);

                    // Begin the observe of this input
                    int resultObserveInput = igs_observeInput(inputName.toStdString().c_str(), onObserveInputCallback, this);

                    if (resultObserveInput == 1) {
                        //qDebug() << "Observe input" << inputName << "on agent" << _igsAgentApplicationName;
                    }
                    else {
                        qCritical() << "Can NOT observe input" << inputName << "on agent" << _igsAgentApplicationName << "Error code:" << resultObserveInput;
                    }

                    // The mapping is activated (connected)
                    if (isMappingConnected)
                    {
                        // Add mapping between our input and this output
                        unsigned long id = igs_addMappingEntry(inputName.toStdString().c_str(), agentName.toStdString().c_str(), outputName.toStdString().c_str());

                        if (id > 0) {
                            //qDebug() << "Mapping added between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "(id" << id << ")";
                        }
                        else {
                            qCritical() << "Can NOT add mapping between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "Error code:" << id;
                        }
                    }
                }
                else {
                    qCritical() << "Can NOT create input" << inputName << "on agent" << _igsAgentApplicationName << "with value type" << AgentIOPValueTypes::staticEnumToString(valueType) << "Error code:" << resultCreateInput;
                }
            }
        }
    }
}


/**
 * @brief Slot called when inputs must be removed from our application for a list of agent outputs
 * @param agentName
 * @param oldOutputsIds
 * @param isMappingConnected
 */
void NetworkController::onRemoveInputsFromOurApplicationForAgentOutputs(QString agentName, QStringList oldOutputsIds, bool isMappingConnected)
{
    Q_UNUSED(isMappingConnected)

    if ((_agentEditor != nullptr) && (_agentEditor->definition() != nullptr) && !oldOutputsIds.isEmpty())
    {
        for (QString outputId : oldOutputsIds)
        {
            // Get the name and the value type of the output from its id
            QPair<QString, AgentIOPValueTypes::Value> pair = AgentIOPM::getNameAndValueTypeFromId(outputId);

            if (!pair.first.isEmpty() && (pair.second != AgentIOPValueTypes::UNKNOWN))
            {
                QString outputName = pair.first;
                AgentIOPValueTypes::Value valueType = pair.second;

                QString inputName = QString("%1%2%3").arg(agentName, SEPARATOR_AGENT_NAME_AND_IOP, outputId);

                // The mapping is activated (connected)
                //if (isMappingConnected)
                //{
                // Remove mapping between our input and this output
                int resultRemoveMappingEntry = igs_removeMappingEntryWithName(inputName.toStdString().c_str(), agentName.toStdString().c_str(), outputName.toStdString().c_str());

                if (resultRemoveMappingEntry == 1)
                {
                    //qDebug() << "Mapping removed between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _igsAgentApplicationName;
                }
                else {
                    qCritical() << "Can NOT remove mapping between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _igsAgentApplicationName << "Error code:" << resultRemoveMappingEntry;
                }
                //}

                // Remove our input
                int resultRemoveInput = igs_removeInput(inputName.toStdString().c_str());

                if (resultRemoveInput == 1)
                {
                    qDebug() << "Input" << inputName << "removed on agent" << _igsAgentApplicationName << "with value type" << AgentIOPValueTypes::staticEnumToString(valueType);

                    // Get the Input with its name
                    AgentIOPM* input = _agentEditor->definition()->getInputWithName(inputName);
                    if (input != nullptr)
                    {
                        // Remove the input from the definition of our agent "IngeScape Editor"
                        _agentEditor->definition()->inputsList()->remove(input);
                    }
                }
                else {
                    qCritical() << "Can NOT remove input" << inputName << "on agent" << _igsAgentApplicationName << "with value type" << AgentIOPValueTypes::staticEnumToString(valueType) << "Error code:" << resultRemoveInput;
                }
            }
        }
    }
}
