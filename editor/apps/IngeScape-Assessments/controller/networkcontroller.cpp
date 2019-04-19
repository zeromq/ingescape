/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
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
    if (networkController != nullptr)
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
            //bool isIngeScapeAssessments = false;
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

            // IngeScape EDITOR
            if (isIngeScapeEditor)
            {
                qDebug() << "Our zyre event is about IngeScape EDITOR";
            }
            /*else {
                qDebug() << "Our zyre event is about an element without headers, we ignore it !";
            }*/
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
            /*if (message.startsWith(prefix_Muted))
            {
                // Manage the message "MUTED / UN-MUTED"
                networkController->manageMessageMutedUnmuted(peerId, message.remove(0, prefix_Muted.length()));
            }
            // CAN BE FROZEN / CAN NOT BE FROZEN
            else if (message.startsWith(prefix_CanBeFrozen))
            {
                // Manage the message "CAN BE FROZEN / CAN NOT BE FROZEN"
                networkController->manageMessageCanBeFrozenOrNot(peerId, message.remove(0, prefix_CanBeFrozen.length()));
            }
            else
            {*/
                qWarning() << "Not yet managed (SHOUT) message '" << message << "' for agent" << peerName << "(" << peerId << ")";
            //}

            zmsg_destroy(&msg_dup);
        }
        // WHISPER
        else if (streq(event, "WHISPER"))
        {
            zmsg_t* msg_dup = zmsg_dup(msg);
            QString message = zmsg_popstr(msg_dup);

            // Definition
            /*if (message.startsWith(prefix_Definition))
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
            else
            {*/
                qWarning() << "Not yet managed (WHISPER) message '" << message << "' for agent" << peerName << "(" << peerId << ")";
            //}

            zmsg_destroy(&msg_dup);
        }
        // EXIT
        else if (streq(event, "EXIT"))
        {
            qDebug() << QString("<-- %1 (%2) exited").arg(peerName, peerId);

            /*// Get the IngeScape type of a peer id
            IngeScapeTypes::Value ingeScapeType = networkController->getIngeScapeTypeOfPeerId(peerId);

            switch (ingeScapeType)
            {
            // IngeScape LAUNCHER
            case IngeScapeTypes::LAUNCHER:
            {
                QString hostname = "";

                if (peerName.endsWith(suffix_Launcher)) {
                    hostname = peerName.left(peerName.length() - suffix_Launcher.length());
                }

                // Emit the signal "Launcher Exited"
                Q_EMIT networkController->launcherExited(peerId, hostname);

                break;
            }
            // IngeScape RECORDER
            case IngeScapeTypes::RECORDER:
            {
                // Emit the signal "Recorder Exited"
                Q_EMIT networkController->recorderExited(peerId, peerName);

                break;
            }
            // IngeScape AGENT
            case IngeScapeTypes::AGENT:
            {
                // Emit the signal "Agent Exited"
                Q_EMIT networkController->agentExited(peerId, peerName);

                break;
            }
            default:
                qWarning() << "Unknown peer id" << peerId << "(" << peerName << ")";
                break;
            }

            // Manage the peer id which exited the network
            networkController->manageExitedPeerId(peerId);*/
        }
    }
}


//--------------------------------------------------------------
//
// NetworkController
//
//--------------------------------------------------------------


/**
 * @brief Constructor
 * @param parent
 */
NetworkController::NetworkController(QObject *parent) : QObject(parent),
    _assessmentsAgentName(""),
    _isIngeScapeAgentStarted(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Init the name of our IngeScape agent with the application name
    _assessmentsAgentName = QApplication::instance()->applicationName();
    QString organizationName = QApplication::instance()->organizationName();
    QString version = QApplication::instance()->applicationVersion();

    // Set trace mode
    igs_setVerbose(true);

    // Set the name of our agent
    igs_setAgentName(_assessmentsAgentName.toStdString().c_str());

    // Add  header to declare ourselves as assessments
    //igs_busAddServiceDescription("isAssessments", "1");

    //
    // Create our internal definition
    //
    // Set definition and mapping by default to editor
    QString definitionByDefault = "{  \
                                  \"definition\": {  \
                                  \"name\": \""+ _assessmentsAgentName + "\",   \
                                  \"description\": \"Definition of " + _assessmentsAgentName + " made by "+ organizationName +"\",  \
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
                                  \"name\": \"" + _assessmentsAgentName + "\",   \
                                  \"description\": \"Mapping of " + _assessmentsAgentName + " made by "+ organizationName + "\",  \
                                  \"version\": \"" + version + "\",  \
                                  \"mapping_out\": [],   \
                                  \"mapping_cat\": [] }}";

    igs_loadMapping(mappingByDefault.toStdString().c_str());


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
    // Stop our IngeScape agent
    stop();

}


/**
 * @brief Start our INGESCAPE agent with a network device (or an IP address) and a port
 * @param networkDevice
 * @param ipAddress
 * @param port
 * @return
 */
bool NetworkController::start(QString networkDevice, QString ipAddress, uint port)
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
            qInfo() << "IngeScape Agent" << _assessmentsAgentName << "started";
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
 * @brief Update the list of available network devices
 */
void NetworkController::updateAvailableNetworkDevices()
{
    QStringList networkDevices;

    char **devices = nullptr;
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
