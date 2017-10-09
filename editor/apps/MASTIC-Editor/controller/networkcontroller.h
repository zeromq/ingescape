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

#include "model/agentm.h"

/**
 * @brief The NetworkController class defines the main controller of our application
 */
class NetworkController: public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Default constructor
     * @param network devece
     * @param ip address
     * @param port number
     * @param parent
     */
    explicit NetworkController(QString networkDevice, QString ipAddress, int port, QObject *parent = 0);


    /**
      * @brief Destructor
      */
    ~NetworkController();

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
     * @param definition
     */
    void definitionReceived(QString peerId, QString peerName, QString definition);


    /**
     * @brief Signal emitted when an agent quit the network
     * @param peer id
     * @param peer name
     */
    void agentExited(QString peerId, QString peerName);


public Q_SLOTS:


protected:



};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
