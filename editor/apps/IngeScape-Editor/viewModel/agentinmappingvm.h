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

#ifndef AGENTINMAPPINGVM_H
#define AGENTINMAPPINGVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <viewModel/agentsgroupedbynamevm.h>
//#include <model/agentm.h>
//#include <viewModel/iop/inputvm.h>
//#include <viewModel/iop/outputvm.h>
//#include <viewModel/iop/parametervm.h>
#include <viewModel/link/linkinputvm.h>
#include <viewModel/link/linkoutputvm.h>


/**
 * @brief The AgentInMappingVM class a view model of agent in the global mapping
 * Allows to manage agents and links graphically
 */
class AgentInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, name)

    // Agents grouped by name
    I2_QML_PROPERTY_READONLY(AgentsGroupedByNameVM*, agentsGroupedByName)

    // List of view models of link inputs
    I2_QOBJECT_LISTMODEL(LinkInputVM, linkInputsList)

    // List of view models of link outputs
    I2_QOBJECT_LISTMODEL(LinkOutputVM, linkOutputsList)

    // The position corresponds to the corner Top-Left of the box
    I2_QML_PROPERTY(QPointF, position)

    // Flag indicating if our agent is reduced
    I2_QML_PROPERTY(bool, isReduced)

    // Group of value type of the reduced map (= brin) in input of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedMapValueTypeGroupInInput)

    // Group of value type of the reduced map (= brin) in output of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedMapValueTypeGroupInOutput)

    // Mapping currently edited (temporary until the user activate the mapping)
    I2_CPP_NOSIGNAL_PROPERTY(AgentMappingM*, temporaryMapping)


public:
    /**
     * @brief Constructor
     * @param agentsGroupedByName Models of agents grouped by the same name
     * @param position Position of the top left corner
     * @param parent
     */
    explicit AgentInMappingVM(AgentsGroupedByNameVM* agentsGroupedByName,
                              QPointF position,
                              QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();


    /**
     * @brief Add a temporary link (this temporary link will became a real link when the user will activate the mapping)
     * @param inputName
     * @param outputAgentName
     * @param outputName
     * @return true if the link has been added
     */
    bool addTemporaryLink(QString inputName, QString outputAgentName, QString outputName);


    /**
     * @brief Remove temporary link (this temporary link will be removed when the user will activate the mapping)
     * @param inputName
     * @param outputAgentName
     * @param outputName
     * @return true if the link has been removed
     */
    bool removeTemporaryLink(QString inputName, QString outputAgentName, QString outputName);


Q_SIGNALS:

    /**
     * @brief Signal emitted when some view models of inputs have been added to our agent in mapping
     * @param inputsListHaveBeenAdded
     */
    void inputsListHaveBeenAdded(QList<InputVM*> inputsListHaveBeenAdded);


    /**
     * @brief Signal emitted when some view models of outputs have been added to our agent in mapping
     * @param outputsListHaveBeenAdded
     */
    void outputsListHaveBeenAdded(QList<OutputVM*> outputsListHaveBeenAdded);


    /**
     * @brief Signal emitted when some view models of inputs will be removed from our agent in mapping
     * @param inputsListWillBeRemoved
     */
    void inputsListWillBeRemoved(QList<InputVM*> inputsListWillBeRemoved);


    /**
     * @brief Signal emitted when some view models of outputs will be removed from our agent in mapping
     * @param outputsListWillBeRemoved
     */
    void outputsListWillBeRemoved(QList<OutputVM*> outputsListWillBeRemoved);


    /**
     * @brief Signal emitted when the models of Inputs/Outputs/Parameters changed
     */
    void modelsOfIOPChanged();


private Q_SLOTS:

    /**
     * @brief Slot when the list of models changed
     */
    //void _onModelsChanged();


    /**
     * @brief Slot called when the definition of a model changed (with previous and new values)
     * @param previousValue
     * @param newValue
     */
    void _onDefinitionOfModelChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue);


private:

    /**
     * @brief A model of agent has been added to our list
     * @param model
     */
    //void _agentModelAdded(AgentM* model);


    /**
     * @brief A model of agent has been removed from our list
     * @param model
     */
    //void _agentModelRemoved(AgentM* model);


    /**
     * @brief A model of input has been added
     * @param input
     * @return
     */
    //InputVM* _inputModelAdded(AgentIOPM* input);


    /**
     * @brief A model of input has been removed
     * @param input
     * @return
     */
    //InputVM* _inputModelRemoved(AgentIOPM* input);


    /**
     * @brief A model of output has been added
     * @param output
     * @return
     */
    //OutputVM* _outputModelAdded(OutputM* output);


    /**
     * @brief A model of output has been removed
     * @param output
     * @return
     */
    //OutputVM* _outputModelRemoved(OutputM* output);



    /**
     * @brief A model of parameter has been added
     * @param parameter
     * @return
     */
    //ParameterVM* _parameterModelAdded(AgentIOPM* parameter);


    /**
     * @brief A model of parameter has been removed
     * @param parameter
     * @return
     */
    //ParameterVM* _parameterModelRemoved(AgentIOPM* parameter);


    /**
     * @brief Update the flag "Is Defined in All Definitions" for each Input/Output/Parameter
     */
    //void _updateIsDefinedInAllDefinitionsForEachIOP();


    /**
     * @brief Update the group (of value type) of the reduced map (= brin) in input of our agent
     */
    void _updateReducedMapValueTypeGroupInInput();


    /**
     * @brief Update the group (of value type) of the reduced map (= brin) in output of our agent
     */
    void _updateReducedMapValueTypeGroupInOutput();


    /**
     * @brief Get the temporary link with same names
     * @param inputName
     * @param outputAgentName
     * @param outputName
     * @return
     */
    ElementMappingM* _getTemporaryLink(QString inputName, QString outputAgentName, QString outputName);


private:

};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
