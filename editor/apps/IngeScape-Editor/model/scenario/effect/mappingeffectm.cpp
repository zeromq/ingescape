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
 *
 */

#include "mappingeffectm.h"


#include <QDebug>

/**
 * @brief Enum "MappingEffectValues" to string
 * @param value
 * @return
 */
QString MappingEffectValues::enumToString(int value)
{
    switch (value)
    {
    case MappingEffectValues::MAPPED:
        return tr("MAPPED");

    case MappingEffectValues::UNMAPPED:
        return tr("UNMAPPED");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// MappingEffectM
//
//--------------------------------------------------------------


/**
 * @brief Constructor
 * @param parent
 */
MappingEffectM::MappingEffectM(QObject *parent) : ActionEffectM(parent),
    _mappingEffectValue(MappingEffectValues::MAPPED),
    _outputAgent(NULL),
    _output(NULL),
    _outputName(""),
    _input(NULL),
    _inputName("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
MappingEffectM::~MappingEffectM()
{
    // Clear our lists
    _inputsList.clear();
    _outputsList.clear();

    // Reset output agent
    setoutputAgent(nullptr);

    // Reset output
    setoutput(nullptr);

    // Reset input
    setinput(nullptr);
}


/**
* @brief Custom setter for property "output agent"
* @param value
*/
void MappingEffectM::setoutputAgent(AgentsGroupedByNameVM* value)
{
    // Value of (output) agent changed
    if (_outputAgent != value)
    {
        if (_outputAgent != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_outputAgent, &AgentsGroupedByNameVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);

            disconnect(_outputAgent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &MappingEffectM::_onModelsOfIOPofOutputAgentChanged);
        }

        // set the new value
        _outputAgent = value;

        // Reset the output
        setoutput(nullptr);

        // Clear the list and the selected output
        _outputsList.clear();

        if (_outputAgent != nullptr)
        {
            // Subscribe to destruction
            connect(_outputAgent, &AgentsGroupedByNameVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);

            // Fill outputs
            foreach (OutputVM* output, _outputAgent->outputsList()->toList())
            {
                if ((output != nullptr) && (output->firstModel() != nullptr)) {
                    _outputsList.append(output->firstModel());
                }
            }

            // Select the first item
            if (!_outputsList.isEmpty()) {
                setoutput(_outputsList.at(0));
            }

            connect(_outputAgent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &MappingEffectM::_onModelsOfIOPofOutputAgentChanged);
        }

        Q_EMIT outputAgentChanged(value);
    }
}


/**
* @brief Custom setter for output
* @param output
*/
void MappingEffectM::setoutput(AgentIOPM* value)
{
    if (_output != value)
    {
        if (_output != nullptr)
        {
            // UnSubscribe to destruction
            disconnect(_output, &AgentIOPM::destroyed, this, &MappingEffectM::_onOutputDestroyed);
        }

        _output = value;

        if (_output != nullptr)
        {
            setoutputName(_output->name());

            // Subscribe to destruction
            connect(_output, &AgentIOPM::destroyed, this, &MappingEffectM::_onOutputDestroyed);
        }

        Q_EMIT outputChanged(value);
    }
}


/**
* @brief Custom setter for input
* @param input
*/
void MappingEffectM::setinput(AgentIOPM* value)
{
    if (_input != value)
    {
        if(_input != nullptr)
        {
            // UnSubscribe to destruction
            disconnect(_input, &AgentIOPM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        _input = value;

        if(_input != nullptr)
        {
            setinputName(_input->name());

            // Subscribe to destruction
            connect(_input, &AgentIOPM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        Q_EMIT inputChanged(value);
    }
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void MappingEffectM::copyFrom(ActionEffectM *effect)
{
    // Call our mother class
    ActionEffectM::copyFrom(effect);

    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effect);
    if(mappingEffect != nullptr)
    {
        _inputsList.clear();
        _inputsList.append(mappingEffect->inputsList()->toList());
        _outputsList.clear();
        _outputsList.append(mappingEffect->outputsList()->toList());

        setoutputAgent(mappingEffect->outputAgent());
        setoutput(mappingEffect->output());
        setinput(mappingEffect->input());

        setmappingEffectValue(mappingEffect->mappingEffectValue());
    }
}


/**
* @brief Setter for property "Agent"
* @param agent
*/
void MappingEffectM::setagent(AgentsGroupedByNameVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentsGroupedByNameVM* previousAgent = _agent;

    // Call setter of mother class
    ActionEffectM::setagent(agent);

    // Value of (input) agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != nullptr) {
            disconnect(previousAgent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &MappingEffectM::_onModelsOfIOPofInputAgentChanged);
        }

        // Reset the input
        setinput(nullptr);

        // Clear the inputs list
        _inputsList.clear();

        if (_agent != nullptr)
        {
            // Fill inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if ((input != nullptr) && (input->firstModel() != nullptr)) {
                    _inputsList.append(input->firstModel());
                }
            }

            // Select the first item
            if (!_inputsList.isEmpty()) {
                setinput(_inputsList.at(0));
            }

            connect(_agent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &MappingEffectM::_onModelsOfIOPofInputAgentChanged);
        }
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentsGroupedByNameVM*, QStringList> MappingEffectM::getAgentAndCommandWithParameters()
{
    QPair<AgentsGroupedByNameVM*, QStringList> pairAgentAndCommandWithParameters;

    if ((_agent != nullptr) && (_input != nullptr) && (_outputAgent != nullptr) && (_output != nullptr))
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        switch (_mappingEffectValue)
        {
        case MappingEffectValues::MAPPED: {
            commandAndParameters << command_MapAgents;
            break;
        }
        case MappingEffectValues::UNMAPPED: {
            commandAndParameters << command_UnmapAgents;
            break;
        }
        default:
            break;
        }

        commandAndParameters << _input->name() << _outputAgent->name() << _output->name();

        pairAgentAndCommandWithParameters.second = commandAndParameters;
    }

    return pairAgentAndCommandWithParameters;
}


/**
 * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
 * @return
 */
QPair<QString, QStringList> MappingEffectM::getAgentNameAndReverseCommandWithParameters()
{
    QPair<QString, QStringList> pairAgentNameAndReverseCommand;

    if ((_agent != nullptr) && (_input != nullptr) && (_outputAgent != nullptr) && (_output != nullptr))
    {
        pairAgentNameAndReverseCommand.first = _agent->name();

        QStringList reverseCommandAndParameters;

        switch (_mappingEffectValue)
        {
        case MappingEffectValues::MAPPED: {
            reverseCommandAndParameters << command_UnmapAgents;
            break;
        }
        case MappingEffectValues::UNMAPPED: {
            reverseCommandAndParameters << command_MapAgents;
            break;
        }
        default:
            break;
        }

        reverseCommandAndParameters << _input->name() << _outputAgent->name() << _output->name();

        pairAgentNameAndReverseCommand.second = reverseCommandAndParameters;
    }

    return pairAgentNameAndReverseCommand;
}


/**
 * @brief Called when our "output agent" is destroyed
 * @param sender
 */
void MappingEffectM::_onOutputAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutputAgent(nullptr);
}


/**
 * @brief Called when our input is destroyed
 * @param sender
 */
void MappingEffectM::_onInputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setinput(nullptr);
}

/**
 * @brief Called when our output is destroyed
 * @param sender
 */
void MappingEffectM::_onOutputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutput(nullptr);
}


/**
  * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the "Input agent (in mapping)"
  */
void MappingEffectM::_onModelsOfIOPofInputAgentChanged()
{
    // If we have a selected agent IOP
    if ((_agent != nullptr) && !_inputName.isEmpty())
    {
        _inputsList.clear();

        foreach (InputVM* inputVM, _agent->inputsList()->toList())
        {
            if ((inputVM != nullptr) && inputVM->firstModel() != nullptr)
            {
                _inputsList.append(inputVM->firstModel());

                // Check that our inputs list update concern our selected agent IOP
                if (inputVM->name() == _inputName)
                {
                    _updateInputSelected();
                }
            }
        }
    }
}


/**
  * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the "Output agent (in mapping)"
  */
void MappingEffectM::_onModelsOfIOPofOutputAgentChanged()
{
    // If we have a selected agent IOP
    if ((_outputAgent != nullptr) && !_outputName.isEmpty())
    {
        _outputsList.clear();

        foreach (OutputVM* outputVM, _outputAgent->outputsList()->toList())
        {
            if ((outputVM != nullptr) && outputVM->firstModel() != nullptr)
            {
                _outputsList.append(outputVM->firstModel());

                // Check that our outputs list update concern our selected agent IOP
                if (outputVM->name() == _outputName) {
                    _updateOutputSelected();
                }
            }
        }
    }
}


/**
* @brief Update the selected input
*/
void MappingEffectM::_updateInputSelected()
{
    if ((_agent != nullptr) && !_inputName.isEmpty())
    {
        AgentIOPM* newAgentIOP = NULL;

        foreach (InputVM* input, _agent->inputsList()->toList())
        {
            if ((input != nullptr) && (input->firstModel() != nullptr))
            {
                if (_inputName == input->firstModel()->name()) {
                    newAgentIOP = input->firstModel();
                    break;
                }
            }
        }

        // Change the Input
        if (newAgentIOP != _input) {
            setinput(newAgentIOP);
        }
    }
}


/**
* @brief Update the selected output
*/
void MappingEffectM::_updateOutputSelected()
{
    if ((_outputAgent != nullptr) && !_outputName.isEmpty())
    {
        AgentIOPM* newAgentIOP = NULL;

        foreach (OutputVM* output, _outputAgent->outputsList()->toList())
        {
            if ((output != nullptr) && (output->firstModel() != nullptr))
            {
                if (_outputName == output->firstModel()->name()) {
                    newAgentIOP = output->firstModel();
                    break;
                }
            }
        }

        // Change the Output
        if (newAgentIOP != _output) {
            setoutput(newAgentIOP);
        }
    }
}

