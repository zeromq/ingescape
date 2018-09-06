/*
 *	IngeScape Editor
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Deliencourt <deliencourt@ingenuity.io>
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#include "networkcontroller.h"

#include <QDebug>
#include <QApplication>

extern "C" {
#include <ingescape_advanced.h>
#include <ingescape_private.h>
#include <czmq.h>
}

#include "model/definitionm.h"

static const QString suffix_Launcher = ".ingescapelauncher";

static const QString prefix_Definition = "EXTERNAL_DEFINITION#";
static const QString prefix_Mapping = "EXTERNAL_MAPPING#";

static const QString prefix_Muted = "MUTED=";
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
static const QString prefix_AddedRecord = "ADDED_RECORD=";
static const QString prefix_DeletedRecord = "DELETED_RECORD=";
static const QString prefix_LoadingRecord = "RECORD_LOADING=";
static const QString prefix_LoadedRecord = "RECORD_LOADED";
static const QString prefix_EndedRecord = "RECORD_ENDED";
static const QString prefix_HighlightLink = "HIGHLIGHT_LINK=";


/**
 * @brief Callback for incomming messages on the bus
 * @param evt
 * @param peer
 * @param name
 * @param address
 * @param channel
 * @param headers
 * @param msg
 * @param myData
 */
void onIncommingBusMessageCallback(const char *event, const char *peer, const char *name, const char *address, const char *channel, zhash_t *headers, zmsg_t *msg, void *myData)
{
    Q_UNUSED(channel)

    NetworkController* networkController = (NetworkController*)myData;
    if (networkController != NULL)
    {
        QString peerId = QString(peer);
        QString peerName = QString(name);
        QString peerAddress = QString(address);

        // ENTER
        if (streq(event, "ENTER"))
        {
            qDebug() << QString("--> %1 has entered the network with peer id %2 (and address %3)").arg(peerName, peerId, peerAddress);

            QString ipAddress = "";

            // Get IP address (Example of peerAddress: "tcp://10.0.0.17:49153")
            if (peerAddress.length() > 6)
            {
                // Remove "tcp://" and then split IP address and port
                QStringList ipAddressAndPort = peerAddress.remove(0, 6).split(":");

                if (ipAddressAndPort.count() == 2) {
                    ipAddress = ipAddressAndPort.first();
                }
            }

            // Initialize properties related to message headers
            bool isIngeScapeEditor = false;
            bool isIngeScapeLauncher = false;
            bool isIngeScapeRecorder = false;
            bool isIngeScapePlayer = false;
            QString hostname = "";
            bool canBeFrozen = false;
            QString commandLine = "";
            QString loggerPort = "";
            QString streamingPort = "";

            zlist_t *keys = zhash_keys(headers);
            size_t nbKeys = zlist_size(keys);
            if (nbKeys > 0)
            {
                char *k;
                char *v;
                QString key = "";
                QString value = "";

                while ((k = (char *)zlist_pop(keys)))
                {
                    v = (char *)zhash_lookup(headers, k);

                    key = QString(k);
                    value = QString(v);

                    if (key == "isEditor") {
                        if (value == "1") {
                            isIngeScapeEditor = true;
                        }
                    }
                    else if (key == "isLauncher") {
                        if (value == "1") {
                            isIngeScapeLauncher = true;
                        }
                    }
                    else if (key == "isRecorder") {
                        if (value == "1") {
                            isIngeScapeRecorder = true;

                            // FIXME flag isIngeScapePlayer
                            isIngeScapePlayer = true;
                        }
                    }
                    else if (key == "hostname") {
                        hostname = value;
                    }
                    else if (key == "canBeFrozen") {
                        if (value == "1") {
                            canBeFrozen = true;
                        }
                    }
                    else if (key == "commandline") {
                        commandLine = value;
                    }
                    else if (key == "logger") {
                        loggerPort = value;
                    }
                    else if (key == "videoStream") {
                        streamingPort = value;
                    }
                }

                free(k);
                //free(v);
            }
            zlist_destroy(&keys);


            // IngeScape LAUNCHER
            if (isIngeScapeLauncher)
            {
                qDebug() << "Our zyre event is about IngeScape LAUNCHER";

                // Save the peer id of this launcher
                QStringList peerIdOfLaunchers = networkController->peerIdOfLaunchers();
                peerIdOfLaunchers.append(peerId);
                networkController->setpeerIdOfLaunchers(peerIdOfLaunchers);

                if (peerName.endsWith(suffix_Launcher)) {
                    hostname = peerName.left(peerName.length() - suffix_Launcher.length());
                }

                // Emit the signal "Launcher Entered"
                Q_EMIT networkController->launcherEntered(peerId, hostname, ipAddress, streamingPort);
            }
            // IngeScape RECORDER
            else if (isIngeScapeRecorder)
            {
                qDebug() << "Our zyre event is about IngeScape RECORDER";

                // Save the peer id of this recorder
                QStringList peerIdOfRecorders = networkController->peerIdOfRecorders();
                peerIdOfRecorders.append(peerId);
                networkController->setpeerIdOfRecorders(peerIdOfRecorders);

                // Emit the signal "Recorder Entered"
                Q_EMIT networkController->recorderEntered(peerId, peerName, ipAddress, hostname);
            }
            // IngeScape AGENT
            else if ((nbKeys > 0) && !isIngeScapeEditor)
            {
                qDebug() << "Our zyre event is about IngeScape AGENT on" << hostname;

                // Emit the signal "Agent Entered"
                Q_EMIT networkController->agentEntered(peerId, peerName, ipAddress, hostname, commandLine, canBeFrozen, loggerPort);
            }
            else {
                qDebug() << "Our zyre event is about an element without headers, we ignore it !";
            }
        }
        // JOIN (group)
        else if (streq(event, "JOIN"))
        {
            //qDebug() << QString("++ %1 has joined %2").arg(peerName, group);
        }
        // LEAVE (group)
        else if (streq(event, "LEAVE"))
        {
            //qDebug() << QString("-- %1 has left %2").arg(peerName, group);
        }
        // SHOUT
        else if (streq(event, "SHOUT"))
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            // MUTED / UN-MUTED
            if (message.startsWith(prefix_Muted))
            {
                // Manage the message "MUTED / UN-MUTED"
                networkController->manageMessageMutedUnmuted(peerId, message.remove(0, prefix_Muted.length()));
            }
            // FROZEN / UN-FROZEN
            else if (message.startsWith(prefix_Frozen))
            {
                // Manage the message "FROZEN / UN-FROZEN"
                networkController->manageMessageFrozenUnfrozen(peerId, message.remove(0, prefix_Frozen.length()));
            }
            // OUTPUT MUTED
            else if (message.startsWith(prefix_OutputMuted))
            {
                QString outputName = message.remove(0, prefix_OutputMuted.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
            }
            // OUTPUT UN-MUTED
            else if (message.startsWith(prefix_OutputUnmuted))
            {
                QString outputName = message.remove(0, prefix_OutputUnmuted.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
            }
            // STATE
            else if (message.startsWith(prefix_State))
            {
                QString stateName = message.remove(0, prefix_State.length());

                // Emit the signal "State changed"
                Q_EMIT networkController->agentStateChanged(peerId, stateName);
            }
            // LOG IN STREAM
            else if (message.startsWith(prefix_LogInStream))
            {
                QString hasLogInStream = message.remove(0, prefix_LogInStream.length());
                if (hasLogInStream == "1") {
                    Q_EMIT networkController->agentHasLogInStream(peerId, true);
                }
                else {
                    Q_EMIT networkController->agentHasLogInStream(peerId, false);
                }
            }
            // LOG IN FILE
            else if (message.startsWith(prefix_LogInFile))
            {
                QString hasLogInFile = message.remove(0, prefix_LogInFile.length());
                if (hasLogInFile == "1") {
                    Q_EMIT networkController->agentHasLogInFile(peerId, true);
                }
                else {
                    Q_EMIT networkController->agentHasLogInFile(peerId, false);
                }
            }
            // LOG FILE PATH
            else if (message.startsWith(prefix_LogFilePath))
            {
                QString logFilePath = message.remove(0, prefix_LogFilePath.length());

                Q_EMIT networkController->agentLogFilePath(peerId, logFilePath);
            }
            // DEFINITION FILE PATH
            else if (message.startsWith(prefix_DefinitionFilePath))
            {
                QString definitionFilePath = message.remove(0, prefix_DefinitionFilePath.length());

                Q_EMIT networkController->agentDefinitionFilePath(peerId, definitionFilePath);
            }
            // MAPPING FILE PATH
            else if (message.startsWith(prefix_MappingFilePath))
            {
                QString mappingFilePath = message.remove(0, prefix_MappingFilePath.length());

                Q_EMIT networkController->agentMappingFilePath(peerId, mappingFilePath);
            }
            else
            {
                qWarning() << "Not yet managed (SHOUT) message '" << message << "' for agent" << peerName << "(" << peerId << ")";
            }

            zmsg_destroy(&msg_dup);
        }
        // WHISPER
        else if (streq(event, "WHISPER"))
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            // Definition
            if (message.startsWith(prefix_Definition))
            {
                message.remove(0, prefix_Definition.length());

                // Emit the signal "Definition Received"
                Q_EMIT networkController->definitionReceived(peerId, peerName, message);
            }
            // Mapping
            else if (message.startsWith(prefix_Mapping))
            {
                message.remove(0, prefix_Mapping.length());

                // Emit the signal "Mapping Received"
                Q_EMIT networkController->mappingReceived(peerId, peerName, message);
            }
            // All records
            else if (message.startsWith(prefix_AllRecords))
            {
                message.remove(0, prefix_AllRecords.length());

                // Emit the signal "All records Received"
                Q_EMIT networkController->allRecordsReceived(message);
            }
            // Added record
            else if (message.startsWith(prefix_AddedRecord))
            {
                message.remove(0, prefix_AddedRecord.length());

                // Emit the signal "Added record received"
                Q_EMIT networkController->addedRecordReceived(message);
            }
            // Deleted Record
            else if (message.startsWith(prefix_DeletedRecord))
            {
                message.remove(0, prefix_DeletedRecord.length());

                // Emit the signal "Deleted record received"
                Q_EMIT networkController->deletedRecordReceived(message);
            }
            // Loading record
            else if (message == prefix_LoadingRecord)
            {
                qDebug() << prefix_LoadingRecord << zmsg_size(msg_dup) << "frames";

                // Check that there are still 3 frames
                if (zmsg_size(msg_dup) == 3)
                {
                    QString deltaTimeFromTimeLine = zmsg_popstr(msg_dup);
                    QString jsonPlatform = zmsg_popstr(msg_dup);
                    QString jsonExecutedActions = zmsg_popstr(msg_dup);

                    // Emit the signal "Loading record received"
                    Q_EMIT networkController->loadingRecordReceived(deltaTimeFromTimeLine.toInt(), jsonPlatform, jsonExecutedActions);
                }
            }
            // Loaded record
            else if (message == prefix_LoadedRecord)
            {
                qDebug() << prefix_LoadedRecord;

                // Emit the signal "Loaded record received"
                Q_EMIT networkController->loadedRecordReceived();
            }
            // End of record
            else if (message == prefix_EndedRecord)
            {
                qDebug() << prefix_EndedRecord;

                // Emit the signal "End of record Received"
                Q_EMIT networkController->endOfRecordReceived();
            }
            // MUTED / UN-MUTED
            else if (message.startsWith(prefix_Muted))
            {
                message.remove(0, prefix_Muted.length());

                // Manage the message "MUTED / UN-MUTED"
                networkController->manageMessageMutedUnmuted(peerId, message);
            }
            // FROZEN / UN-FROZEN
            else if (message.startsWith(prefix_Frozen))
            {
                message.remove(0, prefix_Frozen.length());

                // Manage the message "FROZEN / UN-FROZEN"
                networkController->manageMessageFrozenUnfrozen(peerId, message);
            }
            // OUTPUT MUTED
            else if (message.startsWith(prefix_OutputMuted))
            {
                QString outputName = message.remove(0, prefix_OutputMuted.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
            }
            // OUTPUT UN-MUTED
            else if (message.startsWith(prefix_OutputUnmuted))
            {
                QString outputName = message.remove(0, prefix_OutputUnmuted.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
            }
            // STATE
            else if (message.startsWith(prefix_State))
            {
                QString stateName = message.remove(0, prefix_State.length());

                // Emit the signal "State changed"
                Q_EMIT networkController->agentStateChanged(peerId, stateName);
            }
            // LOG IN STREAM
            else if (message.startsWith(prefix_LogInStream))
            {
                QString hasLogInStream = message.remove(0, prefix_LogInStream.length());
                if (hasLogInStream == "1") {
                    Q_EMIT networkController->agentHasLogInStream(peerId, true);
                }
                else {
                    Q_EMIT networkController->agentHasLogInStream(peerId, false);
                }
            }
            // LOG IN FILE
            else if (message.startsWith(prefix_LogInFile))
            {
                QString hasLogInFile = message.remove(0, prefix_LogInFile.length());
                if (hasLogInFile == "1") {
                    Q_EMIT networkController->agentHasLogInFile(peerId, true);
                }
                else {
                    Q_EMIT networkController->agentHasLogInFile(peerId, false);
                }
            }
            // LOG FILE PATH
            else if (message.startsWith(prefix_LogFilePath))
            {
                QString logFilePath = message.remove(0, prefix_LogFilePath.length());

                Q_EMIT networkController->agentLogFilePath(peerId, logFilePath);
            }
            // DEFINITION FILE PATH
            else if (message.startsWith(prefix_DefinitionFilePath))
            {
                QString definitionFilePath = message.remove(0, prefix_DefinitionFilePath.length());

                Q_EMIT networkController->agentDefinitionFilePath(peerId, definitionFilePath);
            }
            // MAPPING FILE PATH
            else if (message.startsWith(prefix_MappingFilePath))
            {
                QString mappingFilePath = message.remove(0, prefix_MappingFilePath.length());

                Q_EMIT networkController->agentMappingFilePath(peerId, mappingFilePath);
            }
            // HIGHLIGHT LINK
            else if (message.startsWith(prefix_HighlightLink))
            {
                message.remove(0, prefix_HighlightLink.length());

                Q_EMIT networkController->highlightLink(message.split('|'));
            }
            else
            {
                qWarning() << "Not yet managed (WHISPER) message '" << message << "' for agent" << peerName << "(" << peerId << ")";
            }

            zmsg_destroy(&msg_dup);
        }
        // EXIT
        else if (streq(event, "EXIT"))
        {
            qDebug() << QString("<-- %1 (%2) exited").arg(peerName, peerId);

            // IngeScape LAUNCHER
            if (networkController->peerIdOfLaunchers().contains(peerId))
            {
                QString hostname = "";

                if (peerName.endsWith(suffix_Launcher)) {
                    hostname = peerName.left(peerName.length() - suffix_Launcher.length());
                }

                // Emit the signal "Launcher Exited"
                Q_EMIT networkController->launcherExited(peerId, hostname);
            }
            // IngeScape RECORDER
            else if (networkController->peerIdOfRecorders().contains(peerId))
            {
                // Emit the signal "Recorder Exited"
                Q_EMIT networkController->recorderExited(peerId, peerName);

                QStringList peerIdOfRecorders = networkController->peerIdOfRecorders();
                peerIdOfRecorders.removeOne(peerId);
                networkController->setpeerIdOfRecorders(peerIdOfRecorders);
            }
            // IngeScape AGENT
            else
            {
                // Emit the signal "Agent Exited"
                Q_EMIT networkController->agentExited(peerId, peerName);
            }
        }
    }
}


/**
 * @brief Callback for Observing inputs of our agent "INGESCAPE-Editor"
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param myData
 */
void onObserveInputCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, size_t valueSize, void* myData)
{
    Q_UNUSED(value);
    Q_UNUSED(valueSize)

    // Historique: on log la value et le dateTime.
    NetworkController* networkController = (NetworkController*)myData;
    if (networkController != NULL)
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
                case IGS_INTEGER_T: {
                    //int* newValue = (int*)value;
                    int newValue = igs_readInputAsInt(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case IGS_DOUBLE_T: {
                    //double* newValue = (double*)value;
                    double newValue = igs_readInputAsDouble(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case IGS_STRING_T: {
                    //QString newValue = QString((char*)value);
                    QString newValue = igs_readInputAsString(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case IGS_BOOL_T: {
                    //bool* newValue = (bool*)value;
                    bool newValue = igs_readInputAsBool(name);

                    currentValue = QVariant(newValue);
                    isValid = true;

                    //qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case IGS_IMPULSION_T: {
                    isValid = true;

                    //qDebug() << "New IMPULSION received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case IGS_DATA_T: {
                    /*// On peut utiliser directement value plutôt que de re-générer un tableau de bytes ??
                    // On stocke dans un dossier le media (eg video, son, image) et on log le path et le start time ??
                    void* data = NULL;
                    int result = igs_readInputAsData(name, &data, &valueSize);
                    if (result == 1) {
                        // data must be a char* to have automatic conversion
                        //QByteArray newValue = QByteArray(data, valueSize);
                        //currentValue = QVariant(newValue);
                        //isValid = true;

                        qDebug() << "New DATA with size" << valueSize << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    }
                    else {
                        qCritical() << "Can NOT read input" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType) << "(DATA of size:" << valueSize << ")";
                    }*/

                    // FIXME TODO: use igs_readInputAsData
                    isValid = true;

                    break;
                }
                default: {
                    break;
                }
                }

                if (isValid) {
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
// NetworkController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
NetworkController::NetworkController(QObject *parent) : QObject(parent),
    _isIngeScapeAgentStarted(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Init the name of our IngeScape agent with the application name
    _editorAgentName = QApplication::instance()->applicationName();
    QString organizationName = QApplication::instance()->organizationName();
    QString version = QApplication::instance()->applicationVersion();

    // Set trace mode
    igs_setVerbose(true);

    // Set the name of our agent
    igs_setAgentName(_editorAgentName.toStdString().c_str());

    // Add  header to declare ourselves as an editor
    igs_busAddServiceDescription("isEditor", "1");

    //
    // Create our internal definition
    //
    // Set definition and mapping by default to editor
    QString definitionByDefault = "{  \
                                  \"definition\": {  \
                                  \"name\": \""+ _editorAgentName + "\",   \
                                  \"description\": \"Definition of " + _editorAgentName + " made by "+ organizationName +"\",  \
                                  \"version\": \"" + version + "\",  \
                                  \"parameters\": [],   \
                                  \"inputs\": [],       \
                                  \"outputs\": [] }}";

    igs_loadDefinition(definitionByDefault.toStdString().c_str());

    //
    // Create our internal mapping
    //
    QString mappingByDefault = "{      \
                                  \"mapping\": {    \
                                  \"name\": \"" + _editorAgentName + "\",   \
                                  \"description\": \"Mapping of " + _editorAgentName + " made by "+ organizationName + "\",  \
                                  \"version\": \"" + version + "\",  \
                                  \"mapping_out\": [],   \
                                  \"mapping_cat\": [] }}";

    igs_loadMapping(mappingByDefault.toStdString().c_str());


    // Update the list of available network devices
    updateAvailableNetworkDevices();


    // Begin to observe incoming messages on the bus
    int result = igs_observeBus(&onIncommingBusMessageCallback, this);
    if (result == 0) {
        qCritical() << "The callback on zyre messages has NOT been registered !";
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Stop our INGESCAPE agent
    stop();
}


/**
 * @brief Start our INGESCAPE agent with a network device (or an IP address) and a port
 * @param networkDevice
 * @param ipAddress
 * @param port
 * @return
 */
bool NetworkController::start(QString networkDevice, QString ipAddress, int port)
{
    if (_isIngeScapeAgentStarted == 0)
    {
        // Start service with network device
        if (!networkDevice.isEmpty()) {
            _isIngeScapeAgentStarted = igs_startWithDevice(networkDevice.toStdString().c_str(), port);
        }

        // Start service with ip address (if start with network device has failed)
        if ((_isIngeScapeAgentStarted != 1) && !ipAddress.isEmpty()) {
            _isIngeScapeAgentStarted = igs_startWithIP(ipAddress.toStdString().c_str(), port);
        }

        if (_isIngeScapeAgentStarted == 1)
        {
            qInfo() << "IngeScape Agent" << _editorAgentName << "started";
        }
        else {
            qCritical() << "The network has NOT been initialized on" << networkDevice << "or" << ipAddress << "and port" << QString::number(port);
        }
    }

    return _isIngeScapeAgentStarted;
}


/**
 * @brief Stop our INGESCAPE agent
 */
void NetworkController::stop()
{
    if (_isIngeScapeAgentStarted == 1)
    {
        // Stop network services
        igs_stop();

        _isIngeScapeAgentStarted = 0;
    }
}


/**
 * @brief Manage the message "MUTED / UN-MUTED"
 * @param peerId
 * @param message
 */
void NetworkController::manageMessageMutedUnmuted(QString peerId, QString message)
{
    if (message == "0") {
        //qDebug() << peerName << "(" << peerId << ") UN-MUTED";

        // Emit the signal "is Muted from Agent Updated"
        Q_EMIT isMutedFromAgentUpdated(peerId, false);
    }
    else if (message == "1") {
        //qDebug() << peerName << "(" << peerId << ") MUTED";

        // Emit the signal "is Muted from Agent Updated"
        Q_EMIT isMutedFromAgentUpdated(peerId, true);
    }
}


/**
 * @brief Manage the message "FROZEN / UN-FROZEN"
 * @param peerId
 * @param message
 */
void NetworkController::manageMessageFrozenUnfrozen(QString peerId, QString message)
{
    if (message == "0") {
        //qDebug() << peerName << "(" << peerId << ") UN-FROZEN";

        // Emit the signal "is Frozen from Agent Updated"
        Q_EMIT isFrozenFromAgentUpdated(peerId, false);
    }
    else if (message == "1") {
        //qDebug() << peerName << "(" << peerId << ") FROZEN";

        // Emit the signal "is Frozen from Agent Updated"
        Q_EMIT isFrozenFromAgentUpdated(peerId, true);
    }
}


/**
 * @brief Update the list of available network devices
 */
void NetworkController::updateAvailableNetworkDevices()
{
    QStringList networkDevices;

    char **devices = NULL;
    int nb = 0;
    igs_getNetdevicesList(&devices, &nb);

    for (int i = 0; i < nb; i++)
    {
        QString availableNetworkDevice = QString(devices[i]);
        networkDevices.append(availableNetworkDevice);
    }
    igs_freeNetdevicesList(devices, nb);

    setavailableNetworkDevices(networkDevices);

    qInfo() << "Update available Network Devices:" << _availableNetworkDevices;
}


/**
 * @brief Return true if the network device is available
 * @param networkDevice
 * @return
 */
bool NetworkController::isAvailableNetworkDevice(QString networkDevice)
{
    if (!networkDevice.isEmpty() && _availableNetworkDevices.contains(networkDevice)) {
        return true;
    }
    else {
        return false;
    }
}


/**
 * @brief Send a command, parameters and the content of a JSON file to the recorder
 * @param commandAndParameters
 */
void NetworkController::sendCommandWithJsonToRecorder(QStringList commandAndParameters)
{
    if (_peerIdOfRecorders.count() == 1)
    {
        QString peerIdOfRecorder = _peerIdOfRecorders.first();

        if (!peerIdOfRecorder.isEmpty() && !commandAndParameters.isEmpty())
        {
            // Create ZMQ message
            zmsg_t* msg = zmsg_new();

            foreach (QString string, commandAndParameters)
            {
                // Add a frame with STRING
                zframe_t* frameString = zframe_new(string.toStdString().c_str(), string.length());
                zmsg_append(msg, &frameString);
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
    else if (_peerIdOfRecorders.count() == 0) {
        qDebug() << "There is no recorder !";
    }
    else {
        qWarning() << "There are several recorders (" << _peerIdOfRecorders.count() << ")";
    }
}


/**
 * @brief Slot when a command must be sent on the network to a launcher
 * @param command
 * @param hostname
 * @param commandLine
 */
void NetworkController::onCommandAskedToLauncher(QString command, QString hostname, QString commandLine)
{
    if (!hostname.isEmpty())
    {
        // Get the peer id of The IngeScape Launcher with a HostName
        QString peerIdLauncher = IngeScapeLauncherManager::Instance().getPeerIdOfLauncherWithHostName(hostname);

        if (!peerIdLauncher.isEmpty())
        {
            // Send the command with command line to the peer id of the launcher
            int success = igs_busSendStringToAgent(peerIdLauncher.toStdString().c_str(),
                                                   "%s %s",
                                                   command.toStdString().c_str(),
                                                   commandLine.toStdString().c_str());

            qInfo() << "Send command" << command << "to launcher on" << hostname << "with command line" << commandLine << "with success ?" << success;
        }
        else {
            qInfo() << "There is no launcher on" << hostname;
        }
    }
}


/**
 * @brief Slot called when a command must be sent on the network to a recorder
 * @param commandAndParameters
 */
void NetworkController::onCommandAskedToRecorder(QString commandAndParameters)
{
    if (_peerIdOfRecorders.count() == 1)
    {
        QString peerIdOfRecorder = _peerIdOfRecorders.first();
        if (!peerIdOfRecorder.isEmpty())
        {
            // Send the command (and parameters) to the peer id of the recorder
            int success = igs_busSendStringToAgent(peerIdOfRecorder.toStdString().c_str(), "%s", commandAndParameters.toStdString().c_str());

            qInfo() << "Send command (and parameters)" << commandAndParameters << "to recorder" << peerIdOfRecorder << "with success ?" << success;
        }
    }
    else if (_peerIdOfRecorders.count() == 0) {
        qDebug() << "There is no recorder !";
    }
    else {
        qWarning() << "There are several recorders (" << _peerIdOfRecorders.count() << ")";
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
        foreach (QString peerId, peerIdsList)
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
        foreach (QString peerId, peerIdsList)
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
        foreach (QString peerId, peerIdsList)
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
    foreach (QString peerId, peerIdsList)
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
 * @brief Slot when inputs must be added to our Editor for a list of outputs
 * @param agentName
 * @param outputsList
 */
void NetworkController::onAddInputsToEditorForOutputs(QString agentName, QList<OutputM*> outputsList)
{
    foreach (OutputM* output, outputsList)
    {
        if ((output != NULL) && !output->id().isEmpty())
        {
            QString inputName = QString("%1%2%3").arg(agentName, SEPARATOR_AGENT_NAME_AND_IOP, output->id());

            // Get the number of agents in state ON with an "Input (on our editor) Name"
            int numberOfAgentsON = _getNumberOfAgentsONwithInputName(inputName);

            // If there is not yet an agent in state ON for this input name, we create a new input on our agent
            if (numberOfAgentsON == 0)
            {
                int resultCreateInput = 0;

                switch (output->agentIOPValueType())
                {
                case AgentIOPValueTypes::INTEGER: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_INTEGER_T, NULL, 0);
                    break;
                }
                case AgentIOPValueTypes::DOUBLE: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_DOUBLE_T, NULL, 0);
                    break;
                }
                case AgentIOPValueTypes::STRING: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_STRING_T, NULL, 0);
                    break;
                }
                case AgentIOPValueTypes::BOOL: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_BOOL_T, NULL, 0);
                    break;
                }
                case AgentIOPValueTypes::IMPULSION: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_IMPULSION_T, NULL, 0);
                    break;
                }
                case AgentIOPValueTypes::DATA: {
                    resultCreateInput = igs_createInput(inputName.toStdString().c_str(), IGS_DATA_T, NULL, 0);
                    break;
                }
                default: {
                    qCritical() << "Wrong type for the value of output" << output->name() << "of agent" << agentName;
                    break;
                }
                }

                if (resultCreateInput == 1) {
                    qDebug() << "Create input" << inputName << "on agent" << _editorAgentName;

                    // Begin the observe of this input
                    int resultObserveInput = igs_observeInput(inputName.toStdString().c_str(), onObserveInputCallback, this);

                    if (resultObserveInput == 1) {
                        qDebug() << "Observe input" << inputName << "on agent" << _editorAgentName;
                    }
                    else {
                        qCritical() << "Can NOT observe input" << inputName << "on agent" << _editorAgentName << "Error code:" << resultObserveInput;
                    }

                    // Add mapping between our input and this output
                    unsigned long id = igs_addMappingEntry(inputName.toStdString().c_str(), agentName.toStdString().c_str(), output->name().toStdString().c_str());

                    if (id > 0) {
                        qDebug() << "Add mapping between output" << output->name() << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName << "(id" << id << ")";
                    }
                    else {
                        qCritical() << "Can NOT add mapping between output" << output->name() << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName << "Error code:" << id;
                    }

                }
                else {
                    qCritical() << "Can NOT create input" << inputName << "on agent" << _editorAgentName << "Error code:" << resultCreateInput;
                }
            }

            numberOfAgentsON++;
            qDebug() << "on Add Inputs: There are" << numberOfAgentsON << "agents ON for input name" << inputName;

            _mapFromInputNameToNumberOfAgentsON.insert(inputName, numberOfAgentsON);
        }
    }
}


/**
 * @brief Slot when inputs must be removed to our Editor for a list of outputs
 * @param agentName
 * @param outputsList
 */
void NetworkController::onRemoveInputsToEditorForOutputs(QString agentName, QList<OutputM*> outputsList)
{
    foreach (OutputM* output, outputsList)
    {
        if ((output != NULL) && !output->id().isEmpty())
        {
            QString inputName = QString("%1%2%3").arg(agentName, SEPARATOR_AGENT_NAME_AND_IOP, output->id());

            // Get the number of agents in state ON with an "Input (on our editor) Name"
            int numberOfAgentsON = _getNumberOfAgentsONwithInputName(inputName);

            numberOfAgentsON--;
            _mapFromInputNameToNumberOfAgentsON.insert(inputName, numberOfAgentsON);

            qDebug() << "on Remove Inputs: There are" << numberOfAgentsON << "agents ON for input name" << inputName;

            // FIXME: Debug numberOfAgentsON < 0
            if (numberOfAgentsON < 0) {
                qWarning() << "There are" << numberOfAgentsON << "agents ON for input name" << inputName;
            }

            // If there is no more agent in state ON for this input name, we remove an old input on our agent
            if (numberOfAgentsON == 0)
            {
                // Remove mapping between our input and this output
                int resultRemoveMappingEntry = igs_removeMappingEntryWithName(inputName.toStdString().c_str(), agentName.toStdString().c_str(), output->name().toStdString().c_str());

                if (resultRemoveMappingEntry == 1) {
                    qDebug() << "Remove mapping between output" << output->name() << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName;

                    // Remove our input
                    int resultRemoveInput = igs_removeInput(inputName.toStdString().c_str());

                    if (resultRemoveInput == 1) {
                        qDebug() << "Remove input" << inputName << "on agent" << _editorAgentName;
                    }
                    else {
                        qCritical() << "Can NOT remove input" << inputName << "on agent" << _editorAgentName << "Error code:" << resultRemoveInput;
                    }
                }
                else {
                    qCritical() << "Can NOT remove mapping between output" << output->name() << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName << "Error code:" << resultRemoveMappingEntry;
                }
            }
        }
    }
}


/**
 * @brief Get the number of agents in state ON with an "Input (on our editor) Name"
 * @param inputName name of an input on our editor
 * @return
 */
int NetworkController::_getNumberOfAgentsONwithInputName(QString inputName)
{
    if (_mapFromInputNameToNumberOfAgentsON.contains(inputName)) {
        return _mapFromInputNameToNumberOfAgentsON.value(inputName);
    }
    else {
        return 0;
    }
}

