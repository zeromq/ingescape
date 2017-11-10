/*
 *	MASTIC Editor
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
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

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include "I2PropertyHelpers.h"

#include <model/iop/outputm.h>
#include <model/publishedvaluem.h>

/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController: public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit NetworkController(QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~NetworkController();


    /**
     * @brief Start our MASTIC agent with a network device (or an IP address) and a port
     * @param networkDevice
     * @param ipAddress
     * @param port
     */
    void start(QString networkDevice, QString ipAddress, int port);


    /**
     * @brief Called when a MASTIC Launcher enter the network
     * @param hostname
     * @param peerId
     */
    void masticLauncherEntered(QString hostname, QString peerId);


    /**
     * @brief Called when a MASTIC Launcher exit the network
     * @param hostname
     */
    void masticLauncherExited(QString hostname);


Q_SIGNALS:

    /**
     * @brief Signal emitted when an agent enter the network
     * @param peerId
     * @param peerName
     * @param peerAddress
     * @param pid
     * @param hostname
     * @param executionPath
     * @param canBeFrozen
     */
    void agentEntered(QString peerId, QString peerName, QString peerAddress, int pid, QString hostname, QString executionPath, bool canBeFrozen);


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
     * @brief Signal emitted when an agent quit the network
     * @param peer id
     * @param peer name
     */
    void agentExited(QString peerId, QString peerName);


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


public Q_SLOTS:

    /**
     * @brief Slot when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param executionPath
     */
    void onCommandAskedToLauncher(QString command, QString hostname, QString executionPath);


    /**
     * @brief Slot when a command must be sent on the network
     * @param command
     * @param peerIdsList
     */
    void onCommandAsked(QString command, QStringList peerIdsList);


    /**
     * @brief Slot when a command for an output must be sent on the network
     * @param command
     * @param outputName
     * @param peerIdsList
     */
    void onCommandAskedForOutput(QString command, QString outputName, QStringList peerIdsList);


    /**
     * @brief Slot when inputs must be added to our Editor for a list of outputs
     * @param agentName
     * @param outputsList
     */
    void onAddInputsToEditorForOutputs(QString agentName, QList<OutputM*> outputsList);


    /**
     * @brief Slot when inputs must be removed to our Editor for a list of outputs
     * @param agentName
     * @param pairsList
     */
    void onRemoveInputsToEditorForOutputs(QString agentName, QList<QPair<QString, QString>> pairsList);


private:

    // Name of our agent "MASTIC Editor"
    QString _editorAgentName;

    // Our Mastic agent is successfully started if the result of mtic_startWithDevice / mtic_startWithIP is 1 (O otherwise)
    int _isMasticAgentStarted;

    // Map from "Hostname" to the "Peer Id" of the corresponding MASTIC launcher
    QHash<QString, QString> _mapFromHostnameToMasticLauncherPeerId;

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
