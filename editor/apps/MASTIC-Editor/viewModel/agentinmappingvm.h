/*
 *	MASTIC Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
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

#include <viewModel/iop/inputvm.h>
#include <viewModel/iop/outputvm.h>

#include <model/agentmappingm.h>
#include <model/agentm.h>

/**
 * @brief The AgentInMappingVM class a view model of agent in the mapping
 * Allows to manage when several agents have the same name
 */
class AgentInMappingVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(InputVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(OutputVM, outputsList)

    // The position corresponds to the corner Top-Left of the box
    I2_QML_PROPERTY(QPointF, position)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is reduced
    I2_QML_PROPERTY(bool, isReduced)

    // Define the value type of the reduced map (= brin) in input of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInInput)

    // Define the value type of the reduced map (= brin) in output of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, reducedMapValueTypeInOutput)

    // Flag indicating if our agent is a ghost agent
    I2_QML_PROPERTY_READONLY(bool, isGhost)

    // Flag indicating if all definitions are strictly identicals
    I2_QML_PROPERTY_READONLY(bool, areIdenticalsAllDefinitions)


public:
    /**
     * @brief Default constructor
     * @param models The first agent is needed to instanciate an agent mapping VM.
     * Typically passing during the drag-drop from the list of agents on the left side.
     * @param position Position of the top left corner
     * @param parent
     */
    explicit AgentInMappingVM(QList<AgentM*> models,
                              QPointF position,
                              QObject* parent = nullptr);


    /**
     * @brief Ghost Constructor: model (and definition) is not defined.
     * The agent is an empty shell only defined by a name.
     * @param agentName
     * @param parent
     */
    explicit AgentInMappingVM(QString agentName,
                              QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a definition is added to the agent mapping
     * @param agentInMapping
     */
    void newDefinitionInAgentMapping(AgentInMappingVM* agentInMapping);


public Q_SLOTS:

    /**
     * @brief Return the corresponding view model of input from the input name
     * @param inputName
     */
    InputVM* getInputFromName(QString inputName);


    /**
     * @brief Return the corresponding view model of input from the input id
     * @param inputId
     */
    InputVM* getInputFromId(QString inputId);


    /**
     * @brief Return the corresponding view model of output from the output name
     * @param outputName
     */
    OutputVM* getOutputFromName(QString outputName);


    /**
     * @brief Return the corresponding view model of output from the output id
     * @param outputId
     */
    OutputVM* getOutputFromId(QString outputId);


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot when the flag "is ON" of a model changed
     * @param isON
     */
    void _onIsONofModelChanged(bool isON);


private:

    /**
     * @brief A model of agent has been added to our list
     * @param model
     */
    void _agentModelAdded(AgentM* model);


    /**
     * @brief A model of agent has been removed from our list
     * @param model
     */
    void _agentModelRemoved(AgentM* model);


    /**
     * @brief A model of input has been added
     * @param input
     * @return
     */
    InputVM* _inputModelAdded(AgentIOPM* input);


    /**
     * @brief A model of output has been added
     * @param output
     * @return
     */
    OutputVM* _outputModelAdded(OutputM* output);


    /**
     * @brief Add new points Map to the inputs list from a definition model
     * @param newDefinition The definition model
     */
    //void _addPointMapInInternalInputList(DefinitionM *newDefinition);


    /**
     * @brief Add new points Map to the outputs list from a definition model
     * @param newDefinition The definition model
     */
    //void _addPointMapInInternalOutputList(DefinitionM *newDefinition);


    /**
     * @brief This function check if the OutputVM already exist in the input list
     * @param currentOuput The newly created OutputVM
     */
    //bool _checkIfAlreadyInOutputList(OutputVM* currentOuput);


    /**
     * @brief This function check if the InputVM already exist in the input list
     * @param currentInput The newly created Input VM
     */
    //bool _checkIfAlreadyInInputList(InputVM* currentInput);


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

    // TODO
    QHash<QString, InputVM*> _mapOfInputsFromInputName;
    QHash<QString, InputVM*> _mapOfInputsFromInputId;

    // TODO
    QHash<QString, OutputVM*> _mapOfOutputsFromOutputName;
    QHash<QString, OutputVM*> _mapOfOutputsFromOutputId;

};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
