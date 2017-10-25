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
#include <controller/scenariocontroller.h>


/**
 * @brief The MasticEditorController class defines the main controller of the MASTIC editor
 */
class MasticEditorController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // Network settings - ip address
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

    // Controller for scenario management
    I2_QML_PROPERTY_READONLY(ScenarioController*, scenarioC)


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


    // Unix signal handlers.
    //static void hupSignalHandler(int unused);
    //static void termSignalHandler(int unused);


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


     /**
      * @brief Close a definition
      * @param definition
      */
     Q_INVOKABLE void closeDefinition(DefinitionM* definition);

     /**
      * @brief Close an action editor
      * @param action editor controller
      */
     Q_INVOKABLE void closeActionEditor(ActionEditorController *actionEditorC);


public Q_SLOTS:

     /**
      * @brief Method used to force the creation of our singleton from QML
      */
     void forceCreation();


     // Qt signal handlers.
     //void handleSigHup();
     //void handleSigTerm();


private:
     /*static int sighupFd[2];
     static int sigtermFd[2];

     QSocketNotifier *snHup;
     QSocketNotifier *snTerm;*/

};

QML_DECLARE_TYPE(MasticEditorController)

#endif // MASTICEDITORCONTROLLER_H
