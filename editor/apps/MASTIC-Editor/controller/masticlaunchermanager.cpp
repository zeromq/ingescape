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
 * @brief Destructor
 */
MasticLauncherManager::~MasticLauncherManager()
{
    qInfo() << "Delete Mastic Launcher Manager";

    // Clear the map
    _mapFromNameToHost.clear();

    // Free memory
    _hosts.deleteAllItems();
}


/**
 * @brief Add a Mastic Launcher
 * @param peerId
 * @param hostName
 * @param ipAddress
 */
void MasticLauncherManager::addMasticLauncher(QString peerId, QString hostName, QString ipAddress)
{
    if (!hostName.isEmpty())
    {
        HostM* host = getHostWithName(hostName);
        if (host == NULL)
        {
            // Create a new host
            host = new HostM(hostName, peerId, ipAddress, this);

            // Add to the list
            _hosts.append(host);

            // Add to the map
            _mapFromNameToHost.insert(hostName, host);
        }
        else
        {
            // Update peer id
            if (host->peerId() != peerId) {
                host->setpeerId(peerId);
            }

            // Update IP address
            if (host->ipAddress() != ipAddress) {
                host->setipAddress(ipAddress);
            }
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
    Q_UNUSED(peerId)

    if (!hostName.isEmpty())
    {
        HostM* host = getHostWithName(hostName);
        if (host != NULL)
        {
            // Remove from the list
            _hosts.remove(host);

            // Remove from the map
            _mapFromNameToHost.remove(hostName);
        }
    }
}


/**
 * @brief Get the host with a (Host)Name
 * @param hostName
 * @return
 */
HostM* MasticLauncherManager::getHostWithName(QString hostName)
{
    if (_mapFromNameToHost.contains(hostName)) {
        return _mapFromNameToHost.value(hostName);
    }
    else {
        return NULL;
    }
}


/**
 * @brief Get the peer id of a Launcher with a HostName
 * @param hostName
 * @return
 */
QString MasticLauncherManager::getPeerIdOfLauncherWithHostName(QString hostName)
{
    HostM* host = getHostWithName(hostName);
    if (host != NULL) {
        return host->peerId();
    }
    else {
        return "";
    }
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
