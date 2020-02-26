/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Chloé Roumieu    <roumieu@ingenuity.io>
 */

#ifndef INGESCAPEEDITORCONTROLLER_H
#define INGESCAPEEDITORCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2PropertyHelpers.h>

#include <controller/abstracttimeactionslinescenarioviewcontroller.h>
#include <controller/agentsmappingcontroller.h>
#include <controller/agentssupervisioncontroller.h>
#include <controller/callhomecontroller.h>
#include <controller/editormodelmanager.h>
#include <controller/hostssupervisioncontroller.h>
#include <controller/licensescontroller.h>
#include <controller/logstreamcontroller.h>
#include <controller/networkcontroller.h>
#include <controller/recordssupervisioncontroller.h>
#include <controller/scenariocontroller.h>
#include <controller/valueshistorycontroller.h>

#include <misc/terminationsignalwatcher.h>


/**
 * @brief The IngeScapeEditorController class defines the main controller of the IngeScape editor
 */
class IngeScapeEditorController : public QObject
{
    Q_OBJECT

    // Network settings - network device
    I2_QML_PROPERTY(QString, networkDevice)

    // Network settings - ip address
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Network settings - port
    I2_QML_PROPERTY(uint, port)

    I2_QML_PROPERTY_READONLY(bool, ingescapeShouldBeStartedAtLaunch)

    // Flag indicating if the Model/View Model Visualizer is available
    I2_QML_PROPERTY_READONLY(bool, isAvailableModelVisualizer)

    // Flag indicating if the Model/View Model Visualizer is visible
    I2_QML_PROPERTY(bool, isVisibleModelVisualizer)

    // Snapshot Directory
    I2_QML_PROPERTY_READONLY(QString, snapshotDirectory)

    // Controller to manage the data model of our IngeScape editor
    I2_QML_PROPERTY_READONLY(EditorModelManager*, modelManager)

    // Controller to manage network communications
    I2_QML_PROPERTY_READONLY(NetworkController*, networkC)

    // Controller to manage the agents list
    I2_QML_PROPERTY_READONLY(AgentsSupervisionController*, agentsSupervisionC)

    // Controller to manager the call home at startup
    I2_QML_PROPERTY_READONLY(CallHomeController*, callHomeC)

    // Controller to manage hosts
    I2_QML_PROPERTY_READONLY(HostsSupervisionController*, hostsSupervisionC)

    // Controller for records supervision
    I2_QML_PROPERTY_READONLY(RecordsSupervisionController*, recordsSupervisionC)

    // Controller to manage the agents mapping
    I2_QML_PROPERTY_READONLY(AgentsMappingController*, agentsMappingC)

    // Controller to manage the scenario
    I2_QML_PROPERTY_READONLY(ScenarioController*, scenarioC)

    // Controller to manage the history of values
    I2_QML_PROPERTY_READONLY(ValuesHistoryController*, valuesHistoryC)

    // Controller to manage IngeScape licenses
    I2_QML_PROPERTY_READONLY(LicensesController*, licensesC)

    // Controller to manage the time line
    I2_QML_PROPERTY_READONLY(AbstractTimeActionslineScenarioViewController*, timeLineC)

    // Opened log stream viewers
    I2_QOBJECT_LISTMODEL(LogStreamController, openedLogStreamControllers)

    // List of opened windows
    I2_QOBJECT_LISTMODEL(QObject, openedWindows)

    // Peer id of the IngeScape agent "Expe"
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerIdOfExpe)

    // Peer name of the IngeScape agent "Expe"
    I2_CPP_NOSIGNAL_PROPERTY(QString, peerNameOfExpe)

    // Name of the current IngeScape platform
    I2_QML_PROPERTY_READONLY(QString, currentPlatformName)

    // Flag indicating if a platform has been loaded (opened) by the user or if we are in a "new" or "last" state
    I2_CPP_NOSIGNAL_PROPERTY(bool, hasAPlatformBeenLoadedByUser)

    // Flag indicating if we must show the getting started page on startup
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, gettingStartedShowAtStartup)

    I2_QML_PROPERTY(QString, platformNameBeforeLoadReplay)

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
      * @brief Load a platform (agents, mappings, actions, palette, timeline actions)
      * from the file selected by the user
      */
    Q_INVOKABLE void loadPlatformFromSelectedFile();


    /**
      * @brief Save the platform (agents, mappings, actions, palette, timeline actions)
      * to the file selected by the user
      */
    Q_INVOKABLE void selectFileToSavePlatform();


    /**
     * @brief Save the current platform to the last loaded platform file
     */
    Q_INVOKABLE void savePlatformToCurrentlyLoadedFile();


    /**
      * @brief Clear the current platform (agents, mappings, actions, palette, timeline actions)
      *        by deleting all existing data
      */
    Q_INVOKABLE void clearCurrentPlatform();


    /**
      * @brief Actions to perform before the application closing
      */
    Q_INVOKABLE void processBeforeClosing();


    /**
      * @brief Check if an agents grouped by definition is used in the current platform
      *        In the mapping or/and in the scenario (actions, conditions, effects)
      * @param agentsGroupedByDefinition
      */
    Q_INVOKABLE bool isAgentUsedInPlatform(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
      * @brief Close a definition
      * @param definition
      */
    Q_INVOKABLE void closeDefinition(DefinitionM* definition);


    /**
      * @brief Close an action editor
      * @param actionEditorC
      */
    Q_INVOKABLE void closeActionEditor(ActionEditorController* actionEditorC);


    /**
     * @brief Close a "Log Stream" controller
     * @param logStreamC
     */
    Q_INVOKABLE void closeLogStreamController(LogStreamController* logStreamC);


    /**
     * @brief Add the window to the list of opened windows
     * @param window
     */
    Q_INVOKABLE void addOpenedWindow(QObject* window);


    /**
     * @brief Remove the window from the list of opened windows
     * @param window
     */
    Q_INVOKABLE void removeOpenedWindow(QObject* window);


    /**
     * @brief Checks if there was changes since the platform was last saved
     * @return
     */
    Q_INVOKABLE bool hasPlatformChanged();


    /**
     * @brief If _selectedNetwork not available will try to auto select another one
     */
    Q_INVOKABLE bool startIngeScape();

    /**
     * @brief Stop IngeScape
     */
    Q_INVOKABLE void stopIngeScape(bool hasToClearPlatform);


public Q_SLOTS:

    /**
      * @brief Method used to force the creation of our singleton from QML
      */
    void forceCreation();


Q_SIGNALS:
    /**
     * @brief Triggered to open our license popup
     */
    void openPopupLicense();


    /**
     * @brief Triggered to open our "failed to load agent(s) definition" popup
     */
    void openPopupFailedToLoadAgentDefinition();


    /**
      * @brief Signal emitted to reset the mapping view
      */
    void resetMappindView();


    /**
      * @brief Signal emitted to reset the timeline view
      */
    void resetTimeLineView(bool showIt);


private Q_SLOTS:


    void _onAgentEditorStartedOrStopped(bool started);

    /**
     * @brief Called when our application receives an "open file" request
     * @param url
     * @param filePath
     */
    void _onOpenFileRequest(QUrl url, QString filePath);


    /**
     * @brief Slot called when we have to open the "Log Stream" of a list of agents
     * @param models
     */
    void _onOpenLogStreamOfAgents(QList<AgentM*> models);


    /**
     * @brief Slot called when the user wants to start to record
     */
    void _onStartToRecord();


    /**
     * @brief Slot called when a replay is currently loading
     * @param deltaTimeFromTimeLineStart
     * @param jsonPlatform
     * @param jsonExecutedActions
     */
    void _onReplayLoading(int deltaTimeFromTimeLineStart, QString jsonPlatform, QString jsonExecutedActions);


    /**
     * @brief Slot called when a replay is  unloaded : allow to reload platform before "record mode"
     */
    void _onReplayUNloaded();


    /**
     * @brief Slot called when we receive the command "Load Platform File From Path"
     * @param platformFilePath
     */
    void _onLoadPlatformFileFromPath(QString platformFilePath);


    /**
     * @brief Slot called when we receive the command "Update TimeLine State"
     * @param state
     */
    void _onUpdateTimeLineState(QString state);


    /**
     * @brief Slot called when we receive the command "Update Record State"
     * @param state
     */
    void _onUpdateRecordState(QString state);


    /**
     * @brief Slot called just before an action is performed
     * (the message "EXECUTED ACTION" must be sent on the network to the recorder)
     * @param message
     */
    void _onActionWillBeExecuted(QString message);


    /**
     * @brief Slot called when the state of the TimeLine updated
     * @param state
     */
    void _onTimeLineStateUpdated(QString state);


    /**
     * @brief Slot called when an expe enter the network
     * @param peerId
     * @param peerName
     * @param ipAddress
     * @param hostname
     */
    void _onExpeEntered(QString peerId, QString peerName, QString ipAddress, QString hostname);


    /**
     * @brief Slot called when an expe quit the network
     * @param peerId
     * @param peerName
     */
    void _onExpeExited(QString peerId, QString peerName);


    /**
     * @brief Slot called when the licenses have been updated
     */
    void _onLicensesUpdated();


    /**
     * @brief Called when our network device is not available
     */
    void _onNetworkDeviceIsNotAvailable();


    /**
     * @brief Called when our network device is available again
     */
    void _onNetworkDeviceIsAvailableAgain();


    /**
     * @brief Called when our network device has a new IP address
     */
    void _onNetworkDeviceIpAddressHasChanged();


    /**
     * @brief Called when our machine will go to sleep
     */
    void _onSystemSleep();


    /**
     * @brief Called when our machine did wake from sleep
     */
    void _onSystemWake();


    /**
     * @brief Called when a network configuration is added, removed or changed
     */
    void _onSystemNetworkConfigurationsUpdated();


private:

    /**
     * @brief Load the platform from a JSON file
     * @param platformFilePath
     * @return
     */
    bool _loadPlatformFromFile(QString platformFilePath);


    /**
     * @brief Clear our current platform and load a new platform from a given file
     * @param platformFilePath
     * @return
     */
    bool _clearAndLoadPlatformFromFile(QString platformFilePath);


    /**
      * @brief Save the platform to a JSON file
      * @param platformFilePath
      */
    void _savePlatformToFile(QString platformFilePath);


    /**
     * @brief Load the platform from JSON
     * @param jsonDocument
     * @return
     */
    bool _loadPlatformFromJSON(QJsonDocument jsonDocument);


    /**
     * @brief Get the JSON of the current platform
     * @return
     */
    QJsonDocument _getJsonOfCurrentPlatform();


    /**
     * @brief Restart IngeScape
     */
    bool _restartIngeScape(bool hasToClearPlatform);


private:
    // To subscribe to termination signals
    TerminationSignalWatcher *_terminationSignalWatcher;

    // Path to the directory containing JSON files about platforms
    QString _platformDirectoryPath;

    // Path to the currently opened platform file (*.igsplatform)
    QString _currentPlatformFilePath;

    bool _wasAgentEditorStarted_beforeSystemSleep;

    // Default name to save the platform when exiting
    static const QString EXAMPLE_PLATFORM_NAME;

    // Default name when creating a new platform
    static const QString NEW_PLATFORM_NAME;

    // Default name when creating a new platform
    static const QString SPECIAL_EMPTY_LAST_PLATFORM;
};

QML_DECLARE_TYPE(IngeScapeEditorController)

#endif // INGESCAPEEDITORCONTROLLER_H
