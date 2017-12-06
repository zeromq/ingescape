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


/**
 * @brief The MasticLauncherManager class defines the manager for launchers of MASTIC agents
 */
class MasticLauncherManager : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Get our instance
     * @return
     */
    static MasticLauncherManager& Instance();


    /**
     * @brief Add a Mastic Launcher
     * @param hostName
     * @param peerId
     */
    void addMasticLauncher(QString hostName, QString peerId);


    /**
     * @brief Remove a Mastic Launcher
     * @param hostName
     */
    void removeMasticLauncher(QString hostName);


    /**
     * @brief Get the peer id of The Mastic Launcher with a HostName
     * @param hostName
     * @return
     */
    QString getPeerIdOfMasticLauncherWithHostName(QString hostName);


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

    // Map from "HostName" to the "Peer Id" of the corresponding MASTIC launcher
    QHash<QString, QString> _mapFromHostNameToMasticLauncherPeerId;

};

QML_DECLARE_TYPE(MasticLauncherManager)

#endif // MASTICLAUNCHERMANAGER_H
