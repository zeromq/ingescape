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
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef HOSTVM_H
#define HOSTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <model/hostm.h>
#include <model/agent/agentm.h>


/**
 * @brief The HostVM class defines a view model of host in the supervision list
 */
class HostVM : public QObject
{
    Q_OBJECT

    // Name of our host
    I2_QML_PROPERTY_READONLY(QString, name)

    // Model of our host
    I2_QML_PROPERTY_READONLY(HostM*, modelM)

    // Sorted list of associated agents
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentM, agentsList)

    // Flag indicating if our host can provide stream
    I2_QML_PROPERTY_READONLY(bool, canProvideStream)

    // Flag indicating if the host is streaming or not
    I2_QML_PROPERTY(bool, isStreaming)


public:

    /**
     * @brief Constructor
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


    /**
     * @brief Start an agent
     * @param agent
     */
    Q_INVOKABLE void startAgent(AgentM* agent);


    /**
     * @brief Stop an agent
     * @param agent
     */
    Q_INVOKABLE void stopAgent(AgentM* agent);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param peerIdOfLauncher
     * @param command
     * @param commandLine
     */
    void commandAskedToLauncher(QString peerIdOfLauncher, QString command, QString commandLine);

};

QML_DECLARE_TYPE(HostVM)

#endif // HOSTVM_H
