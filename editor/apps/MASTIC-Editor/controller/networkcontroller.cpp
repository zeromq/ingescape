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
            else if (message.startsWith("MAPPED"))
            {
                qDebug() << peerName << "MAPPED" << message;
            }
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


//--------------------------------------------------------------
//
// NetworkController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param network device
 * @param ip address
 * @param port number
 * @param parent
 */
NetworkController::NetworkController(QString networkDevice, QString ipAddress, int port, QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Application name
    QString agentName = QApplication::instance()->applicationName();

    // Network is ok if the result of mtic_startWithDevice is 1, O otherwise.
    int networkInitialized = 0;

    // Set trace mode
    mtic_setVerbose(true);

    // Read our internal definition
    QString myDefinitionPath = QString("%1definition.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkDefinitionFile(myDefinitionPath);
    if (!checkDefinitionFile.exists() || !checkDefinitionFile.isFile())
    {
        qWarning() << "No definition has been found : " << myDefinitionPath << ". Set definition by default";
        // Set definition and mapping by default to editor
        QString definitionByDefault = "{  \
                                      \"definition\": {  \
                                      \"name\": \""+agentName+"\",   \
                                      \"description\": \"Definition of "+agentName+" made by "+QApplication::instance()->organizationName()+"\",  \
                                      \"version\": \""+QApplication::instance()->applicationVersion()+"\",  \
                                      \"parameters\": [],   \
                                      \"inputs\": [],       \
                                      \"outputs\": [] }}";
        mtic_loadDefinition(definitionByDefault.toStdString().c_str());
    }
    else {
        mtic_loadDefinitionFromPath(myDefinitionPath.toStdString().c_str());
    }

    // Read our internal mapping
    QString myMappingPath = QString("%1mapping.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkMappingFile(myMappingPath);
    if (!checkMappingFile.exists() || !checkMappingFile.isFile())
    {
        qWarning() << "No mapping has been found : " << myMappingPath << ". Set definition by default";
        QString mappingByDefault = "{      \
                                      \"mapping\": {    \
                                      \"name\": \""+agentName+"\",   \
                                      \"description\": \"Mapping of "+agentName+" made by "+QApplication::instance()->organizationName()+"\",  \
                                      \"version\": \""+QApplication::instance()->applicationVersion()+"\",  \
                                      \"mapping_out\": [],   \
                                      \"mapping_cat\": [] }}";


        mtic_loadMapping(mappingByDefault.toStdString().c_str());
    }
    else {
        mtic_loadMappingFromPath(myMappingPath.toStdString().c_str());
    }

    mtic_setAgentName(agentName.toStdString().c_str());

    // Start service with network device
    if (!networkDevice.isEmpty())
    {
        networkInitialized = mtic_startWithDevice(networkDevice.toStdString().c_str(), port);
    }

    // Start service with ip if start with network device has failed
    if ((networkInitialized != 1) && !ipAddress.isEmpty())
    {
        networkInitialized = mtic_startWithIP(ipAddress.toStdString().c_str(), port);
    }

    if (networkInitialized == 1)
    {
        qInfo() << "Network services started ";

        // begin the observe on transiting zyre messages
        int result = network_observeZyre(&onIncommingZyreMessageCallback, this);

        qInfo() << "Network services started result=" << QString::number(result);
    }
    else
    {
        qCritical() << "The network has not been initialized on " << networkDevice << ipAddress << QString::number(port);
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Stop network services
    mtic_stop();
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

