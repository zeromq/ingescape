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
    _modelManager(NULL),
    _agentsSupervisionC(NULL),
    _agentsMappingC(NULL),
    _networkC(NULL),
    _scenarioC(NULL),
    _valuesHistoryC(NULL),
    _timeLineC(NULL),
    _launcherManager(NULL),
    _terminationSignalWatcher(NULL),
    _platformDirectoryPath(""),
    _jsonHelper(NULL)
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


    // Directory for agents lists
    QString agentsListPath = IngeScapeEditorUtils::getAgentsListPath();
    QDir agentsListDir(agentsListPath);
    if (!agentsListDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsListPath << "' !";
    }

    // Directory for agents mappings
    QString agentsMappingsPath = IngeScapeEditorUtils::getAgentsMappingsPath();
    QDir agentsMappingsDir(agentsMappingsPath);
    if (!agentsMappingsDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsMappingsPath << "' !";
    }

    // Directory for data
    QString dataPath = IngeScapeEditorUtils::getDataPath();
    QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << dataPath << "' !";
    }

    // Directory for scenarios
    QString scenariosPath = IngeScapeEditorUtils::getScenariosPath();
    QDir scenariosDir(scenariosPath);
    if (!scenariosDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << scenariosPath << "' !";
    }

    // Directory for platform files
    QString platformPath = IngeScapeEditorUtils::getPlatformsPath();
    QDir platformDir(platformPath);
    if (!platformDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << platformPath << "' !";
    } else {
        _platformDirectoryPath = platformPath;
    }

    QDate today = QDate::currentDate();
    _platformDefaultFilePath = QString("%1platform_%2.json").arg(_platformDirectoryPath, today.toString("ddMMyy"));

    // Create the helper to manage JSON definitions of agents
    _jsonHelper = new JsonHelper(this);

    //
    // Create sub-controllers
    //

    // Create the manager for the data model of INGESCAPE
    _modelManager = new IngeScapeModelManager(agentsListPath, agentsMappingsPath, dataPath, this);

    // Create the controller for network communications
    _networkC = new NetworkController(this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, this);

    // Create the controller for hosts supervision
    _hostsSupervisionC = new HostsSupervisionController(_launcherManager, this);

    // Create the controller for records supervision
    _recordsSupervisionC = new RecordsSupervisionController(_modelManager, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, agentsMappingsPath, this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(_modelManager, scenariosPath, this);

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
    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &IngeScapeModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &IngeScapeModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::allRecordsReceived, _modelManager, &IngeScapeModelManager::onAllRecordsReceived);
    connect(_networkC, &NetworkController::newRecordReceived, _modelManager, &IngeScapeModelManager::onNewRecordReceived);
    connect(_networkC, &NetworkController::valuePublished, _modelManager, &IngeScapeModelManager::onValuePublished);
    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &IngeScapeModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::isFrozenFromAgentUpdated, _modelManager, &IngeScapeModelManager::onIsFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isMutedFromOutputOfAgentUpdated, _modelManager, &IngeScapeModelManager::onIsMutedFromOutputOfAgentUpdated);
    connect(_networkC, &NetworkController::agentStateChanged, _modelManager, &IngeScapeModelManager::onAgentStateChanged);


    // Connect to signals from the model manager
    connect(_modelManager, &IngeScapeModelManager::isMappingActivatedChanged, _agentsMappingC, &AgentsMappingController::onIsMappingActivatedChanged);
    connect(_modelManager, &IngeScapeModelManager::isMappingControlledChanged, _agentsMappingC, &AgentsMappingController::onIsMappingControlledChanged);

    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _hostsSupervisionC, &HostsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &IngeScapeModelManager::agentModelCreated, _recordsSupervisionC, &RecordsSupervisionController::onAgentModelCreated);

    connect(_modelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentModelWillBeDeleted);
    connect(_modelManager, &IngeScapeModelManager::agentModelWillBeDeleted, _hostsSupervisionC, &HostsSupervisionController::onAgentModelWillBeDeleted);

    connect(_modelManager, &IngeScapeModelManager::activeAgentDefined, _agentsMappingC, &AgentsMappingController::onActiveAgentDefined);
    connect(_modelManager, &IngeScapeModelManager::activeAgentMappingDefined, _agentsMappingC, &AgentsMappingController::onActiveAgentMappingDefined);
    connect(_modelManager, &IngeScapeModelManager::mapped, _agentsMappingC, &AgentsMappingController::onMapped);
    connect(_modelManager, &IngeScapeModelManager::unmapped, _agentsMappingC, &AgentsMappingController::onUnmapped);

    connect(_modelManager, &IngeScapeModelManager::addInputsToEditorForOutputs, _valuesHistoryC, &ValuesHistoryController::onAgentOutputsObserved);
    connect(_modelManager, &IngeScapeModelManager::addInputsToEditorForOutputs, _networkC, &NetworkController::onAddInputsToEditorForOutputs);
    connect(_modelManager, &IngeScapeModelManager::removeInputsToEditorForOutputs, _networkC, &NetworkController::onRemoveInputsToEditorForOutputs);

    connect(_modelManager, &IngeScapeModelManager::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_modelManager, &IngeScapeModelManager::recordsListChanged, _recordsSupervisionC, &RecordsSupervisionController::onRecordsListChanged);
    connect(_modelManager, &IngeScapeModelManager::recordAdded, _recordsSupervisionC, &RecordsSupervisionController::onRecordAdded);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgentAboutOutput, _networkC, &NetworkController::onCommandAskedToAgentAboutOutput);
    connect(_agentsSupervisionC, &AgentsSupervisionController::identicalAgentModelReplaced, _agentsMappingC, &AgentsMappingController::onIdenticalAgentModelReplaced);
    connect(_agentsSupervisionC, &AgentsSupervisionController::identicalAgentModelAdded, _agentsMappingC, &AgentsMappingController::onIdenticalAgentModelAdded);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openValuesHistoryOfAgent, _valuesHistoryC, &ValuesHistoryController::filterValuesToShowOnlyAgent);

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
    connect(_agentsMappingC, &AgentsMappingController::agentCreatedByMapping, _modelManager, &IngeScapeModelManager::addAgentModel);


    // Connect to signals from the agents mapping list to the action editor
    connect(_scenarioC, &ScenarioController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_scenarioC, &ScenarioController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);

    // Connect to signals from the time line time line visible range change to the scenario controller to filter the action view models
    connect(_timeLineC, &AbstractTimeActionslineScenarioViewController::timeRangeChanged, _scenarioC, &ScenarioController::ontimeRangeChange);

    // Connect to signals from Record supervision controller
    connect(_recordsSupervisionC, &RecordsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);

    // Initialize agents list from default file
    _modelManager->importAgentsListFromDefaultFile();

    // Start our INGESCAPE agent with a network device (or an IP address) and a port
    bool isStarted = _networkC->start(_networkDevice, _ipAddress, _port);

    if (isStarted && (_modelManager != NULL))
    {
        // Initialize platform from online mapping
        _modelManager->setisMappingActivated(true);
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
    if(_jsonHelper != NULL)
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
 * @brief Close a definition
 * @param definition
 */
void IngeScapeEditorController::closeDefinition(DefinitionM* definition)
{
    if ((definition != NULL) && (_modelManager != NULL))
    {
        _modelManager->openedDefinitions()->remove(definition);
    }
}

/**
 * @brief Close an action editor
 * @param action editor controller
 */
void IngeScapeEditorController::closeActionEditor(ActionEditorController* actionEditorC)
{
    if (_scenarioC != NULL)
    {
        _scenarioC->closeActionEditor(actionEditorC);
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
 * @brief Open a platform file (actions, palette, timeline actions, mappings)
 */
void IngeScapeEditorController::openPlatformFromFile()
{
    // "File Dialog" to get the files (paths) to open
    QString platformFilePath = QFileDialog::getOpenFileName(NULL,
                                                                "Open platform",
                                                                _platformDirectoryPath,
                                                                "JSON (*.json)");

    // Open the platform from JSON file
    _openPlatformFromFile(platformFilePath);
}


/**
 * @brief Open the platform from JSON file
 * @param platformFilePath
 */
void IngeScapeEditorController::_openPlatformFromFile(QString platformFilePath)
{
    if (!platformFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Open the platform from JSON file" << platformFilePath;

        QFile jsonFile(platformFilePath);
        if (jsonFile.exists())
        {
            if (jsonFile.open(QIODevice::ReadOnly))
            {
                QByteArray byteArrayOfJson = jsonFile.readAll();
                jsonFile.close();

                // Import mapping
                if(_agentsMappingC != NULL)
                {
                    _agentsMappingC->importMappingFromJson(byteArrayOfJson, true);
                }

                // Import scenario
                if(_scenarioC != NULL)
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
 * @brief Save a platform to a selected file (actions, palette, timeline actions, mappings)
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
 * @brief Save a platform to the default file (actions, palette, timeline actions, mappings)
 */
void IngeScapeEditorController::savePlatformToDefaultFile()
{
    if (!_platformDefaultFilePath.isEmpty()) {
        // Save the platform to JSON file
        _savePlatformToFile(_platformDefaultFilePath);
    }
}

/**
 * @brief Save the platform to JSON file
 * @param platformFilePath
 */
void IngeScapeEditorController::_savePlatformToFile(QString platformFilePath)
{
    if (!platformFilePath.isEmpty() && (_jsonHelper != NULL))
    {
        qInfo() << "Save the scenario to JSON file" << platformFilePath;

        QJsonObject platformJsonObject;

        // Save the scenario
        if(_scenarioC != NULL)
        {
            platformJsonObject = _jsonHelper->exportScenario(_scenarioC->actionsList()->toList(),_scenarioC->actionsInPaletteList()->toList(),_scenarioC->actionsInTimeLine()->toList());
        }

        // Save mapping
        if(_agentsMappingC != NULL)
        {
            QJsonArray jsonArray = _jsonHelper->exportAllAgentsInMapping(_agentsMappingC->allAgentsInMapping()->toList());

            if(jsonArray.count() > 0)
            {
                platformJsonObject.insert("mappings",jsonArray);
            }
        }

        // Conversion into byteArray
        QByteArray byteArrayOfJson = QJsonDocument(platformJsonObject).toJson();

        QFile jsonFile(platformFilePath);
        if (jsonFile.open(QIODevice::WriteOnly))
        {
            jsonFile.write(byteArrayOfJson);
            jsonFile.close();
        }
        else {
            qCritical() << "Can not open file" << platformFilePath;
        }
    }
}


/**
 * @brief Create a new platform (actions, palette, timeline actions, mappings)
 *        by deleting all existing data
 */
void IngeScapeEditorController::createNewPlatform()
{
    // Create new mapping
    if (_agentsMappingC != NULL)
    {
        _agentsMappingC->createNewMapping();
    }

    // Create new scenario
    if (_scenarioC != NULL)
    {
        // Reset scenario
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
    // Save the agent list
    if (_agentsSupervisionC != NULL)
    {
        _agentsSupervisionC->exportAgentsListToDefaultFile();
    }

    // Save the platform to the default file
    savePlatformToDefaultFile();
}


/**
 * @brief Can delete an agent view model from the list function
 *        Check dependencies in the mapping and in the actions (conditions, effects)
 * @param agent to delete
 */
bool IngeScapeEditorController::canDeleteAgentVMFromList(AgentVM* agent)
{
    bool canBeDeleted = true;

    // Check if the agent is in the curent mapping
    if (_agentsMappingC && (agent != NULL))
    {
        AgentInMappingVM* agentInMapping = _agentsMappingC->getAgentInMappingFromName(agent->name());
        if (agentInMapping != NULL)
        {
            canBeDeleted = false;
        }
    }

    // Check if the agent is in action condition or effect
    if (canBeDeleted && (_scenarioC != NULL))
    {
        canBeDeleted = !_scenarioC->isAgentDefinedInActions(agent->name());
    }

    return canBeDeleted;
}


/**
 * @brief Can delete an agent in mapping from the mapping view
 *        Check dependencies in the actions (conditions, effects)
 * @param agent in mapping to delete
 */
bool IngeScapeEditorController::canDeleteAgentInMapping(AgentInMappingVM* agentInMapping)
{
    bool canBeDeleted = true;

    // Check if the agent is in action condition or effect
    if (_scenarioC != NULL)
    {
        canBeDeleted = !_scenarioC->isAgentDefinedInActions(agentInMapping->name());
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

    if ((_networkC != NULL) && (_modelManager != NULL))
    {
        bool isInt = false;
        int nPort = strPort.toInt(&isInt);
        if (isInt)
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
            qWarning() << "Port" << strPort << "is not an int !";
        }
    }
    return success;
}
