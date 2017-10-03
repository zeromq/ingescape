/*
 *	NetworkController
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Deliencourt  <deliencourt@ingenuity.io>
 *
 */

#include "networkcontroller.h"


#include <QDebug>




static const QString definitionPrefix = "DEFINITION#";
static const QString mappingPrefix = "MAPPING#";

#include "misc/masticeditorutils.h"

int myZyreIncommingMessageCallback (const zyre_event_t *cst_zyre_event, void *arg)
{
    NetworkController * myNetworkController = (NetworkController *)arg;

    if(myNetworkController != NULL)
    {
        zyre_event_t * zyre_event = (zyre_event_t *)cst_zyre_event;
        QString event = zyre_event_type(zyre_event);
        QString peer = zyre_event_peer_uuid(zyre_event);
        QString name = zyre_event_peer_name (zyre_event);
        QString address = zyre_event_peer_addr (zyre_event);
        //zhash_t *headers = zyre_event_headers (zyre_event);
        QString group = zyre_event_group (zyre_event);
        zmsg_t *msg = zyre_event_msg (zyre_event);

        //parse event
        if (event.compare("ENTER") == 0)
        {
            qDebug() << QString("->%1 has entered the network with peer id %2and address %3\n").arg( name, peer, address);
        } else if (event.compare("JOIN") == 0){
            qDebug() << QString("+%1 has joined %2").arg(name, group);

        } else if (event.compare("LEAVE") == 0){
            qDebug() << QString("-%1 has left %2").arg(name, group);
        } else if (event.compare("SHOUT") == 0){
            //nothing to do so far
        } else if(event.compare("WHISPER") == 0){
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr (msg_dup);

            //check if message is a definition
            if(message.startsWith(definitionPrefix) == true)
            {
                message.remove(0,definitionPrefix.length());

                // FIXME - TEST ONLY - TO REMOVE
                // Load definition from string content
                definition *newDefinition = parser_loadDefinition(message.toStdString().c_str());
                // Load definition from string content
                qDebug() << "Definition received from : " << newDefinition->name << " version : " << newDefinition->version << " description : " <<newDefinition->description;
                definition_free_definition(newDefinition);

                myNetworkController->agentEntered(name, address, peer, message);
            }
            //check if message is mapping
            else if (message.startsWith(mappingPrefix) == true)
            {
                message.remove(0,mappingPrefix.length());
                qDebug() << "Mapping : " << message;
            }else{
                //other supported messages
                if (message.startsWith("MAPPED") == true){
                    qDebug() << QString("Mapping notification received from %s").arg(name);
                }
            }
            zmsg_destroy(&msg_dup);
        } else if (event.compare("EXIT") == 0){
            qDebug() << QString("<-%1 exited").arg(name);
            myNetworkController->agentExited(peer);
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
    if (checkDefinitionFile.exists() && checkDefinitionFile.isFile())
    {
        mtic_setAgentName("MASTIC-Editor");

        // Start service with network device
        if (networkDevice.isEmpty() == false)
        {
            networkInitialized = mtic_startWithDevice(networkDevice.toStdString().c_str(),port);
        }

        // Start service with ip if start with network device has failed
        if ((networkInitialized != 1) && (ipAddress.isEmpty() == false))
        {
            networkInitialized = mtic_startWithIP(ipAddress.toStdString().c_str(),port);
        }
    }
    else
    {
        qCritical() << "No definition has been found : " << myDefinitionPath;
    }

    if (networkInitialized == 1)
    {
        qInfo() << "Network services started";
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


