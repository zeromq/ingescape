/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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
#include <misc/ingescapeeditorsettings.h>
#include <misc/ingescapeeditorutils.h>


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
    _modelManager(nullptr),
    _agentsSupervisionC(nullptr),
    _agentsMappingC(nullptr),
    _networkC(nullptr),
    _scenarioC(nullptr),
    _valuesHistoryC(nullptr),
    _timeLineC(nullptr),
    _terminationSignalWatcher(nullptr),
    _jsonHelper(nullptr),
    _platformDirectoryPath(""),
    _platformDefaultFilePath("")
{
    qInfo() << "New IngeScape Editor Controller";

    //
    // Snapshots directory
    //
    QString snapshotsDirectoryPath = IngeScapeEditorUtils::getSnapshotsPath();
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
    IngeScapeEditorSettings& settings = IngeScapeEditorSettings::Instance();

    // Settings about the "Network"
    settings.beginGroup("network");
    _networkDevice = settings.value("networkDevice", QVariant("")).toString();
    _ipAddress = settings.value("ipAddress", QVariant("")).toString();
    _port = settings.value("port", QVariant(0)).toInt();
    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);
    settings.endGroup();

    // Settings about "Debug"
    settings.beginGroup("debug");
    _isAvailableModelVisualizer = settings.value("modelVisualizer", QVariant(false)).toBool();
    qDebug() << "The Model/View Model Visualizer is available ?" << _isAvailableModelVisualizer;
    settings.endGroup();


    // Root directory
    QString rootPath = IngeScapeEditorUtils::getRootPath();
    QDir rootDir(rootPath);
    if (!rootDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << rootPath << "' !";
    }

    // Directory for agents lists
    /*QString agentsListPath = IngeScapeEditorUtils::getAgentsListPath();
    QDir agentsListDir(agentsListPath);
    if (!agentsListDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsListPath << "' !";
    }*/

    // Directory for platform files
    QString platformPath = IngeScapeEditorUtils::getPlatformsPath();

    QDir platformDir(platformPath);
    if (!platformDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << platformPath << "' !";
    }
    else
    {
        _platformDirectoryPath = platformPath;

        // Init the path to the JSON file to load the last platform
        _platformDefaultFilePath = QString("%1last.json").arg(_platformDirectoryPath);
    }


    // Create the helper to manage JSON files
    _jsonHelper = new JsonHelper(this);

    //
    // Create sub-controllers
    //

    // Create the manager for the data model of INGESCAPE
    _modelManager = new IngeScapeModelManager(_jsonHelper, rootPath, this);

    // Create the controller for network communications
    _networkC = new NetworkController(this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, _jsonHelper, this);

    // Create the controller for hosts supervision
    _hostsSupervisionC = new HostsSupervisionController(_modelManager, this);

    // Create the controller for records supervision
    _recordsSupervisionC = new RecordsSupervisionController(_modelManager, _jsonHelper, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, _jsonHelper, this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(_modelManager, _jsonHelper, this);

    // Create the controller for the history of values
    _valuesHistoryC = new ValuesHistoryController(_modelManager, this);

    // Create the controller for the time line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::agentEntered, _modelManager, &IngeScapeModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &IngeScapeModelManager::onAgentExited);
    connect(_networkC, &NetworkController::launcherEntered, _modelManager, &IngeScapeModelManager::onLauncherEntered);
    connect(_networkC, &NetworkController::launcherExited, _modelManager, &IngeScapeModelManager::onLauncherExited);
    connect(_networkC, &NetworkController::recorderEntered, _recordsSupervisionC, &RecordsSupervisionController::onRecorderEntered);
    connect(_networkC, &NetworkController::recorderExited, _recordsSupervisionC, &RecordsSupervisionController::onRecorderExited);

    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &IngeScapeModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &IngeScapeModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::valuePublished, _modelManager, &IngeScapeModelManager::onValuePublished);
    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &IngeScapeModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::canBeFrozenFromAgentUpdated, _modelManager, &IngeScapeModelManager::onCanBeFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isFrozenFromAgentUpdated, _modelManager, &IngeScapeModelManager::onIsFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isMutedFromOutputOfAgentUpdated, _modelManager, &IngeScapeModelManager::onIsMutedFromOutputOfAgentUpdated);
    connect(_networkC, &NetworkController::agentStateChanged, _modelManager, &IngeScapeModelManager::onAgentStateChanged);
    connect(_networkC, &NetworkController::agentHasLogInStream, _modelManager, &IngeScapeModelManager::onAgentHasLogInStream);
    connect(_networkC, &NetworkController::agentHasLogInFile, _modelManager, &IngeScapeModelManager::onAgentHasLogInFile);
    connect(_networkC, &NetworkController::agentLogFilePath, _modelManager, &IngeScapeModelManager::onAgentLogFilePath);
    connect(_networkC, &NetworkController::agentDefinitionFilePath, _modelManager, &IngeScapeModelManager::onAgentDefinitionFilePath);
    connect(_networkC, &NetworkController::agentMappingFilePath, _modelManager, &IngeScapeModelManager::onAgentMappingFilePath);

    connect(_networkC, &NetworkController::allRecordsReceived, _recordsSupervisionC, &RecordsSupervisionController::onAllRecordsReceived);
    connect(_networkC, &NetworkController::addedRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onAddedRecord);
    connect(_networkC, &NetworkController::deletedRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onDeletedRecord);
    connect(_networkC, &NetworkController::loadingRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onLoadingRecord);
    connect(_networkC, &NetworkController::loadingRecordReceived, this, &IngeScapeEditorController::_onLoadingRecord);
    connect(_networkC, &NetworkController::loadedRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onLoadedRecord);
    connect(_networkC, &NetworkController::endOfRecordReceived, _recordsSupervisionC, &RecordsSupervisionController::onEndOfRecord);

    connect(_networkC, &NetworkController::highlightLink, _agentsMappingC, &AgentsMappingController::onHighlightLink);


    // Connect to signals from the model manager
    connect(_modelManager, &IngeScapeModelManager::isMappingActivatedChanged, _agentsMappingC, &AgentsMappingController::onIsMappingActivatedChanged);
    connect(_modelManager, &IngeScapeModelManager::isMappingControlledChanged, _agentsMappingC, &AgentsMappingController::onIsMappingControlledChanged);

    connect(_modelManager, &IngeScapeModelManager::agentModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onAgentModelHasBeenCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onAgentModelWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::hostModelHasBeenCreated, _hostsSupervisionC, &HostsSupervisionController::onHostModelHasBeenCreated);
    connect(_modelManager, &IngeScapeModelManager::hostModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onHostModelWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::agentsGroupedByNameHasBeenCreated, _valuesHistoryC, &ValuesHistoryController::onAgentsGroupedByNameHasBeenCreated);
    connect(_modelManager, &IngeScapeModelManager::agentsGroupedByNameHasBeenCreated, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameHasBeenCreated);
    connect(_modelManager, &IngeScapeModelManager::agentsGroupedByNameWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentsGroupedByNameWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::agentsGroupedByDefinitionHasBeenCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionHasBeenCreated);
    connect(_modelManager, &IngeScapeModelManager::agentsGroupedByDefinitionWillBeDeleted, _agentsSupervisionC, &AgentsSupervisionController::onAgentsGroupedByDefinitionWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::addInputsToEditorForOutputs, _networkC, &NetworkController::onAddInputsToEditorForOutputs);
    connect(_modelManager, &IngeScapeModelManager::removeInputsToEditorForOutputs, _networkC, &NetworkController::onRemoveInputsToEditorForOutputs);


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


    // Connect to signals from the controller of the scenario
    connect(_scenarioC, &ScenarioController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_scenarioC, &ScenarioController::commandAskedToRecorder, this, &IngeScapeEditorController::_onCommandAskedToRecorder);
    connect(_scenarioC, &ScenarioController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);

    // Connect to signals from the time line time line visible range change to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::onTimeRangeChanged);

    // Connect to signals from Record supervision controller
    connect(_recordsSupervisionC, &RecordsSupervisionController::commandAskedToRecorder, _networkC, &NetworkController::onCommandAskedToRecorder);
    connect(_recordsSupervisionC, &RecordsSupervisionController::startToRecord, this, &IngeScapeEditorController::_onStartToRecord);


    if (!_platformDefaultFilePath.isEmpty())
    {
        // Load the platform (agents, mappings, actions, palette, timeline actions)
        // from the default file "last.json"
        _loadPlatformFromFile(_platformDefaultFilePath);
    }


    // Create a fake launcher for fake agents
    _modelManager->onLauncherEntered("0", HOSTNAME_NOT_DEFINED, "0.0.0.0", "");


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
        // Start a (single shot) timer
        QTimer::singleShot(1000, _agentsMappingC, &AgentsMappingController::onNetworkStartedSince1sec);

        // Initialize platform from online mapping
        _modelManager->setisMappingActivated(true);
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
    //QThread::msleep(2000);
}


/**
 * @brief Destructor
 */
IngeScapeEditorController::~IngeScapeEditorController()
{
    // Delete all log stream viewers
    _openedLogStreamControllers.deleteAllItems();


    //
    // Clean-up our TerminationSignalWatcher first
    //
    if (_terminationSignalWatcher != nullptr)
    {
        disconnect(_terminationSignalWatcher, 0);
        delete _terminationSignalWatcher;
        _terminationSignalWatcher = nullptr;
    }

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

    if (_agentsSupervisionC != nullptr)
    {
        disconnect(_agentsSupervisionC);

        AgentsSupervisionController* temp = _agentsSupervisionC;
        setagentsSupervisionC(nullptr);
        delete temp;
        temp = nullptr;
    }

    if (_modelManager != nullptr)
    {
        disconnect(_modelManager);

        IngeScapeModelManager* temp = _modelManager;
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

    if (_scenarioC != nullptr)
    {
        disconnect(_scenarioC);

        ScenarioController* temp = _scenarioC;
        setscenarioC(nullptr);
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
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new IngeScapeEditorController();
}


/**
 * @brief Load a platform (agents, mappings, actions, palette, timeline actions)
 * from the file selected by the user
 */
void IngeScapeEditorController::loadPlatformFromSelectedFile()
{
    // "File Dialog" to get the files (paths) to open
    QString platformFilePath = QFileDialog::getOpenFileName(nullptr,
                                                            "Open platform",
                                                            _platformDirectoryPath,
                                                            "JSON (*.json)");

    if (!platformFilePath.isEmpty())
    {
        // First, clear the current platform by deleting all existing data
        clearCurrentPlatform();

        // Load the platform from JSON file
        _loadPlatformFromFile(platformFilePath);

        // Force our global mapping to CONTROLLED
        if (_modelManager != nullptr) {
            _modelManager->setisMappingControlled(true);
        }
    }
    else {
        qDebug() << "Platform file path is empty, nothing to do";
    }
}


/**
 * @brief Save the platform (agents, mappings, actions, palette, timeline actions)
 * to the file selected by the user
 */
void IngeScapeEditorController::savePlatformToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString platformFilePath = QFileDialog::getSaveFileName(nullptr,
                                                            "Save platform",
                                                            _platformDirectoryPath,
                                                            "JSON (*.json)");

    if (!platformFilePath.isEmpty()) {
        // Save the platform to JSON file
        _savePlatformToFile(platformFilePath);
    }
}


/**
 * @brief Save the platform (agents, mappings, actions, palette, timeline actions)
 * to the default file "last.json"
 */
void IngeScapeEditorController::savePlatformToDefaultFile()
{
    if (!_platformDefaultFilePath.isEmpty())
    {
        // Save the platform to JSON file
        _savePlatformToFile(_platformDefaultFilePath);
    }
}


/**
 * @brief Clear the current platform (agents, mappings, actions, palette, timeline actions)
 * by deleting all existing data
 */
void IngeScapeEditorController::clearCurrentPlatform()
{
    // Clear the current mapping
    if (_agentsMappingC != nullptr) {
        _agentsMappingC->clearMapping();
    }

    // Clear the current scenario
    if (_scenarioC != nullptr) {
        _scenarioC->clearScenario();
    }

    // Delete agents OFF
    if (_modelManager != nullptr) {
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
    // Save the platform to the default file
    savePlatformToDefaultFile();
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

    // Reset the error message
    seterrorMessageWhenConnectionFailed("");

    if ((_networkC != nullptr) && (_modelManager != nullptr))
    {
        bool isInt = false;
        int nPort = strPort.toInt(&isInt);
        if (isInt && (nPort > 0))
        {
            // Port and Network device have not changed...
            if ((nPort == _port) && (networkDevice == _networkDevice))
            {
                // Nothing to do
                success = true;
            }
            // Port and Network device
            else
            {
                if (hasToClearPlatform) {
                    qInfo() << "Restart the network on" << networkDevice << "with" << strPort << "(and CLEAR the current platform)";
                }
                else
                {
                    qInfo() << "Restart the network on" << networkDevice << "with" << strPort << "(and KEEP the current platform)";
                }

                _modelManager->setisMappingActivated(false);
                _modelManager->setisMappingControlled(false);

                // Stop our INGESCAPE agent
                _networkC->stop();

                // Update properties
                setnetworkDevice(networkDevice);
                setport(nPort);

                // Update settings file
                IngeScapeEditorSettings& settings = IngeScapeEditorSettings::Instance();
                settings.beginGroup("network");
                settings.setValue("networkDevice", networkDevice);
                settings.setValue("port", nPort);
                settings.endGroup();
                // Save new values
                settings.sync();


                // Simulate an exit for agent ON
                _modelManager->simulateExitForEachAgentON();

                // Has to clear the current platform
                if (hasToClearPlatform)
                {
                    // Clear the current platform by deleting all existing data
                    clearCurrentPlatform();
                }

                // Start our INGESCAPE agent with the network device and the port
                success = _networkC->start(networkDevice, "", nPort);

                if (success) {
                    _modelManager->setisMappingActivated(true);
                }
            }
        }
        else {
            if (!isInt) {
                qWarning() << "Port" << strPort << "is not an int !";
            }
            else if (nPort <= 0) {
                qWarning() << "Port" << strPort << "is negative or null !";
            }
        }
    }

    if (!success) {
        seterrorMessageWhenConnectionFailed(tr("Failed to connect with network device %1 on port %2").arg(networkDevice, strPort));
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
                    LogStreamController* logStreamController = new LogStreamController(model->name(), model->hostname(), subscriberAddress, this);

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
        commandAndParameters.append(command_StartToRecord);

        // Add the delta of the start time from the Time Line
        int deltaTimeFromTimeLine = 0;

        if (_scenarioC != nullptr) {
            deltaTimeFromTimeLine = _scenarioC->currentTime().msecsSinceStartOfDay();
        }
        commandAndParameters.append(QString::number(deltaTimeFromTimeLine));

        // Add the content of the JSON file
        commandAndParameters.append(jsonString);

        // Send the command, parameters and the content of the JSON file to the recorder
        _networkC->sendCommandWithJsonToRecorder(_recordsSupervisionC->peerIdOfRecorder(), commandAndParameters);
    }
}


/**
 * @brief Slot called when a record is loading
 * @param deltaTimeFromTimeLine
 * @param jsonPlatform
 * @param jsonExecutedActions
 */
void IngeScapeEditorController::_onLoadingRecord(int deltaTimeFromTimeLine, QString jsonPlatform, QString jsonExecutedActions)
{
    if ((deltaTimeFromTimeLine >= 0) && !jsonPlatform.isEmpty())
    {
        // First, clear the current platform by deleting all existing data
        clearCurrentPlatform();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonPlatform.toUtf8());

        // Load the platform from JSON
        _loadPlatformFromJSON(jsonDocument);

        if (_scenarioC != nullptr)
        {
            // Update the current time
            _scenarioC->setcurrentTime(QTime::fromMSecsSinceStartOfDay(deltaTimeFromTimeLine));

            // FIXME TODO jsonExecutedActions
            qDebug() << "jsonExecutedActions" << jsonExecutedActions;

            // Import the executed actions for this scenario from JSON
            _scenarioC->importExecutedActionsFromJson(jsonExecutedActions.toUtf8());
        }

        // Notify QML to reset view
        Q_EMIT resetMappindAndTimeLineViews();
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
 * @brief Load the platform from a JSON file
 * @param platformFilePath
 */
void IngeScapeEditorController::_loadPlatformFromFile(QString platformFilePath)
{
    if (!platformFilePath.isEmpty())
    {
        qInfo() << "Load the platform from JSON file" << platformFilePath;

        QFile jsonFile(platformFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

                // Load the platform from JSON
                _loadPlatformFromJSON(jsonDocument);

                // Notify QML to reset view
                Q_EMIT resetMappindAndTimeLineViews();
            }
            else {
                qCritical() << "Can not open file" << platformFilePath;
            }
        }
        else {
            qWarning() << "There is no file" << platformFilePath;
        }
    }
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
  */
void IngeScapeEditorController::_loadPlatformFromJSON(QJsonDocument jsonDocument)
{
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

        // Import the global mapping (of agents) from JSON
        if ((_agentsMappingC != nullptr) && jsonRoot.contains("mapping"))
        {
            _agentsMappingC->importMappingFromJson(jsonRoot.value("mapping").toArray());
        }

        // Import the scenario from JSON
        if ((_scenarioC != nullptr) && jsonRoot.contains("scenario"))
        {
            _scenarioC->importScenarioFromJson(jsonRoot.value("scenario").toObject());
        }
    }
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
