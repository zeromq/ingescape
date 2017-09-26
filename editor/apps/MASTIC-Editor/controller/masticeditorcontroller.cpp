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
MasticEditorController::MasticEditorController(QObject *parent) : QObject(parent)
{
    qInfo() << "New MASTIC Editor Controller";

    QString rootDirectoryPath = I2Utils::getOrCreateAppRootPathInDocumentDir("MASTIC");
    if (!rootDirectoryPath.isEmpty())
    {
        _snapshotDirectory = QString("%1Snapshots").arg(rootDirectoryPath);
    }

    // Get our settings
    MasticEditorSettings& settings = MasticEditorSettings::Instance();

    // Settings about the "Window"
    settings.beginGroup("window");
    _width = settings.value("width", 1920).toInt();
    _height = settings.value("height", 1080).toInt();
    qInfo() << "width" << _width << "height" << _height;
    settings.endGroup();

    // Settings about the "Network"
    settings.beginGroup("network");
    _host = settings.value("host", "localhost").toString();
    _networkDevice = settings.value("networkDevice", "en0").toString();
    _ipAddress = settings.value("ipAddress", "127.0.0.1").toString();
    _port = settings.value("port", 5556).toInt();
    qInfo() << "host" << _host << "networkDevice" << _networkDevice << "ipAddress" << _ipAddress << "port" << QString::number(_port);
    settings.endGroup();


    // Create the manager for the data model of our MASTIC editor
    _modelManager = new MasticModelManager(this);

    // Create the controller for agents supervision
    _agentsSupervisionC = new AgentsSupervisionController(_modelManager, this);

    // Create the controller for agents mapping
    _agentsMappingC = new AgentsMappingController(_modelManager, this);

    // Create the controller for network comunications
    _networkC = new NetworkController(_networkDevice,_ipAddress,_port, this);

    //TEMP sleep to display our loading screen
    QThread::msleep(1000);
}


/**
 * @brief Destructor
 */
MasticEditorController::~MasticEditorController()
{
    if (_agentsMappingC != NULL)
    {
        AgentsMappingController* temp = _agentsMappingC;
        setagentsMappingC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_agentsSupervisionC != NULL)
    {
        AgentsSupervisionController* temp = _agentsSupervisionC;
        setagentsSupervisionC(NULL);
        delete temp;
        temp = NULL;
    }

    if (_modelManager != NULL)
    {
        MasticModelManager* temp = _modelManager;
        setmodelManager(NULL);
        delete temp;
        temp = NULL;
    }

    if (_networkC != NULL)
    {
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
 * @brief Method used to force the creation of our singleton from QML
 */
void MasticEditorController::forceCreation()
{
    qDebug() << "Force the creation of our singleton from QML";
}
