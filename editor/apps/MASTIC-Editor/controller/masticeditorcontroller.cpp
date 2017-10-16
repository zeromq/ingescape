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

#include "masticeditorcontroller.h"

#include "misc/masticeditorsettings.h"


#include <I2Quick.h>

#include <QThread>


/**
 * @brief Constructor
 * @param parent
 */
MasticEditorController::MasticEditorController(QObject *parent) : QObject(parent),
    _modelManager(NULL),
    _agentsSupervisionC(NULL),
    _agentsMappingC(NULL),
    _networkC(NULL)
{
    qInfo() << "New MASTIC Editor Controller";

    QString rootDirectoryPath = I2Utils::getOrCreateAppRootPathInDocumentDir("MASTIC");
    if (!rootDirectoryPath.isEmpty())
    {
        _snapshotDirectory = QString("%1Snapshots").arg(rootDirectoryPath);
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
    // Create sub-controllers
    //

    // Create the manager for the data model of MASTIC
    _modelManager = new MasticModelManager(this);

    // Create the controller for network communications
    _networkC = new NetworkController(_networkDevice, _ipAddress, _port, this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, this);

    // Connect to signals from the network controller
    connect(_networkC, &NetworkController::agentEntered, _modelManager, &MasticModelManager::onAgentEntered);
    connect(_networkC, &NetworkController::definitionReceived, _modelManager, &MasticModelManager::onDefinitionReceived);
    connect(_networkC, &NetworkController::mappingReceived, _modelManager, &MasticModelManager::onMappingReceived);
    connect(_networkC, &NetworkController::agentExited, _modelManager, &MasticModelManager::onAgentExited);

    // Connect to signals from the model manager
    connect(_modelManager, &MasticModelManager::agentModelCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentModelCreated);
    connect(_modelManager, &MasticModelManager::agentDefinitionCreated, _agentsSupervisionC, &AgentsSupervisionController::onAgentDefinitionCreated);

    // Connect to signals from the controller for supervision of agents
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAsked, _networkC, &NetworkController::onCommandAsked);
    connect(_agentsSupervisionC, &AgentsSupervisionController::commandAskedForOutput, _networkC, &NetworkController::onCommandAskedForOutput);

    // Initialize agents with JSON files
    _modelManager->initAgentsWithFiles();

    // TEMP sleep to display our loading screen
    QThread::msleep(2000);
}


/**
 * @brief Destructor
 */
MasticEditorController::~MasticEditorController()
{
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
 * @brief Method used to force the creation of our singleton from QML
 */
void MasticEditorController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}
