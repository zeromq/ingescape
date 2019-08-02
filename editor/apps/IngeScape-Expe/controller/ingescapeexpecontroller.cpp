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
    //_isPlayingTimeLine(false),
    _timeLineState(TimeLineStates::STOPPED),
    //_isRecording(false),
    _terminationSignalWatcher(nullptr),
    _jsonHelper(nullptr),
    _platformDirectoryPath(""),
    _withRecord(true)
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


    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::editorEntered, _modelManager, &ExpeModelManager::onEditorEntered);
    connect(_networkC, &NetworkController::editorExited, _modelManager, &ExpeModelManager::onEditorExited);
    connect(_networkC, &NetworkController::recorderEntered, _modelManager, &ExpeModelManager::onRecorderEntered);
    connect(_networkC, &NetworkController::recorderExited, _modelManager, &ExpeModelManager::onRecorderExited);
    connect(_networkC, &NetworkController::agentEntered, _modelManager, &ExpeModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &ExpeModelManager::onAgentExited);
    connect(_networkC, &NetworkController::statusReceivedAbout_LoadPlatformFile, _modelManager, &ExpeModelManager::onStatusReceivedAbout_LoadPlatformFile);
    connect(_networkC, &NetworkController::timeLineStateUpdated, this, &IngeScapeExpeController::_onTimeLineStateUpdated);


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
    // Open a directory dialog box
    QString directoryPath = QFileDialog::getExistingDirectory(nullptr,
                                                              "Open a directory with IngeScape platform files",
                                                              _platformDirectoryPath);

    if (_modelManager)
    {
        // List all IngeScape platforms in this directory
        _modelManager->listPlatformsInDirectory(directoryPath);
    }
}


/**
 * @brief Open a platform
 * @param platform
 */
void IngeScapeExpeController::openPlatform(PlatformM* platform)
{
    if ((platform != nullptr) && (_modelManager != nullptr) && _modelManager->isEditorON() && (_networkC != nullptr))
    {
        qInfo() << "Open platform" << platform->name() << "at" << platform->filePath();

        QString commandAndParameters = QString("%1=%2").arg(command_LoadPlatformFile, platform->filePath());

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);
    }
}


/**
 * @brief Play the TimeLine and Start to record if flag "withRecord" is true
 * OR only resume the TimeLine
 * @param withRecord
 */
void IngeScapeExpeController::playOrResumeTimeLine(bool withRecord)
{
    if ((_modelManager != nullptr) && _modelManager->isEditorON() && (_modelManager->currentLoadedPlatform() != nullptr)
            && (_networkC != nullptr))
    {
        // The current loaded platform is currently recording...
        if (_modelManager->currentLoadedPlatform()->recordState() == RecordStates::RECORDING)
        {
            // RESUME TimeLine
            qInfo() << "Resume the timeline of platform" << _modelManager->currentLoadedPlatform()->name();
        }
        else
        {
            // Update flag
            _withRecord = withRecord;

            if (_withRecord)
            {
                // Start recording
                _startRecording();
            }

            // START TimeLine
            qInfo() << "Play the timeline of platform" << _modelManager->currentLoadedPlatform()->name();
        }

        QString commandAndParameters = QString("%1=%2").arg(command_UpdateTimeLineState, PLAY);

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);
    }
}


/**
 * @brief Pause the TimeLine
 */
void IngeScapeExpeController::pauseTimeLine()
{
    if ((_modelManager != nullptr) && _modelManager->isEditorON() && (_modelManager->currentLoadedPlatform() != nullptr)
            && (_networkC != nullptr))
    {
        qInfo() << "Pause the timeline of platform" << _modelManager->currentLoadedPlatform()->name();

        QString commandAndParameters = QString("%1=%2").arg(command_UpdateTimeLineState, PAUSE);

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);
    }
}


/**
 * @brief Stop the TimeLine
 */
void IngeScapeExpeController::stopTimeLine()
{
    if ((_modelManager != nullptr) && _modelManager->isEditorON() && (_modelManager->currentLoadedPlatform() != nullptr)
            && (_networkC != nullptr))
    {
        qInfo() << "STOP the timeline of platform" << _modelManager->currentLoadedPlatform()->name();

        QString commandAndParameters = QString("%1=%2").arg(command_UpdateTimeLineState, RESET);

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);


        if (_withRecord)
        {
            // Stop recording
            _stopRecording();

            // Reset flag
            _withRecord = false;
        }
    }
}


/**
 * @brief Start Recording
 */
void IngeScapeExpeController::_startRecording()
{
    if ((_modelManager != nullptr) && _modelManager->isEditorON() && (_modelManager->currentLoadedPlatform() != nullptr)
            && (_networkC != nullptr))
    {
        qInfo() << "Start recording the platform" << _modelManager->currentLoadedPlatform()->name();

        QString commandAndParameters = QString("%1=%2").arg(command_UpdateRecordState, START);

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);


        // FIXME: get state from recorder messages
        _modelManager->currentLoadedPlatform()->setrecordState(RecordStates::RECORDING);
    }
}


/**
 * @brief Stop Recording
 */
void IngeScapeExpeController::_stopRecording()
{
    if ((_modelManager != nullptr) && _modelManager->isEditorON() && (_modelManager->currentLoadedPlatform() != nullptr)
            && (_networkC != nullptr))
    {
        qInfo() << "Stop recording the platform" << _modelManager->currentLoadedPlatform()->name();

        QString commandAndParameters = QString("%1=%2").arg(command_UpdateRecordState, STOP);

        // Send the command and parameters to the editor
        _networkC->sendCommandToEditor(_modelManager->peerIdOfEditor(), commandAndParameters);


        // FIXME: get state from recorder messages
        _modelManager->currentLoadedPlatform()->setrecordState(RecordStates::RECORDED);
    }
}


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeExpeController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}


/**
 * @brief Slot called when the state of the TimeLine updated (in Editor app)
 * @param parameters
 */
void IngeScapeExpeController::_onTimeLineStateUpdated(QString parameters)
{
    if (!parameters.isEmpty())
    {
        QStringList parametersList = parameters.split("|");
        if (parametersList.count() == 2)
        {
            QString state = parametersList.at(0);
            QString deltaTimeFromTimeLineStart = parametersList.at(1);

            qDebug() << state << "the timeline at" << deltaTimeFromTimeLineStart;

            // PLAY (the TimeLine)
            if (state == PLAY) {
                //setisPlayingTimeLine(true);
                settimeLineState(TimeLineStates::PLAYING);
            }
            // PAUSE (the TimeLine)
            else if (state == PAUSE) {
                //setisPlayingTimeLine(false);
                settimeLineState(TimeLineStates::PAUSED);
            }
            // RESET (the TimeLine)
            else if (state == RESET) {
                //setisPlayingTimeLine(false);
                settimeLineState(TimeLineStates::STOPPED);
            }
            else {
                qCritical() << "Unknown state" << state << "of the TimeLine !";
            }
        }
        else {
            qCritical() << parameters << "must have 2 parameters !";
        }
    }
}

