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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "editormodelmanager.h"

#include <QQmlEngine>
#include <QFileDialog>
#include <I2Quick.h>


/**
 * @brief Constructor
 * @param jsonHelper
 * @param rootDirectoryPath
 * @param parent
 */
EditorModelManager::EditorModelManager(JsonHelper* jsonHelper,
                                       QString rootDirectoryPath,
                                       QObject *parent) : IngeScapeModelManager(jsonHelper,
                                                                                rootDirectoryPath,
                                                                                parent),
    //_isMappingActivated(false),
    _isMappingControlled(false)
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

    // Mother class is automatically called
    //IngeScapeModelManager::~IngeScapeModelManager();
}


/**
 * @brief Setter for property "is Mapping Controlled"
 * @param value
 */
void EditorModelManager::setisMappingControlled(bool value)
{
    if (_isMappingControlled != value)
    {
        _isMappingControlled = value;

        if (_isMappingControlled) {
            qInfo() << "Mapping Controlled";
        }
        else {
            qInfo() << "Mapping Observed";
        }

        Q_EMIT isMappingControlledChanged(value);
    }
}


/**
 * @brief Export the agents into JSON
 * @return array of all agents (grouped by name)
 */
QJsonArray EditorModelManager::exportAgentsToJSON()
{
    QJsonArray jsonArrayAgentsGroupedByName = QJsonArray();

    if (_jsonHelper != nullptr)
    {
        // List of all groups (of agents) grouped by name
        for (AgentsGroupedByNameVM* agentsGroupedByName : _allAgentsGroupsByName.toList())
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
                            QJsonObject jsonDefinition = _jsonHelper->exportAgentDefinitionToJson(agentsGroupedByDefinition->definition());
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
                                                              _rootDirectoryPath,
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
         createAgentModel(agent->name(),
                          copyOfDefinition,
                          agent->hostname(),
                          newCommandLine);
    }
}


/**
 * @brief Slot called when an agent definition has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param definition in JSON format
 */
/*void EditorModelManager::onDefinitionReceived(QString peerId, QString agentName, QString definitionJSON)
{
    Q_UNUSED(agentName)

    AgentM* agent = getAgentModelFromPeerId(peerId);

    if ((agent != nullptr) && (_jsonHelper != nullptr) && !definitionJSON.isEmpty())
    {
        // Save the previous agent definition
        DefinitionM* previousDefinition = agent->definition();

        // Create the new model of agent definition from JSON
        DefinitionM* newDefinition = _jsonHelper->createModelOfAgentDefinitionFromBytes(definitionJSON.toUtf8());

        if (newDefinition != nullptr)
        {
            // Set this new definition to the agent
            agent->setdefinition(newDefinition);

            // Free memory
            if (previousDefinition != nullptr) {
                delete previousDefinition;
            }
        }
    }
}*/


/**
 * @brief Slot called when an agent mapping has been received and must be processed
 * @param peer Id
 * @param agent name
 * @param mapping in JSON format
 */
/*void EditorModelManager::onMappingReceived(QString peerId, QString agentName, QString mappingJSON)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);

    if ((agent != nullptr) && (_jsonHelper != nullptr))
    {
        // Save the previous agent mapping
        AgentMappingM* previousMapping = agent->mapping();

        AgentMappingM* newMapping = nullptr;

        if (mappingJSON.isEmpty())
        {
            QString mappingName = QString("EMPTY MAPPING of %1").arg(agentName);
            newMapping = new AgentMappingM(mappingName, "", "");
        }
        else
        {
            // Create the new model of agent mapping from the JSON
            newMapping = _jsonHelper->createModelOfAgentMappingFromBytes(agentName, mappingJSON.toUtf8());
        }

        if (newMapping != nullptr)
        {
            // Set this new mapping to the agent
            agent->setmapping(newMapping);

            // Free memory
            if (previousMapping != nullptr) {
                delete previousMapping;
            }
        }
    }
}*/


/**
 * @brief Slot called when the flag "is Muted" from an agent updated
 * @param peerId
 * @param isMuted
 */
void EditorModelManager::onisMutedFromAgentUpdated(QString peerId, bool isMuted)
{
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
    AgentM* agent = getAgentModelFromPeerId(peerId);
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
 * @brief Create a new view model of agents grouped by name
 * @param model
 */
void EditorModelManager::_createAgentsGroupedByName(AgentM* model)
{
    // Call our mother class
    IngeScapeModelManager::_createAgentsGroupedByName(model);

    if ((model != nullptr) && !model->name().isEmpty())
    {
        // Get the (view model of) agents grouped for this name
        AgentsGroupedByNameVM* agentsGroupedByName = getAgentsGroupedForName(model->name());
        if (agentsGroupedByName != nullptr)
        {
            // Connect to signals from this new view model of agents grouped by definition
            connect(agentsGroupedByName, &AgentsGroupedByNameVM::definitionsToOpen, this, &EditorModelManager::_onDefinitionsToOpen);
        }
    }
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
