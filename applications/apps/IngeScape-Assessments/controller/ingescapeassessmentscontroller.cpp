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

#include <controller/assessmentsmodelmanager.h>
#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeAssessmentsController::IngeScapeAssessmentsController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _licensesPath(""),
    _errorMessageWhenConnectionFailed(""),
    _snapshotDirectory(""),
    _networkC(nullptr),
    _licensesC(nullptr),
    _experimentationsListC(nullptr),
    _experimentationC(nullptr),
    _subjectsC(nullptr),
    _tasksC(nullptr),
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

    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
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

    // Create the controller to manage the list of experimentations
    _experimentationsListC = new ExperimentationsListController(this);

    // Create the controller to manage the current experimentation
    _experimentationC = new ExperimentationController(this);

    // Create the controller to manage the subjects of the current experimentation
    _subjectsC = new SubjectsController(this);

    // Create the controller to manage the tasks of the current experimentation
    _tasksC = new TasksController(this);

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


    // Update the list of available network devices
    _networkC->updateAvailableNetworkDevices();

    // There is only one available network device, we use it !
    if (_networkC->availableNetworkDevices().count() == 1) {
        _networkDevice = _networkC->availableNetworkDevices().at(0);
    }

    // Start our INGESCAPE agent with a network device (or an IP address) and a port
    bool isStarted = _networkC->start(_networkDevice, _ipAddress, _port);

    if (isStarted && (_modelManager != nullptr))
    {
        // Initialize platform from online mapping
        _modelManager->setisMappingConnected(true);
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

}


/**
* @brief Destructor
*/
IngeScapeAssessmentsController::~IngeScapeAssessmentsController()
{
    // Unsubscribe to OS events
    //disconnect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeAssessmentsController::_onSystemSleep);
    //disconnect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeAssessmentsController::_onSystemWake);
    //disconnect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeAssessmentsController::_onSystemNetworkConfigurationsUpdated);

    // Unsubscribe to our application
    /*if (IngescapeApplication::instance() != nullptr)
    {
        disconnect(IngescapeApplication::instance(), &IngescapeApplication::openFileRequest, this, &IngeScapeAssessmentsController::_onOpenFileRequest);
    }*/


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

    if (_tasksC != nullptr)
    {
        disconnect(_tasksC);

        TasksController* temp = _tasksC;
        settasksC(nullptr);
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

    AssessmentsModelManager::destroyInstance();

    if (_networkC != nullptr)
    {
        disconnect(_networkC);

        NetworkController* temp = _networkC;
        setnetworkC(nullptr);
        delete temp;
        temp = nullptr;
    }

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
    // FXME TODO...
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
    Q_UNUSED(hasToClearPlatform)

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
            success = _restartIngeScape();
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
    // => we don't need to check available network devices
    _startIngeScape(false);
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

        if (_tasksC != nullptr) {
            _tasksC->setcurrentExperimentation(currentExperimentation);
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
 * @brief Restart IngeScape
 * @return true if success
 */
bool IngeScapeAssessmentsController::_restartIngeScape()
{
    bool success = false;

    // Reset the error message
    seterrorMessageWhenConnectionFailed("");

    if ((_networkC != nullptr) && (_modelManager != nullptr))
    {
        qInfo() << "Restart the network on" << _networkDevice << "with" << _port;

        _modelManager->setisMappingConnected(false);
        //_modelManager->setisMappingControlled(false);

        // Stop our IngeScape agent
        _networkC->stop();

        // Simulate an exit for each agent ON
        //modelManager->simulateExitForEachAgentON();

        // Simulate an exit for each launcher
        //modelManager->simulateExitForEachLauncher();

        // Start our IngeScape agent with the network device and the port
        success = _networkC->start(_networkDevice, "", _port);

        if (success) {
            _modelManager->setisMappingConnected(true);
        }
    }

    if (!success) {
        seterrorMessageWhenConnectionFailed(tr("Failed to connect with network device %1 on port %2").arg(_networkDevice, QString::number(_port)));
    }

    return success;
}

