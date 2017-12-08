/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef HOSTM_H
#define HOSTM_H

#include <QObject>
#include <I2PropertyHelpers.h>

/**
 * @brief The HostM class defines a model of host
 */
class HostM : public QObject
{
    Q_OBJECT

    // Name of our host
    I2_QML_PROPERTY_READONLY(QString, name)

    // Peer ID of our host (unique identifier)
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerId)

    // IP address of our host
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Flag indicating if our host is ON (vs OFF)
    //I2_QML_PROPERTY_READONLY(bool, isON)


public:
    /**
     * @brief Constructor
     * @param name
     * @param peerId
     * @param ipAddress
     * @param parent
     */
    explicit HostM(QString name,
                   QString peerId,
                   QString ipAddress,
                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostM();

};

QML_DECLARE_TYPE(HostM)

#endif // HOSTM_H
