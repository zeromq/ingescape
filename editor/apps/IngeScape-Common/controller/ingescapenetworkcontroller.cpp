/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou  <peyruqueou@ingenuity.io>
 *
 */

#include "ingescapenetworkcontroller.h"

#include <QDebug>
#include <QApplication>


static const QString suffix_Launcher = ".ingescapelauncher";


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

    IngeScapeNetworkController* networkController = (IngeScapeNetworkController*)myData;
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
            bool isIngeScapeLauncher = false;
            bool isIngeScapeRecorder = false;
            bool isIngeScapeEditor = false;
            bool isIngeScapeAssessments = false;
            bool isIngeScapeExpe = false;
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

                    if (key == "isLauncher") {
                        if (value == "1") {
                            isIngeScapeLauncher = true;
                        }
                    }
                    else if (key == "isRecorder") {
                        if (value == "1") {
                            isIngeScapeRecorder = true;
                        }
                    }
                    else if (key == "isEditor") {
                        if (value == "1") {
                            isIngeScapeEditor = true;
                        }
                    }
                    else if (key == "isAssessments") {
                        if (value == "1") {
                            isIngeScapeAssessments = true;
                        }
                    }
                    else if (key == "isExpe") {
                        if (value == "1") {
                            isIngeScapeExpe = true;
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
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::LAUNCHER);
                networkController->setnumberOfLaunchers(networkController->numberOfLaunchers() + 1);

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
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::RECORDER);
                networkController->setnumberOfRecorders(networkController->numberOfRecorders() + 1);

                // Emit the signal "Recorder Entered"
                Q_EMIT networkController->recorderEntered(peerId, peerName, ipAddress, hostname);
            }
            // IngeScape EDITOR
            else if (isIngeScapeEditor)
            {
                qDebug() << "Our zyre event is about IngeScape EDITOR";

                // Save the peer id of this editor
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::EDITOR);
                networkController->setnumberOfEditors(networkController->numberOfEditors() + 1);

                // Emit the signal "Editor Entered"
                Q_EMIT networkController->editorEntered(peerId, peerName, ipAddress, hostname);
            }
            // IngeScape ASSESSMENTS
            else if (isIngeScapeAssessments)
            {
                qDebug() << "Our zyre event is about IngeScape ASSESSMENTS";

                // Save the peer id of this recorder
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::ASSESSMENTS);
                networkController->setnumberOfAssessments(networkController->numberOfAssessments() + 1);

                // Emit the signal "Assessments Entered"
                Q_EMIT networkController->assessmentsEntered(peerId, peerName, ipAddress, hostname);
            }
            // IngeScape EXPE
            else if (isIngeScapeExpe)
            {
                qDebug() << "Our zyre event is about IngeScape EXPE";

                // Save the peer id of this recorder
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::EXPE);
                networkController->setnumberOfExpes(networkController->numberOfExpes() + 1);

                // Emit the signal "Expe Entered"
                Q_EMIT networkController->expeEntered(peerId, peerName, ipAddress, hostname);
            }
            // IngeScape AGENT
            else if (nbKeys > 0)
            {
                qDebug() << "Our zyre event is about IngeScape AGENT on" << hostname;

                // Save the peer id of this agent
                networkController->manageEnteredPeerId(peerId, IngeScapeTypes::AGENT);
                networkController->setnumberOfAgents(networkController->numberOfAgents() + 1);

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

            // Manage the "Shouted" message
            networkController->manageShoutedMessage(peerId, peerName, msg_dup);

            zmsg_destroy(&msg_dup);
        }
        // WHISPER
        else if (streq(event, "WHISPER"))
        {
            zmsg_t* msg_dup = zmsg_dup(msg);

            // Manage the "Whispered" message
            networkController->manageWhisperedMessage(peerId, peerName, msg_dup);

            zmsg_destroy(&msg_dup);
        }
        // EXIT
        else if (streq(event, "EXIT"))
        {
            qDebug() << QString("<-- %1 (%2) exited").arg(peerName, peerId);

            // Get the IngeScape type of a peer id
            IngeScapeTypes::Value ingeScapeType = networkController->getIngeScapeTypeOfPeerId(peerId);

            switch (ingeScapeType)
            {
            // IngeScape AGENT
            case IngeScapeTypes::AGENT:
            {
                // Emit the signal "Agent Exited"
                Q_EMIT networkController->agentExited(peerId, peerName);

                networkController->setnumberOfAgents(networkController->numberOfAgents() - 1);

                break;
            }
            // IngeScape LAUNCHER
            case IngeScapeTypes::LAUNCHER:
            {
                QString hostname = "";

                if (peerName.endsWith(suffix_Launcher)) {
                    hostname = peerName.left(peerName.length() - suffix_Launcher.length());
                }

                // Emit the signal "Launcher Exited"
                Q_EMIT networkController->launcherExited(peerId, hostname);

                networkController->setnumberOfLaunchers(networkController->numberOfLaunchers() - 1);

                break;
            }
            // IngeScape RECORDER
            case IngeScapeTypes::RECORDER:
            {
                // Emit the signal "Recorder Exited"
                Q_EMIT networkController->recorderExited(peerId, peerName);

                networkController->setnumberOfRecorders(networkController->numberOfRecorders() - 1);

                break;
            }
            // IngeScape EDITOR
            case IngeScapeTypes::EDITOR:
            {
                // Emit the signal "Editor Exited"
                Q_EMIT networkController->editorExited(peerId, peerName);

                networkController->setnumberOfEditors(networkController->numberOfEditors() - 1);

                break;
            }
            // IngeScape ASSESSMENTS
            case IngeScapeTypes::ASSESSMENTS:
            {
                // Emit the signal "Assessments Exited"
                Q_EMIT networkController->assessmentsExited(peerId, peerName);

                networkController->setnumberOfAssessments(networkController->numberOfAssessments() - 1);

                break;
            }
            // IngeScape EXPE
            case IngeScapeTypes::EXPE:
            {
                // Emit the signal "Expe Exited"
                Q_EMIT networkController->expeExited(peerId, peerName);

                networkController->setnumberOfExpes(networkController->numberOfExpes() - 1);

                break;
            }
            default:
                qWarning() << "Unknown peer id" << peerId << "(" << peerName << ")";
                break;
            }

            // Manage the peer id which exited the network
            networkController->manageExitedPeerId(peerId);
        }
    }
}


/**
 * @brief onLicenseCallback
 * @param limit
 * @param myData
 */
void onLicenseCallback(igs_license_limit_t limit, void *myData)
{
    IngeScapeNetworkController* networkController = (IngeScapeNetworkController*)myData;
    if (networkController != nullptr)
    {
        switch (limit)
        {
        case IGS_LICENSE_TIMEOUT:
            qCritical("IngeScape is stopped because demonstration mode timeout has been reached");
            //Q_EMIT networkController->licenseError();
            break;

        case IGS_LICENSE_TOO_MANY_AGENTS:
            qCritical("IngeScape is stopped because too many agents are running on the platform compared to what the license allows");
            break;

        case IGS_LICENSE_TOO_MANY_IOPS:
            qCritical("IngeScape is stopped because too many IOPs have been created on the platform compared to what the license allows");
            break;

        default:
            break;
        }
    }
}


//--------------------------------------------------------------
//
// IngeScape Network Controller
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param igsServiceDescription
 * @param parent
 */
IngeScapeNetworkController::IngeScapeNetworkController(QObject *parent) : QObject(parent),
    _numberOfAgents(0),
    _numberOfLaunchers(0),
    _numberOfRecorders(0),
    _numberOfEditors(0),
    _numberOfAssessments(0),
    _numberOfExpes(0),
    _igsAgentApplicationName(""),
    _isIngeScapeAgentStarted(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Init the name of the "IngeScape" agent that correspond to our application with the application name
    _igsAgentApplicationName = QApplication::instance()->applicationName();
    QString organizationName = QApplication::instance()->organizationName();
    QString version = QApplication::instance()->applicationVersion();

    // Set the name of our agent
    igs_setAgentName(_igsAgentApplicationName.toStdString().c_str());


    //
    // Create our internal definition
    //
    QString definitionByDefault = "{  \
                                  \"definition\": {  \
                                  \"name\": \""+ _igsAgentApplicationName + "\",   \
                                  \"description\": \"Definition of " + _igsAgentApplicationName + " made by "+ organizationName +"\",  \
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
                                  \"name\": \"" + _igsAgentApplicationName + "\",   \
                                  \"description\": \"Mapping of " + _igsAgentApplicationName + " made by "+ organizationName + "\",  \
                                  \"version\": \"" + version + "\",  \
                                  \"mapping_out\": [],   \
                                  \"mapping_cat\": [] }}";

    igs_loadMapping(mappingByDefault.toStdString().c_str());


    // Begin to observe incoming messages on the bus
    int result = igs_observeBus(&onIncommingBusMessageCallback, this);
    if (result == 0) {
        qCritical() << "The callback on zyre messages has NOT been registered !";
    }

    // Begin to observe license events (events are triggered only when no valid license is available)
    igs_observeLicense(onLicenseCallback, this);
}


/**
 * @brief Destructor
 */
IngeScapeNetworkController::~IngeScapeNetworkController()
{
    // Stop our IngeScape agent
    stop();
}


/**
 * @brief Start our IngeScape agent with a network device (or an IP address) and a port
 * @param networkDevice
 * @param ipAddress
 * @param port
 * @return
 */
bool IngeScapeNetworkController::start(QString networkDevice, QString ipAddress, uint port)
{
    if (_isIngeScapeAgentStarted == 0)
    {
        // Start service with network device
        if (!networkDevice.isEmpty())
        {
#ifdef Q_OS_WIN
            // igs_startWithDevice compares networkDevice with latin1 values to get the IP address of the network device
            _isIngeScapeAgentStarted = igs_startWithDevice(networkDevice.toLatin1().toStdString().c_str(), port);

#else
            _isIngeScapeAgentStarted = igs_startWithDevice(networkDevice.toStdString().c_str(), port);
#endif
        }

        // Start service with ip address (if start with network device has failed)
        if ((_isIngeScapeAgentStarted != 1) && !ipAddress.isEmpty())
        {
            _isIngeScapeAgentStarted = igs_startWithIP(ipAddress.toStdString().c_str(), port);
        }

        if (_isIngeScapeAgentStarted == 1)
        {
            qInfo() << "IngeScape Agent" << _igsAgentApplicationName << "started";
        }
        else {
            qCritical() << "The network has NOT been initialized on" << networkDevice << "or" << ipAddress << "and port" << QString::number(port);
        }
    }

    return _isIngeScapeAgentStarted;
}


/**
 * @brief Stop our IngeScape agent
 */
void IngeScapeNetworkController::stop()
{
    if (_isIngeScapeAgentStarted == 1)
    {
        // Stop network services
        igs_stop();

        _isIngeScapeAgentStarted = 0;
    }
}


/**
 * @brief Get the IngeScape type of a peer id
 * @param peerId
 * @return
 */
IngeScapeTypes::Value IngeScapeNetworkController::getIngeScapeTypeOfPeerId(QString peerId)
{
    if (_hashFromPeerIdToIngeScapeType.contains(peerId)) {
        return _hashFromPeerIdToIngeScapeType.value(peerId);
    }
    else {
        return IngeScapeTypes::UNKNOWN;
    }
}


/**
 * @brief Manage a peer id which entered the network
 * @param peerId
 * @param ingeScapeType
 */
void IngeScapeNetworkController::manageEnteredPeerId(QString peerId, IngeScapeTypes::Value ingeScapeType)
{
    if (!_hashFromPeerIdToIngeScapeType.contains(peerId)) {
        _hashFromPeerIdToIngeScapeType.insert(peerId, ingeScapeType);
    }
}


/**
 * @brief Manage a peer id which exited the network
 * @param peerId
 */
void IngeScapeNetworkController::manageExitedPeerId(QString peerId)
{
    if (_hashFromPeerIdToIngeScapeType.contains(peerId)) {
        _hashFromPeerIdToIngeScapeType.remove(peerId);
    }
}


/**
 * @brief Update the list of available network devices
 */
void IngeScapeNetworkController::updateAvailableNetworkDevices()
{
    QStringList networkDevices;

    char **devices = nullptr;
    int nb = 0;
    igs_getNetdevicesList(&devices, &nb);

    for (int i = 0; i < nb; i++)
    {
#ifdef Q_OS_WIN
        // igs_getNetdevicesList return latin1 values
        QString availableNetworkDevice = QString::fromLatin1(devices[i]);
#else
        QString availableNetworkDevice = QString(devices[i]);
#endif

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
bool IngeScapeNetworkController::isAvailableNetworkDevice(QString networkDevice)
{
    return (!networkDevice.isEmpty() && _availableNetworkDevices.contains(networkDevice));
}


/**
 * @brief Manage a "Shouted" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void IngeScapeNetworkController::manageShoutedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{
    QString message = zmsg_popstr(zMessage);

    qDebug() << "Not yet managed SHOUTED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
}


/**
 * @brief Manage a "Whispered" message
 * @param peerId
 * @param peerName
 * @param zMessage
 */
void IngeScapeNetworkController::manageWhisperedMessage(QString peerId, QString peerName, zmsg_t* zMessage)
{
    QString message = zmsg_popstr(zMessage);

    qDebug() << "Not yet managed WHISPERED message '" << message << "' for agent" << peerName << "(" << peerId << ")";
}
