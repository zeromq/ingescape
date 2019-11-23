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
 *
 */

#ifndef EDITORMODELMANAGER_H
#define EDITORMODELMANAGER_H

#include <QObject>
//#include <QtQml>
#include <I2PropertyHelpers.h>
#include <model/editorenums.h>
#include <controller/ingescapemodelmanager.h>


/**
 * @brief The EditorModelManager class defines the manager for the data model of IngeScape
 */
class EditorModelManager : public QObject
{
    Q_OBJECT

    // Flag indicating if our global mapping is controlled (or passive)
    I2_QML_PROPERTY_CUSTOM_SETTER(bool, isMappingControlled)

    // List of opened definitions
    I2_QOBJECT_LISTMODEL(DefinitionM, openedDefinitions)


public:

    /**
     * @brief Accessor to the singleton instance
     * @return
     */
    //static EditorModelManager* instance();


    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
     //static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


    /**
     * @brief Constructor
     * @param parent
     */
    explicit EditorModelManager(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~EditorModelManager();


    /**
     * @brief Export the agents into JSON
     * @return array of all agents (grouped by name)
     */
    QJsonArray exportAgentsToJSON();


    /**
     * @brief Export the agents list to selected file
     */
    Q_INVOKABLE void exportAgentsListToSelectedFile();


    /**
     * @brief Open a definition
     * If there are variants of this definition, we open each variant
     * @param definition
     */
    Q_INVOKABLE void openDefinition(DefinitionM* definition);


    /**
     * @brief Duplicate the agent with a new command line
     * @param agent
     * @param newCommandLine
     */
    Q_INVOKABLE void duplicateAgentWithNewCommandLine(AgentM* agent, QString newCommandLine);


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot called when a new view model of agents grouped by name has been created
     * @param agentsGroupedByName
     */
    void onAgentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Slot called when the flag "is Muted" from an agent updated
     * @param peerId
     * @param isMuted
     */
    void onisMutedFromAgentUpdated(QString peerId, bool isMuted);


    /**
     * @brief Slot called when the flag "can be Frozen" from an agent updated
     * @param peerId
     * @param canBeFrozen
     */
    void onCanBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen);


    /**
     * @brief Slot called when the flag "is Frozen" from an agent updated
     * @param peerId
     * @param isFrozen
     */
    void onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen);


    /**
     * @brief Slot called when the flag "is Muted" from an output of agent updated
     * @param peerId
     * @param isMuted
     * @param outputName
     */
    void onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName);


    /**
     * @brief Slot called when the state of an agent changes
     * @param peerId
     * @param stateName
     */
    void onAgentStateChanged(QString peerId, QString stateName);


    /**
     * @brief Slot called when we receive the flag "Log In Stream" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void onAgentHasLogInStream(QString peerId, bool hasLogInStream);


    /**
     * @brief Slot called when we receive the flag "Log In File" for an agent
     * @param peerId
     * @param hasLogInStream
     */
    void onAgentHasLogInFile(QString peerId, bool hasLogInFile);


    /**
     * @brief Slot called when we receive the path of "Log File" for an agent
     * @param peerId
     * @param logFilePath
     */
    void onAgentLogFilePath(QString peerId, QString logFilePath);


    /**
     * @brief Slot called when we receive the path of "Definition File" for an agent
     * @param peerId
     * @param definitionFilePath
     */
    void onAgentDefinitionFilePath(QString peerId, QString definitionFilePath);


    /**
     * @brief Slot called when we receive the path of "Mapping File" for an agent
     * @param peerId
     * @param mappingFilePath
     */
    void onAgentMappingFilePath(QString peerId, QString mappingFilePath);


private Q_SLOTS:

    /**
     * @brief Slot called when the definition(s) of an agent (agents grouped by name) must be opened
     * @param definitionsList
     */
    void _onDefinitionsToOpen(QList<DefinitionM*> definitionsList);


private:

    /**
     * @brief Open a list of definitions (if the definition is already opened, we bring it to front)
     * @param definitionsToOpen
     */
    void _openDefinitions(QList<DefinitionM*> definitionsToOpen);


private:


};

QML_DECLARE_TYPE(EditorModelManager)

#endif // EDITORMODELMANAGER_H
