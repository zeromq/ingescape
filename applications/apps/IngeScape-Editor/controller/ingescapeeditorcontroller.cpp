/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#include "controller/ingescapeeditorcontroller.h"

#include <I2Quick.h>
#include <QFileDialog>
#include <QThread>
#include <QApplication>
#include <QCursor>
#include <QCommandLineParser>

#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>
#include <platformsupport/osutils.h>
#include <platformsupport/IngescapeApplication.h>
#include <controller/ingescapenetworkcontroller.h>


// Name of the example platform to load when no other platform has been loaded yet
const QString IngeScapeEditorController::EXAMPLE_PLATFORM_NAME = "example";

// Default name when creating a new platform
const QString IngeScapeEditorController::NEW_PLATFORM_NAME = "New Platform";

// Default name when creating a new platform
const QString IngeScapeEditorController::SPECIAL_EMPTY_LAST_PLATFORM = "empty";


/**
 * @brief Constructor
 */
IngeScapeEditorController::IngeScapeEditorController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _editorShouldBeOnlineAndImposeMappingAtLaunch(false),
    _isAvailableModelVisualizer(false),
    _isVisibleModelVisualizer(false),
    _snapshotDirectory(""),
    _modelManager(nullptr),
    _networkC(nullptr),
    _agentsSupervisionC(nullptr),
    _callHomeC(nullptr),
    _agentsMappingC(nullptr),
    _scenarioC(nullptr),
    _valuesHistoryC(nullptr),
    _licensesC(nullptr),
    _timeLineC(nullptr),
    _peerOfExpe(nullptr),
    _currentPlatformName(EXAMPLE_PLATFORM_NAME),
    _hasAPlatformBeenLoadedByUser(false),
    _gettingStartedShowAtStartup(true),
    _platformNameBeforeLoadReplay(""),
    _terminationSignalWatcher(nullptr),
    _platformDirectoryPath(""),
    _currentPlatformFilePath("")
{
    qInfo() << "New IngeScape Editor Controller";

    // Root directory path
    QString rootPath = IngeScapeUtils::getRootPath();
    QDir rootDir(rootPath);
    if (!rootDir.exists())
    {
        qCritical() << "ERROR: could not create directory at '" << rootPath << "' !";
    }

    // Snapshots directory path
    QString snapshotsPath = IngeScapeUtils::getSnapshotsPath();
    QDir snapshotsDirectory(snapshotsPath);
    if (snapshotsDirectory.exists())
    {
        _snapshotDirectory = snapshotsPath;
    }
    else
    {
        qCritical() << "ERROR: could not create directory at '" << snapshotsPath << "' !";
    }

    // Path to the example.igsPlatform
    QString platformDefaultFilePath;

    // Directory for platform files
    QString platformPath = IngeScapeUtils::getPlatformsPath();

    QDir platformDir(platformPath);
    if (!platformDir.exists())
    {
        qCritical() << "ERROR: could not create directory at '" << platformPath << "' !";
    }
    else
    {
        _platformDirectoryPath = platformPath;

        // Init the path to the platform file to load the example file
        platformDefaultFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, EXAMPLE_PLATFORM_NAME);
    }

    // Create the (sub) directory "exports" if not exist (the directory contains CSV files about exports)
    IngeScapeUtils::getExportsPath();


    //------------------
    //
    // Settings
    //
    //------------------
    IngeScapeSettings &settings = IngeScapeSettings::Instance();

    // NETWORK settings
    settings.beginGroup("network");
    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
    _ipAddress = settings.value("ipAddress", QVariant("")).toString();
    _port = settings.value("port", QVariant(5670)).toUInt();



    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);

    bool wasAgentEditorStarted = settings.value("connected", true).toBool();
    settings.endGroup();

    // MAPPING settings
    settings.beginGroup("mapping");
    bool wasMappingImposed = settings.value("imposedToNetwork", true).toBool();
    settings.endGroup();

    // DEBUG settings
    settings.beginGroup("debug");
#ifdef QT_DEBUG
    _isAvailableModelVisualizer = true;
#else
    _isAvailableModelVisualizer = settings.value("modelVisualizer", QVariant(false)).toBool();
#endif
    qDebug() << "The Model/View Model Visualizer is available ?" << _isAvailableModelVisualizer;
    settings.endGroup();

    // PLATFORM settings
    settings.beginGroup("platform");
    _currentPlatformFilePath = settings.value("last", platformDefaultFilePath).toString();
    settings.endGroup();

    // HELP settings
    settings.beginGroup("help");
    _gettingStartedShowAtStartup = settings.value("showAtStartup", true).toBool();
    settings.endGroup();

    //-------------------------------
    //
    // Command line options
    //
    //-------------------------------

    // Define command line options
    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription("IngeScape Editor");
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption();
    commandLineParser.addPositionalArgument("file", tr("File to open"));

    // Process command line options
    commandLineParser.process((qApp != nullptr) ? qApp->arguments() : QStringList());

    // Check if we have a file
    // NB: On Windows, we will not receive DDE messages until our main window is displayed
    //     that's why we must handle a command line argument
    // NB: On linux, we only rely on this command line argument
    QStringList positionalArguments = commandLineParser.positionalArguments();
    if ((positionalArguments.count() > 0) && (IngescapeApplication::instance() != nullptr))
    {
        QString filePath = positionalArguments.at(0);
        IngescapeApplication::instance()->addPendingOpenFileRequest(QUrl::fromLocalFile(filePath), filePath);
    }


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

    // FIXME TODO: EditorModelManager* editorModelManager = EditorModelManager::instance();
    // Create the manager for the data model of our IngeScape Editor application
    _modelManager = new EditorModelManager(this);

    // Create the controller to manage IngeScape network communications
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    if (ingeScapeNetworkC == nullptr) {
        qCritical() << "IngeScape Network Controller is null !";
    }

    // Create the controller to manage network communications specific to our our Editor application
    _networkC = new NetworkController(this);

    // Create the controller to manage IngeScape licenses
    _licensesC = new LicensesController(this);

    // Create the controller to manage the agents list
    _agentsSupervisionC = new AgentsSupervisionController(this);

    // Create the controller to manage hosts
    _hostsSupervisionC = new HostsSupervisionController(this);

    // Create the controller for records supervision
    _recordsSupervisionC = new RecordsSupervisionController(this);

    // Create the controller to manage the agents mapping
    _agentsMappingC = new AgentsMappingController(this);

    // Create the controller to manage the call home at startup
    _callHomeC = new CallHomeController(this);

    // Create the controller to manage the scenario
    _scenarioC = new ScenarioController(this);

    // Create the controller to manage the history of values
    _valuesHistoryC = new ValuesHistoryController(this);

    // Create the controller to manage the time line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);


    // Connect to signals from our licenses manager
    connect(_licensesC, &LicensesController::licensesUpdated, this, &IngeScapeEditorController::_onLicensesUpdated);
    connect(_licensesC, &LicensesController::licenseLimitationReached, this, &IngeScapeEditorController::_onLicenseLimitationReached);

    // Connect to signals from network controllers
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::isStartedChanged, this, &IngeScapeEditorController::_onAgentEditorStartedOrStopped);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsNotAvailable, this, &IngeScapeEditorController::_onNetworkDeviceIsNotAvailable);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIsAvailableAgain, this, &IngeScapeEditorController::_onNetworkDeviceIsAvailableAgain);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::networkDeviceIpAddressHasChanged, this, &IngeScapeEditorController::_onNetworkDeviceIpAddressHasChanged);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::peerOfAgentsEntered, ingeScapeModelManager, &IngeScapeModelManager::onPeerOfAgentsEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::peerOfAgentsExited, ingeScapeModelManager, &IngeScapeModelManager::onPeerOfAgentsExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::agentExited, ingeScapeModelManager, &IngeScapeModelManager::onAgentExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherEntered, ingeScapeModelManager, &IngeScapeModelManager::onLauncherEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::launcherExited, ingeScapeModelManager, &IngeScapeModelManager::onLauncherExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderEntered, _recordsSupervisionC, &RecordsSupervisionController::onRecorderEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::recorderExited, _recordsSupervisionC, &RecordsSupervisionController::onRecorderExited);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::expeEntered, this, &IngeScapeEditorController::_onExpeEntered);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::expeExited, this, &IngeScapeEditorController::_onExpeExited);

    connect(ingeScapeNetworkC, &IngeScapeNetworkController::definitionReceived, ingeScapeModelManager, &IngeScapeModelManager::onDefinitionReceived);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::mappingReceived, ingeScapeModelManager, &IngeScapeModelManager::onMappingReceived);
    connect(ingeScapeNetworkC, &IngeScapeNetworkController::valuePublished, ingeScapeModelManager, &IngeScapeModelManager::onValuePublished);

    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &EditorModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::isFrozenFromAgentUpdated, _modelManager, &EditorModelManager::onIsFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isMutedFromOutputOfAgentUpdated, _modelManager, &EditorModelManager::onIsMutedFromOutputOfAgentUpdated);
    connect(_networkC, &NetworkController::agentStateChanged, _modelManager, &EditorModelManager::onAgentStateChanged);
    connect(_networkC, &NetworkController::agentHasLogInStream, _modelManager, &EditorModelManager::onAgentHasLogInStream);
    connect(_networkC, &NetworkController::agentHasLogInFile, _modelManager, &EditorModelManager::onAgentHasLogInFile);
    connect(_networkC, &NetworkController::agentLogFilePath, _modelManager, &EditorModelManager::onAgentLogFilePath);
    connect(_networkC, &NetworkController::agentDefinitionFilePath, _modelManager, &EditorModelManager::onAgentDefinitionFilePath);
    connect(_networkC, &NetworkController::agentMappingFilePath, _modelManager, &EditorModelManager::onAgentMappingFilePath);

    connect(_networkC, &NetworkController::allRecordsReceived, _recordsSupervisionC, &RecordsSupervisionController::onAllRecordsReceived);
    connect(_networkC, &NetworkController::recordStartedReceived, _recordsSupervisionC, &RecordsSupervisionController::onRecordStartedReceived);
    connect(_networkC, &NetworkController::recordStoppedReceived, _recordsSupervisionC, &RecordsSupervisionController::onRecordStoppedReceived);
    connect(_networkC, &NetworkController::addedRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onAddedRecord);
    connect(_networkC, &NetworkController::deletedRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onDeletedRecord);
    connect(_networkC, &NetworkController::replayLoadingReceived, this, &IngeScapeEditorController::_onReplayLoading);
    connect(_networkC, &NetworkController::replayLoadedReceived, _recordsSupervisionC, &RecordsSupervisionController::onReplayLoaded);
    connect(_networkC, &NetworkController::replayUNloadedReceived, _recordsSupervisionC, &RecordsSupervisionController::onReplayUNloaded);
    connect(_networkC, &NetworkController::replayUNloadedReceived, this, &IngeScapeEditorController::_onReplayUNloaded);
    connect(_networkC, &NetworkController::replayEndedReceived, _recordsSupervisionC, &RecordsSupervisionController::onReplayEnded);
    connect(_networkC, &NetworkController::recordExported, _recordsSupervisionC, &RecordsSupervisionController::onRecordExported);

    connect(_networkC, &NetworkController::runAction, _scenarioC, &ScenarioController::onRunAction);
    connect(_networkC, &NetworkController::loadPlatformFileFromPath, this, &IngeScapeEditorController::_onLoadPlatformFileFromPath);
    connect(_networkC, &NetworkController::updateTimeLineState, this, &IngeScapeEditorController::_onUpdateTimeLineState);
    connect(_networkC, &NetworkController::updateRecordState, this, &IngeScapeEditorController::_onUpdateRecordState);
    connect(_networkC, &NetworkController::highlightLink, _agentsMappingC, &AgentsMappingController::onHighlightLink);


    // Connect to signals from model managers
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onAgentModelHasBeenCreatedORonAgentModelBackOnNetwork);
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onAgentModelWillBeDeleted);

    connect(ingeScapeModelManager, &IngeScapeModelManager::agentModelBackOnNetwork, _hostsSupervisionC, &HostsSupervisionController::onAgentModelHasBeenCreatedORonAgentModelBackOnNetwork);

    connect(ingeScapeModelManager, &IngeScapeModelManager::hostModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onHostModelHasBeenCreated);
    connect(ingeScapeModelManager, &IngeScapeModelManager::hostModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onHostModelWillBeDeleted);
    connect(ingeScapeModelManager, &IngeScapeModelManager::previousHostParsed, _hostsSupervisionC, &HostsSupervisionController::onPreviousHostParsed);

    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByNameHasBeenCreated, _modelManager, &EditorModelManager::onAgentsGroupedByNameHasBeenCreated);
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByNameHasBeenCreated, _valuesHistoryC, &ValuesHistoryController::onAgentsGroupedByNameHasBeenCreated);
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByNameHasBeenCreated, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameHasBeenCreated);
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByNameWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameWillBeDeleted);

    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByDefinitionHasBeenCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionHasBeenCreated);
    connect(ingeScapeModelManager, &IngeScapeModelManager::agentsGroupedByDefinitionWillBeDeleted, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionWillBeDeleted);

    connect(ingeScapeModelManager, &IngeScapeModelManager::actionModelWillBeDeleted, _agentsMappingC, &AgentsMappingController::onActionModelWillBeDeleted);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::openValuesHistoryOfAgent, _valuesHistoryC, &ValuesHistoryController::filterValuesToShowOnlyAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openLogStreamOfAgents, this, &IngeScapeEditorController::_onOpenLogStreamOfAgents);


    // Connect to signals from the controller for mapping of agents
    connect(_agentsMappingC, &AgentsMappingController::executeAction, _scenarioC, &ScenarioController::onExecuteAction);

    // Connect to signals from the controller of the scenario
    connect(_scenarioC, &ScenarioController::actionWillBeExecuted, this, &IngeScapeEditorController::_onActionWillBeExecuted);
    connect(_scenarioC, &ScenarioController::timeLineStateUpdated, this, &IngeScapeEditorController::_onTimeLineStateUpdated);

    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::onTimeRangeChanged);

    // Connect to signals from Record supervision controller
    connect(_recordsSupervisionC, &RecordsSupervisionController::startToRecord, this, &IngeScapeEditorController::_onStartToRecord);


    // Connect to OS events
    connect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeEditorController::_onSystemSleep);
    connect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeEditorController::_onSystemWake);
    connect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeEditorController::_onSystemNetworkConfigurationsUpdated);


    if (_isAvailableModelVisualizer)
    {
        // Emit the signal "Previous Host Parsed" to create the fake host "HOSTNAME_NOT_DEFINED"
        Q_EMIT ingeScapeModelManager->previousHostParsed(HOSTNAME_NOT_DEFINED);
    }


    // Check if there is a pending "open file" request
    // NB: We must perform this check because IngeScapeEditorController is created asynchronously
    bool hasPendingOpenFileRequest = false;
    QUrl pendingOpenFileRequestUrl;
    QString pendingOpenFileRequestFilePath;
    if (IngescapeApplication::instance() != nullptr)
    {
        // Check if there is a pending "open file" request
        // NB: We must perform this check because IngeScapeEditorController is created asynchronously
        if (IngescapeApplication::instance()->hasPendingOpenFileRequest())
        {
            QPair<QUrl, QString> pendingRequest = IngescapeApplication::instance()->getPendingOpenFileRequest();
            hasPendingOpenFileRequest = true;
            pendingOpenFileRequestUrl = pendingRequest.first;
            pendingOpenFileRequestFilePath = pendingRequest.second;

            QString scheme = pendingOpenFileRequestUrl.scheme();
            if (QString::compare(scheme, QStringLiteral("file"), Qt::CaseSensitive) == 0)
            {
                QFileInfo fileInfo(pendingRequest.second);
                if (
                    (QString::compare(fileInfo.suffix(), QStringLiteral("igsplatform")) == 0)
                    ||
                    // Our application may be launched with a .json file as command line argument
                    // We assume that this .json file is an IGS platform file
                    (QString::compare(fileInfo.suffix(), QStringLiteral("json")) == 0)
                   )
                {
                    _currentPlatformFilePath = pendingOpenFileRequestFilePath;

                    // Request will be handled by _loadPlatformFromFile()
                    hasPendingOpenFileRequest = false;
                }
            }
            // Else: scheme is not supported
        }
    }

    
    //
    // Load our platform file if needed
    //
    if (!_currentPlatformFilePath.isEmpty())
    {
        if (_currentPlatformFilePath == SPECIAL_EMPTY_LAST_PLATFORM)
        {
            qWarning() << "There is no 'last' platform to load !";
            _currentPlatformFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, NEW_PLATFORM_NAME);
        }
        else
        {
            // Load the platform (agents, mappings, actions, palette, timeline actions)
            // from the last opened platform (saved in the settings)
            bool success = _loadPlatformFromFile(_currentPlatformFilePath);

            if (!success)
            {
                qCritical() << "The loading of the last platform failed !";
                _currentPlatformFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, NEW_PLATFORM_NAME);
            }
            else
            {
                sethasAPlatformBeenLoadedByUser(true);
            }
        }
    }

    //
    // Application
    //
    if (IngescapeApplication::instance() != nullptr)
    {
        // Check if there is a pending "open file" request (agent(s) definition (.igsdefinition) or license file (.igslicense))
        if (hasPendingOpenFileRequest)
        {
            _onOpenFileRequest(pendingOpenFileRequestUrl, pendingOpenFileRequestFilePath);
        }

        // Subscribe to our application
        connect(IngescapeApplication::instance(), &IngescapeApplication::openFileRequest, this, &IngeScapeEditorController::_onOpenFileRequest);
    }


    //
    // Editor ONLINE/OFFLINE, mapping imposed/NOT imposed at launch, open popup to warn user ...
    //

    // Configure impose/not impose mapping
    _agentsMappingC->setimposeMappingToAgentsON(wasMappingImposed);

    // Network device choice handle
    ingeScapeNetworkC->updateAvailableNetworkDevices();
    if (_networkDevice.isEmpty())
    {
        // User don't choose a network device last time (or it is first launch) ...
        // Try to auto select one network device
        setnetworkDevice(_autoFindAnAvailableDevice());
    }

    if (ingeScapeNetworkC->isAvailableNetworkDevice(_networkDevice))
    {
        // Start monitoring to detect events relative to our network config
        ingeScapeNetworkC->startMonitoring(_networkDevice, _port);

        // Connect editor if necessary
        if (wasAgentEditorStarted && ingeScapeNetworkC->isAvailableNetworkDevice(_networkDevice))
        {
            // User was ONLINE last time (or it is first launch)
            // N.B. : TODO FIXME : check first launch with existing .ini file is not the final solution
            //        Maybe, we can use this popup when agents launch on network are radically different from our platform/there is no agents launched on network ?
            if (_agentsMappingC->imposeMappingToAgentsON() && (!settings.areDefaultSettings()))
            {
                // Mapping was imposed, we verify with user
                seteditorShouldBeOnlineAndImposeMappingAtLaunch(true);
            }
            else
            {
                // Mapping was NOT imposed, we can start ingescape
                startIngeScape();
            }
        }
    }
    else
    {
        // Reset network device : user can't connect editor with this network device
        setnetworkDevice("");
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
IngeScapeEditorController::~IngeScapeEditorController()
{
    // 1- Stop monitoring
    IngeScapeNetworkController::instance()->stopMonitoring();

    // 2- Stop IngeScape
    stopIngeScape();

    // Unsubscribe to OS events
    disconnect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeEditorController::_onSystemSleep);
    disconnect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeEditorController::_onSystemWake);
    disconnect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeEditorController::_onSystemNetworkConfigurationsUpdated);

    // Unsubscribe to our application
    if (IngescapeApplication::instance() != nullptr)
    {
        disconnect(IngescapeApplication::instance(), &IngescapeApplication::openFileRequest, this, &IngeScapeEditorController::_onOpenFileRequest);
    }


    // Delete all log stream viewers
    _openedLogStreamControllers.deleteAllItems();

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
    if (_timeLineC != nullptr)
    {
        disconnect(_timeLineC);
        AbstractTimeActionslineScenarioViewController* temp = _timeLineC;
        settimeLineC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_valuesHistoryC != nullptr)
    {
        disconnect(_valuesHistoryC);
        ValuesHistoryController* temp = _valuesHistoryC;
        setvaluesHistoryC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_agentsMappingC != nullptr)
    {
        disconnect(_agentsMappingC);
        AgentsMappingController* temp = _agentsMappingC;
        setagentsMappingC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_callHomeC != nullptr)
    {
        disconnect(_callHomeC);
        CallHomeController* temp = _callHomeC;
        setcallHomeC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_agentsSupervisionC != nullptr)
    {
        disconnect(_agentsSupervisionC);
        AgentsSupervisionController* temp = _agentsSupervisionC;
        setagentsSupervisionC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_scenarioC != nullptr)
    {
        disconnect(_scenarioC);
        ScenarioController* temp = _scenarioC;
        setscenarioC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_modelManager != nullptr)
    {
        disconnect(_modelManager);
        EditorModelManager* temp = _modelManager;
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

    if (_licensesC != nullptr)
    {
        disconnect(_licensesC);
        LicensesController* temp = _licensesC;
        setlicensesC(nullptr);
        delete temp;
        temp = nullptr;
    }

    qInfo() << "Delete IngeScape Editor Controller";
}


/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* IngeScapeEditorController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new IngeScapeEditorController();
}


void IngeScapeEditorController::setnetworkDevice(QString value)
{
    if (value != _networkDevice)
    {
        _networkDevice = value;

        // Restart monitoring to detect events relative to our new network config
        if (!_networkDevice.isEmpty())
        {
            IngeScapeNetworkController::instance()->stopMonitoring();
            IngeScapeNetworkController::instance()->startMonitoring(_networkDevice, _port);
        }

        Q_EMIT networkDeviceChanged(value);
    }
}

void IngeScapeEditorController::setport(uint value)
{
    if (value != _port)
    {
        _port = value;

        // Restart monitoring to detect events relative to our new network config
        if (!_networkDevice.isEmpty())
        {
            IngeScapeNetworkController::instance()->stopMonitoring();
            IngeScapeNetworkController::instance()->startMonitoring(_networkDevice, _port);
        }

        Q_EMIT portChanged(value);
    }
}

/**
 * @brief Custom setter for the gettingStartedShowAtStartup property.
 * Save the new value to INI file immediatly
 * @param value
 */
void IngeScapeEditorController::setgettingStartedShowAtStartup(bool value)
{
    if (value != _gettingStartedShowAtStartup)
    {
        _gettingStartedShowAtStartup = value;

        IngeScapeSettings &settings = IngeScapeSettings::Instance();
        settings.beginGroup("help");
        // Clear the value if we close with an unsaved new platform
        settings.setValue("showAtStartup", value);
        settings.endGroup();

        // Save new values
        settings.sync();

        Q_EMIT gettingStartedShowAtStartupChanged(value);
    }
}


/**
 * @brief Load a platform (agents, mappings, actions, palette, timeline actions)
 * from the file selected by the user
 */
void IngeScapeEditorController::loadPlatformFromSelectedFile()
{
    // "File Dialog" to get the files (paths) to open
    QString platformFilePath = QFileDialog::getOpenFileName(nullptr,
                                                            tr("Open platform"),
                                                            _platformDirectoryPath,
                                                            tr("Platform (*.igsplatform *.json)"));
    if (!platformFilePath.isEmpty())
    {
        if (IngeScapeNetworkController::instance()->isStarted() && (_modelManager != nullptr))
        {
            // if we are connected, we don't impose mapping to the network anymore
            _agentsMappingC->setimposeMappingToAgentsON(false);
        }
        _clearAndLoadPlatformFromFile(platformFilePath);
    }
    else
    {
        qDebug() << Q_FUNC_INFO << ":Platform file path is empty, nothing to do";
    }
}


/**
 * @brief Save the platform (agents, mappings, actions, palette, timeline actions)
 * to the file selected by the user
 */
void IngeScapeEditorController::selectFileToSavePlatform()
{
        QString platformFilePath = QFileDialog::getSaveFileName(nullptr,
                                                                tr("Save platform"),
                                                                _currentPlatformFilePath,
                                                                tr("Platform (*.igsplatform)")
                                                                );

        if (!platformFilePath.isEmpty()) {
            // Save the platform to JSON file
            _savePlatformToFile(platformFilePath);
        }
}


/**
 * @brief Save the current platform to the last loaded platform file
 */
void IngeScapeEditorController::savePlatformToCurrentlyLoadedFile()
{
    if (!_hasAPlatformBeenLoadedByUser || (_currentPlatformName == EXAMPLE_PLATFORM_NAME))
    {
        // No platform was loaded. Cannot save to current file.
        selectFileToSavePlatform();
    }
    else
    {
        // Save the platform to JSON file
        _savePlatformToFile(_currentPlatformFilePath);
    }
}


/**
 * @brief Clear the current platform (agents, mappings, actions, palette, timeline actions, hosts)
 * by deleting all existing data
 */
void IngeScapeEditorController::createNewPlatform()
{
    if (IngeScapeNetworkController::instance()->isStarted() && (_modelManager != nullptr))
    {
        // if we are connected, we don't impose mapping to the network anymore
        _agentsMappingC->setimposeMappingToAgentsON(false);
    }

    // Update the current platform name
    setcurrentPlatformName(NEW_PLATFORM_NAME);
    _currentPlatformFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, NEW_PLATFORM_NAME);
    sethasAPlatformBeenLoadedByUser(false);

    _clearCurrentPlatform();
}


/**
 * @brief Actions to perform before the application closing
 */
void IngeScapeEditorController::processBeforeClosing()
{
    // Save settings from the app
    IngeScapeSettings &settings = IngeScapeSettings::Instance();

    settings.beginGroup("network");
    settings.setValue("connected", IngeScapeNetworkController::instance()->isStarted());
    settings.setValue("networkDevice", _networkDevice);
    settings.setValue("ipAddress", _ipAddress);
    settings.setValue("port", _port);
    settings.endGroup();

    settings.beginGroup("mapping");
    settings.setValue("imposedToNetwork", _agentsMappingC->imposeMappingToAgentsON());
    settings.endGroup();

    settings.beginGroup("platform");
    // Clear the value if we close with an unsaved new platform
    settings.setValue("last", _hasAPlatformBeenLoadedByUser ? _currentPlatformFilePath : SPECIAL_EMPTY_LAST_PLATFORM);
    settings.endGroup();

    // Save new values
    settings.sync();
}


/**
  * @brief Check if an agents grouped by definition is used in the current platform
  *        In the mapping or/and in the scenario (actions, conditions, effects)
  * @param agentsGroupedByDefinition
  */
bool IngeScapeEditorController::isAgentUsedInPlatform(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    bool isUsed = false;

    if ((agentsGroupedByDefinition != nullptr) && !agentsGroupedByDefinition->name().isEmpty()
            && (_agentsMappingC != nullptr) && (_scenarioC != nullptr))
    {
        QString agentName = agentsGroupedByDefinition->name();

        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = IngeScapeModelManager::instance()->getAgentsGroupedForName(agentName);
        if (agentsGroupedByName != nullptr)
        {
            // It is the last agents grouped by definition
            if (agentsGroupedByName->allAgentsGroupsByDefinition()->count() == 1)
            {
                // Check if the agent is in the curent mapping
                if (_agentsMappingC->getAgentInMappingFromName(agentName) != nullptr)
                {
                    isUsed = true;
                }

                if (!isUsed)
                {
                    // Check if the agent is used in the current scenario (actions, conditions, effects)
                    isUsed = _scenarioC->isAgentUsedInScenario(agentName);
                }
            }
        }
    }
    return isUsed;
}


/**
 * @brief Close a definition
 * @param definition
 */
void IngeScapeEditorController::closeDefinition(DefinitionM* definition)
{
    if ((definition != nullptr) && (_modelManager != nullptr) && _modelManager->openedDefinitions()->contains(definition))
    {
        _modelManager->openedDefinitions()->remove(definition);
    }
}


/**
 * @brief Close an action editor
 * @param actionEditorC
 */
void IngeScapeEditorController::closeActionEditor(ActionEditorController* actionEditorC)
{
    if ((actionEditorC != nullptr) && (_scenarioC != nullptr)) {
        _scenarioC->closeActionEditor(actionEditorC);
    }
}


/**
 * @brief Close a "Log Stream" controller
 * @param logStreamC
 */
void IngeScapeEditorController::closeLogStreamController(LogStreamController* logStreamC)
{
    if (logStreamC != nullptr)
    {
        // Remove from the list
        _openedLogStreamControllers.remove(logStreamC);

        // Free memory
        delete logStreamC;
    }
}


/**
 * @brief Add the window to the list of opened windows
 * @param window
 */
void IngeScapeEditorController::addOpenedWindow(QObject* window)
{
    if (window != nullptr)
    {
        //qDebug() << "Add Opened Window:" << window << "(" << _openedWindows.count() << ")";

        if (!_openedWindows.contains(window)) {
            _openedWindows.append(window);
        }
    }
}


/**
 * @brief Remove the window from the list of opened windows
 * @param window
 */
void IngeScapeEditorController::removeOpenedWindow(QObject* window)
{
    if (window != nullptr)
    {
        //qDebug() << "Remove Opened Window:" << window << "(" << _openedWindows.count() << ")";

        if (_openedWindows.contains(window)) {
            _openedWindows.remove(window);
        }
    }
}

/**
 * @brief Checks if there was changes since the platform was last saved
 * @return
 */
bool IngeScapeEditorController::hasPlatformChanged()
{
    if (_hasAPlatformBeenLoadedByUser)
    {
        QJsonDocument loadedPlatform;
        QFile jsonFile(_currentPlatformFilePath);
        if (jsonFile.exists() && jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();
            loadedPlatform = QJsonDocument::fromJson(byteArrayOfJson);
        }

        QJsonDocument currentPlatform = _getJsonOfCurrentPlatform();

        qDebug() << "Platform has" << (loadedPlatform != currentPlatform ? "" : "NOT") << "changed";

        return loadedPlatform != currentPlatform;
    }
    else {
        // Always ask to save the current platform if none has been loaded
        return true;
    }
}


/**
 * @brief If _selectedNetwork not available will try to auto select another one
 */
bool IngeScapeEditorController::startIngeScape()
{
    bool success = false;
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();

    if ((ingeScapeNetworkC != nullptr) && (ingeScapeModelManager != nullptr)
            && (_modelManager != nullptr))
    {
        success = ingeScapeNetworkC->start(_networkDevice, _ipAddress, _port); // will failed if networkDevice is not available
    }
    return success;
}


void IngeScapeEditorController::stopIngeScape()
{
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();

    if ((ingeScapeNetworkC != nullptr) && (_modelManager != nullptr))
    {
        qInfo() << "Stop the network on" << _networkDevice << "with" << _port;

        // Stop our IngeScape agent
        ingeScapeNetworkC->stop();

        // We don't see itself
        ingeScapeNetworkC->setnumberOfEditors(1);
    }
}


bool IngeScapeEditorController::restartIngeScape()
{
    qInfo() << "Restart the network on" << _networkDevice << "with" << _port;
    stopIngeScape();
    return startIngeScape();
}


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeEditorController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}

void IngeScapeEditorController::_onAgentEditorStartedOrStopped(bool started)
{
    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if (!started)
    {
        // Simulate an exit for each agent ON
        ingeScapeModelManager->simulateExitForEachAgentON();

        // Simulate an exit for each launcher
        ingeScapeModelManager->simulateExitForEachLauncher();

        // Simulate an exit for the recorder
        if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON() && (_recordsSupervisionC->peerOfRecorder() != nullptr))
        {
            _recordsSupervisionC->onRecorderExited(_recordsSupervisionC->peerOfRecorder());
        }
    }
}


/**
 * @brief Called when our application receives an "open file" request
 * @param url
 * @param filePath
 */
void IngeScapeEditorController::_onOpenFileRequest(QUrl url, QString filePath)
{
    qInfo() << Q_FUNC_INFO << "url=" << url << ", filePath=" << filePath;

    QString scheme = url.scheme();

    if (QString::compare(scheme, QStringLiteral("file"), Qt::CaseSensitive) == 0)
    {
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists())
        {
            if (QString::compare(fileInfo.suffix(), QStringLiteral("igsplatform")) == 0)
            {
                // Platform
                _clearAndLoadPlatformFromFile(filePath);
            }
            else if (QString::compare(fileInfo.suffix(), QStringLiteral("igsdefinition")) == 0)
            {
                // Definition file

                // We need to check if we have a valid license
                if ((_licensesC != nullptr)
                    && (_licensesC->mergedLicense() != nullptr)
                    && _licensesC->mergedLicense()->editorLicenseValidity())
                {
                    if (_modelManager != nullptr)
                    {
                        bool succeeded = IngeScapeModelManager::instance()->importAgentOrAgentsListFromFilePath(filePath);
                        if (!succeeded)
                        {
                            Q_EMIT openPopupFailedToLoadAgentDefinition();
                        }
                    }
                }
                else
                {
                    // Invalid license
                    Q_EMIT openPopupLicense();
                }
            }
            else if (QString::compare(fileInfo.suffix(), QStringLiteral("igslicense")) == 0)
            {
                // License file
                if (_licensesC != nullptr)
                {
                    QList<QUrl> listOfUrls;
                    listOfUrls.append(url);
                    _licensesC->addLicenses(listOfUrls);
                }
            }
            else
            {
                qInfo() << Q_FUNC_INFO <<": unknown file type " << filePath;
            }
        }
        else
        {
            // NB: Should not happen IF we don't create fake QFileOpenEvent events
            qWarning() << Q_FUNC_INFO << "warning: file" << filePath << "does not exist";
        }
    }
    else
    {
        qWarning() << Q_FUNC_INFO << "warning: scheme" << scheme << "not supported";
    }
}



/**
 * @brief Slot called when we have to open the "Log Stream" of a list of agents
 */
void IngeScapeEditorController::_onOpenLogStreamOfAgents(QList<AgentM*> models)
{
    if (!models.isEmpty())
    {
        for (AgentM* model : models)
        {
            if ((model != nullptr) && (model->peer() != nullptr) && model->isON())
            {
                QString subscriberAddress = QString("tcp://%1:%2").arg(model->peer()->ipAddress(), model->peer()->loggerPort());

                qDebug() << "Open the 'Log Stream' of" << model->name() << "(Address:" << model->peer()->ipAddress() << "+ Logger Port:" << model->peer()->loggerPort() << "--> Subscriber Address:" << subscriberAddress << ")";

                LogStreamController* logStreamController = nullptr;

                for (LogStreamController* iterator : _openedLogStreamControllers.toList())
                {
                    if ((iterator != nullptr) && (iterator->agentName() == model->name()) && (iterator->subscriberAddress() == subscriberAddress))
                    {
                        logStreamController = iterator;
                    }
                }

                if (logStreamController != nullptr)
                {
                    qDebug() << "The 'Log Stream' for" << logStreamController->agentName() << "on" << logStreamController->subscriberAddress() << "already exists...bring to front !";
                    Q_EMIT logStreamController->bringToFront();
                }
                else
                {
                    // Create a new "Log Stream" controller
                    logStreamController = new LogStreamController(model->name(), model->peer()->hostname(), subscriberAddress, this);
                    _openedLogStreamControllers.append(logStreamController);
                }
            }
        }
    }
}


/**
 * @brief Slot called when the user wants to start to record
 */
void IngeScapeEditorController::_onStartToRecord()
{
    // Get the JSON of the current platform
    QJsonDocument jsonDocument = _getJsonOfCurrentPlatform();

    if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON()
            && (_recordsSupervisionC->peerOfRecorder() != nullptr)
            && !jsonDocument.isNull() && !jsonDocument.isEmpty())
    {
        QString jsonString = QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact));

        // Add the delta from the start time of the TimeLine
        int deltaTimeFromTimeLineStart = 0;

        if (_scenarioC != nullptr) {
            deltaTimeFromTimeLineStart = _scenarioC->currentTime().msecsSinceStartOfDay();
        }

        QStringList message = {
            command_StartRecord,
            _currentPlatformName,
            QString::number(deltaTimeFromTimeLineStart),
            jsonString
        };

        // Send a ZMQ message in several parts to the recorder
        IngeScapeNetworkController::instance()->sendZMQMessageToAgent(_recordsSupervisionC->peerOfRecorder()->uid(), message);
    }
}


/**
 * @brief Slot called when a replay is currently loading
 * @param deltaTimeFromTimeLineStart
 * @param jsonPlatform
 * @param jsonExecutedActions
 */
void IngeScapeEditorController::_onReplayLoading(int deltaTimeFromTimeLineStart, QString jsonPlatform, QString jsonExecutedActions)
{
    QString recordName = "";

    if (_recordsSupervisionC != nullptr)
    {
        // Update the current state of the replay
        _recordsSupervisionC->setreplayState(ReplayStates::LOADING);

        // Get the name of the current replay
        recordName = _recordsSupervisionC->getCurrentReplayName();
    }

    // FIXME TODO: execute in other thread ?

    if ((deltaTimeFromTimeLineStart >= 0) && !jsonPlatform.isEmpty())
    {
        // Set _platformNameBeforeLoadReplay on first replay loading only
        // N.B : "Unload record" is only send when no more record is selected
        if (_platformNameBeforeLoadReplay == "")
        {
            _platformNameBeforeLoadReplay = currentPlatformName();
        }

        // First, clear the current platform by deleting all existing data
        _clearCurrentPlatform();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonPlatform.toUtf8());

        // Load the platform from JSON
        bool success = _loadPlatformFromJSON(jsonDocument);
        if (success)
        {
            // Update the current platform name
            if (!recordName.isEmpty())
            {
                setcurrentPlatformName(recordName);

                _currentPlatformFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, recordName);

                // Platform has been loaded by Editor
                sethasAPlatformBeenLoadedByUser(false);
            }

            qDebug() << "Received executed actions (DO NOTHING WITH IT RIGHT NOW)" << jsonExecutedActions.size();

            Q_EMIT resetTimeLineView(false); // Open timeline view
            Q_EMIT resetMappingView(); // Center mapping view
        }
        else
        {
            qCritical() << "The loading of the replay failed !";
        }
    }
}

/**
 * @brief Slot called when a replay is  unloaded : allow to reload platform before "record mode"
 */
void IngeScapeEditorController::_onReplayUNloaded()
{
    if (_recordsSupervisionC != nullptr)
    {
        // Update the current state of the replay
        _recordsSupervisionC->setreplayState(ReplayStates::UNLOADED);
    }

    // _platformNameBeforeLoadReplay = "" means that last platform is already clear,
    // and current platform is a "NEW PLATFORM"
    if (_platformNameBeforeLoadReplay != "")
    {
        _clearCurrentPlatform();
        _currentPlatformFilePath = QString("%1%2.igsplatform").arg(_platformDirectoryPath, _platformNameBeforeLoadReplay);
        _platformNameBeforeLoadReplay = "";

        // Load the platform from last platform file used before records
        bool success = _loadPlatformFromFile(_currentPlatformFilePath);
        if (!success)
        {
             qCritical() << "The loading of the current platform before replay failed !";
        }
    }
}


/**
 * @brief Slot called when we receive the command "Load Platform File From Path"
 * @param platformFilePath
 */
void IngeScapeEditorController::_onLoadPlatformFileFromPath(QString platformFilePath)
{
    qInfo() << "Received the command 'Load Platform file from path'" << platformFilePath;

    // Clear the current platform and load our new one
    bool success = _clearAndLoadPlatformFromFile(platformFilePath);

    // Send command to Ingescape-Expe if needed
    if ((_networkC != nullptr) && (_peerOfExpe != nullptr)
            && !_peerOfExpe->uid().isEmpty())
    {
        // Reply by sending the command execution status to Expe
        _networkC->sendCommandExecutionStatusToExpe(_peerOfExpe->uid(),
                                                    command_LoadPlatformFile,
                                                    platformFilePath,
                                                    static_cast<int>(success));
    }
    else
    {
        qWarning() << "Peer of Expe is NULL";
    }
}


/**
 * @brief Slot called when we receive the command "Update TimeLine State"
 * @param state
 */
void IngeScapeEditorController::_onUpdateTimeLineState(QString state)
{
    qInfo() << "Received the command 'Update TimeLine State'" << state;

    if (_scenarioC != nullptr)
    {
        // PLAY (the TimeLine)
        if (state == PLAY)
        {
            _scenarioC->playOrResumeTimeLine();
        }
        // PAUSE (the TimeLine)
        else if (state == PAUSE)
        {
            _scenarioC->pauseTimeLine();
        }
        // RESET (the TimeLine)
        else if (state == RESET)
        {
            _scenarioC->stopTimeLine();
        }
        else {
            qCritical() << "Unknown state" << state << "so we cannot update the TimeLine !";
        }
    }

    /*if ((networkC != nullptr) && !_peerIdOfExpe.isEmpty())
    {
        // Reply by sending the command execution status to Expe
        networkC->sendCommandExecutionStatusToExpe(_peerIdOfExpe, command_LoadPlatformFile, platformFilePath, static_cast<int>(success));
    }
    else {
        qWarning() << "Peer Id of Expe is empty" << _peerIdOfExpe;
    }*/
}


/**
 * @brief Slot called when we receive the command "Update Record State"
 * @param state
 */
void IngeScapeEditorController::_onUpdateRecordState(QString state)
{
    qInfo() << "Received the command 'Update Record State'" << state;

    /*if (_scenarioC != nullptr)
    {
        // Update the state of the record
        _scenarioC->updateRecordState(state);
    }*/

    // START (to Record)
    if (state == START)
    {
        // Call the private slot (called when the user wants to start to record)
        _onStartToRecord();
    }
    // STOP (to Record)
    else if (state == STOP)
    {
        if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON()
                && (_recordsSupervisionC->peerOfRecorder() != nullptr))
        {
            // Send the message "Stop Record" to the recorder
            IngeScapeNetworkController::instance()->sendStringMessageToAgent(_recordsSupervisionC->peerOfRecorder()->uid(),
                                                                             command_StopRecord);
        }
    }
    // Unknown
    else {
        qCritical() << "Unknown state" << state << "so we cannot update the Record !";
    }
}


/**
 * @brief Slot called just before an action is performed
 * (the message "EXECUTED ACTION" must be sent on the network to the recorder)
 * @param message
 */
void IngeScapeEditorController::_onActionWillBeExecuted(QString message)
{
    if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON()
            && (_recordsSupervisionC->peerOfRecorder() != nullptr))
    {
        // Send the message "EXECUTED ACTION" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_recordsSupervisionC->peerOfRecorder()->uid(),
                                                                         message);
    }
}


/**
 * @brief Slot called when the state of the TimeLine updated
 * @param state
 */
void IngeScapeEditorController::_onTimeLineStateUpdated(QString state)
{
    // Add the delta from the start time of the TimeLine
    int deltaTimeFromTimeLineStart = 0;

    if (_scenarioC != nullptr)
    {
        deltaTimeFromTimeLineStart = _scenarioC->currentTime().msecsSinceStartOfDay();
    }

    QString notificationAndParameters = QString("%1=%2|%3").arg(notif_TimeLineState, state, QString::number(deltaTimeFromTimeLineStart));

    if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON()
            && (_recordsSupervisionC->peerOfRecorder() != nullptr))
    {
        // Send the message "TIMELINE STATE" to the recorder
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_recordsSupervisionC->peerOfRecorder()->uid(),
                                                                         notificationAndParameters);
    }

    // Notify the Expe app
    if ((_peerOfExpe != nullptr) && !_peerOfExpe->uid().isEmpty())
    {
        IngeScapeNetworkController::instance()->sendStringMessageToAgent(_peerOfExpe->uid(),
                                                                         notificationAndParameters);
    }
}


/**
 * @brief Slot called when an expe enter the network
 */
void IngeScapeEditorController::_onExpeEntered(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Expe entered" << peer->name() << "(" << peer->uid() << ") on" << peer->hostname() << "(" << peer->ipAddress() << ")";

        if (_peerOfExpe == nullptr) {
            setpeerOfExpe(peer);
        }
        else {
            qCritical() << "We are already connected to an expe:" << _peerOfExpe->name() << "(" << _peerOfExpe->uid() << ")";
        }
    }
}


/**
 * @brief Slot called when an expe quit the network
 */
void IngeScapeEditorController::_onExpeExited(PeerM* peer)
{
    if (peer != nullptr)
    {
        qInfo() << "Expe exited" << peer->name() << "(" << peer->uid() << ")";

        if (_peerOfExpe == peer)
        {
            setpeerOfExpe(nullptr);
        }
    }
}


/**
 * @brief Slot called when the licenses have been updated
 */
void IngeScapeEditorController::_onLicensesUpdated()
{
    qDebug() << "on License Updated";
    // DO nothing
}

/**
 * @brief Slot called when user's license limitations are reached
 */
void IngeScapeEditorController::_onLicenseLimitationReached()
{
    stopIngeScape();
}


/**
 * @brief Called when our network device is no more available
 */
void IngeScapeEditorController::_onNetworkDeviceIsNotAvailable()
{
    qDebug() << Q_FUNC_INFO;

    if (IngeScapeNetworkController::instance()->isStarted())
    {
        stopIngeScape();
    }
    setnetworkDevice("");
}


/**
 * @brief Called when our network device is available again
 */
void IngeScapeEditorController::_onNetworkDeviceIsAvailableAgain()
{
    qDebug() << Q_FUNC_INFO;
    // N.B. : If we want to restart our editor, it is here
}


/**
 * @brief Called when the IP address of our network device has changed
 */
void IngeScapeEditorController::_onNetworkDeviceIpAddressHasChanged()
{
    qDebug() << Q_FUNC_INFO;
    if (IngeScapeNetworkController::instance()->isStarted())
    {
        restartIngeScape();
    }
}


/**
 * @brief Called when our machine will go to sleep
 */
void IngeScapeEditorController::_onSystemSleep()
{
    qDebug() << Q_FUNC_INFO;
    // Stop monitoring to save energy (will be relaunch on system wake)
    IngeScapeNetworkController::instance()->stopMonitoring();

    if (IngeScapeNetworkController::instance()->isStarted())
    {
        stopIngeScape();
    }
}


/**
 * @brief Called when our machine did wake from sleep
 */
void IngeScapeEditorController::_onSystemWake()
{  
    // Restart monitoring to detect events relative to our expected network
    IngeScapeNetworkController::instance()->startMonitoring(_networkDevice, _port);

    // Not relaunch the editor, stay OFFLINE
}


/**
 * @brief Called when a network configuration is added, removed or changed
 */
void IngeScapeEditorController::_onSystemNetworkConfigurationsUpdated()
{
    IngeScapeNetworkController::instance()->updateAvailableNetworkDevices();
}


/**
 * @brief Load the platform from a JSON file
 */
bool IngeScapeEditorController::_loadPlatformFromFile(QString platformFilePath)
{
    bool success = false;

    if (!platformFilePath.isEmpty())
    {
        qInfo() << "Load the platform from file" << platformFilePath;

        QFile jsonFile(platformFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                // Update the current platform name
                QFileInfo fileInfo = QFileInfo(jsonFile);
                setcurrentPlatformName(fileInfo.baseName());
                _currentPlatformFilePath = fileInfo.absoluteFilePath();

                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

                // Load the platform from JSON
                success = _loadPlatformFromJSON(jsonDocument);
                if (success)
                {
                    sethasAPlatformBeenLoadedByUser(true);
                }

                Q_EMIT resetTimeLineView(false); // Close timeline view
                Q_EMIT resetMappingView(); // Center mapping view
            }
            else
            {
                qCritical() << "Can not open file" << platformFilePath;
            }
        }
        else
        {
            qWarning() << "There is no file" << platformFilePath;
        }
    }

    return success;
}

/**
 * @brief Clear the current platform (agents, mappings, actions, palette, timeline actions, hosts)
 * by deleting all existing data
 */
void IngeScapeEditorController::_clearCurrentPlatform()
{
    qInfo() << "Clear Current Platform (" << _currentPlatformName << ")";

    // Clear agents OFF and their mapping
    if (_agentsMappingC != nullptr) {
        _agentsMappingC->clearMapping();
    }

    // Clear the current scenario
    if (_scenarioC != nullptr) {
        _scenarioC->clearScenario();
    }

    if (_hostsSupervisionC != nullptr)
    {
        // Delete hosts OFF
        _hostsSupervisionC->deleteHostsOFF();
    }

    IngeScapeModelManager* ingeScapeModelManager = IngeScapeModelManager::instance();
    if (ingeScapeModelManager != nullptr)
    {
        // Delete all published values
        ingeScapeModelManager->deleteAllPublishedValues();

        // Delete all (models of) actions
        ingeScapeModelManager->deleteAllActions();

        // Delete agents OFF
        QStringList namesListOfAgentsON = ingeScapeModelManager->deleteAgentsOFF();

        if (_valuesHistoryC != nullptr)
        {
            // Set both list of agent names with agents ON
            _valuesHistoryC->setAgentNamesList(namesListOfAgentsON);
        }
    }

    if (_timeLineC != nullptr) {
        // Reset timeline parameters
        _timeLineC->resetTimeline();
    }

    Q_EMIT resetTimeLineView(false); // Close timeline view
    Q_EMIT resetMappingView(); // Center mapping view
}



/**
 * @brief Clear our current platform and load a new platform from a given file
 */
bool IngeScapeEditorController::_clearAndLoadPlatformFromFile(QString platformFilePath)
{
    bool succeeded = false;

    if (!platformFilePath.isEmpty())
    {
        // First, clear the current platform by deleting all existing data
        _clearCurrentPlatform();

        // Load the platform from JSON file
        succeeded = _loadPlatformFromFile(platformFilePath);

        if (!succeeded)
        {
            qCritical() << Q_FUNC_INFO << ": Failed to load platform" <<  platformFilePath;
        }
        else
        {
            sethasAPlatformBeenLoadedByUser(true);
        }
    }
    else
    {
        qDebug() << Q_FUNC_INFO << ": Platform file path is empty, nothing to do";
    }

    return succeeded;
}


/**
 * @brief Save the platform to a JSON file
 * @param platformFilePath
 */
void IngeScapeEditorController::_savePlatformToFile(QString platformFilePath)
{
    if (!platformFilePath.isEmpty())
    {
        qInfo() << "Save the current platform to file" << platformFilePath;

        // Get the JSON of the current platform
        QJsonDocument jsonDocument = _getJsonOfCurrentPlatform();
        if (!jsonDocument.isNull() && !jsonDocument.isEmpty())
        {
            QFile jsonFile(platformFilePath);
            if (jsonFile.open(QIODevice::WriteOnly))
            {
                // Update the current platform name
                QFileInfo fileInfo = QFileInfo(jsonFile);
                setcurrentPlatformName(fileInfo.baseName());
                _currentPlatformFilePath = fileInfo.absoluteFilePath();
                sethasAPlatformBeenLoadedByUser(true);

                jsonFile.write(jsonDocument.toJson(QJsonDocument::Indented));
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << platformFilePath;
            }
        }
    }
}


/**
 * @brief Load the platform from JSON
 * @param jsonDocument
 * @return
 */
bool IngeScapeEditorController::_loadPlatformFromJSON(QJsonDocument jsonDocument)
{
    bool success = false;

    if (jsonDocument.isObject())
    {
        QJsonObject jsonRoot = jsonDocument.object();

        // Version
        QString versionJsonPlatform = "";
        if (jsonRoot.contains("version"))
        {
            versionJsonPlatform = jsonRoot.value("version").toString();

            qDebug() << "Version of JSON platform is" << versionJsonPlatform;
        }
        else {
            qDebug() << "UNDEFINED version of JSON platform";
        }

        // Import the agents list from JSON
        if (jsonRoot.contains("agents"))
        {
            IngeScapeModelManager::instance()->importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);
        }

        // Import the scenario from JSON
        if ((_scenarioC != nullptr) && jsonRoot.contains("scenario"))
        {
            _scenarioC->importScenarioFromJson(jsonRoot.value("scenario").toObject());
        }

        // Import the global mapping (of agents) from JSON
        if ((_agentsMappingC != nullptr) && jsonRoot.contains("mapping"))
        {
            _agentsMappingC->importMappingFromJson(jsonRoot.value("mapping").toArray());
        }

        // Import timeline settings from JSON
        if ((_timeLineC != nullptr) && jsonRoot.contains("timeline"))
        {
            _timeLineC->importTimelineFromJson(jsonRoot.value("timeline").toObject());
        }
        success = true;
    }
    return success;
}


/**
 * @brief Get the JSON of the current platform
 * @return
 */
QJsonDocument IngeScapeEditorController::_getJsonOfCurrentPlatform()
{
    QJsonDocument jsonDocument;
    QJsonObject platformJsonObject;

    platformJsonObject.insert("version", VERSION_JSON_PLATFORM);

    // Save the agents
    if (_modelManager != nullptr)
    {
        // Export the agents into JSON
        QJsonArray arrayOfAgents = _modelManager->exportAgentsToJSON();
        platformJsonObject.insert("agents", arrayOfAgents);
    }

    // Save the mapping
    if (_agentsMappingC != nullptr)
    {
        // Export the global mapping (of agents) into JSON
        QJsonArray arrayOfAgentsInMapping = _agentsMappingC->exportGlobalMappingToJSON();
        platformJsonObject.insert("mapping", arrayOfAgentsInMapping);
    }

    // Save the scenario
    if (_scenarioC != nullptr)
    {
        // actions list
        // actions list in the palette
        // actions list in the timeline
        QJsonObject jsonScenario = JsonHelper::exportScenario(_scenarioC->actionsList()->toList(),
                                                              _scenarioC->actionsInPaletteList()->toList(),
                                                              _scenarioC->actionsInTimeLine()->toList());

        platformJsonObject.insert("scenario", jsonScenario);
    }

    // Save timeline settings
    if ((_timeLineC != nullptr))
    {
        // Zoom level (actual pixels number per minute in timeline)
        QJsonObject jsonTimeline;
        jsonTimeline.insert("pixels_per_minute", _timeLineC->pixelsPerMinute());

        platformJsonObject.insert("timeline", jsonTimeline);
    }

    return QJsonDocument(platformJsonObject);
}


/**
 * @brief Return a network device choose with the logic of an igs agent :
 * If there is only one or if there are 2 and one of them is a loopback return a network device
 * Else return ""
 */
QString IngeScapeEditorController::_autoFindAnAvailableDevice()
{
    IngeScapeNetworkController* ingeScapeNetworkC = IngeScapeNetworkController::instance();
    ingeScapeNetworkC->updateAvailableNetworkDevices();
    int nbDevices = ingeScapeNetworkC->availableNetworkDevices().count();
    QStringList devicesAddresses = ingeScapeNetworkC->availableNetworkDevicesAddresses();
    if (nbDevices == 1)
    {
         // Use the only available network device
        return ingeScapeNetworkC->availableNetworkDevices().at(0);
    }
    else if ((nbDevices == 2)
             && ((devicesAddresses.at(0) == "127.0.0.1")||(devicesAddresses.at(1) == "127.0.0.1")))
    {
        // 2 available devices, one is the loopback : we pick the device that is NOT the loopback
        if (devicesAddresses.at(0) == "127.0.0.1")
        {
            return ingeScapeNetworkC->availableNetworkDevices().at(1);
        }
        else
        {
            return ingeScapeNetworkC->availableNetworkDevices().at(0);
        }
    }
    else
    {
        // No device or several devices available
        return "";
    }
}
