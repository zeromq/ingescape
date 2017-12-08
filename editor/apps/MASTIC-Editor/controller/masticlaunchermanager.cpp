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
 * @param peerId
 * @param hostName
 * @param ipAddress
 */
void MasticLauncherManager::addMasticLauncher(QString peerId, QString hostName, QString ipAddress)
{
    if (!peerId.isEmpty() && !hostName.isEmpty())
    {
        if (!_mapFromNameToHost.contains(hostName))
        {
            // Create a new host
            HostM* host = new HostM(hostName, peerId, ipAddress, this);

            // Add to the list
            _hosts.append(host);

            // Add to the map
            _mapFromNameToHost.insert(hostName, host);
        }
    }
}


/**
 * @brief Remove a Mastic Launcher
 * @param peerId
 * @param hostName
 */
void MasticLauncherManager::removeMasticLauncher(QString peerId, QString hostName)
{
    if (!peerId.isEmpty() && !hostName.isEmpty())
    {
        if (_mapFromNameToHost.contains(hostName))
        {
            HostM* host = _mapFromNameToHost.value(hostName);
            if (host != NULL) {
                // Remove from the list
                _hosts.remove(host);
            }

            // Remove from the map
            _mapFromNameToHost.remove(hostName);
        }
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

    if (!hostName.isEmpty() && _mapFromNameToHost.contains(hostName))
    {
        HostM* host = _mapFromNameToHost.value(hostName);
        if (host != NULL) {
            peerIdMasticLauncher = host->peerId();
        }
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
