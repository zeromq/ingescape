/*
 *	IngeScape Expe
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "expemodelmanager.h"

/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
ExpeModelManager::ExpeModelManager(JsonHelper* jsonHelper,
                                   QString rootDirectoryPath,
                                   QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                            rootDirectoryPath,
                                                                            parent),
    _peerIdOfEditor(""),
    _peerNameOfEditor(),
    _isEditorON(false),
    _currentDirectoryPath(""),
    _currentLoadedPlatform(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Expe Model Manager";

}


/**
 * @brief Destructor
 */
ExpeModelManager::~ExpeModelManager()
{
    qInfo() << "Delete IngeScape Expe Model Manager";


    if (_currentLoadedPlatform != nullptr) {
        setcurrentLoadedPlatform(nullptr);
    }

    // Free memory by deleting all platforms
    _platformsList.deleteAllItems();

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}


/**
 * @brief Setter for property "Current Loaded Platform"
 * @param value
 */
void ExpeModelManager::setcurrentLoadedPlatform(PlatformM *value)
{
    if (_currentLoadedPlatform != value)
    {
        if (_currentLoadedPlatform != nullptr)
        {
            // Clear agents of previous platform
            qDebug() << "Clear agents of previous platform" << _currentLoadedPlatform->name();

            // If there are some agents
            if (!_allAgentsGroupsByName.isEmpty())
            {
                // Delete agents OFF
                deleteAgentsOFF();

                qDebug() << _allAgentsGroupsByName.count() << "agents after the call to 'delete Agents OFF'";
            }
        }

        _currentLoadedPlatform = value;

        if (_currentLoadedPlatform != nullptr)
        {
            // Load agents of new platform
            qDebug() << "Load agents of new platform" << _currentLoadedPlatform->name();

            // Import agents list from the file path
            bool success = importAgentOrAgentsListFromFilePath(_currentLoadedPlatform->filePath());
            if (success)
            {
                qDebug() << _allAgentsGroupsByName.count() << "imported agents";
            }
            else {
                qCritical() << "Error while importing the agents list from" << _currentLoadedPlatform->filePath();
            }
        }

        Q_EMIT currentLoadedPlatformChanged(value);
    }
}


/**
 * @brief List all IngeScape platforms in a directory
 * @param directoryPath
 */
void ExpeModelManager::listPlatformsInDirectory(QString directoryPath)
{
    // First, reset the current loaded platform if needed
    if (_currentLoadedPlatform != nullptr) {
        setcurrentLoadedPlatform(nullptr);
    }

    // Clear the list and delete all platforms
    _platformsList.deleteAllItems();

    // Update the property
    setcurrentDirectoryPath(directoryPath);

    if (!directoryPath.isEmpty())
    {
        QDir dir(directoryPath);
        if (dir.exists())
        {
            dir.setFilter(QDir::Files);

            // FIXME: Don't merge UpperCase / LowerCase
            dir.setSorting(QDir::Name);

            qDebug() << "There are" << dir.count() << "entries in the directory" << directoryPath;

            QFileInfoList fileInfoList = dir.entryInfoList();

            QList<PlatformM*> tempPlatformsList;

            for (QFileInfo fileInfo : fileInfoList)
            {
                //qDebug() << fileInfo.fileName();

                // "toLower" allows to manage both extensions: "json" and "JSON"
                if (fileInfo.completeSuffix().toLower() == "json")
                {
                    // Create a new IngeScape platform
                    PlatformM* platform = new PlatformM(fileInfo.baseName(), fileInfo.absoluteFilePath(), this);

                    tempPlatformsList.append(platform);
                }
            }

            // QML updated only once
            _platformsList.append(tempPlatformsList);
        }
    }
}


/**
 * @brief Slot called when an editor enter the network
 * @param peerId
 * @param peerName
 * @param ipAddress
 * @param hostname
 */
void ExpeModelManager::onEditorEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qInfo() << "Editor entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    if (!_isEditorON  && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfEditor(peerId);
        setpeerNameOfEditor(peerName);

        setisEditorON(true);
    }
    else {
        qCritical() << "We are already connected to an editor:" << _peerNameOfEditor << "(" << _peerIdOfEditor << ")";
    }
}


/**
 * @brief Slot called when an editor quit the network
 * @param peerId
 * @param peerName
 */
void ExpeModelManager::onEditorExited(QString peerId, QString peerName)
{
    qInfo() << "Editor exited (" << peerId << ")" << peerName;

    if (_isEditorON && (_peerIdOfEditor == peerId))
    {
        setpeerIdOfEditor("");
        setpeerNameOfEditor("");

        setisEditorON(false);
    }
}


/**
 * @brief Slot called when an editor reply to our command "Load Platform File" with a status
 * @param commandStatus
 * @param commandParameters
 */
void ExpeModelManager::onStatusReceivedAbout_LoadPlatformFile(bool commandStatus, QString commandParameters)
{
    PlatformM* loadedPlatform = nullptr;

    if (commandStatus)
    {
         qDebug() << "Platform" << commandParameters << "Loaded";

         for (PlatformM* iterator : _platformsList)
         {
             if ((iterator != nullptr) && (iterator->filePath() == commandParameters))
             {
                 loadedPlatform = iterator;
                 break;
             }
         }
    }
    else {
        qCritical() << "Editor failed to load the platform" << commandParameters;
    }

    // Update property "Current Loaded Platform"
    setcurrentLoadedPlatform(loadedPlatform);
}

