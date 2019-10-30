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

#ifndef HOSTSSUPERVISIONCONTROLLER_H
#define HOSTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <controller/editormodelmanager.h>
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
     * @param modelManager
     * @param parent
     */
    explicit HostsSupervisionController(EditorModelManager* modelManager,
                                        QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostsSupervisionController();


    /**
     * @brief Remove a model of agent from a host
     * @param agent
     * @param host
     */
    Q_INVOKABLE void removeAgentModelFromHost(AgentM* agent, HostVM* host);


    /**
     * @brief Delete hosts OFF
     */
    void deleteHostsOFF();


    /**
     * @brief Delete a view model of host
     * @param host
     */
    Q_INVOKABLE void deleteHost(HostVM* host);


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
    void onHostModelHasBeenCreated(HostM* host);


    /**
     * @brief Slot called when a model of host will be deleted
     * @param host
     */
    void onHostModelWillBeDeleted(HostM* host);


    /**
     * @brief Slot called when a previous host has been parsed (in JSON file)
     * @param hostName
     */
    void onPreviousHostParsed(QString hostName);


    /**
     * @brief Slot called when a new model of agent has been created
     * @param agent
     */
    void onAgentModelHasBeenCreated(AgentM* agent);


    /**
     * @brief Slot called when a model of agent will be deleted
     * @param agent
     */
    void onAgentModelWillBeDeleted(AgentM* agent);


private Q_SLOTS:

    /**
     * @brief Slot called when the name of a view model of host changed
     * @param newName
     */
    //void _onHostNameChanged(QString newName);


private:

    /**
     * @brief Get the view model of host with a name
     * @param hostName
     * @return
     */
    HostVM* _getHostWithName(QString hostName);


    /**
     * @brief Create a view model of host with a name and a model (optional)
     * @param hostName
     * @param model
     * @return
     */
    HostVM* _createViewModelOfHost(QString hostName, HostM* model);


private:

    // Manager for the data model of IngeScape
    EditorModelManager* _modelManager;

    // Hash table from "(host)Name" to the "(view model of) Host"
    QHash<QString, HostVM*> _hashFromNameToHost;

    // List of all agents
    QList<AgentM*> _allAgents;

};

QML_DECLARE_TYPE(HostsSupervisionController)

#endif // HOSTSSUPERVISIONCONTROLLER_H
