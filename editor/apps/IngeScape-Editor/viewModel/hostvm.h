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
#include <sortFilter/agentsinhostsortfilter.h>


/**
 * @brief The HostVM class defines a view model of host in the supervision list
 */
class HostVM : public QObject
{
    Q_OBJECT

    // Name of our host
    I2_QML_PROPERTY_READONLY(QString, name)

    // Model of our host
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(HostM*, modelM)

    // Flag indicating if our host is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // (NOT sorted) list of agents executed on our host
    I2_QOBJECT_LISTMODEL(AgentM, agentsList)

    // List of (filtered and) sorted agents executed on our host
    Q_PROPERTY(AgentsInHostSortFilter* sortedAgents READ sortedAgents CONSTANT)

    // Flag indicating if our host can provide stream
    I2_QML_PROPERTY_READONLY(bool, canProvideStream)

    // Flag indicating if the host is streaming or not
    I2_QML_PROPERTY(bool, isStreaming)


public:

    /**
     * @brief Constructor
     * @param name
     * @param model
     * @param parent
     */
    explicit HostVM(QString name,
                    HostM* model = nullptr,
                    QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostVM();


    /**
     * @brief Get our sorted list of agents
     * @return
     */
    AgentsInHostSortFilter* sortedAgents()
    {
        return &_sortedAgents;
    }


    /**
     * @brief Change the stream state of our host
     */
    Q_INVOKABLE void changeStreamState();


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


private:
    // List of (filtered and) sorted agents
    AgentsInHostSortFilter _sortedAgents;

};

QML_DECLARE_TYPE(HostVM)

#endif // HOSTVM_H
