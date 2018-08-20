/*
 *	IngeScape Editor
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

#ifndef AGENTEXECUTIONONHOSTM_H
#define AGENTEXECUTIONONHOSTM_H

#include <QObject>

#include <I2PropertyHelpers.h>


/**
 * @brief The AgentExecutionOnHostM class defines a model of agent execution on a host
 */
class AgentExecutionOnHostM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // Peer ID of our agent (unique identifier)
    //I2_CPP_NOSIGNAL_PROPERTY(QString, peerId)

    // Hostname of our agent
    I2_CPP_NOSIGNAL_PROPERTY(QString, hostname)

    // Command line of our agent
    I2_CPP_NOSIGNAL_PROPERTY(QString, commandLine)


public:
    /**
     * @brief Constructor
     * @param agentName
     * @param hostname
     * @param commandLine
     * @param parent
     */
    explicit AgentExecutionOnHostM(QString agentName,
                                   QString hostname,
                                   QString commandLine,
                                   QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentExecutionOnHostM();

signals:

public slots:

};

QML_DECLARE_TYPE(AgentExecutionOnHostM)

#endif // AGENTEXECUTIONONHOSTM_H
