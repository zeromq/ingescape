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

#include "ingescapeexpecontroller.h"

#include <QApplication>
#include <QFileDialog>

#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeExpeController::IngeScapeExpeController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _errorMessageWhenConnectionFailed(""),
    _snapshotDirectory(""),
    _modelManager(nullptr),
    _networkC(nullptr),
    _currentDirectoryPath(""),
    _platformNamesList(QStringList()),
    _terminationSignalWatcher(nullptr),
    _jsonHelper(nullptr),
    _platformDirectoryPath(""),
    //_platformDefaultFilePath("")
    _platformPathsList(QStringList())
{
    qInfo() << "New IngeScape Expe Controller";

    //
    // Snapshots directory
    //
    QString snapshotsDirectoryPath = IngeScapeUtils::getSnapshotsPath();
    QDir snapshotsDirectory(snapshotsDirectoryPath);
    if (snapshotsDirectory.exists())
    {
        _snapshotDirectory = snapshotsDirectoryPath;
    }
    else
    {
        qCritical() << "ERROR: could not create directory at '" << snapshotsDirectoryPath << "' !";
    }


    //
    // Settings
    //
    IngeScapeSettings& settings = IngeScapeSettings::Instance();

    // Settings about the "Network"
    settings.beginGroup("network");
    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
    _ipAddress = settings.value("ipAddress", QVariant("")).toString();
    _port = settings.value("port", QVariant(0)).toUInt();
    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);
    settings.endGroup();


    // Root directory
    QString rootPath = IngeScapeUtils::getRootPath();
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << rootPath << "' !";
    }

    // Directory for platform files
    QString platformPath = IngeScapeUtils::getPlatformsPath();

    QDir platformDir(platformPath);
    if (!platformDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << platformPath << "' !";
    }
    else
    {
        _platformDirectoryPath = platformPath;
    }


    // Create the helper to manage JSON files
    _jsonHelper = new JsonHelper(this);


    //
    // Create sub-controllers
    //

    // Create the manager for the data model of our IngeScape Expe application
    _modelManager = new ExpeModelManager(_jsonHelper, rootPath, this);

    // Create the controller for network communications
    _networkC = new NetworkController(this);

    // Update the list of available network devices
    _networkC->updateAvailableNetworkDevices();

    // There is only one available network device, we use it !
    if (_networkC->availableNetworkDevices().count() == 1) {
        _networkDevice = _networkC->availableNetworkDevices().at(0);
    }

    // Start our INGESCAPE agent with a network device (or an IP address) and a port
    bool isStarted = _networkC->start(_networkDevice, _ipAddress, _port);

    if (isStarted)
    {
        // Initialize platform from online mapping
        //_modelManager->setisMappingConnected(true);
    }
    else {
        seterrorMessageWhenConnectionFailed(tr("Failed to connect with network device %1 on port %2").arg(_networkDevice, QString::number(_port)));
    }


    //
    // Subscribe to system signals to interceipt interruption and termination signals
    //
    _terminationSignalWatcher = new TerminationSignalWatcher(this);
    connect(_terminationSignalWatcher, &TerminationSignalWatcher::terminationSignal,
            [=] () {
        qDebug() << "\n\n\n CATCH Termination Signal \n\n\n";

        if (QApplication::instance() != nullptr)
        {
            QApplication::instance()->quit();
        }
    });


    // Sleep to display our loading screen
    QThread::msleep(2000);
}


/**
* @brief Destructor
*/
IngeScapeExpeController::~IngeScapeExpeController()
{
    //
    // Clean-up our TerminationSignalWatcher first
    //
    if (_terminationSignalWatcher != nullptr)
    {
        disconnect(_terminationSignalWatcher, nullptr);
        delete _terminationSignalWatcher;
        _terminationSignalWatcher = nullptr;
    }


    //
    // Clean-up sub-controllers
    //

    if (_modelManager != nullptr)
    {
        disconnect(_modelManager);

        ExpeModelManager* temp = _modelManager;
        setmodelManager(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_networkC != nullptr)
    {
        disconnect(_networkC);

        NetworkController* temp = _networkC;
        setnetworkC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Delete json helper
    /*if (_jsonHelper != nullptr)
    {
        delete _jsonHelper;
        _jsonHelper = nullptr;
    }*/

    qInfo() << "Delete IngeScape Expe Controller";
}


/**
* @brief Method used to provide a singleton to QML
* @param engine
* @param scriptEngine
* @return
*/
QObject* IngeScapeExpeController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new IngeScapeExpeController();
}


/**
 * @brief Actions to perform before the application closing
 */
void IngeScapeExpeController::processBeforeClosing()
{
    // TODO...
}


/**
 * @brief Select a directory
 */
void IngeScapeExpeController::selectDirectory()
{
    _platformNamesList.clear();
    _platformPathsList.clear();

    // Open a directory dialog box
    QString directoryPath = QFileDialog::getExistingDirectory(nullptr,
                                                              "Open a directory with IngeScape platform files",
                                                              _platformDirectoryPath);

    // Update the property
    setcurrentDirectoryPath(directoryPath);

    QStringList tempPlatformNamesList;

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
            for (QFileInfo fileInfo : fileInfoList)
            {
                //qDebug() << fileInfo.fileName();

                // "toLower" allows to manage both extensions: "json" and "JSON"
                if (fileInfo.completeSuffix().toLower() == "json")
                {
                    tempPlatformNamesList.append(fileInfo.baseName());
                    _platformPathsList.append(fileInfo.absoluteFilePath());
                }
            }
        }
    }

    setplatformNamesList(tempPlatformNamesList);
}


/**
 * @brief Open a platform (at index)
 * @param index
 */
void IngeScapeExpeController::openPlatform(int index)
{
    if (_platformPathsList.count() == _platformNamesList.count())
    {
        if (index < _platformPathsList.count())
        {
            QString platformFileName = _platformNamesList.at(index);
            QString platformFilePath = _platformPathsList.at(index);

            qInfo() << "Open platform" << platformFileName << "(" << platformFilePath << ")";
        }
    }
}


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeExpeController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}


