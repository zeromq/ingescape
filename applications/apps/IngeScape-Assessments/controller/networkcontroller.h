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

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QObject>

#include "I2PropertyHelpers.h"
#include <controller/ingescapenetworkcontroller.h>


/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController : public QObject
{
    Q_OBJECT


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit NetworkController(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~NetworkController();


Q_SIGNALS:

    /**
     * @brief Signal emitted when the "Recorder app" started to record
     */
    void recordStartedReceived(QString peerId);

    /**
     * @brief Signal emitted when the "Recorder app" stopped to record
     */
    void recordStoppedReceived(QString peerId);

    /**
     * @brief Signal emitted when the "Recorder app" added to record
     */
    void addedRecordReceived(QString message);

    /**
     * @brief Signal emitted when the "Recorder app" deleted a record
     */
    void deletedRecordReceived(QString message);


public Q_SLOTS:


private Q_SLOTS:

    // FIXME error: invalid application of 'sizeof' to an incomplete type '_zmsg_t'
    // Q_STATIC_ASSERT_X(sizeof(T), "Type argument of Q_DECLARE_METATYPE(T*) must be fully defined");
    //void _onShoutedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);
    //void _onWhisperedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);


    /**
     * @brief Slot called when a "Shouted" message (with one part) has been received
     * @param peerId
     * @param peerName
     * @param message
     */
    void _onShoutedMessageReceived(QString peerId, QString peerName, QString message);


    /**
     * @brief Slot called when a "Shouted" message (with several parts) has been received
     * @param peerId
     * @param peerName
     * @param messagePart1
     * @param messageOthersParts
     */
    void _onShoutedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts);


    /**
     * @brief Slot called when "Whispered" message (with one part) has been received
     * @param peerId
     * @param peerName
     * @param message
     */
    void _onWhisperedMessageReceived(QString peerId, QString peerName, QString message);


    /**
     * @brief Slot called when "Whispered" message (with several parts) has been received
     * @param peerId
     * @param peerName
     * @param messagePart1
     * @param messageOthersParts
     */
    void _onWhisperedMessageReceived(QString peerId, QString peerName, QString messagePart1, QStringList messageOthersParts);

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
