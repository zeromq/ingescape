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

#include <QThread>
#include <QApplication>


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
    _terminationSignalWatcher(NULL)
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


    //
    // Agents lists & mappings
    //
    QString agentsListPath = MasticEditorUtils::getAgentsListPath();
    QDir agentsListDir(agentsListPath);
    if (!agentsListDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsListPath << "' !";
    }

    QString agentsMappingsPath = MasticEditorUtils::getAgentsMappingsPath();
    QDir agentsMappingsDir(agentsMappingsPath);
    if (!agentsMappingsDir.exists()) {
        qCritical() << "ERROR: could not create directory at '" << agentsMappingsPath << "' !";
    }


    //
    // Create sub-controllers
    //

    // Create the manager for the data model of MASTIC
    _modelManager = new MasticModelManager(agentsListPath, agentsMappingsPath, this);

    // Create the controller for network communications
    _networkC = new NetworkController(_networkDevice, _ipAddress, _port, this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, this);

    // Create the controller for scenario management
    _scenarioC = new ScenarioController(this);

    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::agentEntered, _modelManager, &MasticModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &MasticModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &MasticModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &MasticModelManager::onAgentExited);
    connect(_networkC, &NetworkController::isMutedFromAgentUpdated, _modelManager, &MasticModelManager::onisMutedFromAgentUpdated);
    connect(_networkC, &NetworkController::isFrozenFromAgentUpdated, _modelManager, &MasticModelManager::onIsFrozenFromAgentUpdated);
    connect(_networkC, &NetworkController::isMutedFromOutputOfAgentUpdated, _modelManager, &MasticModelManager::onIsMutedFromOutputOfAgentUpdated);


    // Connect to signals from the model manager
    connect(_modelManager, &MasticModelManager::agentModelCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &MasticModelManager::agentDefinitionCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentDefinitionCreated);
    connect(_modelManager, &MasticModelManager::isMutedFromOutputOfAgentUpdated, _agentsSupervisionC, &AgentsSupervisionController::onIsMutedFromOutputOfAgentUpdated);
    connect(_modelManager, &MasticModelManager::isActivatedMappingChanged, _agentsMappingC, &AgentsMappingController::onIsActivatedMappingChanged);


    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedToLauncher, _networkC, &NetworkController::onCommandAskedToLauncher);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAsked, _networkC, &NetworkController::onCommandAsked);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedForOutput, _networkC, &NetworkController::onCommandAskedForOutput);
    connect(_agentsSupervisionC, &AgentsSupervisionController::agentDefinitionManaged, _agentsMappingC, &AgentsMappingController::addAgentDefinitionToMapping);


    // Initialize agents list from default file
    _modelManager->importAgentsListFromDefaultFile();



    //
    // Subscribe to system signals to interceipt interruption and termination signals
    //
    _terminationSignalWatcher = new TerminationSignalWatcher(this);
    connect(_terminationSignalWatcher, &TerminationSignalWatcher::terminationSignal,
                     [=] () {
                        qDebug() << "\n\n\nTu connais le tarif Vincent ;-)\n\n\n";

                        if (QApplication::instance() != NULL)
                        {
                            QApplication::instance()->quit();
                        }
                     });




    // TEMP sleep to display our loading screen
    QThread::msleep(2000);
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
    if (_agentsMappingC != NULL)
    {
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
