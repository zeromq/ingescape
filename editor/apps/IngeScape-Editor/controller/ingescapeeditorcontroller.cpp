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
 *
 */

#include "controller/ingescapeeditorcontroller.h"

#include <I2Quick.h>
#include <QFileDialog>
#include <QThread>
#include <QApplication>
#include <QCursor>

#include <misc/ingescapeutils.h>
#include <settings/ingescapesettings.h>

#include <platformsupport/osutils.h>
#include <platformsupport/IngescapeApplication.h>


// Name of the example platform to load when no other platform has been loaded yet
const QString IngeScapeEditorController::EXAMPLE_PLATFORM_NAME = "example";

// Default name when creating a new platform
const QString IngeScapeEditorController::NEW_PLATFORM_NAME = "New Platform";

// Default name when creating a new platform
const QString IngeScapeEditorController::SPECIAL_EMPTY_LAST_PLATFORM = "empty";

// Default remote URL for the getting started page
const QString IngeScapeEditorController::DEFAULT_REMOTE_URL_GETTING_STARTED = ""; //FIXME Define default URL ?

// Default local URL for the getting started page
const QString IngeScapeEditorController::DEFAULT_LOCAL_URL_GETTING_STARTED = ""; //FIXME Define default URL ?



/**
 * @brief Constructor
 * @param parent
 */
IngeScapeEditorController::IngeScapeEditorController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _isAvailableModelVisualizer(false),
    _isVisibleModelVisualizer(false),
    _errorMessageWhenConnectionFailed(""),
    _snapshotDirectory(""),
    _modelManager(nullptr),
    _agentsSupervisionC(nullptr),
    _agentsMappingC(nullptr),
    _networkC(nullptr),
    _scenarioC(nullptr),
    _valuesHistoryC(nullptr),
    _licensesC(nullptr),
    _timeLineC(nullptr),
    _peerIdOfExpe(""),
    _peerNameOfExpe(""),
    _currentPlatformName(EXAMPLE_PLATFORM_NAME),
    _hasAPlatformBeenLoadedByUser(false),
    _gettingStartedRemoteUrl(""),
    _gettingStartedLocalUrl(""),
    _gettingStartedShowAtStartup(true),
    _terminationSignalWatcher(nullptr),
    _jsonHelper(nullptr),
    _platformDirectoryPath(""),
    _platformDefaultFilePath(""),
    _currentPlatformFilePath(""),
    // Connect mapping in observe mode
    _beforeNetworkStop_isMappingConnected(true),
    _beforeNetworkStop_isMappingControlled(false)
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

        // Init the path to the JSON file to load the example platform
        _platformDefaultFilePath = QString("%1%2.json").arg(_platformDirectoryPath, EXAMPLE_PLATFORM_NAME);
    }


    // Create the (sub) directory "exports" if not exist (the directory contains CSV files about exports)
    IngeScapeUtils::getExportsPath();


    //------------------
    //
    // Settings
    //
    //------------------
    IngeScapeSettings &settings = IngeScapeSettings::Instance();

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
    // Settings about "Debug"
    //
    settings.beginGroup("debug");

#ifdef QT_DEBUG
    _isAvailableModelVisualizer = true;
#else
    _isAvailableModelVisualizer = settings.value("modelVisualizer", QVariant(false)).toBool();
#endif
    qDebug() << "The Model/View Model Visualizer is available ?" << _isAvailableModelVisualizer;

    settings.endGroup();


    //
    // Settings about "Platform"
    //
    settings.beginGroup("platform");
    _currentPlatformFilePath = settings.value("last", _platformDefaultFilePath).toString();
    settings.endGroup();

    //
    // Settings about "Help"
    //
    settings.beginGroup("help");
    _gettingStartedRemoteUrl = settings.value("remoteUrlGettingStarted", DEFAULT_REMOTE_URL_GETTING_STARTED).toString();
    _gettingStartedLocalUrl = settings.value("localUrlGettingStarted", DEFAULT_LOCAL_URL_GETTING_STARTED).toString();
    _gettingStartedShowAtStartup = settings.value("showAtStartup", true).toBool();
    settings.endGroup();



    //
    // Create the helper to manage JSON files
    //
    _jsonHelper = new JsonHelper(this);



    //-------------------------------
    //
    // Create sub-controllers
    //
    //-------------------------------

    // Create the manager for the data model of our IngeScape Editor application
    _modelManager = new EditorModelManager(_jsonHelper, rootPath, this);

    // Create the controller to manage network communications
    _networkC = new NetworkController(this);

    // Create the controller to manage the agents list
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, _jsonHelper, this);

    // Create the controller to manage hosts
    _hostsSupervisionC = new HostsSupervisionController(_modelManager, this);

    // Create the controller for records supervision
    _recordsSupervisionC = new RecordsSupervisionController(_modelManager, this);

    // Create the controller to manage the agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, _jsonHelper, this);

    // Create the controller to manage the scenario
    _scenarioC = new ScenarioController(_modelManager, _jsonHelper, this);

    // Create the controller to manage the history of values
    _valuesHistoryC = new ValuesHistoryController(_modelManager, this);

    // Create the controller to manage IngeScape licenses
    _licensesC = new LicensesController(this);

    // Create the controller to manage the time line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);


    // Connect to signals from our licenses manager
    connect(_licensesC, &LicensesController::licensesUpdated, this, &IngeScapeEditorController::_onLicensesUpdated);


    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::networkDeviceIsNotAvailable, this, &IngeScapeEditorController::_onNetworkDeviceIsNotAvailable);
    connect(_networkC, &NetworkController::networkDeviceIsAvailableAgain, this, &IngeScapeEditorController::_onNetworkDeviceIsAvailableAgain);
    connect(_networkC, &NetworkController::networkDeviceIpAddressHasChanged, this, &IngeScapeEditorController::_onNetworkDeviceIpAddressHasChanged);

    connect(_networkC, &NetworkController::agentEntered, _modelManager, &EditorModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &EditorModelManager::onAgentExited);
    connect(_networkC, &NetworkController::launcherEntered, _modelManager, &EditorModelManager::onLauncherEntered);
    connect(_networkC, &NetworkController::launcherExited, _modelManager, &EditorModelManager::onLauncherExited);
    connect(_networkC, &NetworkController::recorderEntered, _recordsSupervisionC, &RecordsSupervisionController::onRecorderEntered);
    connect(_networkC, &NetworkController::recorderExited, _recordsSupervisionC, &RecordsSupervisionController::onRecorderExited);
    connect(_networkC, &NetworkController::expeEntered, this, &IngeScapeEditorController::_onExpeEntered);
    connect(_networkC, &NetworkController::expeExited, this, &IngeScapeEditorController::_onExpeExited);

    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &EditorModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &EditorModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::valuePublished, _modelManager, &EditorModelManager::onValuePublished);
    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &EditorModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::canBeFrozenFromAgentUpdated, _modelManager, &EditorModelManager::onCanBeFrozenFromAgentUpdated);
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
    connect(_networkC, &NetworkController::replayEndedReceived, _recordsSupervisionC, &RecordsSupervisionController::onReplayEnded);
    connect(_networkC, &NetworkController::recordExported, _recordsSupervisionC, &RecordsSupervisionController::onRecordExported);

    connect(_networkC, &NetworkController::runAction, _scenarioC, &ScenarioController::onRunAction);
    connect(_networkC, &NetworkController::loadPlatformFileFromPath, this, &IngeScapeEditorController::_onLoadPlatformFileFromPath);
    connect(_networkC, &NetworkController::updateTimeLineState, this, &IngeScapeEditorController::_onUpdateTimeLineState);
    connect(_networkC, &NetworkController::updateRecordState, this, &IngeScapeEditorController::_onUpdateRecordState);
    connect(_networkC, &NetworkController::highlightLink, _agentsMappingC, &AgentsMappingController::onHighlightLink);


    // Connect to signals from the model manager
    connect(_modelManager, &EditorModelManager::isMappingConnectedChanged, _agentsMappingC, &AgentsMappingController::onIsMappingConnectedChanged);
    connect(_modelManager, &EditorModelManager::isMappingControlledChanged, _agentsMappingC, &AgentsMappingController::onIsMappingControlledChanged);

    connect(_modelManager, &EditorModelManager::agentModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onAgentModelHasBeenCreated);
    connect(_modelManager, &EditorModelManager::agentModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onAgentModelWillBeDeleted);

    connect(_modelManager, &EditorModelManager::hostModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onHostModelHasBeenCreated);
    connect(_modelManager, &EditorModelManager::hostModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onHostModelWillBeDeleted);
    connect(_modelManager, &EditorModelManager::previousHostParsed, _hostsSupervisionC, &HostsSupervisionController::onPreviousHostParsed);

    connect(_modelManager, &EditorModelManager::agentsGroupedByNameHasBeenCreated, _valuesHistoryC, &ValuesHistoryController::onAgentsGroupedByNameHasBeenCreated);
    connect(_modelManager, &EditorModelManager::agentsGroupedByNameHasBeenCreated, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameHasBeenCreated);
    connect(_modelManager, &EditorModelManager::agentsGroupedByNameWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameWillBeDeleted);

    connect(_modelManager, &EditorModelManager::agentsGroupedByDefinitionHasBeenCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionHasBeenCreated);
    connect(_modelManager, &EditorModelManager::agentsGroupedByDefinitionWillBeDeleted, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionWillBeDeleted);

    connect(_modelManager, &EditorModelManager::isMappingConnectedChanged, _networkC, &NetworkController::onIsMappingConnectedChanged);
    connect(_modelManager, &EditorModelManager::addInputsToOurApplicationForAgentOutputs, _networkC, &NetworkController::onAddInputsToOurApplicationForAgentOutputs);
    connect(_modelManager, &EditorModelManager::removeInputsFromOurApplicationForAgentOutputs, _networkC, &NetworkController::onRemoveInputsFromOurApplicationForAgentOutputs);

    connect(_modelManager, &EditorModelManager::actionModelWillBeDeleted, _agentsMappingC, &AgentsMappingController::onActionModelWillBeDeleted);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgentAboutOutput, _networkC, &NetworkController::onCommandAskedToAgentAboutOutput);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openValuesHistoryOfAgent, _valuesHistoryC, &ValuesHistoryController::filterValuesToShowOnlyAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openLogStreamOfAgents, this, &IngeScapeEditorController::_onOpenLogStreamOfAgents);

    // Connect to signals from the controller for supervision of hosts
    connect(_hostsSupervisionC, &HostsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_hostsSupervisionC, &HostsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);

    // Connect to signals from the controller for mapping of agents
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_agentsMappingC, &AgentsMappingController::executeAction, _scenarioC, &ScenarioController::onExecuteAction);

    // Connect to signals from the controller of the scenario
    connect(_scenarioC, &ScenarioController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_scenarioC, &ScenarioController::commandAskedToRecorder, this, &IngeScapeEditorController::_onCommandAskedToRecorder);
    connect(_scenarioC, &ScenarioController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);
    connect(_scenarioC, &ScenarioController::timeLineStateUpdated, this, &IngeScapeEditorController::_onTimeLineStateUpdated);

    // Connect to the signal "time range changed" from the time line
    // to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::onTimeRangeChanged);

    // Connect to signals from Record supervision controller
    connect(_recordsSupervisionC, &RecordsSupervisionController::commandAskedToRecorder, _networkC, &NetworkController::onCommandAskedToRecorder);
    connect(_recordsSupervisionC, &RecordsSupervisionController::startToRecord, this, &IngeScapeEditorController::_onStartToRecord);


    // Connect to OS events
    connect(OSUtils::instance(), &OSUtils::systemSleep, this, &IngeScapeEditorController::_onSystemSleep);
    connect(OSUtils::instance(), &OSUtils::systemWake, this, &IngeScapeEditorController::_onSystemWake);
    connect(OSUtils::instance(), &OSUtils::systemNetworkConfigurationsUpdated, this, &IngeScapeEditorController::_onSystemNetworkConfigurationsUpdated);


    if (_isAvailableModelVisualizer)
    {
        // Emit the signal "Previous Host Parsed" to create the fake host "HOSTNAME_NOT_DEFINED"
        Q_EMIT _modelManager->previousHostParsed(HOSTNAME_NOT_DEFINED);
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
                if (QString::compare(fileInfo.suffix(), QStringLiteral("igsplatform")) == 0)
                {
                    _currentPlatformFilePath = pendingOpenFileRequestFilePath;
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



    // Application
    if (IngescapeApplication::instance() != nullptr)
    {
        // Check if there is a pending "open file" request (agent(s) definition or license file)
        if (hasPendingOpenFileRequest)
        {
            _onOpenFileRequest(pendingOpenFileRequestUrl, pendingOpenFileRequestFilePath);
        }

        // Subscribe to our application
        connect(IngescapeApplication::instance(), &IngescapeApplication::openFileRequest, this, &IngeScapeEditorController::_onOpenFileRequest);
    }



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
IngeScapeEditorController::~IngeScapeEditorController()
{
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

    if (_licensesC != nullptr)
    {
        disconnect(_licensesC);

        LicensesController* temp = _licensesC;
        setlicensesC(nullptr);
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

    if (_networkC != nullptr)
    {
        disconnect(_networkC);

        NetworkController* temp = _networkC;
        setnetworkC(nullptr);
        delete temp;
        temp = nullptr;
    }

    // Delete json helper
    if (_jsonHelper != nullptr)
    {
        delete _jsonHelper;
        _jsonHelper = nullptr;
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
                                                            tr("IGS platform (*.igsplatform *.json)")
                                                            );

    if (!platformFilePath.isEmpty())
    {
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
                                                                "Save platform",
                                                                _currentPlatformFilePath,
                                                                "JSON (*.json)");

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
void IngeScapeEditorController::clearCurrentPlatform()
{
    qInfo() << "Clear Current Platform (" << _currentPlatformName << ")";

    // Update the current platform name
    setcurrentPlatformName(NEW_PLATFORM_NAME);
    _currentPlatformFilePath = QString("%1%2.json").arg(_platformDirectoryPath, NEW_PLATFORM_NAME);
    sethasAPlatformBeenLoadedByUser(false);

    // Clear the current mapping
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

    if (_modelManager != nullptr)
    {
        // Delete all actions
        _modelManager->deleteAllActions();

        // Delete agents OFF
        _modelManager->deleteAgentsOFF();
    }

    // Notify QML to reset view
    Q_EMIT resetMappindAndTimeLineViews();
}


/**
 * @brief Actions to perform before the application closing
 */
void IngeScapeEditorController::processBeforeClosing()
{
    // Save in the app settings the currently opened platform (to open it at next launch)
    IngeScapeSettings &settings = IngeScapeSettings::Instance();
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
            && (_modelManager != nullptr) && (_agentsMappingC != nullptr) && (_scenarioC != nullptr))
    {
        QString agentName = agentsGroupedByDefinition->name();

        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentName);
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
 * @brief Re-Start the network with a port and a network device
 * @param strPort
 * @param networkDevice
 * @param hasToClearPlatform
 * @return true when success
 */
bool IngeScapeEditorController::restartNetwork(QString strPort, QString networkDevice, bool hasToClearPlatform)
{
    bool success = false;

    bool isUInt = false;
    uint port = strPort.toUInt(&isUInt);
    if (isUInt && (port > 0))
    {
        // None changes (Same port, same network device and same licenses path)
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
    else
    {
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
 * @brief Method used to force the creation of our singleton from QML
 */
void IngeScapeEditorController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}


/**
 * @brief Get the position of the mouse cursor in global screen coordinates
 *
 * @remarks You must use mapToGlobal to convert it to local coordinates
 *
 * @return
 */
QPointF IngeScapeEditorController::getGlobalMousePosition()
{
    return QCursor::pos();
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
                if (
                    (_licensesC != nullptr)
                    &&
                    (_licensesC->mergedLicense() != nullptr)
                    &&
                    _licensesC->mergedLicense()->editorLicenseValidity()
                    )
                {
                    if (_modelManager != nullptr)
                    {
                        bool succeeded = _modelManager->importAgentOrAgentsListFromFilePath(filePath);
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
 * @param models
 */
void IngeScapeEditorController::_onOpenLogStreamOfAgents(QList<AgentM*> models)
{
    if (!models.isEmpty())
    {
        for (AgentM* model : models)
        {
            if ((model != nullptr) && model->isON())
            {
                QString subscriberAddress = QString("tcp://%1:%2").arg(model->address(), model->loggerPort());

                qDebug() << "Open the 'Log Stream' of" << model->name() << "(Address:" << model->address() << "+ Logger Port:" << model->loggerPort() << "--> Subscriber Address:" << subscriberAddress << ")";

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
                    logStreamController = new LogStreamController(model->name(), model->hostname(), subscriberAddress, this);

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

    if ((_networkC != nullptr) && (_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON()
            && !jsonDocument.isNull() && !jsonDocument.isEmpty())
    {
        QString jsonString = QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact));

        QStringList commandAndParameters;

        // Add the command
        commandAndParameters.append(command_StartRecord);

        // Add the record name
        //commandAndParameters.append(QString("Record-%1").arg(_currentPlatformName));
        commandAndParameters.append(_currentPlatformName);

        // Add the delta from the start time of the TimeLine
        int deltaTimeFromTimeLineStart = 0;

        if (_scenarioC != nullptr) {
            deltaTimeFromTimeLineStart = _scenarioC->currentTime().msecsSinceStartOfDay();
        }
        commandAndParameters.append(QString::number(deltaTimeFromTimeLineStart));

        // Add the content of the JSON file
        commandAndParameters.append(jsonString);

        // Send the command, parameters and the content of the JSON file to the recorder
        _networkC->sendCommandWithJsonToRecorder(_recordsSupervisionC->peerIdOfRecorder(), commandAndParameters);
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
        // First, clear the current platform by deleting all existing data
        clearCurrentPlatform();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonPlatform.toUtf8());

        // Load the platform from JSON
        bool success = _loadPlatformFromJSON(jsonDocument);
        if (success)
        {
            // Update the current platform name
            if (!recordName.isEmpty())
            {
                setcurrentPlatformName(recordName);

                //_currentPlatformFilePath = QDir(_platformDirectoryPath).absoluteFilePath(recordName);
                _currentPlatformFilePath = QString("%1%2.json").arg(_platformDirectoryPath, recordName);

                sethasAPlatformBeenLoadedByUser(true);
            }

            if (_scenarioC != nullptr)
            {
                // Update the current time
                _scenarioC->setcurrentTime(QTime::fromMSecsSinceStartOfDay(deltaTimeFromTimeLineStart));

                // FIXME TODO jsonExecutedActions
                //qDebug() << "jsonExecutedActions" << jsonExecutedActions;

                // Import the executed actions for this scenario from JSON
                _scenarioC->importExecutedActionsFromJson(jsonExecutedActions.toUtf8());
            }

            // Notify QML to reset view
            Q_EMIT resetMappindAndTimeLineViews();
        }
        else
        {
            qCritical() << "The loading of the replay failed !";
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
    if ((_networkC != nullptr) && !_peerIdOfExpe.isEmpty())
    {
        // Reply by sending the command execution status to Expe
        _networkC->sendCommandExecutionStatusToExpe(_peerIdOfExpe, command_LoadPlatformFile, platformFilePath, static_cast<int>(success));
    }
    else
    {
        qWarning() << "Peer Id of Expe is empty" << _peerIdOfExpe;
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

    /*if ((_networkC != nullptr) && !_peerIdOfExpe.isEmpty())
    {
        // Reply by sending the command execution status to Expe
        _networkC->sendCommandExecutionStatusToExpe(_peerIdOfExpe, command_LoadPlatformFile, platformFilePath, static_cast<int>(success));
    }
    else {
        qWarning() << "Peer Id of Expe is empty" << _peerIdOfExpe;
    }*/
}


/**
 * @brief Slot called when the state of the TimeLine updated
 * @param state
 */
void IngeScapeEditorController::_onTimeLineStateUpdated(QString state)
{
    if (_networkC != nullptr)
    {
        // Add the delta from the start time of the TimeLine
        int deltaTimeFromTimeLineStart = 0;

        if (_scenarioC != nullptr) {
            deltaTimeFromTimeLineStart = _scenarioC->currentTime().msecsSinceStartOfDay();
        }

        QString notificationAndParameters = QString("%1=%2|%3").arg(notif_TimeLineState, state, QString::number(deltaTimeFromTimeLineStart));

        // Notify the Recorder app
        if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON())
        {
            _networkC->sendMessageToPeerId(_recordsSupervisionC->peerIdOfRecorder(), notificationAndParameters);
        }

        // Notify the Expe app
        if (!_peerIdOfExpe.isEmpty())
        {
            _networkC->sendMessageToPeerId(_peerIdOfExpe, notificationAndParameters);
        }
    }
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
        // Call the private slot (called when a command must be sent on the network to a recorder)
        _onCommandAskedToRecorder(command_StopRecord);
    }
    // Unknown
    else {
        qCritical() << "Unknown state" << state << "so we cannot update the Record !";
    }
}


/**
 * @brief Slot called when a command must be sent on the network to a recorder
 * @param commandAndParameters
 */
void IngeScapeEditorController::_onCommandAskedToRecorder(QString commandAndParameters)
{
    if ((_networkC != nullptr) && (_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON())
    {
        _networkC->onCommandAskedToRecorder(_recordsSupervisionC->peerIdOfRecorder(), commandAndParameters);
    }
}


/**
 * @brief Slot called when an expe enter the network
 * @param peerId
 * @param peerName
 * @param ipAddress
 * @param hostname
 */
void IngeScapeEditorController::_onExpeEntered(QString peerId, QString peerName, QString ipAddress, QString hostname)
{
    qInfo() << "Expe entered (" << peerId << ")" << peerName << "on" << hostname << "(" << ipAddress << ")";

    // Check that the peer id of the Expe is empty
    if (_peerIdOfExpe.isEmpty() && !peerId.isEmpty() && !peerName.isEmpty())
    {
        setpeerIdOfExpe(peerId);
        setpeerNameOfExpe(peerName);
    }
    else {
        qCritical() << "We are already connected to an expe:" << _peerNameOfExpe << "(" << _peerIdOfExpe << ")";
    }
}


/**
 * @brief Slot called when an expe quit the network
 * @param peerId
 * @param peerName
 */
void IngeScapeEditorController::_onExpeExited(QString peerId, QString peerName)
{
    qInfo() << "Expe exited (" << peerId << ")" << peerName;

    // Check that the peer id matches
    if (_peerIdOfExpe == peerId)
    {
        // Clear
        setpeerIdOfExpe("");
        setpeerNameOfExpe("");
    }
}


/**
 * @brief Slot called when the licenses have been updated
 */
void IngeScapeEditorController::_onLicensesUpdated()
{
    qDebug() << "on License Updated";

    // Restart IngeScape
    // (Do not clear the current platform, do not check available network devices)
    _restartIngeScape(false, false);
}


/**
 * @brief Called when our network device is not available
 */
void IngeScapeEditorController::_onNetworkDeviceIsNotAvailable()
{
    qDebug() << Q_FUNC_INFO;

    // Stop IngeScape if needed
    if ((_networkC != nullptr) && _networkC->isStarted())
    {
        _stopIngeScape(false);
    }
    // Else: our agent is not started, we don't need to stop it
}


/**
 * @brief Called when our network device is available again
 */
void IngeScapeEditorController::_onNetworkDeviceIsAvailableAgain()
{
    qDebug() << Q_FUNC_INFO;

    // Start IngeScape
    // => we don't need to check available network devices
    _startIngeScape(false);
}


/**
 * @brief Called when the IP address of our network device has changed
 */
void IngeScapeEditorController::_onNetworkDeviceIpAddressHasChanged()
{
    qDebug() << Q_FUNC_INFO;

    // Restart IngeScape
    // (Do not clear platform, do no check available network devices)
    _restartIngeScape(false, false);
}


/**
 * @brief Called when our machine will go to sleep
 */
void IngeScapeEditorController::_onSystemSleep()
{
    qDebug() << Q_FUNC_INFO;

    // Stop monitoring to save energy
    if (_networkC != nullptr)
    {
        _networkC->stopMonitoring();
    }

    // Stop IngeScape
    _stopIngeScape(false);
}


/**
 * @brief Called when our machine did wake from sleep
 */
void IngeScapeEditorController::_onSystemWake()
{
    if (_networkC != nullptr)
    {
        // Start IngeScape
        // => we need to check available network devices
        _startIngeScape(true);
    }
}



/**
 * @brief Called when a network configuration is added, removed or changed
 */
void IngeScapeEditorController::_onSystemNetworkConfigurationsUpdated()
{
    if (_networkC != nullptr)
    {
        _networkC->updateAvailableNetworkDevices();
    }
}




/**
 * @brief Load the platform from a JSON file
 * @param platformFilePath
 * @return
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

                // Notify QML to reset view
                Q_EMIT resetMappindAndTimeLineViews();
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
 * @brief Clear our current platform and load a new platform from a given file
 * @param platformFilePath
 * @return
 */
bool IngeScapeEditorController::_clearAndLoadPlatformFromFile(QString platformFilePath)
{
    bool succeeded = false;

    if (!platformFilePath.isEmpty())
    {
        // First, clear the current platform by deleting all existing data
        clearCurrentPlatform();

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

        // Force our global mapping to CONTROLLED
        if (_modelManager != nullptr)
        {
            _modelManager->setisMappingControlled(true);
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
        qInfo() << "Save the current platform to JSON file" << platformFilePath;

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
        if ((_modelManager != nullptr) && jsonRoot.contains("agents"))
        {
            _modelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray(), versionJsonPlatform);
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

    if (_jsonHelper != nullptr)
    {
        QJsonObject platformJsonObject;

        platformJsonObject.insert("version", VERSION_JSON_PLATFORM);

        // Save the agents
        if (_modelManager != nullptr)
        {
            // Export the agents into JSON
            QJsonArray arrayOfAgents = _modelManager->exportAgentsToJSON();

            if (!arrayOfAgents.isEmpty()) {
                platformJsonObject.insert("agents", arrayOfAgents);
            }
        }

        // Save the mapping
        if (_agentsMappingC != nullptr)
        {
            // Export the global mapping (of agents) into JSON
            QJsonArray arrayOfAgentsInMapping = _agentsMappingC->exportGlobalMappingToJSON();

            if (!arrayOfAgentsInMapping.isEmpty()) {
                platformJsonObject.insert("mapping", arrayOfAgentsInMapping);
            }
        }

        // Save the scenario
        if (_scenarioC != nullptr)
        {
            // actions list
            // actions list in the palette
            // actions list in the timeline
            QJsonObject jsonScenario = _jsonHelper->exportScenario(_scenarioC->actionsList()->toList(),
                                                                   _scenarioC->actionsInPaletteList()->toList(),
                                                                   _scenarioC->actionsInTimeLine()->toList());

            platformJsonObject.insert("scenario", jsonScenario);
        }

        jsonDocument = QJsonDocument(platformJsonObject);
    }
    return jsonDocument;
}


/**
 * @brief Stop IngeScape
 *
 * @param hasToClearPlatform
 */
void IngeScapeEditorController::_stopIngeScape(bool hasToClearPlatform)
{
    if ((_networkC != nullptr) && (_modelManager != nullptr))
    {
        if (hasToClearPlatform)
        {
            qInfo() << "Stop the network on" << _networkDevice << "with" << _port << "(and CLEAR the current platform)";
        }
        else
        {
            qInfo() << "Stop the network on" << _networkDevice << "with" << _port << "(and KEEP the current platform)";
        }

        // Save states of our mapping if needed
        _beforeNetworkStop_isMappingConnected = _modelManager->isMappingConnected();
        _beforeNetworkStop_isMappingControlled = _modelManager->isMappingControlled();


        // Disable mapping
        _modelManager->setisMappingConnected(false);
        _modelManager->setisMappingControlled(false);

        // Stop our IngeScape agent
        _networkC->stop();

        // We don't see itself
        _networkC->setnumberOfEditors(1);

        // Simulate an exit for each agent ON
        _modelManager->simulateExitForEachAgentON();

        // Simulate an exit for each launcher
        _modelManager->simulateExitForEachLauncher();

        // Simulate an exit for the recorder
        //_modelManager->simulateExitForRecorder();
        if ((_recordsSupervisionC != nullptr) && _recordsSupervisionC->isRecorderON())
        {
            _recordsSupervisionC->onRecorderExited(_recordsSupervisionC->peerIdOfRecorder(), _recordsSupervisionC->peerNameOfRecorder());
        }

        // Has to clear the current platform
        if (hasToClearPlatform)
        {
            // Clear the current platform by deleting all existing data
            clearCurrentPlatform();
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
bool IngeScapeEditorController::_startIngeScape(bool checkAvailableNetworkDevices)
{
    bool success = false;

    // Reset the error message
    seterrorMessageWhenConnectionFailed("");

    if ((_networkC != nullptr) && (_modelManager != nullptr))
    {
        if (checkAvailableNetworkDevices)
        {
            // Update the list of available network devices
            _networkC->updateAvailableNetworkDevices();

            // There is only one available network device, we use it !
            if (_networkC->availableNetworkDevices().count() == 1)
            {
                _networkDevice = _networkC->availableNetworkDevices().at(0);
            }
        }


        // Start our IngeScape agent with the network device and the port
        success = _networkC->start(_networkDevice, _ipAddress, _port);

        if (success)
        {
            // Re-enable mapping
            _modelManager->setisMappingConnected(_beforeNetworkStop_isMappingConnected);
            _modelManager->setisMappingControlled(_beforeNetworkStop_isMappingControlled);
        }
    }

    if (!success)
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
bool IngeScapeEditorController::_restartIngeScape(bool hasToClearPlatform, bool checkAvailableNetworkDevices)
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
