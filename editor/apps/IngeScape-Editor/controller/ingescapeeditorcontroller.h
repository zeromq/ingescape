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
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *
 */

#ifndef INGESCAPEEDITORCONTROLLER_H
#define INGESCAPEEDITORCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/ingescapemodelmanager.h>
#include <controller/agentssupervisioncontroller.h>
#include <controller/hostssupervisioncontroller.h>
#include <controller/agentsmappingcontroller.h>
#include <controller/networkcontroller.h>
#include <controller/scenariocontroller.h>
#include <controller/valueshistorycontroller.h>
#include <controller/abstracttimeactionslinescenarioviewcontroller.h>
#include <controller/ingescapelaunchermanager.h>

#include <misc/terminationsignalwatcher.h>



/**
 * @brief The IngeScapeEditorController class defines the main controller of the INGESCAPE editor
 */
class IngeScapeEditorController : public QObject
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

    // Manager for the data model of our INGESCAPE editor
    I2_QML_PROPERTY_READONLY(IngeScapeModelManager*, modelManager)

    // Controller for agents supervision
    I2_QML_PROPERTY_READONLY(AgentsSupervisionController*, agentsSupervisionC)

    // Controller for hosts supervision
    I2_QML_PROPERTY_READONLY(HostsSupervisionController*, hostsSupervisionC)

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

    // Manager for launchers of INGESCAPE agents
    I2_QML_PROPERTY_READONLY(IngeScapeLauncherManager*, launcherManager)


public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeEditorController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeEditorController();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
      * @brief Open a platform file (actions, palette, timeline actions, mappings)
      */
    Q_INVOKABLE void openPlatformFromFile();

    /**
      * @brief Save a platform to a selected file (actions, palette, timeline actions, mappings)
      */
    Q_INVOKABLE void savePlatformToSelectedFile();

    /**
      * @brief Save a platform to the default file (actions, palette, timeline actions, mappings)
      */
    void savePlatformToDefaultFile();

    /**
      * @brief Create a new platform (actions, palette, timeline actions, mappings)
      *        by deleting all existing data
      */
    Q_INVOKABLE void createNewPlatform();

    /**
      * @brief Actions to perform before the application closing
      */
    Q_INVOKABLE void processBeforeClosing();

    /**
      * @brief Can delete an agent view model from the list function
      *        Check dependencies in the mapping and in the actions (conditions, effects)
      * @param agent to delete
      */
    Q_INVOKABLE bool canDeleteAgentVMFromList(AgentVM* agent);

    /**
      * @brief Can delete an agent in mapping from the mapping view
      *        Check dependencies in the actions (conditions, effects)
      * @param agent in mapping to delete
      */
    Q_INVOKABLE bool canDeleteAgentInMapping(AgentInMappingVM* agentInMapping);


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


private Q_SLOTS:

    /**
     * @brief Slot when the timer time out
     */
    void _onTimeout();


Q_SIGNALS:
    /**
      * @brief Reset the mapping and timeline views
      */
    void resetMappindAndTimeLineViews();


private:

    /**
      * @brief Open the platform from JSON file
      * @param platformFilePath
      */
    void _openPlatformFromFile(QString platformFilePath);

    /**
      * @brief Save the platform to JSON file
      * @param platformFilePath
      */
    void _savePlatformToFile(QString platformFilePath);


    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Path to the directory containing JSON files to save platform
    QString _platformDirectoryPath;
    QString _platformDefaultFilePath;

    // Helper to manage JSON the saving/opening platform files
    JsonHelper* _jsonHelper;

    // timer to let agents to connect before setting the application in "mapped" mode
    QTimer _timer;

};

QML_DECLARE_TYPE(IngeScapeEditorController)

#endif // INGESCAPEEDITORCONTROLLER_H
