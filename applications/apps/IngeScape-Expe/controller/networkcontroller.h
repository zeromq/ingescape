/*
 *	IngeScape Expe
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
     * @brief Signal emitted when an editor reply to our command "Load Platform File" with a status
     * @param commandStatus
     * @param commandParameters
     */
    void statusReceivedAbout_LoadPlatformFile(bool commandStatus, QString commandParameters);


    /**
     * @brief Signal emitted when the state of the TimeLine updated (in Editor app)
     * @param parameters
     */
    void timeLineStateUpdated(QString parameters);


public Q_SLOTS:


private Q_SLOTS:

    // FIXME error: invalid application of 'sizeof' to an incomplete type '_zmsg_t'
    // Q_STATIC_ASSERT_X(sizeof(T), "Type argument of Q_DECLARE_METATYPE(T*) must be fully defined");
    //void _onShoutedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);
    //void _onWhisperedMessageReceived(QString peerId, QString peerName, zmsg_t* zMessage);


    /**
     * @brief Slot called when "Whispered" message (with one part) has been received
     * @param peerId
     * @param peerName
     * @param message
     */
    void _onWhisperedMessageReceived(QString peerId, QString peerName, QString message);


};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
