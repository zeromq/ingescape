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

#ifndef AGENTSSUPERVISIONCONTROLLER_H
#define AGENTSSUPERVISIONCONTROLLER_H

#include <QObject>
#include <QtQml>
#include <I2PropertyHelpers.h>
#include <controller/editormodelmanager.h>
#include <viewModel/agentsgroupedbydefinitionvm.h>


/**
 * @brief The AgentsSupervisionController class defines the controller for agents supervision
 */
class AgentsSupervisionController : public QObject
{
    Q_OBJECT

    // Sorted list of agents
    I2_QOBJECT_LISTMODEL_WITH_SORTFILTERPROXY(AgentsGroupedByDefinitionVM, agentsList)


public:

    explicit AgentsSupervisionController(QObject *parent = nullptr);
    ~AgentsSupervisionController();

    /**
     * @brief Remove the agent from the list and delete it
     */
    Q_INVOKABLE void deleteAgentInList(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);

    /**
     * @brief Get Agents grouped by name for this definition
     */
    Q_INVOKABLE AgentsGroupedByNameVM* getAgentsGroupedByName(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);

Q_SIGNALS:

    /**
     * @brief Signal emitted when we have to open the values history of an agent
     * @param agentName
     */
    void openValuesHistoryOfAgent(QString agentName);


    /**
     * @brief Signal emitted when we have to open the "Log Stream" of a list of agents
     * @param models
     */
    void openLogStreamOfAgents(QList<AgentM*> models);


public Q_SLOTS:

    /**
     * @brief Slot called when a new view model of agents grouped by definition has been created
     * @param agentsGroupedByDefinition
     */
    void onAgentsGroupedByDefinitionHasBeenCreated(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Slot called when a view model of agents grouped by definition will be deleted
     * @param agentsGroupedByDefinition
     */
    void onAgentsGroupedByDefinitionWillBeDeleted(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


private Q_SLOTS:

    /**
     * @brief Slot called when we have to load an agent definition from a JSON file (path)
     * @param peerIdsList
     * @param definitionFilePath
     */
    void _onLoadAgentDefinitionFromPath(QStringList peerIdsList, QString definitionFilePath);


    /**
     * @brief Slot called when we have to load an agent mapping from a JSON file (path)
     * @param mappingFilePath
     */
    void _onLoadAgentMappingFromPath(QStringList peerIdsList, QString mappingFilePath);


    /**
     * @brief Slot called when we have to download an agent definition to a JSON file (path)
     * @param agentDefinition
     * @param definitionFilePath
     */
    void _onDownloadAgentDefinitionToPath(DefinitionM* agentDefinition, QString definitionFilePath);


    /**
     * @brief Slot called when we have to download an agent mapping to a JSON file (path)
     * @param agentMapping
     * @param mappingFilePath
     */
    void _onDownloadAgentMappingToPath(AgentMappingM* agentMapping, QString mappingFilePath);


private:

    /**
     * @brief Get the list of definitions with a name
     * @param definitionName
     * @return
     */
    QList<DefinitionM*> _getDefinitionsListWithName(QString definitionName);


    /**
     * @brief Update the definition variants (same name, same version but the lists of I/O/P are differents)
     * @param definitionName
     * @param definitionsList
     */
    void _updateDefinitionVariants(QString definitionName, QList<DefinitionM*> definitionsList);


private:

    // Hash table from a definition name to a list of definitions with this name
    QHash<QString, QList<DefinitionM*>> _hashFromDefinitionNameToDefinitionsList;
};

QML_DECLARE_TYPE(AgentsSupervisionController)

#endif // AGENTSSUPERVISIONCONTROLLER_H
