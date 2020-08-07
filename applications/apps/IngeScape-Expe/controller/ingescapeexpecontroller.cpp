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
#include <platformsupport/osutils.h>


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeExpeController::IngeScapeExpeController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _licensesPath(""),
    _errorMessageWhenConnectionFailed(""),
    _snapshotDirectory(""),
    _modelManager(nullptr),
    _networkC(nullptr),
    //_isPlayingTimeLine(false),
    _timeLineState(TimeLineStates::STOPPED),
    //_isRecording(false),
    _terminationSignalWatcher(nullptr),
    _platformDirectoryPath(""),
    _withRecord(true)
{
    qInfo() << "New IngeScape Expe Controller";

    // Root directory
    QString rootPath = IngeScapeUtils::getRootPath();
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << rootPath << "' !";
    }

    // Snapshots directory
    QString snapshotsDirectoryPath = IngeScapeUtils::getSnapshotsPath();
    QDir snapshotsDirectory(snapshotsDirectoryPath);
    if (snapshotsDirectory.exists()) {
        _snapshotDirectory = snapshotsDirectoryPath;
    }
    else {
        qCritical() << "ERROR: could not create directory at '" << snapshotsDirectoryPath << "' !";
    }


    //
    // Settings
    //
    IngeScapeSettings& settings = IngeScapeSettings::Instance();

    //
    // Settings about the "Network"
    //
    settings.beginGroup("network");

    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
    _ipAddress = settings.value("ipAddress", QVariant("")).toString();
    _port = settings.value("port", QVariant(0)).toUInt();
    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);

    settings.endGroup();


    //
    // Settings about the "Licenses"
    //
    settings.beginGroup("licenses");

    // Get the default path for "Licenses"
    QString defaultLicensesPath = IngeScapeUtils::getLicensesPath();

    _licensesPath = settings.value("directoryPath", QVariant(defaultLicensesPath)).toString();
    qDebug() << "Licenses path:" << _licensesPath;

    // Set the IngeScape license path
    igs_setLicensePath(_licensesPath.toStdString().c_str());

    settings.endGroup();


    // Directory for platform files
    QString platformPath = IngeScapeUtils::getPlatformsPath();

    QDir platformDir(platformPath);
    if (!platformDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << platformPath << "' !";
    }
    else {
        _platformDirectoryPath = platformPath;
    }


    //
    // Create sub-controllers
    //

    // Create the manager for the IngeScape data model
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if (ingeScapeModelManager == nullptr) {
        qCritical() << "IngeScape Model Manager is null !";
    }

    // Create the manager for the data model of our IngeScape Expe application
    _modelManager = new ExpeModelManager(this);

    // Create the controller to manage IngeScape network communications
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC == nullptr) {
        qCritical() << "IngeScape Network Controller is null !";
    }

    // Create the controller for network communications
    _networkC = new NetworkController(this);

    // Create the controller to manage IngeScape licenses
    //_licensesC = new LicensesController(this);

    // Connect to signals from network controllers
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsNotAvailable, this, &IngeScapeExpeController::_onNetworkDeviceIsNotAvailable);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsAvailableAgain, this, &IngeScapeExpeController::_onNetworkDeviceIsAvailableAgain);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIpAddressHasChanged, this, &IngeScapeExpeController::_onNetworkDeviceIpAddressHasChanged);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::editorEntered, _modelManager, &ExpeModelManager::onEditorEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::editorExited, _modelManager, &ExpeModelManager::onEditorExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderEntered, _modelManager, &ExpeModelManager::onRecorderEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderExited, _modelManager, &ExpeModelManager::onRecorderExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::peerOfAgentsEntered, ingeScapeModelManager, &IngeScapeModelManager::onPeerOfAgentsEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::peerOfAgentsExited, ingeScapeModelManager, &IngeScapeModelManager::onPeerOfAgentsExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::agentExited, ingeScapeModelManager, &IngeScapeModelManager::onAgentExited);
    //connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherEntered, ingeScapeModelManager, &IngeScapeModelManager::onLauncherEntered);
    //connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherExited, ingeScapeModelManager, &IngeScapeModelManager::onLauncherExited);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::definitionReceived, ingeScapeModelManager, &IngeScapeModelManager::onDefinitionReceived);

    connect(_networkC, &NetworkController::statusReceivedAbout_LoadPlatformFile, _modelManager, &ExpeModelManager::onStatusReceivedAbout_LoadPlatformFile);
    connect(_networkC, &NetworkController::timeLineStateUpdated, this, &IngeScapeExpeController::_onTimeLineStateUpdated);


    // Connect to OS events
    connect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeExpeController::_onSystemSleep);
    connect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeExpeController::_onSystemWake);
    connect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeExpeController::_onSystemNetworkConfigurationsUpdated);


    //
    // Start IngeScape
    //
    _startIngeScape(true);


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
    //QThread::msleep(2000);
}


/**
* @brief Destructor
*/
IngeScapeExpeController::~IngeScapeExpeController()
{
    // 1- Stop monitoring
    IngeScapeNetworkController::instance()->stopMonitoring();

    // 2- Stop IngeScape
    _stopIngeScape(false);

    // Unsubscribe to OS events
    disconnect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeExpeController::_onSystemSleep);
    disconnect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeExpeController::_onSystemWake);
    disconnect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeExpeController::_onSystemNetworkConfigurationsUpdated);


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

    // Free memory
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    disconnect(ingeScapeModelManager);
    // Do not call "delete" now, the destructor will be called automatically
    //delete ingeScapeModelManager;

    if (_networkC != nullptr)
    {
        disconnect(_networkC);

        NetworkController* temp = _networkC;
        setnetworkC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Free memory
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    disconnect(ingeScapeNetworkC);
    // Do not call "delete" now, the destructor will be called automatically
    //delete ingeScapeNetworkC;

    /*if (_licensesC != nullptr)
    {
        disconnect(_licensesC);

        LicensesController* temp = _licensesC;
        setlicensesC(nullptr);
        delete temp;
        temp = nullptr;
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
    if ((platform != nullptr) && (_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr))
    {
        qInfo() << "Open platform" << platform->name() << "at" << platform->filePath();

        QString message = QString("%1=%2").arg(command_LoadPlatformFile, platform->filePath());

        // Send the message "Load Platform File" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);
    }
}


/**
 * @brief Play the TimeLine and Start to record if flag "withRecord" is true
 * OR only resume the TimeLine
 * @param withRecord
 */
void IngeScapeExpeController::playOrResumeTimeLine(bool withRecord)
{
    if ((_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr)
            && (_modelManager->currentLoadedPlatform() != nullptr))
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

        QString message = QString("%1=%2").arg(command_UpdateTimeLineState, PLAY);

        // Send the message "Update TimeLine State" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);
    }
}


/**
 * @brief Pause the TimeLine
 */
void IngeScapeExpeController::pauseTimeLine()
{
    if ((_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr)
            && (_modelManager->currentLoadedPlatform() != nullptr))
    {
        qInfo() << "Pause the timeline of platform" << _modelManager->currentLoadedPlatform()->name();

        QString message = QString("%1=%2").arg(command_UpdateTimeLineState, PAUSE);

        // Send the message "Update TimeLine State" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);
    }
}


/**
 * @brief Stop the TimeLine
 */
void IngeScapeExpeController::stopTimeLine()
{
    if ((_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr)
            && (_modelManager->currentLoadedPlatform() != nullptr))
    {
        qInfo() << "STOP the timeline of platform" << _modelManager->currentLoadedPlatform()->name();

        QString message = QString("%1=%2").arg(command_UpdateTimeLineState, RESET);

        // Send the message "Update TimeLine State" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);


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
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeExpeController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}


/**
 * @brief Called when our network device is not available
 */
void IngeScapeExpeController::_onNetworkDeviceIsNotAvailable()
{
    qDebug() << Q_FUNC_INFO;

    // Stop IngeScape if needed
    if (IngeScapeNetworkController::instance()->isStarted())
    {
        _stopIngeScape(false);
    }
    // Else: our agent is not started, we don't need to stop it
}


/**
 * @brief Called when our network device is available again
 */
void IngeScapeExpeController::_onNetworkDeviceIsAvailableAgain()
{
    qDebug() << Q_FUNC_INFO;

    // Start IngeScape
    // => we don't need to check available network devices
    _startIngeScape(false);
}


/**
 * @brief Called when the IP address of our network device has changed
 */
void IngeScapeExpeController::_onNetworkDeviceIpAddressHasChanged()
{
    qDebug() << Q_FUNC_INFO;

    // Restart IngeScape
    // (Do not clear platform, do no check available network devices)
    _restartIngeScape(false, false);
}


/**
 * @brief Called when our machine will go to sleep
 */
void IngeScapeExpeController::_onSystemSleep()
{
    qDebug() << Q_FUNC_INFO;

    // Stop monitoring to save energy
    IngeScapeNetworkController::instance()->stopMonitoring();

    // Stop IngeScape
    _stopIngeScape(false);
}


/**
 * @brief Called when our machine did wake from sleep
 */
void IngeScapeExpeController::_onSystemWake()
{
    // Start IngeScape
    // => we need to check available network devices
    _startIngeScape(true);
}



/**
 * @brief Called when a network configuration is added, removed or changed
 */
void IngeScapeExpeController::_onSystemNetworkConfigurationsUpdated()
{
    IngeScapeNetworkController::instance()->updateAvailableNetworkDevices();
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


/**
 * @brief Start IngeScape
 *
 * @param checkAvailableNetworkDevices
 *
 * @return
 */
bool IngeScapeExpeController::_startIngeScape(bool checkAvailableNetworkDevices)
{
    bool success = false;

    // Reset the error message
    seterrorMessageWhenConnectionFailed("");

    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

    if ((ingeScapeNetworkC != nullptr) && (ingeScapeModelManager != nullptr))
    {
        if (checkAvailableNetworkDevices)
        {
            // Update the list of available network devices
            ingeScapeNetworkC->updateAvailableNetworkDevices();

            // There is only one available network device, we use it !
            if (ingeScapeNetworkC->availableNetworkDevices().count() == 1)
            {
                _networkDevice = ingeScapeNetworkC->availableNetworkDevices().at(0);
            }
        }

        // Start our IngeScape agent with the network device and the port
        success = ingeScapeNetworkC->start(_networkDevice, _ipAddress, _port);
    }

    if (!success && !_networkDevice.isEmpty())
    {
        seterrorMessageWhenConnectionFailed(tr("Failed to connect on network device %1 with port %2").arg(_networkDevice, QString::number(_port)));
    }

    return success;
}


/**
 * @brief Restart IngeScape
 *
 * @param hasToClearPlatform
 * @param checkAvailableNetworkDevices
 *
 * @return true if success
 */
bool IngeScapeExpeController::_restartIngeScape(bool hasToClearPlatform, bool checkAvailableNetworkDevices)
{
    if (hasToClearPlatform)
    {
        qInfo() << "Restart the network on" << _networkDevice << "with" << _port << "(and CLEAR the current platform)";
    }
    else
    {
        qInfo() << "Restart the network on" << _networkDevice << "with" << _port << "(and KEEP the current platform)";
    }

    // Stop IngeScape
    _stopIngeScape(hasToClearPlatform);

    // Start IngeScape
    return _startIngeScape(checkAvailableNetworkDevices);
}


/**
 * @brief Stop IngeScape
 *
 * @param hasToClearPlatform
 */
void IngeScapeExpeController::_stopIngeScape(bool hasToClearPlatform)
{
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

    if ((ingeScapeNetworkC != nullptr) && (ingeScapeModelManager != nullptr))
    {
        if (hasToClearPlatform)
        {
            qInfo() << "Stop the network on" << _networkDevice << "with" << _port << "(and CLEAR the current platform)";
        }
        else
        {
            qInfo() << "Stop the network on" << _networkDevice << "with" << _port << "(and KEEP the current platform)";
        }

        // Stop our IngeScape agent
        ingeScapeNetworkC->stop();

        // We don't see itself
        ingeScapeNetworkC->setnumberOfExpes(1);

        // Simulate an exit for each agent ON
        ingeScapeModelManager->simulateExitForEachAgentON();

        // Simulate an exit for each launcher
        ingeScapeModelManager->simulateExitForEachLauncher();

        // Simulate an exit for the recorder
        if ((_modelManager != nullptr) && _modelManager->isRecorderON() && (_modelManager->peerOfRecorder() != nullptr))
        {
            _modelManager->onRecorderExited(_modelManager->peerOfRecorder());
        }
    }
}


/**
 * @brief Start Recording
 */
void IngeScapeExpeController::_startRecording()
{
    if ((_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr)
            && (_modelManager->currentLoadedPlatform() != nullptr))
    {
        qInfo() << "Start recording the platform" << _modelManager->currentLoadedPlatform()->name();

        QString message = QString("%1=%2").arg(command_UpdateRecordState, START);

        // Send the message "Update Record State" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);

        // FIXME: get state from recorder messages
        _modelManager->currentLoadedPlatform()->setrecordState(RecordStates::RECORDING);
    }
}


/**
 * @brief Stop Recording
 */
void IngeScapeExpeController::_stopRecording()
{
    if ((_modelManager != nullptr) && (_networkC != nullptr)
            && _modelManager->isEditorON() && (_modelManager->peerOfEditor() != nullptr)
            && (_modelManager->currentLoadedPlatform() != nullptr))
    {
        qInfo() << "Stop recording the platform" << _modelManager->currentLoadedPlatform()->name();

        QString message = QString("%1=%2").arg(command_UpdateRecordState, STOP);

        // Send the message "Update Record State" to the editor
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_modelManager->peerOfEditor()->uid(), message);


        // FIXME: get state from recorder messages
        _modelManager->currentLoadedPlatform()->setrecordState(RecordStates::RECORDED);
    }
}


