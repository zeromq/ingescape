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

#include <viewModel/iop/inputvm.h>
#include <viewModel/iop/outputvm.h>
#include <viewModel/iop/parametervm.h>


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

    // List of peer ids of models
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, peerIdsList)

    // Flag indicating if our agent(s) is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Number of agents which are ON
    I2_QML_PROPERTY_READONLY(int, numberOfAgentsON)

    // Number of agents which are OFF
    I2_QML_PROPERTY_READONLY(int, numberOfAgentsOFF)

    // List of view models of inputs
    I2_QOBJECT_LISTMODEL(InputVM, inputsList)

    // List of view models of outputs
    I2_QOBJECT_LISTMODEL(OutputVM, outputsList)

    // List of view models of parameters
    I2_QOBJECT_LISTMODEL(ParameterVM, parametersList)

    // FIXME DEBUG: List of groups of agents, grouped by definition
    I2_QOBJECT_LISTMODEL(AgentsGroupedByDefinitionVM, listOfGroupsByDefinition)

    // Mapping currently edited (temporary until the user activate the mapping)
    //I2_CPP_NOSIGNAL_PROPERTY(AgentMappingM*, currentMapping)


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
     * And emit the signal "agentModelHasToBeDeleted" for each of its model of agent
     * @param agentsGroupedByDefinition
     */
    void deleteAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Get the list of definitions with a specific name
     * @param definitionName
     * @return
     */
    QList<DefinitionM*> getDefinitionsWithName(QString definitionName);


    /**
     * @brief Open the definition(s)
     * If there are several "Agents Grouped by Definition", we have to open each definition
     */
    Q_INVOKABLE void openDefinition();


    /**
     * @brief Return the list of view models of input from an input name
     * @param inputName
     */
    QList<InputVM*> getInputsListFromName(QString inputName);


    /**
     * @brief Return the view model of input from an input id
     * @param inputId
     */
    InputVM* getInputFromId(QString inputId);


    /**
     * @brief Return the list of view models of output from an output name
     * @param outputName
     */
    QList<OutputVM*> getOutputsListFromName(QString outputName);


    /**
     * @brief Return the view model of output from an output id
     * @param outputId
     */
    OutputVM* getOutputFromId(QString outputId);


    /**
     * @brief Return the list of view models of parameter from a parameter name
     * @param parameterName
     */
    QList<ParameterVM*> getParametersListFromName(QString parameterName);


    /**
     * @brief Return the view model of parameter from a parameter id
     * @param parameterId
     */
    ParameterVM* getParameterFromId(QString parameterId);


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
     * @brief FIXME Usefull ? Signal emitted when a previous model of agent will be replaced by a new one strictly identical
     * @param previousModel
     * @param newModel
     */
    //void identicalAgentModelWillBeReplaced(AgentM* previousModel, AgentM* newModel);


    /**
     * @brief Signal emitted when the definition(s) of our agent must be opened
     * @param definitionsList
     */
    void definitionsToOpen(QList<DefinitionM*> definitionsList);


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
     * @brief Check if we have to merge the model of agent with an existing view model of agents (grouped by definition) that have the same definition
     * @param model
     */
    void _checkHaveToMergeAgent(AgentM* model);


    /**
     * @brief Save a new view model of agents grouped by definition
     * @param agentsGroupedByDefinition
     */
    void _saveNewAgentsGroupedByDefinition(AgentsGroupedByDefinitionVM* agentsGroupedByDefinition);


    /**
     * @brief Manage the list of inputs of the new definition
     * @param definition
     */
    void _manageInputsOfNewDefinition(DefinitionM* definition);


    /**
     * @brief Manage the list of outputs of the new definition
     * @param definition
     */
    void _manageOutputsOfNewDefinition(DefinitionM* definition);


    /**
     * @brief Manage the list of parameters of the new definition
     * @param definition
     */
    void _manageParametersOfNewDefinition(DefinitionM* definition);


    /**
     * @brief Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
     * @param numberOfDefinitions
     */
    void _updateIsDefinedInAllDefinitionsForEachIOP(int numberOfDefinitions);


    /**
     * @brief Manage a new model of input
     * @param input
     * @return
     */
    InputVM* _manageNewInputModel(AgentIOPM* input);


    /**
     * @brief Manage a new model of output
     * @param output
     * @return
     */
    OutputVM* _manageNewOutputModel(OutputM* output);


    /**
     * @brief Manage a new model of parameter
     * @param parameter
     * @return
     */
    ParameterVM* _manageNewParameterModel(AgentIOPM* parameter);



private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // View model of agents grouped by definition NULL
    AgentsGroupedByDefinitionVM* _agentsGroupedByDefinitionNULL = nullptr;

    // Hash table from a definition to a (view model of) agents grouped by definition
    QHash<DefinitionM*, AgentsGroupedByDefinitionVM*> _hashFromDefinitionToAgentsGroupedByDefinition;

    // Input name as key is not unique (value type can be different)
    // Hash table from an input name to a list of view models of inputs
    QHash<QString, QList<InputVM*>> _hashFromNameToInputsList;

    // Hash table from a (unique) input id to a view model of input
    QHash<QString, InputVM*> _hashFromIdToInput;

    // Output name as key is not unique (value type can be different)
    // Hash table from an output name to a list of view models of outputs
    QHash<QString, QList<OutputVM*>> _hashFromNameToOutputsList;

    // Hash table from a (unique) output id to a view model of output
    QHash<QString, OutputVM*> _hashFromIdToOutput;

    // Parameter name as key is not unique (value type can be different)
    // Hash table from a parameter name to a list of view models of parameters
    QHash<QString, QList<ParameterVM*>> _hashFromNameToParametersList;

    // Hash table from a (unique) parameter id to a view model of parameter
    QHash<QString, ParameterVM*> _hashFromIdToParameter;
};

QML_DECLARE_TYPE(AgentsGroupedByNameVM)

#endif // AGENTSGROUPEDBYNAMEVM_H
