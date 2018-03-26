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

#ifndef HOSTSSUPERVISIONCONTROLLER_H
#define HOSTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/masticlaunchermanager.h>
#include <viewModel/hostvm.h>


/**
 * @brief The HostsSupervisionController class defines the controller for hosts supervision
 */
class HostsSupervisionController : public QObject
{
    Q_OBJECT

    // Sorted list of host
    I2_QOBJECT_LISTMODEL(HostVM, hostsList)

    // Selected host in the hosts list
    I2_QML_PROPERTY_DELETE_PROOF(HostVM*, selectedHost)


public:
    /**
     * @brief Default constructor
     * @param modelManager
     * @param parent
     */
    explicit HostsSupervisionController(MasticLauncherManager* masticLauncherManager, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostsSupervisionController();




Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToHost(QString command, QString hostname, QString commandLine);


public Q_SLOTS:

    /**
     * @brief Slot when a new model of host has been created
     * @param host
     */
    void onHostModelCreated(HostM* hostModel);

    /**
     * @brief Slot when a model of host will be removed
     * @param host
     */
    void onHostModelWillBeRemoved(HostM* hostModel);

private Q_SLOTS:




private:


private:
    // Manager for the Mastic launchers
    MasticLauncherManager* _masticLauncherManager;

    // Map from agent name to a list of view models of agent
    QHash<HostM*, HostVM*> _mapFromHostModelToViewModel;
};

QML_DECLARE_TYPE(HostsSupervisionController)

#endif // HOSTSSUPERVISIONCONTROLLER_H
