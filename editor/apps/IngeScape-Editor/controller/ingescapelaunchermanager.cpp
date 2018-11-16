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

#include "ingescapelaunchermanager.h"

/**
 * @brief Get our instance
 * @return
 */
IngeScapeLauncherManager& IngeScapeLauncherManager::Instance()
{
    static IngeScapeLauncherManager instance;
    return instance;
}


/**
 * @brief Destructor
 */
IngeScapeLauncherManager::~IngeScapeLauncherManager()
{
    qInfo() << "Delete IngeScape Launcher Manager";

    // Reset the list of launchers
    reset();
}


/**
 * @brief Add an IngeScape Launcher
 * @param peerId
 * @param hostName
 * @param ipAddress
 */
void IngeScapeLauncherManager::addIngeScapeLauncher(QString peerId, QString hostName, QString ipAddress, QString streamingPort)
{
    if (!hostName.isEmpty())
    {
        // For test purposes, we create a host each time there is a new launcher detected
        HostM* host = getHostWithName(hostName);
        if (host == nullptr)
        {
            // Create a new host
            host = new HostM(hostName, peerId, ipAddress, streamingPort, this);

            // Add to the list
            _hosts.append(host);

            // Add to the map
            _mapFromNameToHost.insert(hostName, host);

            // Emit signal that a new  host model has been created
            Q_EMIT hostModelCreated(host);
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

            // Update streaming port
            if (host->streamingPort() != streamingPort) {
                host->setstreamingPort(streamingPort);
            }
        }
    }
}


/**
 * @brief Remove an IngeScape Launcher
 * @param peerId
 * @param hostName
 */
void IngeScapeLauncherManager::removeIngeScapeLauncher(QString peerId, QString hostName)
{
    Q_UNUSED(peerId)

    if (!hostName.isEmpty())
    {
        HostM* host = getHostWithName(hostName);
        if (host != nullptr)
        {
            // Emit signal that the host model will be removed
            Q_EMIT hostModelWillBeRemoved(host);

            // Remove from the list
            _hosts.remove(host);

            // Remove from the map
            _mapFromNameToHost.remove(hostName);

            // Free memory
            delete host;
        }
    }
}


/**
 * @brief Get the host with a (Host)Name
 * @param hostName
 * @return
 */
HostM* IngeScapeLauncherManager::getHostWithName(QString hostName)
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
QString IngeScapeLauncherManager::getPeerIdOfLauncherWithHostName(QString hostName)
{
    HostM* host = getHostWithName(hostName);
    if (host != nullptr) {
        return host->peerId();
    }
    else {
        return "";
    }
}


/**
 * @brief Reset the list of launchers (hosts)
 */
void IngeScapeLauncherManager::reset()
{
    for (HostM* host : _hosts)
    {
        if (host != nullptr)
        {
            // Emit signal that the host model will be removed
            Q_EMIT hostModelWillBeRemoved(host);
        }
    }

    // Clear the hash table
    _mapFromNameToHost.clear();

    // Free memory
    _hosts.deleteAllItems();
}


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeLauncherManager::IngeScapeLauncherManager(QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Launcher Manager";
}
