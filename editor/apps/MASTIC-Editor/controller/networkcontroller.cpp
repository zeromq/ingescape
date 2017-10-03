/*
 *	NetworkController
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Deliencourt  <deliencourt@ingenuity.io>
 *
 */

#include "networkcontroller.h"


#include <QDebug>

extern "C" {
#include <mastic/mastic_private.h>
}

#include "misc/masticeditorutils.h"


//--------------------------------------------------------------
//
// NetworkController
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param network devece
 * @param ip address
 * @param port number
 * @param parent
 */
NetworkController::NetworkController(QString networkDevice, QString ipAddress, int port, QObject *parent) : QObject(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);


    // Network is ok if the result of mtic_startWithDevice is 1, O otherwise.
    int networkInitialized = 0;

    // Set trace mode
    mtic_setVerbose(true);

    // Read our internal definition
    QString myDefinitionPath = QString("%1/definition.json").arg(MasticEditorUtils::getDataPath());
    QFileInfo checkDefinitionFile(myDefinitionPath);
    if (checkDefinitionFile.exists() && checkDefinitionFile.isFile())
    {
        // Load the definition
        //mtic_loadDefinitionFromPath(myDefinitionPath.toStdString().c_str());

        // Start service with network device
        if (networkDevice.isEmpty() == false)
        {
            networkInitialized = mtic_startWithDevice(networkDevice.toStdString().c_str(),port);
        }

        // Start service with ip if start with network device has failed
        if ((networkInitialized != 1) && (ipAddress.isEmpty() == false))
        {
            networkInitialized = mtic_startWithIP(ipAddress.toStdString().c_str(),port);
        }
    }
    else
    {
        qCritical() << "No definition has been found : " << myDefinitionPath;
    }

    if (networkInitialized == 1)
    {
        qInfo() << "Network services started";
    }
    else
    {
        qCritical() << "The network has not been initialized on " << networkDevice << ipAddress << QString::number(port);
    }
}


/**
 * @brief Destructor
 */
NetworkController::~NetworkController()
{
    // Stop network services
    mtic_stop();
}


