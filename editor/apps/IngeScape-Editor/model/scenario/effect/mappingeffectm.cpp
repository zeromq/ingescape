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
 * @brief Default constructor
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
    setoutputAgent(NULL);

    // Reset output
    setoutput(NULL);

    // Reset input
    setinput(NULL);
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void MappingEffectM::copyFrom(ActionEffectM *effect)
{
    // Call mother class
    ActionEffectM::copyFrom(effect);

    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effect);
    if(mappingEffect != NULL)
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
* @brief Custom setter on set agent to fill inputs and outputs
* @param agent
*/
void MappingEffectM::setagent(AgentInMappingVM* agent)
{
    AgentInMappingVM* previousAgent = _agent;

    // Call setter of mother class
    ActionEffectM::setagent(agent);

    if (previousAgent != _agent)
    {
        if(_agent != NULL)
        {
            disconnect(_agent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &MappingEffectM::_onAgentIOPInputsOutputsListChange);
        }

        // Clear the inputs list and the selected input
        _inputsList.clear();
        setinput(NULL);

        if (_agent != NULL)
        {
            // Fill inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if ((input != NULL) && (input->firstModel() != NULL))
                {
                    _inputsList.append(input->firstModel());
                }
            }

            // Select the first item
            if (_inputsList.count() > 0) {
                setinput(_inputsList.at(0));
            }

            connect(_agent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &MappingEffectM::_onAgentIOPInputsOutputsListChange);
        }
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentInMappingVM*, QStringList> MappingEffectM::getAgentAndCommandWithParameters()
{
    QPair<AgentInMappingVM*, QStringList> pairAgentAndCommandWithParameters;

    if ((_agent != NULL) && (_input != NULL) && (_outputAgent != NULL) && (_output != NULL))
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        switch (_mappingEffectValue)
        {
        case MappingEffectValues::MAPPED: {
            commandAndParameters << "MAP";
            break;
        }
        case MappingEffectValues::UNMAPPED: {
            commandAndParameters << "UNMAP";
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

    if ((_agent != NULL) && (_input != NULL) && (_outputAgent != NULL) && (_output != NULL))
    {
        pairAgentNameAndReverseCommand.first = _agent->name();

        QStringList reverseCommandAndParameters;

        switch (_mappingEffectValue)
        {
        case MappingEffectValues::MAPPED: {
            reverseCommandAndParameters << "UNMAP";
            break;
        }
        case MappingEffectValues::UNMAPPED: {
            reverseCommandAndParameters << "MAP";
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
* @brief Custom setter for property "output agent" to fill inputs and outputs
* @param value
*/
void MappingEffectM::setoutputAgent(AgentInMappingVM* value)
{
    if (_outputAgent != value)
    {
        if (_outputAgent != NULL)
        {
            // UN-subscribe to destruction
            disconnect(_outputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);

            // Disconnect to output agent change of inputs/ouputs iop
            disconnect(_outputAgent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &MappingEffectM::_onOutputAgentIOPInputsOutputsListChange);
        }

        // set the new value
        _outputAgent = value;

        // Clear the list and the selected output
        _outputsList.clear();
        setoutput(NULL);

        if (_outputAgent != NULL)
        {
            // Subscribe to destruction
            connect(_outputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);

            foreach (OutputVM* output, _outputAgent->outputsList()->toList())
            {
                if ((output != NULL) && (output->firstModel() != NULL)) {
                    _outputsList.append(output->firstModel());
                }
            }

            // Select the first item
            if (_outputsList.count() > 0) {
                setoutput(_outputsList.at(0));
            }

            // Connect to output agent change of inputs/ouputs iop
            connect(_outputAgent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &MappingEffectM::_onOutputAgentIOPInputsOutputsListChange);
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
    if(_output != value)
    {
        if(_output != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_output, &AgentIOPM::destroyed, this, &MappingEffectM::_onOutputDestroyed);
        }

        _output = value;

        if(_output != NULL)
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
    if(_input != value)
    {
        if(_input != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_input, &AgentIOPM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        _input = value;

        if(_input != NULL)
        {
            setinputName(_input->name());

            // Subscribe to destruction
            connect(_input, &AgentIOPM::destroyed, this, &MappingEffectM::_onInputDestroyed);
        }

        Q_EMIT inputChanged(value);
    }
}

/**
 * @brief Called when our input is destroyed
 * @param sender
 */
void MappingEffectM::_onInputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setinput(NULL);
}

/**
 * @brief Called when our output is destroyed
 * @param sender
 */
void MappingEffectM::_onOutputDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutput(NULL);
}

/**
 * @brief Called when our "output agent" is destroyed
 * @param sender
 */
void MappingEffectM::_onOutputAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutputAgent(NULL);
}

/**
  * @brief Slot on agent inputs/outputs lists change
  */
void MappingEffectM::_onAgentIOPInputsOutputsListChange()
{
    // If we have a selected agent iop
    if (!_inputName.isEmpty())
    {
        _inputsList.clear();
        // Check that our input list update concern our selected agent iop
        foreach (InputVM* inputVM, _agent->inputsList()->toList())
        {
            if (inputVM->firstModel() != NULL)
            {
                _inputsList.append(inputVM->firstModel());
                if (inputVM->name() == _inputName)
                {
                    updateInputSelected();
                }
            }
        }
    }
}


/**
  * @brief Slot on output agent inputs/outputs lists change
  */
void MappingEffectM::_onOutputAgentIOPInputsOutputsListChange()
{
    // If we have a selected agent iop
    if (!_outputName.isEmpty())
    {
        _outputsList.clear();

        // Check that our input list update concern our selected agent iop
        foreach (OutputVM* outputVM, _agent->outputsList()->toList())
        {
            if(outputVM->firstModel() != NULL)
            {
                _outputsList.append(outputVM->firstModel());
                if(outputVM->name() == _outputName)
                {
                    updateOutputSelected();
                }
            }
        }
    }
}

/**
* @brief Update the selected input
*/
void MappingEffectM::updateInputSelected()
{
    if ((_agent != NULL) && !_inputName.isEmpty())
    {
        AgentIOPM* newAgentIOP = NULL;

        // Fill with outputs
        /*foreach (OutputVM* output, _agent->outputsList()->toList())
        {
            if (output->firstModel() != NULL)
            {
                if ((newAgentIOP == NULL) && (_inputName == output->firstModel()->name())) {
                    newAgentIOP = output->firstModel();
                    break;
                }
            }
        }*/

        // Fill with inputs
        foreach (InputVM* input, _agent->inputsList()->toList())
        {
            if(input->firstModel() != NULL)
            {
                if ((newAgentIOP == NULL) && (_inputName == input->firstModel()->name())) {
                    newAgentIOP = input->firstModel();
                    break;
                }
            }
        }

        // Change the input
        if (newAgentIOP != _input)
        {
            // Set the new agent
            setinput(newAgentIOP);
        }
    }
}


/**
* @brief Update the selected output
*/
void MappingEffectM::updateOutputSelected()
{
    if ((_agent != NULL) && !_outputName.isEmpty())
    {
        AgentIOPM* newAgentIOP = NULL;

        // Fill with outputs
        foreach (OutputVM* output, _agent->outputsList()->toList())
        {
            if (output->firstModel() != NULL)
            {
                if ((newAgentIOP == NULL) && (_outputName == output->firstModel()->name())) {
                    newAgentIOP = output->firstModel();
                    break;
                }
            }
        }

        // Fill with inputs
        /*foreach (InputVM* input, _agent->inputsList()->toList())
        {
            if(input->firstModel() != NULL)
            {
                if ((newAgentIOP == NULL) && (_outputName == input->firstModel()->name())) {
                    newAgentIOP = input->firstModel();
                }
            }
        }*/

        // Change the input
        if (newAgentIOP != _output)
        {
            // Set the new agent
            setoutput(newAgentIOP);
        }
    }
}

