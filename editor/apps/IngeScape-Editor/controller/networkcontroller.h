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

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"
#include <model/agent/agentm.h>
#include <model/publishedvaluem.h>


/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController: public QObject
{
    Q_OBJECT

    // Model of our agent "IngeScape Editor"
    I2_QML_PROPERTY_READONLY(AgentM*, agentEditor)

    // List of available network devices
    I2_QML_PROPERTY_READONLY(QStringList, availableNetworkDevices)


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit NetworkController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~NetworkController();


    /**
     * @brief Start our INGESCAPE agent with a network device (or an IP address) and a port
     * @param networkDevice
     * @param ipAddress
     * @param port
     * @return
     */
    bool start(QString networkDevice, QString ipAddress, uint port);


    /**
     * @brief Stop our INGESCAPE agent
     */
    void stop();


    /**
     * @brief Get the IngeScape type of a peer id
     * @param peerId
     * @return
     */
    IngeScapeTypes::Value getIngeScapeTypeOfPeerId(QString peerId);


    /**
     * @brief Manage a peer id which entered the network
     * @param peerId
     * @param ingeScapeType
     */
    void manageEnteredPeerId(QString peerId, IngeScapeTypes::Value ingeScapeType);


    /**
     * @brief Manage a peer id which exited the network
     * @param peerId
     */
    void manageExitedPeerId(QString peerId);


    /**
     * @brief Manage the message "MUTED / UN-MUTED"
     * @param peerId
     * @param message
     */
    void manageMessageMutedUnmuted(QString peerId, QString message);


    /**
     * @brief Manage the message "CAN BE FROZEN / CAN NOT BE FROZEN"
     * @param peerId
     * @param message
     */
    void manageMessageCanBeFrozenOrNot(QString peerId, QString message);


    /**
     * @brief Manage the message "FROZEN / UN-FROZEN"
     * @param peerId
     * @param message
     */
    void manageMessageFrozenUnfrozen(QString peerId, QString message);


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
     * @brief Send a command, parameters and the content of a JSON file to the recorder
     * @param peerIdOfRecorder
     * @param commandAndParameters
     */
    void sendCommandWithJsonToRecorder(QString peerIdOfRecorder, QStringList commandAndParameters);


Q_SIGNALS:

    /**
     * @brief Signal emitted when an agent enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     * @param commandLine
     * @param canBeFrozen
     * @param loggerPort
     */
    void agentEntered(QString peerId, QString peerName, QString ipAddress, QString hostname, QString commandLine, bool canBeFrozen, QString loggerPort);


    /**
     * @brief Signal emitted when an agent quit the network
     * @param peer id
     * @param peer name
     */
    void agentExited(QString peerId, QString peerName);


    /**
     * @brief Signal emitted when a launcher enter the network
     * @param peerId
     * @param hostname
     * @param ipAddress
     */
    void launcherEntered(QString peerId, QString hostname, QString ipAddress, QString streamingPort);


    /**
     * @brief Signal emitted when a launcher quit the network
     * @param peerId
     * @param hostname
     */
    void launcherExited(QString peerId, QString hostname);


    /**
     * @brief Signal emitted when a recorder enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void recorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Signal emitted when a recorder quit the network
     * @param peerId
     * @param peerName
     */
    void recorderExited(QString peerId, QString peerName);


    /**
     * @brief Signal emitted when an agent definition has been received
     * @param peer id
     * @param peer name
     * @param definitionJSON
     */
    void definitionReceived(QString peerId, QString peerName, QString definitionJSON);


    /**
     * @brief Signal emitted when an agent mapping has been received
     * @param peerId
     * @param peerName
     * @param mappingJSON
     */
    void mappingReceived(QString peerId, QString peerName, QString mappingJSON);


    /**
     * @brief Signal emitted when all records from DB have been received
     * @param recordsJSON
     */
    void allRecordsReceived(QString recordsJSON);


    /**
     * @brief Signal emitted when a new record has been added
     * @param recordJSON
     */
    void addedRecordReceived(QString recordJSON);


    /**
     * @brief Signal emitted when a record has been deleted
     * @param recordId
     */
    void deletedRecordReceived(QString recordId);


    /**
     * @brief Signal emitted when a record is loading
     * @param deltaTimeFromTimeLine
     * @param jsonPlatform
     * @param jsonExecutedActions
     */
    void loadingRecordReceived(int deltaTimeFromTimeLine, QString jsonPlatform, QString jsonExecutedActions);


    /**
     * @brief Signal emitted when a record has been loaded
     */
    void loadedRecordReceived();


    /**
     * @brief Signal emitted when a record playing has ended
     */
    void endOfRecordReceived();


    /**
     * @brief Signal emitted when a new value is published
     * @param publishedValue
     */
    void valuePublished(PublishedValueM* publishedValue);


    /**
     * @brief Signal emitted when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void isMutedFromAgentUpdated(QString peerId, bool isMuted);


    /**
     * @brief Signal emitted when the flag "can be Frozen" from an agent updated
     * @param peerId
     * @param canBeFrozen
     */
    void canBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen);


    /**
     * @brief Signal emitted when the flag "is Frozen" from an agent updated
     * @param peerId
     * @param isFrozen
     */
    void isFrozenFromAgentUpdated(QString peerId, bool isFrozen);


    /**
     * @brief Signal emitted when the flag "is Muted" from an output of agent updated
     * @param peerId
     * @param isMuted
     * @param outputName
     */
    void isMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName);


    /**
     * @brief Signal emitted when the state of an agent changes
     * @param peerId
     * @param stateName
     */
    void agentStateChanged(QString peerId, QString stateName);


    /**
     * @brief Signal emitted when we receive the flag "Log In Stream" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void agentHasLogInStream(QString peerId, bool hasLogInStream);


    /**
     * @brief Signal emitted when we receive the flag "Log In File" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void agentHasLogInFile(QString peerId, bool hasLogInFile);


    /**
     * @brief Signal emitted when we receive the path of "Log File" for an agent
     * @param peerId
     * @param logFilePath
     */
    void agentLogFilePath(QString peerId, QString logFilePath);


    /**
     * @brief Signal emitted when we receive the path of "Definition File" for an agent
     * @param peerId
     * @param definitionFilePath
     */
    void agentDefinitionFilePath(QString peerId, QString definitionFilePath);


    /**
     * @brief Signal emitted when we receive the path of "Mapping File" for an agent
     * @param peerId
     * @param mappingFilePath
     */
    void agentMappingFilePath(QString peerId, QString mappingFilePath);


    /**
     * @brief Signal emitted when we receive the command "highlight link" from a recorder
     * @param parameters
     */
    void highlightLink(QStringList parameters);


    /**
     * @brief Signal emitted when we receive the command "run action" from a recorder
     * @param actionID
     */
    void runAction(QString actionID);


public Q_SLOTS:

    /**
     * @brief Slot called when a command must be sent on the network to a launcher
     * @param peerIdOfLauncher
     * @param command
     * @param commandLine
     */
    void onCommandAskedToLauncher(QString peerIdOfLauncher, QString command, QString commandLine);


    /**
     * @brief Slot called when a command must be sent on the network to a recorder
     * @param peerIdOfRecorder
     * @param commandAndParameters
     */
    void onCommandAskedToRecorder(QString peerIdOfRecorder, QString commandAndParameters);


    /**
     * @brief Slot called when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void onCommandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about one of its output
     * @param peerIdsList
     * @param command
     * @param outputName
     */
    void onCommandAskedToAgentAboutOutput(QStringList peerIdsList, QString command, QString outputName);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about setting a value to one of its Input/Output/Parameter
     * @param peerIdsList
     * @param command
     * @param agentIOPName
     * @param value
     */
    void onCommandAskedToAgentAboutSettingValue(QStringList peerIdsList, QString command, QString agentIOPName, QString value);


    /**
     * @brief Slot called when a command must be sent on the network to an agent about mapping one of its input
     * @param peerIdsList
     * @param command
     * @param inputName
     * @param outputAgentName
     * @param outputName
     */
    void onCommandAskedToAgentAboutMappingInput(QStringList peerIdsList, QString command, QString inputName, QString outputAgentName, QString outputName);


    /**
     * @brief Slot called when the flag "is Mapping Activated" changed
     * @param isMappingActivated
     */
    void onIsMappingActivatedChanged(bool isMappingActivated);


    /**
     * @brief Slot called when inputs must be added to our Editor for a list of outputs
     * @param agentName
     * @param newOutputsIds
     * @param isMappingActivated
     */
    void onAddInputsToEditorForOutputs(QString agentName, QStringList newOutputsIds, bool isMappingActivated);


    /**
     * @brief Slot called when inputs must be removed from our Editor for a list of outputs
     * @param agentName
     * @param oldOutputsIds
     * @param isMappingActivated
     */
    void onRemoveInputsToEditorForOutputs(QString agentName, QStringList oldOutputsIds, bool isMappingActivated);


private:

    // Name of our agent "IngeScape Editor"
    QString _editorAgentName;

    // Our IngeScape agent is successfully started if the result of igs_startWithDevice / igs_startWithIP is 1 (O otherwise)
    int _isIngeScapeAgentStarted;

    // Hash table from a peer id to a type of IngeScape elements on the network
    QHash<QString, IngeScapeTypes::Value> _hashFromPeerIdToIngeScapeType;

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
