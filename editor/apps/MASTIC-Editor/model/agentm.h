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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#ifndef AGENTM_H
#define AGENTM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>


/**
 * @brief The AgentM class defines a model of agent
 */
class AgentM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY(QString, name)

    // Peer ID of our agent (unique identifier)
    I2_QML_PROPERTY_READONLY(QString, peerId)

    // Network device of our agent
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // IP address of our agent
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Port of our agent
    I2_QML_PROPERTY_READONLY(int, port)

    // HostName of our agent
    I2_QML_PROPERTY_READONLY(QString, hostname)

    // Execution path of our agent
    I2_QML_PROPERTY_READONLY(QString, executionPath)

    // PID of our agent ???
    //I2_CPP_NOSIGNAL_PROPERTY(int, pid)

    // Publisher of our agent ???
    //I2_CPP_NOSIGNAL_PROPERTY(int, publisher)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Md5 hash value for the definition string
    //I2_QML_PROPERTY_READONLY(QString, md5Hash)


public:
    /**
     * @brief Constructor
     * @param name
     * @param peerId
     * @param parent
     */
    explicit AgentM(QString name,
                    QString peerId = "",
                    QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentM)

#endif // AGENTM_H
