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

#include "masticlaunchermanager.h"

/**
 * @brief Get our instance
 * @return
 */
MasticLauncherManager& MasticLauncherManager::Instance()
{
    static MasticLauncherManager instance;
    return instance;
}


/**
 * @brief Add a Mastic Launcher
 * @param hostName
 * @param peerId
 */
void MasticLauncherManager::addMasticLauncher(QString hostName, QString peerId)
{
    if (!hostName.isEmpty() && !peerId.isEmpty()) {
        _mapFromHostNameToMasticLauncherPeerId.insert(hostName, peerId);
    }
}


/**
 * @brief Remove a Mastic Launcher
 * @param hostName
 */
void MasticLauncherManager::removeMasticLauncher(QString hostName)
{
    if (!hostName.isEmpty()) {
        _mapFromHostNameToMasticLauncherPeerId.remove(hostName);
    }
}


/**
 * @brief Get the peer id of The Mastic Launcher with a HostName
 * @param hostName
 * @return
 */
QString MasticLauncherManager::getPeerIdOfMasticLauncherWithHostName(QString hostName)
{
    QString peerIdMasticLauncher = "";

    if (!hostName.isEmpty() && _mapFromHostNameToMasticLauncherPeerId.contains(hostName)) {
        peerIdMasticLauncher = _mapFromHostNameToMasticLauncherPeerId.value(hostName);
    }
    return peerIdMasticLauncher;
}


/**
 * @brief Constructor
 * @param parent
 */
MasticLauncherManager::MasticLauncherManager(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New Mastic Launcher Manager";
}
