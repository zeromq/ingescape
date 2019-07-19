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
#include <QRandomGenerator>

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
    _peerIdOfRecorder(""),
    _peerNameOfRecorder(""),
    _isRecorderON(false),
    _currentDirectoryPath(""),
    _currentLoadedPlatform(nullptr)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Expe Model Manager";

    // Platforms are sorted on their name (alphabetical order)
    _platformsList.setSortProperty("currentIndex");

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

            // Udpate the flag "is Loaded"
            _currentLoadedPlatform->setisLoaded(false);

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

            // Udpate the flag "is Loaded"
            _currentLoadedPlatform->setisLoaded(true);

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
            QStringList nameFilters = { "*.json", "*.JSON" };

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
 * @brief Slot called when a recorder enter the network
 * @param peerId
 * @param peerName
 * @param ipAddress
 * @param hostname
 */
void ExpeModelManager::onRecorderEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qInfo() << "Recorder entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    if (!_isRecorderON  && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfRecorder(peerId);
        setpeerNameOfRecorder(peerName);

        setisRecorderON(true);
    }
    else {
        qCritical() << "We are already connected to a recorder:" << _peerNameOfRecorder << "(" << _peerIdOfRecorder << ")";
    }
}


/**
 * @brief Slot called when a recorder quit the network
 * @param peerId
 * @param peerName
 */
void ExpeModelManager::onRecorderExited(QString peerId, QString peerName)
{
    qInfo() << "Recorder exited (" << peerId << ")" << peerName;

    if (_isRecorderON && (_peerIdOfRecorder == peerId))
    {
        setpeerIdOfRecorder("");
        setpeerNameOfRecorder("");

        setisRecorderON(false);
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
        QRandomGenerator* randomGenerator = QRandomGenerator::global();

        QVector<quint32> vector = QVector<quint32>(max);

        randomGenerator->fillRange(vector.data(), vector.size());

        for (int i = max; i > 0; i--)
        {
            quint32 randomUint32 = vector.at(i - 1);

            quint32 randomIndex = randomUint32 % static_cast<quint32>(i);

            randomIndexes.append(static_cast<int>(randomIndex));

            //qDebug() << i << ":" << randomIndex;
        }

        /*QStringList data1 = QStringList();
        QStringList data2 = QStringList();

        for (int i = 1; i <= 20; i++)
        {
            data1.append(QString("igs-%1").arg(i));
        }
        qDebug() << "AVANT: data1" << data1;

        QVector<quint32> vector = QVector<quint32>(20);
        //qDebug() << "AVANT vector" << vector;
        randomGenerator->fillRange(vector.data(), vector.size());
        qDebug() << "Randoms:" << vector;

        for (int i = 20; i > 0; i--)
        {
            quint32 randomUint32 = vector.at(i - 1);
            int randomIndex = randomUint32 % i;

            if (randomIndex < data1.length())
            {
                QString item = data1.takeAt(randomIndex);
                data2.append(item);

                qDebug() << i << ":" << randomIndex << item;
                //qDebug() << "data1" << data1;
                //qDebug() << "data2" << data2;
            }
            else {
                qCritical() <<  "random index" << randomIndex << "too high for 'data1'" << data1.length();
            }
        }

        //qDebug() << "APRES: data1" << data1;
        qDebug() << "APRES: data2" << data2;*/
    }
    return randomIndexes;
}
