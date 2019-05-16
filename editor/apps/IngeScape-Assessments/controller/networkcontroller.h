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


/**
 * @brief The NetworkController class defines the controller for network communications
 */
class NetworkController : public QObject
{
    Q_OBJECT

    // List of available network devices
    I2_QML_PROPERTY_READONLY(QStringList, availableNetworkDevices)


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
     * @brief Update the list of available network devices
     */
    Q_INVOKABLE void updateAvailableNetworkDevices();


Q_SIGNALS:

public Q_SLOTS:


private:

    // Name of our agent "IngeScape Assessments"
    QString _assessmentsAgentName;

    // Our IngeScape agent is successfully started if the result of igs_startWithDevice / igs_startWithIP is 1 (O otherwise)
    int _isIngeScapeAgentStarted;

    // Hash table from a peer id to a type of IngeScape elements on the network
    //QHash<QString, IngeScapeTypes::Value> _hashFromPeerIdToIngeScapeType;

};

QML_DECLARE_TYPE(NetworkController)

#endif // NETWORKCONTROLLER_H
