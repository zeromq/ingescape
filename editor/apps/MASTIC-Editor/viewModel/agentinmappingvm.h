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
    I2_QML_PROPERTY_READONLY(QString, name)

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

    // Group of value type of the reduced map (= brin) in input of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedMapValueTypeGroupInInput)

    // Group of value type of the reduced map (= brin) in output of the agent
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, reducedMapValueTypeGroupInOutput)

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
     * @param name
     * @param parent
     */
    explicit AgentInMappingVM(QString name,
                              QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentInMappingVM();


Q_SIGNALS:

    /**
     * @brief Signal emitted when some view models of inputs have been added to our agent in mapping
     * @param inputsListAdded
     */
    void inputsListAdded(QList<InputVM*> inputsListAdded);


    /**
     * @brief Signal emitted when some view models of outputs have been added to our agent in mapping
     * @param outputsListAdded
     */
    void outputsListAdded(QList<OutputVM*> outputsListAdded);


    /**
     * @brief Signal emitted when some view models of outputs will be removed from our agent in mapping
     * @param outputsListWillBeRemoved
     */
    void outputsListRemoved(QList<OutputVM*> outputsListWillBeRemoved);


public Q_SLOTS:

    /**
     * @brief Return the list of view models of input from the input name
     * @param inputName
     */
    QList<InputVM*> getInputsListFromName(QString inputName);


    /**
     * @brief Return the view model of input from the input id
     * @param inputId
     */
    InputVM* getInputFromId(QString inputId);


    /**
     * @brief Return the list of view models of output from the output name
     * @param outputName
     */
    QList<OutputVM*> getOutputsListFromName(QString outputName);


    /**
     * @brief Return the view model of output from the output id
     * @param outputId
     */
    OutputVM* getOutputFromId(QString outputId);


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot when the list of (view models of) inputs changed
     */
    //void _onInputsListChanged();


    /**
     * @brief Slot when the list of (view models of) outputs changed
     */
    //void _onOutputsListChanged();


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
     * @return a view model of input only if it is a new one
     */
    InputVM* _inputModelAdded(AgentIOPM* input);


    /**
     * @brief A model of input has been removed
     * @param input
     * @return
     */
    InputVM* _inputModelRemoved(AgentIOPM* input);


    /**
     * @brief A model of output has been added
     * @param output
     * @return a view model of output only if it is a new one
     */
    OutputVM* _outputModelAdded(OutputM* output);


    /**
     * @brief A model of output has been removed
     * @param output
     * @return
     */
    OutputVM* _outputModelRemoved(OutputM* output);


    /**
     * @brief Update with all models of agents
     */
    void _updateWithAllModels();


    /**
     * @brief Update the flag "is ON" in function of flags of models
     */
    void _updateIsON();


    /**
     * @brief Update the group (of value type) of the reduced map (= brin) in input of our agent
     */
    void _updateReducedMapValueTypeGroupInInput();


    /**
     * @brief Update the group (of value type) of the reduced map (= brin) in output of our agent
     */
    void _updateReducedMapValueTypeGroupInOutput();


private:

    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // Input name as key is not unique (value type can be different)
    // Map from an input name to a list of view models of inputs
    QHash<QString, QList<InputVM*>> _mapFromNameToInputsList;

    // Map from a (unique) input id to a view model of input
    QHash<QString, InputVM*> _mapFromUniqueIdToInput;

    // Output name as key is not unique (value type can be different)
    // Map from an output name to a list of view models of outputs
    QHash<QString, QList<OutputVM*>> _mapFromNameToOutputsList;

    // Map from a (unique) output id to a view model of output
    QHash<QString, OutputVM*> _mapFromUniqueIdToOutput;

};

QML_DECLARE_TYPE(AgentInMappingVM)

#endif // AGENTINMAPPINGVM_H
