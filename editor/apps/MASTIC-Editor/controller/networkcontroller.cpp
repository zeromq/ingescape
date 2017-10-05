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

extern "C" {
#include <mastic_private.h>
}

#include "misc/masticeditorutils.h"

static const QString definitionPrefix = "EXTERNAL_DEFINITION#";
static const QString mappingPrefix = "EXTERNAL_MAPPING#";

#include "misc/masticeditorutils.h"

/**
 * @brief myZyreIncommingMessageCallback
 * @param cst_zyre_event
 * @param arg
 * @return
 */
int myZyreIncommingMessageCallback (const zyre_event_t *cst_zyre_event, void *arg)
{
    NetworkController * myNetworkController = (NetworkController *)arg;

    if (myNetworkController != NULL)
    {
        // Usefull to create a new one ?
        zyre_event_t* zyre_event = (zyre_event_t *)cst_zyre_event;

        QString event = zyre_event_type(zyre_event);
        QString peerId = zyre_event_peer_uuid(zyre_event);
        QString name = zyre_event_peer_name(zyre_event);
        QString address = zyre_event_peer_addr(zyre_event);
        zhash_t* headers = zyre_event_headers(zyre_event);
        QString group = zyre_event_group(zyre_event);
        zmsg_t* msg = zyre_event_msg(zyre_event);

        // ENTER
        if (event.compare("ENTER") == 0)
        {
            qDebug() << QString("--> %1 has entered the network with peer id %2 and address %3").arg( name, peerId, address);

            char *k;
            const char *v;
            zlist_t *keys = zhash_keys(headers);
            size_t nbKeys = zlist_size(keys);
            if (nbKeys > 0){
                qDebug() << nbKeys << "headers";

                while ((k = (char *)zlist_pop(keys))) {
                    v = zyre_event_header (zyre_event, k);
                    qDebug() << "key" << k << ":" << v;

                    // we extract the publisher adress to subscribe to from the zyre message header
                    if(strncmp(k, "publisher", strlen("publisher")) == 0)
                    {
                        qDebug() << "our zyre event is about MASTIC publisher !";

                        // Subscribers
                        int n = HASH_COUNT(subscribers);
                        qDebug() << n << "subscribers in the list";
                        subscriber_t *sub, *tmpSub;
                        HASH_ITER(hh, subscribers, sub, tmpSub) {
                            qDebug() << "subscriber:" << sub->agentName << "with peer id" << sub->agentPeerId;
                        }
                    }
                }
            }
        }
        else if (event.compare("JOIN") == 0)
        {
            qDebug() << QString("+%1 has joined %2").arg(name, group);
        }
        else if (event.compare("LEAVE") == 0)
        {
            qDebug() << QString("-%1 has left %2").arg(name, group);
        }
        else if (event.compare("SHOUT") == 0)
        {
            // nothing to do so far
        }
        else if (event.compare("WHISPER") == 0)
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            // check if message is about Definition
            if(message.startsWith(definitionPrefix))
            {
                message.remove(0, definitionPrefix.length());

                // FIXME - TEST ONLY - TO REMOVE
                // Load definition from string content
                definition *newDefinition = parser_loadDefinition(message.toStdString().c_str());
                // Load definition from string content
                qDebug() << "Definition received from : " << newDefinition->name << " version : " << newDefinition->version << " description : " <<newDefinition->description;
                definition_freeDefinition(newDefinition);

                // Emit signal "agentEntered"
                emit myNetworkController->agentEntered(name, address, peerId, message);
            }
            // check if message is about Mapping
            else if (message.startsWith(mappingPrefix))
            {
                message.remove(0, mappingPrefix.length());
                qDebug() << "Mapping : " << message;
            }
            else {
                // Others supported messages
                if (message.startsWith("MAPPED")) {
                    qDebug() << QString("Mapping notification received from %s").arg(name);
                }
                else {
                    qDebug() << "unknown message received : " << message;
                }
            }
            zmsg_destroy(&msg_dup);
        }
        // EXIT
        else if (event.compare("EXIT") == 0)
        {
            qDebug() << QString("<-- %1 (%2) exited").arg(name, peerId);

            // Emit signal "agentExited"
            emit myNetworkController->agentExited(peerId);
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
 * @param network devece
 * @param ip address
 * @param port number
 * @param parent
 */
NetworkController::NetworkController(QString networkDevice, QString ipAddress, int port, QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    // Network is ok if the result of mtic_startWithDevice is 1, O otherwise.
    int networkInitialized = 0;

    // Set trace mode
    mtic_setVerbose(true);

    // Read our internal definition
    QString myDefinitionPath = QString("%1/definition.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkDefinitionFile(myDefinitionPath);
    if (!checkDefinitionFile.exists() || !checkDefinitionFile.isFile())
    {
        qWarning() << "No definition has been found : " << myDefinitionPath;
    }

    mtic_setAgentName("MASTIC-Editor");

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
        int result = network_observeZyre(&myZyreIncommingMessageCallback, this);
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


