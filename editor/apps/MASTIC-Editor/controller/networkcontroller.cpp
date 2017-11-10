/*
 *	MASTIC Editor
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
#include <mastic_private.h>
}

#include "misc/masticeditorutils.h"
#include "model/definitionm.h"


static const QString launcherSuffix = ".masticlauncher";

static const QString definitionPrefix = "EXTERNAL_DEFINITION#";
static const QString mappingPrefix = "EXTERNAL_MAPPING#";

static const QString mutedAllPrefix = "MUTED=";
static const QString frozenPrefix = "FROZEN=";
static const QString mutedOutputPrefix = "OUTPUT_MUTED ";
static const QString unmutedOutputPrefix = "OUTPUT_UNMUTED ";

static const QString NAME_SEPARATOR = "##";


/**
 * @brief Callback for Incomming Zyre Messages
 * @param cst_zyre_event
 * @param arg
 * @return
 */
int onIncommingZyreMessageCallback(const zyre_event_t *cst_zyre_event, void *arg)
{
    NetworkController* networkController = (NetworkController*)arg;
    if (networkController != NULL)
    {
        zyre_event_t* zyre_event = (zyre_event_t *)cst_zyre_event;

        QString event = zyre_event_type(zyre_event);
        QString peerId = zyre_event_peer_uuid(zyre_event);
        QString peerName = zyre_event_peer_name(zyre_event);
        QString peerAddress = zyre_event_peer_addr(zyre_event);
        zhash_t* headers = zyre_event_headers(zyre_event);
        QString group = zyre_event_group(zyre_event);
        zmsg_t* msg = zyre_event_msg(zyre_event);

        // ENTER
        if (event.compare("ENTER") == 0)
        {
            qDebug() << QString("--> %1 has entered the network with peer id %2 (and address %3)").arg(peerName, peerId, peerAddress);

            // Mastic Launcher
            if (peerName.endsWith(launcherSuffix)) {
                QString hostname = peerName.left(peerName.length() - launcherSuffix.length());
                networkController->masticLauncherEntered(hostname, peerId);
            }

            bool isMasticPublisher = false;
            bool isIntPID = false;
            int pid = -1;
            bool canBeFrozen = false;
            QString executionPath = "";
            QString hostname = "";

            zlist_t *keys = zhash_keys(headers);
            size_t nbKeys = zlist_size(keys);
            if (nbKeys > 0)
            {
                //qDebug() << nbKeys << "headers";

                char *k;
                const char *v;
                QString key = "";
                QString value = "";

                while ((k = (char *)zlist_pop(keys))) {
                    v = zyre_event_header(zyre_event, k);

                    key = QString(k);
                    value = QString(v);
                    //qDebug() << "key" << key << ":" << value;

                    // We check that the key "publisher" exists
                    if (key == "publisher") {
                        isMasticPublisher = true;
                    }
                    else if (key == "pid") {
                        pid = value.toInt(&isIntPID);
                    }
                    else if (key == "canBeFrozen") {
                        if (value == "1") {
                            canBeFrozen = true;
                        }
                    }
                    else if (key == "execpath") {
                        executionPath = value;
                    }
                    else if (key == "hostname") {
                        hostname = value;
                    }
                }

                free(k);
            }
            zlist_destroy(&keys);

            // Subscribers
            /*int n = HASH_COUNT(subscribers);
            qDebug() << n << "subscribers in the list";
            subscriber_t *sub, *tmpSub;
            HASH_ITER(hh, subscribers, sub, tmpSub) {
                qDebug() << "subscriber:" << sub->agentName << "with peer id" << sub->agentPeerId;
            }*/

            if (isMasticPublisher && isIntPID) {
                qDebug() << "our zyre event is about MASTIC publisher:" << pid << hostname << executionPath;

                // Emit the signal "Agent Entered"
                Q_EMIT networkController->agentEntered(peerId, peerName, peerAddress, pid, hostname, executionPath, canBeFrozen);
            }
        }
        // JOIN (group)
        else if (event.compare("JOIN") == 0)
        {
            //qDebug() << QString("++ %1 has joined %2").arg(peerName, group);
        }
        // LEAVE (group)
        else if (event.compare("LEAVE") == 0)
        {
            //qDebug() << QString("-- %1 has left %2").arg(peerName, group);
        }
        // SHOUT
        else if (event.compare("SHOUT") == 0)
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            // MUTED / UN-MUTED
            if (message.startsWith(mutedAllPrefix))
            {
                message.remove(0, mutedAllPrefix.length());

                if (message == "0") {
                    //qDebug() << peerName << "(" << peerId << ") UN-MUTED";

                    // Emit the signal "is Muted from Agent Updated"
                    Q_EMIT networkController->isMutedFromAgentUpdated(peerId, false);
                }
                else if (message == "1") {
                    //qDebug() << peerName << "(" << peerId << ") MUTED";

                    // Emit the signal "is Muted from Agent Updated"
                    Q_EMIT networkController->isMutedFromAgentUpdated(peerId, true);
                }
            }
            // FROZEN / UN-FROZEN
            else if (message.startsWith(frozenPrefix))
            {
                message.remove(0, frozenPrefix.length());

                if (message == "0") {
                    //qDebug() << peerName << "(" << peerId << ") UN-FROZEN";

                    // Emit the signal "is Frozen from Agent Updated"
                    Q_EMIT networkController->isFrozenFromAgentUpdated(peerId, false);
                }
                else if (message == "1") {
                    //qDebug() << peerName << "(" << peerId << ") FROZEN";

                    // Emit the signal "is Frozen from Agent Updated"
                    Q_EMIT networkController->isFrozenFromAgentUpdated(peerId, true);
                }
            }
            // OUTPUT MUTED
            else if (message.startsWith(mutedOutputPrefix))
            {
                QString outputName = message.remove(0, mutedOutputPrefix.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, true, outputName);
            }
            // OUTPUT UN-MUTED
            else if (message.startsWith(unmutedOutputPrefix))
            {
                QString outputName = message.remove(0, unmutedOutputPrefix.length());

                // Emit the signal "is Muted from OUTPUT of Agent Updated"
                Q_EMIT networkController->isMutedFromOutputOfAgentUpdated(peerId, false, outputName);
            }
            else
            {
                qWarning() << "Not yet managed (SHOUT) message '" << message << "' for agent" << peerName << "(" << peerId << ")";
            }

            zmsg_destroy(&msg_dup);
        }
        // WHISPER
        else if (event.compare("WHISPER") == 0)
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            //
            // Definition
            //
            if (message.startsWith(definitionPrefix))
            {
                message.remove(0, definitionPrefix.length());

                // FIXME - TEST ONLY - TO REMOVE
                /*// Load definition from string content
                definition* newDefinition = parser_loadDefinition(message.toStdString().c_str());
                qDebug() << "Definition received from : " << newDefinition->name << " version : " << newDefinition->version << " description : " << newDefinition->description;
                definition_freeDefinition(newDefinition);*/

                // Emit the signal "Definition Received"
                Q_EMIT networkController->definitionReceived(peerId, peerName, message);
            }
            //
            // Mapping
            //
            else if (message.startsWith(mappingPrefix))
            {
                message.remove(0, mappingPrefix.length());

                // FIXME - TEST ONLY - TO REMOVE
                /*// Load mapping from string content
                mapping* newMapping = parser_LoadMap((message.toStdString().c_str()));
                qDebug() << "Mapping received from : " << newMapping->name << " version : " << newMapping->version << " description : " << newMapping->description;
                mapping_freeMapping(newMapping);*/

                // Emit the signal "Mapping Received"
                Q_EMIT networkController->mappingReceived(peerId, peerName, message);
            }
            // MUTED / UN-MUTED
            else if (message.startsWith(mutedAllPrefix))
            {
                message.remove(0, mutedAllPrefix.length());

                if (message == "0") {
                    //qDebug() << peerName << "(" << peerId << ") UN-MUTED";

                    // Emit the signal "is Muted from Agent Updated"
                    Q_EMIT networkController->isMutedFromAgentUpdated(peerId, false);
                }
                else if (message == "1") {
                    //qDebug() << peerName << "(" << peerId << ") MUTED";

                    // Emit the signal "is Muted from Agent Updated"
                    Q_EMIT networkController->isMutedFromAgentUpdated(peerId, true);
                }
            }
            // FROZEN / UN-FROZEN
            else if (message.startsWith(frozenPrefix))
            {
                message.remove(0, frozenPrefix.length());

                if (message == "0") {
                    //qDebug() << peerName << "(" << peerId << ") UN-FROZEN";

                    // Emit the signal "is Frozen from Agent Updated"
                    Q_EMIT networkController->isFrozenFromAgentUpdated(peerId, false);
                }
                else if (message == "1") {
                    //qDebug() << peerName << "(" << peerId << ") FROZEN";

                    // Emit the signal "is Frozen from Agent Updated"
                    Q_EMIT networkController->isFrozenFromAgentUpdated(peerId, true);
                }
            }
            // MAPPED
            /*else if (message.startsWith("MAPPED"))
            {
                // FIXME Nothing TODO ?
                //qDebug() << peerName << "MAPPED" << message;
            }*/
            else
            {
                qDebug() << "Unknown message received:" << message;
            }

            zmsg_destroy(&msg_dup);
        }
        // EXIT
        else if (event.compare("EXIT") == 0)
        {
            qDebug() << QString("<-- %1 (%2) exited").arg(peerName, peerId);

            // Mastic Launcher
            if (peerName.endsWith(launcherSuffix)) {
                QString hostname = peerName.left(peerName.length() - launcherSuffix.length());
                networkController->masticLauncherExited(hostname);
            }

            // Emit the signal "Agent Exited"
            Q_EMIT networkController->agentExited(peerId, peerName);
        }
    }

    return 0;
}


/**
 * @brief Callback for Observing inputs of our agent "MASTIC-Editor"
 * @param iopType
 * @param name
 * @param valueType
 * @param value
 * @param valueSize
 * @param myData
 */
void onObserveInputCallback(iop_t iopType, const char* name, iopType_t valueType, void* value, long valueSize, void* myData)
{
    Q_UNUSED(value);

    // Historique: on log la value et le dateTime.
    NetworkController* networkController = (NetworkController*)myData;
    if (networkController != NULL)
    {
        if (iopType == INPUT_T)
        {
            QString inputName = name;
            QStringList names = inputName.split(NAME_SEPARATOR);
            if (names.count() == 2)
            {
                QString outputAgentName = names.at(0);
                QString outputName = names.at(1);
                qDebug() << "outputAgentName" << outputAgentName << "outputName" << outputName;

                AgentIOPValueTypes::Value agentIOPValueType = static_cast<AgentIOPValueTypes::Value>(valueType);

                switch (valueType)
                {
                case INTEGER_T: {
                    //int* newValue = (int*)value;
                    int newValue = mtic_readInputAsInt(name);
                    qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case DOUBLE_T: {
                    //double* newValue = (double*)value;
                    double newValue = mtic_readInputAsDouble(name);
                    qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case STRING_T: {
                    //QString newValue = QString((char*)value);
                    QString newValue = mtic_readInputAsString(name);
                    qDebug() << "New value" << newValue << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case BOOL_T: {
                    //bool* newValue = (bool*)value;
                    bool newValue = mtic_readInputAsBool(name);
                    if (newValue) {
                        qDebug() << "New value TRUE received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    }
                    else {
                        qDebug() << "New value FALSE received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    }
                    break;
                }
                case IMPULSION_T: {
                    qDebug() << "New IMPULSION received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    break;
                }
                case DATA_T: {
                    // On stocke dans un dossier le media (eg video, son, image) et on log le path et le start time!!.
                    void* data = NULL;
                    int result = mtic_readInputAsData(name, &data, &valueSize);
                    if (result == 1) {
                        qDebug() << "New DATA with size" << valueSize << "received on" << inputName << "with type" << AgentIOPValueTypes::staticEnumToString(agentIOPValueType);
                    }
                    else {
                        // FIXME TODO
                    }
                    break;
                }
                default: {
                    break;
                }
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
    _isMasticAgentStarted(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Init the name of our Mastic agent with the application name
    _editorAgentName = QApplication::instance()->applicationName();
    QString organizationName = QApplication::instance()->organizationName();
    QString version = QApplication::instance()->applicationVersion();

    // Set trace mode
    mtic_setVerbose(true);

    // Set the name of our agent
    mtic_setAgentName(_editorAgentName.toStdString().c_str());

    //
    // Read our internal definition
    //
    QString myDefinitionPath = QString("%1definition.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkDefinitionFile(myDefinitionPath);
    if (!checkDefinitionFile.exists() || !checkDefinitionFile.isFile())
    {
        qWarning() << "No definition has been found : " << myDefinitionPath << ". Set default definition";

        // Set definition and mapping by default to editor
        QString definitionByDefault = "{  \
                                      \"definition\": {  \
                                      \"name\": \""+ _editorAgentName + "\",   \
                                      \"description\": \"Definition of " + _editorAgentName + " made by "+ organizationName +"\",  \
                                      \"version\": \"" + version + "\",  \
                                      \"parameters\": [],   \
                                      \"inputs\": [],       \
                                      \"outputs\": [] }}";
        mtic_loadDefinition(definitionByDefault.toStdString().c_str());
    }
    else {
        mtic_loadDefinitionFromPath(myDefinitionPath.toStdString().c_str());
    }


    //
    // Read our internal mapping
    //
    QString myMappingPath = QString("%1mapping.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkMappingFile(myMappingPath);
    if (!checkMappingFile.exists() || !checkMappingFile.isFile())
    {
        qWarning() << "No mapping has been found : " << myMappingPath << ". Set default mapping";
        QString mappingByDefault = "{      \
                                      \"mapping\": {    \
                                      \"name\": \"" + _editorAgentName + "\",   \
                                      \"description\": \"Mapping of " + _editorAgentName + " made by "+ organizationName + "\",  \
                                      \"version\": \"" + version + "\",  \
                                      \"mapping_out\": [],   \
                                      \"mapping_cat\": [] }}";


        mtic_loadMapping(mappingByDefault.toStdString().c_str());
    }
    else {
        mtic_loadMappingFromPath(myMappingPath.toStdString().c_str());
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    if (_isMasticAgentStarted == 1) {
        // Stop network services
        mtic_stop();
    }
}


/**
 * @brief Start our MASTIC agent with a network device (or an IP address) and a port
 * @param networkDevice
 * @param ipAddress
 * @param port
 */
void NetworkController::start(QString networkDevice, QString ipAddress, int port)
{
    if (_isMasticAgentStarted == 0)
    {
        // Start service with network device
        if (!networkDevice.isEmpty()) {
            _isMasticAgentStarted = mtic_startWithDevice(networkDevice.toStdString().c_str(), port);
        }

        // Start service with ip address (if start with network device has failed)
        if ((_isMasticAgentStarted != 1) && !ipAddress.isEmpty()) {
            _isMasticAgentStarted = mtic_startWithIP(ipAddress.toStdString().c_str(), port);
        }

        if (_isMasticAgentStarted == 1)
        {
            qInfo() << "Mastic Agent" << _editorAgentName << "started";

            // Begin the observe on transiting zyre messages
            int result = network_observeZyre(&onIncommingZyreMessageCallback, this);

            if (result == 0) {
                qCritical() << "The callback on zyre messages has NOT been registered !";
            }
        }
        else {
            qCritical() << "The network has NOT been initialized on" << networkDevice << "or" << ipAddress << "and port" << QString::number(port);
        }
    }
}


/**
 * @brief Called when a MASTIC Launcher enter the network
 * @param hostname
 * @param peerId
 */
void NetworkController::masticLauncherEntered(QString hostname, QString peerId)
{
    qInfo() << "MASTIC Launcher on" << hostname << "entered";

    _mapFromHostnameToMasticLauncherPeerId.insert(hostname, peerId);
}


/**
 * @brief Called when a MASTIC Launcher exit the network
 * @param hostname
 */
void NetworkController::masticLauncherExited(QString hostname)
{
    qInfo() << "MASTIC Launcher on" << hostname << "exited";

    _mapFromHostnameToMasticLauncherPeerId.remove(hostname);
}


/**
 * @brief Slot when a command must be sent on the network to a launcher
 * @param command
 * @param hostname
 * @param executionPath
 */
void NetworkController::onCommandAskedToLauncher(QString command, QString hostname, QString executionPath)
{
    //Q_UNUSED(command)

    if (!hostname.isEmpty() && !executionPath.isEmpty())
    {
        if (_mapFromHostnameToMasticLauncherPeerId.contains(hostname)) {
            QString peerId = _mapFromHostnameToMasticLauncherPeerId.value(hostname);

            if (!peerId.isEmpty()) {
                // Send the command with execution path to the peer id of the launcher
                int success = zyre_whispers(agentElements->node, peerId.toStdString().c_str(), "%s %s", command.toStdString().c_str(), executionPath.toStdString().c_str());

                qInfo() << "Send command" << command << "to launcher on" << hostname << "with execution path" << executionPath << "with success ?" << success;
            }
        }
        else {
            qInfo() << "There is no launcher on" << hostname;
        }
    }
}


/**
 * @brief Slot when a command must be sent on the network
 * @param command
 * @param peerIdsList
 */
void NetworkController::onCommandAsked(QString command, QStringList peerIdsList)
{
    if (!command.isEmpty() && (peerIdsList.count() > 0)) {
        foreach (QString peerId, peerIdsList)
        {
            // Send the command to a peer id of agent
            int success = zyre_whispers(agentElements->node, peerId.toStdString().c_str(), "%s", command.toStdString().c_str());

            qDebug() << "Send command" << command << "for agent" << peerId << "with success ?" << success;
        }
    }
}


/**
 * @brief Slot when a command for an output must be sent on the network
 * @param command
 * @param outputName
 * @param peerIdsList
 */
void NetworkController::onCommandAskedForOutput(QString command, QString outputName, QStringList peerIdsList)
{
    if (!command.isEmpty() && !outputName.isEmpty() && (peerIdsList.count() > 0)) {
        foreach (QString peerId, peerIdsList)
        {
            // Send the command with the output name to a peer id of agent
            int success = zyre_whispers(agentElements->node, peerId.toStdString().c_str(), "%s %s", command.toStdString().c_str(), outputName.toStdString().c_str());

            qDebug() << "Send command" << command << "for agent" << peerId << "and output" << outputName << "with success ?" << success;
        }
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
        if (output != NULL)
        {
            QString inputName = QString("%1%2%3").arg(agentName, NAME_SEPARATOR, output->id());

            int resultCreateInput = 0;

            switch (output->agentIOPValueType())
            {
            case AgentIOPValueTypes::INTEGER: {
                bool success = false;
                int defaultValue = output->defaultValue().toInt(&success);
                if (success) {
                    resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), INTEGER_T, &defaultValue, sizeof(int));
                }
                break;
            }
            case AgentIOPValueTypes::DOUBLE: {
                bool success = false;
                double defaultValue = output->defaultValue().toDouble(&success);
                if (success) {
                    resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), DOUBLE_T, &defaultValue, sizeof(double));
                }
                break;
            }
            case AgentIOPValueTypes::STRING: {
                const char* defaultValue = output->defaultValue().toString().toStdString().c_str();
                //resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), STRING_T, (void*)defaultValue, strlen(defaultValue) * sizeof(char));
                resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), STRING_T, (void*)defaultValue, (strlen(defaultValue) + 1) * sizeof(char));
                break;
            }
            case AgentIOPValueTypes::BOOL: {
                bool defaultValue = output->defaultValue().toBool();
                resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), BOOL_T, &defaultValue, sizeof(bool));
                break;
            }
            case AgentIOPValueTypes::IMPULSION: {
                resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), IMPULSION_T, NULL, 0);
                break;
            }
            case AgentIOPValueTypes::DATA: {
                // FIXME TODO
                //resultCreateInput = mtic_createInput(inputName.toStdString().c_str(), DATA_T, &defaultValue, sizeof(...));
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
                int resultObserveInput = mtic_observeInput(inputName.toStdString().c_str(), &onObserveInputCallback, this);

                if (resultObserveInput == 1) {
                    qDebug() << "Observe input" << inputName << "on agent" << _editorAgentName;
                }
                else {
                    qCritical() << "Can NOT observe input" << inputName << "on agent" << _editorAgentName << "Error code:" << resultObserveInput;
                }

                // Add mapping between our input and this output
                unsigned long id = mtic_addMappingEntry(inputName.toStdString().c_str(), agentName.toStdString().c_str(), output->name().toStdString().c_str());

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
    }
}


/**
 * @brief Slot when inputs must be removed to our Editor for a list of outputs
 * @param agentName
 * @param pairsList
 */
void NetworkController::onRemoveInputsToEditorForOutputs(QString agentName, QList<QPair<QString, QString>> pairsList)
{
    for (int i = 0; i < pairsList.count(); i++)
    {
        QPair<QString, QString> pair = pairsList.at(i);
        QString outputId = pair.first;
        QString outputName = pair.second;

        if (!outputId.isEmpty() && !outputName.isEmpty())
        {
            QString inputName = QString("%1%2%3").arg(agentName, NAME_SEPARATOR, outputId);

            // Remove mapping between our input and this output
            int resultRemoveMappingEntry = mtic_removeMappingEntryWithName(inputName.toStdString().c_str(), agentName.toStdString().c_str(), outputName.toStdString().c_str());

            if (resultRemoveMappingEntry == 1) {
                qDebug() << "Remove mapping between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName;

                // Remove our input
                int resultRemoveInput = mtic_removeInput(inputName.toStdString().c_str());

                if (resultRemoveInput == 1) {
                    qDebug() << "Remove input" << inputName << "on agent" << _editorAgentName;
                }
                else {
                    qCritical() << "Can NOT remove input" << inputName << "on agent" << _editorAgentName << "Error code:" << resultRemoveInput;
                }
            }
            else {
                qCritical() << "Can NOT remove mapping between output" << outputName << "of agent" << agentName << "and input" << inputName << "of agent" << _editorAgentName << "Error code:" << resultRemoveMappingEntry;
            }
        }
    }
}

