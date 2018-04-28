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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *
 */

#ifndef HOSTVM_H
#define HOSTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include "model/hostm.h"
#include "model/agentm.h"

/**
 * @brief The HostVM class defines a view model of host in the supervision list
 */
class HostVM : public QObject
{
    Q_OBJECT

    // Model of our host
    I2_QML_PROPERTY_DELETE_PROOF(HostM*, hostModel)

    // List of associated Agents
    I2_QOBJECT_LISTMODEL(AgentM, listOfAgents)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canProvideStream)

    // Falg indicating if the host is streaming or not
    I2_QML_PROPERTY(bool, isStreaming)

public:

    /**
     * @brief HostVM Default constructor
     * @param parent
     */
    explicit HostVM(HostM* model, QObject *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~HostVM();

    /**
     * @brief Change the state of our host
     */
    Q_INVOKABLE void changeState();

Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToHost(QString command, QString hostname, QString commandLine);

};

QML_DECLARE_TYPE(HostVM)

#endif // HOSTVM_H
