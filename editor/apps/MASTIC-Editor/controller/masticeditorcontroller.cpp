/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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

#include "controller/masticeditorcontroller.h"

#include "misc/masticeditorsettings.h"
#include "misc/masticeditorutils.h"

#include <I2Quick.h>

#include <QFileDialog>
#include <QThread>
#include <QApplication>
#include <QCursor>


/**
 * @brief Constructor
 * @param parent
 */
MasticEditorController::MasticEditorController(QObject *parent) : QObject(parent),
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
    qInfo() << "New MASTIC Editor Controller";

    //
    // Snapshots directory
    //
    QString snapshotsDirectoryPath = MasticEditorUtils::getSnapshotsPath();
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
    MasticEditorSettings& settings = MasticEditorSettings::Instance();

    // Settings about the "Network"
    settings.beginGroup("network");
    _networkDevice = settings.value("networkDevice").toString();
    _ipAddress = settings.value("ipAddress").toString();
    _port = settings.value("port").toInt();
    qInfo() << "Network Device:" << _networkDevice << "-- IP address:" << _ipAddress << "-- Port" << QString::number(_port);
    settings.endGroup();


    // Directory for agents lists
    QString agentsListPath = MasticEditorUtils::getAgentsListPath();
    QDir agentsListDir(agentsListPath);
    if (!agentsListDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsListPath << "' !";
    }

    // Directory for agents mappings
    QString agentsMappingsPath = MasticEditorUtils::getAgentsMappingsPath();
    QDir agentsMappingsDir(agentsMappingsPath);
    if (!agentsMappingsDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsMappingsPath << "' !";
    }

    // Directory for data
    QString dataPath = MasticEditorUtils::getDataPath();
    QDir dataDir(dataPath);
    if (!dataDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << dataPath << "' !";
    }

    // Directory for scenarios
    QString scenariosPath = MasticEditorUtils::getScenariosPath();
    QDir scenariosDir(scenariosPath);
    if (!scenariosDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << scenariosPath << "' !";
    }

    // Directory for platform files
    QString platformPath = MasticEditorUtils::getPlatformsPath();
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

    // Create the manager for the data model of MASTIC
    _modelManager = new MasticModelManager(agentsListPath, agentsMappingsPath, dataPath, this);

    // Create the controller for network communications
    _networkC = new NetworkController(this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, agentsMappingsPath, this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(_modelManager, scenariosPath, this);

    // Create the controller for the history of values
    _valuesHistoryC = new ValuesHistoryController(_modelManager, this);

    // Create the controller for the time line
    _timeLineC = new AbstractTimeActionslineScenarioViewController(this);

    // Create the manager for launchers of MASTIC agents
    //_launcherManager = new MasticLauncherManager(this);
    _launcherManager = &MasticLauncherManager::Instance();

    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::agentEntered, _modelManager, &MasticModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &MasticModelManager::onAgentExited);
    connect(_networkC, &NetworkController::launcherEntered, _modelManager, &MasticModelManager::onLauncherEntered);
    connect(_networkC, &NetworkController::launcherExited, _modelManager, &MasticModelManager::onLauncherExited);
    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &MasticModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &MasticModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::valuePublished, _modelManager, &MasticModelManager::onValuePublished);
    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &MasticModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::isFrozenFromAgentUpdated, _modelManager, &MasticModelManager::onIsFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isMutedFromOutputOfAgentUpdated, _modelManager, &MasticModelManager::onIsMutedFromOutputOfAgentUpdated);


    // Connect to signals from the model manager
    connect(_modelManager, &MasticModelManager::isActivatedMappingChanged, _agentsMappingC, &AgentsMappingController::onIsActivatedMappingChanged);
    connect(_modelManager, &MasticModelManager::agentModelCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &MasticModelManager::agentModelWillBeDeleted, _agentsMappingC, &AgentsMappingController::onAgentModelWillBeDeleted);
    connect(_modelManager, &MasticModelManager::mapped, _agentsMappingC, &AgentsMappingController::onMapped);
    connect(_modelManager, &MasticModelManager::unmapped, _agentsMappingC, &AgentsMappingController::onUnmapped);
    connect(_modelManager, &MasticModelManager::addInputsToEditorForOutputs, _valuesHistoryC, &ValuesHistoryController::onAgentOutputsObserved);
    connect(_modelManager, &MasticModelManager::addInputsToEditorForOutputs, _networkC, &NetworkController::onAddInputsToEditorForOutputs);
    connect(_modelManager, &MasticModelManager::removeInputsToEditorForOutputs, _networkC, &NetworkController::onRemoveInputsToEditorForOutputs);
    connect(_modelManager, &MasticModelManager::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToAgentAboutOutput, _networkC, &NetworkController::onCommandAskedToAgentAboutOutput);
    connect(_agentsSupervisionC, &AgentsSupervisionController::identicalAgentModelReplaced, _agentsMappingC, &AgentsMappingController::onIdenticalAgentModelReplaced);
    connect(_agentsSupervisionC, &AgentsSupervisionController::identicalAgentModelAdded, _agentsMappingC, &AgentsMappingController::onIdenticalAgentModelAdded);
    connect(_agentsSupervisionC, &AgentsSupervisionController::openValuesHistoryOfAgent, _valuesHistoryC, &ValuesHistoryController::filterValuesToShowOnlyAgent);


    // Connect to signals from the controller for mapping of agents
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_agentsMappingC, &AgentsMappingController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);
    connect(_agentsMappingC, &AgentsMappingController::agentInMappingAdded, _scenarioC, &ScenarioController::onAgentInMappingAdded);
    connect(_agentsMappingC, &AgentsMappingController::agentInMappingRemoved, _scenarioC, &ScenarioController::onAgentInMappingRemoved);


    // Connect to signals from the agents mapping list to the action editor
    connect(_scenarioC, &ScenarioController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_scenarioC, &ScenarioController::commandAskedToAgent, _networkC, &NetworkController::onCommandAskedToAgent);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutSettingValue, _networkC, &NetworkController::onCommandAskedToAgentAboutSettingValue);
    connect(_scenarioC, &ScenarioController::commandAskedToAgentAboutMappingInput, _networkC, &NetworkController::onCommandAskedToAgentAboutMappingInput);


    // Initialize agents list from default file
    _modelManager->importAgentsListFromDefaultFile();

    // Initialize platform from default file
    _openPlatformFromFile(_platformDefaultFilePath);

    // Start our MASTIC agent with a network device (or an IP address) and a port
    _networkC->start(_networkDevice, _ipAddress, _port);


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



    // FIXME: sleep to display our loading screen
    //QThread::msleep(2000);
}


/**
 * @brief Destructor
 */
MasticEditorController::~MasticEditorController()
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

        //MasticLauncherManager* temp = _launcherManager;
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

        MasticModelManager* temp = _modelManager;
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

    qInfo() << "Delete MASTIC Editor Controller";
}


/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
QObject* MasticEditorController::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    // NOTE: A QObject singleton type instance returned from a singleton type provider is owned by the QML engine.
    // For this reason, the singleton type provider function should not be implemented as a singleton factory.
    return new MasticEditorController();
}


/**
 * @brief Close a definition
 * @param definition
 */
void MasticEditorController::closeDefinition(DefinitionM* definition)
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
void MasticEditorController::closeActionEditor(ActionEditorController* actionEditorC)
{
    if (_scenarioC != NULL)
    {
        _scenarioC->closeActionEditor(actionEditorC);
    }
}


/**
 * @brief Method used to force the creation of our singleton from QML
 */
void MasticEditorController::forceCreation()
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
QPointF MasticEditorController::getGlobalMousePosition()
{
    return QCursor::pos();
}

/**
 * @brief Open a platform file (actions, palette, timeline actions, mappings)
 */
void MasticEditorController::openPlatformFromFile()
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
void MasticEditorController::_openPlatformFromFile(QString platformFilePath)
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
void MasticEditorController::savePlatformToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString platformFilePath = QFileDialog::getSaveFileName(NULL,
                                                              "Save platform",
                                                              _platformDirectoryPath,
                                                              "JSON (*.json)");

    if(!platformFilePath.isEmpty()) {
        // Save the platform to JSON file
        _savePlatformToFile(platformFilePath);
    }
}

/**
 * @brief Save a platform to the default file (actions, palette, timeline actions, mappings)
 */
void MasticEditorController::savePlatformToDefaultFile()
{
    if(!_platformDefaultFilePath.isEmpty()) {
        // Save the platform to JSON file
        _savePlatformToFile(_platformDefaultFilePath);
    }
}

/**
 * @brief Save the platform to JSON file
 * @param platformFilePath
 */
void MasticEditorController::_savePlatformToFile(QString platformFilePath)
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
void MasticEditorController::createNewPlatform()
{
    // Create new mapping
    if(_agentsMappingC != NULL)
    {
        _agentsMappingC->createNewMapping();
    }

    // Create new scenario
    if(_scenarioC != NULL)
    {
        // Reset scenario
        _scenarioC->clearScenario();
    }
}

/**
 * @brief Actions to perform before the application closing
 */
void MasticEditorController::processBeforeClosing()
{
    // Save the agent list
    if(_agentsSupervisionC != NULL)
    {
        _agentsSupervisionC->exportAgentsListToDefaultFile();
    }

    // Save the platform to the default file
    savePlatformToDefaultFile();
}

