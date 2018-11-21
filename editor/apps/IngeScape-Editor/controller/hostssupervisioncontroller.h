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
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *
 */

#ifndef HOSTSSUPERVISIONCONTROLLER_H
#define HOSTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/hostvm.h>


/**
 * @brief The HostsSupervisionController class defines the controller for hosts supervision
 */
class HostsSupervisionController : public QObject
{
    Q_OBJECT

    // Sorted list of hosts
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(HostVM, hostsList)

    // Selected host in the hosts list
    I2_QML_PROPERTY_DELETE_PROOF(HostVM*, selectedHost)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit HostsSupervisionController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostsSupervisionController();


    /**
     * @brief Get the peer id of a Launcher on a HostName
     * @param hostName
     * @return
     */
    QString getPeerIdOfLauncherOnHostName(QString hostName);


    /**
     * @brief Remove each UN-active agent (agent with state OFF) from the global list with all agents
     */
    void removeUNactiveAgents();


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


public Q_SLOTS:

    /**
     * @brief Slot called when a new model of host has been created
     * @param host
     */
    void onHostModelCreated(HostM* host);


    /**
     * @brief Slot called when a model of host will be removed
     * @param host
     */
    void onHostModelWillBeRemoved(HostM* host);


    /**
     * @brief Slot called when a new model of agent has been created
     * @param agent
     */
    void onAgentModelCreated(AgentM* agent);


    /**
     * @brief Slot called when a model of agent will be deleted
     * @param agent
     */
    void onAgentModelWillBeDeleted(AgentM* agent);


private Q_SLOTS:

    /**
     * @brief Slot called when the network data (of an agent) will be cleared
     * @param peerId
     */
    void _onNetworkDataOfAgentWillBeCleared(QString peerId);


private:

    /**
     * @brief Get the view model of host with a name
     * @param hostName
     * @return
     */
    HostVM* _getHostWithName(QString hostName);


private:

    // Hash table from "(host)Name" to the "(view model of) Host"
    QHash<QString, HostVM*> _hashFromNameToHost;

    // List of all agents
    QList<AgentM*> _allAgents;

};

QML_DECLARE_TYPE(HostsSupervisionController)

#endif // HOSTSSUPERVISIONCONTROLLER_H
