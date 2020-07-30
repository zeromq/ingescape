/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2020 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#ifndef AGENTSGROUPEDBYDEFINITIONVM_H
#define AGENTSGROUPEDBYDEFINITIONVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/agent/agentm.h>


static const QString command_EnableLogStream = "ENABLE_LOG_STREAM";
static const QString command_DisableLogStream = "DISABLE_LOG_STREAM";
static const QString command_EnableLogFile = "ENABLE_LOG_FILE";
static const QString command_DisableLogFile = "DISABLE_LOG_FILE";
static const QString command_SetLogPath = "SET_LOG_PATH";
static const QString command_SetDefinitionPath = "SET_DEFINITION_PATH";
static const QString command_SetMappingPath = "SET_MAPPING_PATH";
static const QString command_SaveDefinitionToPath = "SAVE_DEFINITION_TO_PATH";
static const QString command_SaveMappingToPath = "SAVE_MAPPING_TO_PATH";


/**
 * @brief The AgentsGroupedByDefinitionVM class defines a view model of group of agents who have the same name and the same definition
 * Allows to manage several models of agents who have the same name and the same definition
 */
class AgentsGroupedByDefinitionVM : public QObject
{
    Q_OBJECT

    // Name of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, name)

    // Definition of our agent(s)
    I2_QML_PROPERTY_READONLY(DefinitionM*, definition)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // List of peer ids of models
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, peerIdsList)

    // Flag indicating if our agent(s) is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Hostname(s) on the network of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, hostnames)

    // Flag indicating if our agent can be restarted (by an IngeScape launcher)
    I2_QML_PROPERTY_READONLY(bool, canBeRestarted)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(bool, isMuted)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Number of clones
    I2_QML_PROPERTY_READONLY(int, clonesNumber)

    // State of our agent
    I2_QML_PROPERTY(QString, state)

    // Flag indicating if our agent has its log in a stream
    I2_QML_PROPERTY_READONLY(bool, hasLogInStream)

    // Flag indicating if our agent has its log in a file
    I2_QML_PROPERTY_READONLY(bool, hasLogInFile)

    // Path of the log file of our agent
    I2_QML_PROPERTY_READONLY(QString, logFilePath)

    // Path of the definition file of our agent
    I2_QML_PROPERTY_READONLY(QString, definitionFilePath)

    // Path of the mapping file of our agent
    I2_QML_PROPERTY_READONLY(QString, mappingFilePath)

    // Flag indicating if the option "View Log Stream" is enabled
    I2_QML_PROPERTY_READONLY(bool, isEnabledViewLogStream)

    // Flag indicating if our agents grouped by definition are selected in our agents list view
    // NB: only set by AgentsGroupedByNameVM
    I2_QML_PROPERTY_READONLY(bool, isSelected)

public:

    explicit AgentsGroupedByDefinitionVM(QString agentName,
                                         DefinitionM* definition,
                                         QObject *parent = nullptr);
    ~AgentsGroupedByDefinitionVM();


    /**
     * @brief Ask to start our agent
     */
    Q_INVOKABLE void askStartAgent();


    /**
     * @brief Ask to stop our agent
     */
    Q_INVOKABLE void askStopAgent();


    /**
     * @brief Mute / UN-mute all outputs of our agent
     */
    Q_INVOKABLE void changeMuteAllOutputs();


    /**
     * @brief Freeze / UN-freeze our agent
     */
    Q_INVOKABLE void changeFreeze();


    /**
     * @brief Load a (new) definition
     */
    Q_INVOKABLE void loadDefinition();


    /**
     * @brief Load a (new) mapping
     */
    Q_INVOKABLE void loadMapping();


    /**
     * @brief Download the current definition
     */
    Q_INVOKABLE void downloadDefinition();


    /**
     * @brief Download the current mapping
     */
    Q_INVOKABLE void downloadMapping();


    /**
     * @brief Change the flag "(has) Log in Stream"
     */
    Q_INVOKABLE void changeLogInStream();


    /**
     * @brief Change the flag "(has) Log in File"
     */
    Q_INVOKABLE void changeLogInFile();


    /**
     * @brief Save the "files paths" of our agent
     * @param definitionFilePath
     * @param mappingFilePath
     * @param logFilePath
     */
    Q_INVOKABLE void saveFilesPaths(QString definitionFilePath, QString mappingFilePath, QString logFilePath);


    /**
     * @brief Save the definition of our agent to its path
     */
    Q_INVOKABLE void saveDefinitionToPath();


    /**
     * @brief Save the mapping of our agent to its path
     */
    Q_INVOKABLE void saveMappingToPath();


    /**
     * @brief Open the "Log Stream" of our agent
     */
    Q_INVOKABLE void openLogStream();


    /**
     * @brief Get the list of agent models on a host
     * @param hostname
     * @return
     */
    QList<AgentM*> getModelsOnHost(QString hostname);


Q_SIGNALS:

    /**
     * @brief Signal emitted when our view model has become useless (no more model)
     */
    void noMoreModelAndUseless();


    /**
     * @brief Signal emitted when we have to open the values history of our agent
     * @param agentName
     */
    void openValuesHistoryOfAgent(QString agentName);


    /**
     * @brief Signal emitted when we have to open the "Log Stream" of a list of agents
     * @param models
     */
    void openLogStreamOfAgents(QList<AgentM*> models);


    /**
     * @brief Signal emitted when we have to load agent(s) definition from a JSON file (path)
     * @param peerIdsList
     * @param definitionFilePath
     */
    void loadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath);


    /**
     * @brief Signal emitted when we have to load agent(s) mapping from a JSON file (path)
     * @param mappingFilePath
     */
    void loadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath);


    /**
     * @brief Signal emitted when we have to download an agent definition to a JSON file (path)
     * @param agentDefinition
     * @param definitionFilePath
     */
    void downloadAgentDefinitionToPath(DefinitionM* agentDefinition, QString definitionFilePath);


    /**
     * @brief Signal emitted when we have to download an agent mapping to a JSON file (path)
     * @param agentMapping
     * @param mappingFilePath
     */
    void downloadAgentMappingToPath(AgentMappingM* agentMapping, QString mappingFilePath);


public Q_SLOTS:


private Q_SLOTS:

    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot called when the hostname of a model changed
     * @param hostname
     */
    void _onHostnameOfModelChanged(QString hostname);


    /**
     * @brief Slot called when the flag "is ON" of a model changed
     * @param isON
     */
    void _onIsONofModelChanged(bool isON);


    /**
     * @brief Slot called when the flag "can Be Restarted" of a model changed
     * @param canBeRestarted
     */
    void _onCanBeRestartedOfModelChanged(bool canBeRestarted);


    /**
     * @brief Slot called when the flag "is Muted" of a model changed
     * @param isMuted
     */
    void _onIsMutedOfModelChanged(bool isMuted);


    /**
     * @brief Slot called when the flag "is Frozen" of a model changed
     * @param isMuted
     */
    void _onIsFrozenOfModelChanged(bool isFrozen);


    /**
     * @brief Slot called when the state of a model changed
     * @param state
     */
    void _onStateOfModelChanged(QString state);


    /**
     * @brief Slot called when the "Logger Port" of a model changed
     * @param loggerPort
     */
    void _onLoggerPortOfModelChanged(QString loggerPort);


    /**
     * @brief Slot called when the flag "has Log in Stream" of a model changed
     * @param hasLogInStream
     */
    void _onHasLogInStreamOfModelChanged(bool hasLogInStream);


    /**
     * @brief Slot called when the flag "has Log in File" of a model changed
     * @param hasLogInFile
     */
    void _onHasLogInFileOfModelChanged(bool hasLogInFile);


    /**
     * @brief Slot called when the path of "Log File" of a model changed
     * @param logFilePath
     */
    void _onLogFilePathOfModelChanged(QString logFilePath);


    /**
     * @brief Slot called when the path of "Definition File" of a model changed
     * @param definitionFilePath
     */
    void _onDefinitionFilePathOfModelChanged(QString definitionFilePath);


    /**
     * @brief Slot called when the path of "Mapping File" of a model changed
     * @param mappingFilePath
     */
    void _onMappingFilePathOfModelChanged(QString mappingFilePath);


    /**
     * @brief Slot called when the flag "is Muted Output" of an output (of a model) changed
     * @param isMutedOutput
     * @param outputName
     */
    void _onIsMutedOutputOfModelChanged(bool isMutedOutput, QString outputName);


    /**
     * @brief Slot called when a command must be sent on the network to agent(s) about one of its output
     * @param command
     * @param outputName
     */
    void _onCommandAskedToAgentAboutOutput(QString command, QString outputName);


    /**
     * @brief Slot called when we have to open the values history of our agent
     */
    void _onOpenValuesHistoryOfAgent();


private:
    /**
     * @brief Update with all models of agents
     */
    void _updateWithAllModels();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // Hash table from a hostname to a list of models of agents
    QHash<QString, QList<AgentM*>> _hashFromHostnameToModels;

};

QML_DECLARE_TYPE(AgentsGroupedByDefinitionVM)

#endif // AGENTSGROUPEDBYDEFINITIONVM_H
