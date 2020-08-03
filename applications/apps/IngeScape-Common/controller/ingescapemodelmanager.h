/*
 *	IngeScape Common
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef INGESCAPEMODELMANAGER_H
#define INGESCAPEMODELMANAGER_H

#include <QObject>
//#include <QtQml>
#include <I2PropertyHelpers.h>
#include <misc/ingescapeutils.h>
#include <model/enums.h>
#include <model/jsonhelper.h>
#include <model/publishedvaluem.h>
#include <model/peerm.h>
#include <model/hostm.h>
#include <viewModel/agentsgroupedbynamevm.h>


static const QString VERSION_JSON_PLATFORM = QString("1.0");
//static const QString VERSION_JSON_PLATFORM = QString("2.0");


/**
 * @brief The IngeScapeModelManager class defines the manager for the data model of IngeScape
 */
class IngeScapeModelManager : public QObject
{
    Q_OBJECT

    // List of all groups (of agents) grouped by name
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentsGroupedByNameVM, allAgentsGroupsByName)

    // List of all published values
    I2_QOBJECT_LISTMODEL(PublishedValueM, publishedValues)


public:

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    static IngeScapeModelManager* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
     * @brief Constructor
     * @param parent
     */
    explicit IngeScapeModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~IngeScapeModelManager();


    /**
     * @brief Create a new model of agent with a name, a definition (can be NULL) and some properties
     * @param agentName
     * @param definition optional (NULL by default)
     * @param hostname optional (default value)
     * @param commandLine optional (empty by default)
     * @param peerId optional (empty by default)
     * @param ipAddress optional (empty by default)
     * @param isON optional (false by default)
     * @return
     */
    AgentM* createAgentModel(QString agentName,
                             DefinitionM* definition = nullptr,
                             QString hostname = HOSTNAME_NOT_DEFINED,
                             QString commandLine = "",
                             QString peerId = "",
                             QString ipAddress = "",
                             bool isON = false);

    AgentM* createAgentModel(PeerM* peer,
                             QString agentUid,
                             QString agentName,
                             DefinitionM* definition = nullptr,
                             bool isON = false);


    /**
     * @brief Delete a model of agent
     * @param agent
     */
    void deleteAgentModel(AgentM* agent);


    /**
     * @brief Delete a view model of agents grouped by name
     * @param agentsGroupedByName
     */
    void deleteAgentsGroupedByName(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Get the model of host with a name
     * @param hostName
     * @return
     */
    HostM* getHostModelWithName(QString hostName);


    /**
     * @brief Get the peer id of the Launcher on a host
     * @param hostName
     * @return
     */
    QString getPeerIdOfLauncherOnHost(QString hostName);


    /**
     * @brief Get the model of agent from a UID
     */
    AgentM* getAgentModelFromUid(QString uid);


    /**
     * @brief Get the (view model of) agents grouped for a name
     * @param name
     * @return
     */
    AgentsGroupedByNameVM* getAgentsGroupedForName(QString name);


    /**
     * @brief Get the model of action with its (unique) id
     * @param actionId
     * @return
     */
    ActionM* getActionWithId(int actionId);


    /**
     * @brief Store a new model of action
     * @param action
     */
    void storeNewAction(ActionM* action);


    /**
     * @brief Delete a model of action
     * @param action
     */
    void deleteAction(ActionM* action);


    /**
     * @brief Delete all (models of) actions
     */
    void deleteAllActions();


    /**
     * @brief Get the hash table from a name to the group of agents with this name
     * @return
     */
    QHash<QString, AgentsGroupedByNameVM*> getHashTableFromNameToAgentsGrouped();


    /**
     * @brief Import an agent (with only its definition) or an agents list from a selected file
     * @return
     */
    Q_INVOKABLE bool importAgentOrAgentsListFromSelectedFile();


    /**
     * @brief Import an agent (with only its definition) or an agents list from a file path
     * @param filePath
     * @return
     */
    bool importAgentOrAgentsListFromFilePath(QString filePath);


    /**
     * @brief Import an agents list from a JSON array
     * @param jsonArrayOfAgents
     * @param versionJsonPlatform
     */
    bool importAgentsListFromJson(QJsonArray jsonArrayOfAgents, QString versionJsonPlatform);


    /**
     * @brief Simulate an exit for each agent ON
     */
    void simulateExitForEachAgentON();


    /**
     * @brief Simulate an exit for each launcher
     */
    void simulateExitForEachLauncher();


    /**
     * @brief Delete agents OFF
     * @return namesListOfAgentsON
     */
    QStringList deleteAgentsOFF();


    /**
     * @brief Delete all published values
     */
    void deleteAllPublishedValues();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a new model of agent has been created
     * @param agent
     */
    void agentModelHasBeenCreated(AgentM* agent);


    /**
     * @brief Signal emitted when a model of agent is back on the network
     * Special case when user changes the IngeScape port and go back on the previous IngeScape port
     * PeerId of agent model did not changed
     * @param agent
     */
    void agentModelBackOnNetwork(AgentM* agent);


    /**
     * @brief Signal emitted when a model of agent will be deleted
     * @param agent
     */
    void agentModelWillBeDeleted(AgentM* agent);


    /**
     * @brief Signal emitted when a new view model of agents grouped by name has been created
     * @param agentsGroupedByName
     */
    void agentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Signal emitted when a view model of agents grouped by name will be deleted
     * @param agentsGroupedByName
     */
    void agentsGroupedByNameWillBeDeleted(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Signal emitted when a new view model of agents grouped by definition has been created
     * @param agentsGroupedByDefinition
     */
    void agentsGroupedByDefinitionHasBeenCreated(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Signal emitted when a view model of agents grouped by definition will be deleted
     * @param agentsGroupedByDefinition
     */
    void agentsGroupedByDefinitionWillBeDeleted(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Signal emitted when a model of action will be deleted
     * @param action
     */
    void actionModelWillBeDeleted(ActionM* action);


    /**
     * @brief Signal emitted when a model of host has been created
     * @param host
     */
    void hostModelHasBeenCreated(HostM* host);


    /**
     * @brief Signal emitted when a model of host will be deleted
     * @param host
     */
    void hostModelWillBeDeleted(HostM* host);


    /**
     * @brief Signal emitted when a previous host has been parsed (in JSON file)
     * @param hostName
     */
    void previousHostParsed(QString hostName);


public Q_SLOTS:

    // Slot called when an agent enter/quit the network
    void onAgentEntered(PeerM* peer);
    void onAgentExited(PeerM* peer);


    // Slot called when a launcher enter/quit the network
    void onLauncherEntered(PeerM* peer);
    void onLauncherExited(PeerM* peer);
    

    /**
     * @brief Slot called when an agent definition has been received and must be processed
     */
    void onDefinitionReceived(PeerM* peer, QString agentUid, QString agentName, QString definitionJSON);


    /**
     * @brief Slot called when an agent mapping has been received and must be processed
     */
    void onMappingReceived(PeerM* peer, QString agentUid, QString mappingJSON);


    /**
     * @brief Slot called when a new value is published
     * @param publishedValue
     */
    void onValuePublished(PublishedValueM* publishedValue);


protected Q_SLOTS:

    /**
     * @brief Slot called when a model of agent has to be deleted
     * @param model
     */
    void _onAgentModelHasToBeDeleted(AgentM* model);


    /**
     * @brief Slot called when some view models of outputs have been added to an agent(s grouped by name)
     * @param newOutputs
     */
    void _onOutputsHaveBeenAddedToAgentsGroupedByName(QList<OutputVM*> newOutputs);


    /**
     * @brief Slot called when some view models of outputs will be removed from an agent(s grouped by name)
     * @param oldOutputs
     */
    void _onOutputsWillBeRemovedFromAgentsGroupedByName(QList<OutputVM*> oldOutputs);


    /**
     * @brief Slot called when a view model of agents grouped by name has become useless (no more agents grouped by definition)
     */
    void _onUselessAgentsGroupedByName();


    /**
     * @brief Slot called when the network data of an agent will be cleared
     * @param peerId
     */
    void _onNetworkDataOfAgentWillBeCleared(QString peerId);


private:

    /**
     * @brief Create a new view model of agents grouped by name
     * @param model
     */
    void _createAgentsGroupedByName(AgentM* model);


private:

    // Map from uid to a model of agent
    QHash<QString, AgentM*> _hashFromUidToAgent;

    // Hash table from a name to the group of agents with this name
    QHash<QString, AgentsGroupedByNameVM*> _hashFromNameToAgentsGrouped;

    // Hash table from name to a model of host (corresponding to an IngeScape launcher)
    QHash<QString, HostM*> _hashFromNameToHost;

    // Hash table from an action (Unique) ID to the corresponding model of action
    QHash<int, ActionM*> _hashFromUidToModelOfAction;

};

QML_DECLARE_TYPE(IngeScapeModelManager)

#endif // INGESCAPEMODELMANAGER_H
