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

#include <model/definitionm.h>
#include <model/agentmappingm.h>


/**
 * @brief The AgentM class defines a model of agent
 */
class AgentM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY(QString, name)

    // Peer ID of our agent (unique identifier)
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerId)

    // Flag indicating if our agent has never yet appeared on the network
    // It has only a definition (and evetually a mapping)
    I2_QML_PROPERTY_READONLY(bool, hasNeverAppearedOnNetwork)

    // Address of our agent
    I2_QML_PROPERTY_READONLY(QString, address)

    // Network device of our agent
    //I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // IP address of our agent
    //I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Port of our agent
    //I2_QML_PROPERTY_READONLY(int, port)

    // HostName of our agent
    I2_QML_PROPERTY_READONLY(QString, hostname)

    // Execution path of our agent
    I2_CPP_NOSIGNAL_PROPERTY(QString, executionPath)

    // Process Id of our agent
    I2_CPP_NOSIGNAL_PROPERTY(int, pid)

    // Publisher of our agent ???
    //I2_CPP_NOSIGNAL_PROPERTY(int, publisher)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Status defined by the agent
    //I2_QML_PROPERTY_READONLY(QString, status)

    // Definition of our agent
    I2_QML_PROPERTY_READONLY(DefinitionM*, definition)

    // Mapping of our agent
    I2_QML_PROPERTY_READONLY(AgentMappingM*, mapping)


public:
    /**
     * @brief Constructor without peer id and address
     * @param name
     * @param parent
     */
    explicit AgentM(QString name,
                    QObject *parent = nullptr);


    /**
     * @brief Constructor with peer id and address
     * @param name
     * @param peerId
     * @param address
     * @param parent
     */
    AgentM(QString name,
           QString peerId,
           QString address,
           QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentM();


    /**
     * @brief Set the flag "is Muted" of an Output of our agent
     * @param isMuted
     * @param outputName
     */
    void setisMutedOfOutput(bool isMuted, QString outputName);


Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentM)

#endif // AGENTM_H
