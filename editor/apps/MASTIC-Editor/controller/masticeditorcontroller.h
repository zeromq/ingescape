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

#ifndef MASTICEDITORCONTROLLER_H
#define MASTICEDITORCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>
#include <controller/agentssupervisioncontroller.h>
#include <controller/agentsmappingcontroller.h>
#include <controller/networkcontroller.h>


/**
 * @brief The MasticEditorController class defines the main controller of the MASTIC editor
 */
class MasticEditorController : public QObject
{
    Q_OBJECT

    // Width and height of our window
    I2_QML_PROPERTY_READONLY(int, width)
    I2_QML_PROPERTY_READONLY(int, height)

    // Network settings - host
    I2_QML_PROPERTY_READONLY(QString, host)

    // Network settings - networkDevice
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // Network settings - ipAddress
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY_READONLY(int, port)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Manager for the data model of our MASTIC editor
    I2_QML_PROPERTY_READONLY(MasticModelManager*, modelManager)

    // Controller for agents supervision
    I2_QML_PROPERTY_READONLY(AgentsSupervisionController*, agentsSupervisionC)

    // Controller for agents mapping
    I2_QML_PROPERTY_READONLY(AgentsMappingController*, agentsMappingC)

    // Controller for network communication
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit MasticEditorController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~MasticEditorController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


public slots:

     /**
      * @brief Method used to force the creation of our singleton from QML
      */
     void forceCreation();

};

QML_DECLARE_TYPE(MasticEditorController)

#endif // MASTICEDITORCONTROLLER_H
