/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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

#include "agentssupervisioncontroller.h"

#include <QQmlEngine>
#include <QDebug>
#include <model/editorenums.h>


/**
 * @brief Constructor
 * @param modelManager
 * @param jsonHelper
 * @param parent
 */
AgentsSupervisionController::AgentsSupervisionController(EditorModelManager* modelManager,
                                                         JsonHelper* jsonHelper,
                                                         QObject *parent) : QObject(parent),
    _selectedAgent(nullptr),
    _modelManager(modelManager),
    _jsonHelper(jsonHelper)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != nullptr)
    {
        // Agents are sorted on their name (alphabetical order)
        _agentsList.setSortProperty("name");

        // FIXME TODO: Add another property for sorting, because variants can provoke re-order in the list
        //_agentsList.setSortProperty("TODO");

        //_agentsList.setFilterProperty("TODO");
        //_agentsList.setFilterFixedString("true");
        //_agentsList.setFilterProperty("currentState");
        //_agentsList.setFilterFixedString(QString::number(SegmentZoneStates::ENCOMBRE));
    }
}


/**
 * @brief Destructor
 */
AgentsSupervisionController::~AgentsSupervisionController()
{
    // Clean-up current selection
    setselectedAgent(nullptr);

    _hashFromDefinitionNameToDefinitionsList.clear();

    //_mapFromNameToAgentViewModelsList.clear();

    // Deleted elsewhere (in the destructor of AgentsGroupedByNameVM)
    //_agentsList.deleteAllItems();
    _agentsList.clear();

    // Reset pointers
    _modelManager = nullptr;
    _jsonHelper = nullptr;
}


/**
 * @brief Remove the agent from the list and delete it
 * @param agent
 */
void AgentsSupervisionController::deleteAgentInList(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if ((_modelManager != nullptr) && (agentsGroupedByDefinition != nullptr))
    {
        qInfo() << "Delete the agent" << agentsGroupedByDefinition->name() << "in the List";

        AgentsGroupedByNameVM* agentsGroupedByName = _modelManager->getAgentsGroupedForName(agentsGroupedByDefinition->name());
        if (agentsGroupedByName != nullptr) {
            // Delete the view model of agents grouped by definition
            agentsGroupedByName->deleteAgentsGroupedByDefinition(agentsGroupedByDefinition);
        }
    }
}


/**
 * @brief Slot called when a new view model of agents grouped by definition has been created
 * @param agentsGroupedByDefinition
 */
void AgentsSupervisionController::onAgentsGroupedByDefinitionHasBeenCreated(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Propagate some signals from this new view model of agents grouped by definition
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToAgent, this, &AgentsSupervisionController::commandAskedToAgent);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToAgentAboutOutput, this, &AgentsSupervisionController::commandAskedToAgentAboutOutput);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::openValuesHistoryOfAgent, this, &AgentsSupervisionController::openValuesHistoryOfAgent);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::openLogStreamOfAgents, this, &AgentsSupervisionController::openLogStreamOfAgents);

        // Connect some signals from this new view model of agents grouped by definition to slots
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::commandAskedToLauncher, this, &AgentsSupervisionController::_onCommandAskedToLauncher);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::loadAgentDefinitionFromPath, this, &AgentsSupervisionController::_onLoadAgentDefinitionFromPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::loadAgentMappingFromPath, this, &AgentsSupervisionController::_onLoadAgentMappingFromPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::downloadAgentDefinitionToPath, this, &AgentsSupervisionController::_onDownloadAgentDefinitionToPath);
        connect(agentsGroupedByDefinition, &AgentsGroupedByDefinitionVM::downloadAgentMappingToPath, this, &AgentsSupervisionController::_onDownloadAgentMappingToPath);

        // Add our view model to the list
        _agentsList.append(agentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();
            QString definitionName = definition->name();

            qDebug() << "on Agents Grouped by Definition" << definitionName << "has been Created" << agentsGroupedByDefinition->name();

            // Get the list of definitions with a name
            QList<DefinitionM*> definitionsList = _getDefinitionsListWithName(definitionName);

            definitionsList.append(definition);
            _hashFromDefinitionNameToDefinitionsList.insert(definitionName, definitionsList);

            // Update the definition variants (same name, same version but the lists of I/O/P are differents)
            _updateDefinitionVariants(definitionName, definitionsList);
        }
        else
        {
            qDebug() << "on Agents Grouped by Definition 'NULL' has been Created" << agentsGroupedByDefinition->name();
        }
    }
}


/**
 * @brief Slot called when a view model of agents grouped by definition will be deleted
 * @param agentsGroupedByDefinition
 */
void AgentsSupervisionController::onAgentsGroupedByDefinitionWillBeDeleted(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition)
{
    if (agentsGroupedByDefinition != nullptr)
    {
        // Unselect our agent if needed
        if (_selectedAgent == agentsGroupedByDefinition) {
            setselectedAgent(nullptr);
        }

        // DIS-connect to signals from this view model of agents grouped by definition
        disconnect(agentsGroupedByDefinition, nullptr, this, nullptr);

        // Remove it from the list
        _agentsList.remove(agentsGroupedByDefinition);

        if (agentsGroupedByDefinition->definition() != nullptr)
        {
            DefinitionM* definition = agentsGroupedByDefinition->definition();
            QString definitionName = definition->name();

            qDebug() << "on Agents Grouped by Definition" << definitionName << "will be Deleted" << agentsGroupedByDefinition->name();

            // Get the list of definitions with a name
            QList<DefinitionM*> definitionsList = _getDefinitionsListWithName(definitionName);

            definitionsList.removeOne(definition);
            _hashFromDefinitionNameToDefinitionsList.insert(definitionName, definitionsList);

            // Update the definition variants (same name, same version but the lists of I/O/P are differents)
            _updateDefinitionVariants(definitionName, definitionsList);
        }
        else
        {
            qDebug() << "on Agents Grouped by Definition 'NULL' will be Deleted" << agentsGroupedByDefinition->name();
        }
    }
}


/**
 * @brief Slot called when a command must be sent on the network to a launcher
 * @param hostname
 * @param command
 * @param commandLine
 */
void AgentsSupervisionController::_onCommandAskedToLauncher(QString hostname, QString command, QString commandLine)
{
    if (_modelManager != nullptr)
    {
        // Get the peer id of the Launcher on this host
        QString peerIdOfLauncher = _modelManager->getPeerIdOfLauncherOnHost(hostname);
        if (!peerIdOfLauncher.isEmpty())
        {
            Q_EMIT commandAskedToLauncher(peerIdOfLauncher, command, commandLine);
        }
    }
}


/**
 * @brief Slot called when we have to load an agent definition from a JSON file (path)
 * @param peerIdsList
 * @param definitionFilePath
 */
void AgentsSupervisionController::_onLoadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && !peerIdsList.isEmpty() && !definitionFilePath.isEmpty())
    {
        QFile jsonFile(definitionFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            // Compact JSON
            QString jsonOfDefinition = QString(jsonDocument.toJson(QJsonDocument::Compact));

            // Create the command "Load Definition"
            QString command = QString("%1%2").arg(command_LoadDefinition, jsonOfDefinition);

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }
        else {
            qCritical() << "Can not open file" << definitionFilePath << "(to load the definition of" << agentsGroupedByDefinition->name() << ")";
        }
    }
}


/**
 * @brief Slot called when we have to load an agent mapping from a JSON file (path)
 * @param mappingFilePath
 */
void AgentsSupervisionController::_onLoadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && !peerIdsList.isEmpty() && !mappingFilePath.isEmpty())
    {
        QFile jsonFile(mappingFilePath);
        if (jsonFile.open(QIODevice::ReadOnly))
        {
            QByteArray byteArrayOfJson = jsonFile.readAll();
            jsonFile.close();

            QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArrayOfJson);

            // Compact JSON
            QString jsonOfMapping = QString(jsonDocument.toJson(QJsonDocument::Compact));

            // Create the command "Load Mapping"
            QString command = QString("%1%2").arg(command_LoadMapping, jsonOfMapping);

            Q_EMIT commandAskedToAgent(peerIdsList, command);
        }
        else {
            qCritical() << "Can not open file" << mappingFilePath << "(to load the mapping of" << agentsGroupedByDefinition->name() << ")";
        }
    }
}


/**
 * @brief Slot called when we have to download an agent definition to a JSON file (path)
 * @param agentDefinition
 * @param definitionFilePath
 */
void AgentsSupervisionController::_onDownloadAgentDefinitionToPath(DefinitionM* agentDefinition, QString definitionFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && (agentDefinition != nullptr) && !definitionFilePath.isEmpty())
    {
        // Get the JSON of the agent definition
        QString jsonOfDefinition = _jsonHelper->getJsonOfAgentDefinition(agentDefinition, QJsonDocument::Indented);
        if (!jsonOfDefinition.isEmpty())
        {
            QFile jsonFile(definitionFilePath);
            //if (jsonFile.exists()) {
            //    qWarning() << "The file" << definitionFilePath << "already exist !";
            //}

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfDefinition.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << definitionFilePath << "(to save the definition of" << agentsGroupedByDefinition->name() << ")";
            }
        }
    }
}


/**
 * @brief Slot called when we have to download an agent mapping to a JSON file (path)
 * @param agentMapping
 * @param mappingFilePath
 */
void AgentsSupervisionController::_onDownloadAgentMappingToPath(AgentMappingM* agentMapping, QString mappingFilePath)
{
    AgentsGroupedByDefinitionVM* agentsGroupedByDefinition = qobject_cast<AgentsGroupedByDefinitionVM*>(sender());
    if ((_jsonHelper != nullptr) && (agentsGroupedByDefinition != nullptr) && (agentMapping != nullptr) && !mappingFilePath.isEmpty())
    {
        // Get the JSON of the agent mapping
        QString jsonOfMapping = _jsonHelper->getJsonOfAgentMapping(agentMapping, QJsonDocument::Indented);
        if (!jsonOfMapping.isEmpty())
        {
            QFile jsonFile(mappingFilePath);
            //if (jsonFile.exists()) {
            //    qWarning() << "The file" << mappingFilePath << "already exist !";
            //}

            if (jsonFile.open(QIODevice::WriteOnly))
            {
                jsonFile.write(jsonOfMapping.toUtf8());
                jsonFile.close();
            }
            else {
                qCritical() << "Can not open file" << mappingFilePath << "(to save the mapping of" << agentsGroupedByDefinition->name() << ")";
            }
        }
    }
}


/**
 * @brief Get the list of definitions with a name
 * @param definitionName
 * @return
 */
QList<DefinitionM*> AgentsSupervisionController::_getDefinitionsListWithName(QString definitionName)
{
    if (_hashFromDefinitionNameToDefinitionsList.contains(definitionName)) {
        return _hashFromDefinitionNameToDefinitionsList.value(definitionName);
    }
    else {
        return QList<DefinitionM*>();
    }
}


/**
 * @brief Update the definition variants (same name, same version but the lists of I/O/P are differents)
 * @param definitionName
 * @param definitionsList
 */
void AgentsSupervisionController::_updateDefinitionVariants(QString definitionName, QList<DefinitionM*> definitionsList)
{
    //Q_UNUSED(definitionName)
    qDebug() << "Update the definition variants for definition name" << definitionName << "(" << definitionsList.count() << "definitions)";

    // We can use versions as keys of the map because the list contains only definition with the same name
    QHash<QString, QList<DefinitionM*>> hashFromVersionToDefinitionsList;
    QList<QString> versionsWithVariant;

    for (DefinitionM* iterator : definitionsList)
    {
        if ((iterator != nullptr) && !iterator->version().isEmpty())
        {
            // First, reset all
            iterator->setisVariant(false);

            QString version = iterator->version();
            QList<DefinitionM*> definitionsListForVersion;

            // Other(s) definition(s) have the same version (and the same name)
            if (hashFromVersionToDefinitionsList.contains(version))
            {
                definitionsListForVersion = hashFromVersionToDefinitionsList.value(version);

                // If this version is not already in the list of versions with variant
                if (!versionsWithVariant.contains(version))
                {
                    // We compare I/O/P between current iterator and the first one
                    DefinitionM* first = definitionsListForVersion.first();
                    if ((first != nullptr) && !DefinitionM::areIdenticals(first, iterator)) {
                        versionsWithVariant.append(version);
                    }
                }
            }

            definitionsListForVersion.append(iterator);
            hashFromVersionToDefinitionsList.insert(version, definitionsListForVersion);
        }
    }

    // The list contains only the versions that have variants
    for (QString version : versionsWithVariant)
    {
        QList<DefinitionM*> definitionsListForVersion = hashFromVersionToDefinitionsList.value(version);
        for (DefinitionM* iterator : definitionsListForVersion)
        {
            if (iterator != nullptr) {
                iterator->setisVariant(true);
                //qDebug() << iterator->name() << iterator->version() << "is variant";
            }
        }
    }
}
