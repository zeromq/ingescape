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
 *      Bruno Lemenicier   <lemenicier@ingenuity.io>
 *
 */

#include "controller/ingescapeeditorcontroller.h"

#include "misc/ingescapeeditorsettings.h"
#include "misc/ingescapeeditorutils.h"

#include <I2Quick.h>

#include <QFileDialog>
#include <QThread>
#include <QApplication>
#include <QCursor>


/**
 * @brief Constructor
 * @param parent
 */
IngeScapeEditorController::IngeScapeEditorController(QObject *parent) : QObject(parent),
    _networkDevice(""),
    _ipAddress(""),
    _port(0),
    _errorMessageWhenConnectionFailed(""),
    _modelManager(NULL),
    _agentsSupervisionC(NULL),
    _agentsMappingC(NULL),
    _networkC(NULL),
    _scenarioC(NULL),
    _valuesHistoryC(NULL),
    _timeLineC(NULL),
    _launcherManager(NULL),
    _terminationSignalWatcher(NULL),
    _jsonHelper(NULL),
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
    _networkDevice = settings.value("networkDevice").toString();
    _ipAddress = settings.value("ipAddress").toString();
    _port = settings.value("port").toInt();
    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);
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
    _hostsSupervisionC = new HostsSupervisionController(this);

    // Create the controller for records supervision
    _recordsSupervisionC = new RecordsSupervisionController(_modelManager, _jsonHelper, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, _jsonHelper, rootPath, this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(_modelManager, _jsonHelper, this);

    // Create the controller for the history of values
    _valuesHistoryC = new ValuesHistoryController(_modelManager, this);

    // Create the controller for the time line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the manager for launchers of INGESCAPE agents
    //_launcherManager = new IngeScapeLauncherManager(this);
    _launcherManager = &IngeScapeLauncherManager::Instance();

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

    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _hostsSupervisionC, &HostsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _valuesHistoryC, &ValuesHistoryController::onAgentModelCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentModelWillBeDeleted);
    connect(_modelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onAgentModelWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::activeAgentDefined, _agentsMappingC, &AgentsMappingController::onActiveAgentDefined);
    connect(_modelManager, &IngeScapeModelManager::activeAgentMappingDefined, _agentsMappingC, &AgentsMappingController::onActiveAgentMappingDefined);
    connect(_modelManager, &IngeScapeModelManager::mapped, _agentsMappingC, &AgentsMappingController::onMapped);
    connect(_modelManager, &IngeScapeModelManager::unmapped, _agentsMappingC, &AgentsMappingController::onUnmapped);

    connect(_modelManager, &IngeScapeModelManager::addInputsToEditorForOutputs, _networkC, &NetworkController::onAddInputsToEditorForOutputs);
    connect(_modelManager, &IngeScapeModelManager::removeInputsToEditorForOutputs, _networkC, &NetworkController::onRemoveInputsToEditorForOutputs);

    connect(_modelManager, &IngeScapeModelManager::agentExecutionOnHost, _hostsSupervisionC, &HostsSupervisionController::onAgentExecutionOnHost);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgentAboutOutput, _networkC, &NetworkController::onCommandAskedToAgentAboutOutput);
    connect(_agentsSupervisionC, &AgentsSupervisionController::identicalAgentModelReplaced, _agentsMappingC, &AgentsMappingController::onIdenticalAgentModelReplaced);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openValuesHistoryOfAgent, _valuesHistoryC, &ValuesHistoryController::filterValuesToShowOnlyAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openLogStreamOfAgents, this, &IngeScapeEditorController::_onOpenLogStreamOfAgents);

    // Connect to signals from the ingescape launcher manager
    connect(_launcherManager, &IngeScapeLauncherManager::hostModelCreated, _hostsSupervisionC, &HostsSupervisionController::onHostModelCreated);
    connect(_launcherManager, &IngeScapeLauncherManager::hostModelWillBeRemoved, _hostsSupervisionC, &HostsSupervisionController::onHostModelWillBeRemoved);

    // Connect to signals from the controller for supervision of hosts
    connect(_hostsSupervisionC, &HostsSupervisionController::commandAskedToHost, _networkC, &NetworkController::onCommandAskedToLauncher);

    // Connect to signals from the controller for mapping of agents
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);
    connect(_agentsMappingC, &AgentsMappingController::agentInMappingAdded, _scenarioC, &ScenarioController::onAgentInMappingAdded);
    connect(_agentsMappingC, &AgentsMappingController::agentInMappingRemoved, _scenarioC, &ScenarioController::onAgentInMappingRemoved);


    // Connect to signals from the controller of the scenario
    connect(_scenarioC, &ScenarioController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_scenarioC, &ScenarioController::commandAskedToRecorder, _networkC, &NetworkController::onCommandAskedToRecorder);
    connect(_scenarioC, &ScenarioController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);

    // Connect to signals from the time line time line visible range change to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::onTimeRangeChanged);

    // Connect to signals from Record supervision controller
    connect(_recordsSupervisionC, &RecordsSupervisionController::commandAskedToRecorder, _networkC, &NetworkController::onCommandAskedToRecorder);
    connect(_recordsSupervisionC, &RecordsSupervisionController::startToRecord, this, &IngeScapeEditorController::_onStartToRecord);


    // Load the platform (agents, mappings, actions, palette, timeline actions)
    // from the default file "last.json"
    loadPlatformFromDefaultFile();

    // Start our INGESCAPE agent with a network device (or an IP address) and a port
    bool isStarted = _networkC->start(_networkDevice, _ipAddress, _port);

    if (isStarted)
    {
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

                        if (QApplication::instance() != NULL)
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
    if (_terminationSignalWatcher != NULL)
    {
        disconnect(_terminationSignalWatcher, 0);
        delete _terminationSignalWatcher;
        _terminationSignalWatcher = NULL;
    }


    //
    // Clean-up sub-controllers
    //
    if (_launcherManager != NULL)
    {
        disconnect(_launcherManager);

        //IngeScapeLauncherManager* temp = _launcherManager;
        setlauncherManager(NULL);
        //delete temp;
        //temp = NULL;
    }

    if (_timeLineC != NULL)
    {
        disconnect(_timeLineC);

        AbstractTimeActionslineScenarioViewController* temp = _timeLineC;
        settimeLineC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_valuesHistoryC != NULL)
    {
        disconnect(_valuesHistoryC);

        ValuesHistoryController* temp = _valuesHistoryC;
        setvaluesHistoryC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_agentsMappingC != NULL)
    {
        disconnect(_agentsMappingC);

        AgentsMappingController* temp = _agentsMappingC;
        setagentsMappingC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_agentsSupervisionC != NULL)
    {
        disconnect(_agentsSupervisionC);

        AgentsSupervisionController* temp = _agentsSupervisionC;
        setagentsSupervisionC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_modelManager != NULL)
    {
        disconnect(_modelManager);

        IngeScapeModelManager* temp = _modelManager;
        setmodelManager(NULL);
        delete temp;
        temp = NULL;
    }

    if (_networkC != NULL)
    {
        disconnect(_networkC);

        NetworkController* temp = _networkC;
        setnetworkC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_scenarioC != NULL)
    {
        disconnect(_scenarioC);

        ScenarioController* temp = _scenarioC;
        setscenarioC(NULL);
        delete temp;
        temp = NULL;
    }

    // Delete json helper
    if (_jsonHelper != NULL)
    {
        delete _jsonHelper;
        _jsonHelper = NULL;
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
    QString platformFilePath = QFileDialog::getOpenFileName(NULL,
                                                                "Open platform",
                                                                _platformDirectoryPath,
                                                                "JSON (*.json)");

    // Load the platform from JSON file
    _loadPlatformFromFile(platformFilePath);
}


/**
 * @brief Load a platform (agents, mappings, actions, palette, timeline actions)
 * from the default file "last.json"
 */
void IngeScapeEditorController::loadPlatformFromDefaultFile()
{
    if (!_platformDefaultFilePath.isEmpty())
    {
        // Load the platform from JSON file
        _loadPlatformFromFile(_platformDefaultFilePath);
    }
}


/**
 * @brief Save the platform (agents, mappings, actions, palette, timeline actions)
 * to the file selected by the user
 */
void IngeScapeEditorController::savePlatformToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString platformFilePath = QFileDialog::getSaveFileName(NULL,
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
 * @brief Create a new platform (agents, mappings, actions, palette, timeline actions)
 * by deleting all existing data
 */
void IngeScapeEditorController::createNewPlatform()
{
    if (_agentsMappingC != NULL)
    {
        // Create a new mapping (clear the previous one)
        _agentsMappingC->createNewMapping();
    }

    if (_scenarioC != NULL)
    {
        // Reset scenario (clear the list of actions in the list, in the palette and in the timeline)
        _scenarioC->clearScenario();
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
  * @brief Check if we can delete an agent (view model) from the list in supervision
  *        Check dependencies in the mapping and in the actions (conditions, effects)
  * @param agentName
  */
bool IngeScapeEditorController::canDeleteAgentFromSupervision(QString agentName)
{
    bool canBeDeleted = true;

    // Check if the agent is in the curent mapping
    if ((_agentsMappingC != NULL) && !agentName.isEmpty())
    {
        AgentInMappingVM* agentInMapping = _agentsMappingC->getAgentInMappingFromName(agentName);
        if (agentInMapping != NULL) {
            canBeDeleted = false;
        }
    }

    // Check if the agent is in action condition or effect
    if (canBeDeleted && (_scenarioC != NULL)) {
        canBeDeleted = !_scenarioC->isAgentDefinedInActions(agentName);
    }

    return canBeDeleted;
}


/**
 * @brief Check if we can delete an agent (in mapping) from the mapping view
 *        Check dependencies in the actions (conditions, effects)
 * @param agentName
 */
bool IngeScapeEditorController::canDeleteAgentInMapping(QString agentName)
{
    bool canBeDeleted = true;

    // Check if the agent is in action condition or effect
    if (_scenarioC != NULL) {
        canBeDeleted = !_scenarioC->isAgentDefinedInActions(agentName);
    }

    return canBeDeleted;
}


/**
 * @brief Re-Start the network with a port and a network device
 * @param strPort
 * @param networkDevice
 * @return true when success
 */
bool IngeScapeEditorController::restartNetwork(QString strPort, QString networkDevice)
{
    bool success = false;

    // Reset the error message
    seterrorMessageWhenConnectionFailed("");

    if ((_networkC != NULL) && (_modelManager != NULL))
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
                qInfo() << "Restart the network on" << networkDevice << "with" << strPort;

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


                // Reset the list of launchers (hosts)
                if (_launcherManager != NULL) {
                    _launcherManager->reset();
                }

                // Reset the list of agents
                if (_hostsSupervisionC != NULL) {
                    _hostsSupervisionC->reset();
                }


                // Create a new empty platform by deleting all existing data
                createNewPlatform();

                // Simulate an exit for each agent
                _modelManager->simulateExitForEachActiveAgent();

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
    if ((definition != NULL) && (_modelManager != NULL) && _modelManager->openedDefinitions()->contains(definition))
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
    if ((actionEditorC != NULL) && (_scenarioC != NULL)) {
        _scenarioC->closeActionEditor(actionEditorC);
    }
}


/**
 * @brief Close a "Log Stream" controller
 * @param logStreamC
 */
void IngeScapeEditorController::closeLogStreamController(LogStreamController* logStreamC)
{
    if (logStreamC != NULL)
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
    if (window != NULL)
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
    if (window != NULL)
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
            if ((model != NULL) && model->isON())
            {
                QString subscriberAddress = QString("tcp://%1:%2").arg(model->address(), model->loggerPort());

                qDebug() << "Open the 'Log Stream' of" << model->name() << "(Address:" << model->address() << "+ Logger Port:" << model->loggerPort() << "--> Subscriber Address:" << subscriberAddress << ")";

                LogStreamController* logStreamController = NULL;

                for (LogStreamController* iterator : _openedLogStreamControllers.toList())
                {
                    if ((iterator != NULL) && (iterator->agentName() == model->name()) && (iterator->subscriberAddress() == subscriberAddress))
                    {
                        logStreamController = iterator;
                    }
                }

                if (logStreamController != NULL)
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

    if ((_networkC != NULL) && !jsonDocument.isNull() && !jsonDocument.isEmpty())
    {
        QString jsonString = QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Compact));

        QStringList commandAndParameters = QStringList();

        // Add the command
        commandAndParameters.append(command_StartToRecord);

        // Add the delta of the start time from the Time Line
        int deltaTimeFromTimeLine = 0;

        if (_scenarioC != NULL) {
            deltaTimeFromTimeLine = _scenarioC->currentTime().msecsSinceStartOfDay();
        }
        commandAndParameters.append(QString::number(deltaTimeFromTimeLine));

        // Add the content of the JSON file
        commandAndParameters.append(jsonString);

        // Send the command, parameters and the content of the JSON file to the recorder
        _networkC->sendCommandWithJsonToRecorder(commandAndParameters);
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
    // FIXME TODO jsonExecutedActions
    Q_UNUSED(jsonExecutedActions)

    if ((deltaTimeFromTimeLine >= 0) && !jsonPlatform.isEmpty())
    {
        qDebug() << "jsonExecutedActions" << jsonExecutedActions;

        // FIXME: use json directly ?
        QByteArray byteArrayOfJson = jsonPlatform.toUtf8();

        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonPlatform.toUtf8());
        if (jsonDocument.isObject())
        {
            QJsonObject jsonRoot = jsonDocument.object();

            // Import the agents list from JSON
            if ((_modelManager != NULL) && jsonRoot.contains("agents"))
            {
                _modelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray());
            }

            // Import the mapping from JSON
            if (_agentsMappingC != NULL) {
                _agentsMappingC->importMappingFromJson(byteArrayOfJson);
            }

            if (_scenarioC != NULL)
            {
                // Clear scenario
                _scenarioC->clearScenario();

                // Import the scenario from JSON
                _scenarioC->importScenarioFromJson(byteArrayOfJson);

                // Update the current time
                _scenarioC->setcurrentTime(QTime::fromMSecsSinceStartOfDay(deltaTimeFromTimeLine));

                // Import the executed actions for this scenario from JSON
                _scenarioC->importExecutedActionsFromJson(jsonExecutedActions.toUtf8());
            }

            // Notify QML to reset view
            Q_EMIT resetMappindAndTimeLineViews();
        }
    }
}


/**
 * @brief Load the platform from JSON file
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
                if (jsonDocument.isObject())
                {
                    QJsonObject jsonRoot = jsonDocument.object();

                    // Import the agents list from JSON
                    if ((_modelManager != NULL) && jsonRoot.contains("agents"))
                    {
                        _modelManager->importAgentsListFromJson(jsonRoot.value("agents").toArray());
                    }
                }

                // Import the mapping from JSON
                if (_agentsMappingC != NULL) {
                    _agentsMappingC->importMappingFromJson(byteArrayOfJson);
                }

                // Import the scenario from JSON
                if (_scenarioC != NULL)
                {
                    // Clear scenario
                    _scenarioC->clearScenario();

                    // Import new scenario
                    _scenarioC->importScenarioFromJson(byteArrayOfJson);
                }

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
 * @brief Save the platform to JSON file
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
 * @brief Get the JSON of the current platform
 * @return
 */
QJsonDocument IngeScapeEditorController::_getJsonOfCurrentPlatform()
{
    QJsonDocument jsonDocument;

    if (_jsonHelper != NULL)
    {
        QJsonObject platformJsonObject;

        // Save the agents list
        if (_agentsSupervisionC != NULL)
        {
            // Export the agents list to JSON
            QJsonArray arrayOfAgents = _agentsSupervisionC->exportAgentsListToJSON();

            if (!arrayOfAgents.isEmpty()) {
                platformJsonObject.insert("agents", arrayOfAgents);
            }
        }

        // Save the mapping
        if (_agentsMappingC != NULL)
        {
            QJsonArray arrayOfMappings = _jsonHelper->exportAllAgentsInMapping(_agentsMappingC->allAgentsInMapping()->toList());

            if (!arrayOfMappings.isEmpty()) {
                platformJsonObject.insert("mapping", arrayOfMappings);
            }
        }

        // Save the scenario
        if (_scenarioC != NULL)
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
