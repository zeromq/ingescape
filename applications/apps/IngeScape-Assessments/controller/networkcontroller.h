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
    //void recordStartedReceived();


    /**
     * @brief Signal emitted when the "Recorder app" stopped to record
     */
    //void recordStoppedReceived();


public Q_SLOTS:


private:

    /**
     * @brief Slot called when a "Shouted" message has been received
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    void _onShoutedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);


    /**
     * @brief Slot called when "Whispered" message has been received
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    void _onWhisperedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);


};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
