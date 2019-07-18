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
class NetworkController : public IngeScapeNetworkController
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
    ~NetworkController() Q_DECL_OVERRIDE;


    /**
     * @brief Manage a "Shouted" message
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    //void manageShoutedMessage(QString peerId, QString peerName, zmsg_t* zMessage) Q_DECL_OVERRIDE;


    /**
     * @brief Manage a "Whispered" message
     * @param peerId
     * @param peerName
     * @param zMessage
     */
    void manageWhisperedMessage(QString peerId, QString peerName, zmsg_t* zMessage) Q_DECL_OVERRIDE;


    /**
     * @brief Send a command and parameters to the editor
     * @param peerIdOfEditor
     * @param commandAndParameters
     */
    void sendCommandToEditor(QString peerIdOfEditor, QString commandAndParameters);


Q_SIGNALS:

    /**
     * @brief Signal emitted when an editor reply to our command "Load Platform File" with a status
     * @param commandStatus
     * @param commandParameters
     */
    void statusReceivedAbout_LoadPlatformFile(bool commandStatus, QString commandParameters);


    /**
     * @brief Signal emitted when the state of the TimeLine updated (in Editor app)
     * @param state
     */
    void timeLineStateUpdated(QString state);


public Q_SLOTS:


private:


};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
