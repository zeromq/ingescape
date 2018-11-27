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
    _outputAgent(nullptr),
    _output(nullptr),
    _input(nullptr)
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
            // DIS-connect to signals from the agents grouped by name
            //disconnect(_outputAgent, &AgentsGroupedByNameVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);
            disconnect(_outputAgent, 0, this, 0);
        }

        // Set the new value
        _outputAgent = value;

        // Reset the output
        setoutput(nullptr);

        // Clear the outputs list
        _outputsList.clear();

        if (_outputAgent != nullptr)
        {
            // Connect to signals from the agents grouped by name
            connect(_outputAgent, &AgentsGroupedByNameVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);
            connect(_outputAgent, &AgentsGroupedByNameVM::outputsHaveBeenAdded, this, &MappingEffectM::_onOutputsHaveBeenAddedToOutputAgent);
            connect(_outputAgent, &AgentsGroupedByNameVM::outputsWillBeRemoved, this, &MappingEffectM::_onOutputsWillBeRemovedToOutputAgent);

            if (!_outputAgent->outputsList()->isEmpty()) {
                _onOutputsHaveBeenAddedToOutputAgent(_outputAgent->outputsList()->toList());
            }

            // By default, select the first item
            if (!_outputsList.isEmpty())
            {
                setoutput(_outputsList.at(0));
            }
        }

        Q_EMIT outputAgentChanged(value);
    }
}


/**
* @brief Custom setter for output
* @param output
*/
/*void MappingEffectM::setoutput(AgentIOPVM* value)
{
    if (_output != value)
    {
        if (_output != nullptr)
        {
            // UnSubscribe to destruction
            disconnect(_output, &AgentIOPVM::destroyed, this, &MappingEffectM::_onOutputDestroyed);
        }

        _output = value;

        if (_output != nullptr)
        {
            setoutputName(_output->name());

            // Subscribe to destruction
            connect(_output, &AgentIOPVM::destroyed, this, &MappingEffectM::_onOutputDestroyed);
        }

        Q_EMIT outputChanged(value);
    }
}*/


/**
* @brief Custom setter for input
* @param input
*/
/*void MappingEffectM::setinput(AgentIOPVM* value)
{
    if (_input != value)
    {
        if (_input != nullptr)
        {
            // UnSubscribe to destruction
            disconnect(_input, &AgentIOPVM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        _input = value;

        if (_input != nullptr)
        {
            setinputName(_input->name());

            // Subscribe to destruction
            connect(_input, &AgentIOPVM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        Q_EMIT inputChanged(value);
    }
}*/


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void MappingEffectM::copyFrom(ActionEffectM *effect)
{
    // Call our mother class
    ActionEffectM::copyFrom(effect);

    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effect);
    if (mappingEffect != nullptr)
    {
        _inputsList.clear();
        _inputsList.append(mappingEffect->inputsList()->toList());

        _outputsList.clear();
        _outputsList.append(mappingEffect->outputsList()->toList());

        setoutputAgent(mappingEffect->outputAgent());
        setoutput(mappingEffect->output());

        // Input Agent is stored in the base class "ActionEffectM"
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
        if (previousAgent != nullptr)
        {
            // DIS-connect to signals from the agents grouped by name
            disconnect(_agent, 0, this, 0);
        }

        // Reset the input
        setinput(nullptr);

        // Clear the inputs list
        _inputsList.clear();

        if (_agent != nullptr)
        {
            // Connect to signals from the agents grouped by name
            connect(_agent, &AgentsGroupedByNameVM::inputsHaveBeenAdded, this, &MappingEffectM::_onInputsHaveBeenAddedToInputAgent);
            connect(_agent, &AgentsGroupedByNameVM::inputsWillBeRemoved, this, &MappingEffectM::_onInputsWillBeRemovedToInputAgent);

            if (!_agent->inputsList()->isEmpty()) {
                _onInputsHaveBeenAddedToInputAgent(_agent->inputsList()->toList());
            }

            // By default, select the first item
            if (!_inputsList.isEmpty())
            {
                setinput(_inputsList.at(0));
            }
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

    //Q_EMIT askForDestruction();
}


/**
 * @brief Slot called when some view models of inputs have been added to the input agent(s grouped by name)
 * @param newInputs
 */
void MappingEffectM::_onInputsHaveBeenAddedToInputAgent(QList<InputVM*> newInputs)
{
    _inputsList.append(newInputs);
}


/**
 * @brief Slot called when some view models of outputs have been added to the output agent(s grouped by name)
 * @param newOutputs
 */
void MappingEffectM::_onOutputsHaveBeenAddedToOutputAgent(QList<OutputVM*> newOutputs)
{
    _outputsList.append(newOutputs);
}


/**
 * @brief Slot called when some view models of inputs will be removed from the input agent(s grouped by name)
 * @param oldInputs
 */
void MappingEffectM::_onInputsWillBeRemovedToInputAgent(QList<InputVM*> oldInputs)
{
    for (InputVM* iterator : oldInputs)
    {
        if ((iterator != nullptr) && _inputsList.contains(iterator))
        {
            // If this input is selected
            if (_input == iterator) {
                setinput(nullptr);
            }

            _inputsList.remove(iterator);
        }
    }
}


/**
 * @brief Slot called when some view models of outputs will be removed from the output agent(s grouped by name)
 * @param oldOutputs
 */
void MappingEffectM::_onOutputsWillBeRemovedToOutputAgent(QList<OutputVM*> oldOutputs)
{
    for (OutputVM* iterator : oldOutputs)
    {
        if ((iterator != nullptr) && _outputsList.contains(iterator))
        {
            // If this output is selected
            if (_output == iterator) {
                setoutput(nullptr);
            }

            _outputsList.remove(iterator);
        }
    }
}


/**
 * @brief Called when our input is destroyed
 * @param sender
 */
/*void MappingEffectM::_onInputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setinput(nullptr);
}*/


/**
 * @brief Called when our output is destroyed
 * @param sender
 */
/*void MappingEffectM::_onOutputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutput(nullptr);
}*/

