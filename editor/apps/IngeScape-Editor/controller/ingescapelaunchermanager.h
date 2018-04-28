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

#ifndef INGESCAPELAUNCHERMANAGER_H
#define INGESCAPELAUNCHERMANAGER_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <model/hostm.h>


/**
 * @brief The IngeScapeLauncherManager class defines the manager for launchers of INGESCAPE agents
 */
class IngeScapeLauncherManager : public QObject
{
    Q_OBJECT

    // List of all hosts
    I2_QOBJECT_LISTMODEL(HostM, hosts)


public:

    /**
     * @brief Get our instance
     * @return
     */
    static IngeScapeLauncherManager& Instance();


    /**
     * @brief Destructor
     */
    ~IngeScapeLauncherManager();


    /**
     * @brief Add a IngeScape Launcher
     * @param peerId
     * @param hostName
     * @param ipAddress
     */
    void addIngeScapeLauncher(QString peerId, QString hostName, QString ipAddress, QString streamingPort);


    /**
     * @brief Remove a IngeScape Launcher
     * @param peerId
     * @param hostName
     */
    void removeIngeScapeLauncher(QString peerId, QString hostName);


    /**
     * @brief Get the host with a (Host)Name
     * @param hostName
     * @return
     */
    HostM* getHostWithName(QString hostName);


    /**
     * @brief Get the peer id of a Launcher with a HostName
     * @param hostName
     * @return
     */
    QString getPeerIdOfLauncherWithHostName(QString hostName);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a new model of host has been created
     * @param agent
     */
    void hostModelCreated(HostM* host);

    /**
     * @brief Signal emitted when a model of host has been removed
     * @param agent
     */
    void hostModelWillBeRemoved(HostM* host);

private:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeLauncherManager(QObject *parent = nullptr);


    /**
     * @brief Copy constructor
     */
    IngeScapeLauncherManager(IngeScapeLauncherManager const&);


    /**
     * @brief Assign operator
     * @return
     */
    IngeScapeLauncherManager& operator=(IngeScapeLauncherManager const&);


private:

    // Map from "Name" to the "Host" of the corresponding INGESCAPE launcher
    QHash<QString, HostM*> _mapFromNameToHost;

};

QML_DECLARE_TYPE(IngeScapeLauncherManager)

#endif // INGESCAPELAUNCHERMANAGER_H
