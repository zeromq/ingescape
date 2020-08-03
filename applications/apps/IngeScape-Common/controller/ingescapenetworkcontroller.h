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
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */

#ifndef INGESCAPE_NETWORKCONTROLLER_H
#define INGESCAPE_NETWORKCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/enums.h>
#include <model/publishedvaluem.h>
#include "model/peerm.h"
#include <model/agent/agentm.h>


extern "C" {
#include "ingescape_private.h"
#include <czmq.h>
}


static const QString MESSAGE_DEFINITION = "EXTERNAL_DEFINITION#";
static const QString MESSAGE_MAPPING = "EXTERNAL_MAPPING#";
static const QString MESSAGE_REMOTE_AGENT_EXIT = "REMOTE_AGENT_EXIT";


/**
 * @brief The IngeScapeNetworkController class defines the controller for IngeScape network communications
 */
class IngeScapeNetworkController : public QObject
{
    Q_OBJECT

    // List of available network devices
    I2_QML_PROPERTY_READONLY(QStringList, availableNetworkDevices)

    // List of addresses of available network devices
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, availableNetworkDevicesAddresses)

    // Flag indicating if our agent is started
    I2_QML_PROPERTY_READONLY(bool, isStarted)

    // Flag indicating if we must start/stop our agent automatically when we receive monitor events
    // NB: this flags exists to allow each application based on IngeScape-Common to define its own behavior
    //     By default, this property is set to false
    I2_QML_PROPERTY(bool, automaticallyStartStopOnMonitorEvents)

    // Model of our agent "IngeScape"
    I2_QML_PROPERTY_READONLY(AgentM*, agentModel)

    // Number of each type of IngeScape applications
    I2_QML_PROPERTY_READONLY(int, numberOfPeersOfAgents)
    I2_QML_PROPERTY_READONLY(int, numberOfLaunchers)
    I2_QML_PROPERTY_READONLY(int, numberOfRecorders)
    I2_QML_PROPERTY_READONLY(int, numberOfEditors)
    I2_QML_PROPERTY_READONLY(int, numberOfAssessments)
    I2_QML_PROPERTY_READONLY(int, numberOfExpes)


public:

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    static IngeScapeNetworkController* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
     * @brief Constructor
     * @param igsServiceDescription
     * @param parent
     */
    explicit IngeScapeNetworkController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~IngeScapeNetworkController();


    /**
     * @brief Start our IngeScape agent with a network device (or an IP address) and a port
     * @param networkDevice
     * @param ipAddress
     * @param port
     * @return
     */
    Q_INVOKABLE bool start(QString networkDevice, QString ipAddress, uint port);


    /**
     * @brief Stop our IngeScape agent
     */
    Q_INVOKABLE void stop();


    /**
     * @brief Restart our ingescape agent (stop + start with arguments used in the last call to start)
     *
     * @return true if our agent has restarted
     */
    bool restart();


    /**
     * @brief start with previous configuration
     * @return
     */
    bool startWithPreviousConfiguration();


    /**
     * @brief Start monitoring
     *
     * @param expectedNetworkDevice
     * @param expectedPort
     */
    void startMonitoring(QString expectedNetworkDevice = "", uint expectedPort = 31520);


    /**
     * @brief Stop monitoring
     */
    void stopMonitoring();


    PeerM* getPeerWithId(QString peerId);


    /**
     * @brief Create a peer which entered the network
     */
    PeerM* createEnteredPeer(IngeScapeTypes::Value igsType,
                             QString peerId,
                             QString peerName,
                             QString ipAddress,
                             QString hostname,
                             QString commandLine);
    void removeExitedPeerId(QString peerId);


    /**
     * @brief Update the list of available network devices
     */
    Q_INVOKABLE void updateAvailableNetworkDevices();


    /**
     * @brief Return true if the network device is available
     * @param networkDevice
     * @return
     */
    Q_INVOKABLE bool isAvailableNetworkDevice(QString networkDevice);


    /**
     * @brief Manage a "Shouted" message
     */
    void manageShoutedMessage(PeerM* peer, zmsg_t* zMessage);


    /**
     * @brief Manage a "Whispered" message
     */
    void manageWhisperedMessage(PeerM* peer, zmsg_t* zMessage);


    /**
     * @brief Send a string message to an agent (identified by its peer id)
     * @param agentId peer id of the agent
     * @param message 1 string
     * @return true if successful, false otherwise
     */
    bool sendStringMessageToAgent(QString agentId, QString message);


    /**
     * @brief Send a strings list message to an agent (identified by its peer id)
     * @param agentId peer id of the agent
     * @param message list of strings
     * @return true if successful, false otherwise
     */
    bool sendStringMessageToAgent(QString agentId, QStringList message);


    /**
     * @brief Send a string message to a list of agents (identified by their peer id)
     * @param agentIds peer ids of the agents
     * @param message 1 string
     * @return true if successful, false otherwise
     */
    bool sendStringMessageToAgents(QStringList agentIds, QString message);


    /**
     * @brief Send a strings list message to an agent (identified by its peer id)
     * @param agentIds peer ids of the agents
     * @param message list of strings
     * @return true if successful, false otherwise
     */
    bool sendStringMessageToAgents(QStringList agentIds, QStringList message);


    /**
     * @brief Send a ZMQ message in several parts to an agent (identified by its peer id)
     * @param agentId
     * @param messageParts
     * @return
     */
    bool sendZMQMessageToAgent(QString agentId, QStringList messageParts);


    /**
     * @brief Add inputs to our application for a list of agent outputs
     * @param agentName
     * @param newOutputsIds
     */
    void addInputsToOurApplicationForAgentOutputs(QString agentName, QStringList newOutputsIds);


    /**
     * @brief Remove inputs from our application for a list of agent outputs
     * @param agentName
     * @param oldOutputsIds
     */
    void removeInputsFromOurApplicationForAgentOutputs(QString agentName, QStringList oldOutputsIds);


Q_SIGNALS:
    /**
     * @brief Triggered when our network device is not available
     */
    void networkDeviceIsNotAvailable();


    /**
     * @brief Triggered when our network device is available again
     */
    void networkDeviceIsAvailableAgain();


    /**
     * @brief Triggered when our network device has a new IP address
     * @param newIpAddress
     */
    void networkDeviceIpAddressHasChanged(QString newIpAddress);


    // Signal emitted when an "IngeScape Agent" enter/quit the network
    void peerOfAgentsEntered(PeerM* peer);
    void peerOfAgentsExited(PeerM* peer);
    void agentExited(PeerM* peer, QString agentUid);


    // Signal emitted when an "IngeScape Launcher" enter/quit the network
    void launcherEntered(PeerM* peer);
    void launcherExited(PeerM* peer);


    // Signal emitted when an "IngeScape Editor" enter/quit the network
    void editorEntered(PeerM* peer);
    void editorExited(PeerM* peer);


    // Signal emitted when an "IngeScape Recorder" enter/quit the network
    void recorderEntered(PeerM* peer);
    void recorderExited(PeerM* peer);


    // Signal emitted when an "IngeScape Expe" enter/quit the network
    void expeEntered(PeerM* peer);
    void expeExited(PeerM* peer);


    // Signal emitted when an "IngeScape Assessments" enter/quit the network
    void assessmentsEntered(PeerM* peer);
    void assessmentsExited(PeerM* peer);


    /**
     * @brief Signal emitted when an agent definition has been received
     */
    void definitionReceived(PeerM* peer, QString agentUid, QString agentName, QString definitionJSON);


    /**
     * @brief Signal emitted when an agent mapping has been received
     */
    void mappingReceived(PeerM* peer, QString agentUid, QString mappingJSON);


    /**
     * @brief Signal emitted when a new value has been published
     * @param publishedValue
     */
    void valuePublished(PublishedValueM* publishedValue);


    /**
     * @brief Signal emitted when a "Shouted" message (with one part) has been received
     * @param peerId
     * @param peerName
     * @param message
     */
    void shoutedMessageReceived(QString peerId, QString peerName, QString message);


    /**
     * @brief Signal emitted when a "Shouted" message (with several parts) has been received
     * @param peerId
     * @param peerName
     * @param messagePart1
     * @param messageOthersParts
     */
    void shoutedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts);


    /**
     * @brief Signal emitted when "Whispered" message (with one part) has been received
     * @param peerId
     * @param peerName
     * @param message
     */
    void whisperedMessageReceived(QString peerId, QString peerName, QString message);


    /**
     * @brief Signal emitted when "Whispered" message (with several parts) has been received
     * @param peerId
     * @param peerName
     * @param messagePart1
     * @param messageOthersParts
     */
    void whisperedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts);


private:
    // Name of our "IngeScape" agent that correspond to our application
    QString _igsAgentApplicationName;

    // Hash table from an uid to a peer on the network
    QHash<QString, PeerM*> _hashFromUidToPeer;

    // Last agruments of start
    QString _lastArgumentsOfStart_networkDevice;
    QString _lastArgumentsOfStart_ipAddress;
    uint _lastArgumentsOfStart_port;
};

QML_DECLARE_TYPE(IngeScapeNetworkController)

#endif // INGESCAPE_NETWORKCONTROLLER_H
