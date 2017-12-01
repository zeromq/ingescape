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
#include <controller/valueshistorycontroller.h>
#include <controller/abstracttimeactionslinescenarioviewcontroller.h>

#include <misc/terminationsignalwatcher.h>



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

    // Controller for the history of values
    I2_QML_PROPERTY_READONLY(ValuesHistoryController*, valuesHistoryC)

    // Controller for the time line
    I2_QML_PROPERTY_READONLY(AbstractTimeActionslineScenarioViewController*, timeLineC)


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



public Q_SLOTS:

     /**
      * @brief Close a definition
      * @param definition
      */
     void closeDefinition(DefinitionM* definition);


     /**
      * @brief Close an action editor
      * @param action editor controller
      */
     void closeActionEditor(ActionEditorController *actionEditorC);


     /**
      * @brief Method used to force the creation of our singleton from QML
      */
     void forceCreation();


     /**
      * @brief Get the position of the mouse cursor in global screen coordinates
      *
      * @remarks You must use mapToGlobal to convert it to local coordinates
      *
      * @return
      */
     QPointF getGlobalMousePosition();


private:
     // To subscribe to termination signals
     TerminationSignalWatcher *_terminationSignalWatcher;
};

QML_DECLARE_TYPE(MasticEditorController)

#endif // MASTICEDITORCONTROLLER_H
