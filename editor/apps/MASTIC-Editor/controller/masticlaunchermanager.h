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
 *
 */

#ifndef MASTICLAUNCHERMANAGER_H
#define MASTICLAUNCHERMANAGER_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <model/hostm.h>


/**
 * @brief The MasticLauncherManager class defines the manager for launchers of MASTIC agents
 */
class MasticLauncherManager : public QObject
{
    Q_OBJECT

    // List of all hosts
    I2_QOBJECT_LISTMODEL(HostM, hosts)


public:

    /**
     * @brief Get our instance
     * @return
     */
    static MasticLauncherManager& Instance();


    /**
     * @brief Destructor
     */
    ~MasticLauncherManager();


    /**
     * @brief Add a Mastic Launcher
     * @param peerId
     * @param hostName
     * @param ipAddress
     */
    void addMasticLauncher(QString peerId, QString hostName, QString ipAddress);


    /**
     * @brief Remove a Mastic Launcher
     * @param peerId
     * @param hostName
     */
    void removeMasticLauncher(QString peerId, QString hostName);


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


private:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit MasticLauncherManager(QObject *parent = nullptr);


    /**
     * @brief Copy constructor
     */
    MasticLauncherManager(MasticLauncherManager const&);


    /**
     * @brief Assign operator
     * @return
     */
    MasticLauncherManager& operator=(MasticLauncherManager const&);


private:

    // Map from "Name" to the "Host" of the corresponding MASTIC launcher
    QHash<QString, HostM*> _mapFromNameToHost;

};

QML_DECLARE_TYPE(MasticLauncherManager)

#endif // MASTICLAUNCHERMANAGER_H
