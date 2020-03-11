/*
 *	IngeScape Assessments
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

#include "ingescapeassessmentscontroller.h"

#include <QApplication>

#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>
#include <platformsupport/osutils.h>


// Agent license id needed for the app to work
const char * IngeScapeAssessmentsController::AGENT_LICENSE_ID_NEEDED = "1A709C3100F94A0C9917E6E86F6B4477";


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeAssessmentsController::IngeScapeAssessmentsController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _licensesPath(""),
    _snapshotDirectory(""),
    _networkC(nullptr),
    _licensesC(nullptr),
    _experimentationsListC(nullptr),
    _experimentationC(nullptr),
    _subjectsC(nullptr),
    _protocolsC(nullptr),
    _exportC(nullptr),
    _terminationSignalWatcher(nullptr)
{
    qInfo() << "New IngeScape Assessments Controller";

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

    // Settings about the "Network"
    settings.beginGroup("network");

    // networkDevice saved is only checked if there is more than 1 network device available
    // in method _startIngescape
//    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
    _ipAddress = settings.value("ipAddress", QVariant("")).toString();
    _port = settings.value("port", QVariant(0)).toUInt();

    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port:" << QString::number(_port);

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


    //-------------------------------
    //
    // Create sub-controllers
    //
    //-------------------------------

    // Create the manager for the IngeScape data model
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if (ingeScapeModelManager == nullptr) {
        qCritical() << "IngeScape Model Manager is null !";
    }

    // Create the manager for the data model of our Assessments application
    AssessmentsModelManager* assessmentsModelManager = AssessmentsModelManager::instance();
    if (assessmentsModelManager == nullptr) {
        qCritical() << "Assessments Model Manager is null !";
    }

    // Create the controller to manage IngeScape network communications
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC == nullptr) {
        qCritical() << "IngeScape Network Controller is null !";
    }

    // Create the controller to manage network communications specific to our our Assessments application
    _networkC = new NetworkController(this);

    // Create the controller to manage IngeScape licenses
    _licensesC = new LicensesController(this);

    // Set assessment agent license ID in LicensesController so that it can handle when user does'nt have a valid license with this agent
    // TODO Uncomment when everyone is ready with their licenses ;)
    _licensesC->setNecessaryLicenseForAgent(globalAgent->agentName, AGENT_LICENSE_ID_NEEDED);

    // Create the controller to manage the list of experimentations
    _experimentationsListC = new ExperimentationsListController(this);

    // Create the controller to manage the current experimentation
    _experimentationC = new ExperimentationController(this);

    // Create the controller to manage the subjects of the current experimentation
    _subjectsC = new SubjectsController(this);

    // Create the controller to manage the protocols of the current experimentation
    _protocolsC = new ProtocolsController(this);

    // Create the controller to export data from the database
    _exportC = new ExportController(this);


    // Connect to signals from our licenses manager
    //connect(_licensesC, &LicensesController::licensesUpdated, this, &IngeScapeAssessmentsController::_onLicensesUpdated);


    // Connect to signals from network controllers
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsNotAvailable, this, &IngeScapeAssessmentsController::_onNetworkDeviceIsNotAvailable);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsAvailableAgain, this, &IngeScapeAssessmentsController::_onNetworkDeviceIsAvailableAgain);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIpAddressHasChanged, this, &IngeScapeAssessmentsController::_onNetworkDeviceIpAddressHasChanged);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::agentEntered, ingeScapeModelManager, &IngeScapeModelManager::onAgentEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::agentExited, ingeScapeModelManager, &IngeScapeModelManager::onAgentExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherEntered, ingeScapeModelManager, &IngeScapeModelManager::onLauncherEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherExited, ingeScapeModelManager, &IngeScapeModelManager::onLauncherExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderEntered, _experimentationC, &ExperimentationController::onRecorderEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderExited, _experimentationC, &ExperimentationController::onRecorderExited);

    connect(_networkC, &NetworkController::recordStartedReceived, _experimentationC, &ExperimentationController::onRecordStartedReceived);
    connect(_networkC, &NetworkController::recordStoppedReceived, _experimentationC, &ExperimentationController::onRecordStoppedReceived);
    connect(_networkC, &NetworkController::addedRecordReceived, _experimentationC, &ExperimentationController::onRecordAddedReceived);
    connect(_networkC, &NetworkController::deletedRecordReceived, _experimentationC, &ExperimentationController::onRecordDeletedReceived);
    connect(_networkC, &NetworkController::exportedRecordReceived, _experimentationC, &ExperimentationController::onExportedRecordReceived);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::definitionReceived, ingeScapeModelManager, &IngeScapeModelManager::onDefinitionReceived);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::mappingReceived, ingeScapeModelManager, &IngeScapeModelManager::onMappingReceived);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::valuePublished, ingeScapeModelManager, &IngeScapeModelManager::onValuePublished);


    // Connect to signals from model managers
    connect(ingeScapeModelManager, &IngeScapeModelManager::isMappingConnectedChanged, ingeScapeNetworkC, &IngeScapeNetworkController::onIsMappingConnectedChanged);

    connect(assessmentsModelManager, &AssessmentsModelManager::isConnectedToDatabaseChanged,
            this, &IngeScapeAssessmentsController::_onIsConnectedToDatabaseChanged);

    // Connect to signals from the experimentation controller
    connect(_experimentationC, &ExperimentationController::currentExperimentationChanged,
            this, &IngeScapeAssessmentsController::_onCurrentExperimentationChanged);


    // Connect to OS events
    connect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeAssessmentsController::_onSystemSleep);
    connect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeAssessmentsController::_onSystemWake);
    connect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeAssessmentsController::_onSystemNetworkConfigurationsUpdated);


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

}


/**
* @brief Destructor
*/
IngeScapeAssessmentsController::~IngeScapeAssessmentsController()
{
    // 1- Stop monitoring
    IngeScapeNetworkController::instance()->stopMonitoring();

    // 2- Stop IngeScape
    _stopIngeScape(false);

    // Unsubscribe to OS events
    disconnect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeAssessmentsController::_onSystemSleep);
    disconnect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeAssessmentsController::_onSystemWake);
    disconnect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeAssessmentsController::_onSystemNetworkConfigurationsUpdated);


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

    if (_experimentationsListC != nullptr)
    {
        disconnect(_experimentationsListC);

        ExperimentationsListController* temp = _experimentationsListC;
        setexperimentationsListC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_experimentationC != nullptr)
    {
        disconnect(_experimentationC);

        ExperimentationController* temp = _experimentationC;
        setexperimentationC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_subjectsC != nullptr)
    {
        disconnect(_subjectsC);

        SubjectsController* temp = _subjectsC;
        setsubjectsC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_protocolsC != nullptr)
    {
        disconnect(_protocolsC);

        ProtocolsController* temp = _protocolsC;
        setprotocolsC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_exportC != nullptr)
    {
        disconnect(_exportC);

        ExportController* temp = _exportC;
        setexportC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Free memory
    AssessmentsModelManager* assessmentsModelManager = AssessmentsModelManager::instance();
    disconnect(assessmentsModelManager);
    // Do not call "delete" now, the destructor will be called automatically
    //delete assessmentsModelManager;

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

    if (_licensesC != nullptr)
    {
        disconnect(_licensesC);

        LicensesController* temp = _licensesC;
        setlicensesC(nullptr);
        delete temp;
        temp = nullptr;
    }

    qInfo() << "Delete IngeScape Assessments Controller";
}


/**
* @brief Method used to provide a singleton to QML
* @param engine
* @param scriptEngine
* @return
*/
QObject* IngeScapeAssessmentsController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new IngeScapeAssessmentsController();
}


/**
 * @brief Actions to perform before the application closing
 */
void IngeScapeAssessmentsController::processBeforeClosing()
{
    // If user is recording, stop it
    if (_experimentationC->isRecording()) {
        _experimentationC->stopToRecord();
    }

    // FIXME TODO THEN ...
}


/**
 * @brief Re-Start the network with a port and a network device
 * @param strPort
 * @param networkDevice
 * @param hasToClearPlatform
 * @return true when success
 */
bool IngeScapeAssessmentsController::restartNetwork(QString strPort, QString networkDevice, bool hasToClearPlatform)
{
    bool success = false;

    bool isUInt = false;
    uint port = strPort.toUInt(&isUInt);
    if (isUInt && (port > 0))
    {
        // None changes (same port and same network device)
        if ((port == _port) && (networkDevice == _networkDevice))
        {
            // Nothing to do
            success = true;
        }
        // Port and Network device
        else
        {
            // Update properties
            setnetworkDevice(networkDevice);
            setport(port);

            // Update settings file
            IngeScapeSettings &settings = IngeScapeSettings::Instance();
            settings.beginGroup("network");
            settings.setValue("networkDevice", networkDevice);
            settings.setValue("port", port);
            settings.endGroup();

            // Save new values
            settings.sync();

            // Restart IngeScape
            success = _restartIngeScape(hasToClearPlatform, false);
        }
    }
    else {
        if (!isUInt)
        {
            qWarning() << "Port" << strPort << "is not an unsigned int !";
        }
        else if (port <= 0)
        {
            qWarning() << "Port" << strPort << "is negative or null !";
        }
    }

    return success;
}


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeAssessmentsController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}


/**
 * @brief Called when our network device is not available
 */
void IngeScapeAssessmentsController::_onNetworkDeviceIsNotAvailable()
{
    qDebug() << Q_FUNC_INFO;

    // Stop IngeScape if needed
    if (IngeScapeNetworkController::instance()->isStarted())
    {
        _stopIngeScape(false);

        // Try to relaunch editor with an available device
        _startIngeScape(true);
    }
    // Else: our agent is not started, we don't need to stop it
}


/**
 * @brief Called when our network device is available again
 */
void IngeScapeAssessmentsController::_onNetworkDeviceIsAvailableAgain()
{
    qDebug() << Q_FUNC_INFO;

    // Start IngeScape
    if (!IngeScapeNetworkController::instance()->isStarted())
    {
        _startIngeScape(true);
    }
}


/**
 * @brief Called when the IP address of our network device has changed
 */
void IngeScapeAssessmentsController::_onNetworkDeviceIpAddressHasChanged()
{
    qDebug() << Q_FUNC_INFO;

    // Restart IngeScape
    // (Do not clear platform, do no check available network devices)
    _restartIngeScape(false, false);
}


/**
 * @brief Called when our machine will go to sleep
 */
void IngeScapeAssessmentsController::_onSystemSleep()
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
void IngeScapeAssessmentsController::_onSystemWake()
{
    // Start IngeScape
    // => we need to check available network devices
    _startIngeScape(true);
}



/**
 * @brief Called when a network configuration is added, removed or changed
 */
void IngeScapeAssessmentsController::_onSystemNetworkConfigurationsUpdated()
{
    IngeScapeNetworkController::instance()->updateAvailableNetworkDevices();

    // If Ingescape is not started try to restarted if there is at least, one network device available
    // N.B: useful when last network device is always unaivalable and another network device become available
    if ((!IngeScapeNetworkController::instance()->isStarted())
            && (IngeScapeNetworkController::instance()->availableNetworkDevices().count() > 0)) {
        _startIngeScape(true);
    }
}


/**
 * @brief Slot called when the flag "Is Connected to Database" changed
 * @param isConnectedToDatabase
 */
void IngeScapeAssessmentsController::_onIsConnectedToDatabaseChanged(bool isConnectedToDatabase)
{
    // Databse connection is BACK
    if (isConnectedToDatabase)
    {
        qInfo("Database connection: NOT connected --> CONNECTED");

        if (_experimentationsListC != nullptr)
        {
            _experimentationsListC->updateWhenConnectedDatabase();
        }
    }
    // Databse connection is LOST
    else
    {
        qInfo("Database connection: CONNECTED --> NOT connected");

        if (_experimentationsListC != nullptr)
        {
            _experimentationsListC->updateWhenDISconnectedDatabase();
        }
    }
}


/**
 * @brief Slot called when the current experimentation changed
 * @param value
 */
void IngeScapeAssessmentsController::_onCurrentExperimentationChanged(ExperimentationM* currentExperimentation)
{
    if (currentExperimentation != nullptr)
    {
        qDebug() << "Current Experimentation = " << currentExperimentation->name();

        if (_subjectsC != nullptr) {
            _subjectsC->setcurrentExperimentation(currentExperimentation);
        }

        if (_protocolsC != nullptr) {
            _protocolsC->setcurrentExperimentation(currentExperimentation);
        }

        if (_exportC != nullptr) {
            _exportC->setcurrentExperimentation(currentExperimentation);
        }

    }
    else {
        qDebug() << "Current Experimentation is NULL !";
    }
}


/**
 * @brief If checkAvailableNetworkDevices : auto select a network device to start Ingescape
 */
bool IngeScapeAssessmentsController::_startIngeScape(bool checkAvailableNetworkDevices)
{
    bool success = false;
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    IngeScapeSettings &settings = IngeScapeSettings::Instance();

    // Always update available network devices (to have our qml list updated)
    ingeScapeNetworkC->updateAvailableNetworkDevices();

    if ((ingeScapeNetworkC != nullptr) && (ingeScapeModelManager != nullptr))
    {
        if (checkAvailableNetworkDevices)
        {
            int nbDevices = ingeScapeNetworkC->availableNetworkDevices().count();
            QStringList devicesAddresses = ingeScapeNetworkC->availableNetworkDevicesAddresses();
            if (nbDevices == 0 )
            {
                 setnetworkDevice("");
            }
            else if (nbDevices == 1)
            {
                // Use the only available network device
                setnetworkDevice(ingeScapeNetworkC->availableNetworkDevices().at(0));
            }
            else if ((nbDevices == 2)
                     && ((devicesAddresses.at(0) == "127.0.0.1") || (devicesAddresses.at(1) == "127.0.0.1")))
            {
                // 2 available devices, one is the loopback : we pick the device that is NOT the loopback
                if (devicesAddresses.at(0) == "127.0.0.1")
                {
                    setnetworkDevice(ingeScapeNetworkC->availableNetworkDevices().at(1));
                }
                else
                {
                    setnetworkDevice(ingeScapeNetworkC->availableNetworkDevices().at(0));
                }
            }
            else
            {
                // Several devices available : look last saving
                settings.beginGroup("network");
                QString lastSaveNetworkDevice = settings.value("networkDevice", QVariant("")).toString();
                settings.endGroup();
                if (!ingeScapeNetworkC->isAvailableNetworkDevice(lastSaveNetworkDevice))
                {
                    // User have to choose a device to launch editor
                    setnetworkDevice("");
                }
                else {
                    setnetworkDevice(lastSaveNetworkDevice);
                }
            }
        }
        success = ingeScapeNetworkC->start(_networkDevice, _ipAddress, _port); //will failed if networkDevice = ""
        if (success)
        {
            ingeScapeModelManager->setisMappingConnected(true); // Always enable mapping in assessment
        }
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
bool IngeScapeAssessmentsController::_restartIngeScape(bool hasToClearPlatform, bool checkAvailableNetworkDevices)
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
void IngeScapeAssessmentsController::_stopIngeScape(bool hasToClearPlatform)
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

        // Disable mapping
        ingeScapeModelManager->setisMappingConnected(false);

        // Stop our IngeScape agent
        ingeScapeNetworkC->stop();

        // We don't see itself
        ingeScapeNetworkC->setnumberOfAssessments(1);

        // Simulate an exit for each agent ON
        ingeScapeModelManager->simulateExitForEachAgentON();

        // Simulate an exit for each launcher
        ingeScapeModelManager->simulateExitForEachLauncher();

        // Simulate an exit for the recorder
        if ((_experimentationC != nullptr) && _experimentationC->isRecorderON())
        {
            _experimentationC->onRecorderExited(_experimentationC->peerIdOfRecorder(), _experimentationC->peerNameOfRecorder());
        }
    }
}


