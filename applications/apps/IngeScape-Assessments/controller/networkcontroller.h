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

    void exportedRecordReceived();


public Q_SLOTS:


private Q_SLOTS:

    // Slot called when a "Shouted" message has been received
    void _onShoutedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters);


    // Slot called when a "Whispered" message has been received
    void _onWhisperedMessageReceived(PeerM* peer, QString messageType, QStringList messageParameters);

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
