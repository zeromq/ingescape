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
//#include <QRandomGenerator>

/**
 * @brief Constructor
 * @param parent
 */
ExpeModelManager::ExpeModelManager(QObject *parent) : QObject(parent),
    _peerOfEditor(nullptr),
    _isEditorON(false),
    _peerOfRecorder(nullptr),
    _isRecorderON(false),
    _currentDirectoryPath(""),
    _currentLoadedPlatform(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Expe Model Manager";

    // Platforms are sorted on their name (alphabetical order)
    _platformsList.setSortProperty("currentIndex");


    //
    // Link our filtered list to the list of all agents grouped by name
    //
    if (IngeScapeModelManager::instance() != nullptr)
    {
         _filteredPlatformAgents.setSourceModel(IngeScapeModelManager::instance()->allAgentsGroupsByName()->sourceModel());
    }
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
}


/**
 * @brief Setter for property "Current Loaded Platform"
 * @param value
 */
void ExpeModelManager::setcurrentLoadedPlatform(PlatformM *value)
{
    if (_currentLoadedPlatform != value)
    {
        IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

        if (_currentLoadedPlatform != nullptr)
        {
            // Clear agents of previous platform
            qDebug() << "Clear agents of previous platform" << _currentLoadedPlatform->name();

            // Udpate the flag "is Loaded"
            _currentLoadedPlatform->setisLoaded(false);

            // If there are some agents
            if ((ingeScapeModelManager != nullptr) && !ingeScapeModelManager->allAgentsGroupsByName()->isEmpty())
            {
                // Delete agents OFF
                ingeScapeModelManager->deleteAgentsOFF();

                qDebug() << ingeScapeModelManager->allAgentsGroupsByName()->count() << "agents after the call to 'delete Agents OFF'";
            }

            // Reset the list of agent names of the filter
            _filteredPlatformAgents.setagentNamesOfPlatform(QStringList());
        }

        _currentLoadedPlatform = value;

        if ((_currentLoadedPlatform != nullptr) && (ingeScapeModelManager != nullptr))
        {
            // Load agents of new platform
            qDebug() << "Load agents of new platform" << _currentLoadedPlatform->name();

            // Udpate the flag "is Loaded"
            _currentLoadedPlatform->setisLoaded(true);

            // Update the list of agent names of the filter
            _filteredPlatformAgents.setagentNamesOfPlatform(_currentLoadedPlatform->agentNamesList());

            // Import agents list from the file path
            bool success = ingeScapeModelManager->importAgentOrAgentsListFromFilePath(_currentLoadedPlatform->filePath());
            if (success)
            {
                qDebug() << ingeScapeModelManager->allAgentsGroupsByName()->count() << "imported agents";
            }
            else {
                qCritical() << "Error while importing the agents list from" << _currentLoadedPlatform->filePath();
            }
        }

        // Update the filter
        _filteredPlatformAgents.updateFilter();

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
            QStringList nameFilters = { "*.igsplatform", "*.json" };

            // Only JSON files and Ignore Case (Upper/Lower case)
            QFileInfoList fileInfoList = dir.entryInfoList(nameFilters, QDir::Files, QDir::IgnoreCase);

            int nbJsonFiles = fileInfoList.length();
            qDebug() << "There are" << nbJsonFiles << "entries in the directory" << directoryPath;

            QList<PlatformM*> tempPlatformsList;

            for (int i = 0; i < nbJsonFiles; i++)
            {
                QFileInfo fileInfo = fileInfoList.at(i);

                //qDebug() << fileInfo.fileName();

                // Create a new IngeScape platform
                PlatformM* platform = new PlatformM(fileInfo.baseName(), fileInfo.absoluteFilePath(), i, this);

                tempPlatformsList.append(platform);
            }

            // QML updated only once
            _platformsList.append(tempPlatformsList);
        }
    }
}


/**
 * @brief Randomize the list of platforms
 */
void ExpeModelManager::randomizePlatformsList()
{
    if (!_platformsList.isEmpty())
    {
        int nbPlatforms = _platformsList.count();

        qDebug() << "Randomize the" << nbPlatforms << "platforms";

        // Get a list of random indexes
        QList<int> randomIndexes = _getRandomIndexes(nbPlatforms);

        qDebug() << "Random indexes:" << randomIndexes;

        // Copy the list
        QList<PlatformM*> copyOfPlatformsList = _platformsList.toList();
        QList<PlatformM*> tempPlatformsList;

        // Clear to prevent QML update
        _platformsList.clear();

        int newIndex = 0;

        // For each platform, set the current index to the random index
        for (int i = 0; i < nbPlatforms; i++)
        {
            int randomIndex = randomIndexes.at(i);

            if (randomIndex < copyOfPlatformsList.count())
            {
                PlatformM* platform = copyOfPlatformsList.takeAt(randomIndex);
                if (platform != nullptr)
                {
                    //_platformsList.append(platform);
                    tempPlatformsList.append(platform);

                    platform->setcurrentIndex(newIndex);

                    //qDebug() << "new index" << newIndex << "random index" << randomIndex << "index of Alphabetic Order" << platform->indexOfAlphabeticOrder();

                    newIndex++;
                }
            }
            else {
                qCritical() <<  "Random index" << randomIndex << "> platforms list count()" << copyOfPlatformsList.count();
            }
        }

        // QML updated only once
        _platformsList.append(tempPlatformsList);
    }
}


/**
 * @brief Sort the list of platforms in alphabetical order
 */
void ExpeModelManager::sortPlatformsListInAlphabeticOrder()
{
    if (!_platformsList.isEmpty())
    {
        int nbPlatforms = _platformsList.count();

        qDebug() << "Sort the" << nbPlatforms << "platforms in alphabetical order";

        // Copy the list
        QList<PlatformM*> tempPlatformsList = _platformsList.toList();

        // Clear to prevent QML update
        _platformsList.clear();

        // For each platform, reset the current index to the alphabetical index
        for (PlatformM* platform : tempPlatformsList)
        {
            if (platform != nullptr) {
                platform->setcurrentIndex(platform->indexOfAlphabeticOrder());
            }
        }

        // QML updated only once
        _platformsList.append(tempPlatformsList);
    }
}


/**
 * @brief Slot called when an editor enter the network
 */
void ExpeModelManager::onEditorEntered(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Editor entered" << peer->name() << "(" << peer->uid() << ") on" << peer->hostname() << "(" << peer->ipAddress() << ")";

        if (!_isEditorON  && (_peerOfEditor == nullptr))
        {
            setpeerOfEditor(peer);
            setisEditorON(true);
        }
        else if (_peerOfEditor != nullptr) {
            qCritical() << "We are already connected to the editor:" << _peerOfEditor->name() << "(" << _peerOfEditor->uid() << ")";
        }
    }
}


/**
 * @brief Slot called when an editor quit the network
 */
void ExpeModelManager::onEditorExited(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Editor exited" << peer->name() << "(" << peer->uid() << ")";

        if (_isEditorON && (_peerOfEditor == peer))
        {
            setpeerOfEditor(nullptr);
            setisEditorON(false);
        }
    }
}


/**
 * @brief Slot called when a recorder enter the network
 */
void ExpeModelManager::onRecorderEntered(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Recorder entered" << peer->name() << "(" << peer->uid() << ") on" << peer->hostname() << "(" << peer->ipAddress() << ")";

        if (!_isRecorderON && (_peerOfRecorder == nullptr))
        {
            setpeerOfRecorder(peer);
            setisRecorderON(true);
        }
        else if (_peerOfRecorder != nullptr) {
            qCritical() << "We are already connected to the recorder:" << _peerOfRecorder->name() << "(" << _peerOfRecorder->uid() << ")";
        }
    }
}


/**
 * @brief Slot called when a recorder quit the network
 */
void ExpeModelManager::onRecorderExited(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Recorder exited" << peer->name() << "(" << peer->uid() << ")";

        if (_isRecorderON && (_peerOfRecorder == peer))
        {
            setpeerOfRecorder(nullptr);
            setisRecorderON(false);
        }
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

         for (PlatformM* iterator : _platformsList.toList())
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


/**
 * @brief Get a list of random indexes
 * @param max
 * @return
 */
QList<int> ExpeModelManager::_getRandomIndexes(int max)
{
    QList<int> randomIndexes;

    if (max > 0)
    {
        /*QRandomGenerator* randomGenerator = QRandomGenerator::global();

        QVector<quint32> vector = QVector<quint32>(max);

        randomGenerator->fillRange(vector.data(), vector.size());

        for (int i = max; i > 0; i--)
        {
            quint32 randomUint32 = vector.at(i - 1);

            quint32 randomIndex = randomUint32 % static_cast<quint32>(i);

            randomIndexes.append(static_cast<int>(randomIndex));

            qDebug() << i << ":" << randomIndex;
        }*/

        for (int i = max; i > 0; i--)
        {
            int randomIndex = qrand() % i;

            randomIndexes.append(randomIndex);

            //qDebug() << i << ":" << randomIndex;
        }
    }
    return randomIndexes;
}

