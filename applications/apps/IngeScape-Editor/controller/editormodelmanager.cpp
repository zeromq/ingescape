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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *      Chloé Roumieu      <roumieu@ingenuity.io>
 */

#include "editormodelmanager.h"

#include <QQmlEngine>
#include <QFileDialog>
#include <I2Quick.h>


// Define our singleton instance
// Creates a global and static object of type QGlobalStatic, of name _singletonInstance and that behaves as a pointer to EditorModelManager.
// The object created by Q_GLOBAL_STATIC initializes itself on the first use, which means that it will not increase the application or the library's load time.
// Additionally, the object is initialized in a thread-safe manner on all platforms.
//Q_GLOBAL_STATIC(EditorModelManager, _singletonInstance)


/**
 * @brief Get our singleton instance
 * @return
 */
/*EditorModelManager* EditorModelManager::instance()
{
    return _singletonInstance;
}*/


/**
 * @brief Method used to provide a singleton to QML
 * @param engine
 * @param scriptEngine
 * @return
 */
/*QObject* EditorModelManager::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);

    return _singletonInstance;
}*/


/**
 * @brief Constructor
 * @param parent
 */
EditorModelManager::EditorModelManager(QObject *parent) : QObject(parent),
    _imposeMappingToAgentsON(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    qInfo() << "New IngeScape Editor Model Manager";
}


/**
 * @brief Destructor
 */
EditorModelManager::~EditorModelManager()
{
    qInfo() << "Delete IngeScape Editor Model Manager";

    // Clear all opened definitions
    _openedDefinitions.clear();
}


/**
 * @brief Export the agents into JSON
 * @return array of all agents (grouped by name)
 */
QJsonArray EditorModelManager::exportAgentsToJSON()
{
    QJsonArray jsonArrayAgentsGroupedByName = QJsonArray();

    // List of all groups (of agents) grouped by name
    for (AgentsGroupedByNameVM* agentsGroupedByName : IngeScapeModelManager::instance()->allAgentsGroupsByName()->toList())
    {
        if ((agentsGroupedByName != nullptr) && !agentsGroupedByName->name().isEmpty())
        {
            QJsonObject jsonAgentsGroupedByName = QJsonObject();

            // Name
            jsonAgentsGroupedByName.insert("agentName", agentsGroupedByName->name());

            QJsonArray jsonArrayAgentsGroupedByDefinition = QJsonArray();

            // List of all groups (of agents) grouped by definition
            for (AgentsGroupedByDefinitionVM* agentsGroupedByDefinition : agentsGroupedByName->allAgentsGroupsByDefinition()->toList())
            {
                if (agentsGroupedByDefinition != nullptr)
                {
                    QJsonObject jsonAgentsGroupedByDefinition = QJsonObject();

                    // Definition
                    if (agentsGroupedByDefinition->definition() != nullptr)
                    {
                        QJsonObject jsonDefinition = JsonHelper::exportAgentDefinitionToJson(agentsGroupedByDefinition->definition());
                        jsonAgentsGroupedByDefinition.insert("definition", jsonDefinition);
                    }
                    else {
                        jsonAgentsGroupedByDefinition.insert("definition", QJsonValue());
                    }

                    // Clones (models)
                    QJsonArray jsonClones = QJsonArray();

                    for (AgentM* model : agentsGroupedByDefinition->models()->toList())
                    {
                        // Hostname and Command Line must be defined to be added to the array of clones
                        if ((model != nullptr) && !model->hostname().isEmpty() && !model->commandLine().isEmpty())
                        {
                            qDebug() << "Export" << model->name() << "on" << model->hostname() << "at" << model->commandLine() << "(" << model->peerId() << ")";

                            QJsonObject jsonClone = QJsonObject();
                            jsonClone.insert("hostname", model->hostname());
                            jsonClone.insert("commandLine", model->commandLine());
                            //jsonClone.insert("peerId", model->peerId());
                            //jsonClone.insert("address", model->address());

                            jsonClones.append(jsonClone);
                        }
                    }

                    jsonAgentsGroupedByDefinition.insert("clones", jsonClones);

                    jsonArrayAgentsGroupedByDefinition.append(jsonAgentsGroupedByDefinition);
                }
            }

            jsonAgentsGroupedByName.insert("definitions", jsonArrayAgentsGroupedByDefinition);

            jsonArrayAgentsGroupedByName.append(jsonAgentsGroupedByName);
        }
    }
    return jsonArrayAgentsGroupedByName;
}


/**
 * @brief Export the agents list to selected file
 */
void EditorModelManager::exportAgentsListToSelectedFile()
{
    // "File Dialog" to get the file (path) to save
    QString agentsListFilePath = QFileDialog::getSaveFileName(nullptr,
                                                              tr("Save agents"),
                                                              IngeScapeUtils::getRootPath(),
                                                              tr("List of agents (*.json)")
                                                              );

    if (!agentsListFilePath.isEmpty())
    {
        qInfo() << "Save the agents list to JSON file" << agentsListFilePath;

        // Export the agents into JSON
        QJsonArray jsonArrayOfAgents = exportAgentsToJSON();

        QJsonObject jsonRoot = QJsonObject();

        jsonRoot.insert("version", VERSION_JSON_PLATFORM);

        jsonRoot.insert("agents", jsonArrayOfAgents);

        QByteArray byteArrayOfJson = QJsonDocument(jsonRoot).toJson(QJsonDocument::Indented);

        QFile jsonFile(agentsListFilePath);
        if (jsonFile.open(QIODevice::WriteOnly))
        {
            jsonFile.write(byteArrayOfJson);
            jsonFile.close();
        }
        else {
            qCritical() << "Can not open file" << agentsListFilePath;
        }
    }
}


/**
 * @brief Open a definition
 * If there are variants of this definition, we open each variant
 * @param definition
 */
void EditorModelManager::openDefinition(DefinitionM* definition)
{
    if (definition != nullptr)
    {
        QString definitionName = definition->name();

        qDebug() << "Open the definition" << definitionName;

        QList<DefinitionM*> definitionsToOpen;

        /*// Variant --> we have to open each variants of this definition
        if (definition->isVariant())
        {
            for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
            {
                if (agentsGroupedByName != nullptr)
                {
                    // Get the list of definitions with a specific name
                    QList<DefinitionM*> definitionsList = agentsGroupedByName->getDefinitionsWithName(definitionName);

                    if (!definitionsList.isEmpty())
                    {
                        for (DefinitionM* iterator : definitionsList)
                        {
                            // Same name, same version and variant, we have to open it
                            if ((iterator != nullptr) && (iterator->version() == definition->version()) && iterator->isVariant()) {
                                definitionsToOpen.append(iterator);
                            }
                        }
                    }
                }
            }
        }
        else
        {
            // Simply add the definition
            definitionsToOpen.append(definition);
        }*/

        // Simply add the definition
        definitionsToOpen.append(definition);

        // Open the list of definitions
        _openDefinitions(definitionsToOpen);
    }
}


/**
 * @brief Duplicate the agent with a new command line
 * @param agent
 * @param newCommandLine
 */
void EditorModelManager::duplicateAgentWithNewCommandLine(AgentM* agent, QString newCommandLine)
{
    if (agent != nullptr)
    {
         qDebug() << "Duplicate the agent" << agent->name() << "with the new command line" << newCommandLine << "on" << agent->hostname();

         DefinitionM* copyOfDefinition = nullptr;
         if (agent->definition() != nullptr) {
             copyOfDefinition = agent->definition()->copy();
         }

         // Duplicate the agent with the new command line
         IngeScapeModelManager::instance()->createAgentModel(agent->name(),
                                                             copyOfDefinition,
                                                             agent->hostname(),
                                                             newCommandLine);
    }
}


/**
 * @brief Slot called when a new view model of agents grouped by name has been created
 * @param agentsGroupedByName
 */
void EditorModelManager::onAgentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName)
{
    if (agentsGroupedByName != nullptr)
    {
        // Connect to signals from this new view model of agents grouped by definition
        connect(agentsGroupedByName, &AgentsGroupedByNameVM::definitionsToOpen, this, &EditorModelManager::_onDefinitionsToOpen);
    }
}


/**
 * @brief Slot called when the flag "is Muted" from an agent updated
 * @param peerId
 * @param isMuted
 */
void EditorModelManager::onisMutedFromAgentUpdated(QString peerId, bool isMuted)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisMuted(isMuted);
    }
}


/**
 * @brief Slot called when the flag "can be Frozen" from an agent updated
 * @param peerId
 * @param canBeFrozen
 */
void EditorModelManager::onCanBeFrozenFromAgentUpdated(QString peerId, bool canBeFrozen)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setcanBeFrozen(canBeFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Frozen" from an agent updated
 * @param peerId
 * @param isFrozen
 */
void EditorModelManager::onIsFrozenFromAgentUpdated(QString peerId, bool isFrozen)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisFrozen(isFrozen);
    }
}


/**
 * @brief Slot called when the flag "is Muted" from an output of agent updated
 * @param peerId
 * @param isMuted
 * @param outputName
 */
void EditorModelManager::onIsMutedFromOutputOfAgentUpdated(QString peerId, bool isMuted, QString outputName)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setisMutedOfOutput(isMuted, outputName);
    }
}


/**
 * @brief Slot called when the state of an agent changes
 * @param peerId
 * @param stateName
 */
void EditorModelManager::onAgentStateChanged(QString peerId, QString stateName)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setstate(stateName);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In Stream" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void EditorModelManager::onAgentHasLogInStream(QString peerId, bool hasLogInStream)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->sethasLogInStream(hasLogInStream);
    }
}


/**
 * @brief Slot called when we receive the flag "Log In File" for an agent
 * @param peerId
 * @param hasLogInStream
 */
void EditorModelManager::onAgentHasLogInFile(QString peerId, bool hasLogInFile)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->sethasLogInFile(hasLogInFile);
    }
}


/**
 * @brief Slot called when we receive the path of "Log File" for an agent
 * @param peerId
 * @param logFilePath
 */
void EditorModelManager::onAgentLogFilePath(QString peerId, QString logFilePath)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setlogFilePath(logFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Definition File" for an agent
 * @param peerId
 * @param definitionFilePath
 */
void EditorModelManager::onAgentDefinitionFilePath(QString peerId, QString definitionFilePath)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setdefinitionFilePath(definitionFilePath);
    }
}


/**
 * @brief Slot called when we receive the path of "Mapping File" for an agent
 * @param peerId
 * @param mappingFilePath
 */
void EditorModelManager::onAgentMappingFilePath(QString peerId, QString mappingFilePath)
{
    AgentM* agent = IngeScapeModelManager::instance()->getAgentModelFromPeerId(peerId);
    if (agent != nullptr) {
        agent->setmappingFilePath(mappingFilePath);
    }
}


/**
 * @brief Slot called when the definition(s) of an agent (agents grouped by name) must be opened
 * @param definitionsList
 */
void EditorModelManager::_onDefinitionsToOpen(QList<DefinitionM*> definitionsList)
{
    _openDefinitions(definitionsList);
}


/**
 * @brief Open a list of definitions (if the definition is already opened, we bring it to front)
 * @param definitionsToOpen
 */
void EditorModelManager::_openDefinitions(QList<DefinitionM*> definitionsToOpen)
{
    // Traverse the list of definitions to open
    for (DefinitionM* definition : definitionsToOpen)
    {
        if (definition != nullptr)
        {
            if (!_openedDefinitions.contains(definition)) {
                _openedDefinitions.append(definition);
            }
            else {
                qDebug() << "The 'Definition'" << definition->name() << "is already opened...bring it to front !";

                Q_EMIT definition->bringToFront();
            }
        }
    }
}
