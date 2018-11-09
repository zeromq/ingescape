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

#ifndef AGENTSGROUPEDBYNAMEVM_H
#define AGENTSGROUPEDBYNAMEVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/agentm.h>
#include <model/publishedvaluem.h>
#include <viewModel/agentsgroupedbydefinitionvm.h>


/**
 * @brief The AgentsGroupedByNameVM class defines a view model of group of agents who have the same name
 * Allows to manage several models of agents who have the same name
 */
class AgentsGroupedByNameVM : public QObject
{
    Q_OBJECT

    // Name of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, name)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Hostname(s) on the network of our agent(s)
    //I2_QML_PROPERTY_READONLY(QString, hostnames)

    // FIXME DEBUG: List of groups of agents, grouped by definition
    I2_QOBJECT_LISTMODEL(AgentsGroupedByDefinitionVM, listOfGroupsByDefinition)


public:
    /**
     * @brief Constructor
     * @param agentName
     * @param parent
     */
    explicit AgentsGroupedByNameVM(QString agentName, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsGroupedByNameVM();


    /**
     * @brief Manage a new model of agent
     * @param model
     */
    void manageNewModel(AgentM* model);


    /**
     * @brief Update the current value of an I/O/P of our agent(s)
     * @param publishedValue
     */
    void updateCurrentValueOfIOP(PublishedValueM* publishedValue);


    /**
     * @brief Delete the view model of agents grouped by definition
     * @param agentsGroupedByDefinition
     */
    void deleteAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


Q_SIGNALS:
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
     * @brief Signal emitted when a model of agent has to be deleted
     * @param model
     */
    void agentModelHasToBeDeleted(AgentM* model);


    /**
     * @brief Signal emitted when a previous model of agent will be replaced by a new one strictly identical
     * @param previousModel
     * @param newModel
     */
    void identicalAgentModelWillBeReplaced(AgentM* previousModel, AgentM* newModel);


public Q_SLOTS:


private Q_SLOTS:

    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot called when the flag "is ON" of a model changed
     * @param isON
     */
    void _onIsONofModelChanged(bool isON);


    /**
     * @brief Slot called when the definition of a model changed (with previous and new values)
     * @param previousDefinition
     * @param newDefinition
     */
    void _onDefinitionOfModelChangedWithPreviousAndNewValues(DefinitionM* previousDefinition, DefinitionM* newDefinition);


    /**
     * @brief Slot called when a view model of agents grouped by definition has become useless (no more model)
     */
    void _onUselessAgentsGroupedByDefinition();


private:

    /**
     * @brief Update with all models of agents
     */
    void _updateWithAllModels();


    /**
     * @brief Update the flag "is ON" in function of flags of models
     */
    void _updateIsON();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // List of peer ids of our models
    QStringList _peerIdsList;

    // View model of agents grouped by definition NULL
    AgentsGroupedByDefinitionVM* _agentsGroupedByDefinitionNULL = nullptr;

    // Hash table from a definition to a (view model of) agents grouped by definition
    QHash<DefinitionM*, AgentsGroupedByDefinitionVM*> _hashFromDefinitionToAgentsGroupedByDefinition;

    // Map from "definition name" to a list (of view models) of agents grouped by definition
    //QHash<QString, QList<AgentsGroupedByDefinitionVM*>> _mapFromDefinitionNameToAgentsGroupedByDefinitionList;

    // Hash table from a hostname to a list of models of agents
    //QHash<QString, QList<AgentM*>> _hashFromHostnameToModels;
};

QML_DECLARE_TYPE(AgentsGroupedByNameVM)

#endif // AGENTSGROUPEDBYNAMEVM_H
