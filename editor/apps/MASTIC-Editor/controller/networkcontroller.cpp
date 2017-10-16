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


static const QString definitionPrefix = "EXTERNAL_DEFINITION#";
static const QString mappingPrefix = "EXTERNAL_MAPPING#";

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

                // Emit signal "Agent Entered"
                Q_EMIT networkController->agentEntered(peerId, peerName, peerAddress, pid, hostname, executionPath, canBeFrozen);
            }
        }
        else if (event.compare("JOIN") == 0)
        {
            //qDebug() << QString("++ %1 has joined %2").arg(peerName, group);
        }
        else if (event.compare("LEAVE") == 0)
        {
            //qDebug() << QString("-- %1 has left %2").arg(peerName, group);
        }
        else if (event.compare("SHOUT") == 0)
        {
            // nothing to do so far
        }
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

                // Emit signal "Definition Received"
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

                // Emit signal "Mapping Received"
                Q_EMIT networkController->mappingReceived(peerId, peerName, message);
            }
            else if (message.startsWith("MAPPED"))
            {
                qDebug() << peerName << "MAPPED" << message;
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

            // Emit signal "Agent Exited"
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
    QString myDefinitionPath = QString("%1/definition.json").arg(MasticEditorUtils::getDataPath());
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
    QString myMappingPath = QString("%1/mapping.json").arg(MasticEditorUtils::getDataPath());
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
 * @brief Slot when a command must be sent on the network
 * @param peerIdsList
 * @param command
 */
void NetworkController::onCommandAsked(QStringList peerIdsList, QString command)
{
    if ((peerIdsList.count() > 0) && !command.isEmpty()) {
        foreach (QString peerId, peerIdsList)
        {
            // Send the command for a peer id of agent
            int success = zyre_whispers(agentElements->node, peerId.toStdString().c_str(), "%s", command.toStdString().c_str());

            qDebug() << "Send command" << command << "for agent" << peerId << "with success ?" << success;
        }
    }
}

