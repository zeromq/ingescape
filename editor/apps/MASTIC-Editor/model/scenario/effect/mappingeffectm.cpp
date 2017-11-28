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

#include "mappingeffectm.h"


#include <QDebug>

/**
 * @brief Enum "MappingEffectValues" to string
 * @param value
 * @return
 */
QString MappingEffectValues::enumToString(int value)
{
    QString string = "Mapping Effect Value";

    switch (value)
    {
    case MappingEffectValues::MAPPED:
        string = "MAPPED";
        break;

    case MappingEffectValues::UNMAPPED:
        string = "UNMAPPED";
        break;

    default:
        break;
    }

    return string;
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
            disconnect(_agent, &AgentInMappingVM::inputsListWillBeRemoved, this, &MappingEffectM::_onAgentIOPInputsListChange);
            disconnect(_agent, &AgentInMappingVM::inputsListAdded, this, &MappingEffectM::_onAgentIOPInputsListChange);
            disconnect(_agent, &AgentInMappingVM::outputsListWillBeRemoved, this, &MappingEffectM::_onAgentIOPOutputsListChange);
            disconnect(_agent, &AgentInMappingVM::outputsListAdded, this, &MappingEffectM::_onAgentIOPOutputsListChange);
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

            connect(_agent, &AgentInMappingVM::inputsListWillBeRemoved, this, &MappingEffectM::_onAgentIOPInputsListChange);
            connect(_agent, &AgentInMappingVM::inputsListAdded, this, &MappingEffectM::_onAgentIOPInputsListChange);
            connect(_agent, &AgentInMappingVM::outputsListWillBeRemoved, this, &MappingEffectM::_onAgentIOPOutputsListChange);
            connect(_agent, &AgentInMappingVM::outputsListAdded, this, &MappingEffectM::_onAgentIOPOutputsListChange);
        }
    }
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
            disconnect(_outputAgent, &AgentInMappingVM::inputsListWillBeRemoved, this, &MappingEffectM::_onOutputAgentIOPInputsListChange);
            disconnect(_outputAgent, &AgentInMappingVM::inputsListAdded, this, &MappingEffectM::_onOutputAgentIOPInputsListChange);
            disconnect(_outputAgent, &AgentInMappingVM::outputsListWillBeRemoved, this, &MappingEffectM::_onOutputAgentIOPOutputsListChange);
            disconnect(_outputAgent, &AgentInMappingVM::outputsListAdded, this, &MappingEffectM::_onOutputAgentIOPOutputsListChange);
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
            connect(_outputAgent, &AgentInMappingVM::inputsListWillBeRemoved, this, &MappingEffectM::_onOutputAgentIOPInputsListChange);
            connect(_outputAgent, &AgentInMappingVM::inputsListAdded, this, &MappingEffectM::_onOutputAgentIOPInputsListChange);
            connect(_outputAgent, &AgentInMappingVM::outputsListWillBeRemoved, this, &MappingEffectM::_onOutputAgentIOPOutputsListChange);
            connect(_outputAgent, &AgentInMappingVM::outputsListAdded, this, &MappingEffectM::_onOutputAgentIOPOutputsListChange);

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
  * @brief Slot on agent inputs list change
  */
void MappingEffectM::_onAgentIOPInputsListChange(QList<InputVM*> inputsList)
{
    // If we have a selected agent iop
    if(_inputName.isEmpty() == false)
    {
        // Check that our input list update concern our selected agent iop
        foreach (InputVM* inputVM, inputsList)
        {
            if(inputVM->name() == _inputName)
            {
                updateInputSelected();
                break;
            }
        }
    }
}

/**
  * @brief Slot on agent outputs list change
  */
void MappingEffectM::_onAgentIOPOutputsListChange(QList<OutputVM*> outputsList)
{
    // If we have a selected agent iop
    if(_inputName.isEmpty() == false)
    {
        // Check that our output list update concern our selected agent iop
        foreach (OutputVM* outputVM, outputsList)
        {
            if(outputVM->name() == _inputName)
            {
                updateInputSelected();
                break;
            }
        }
    }
}

/**
  * @brief Slot on output agent inputs list change
  */
void MappingEffectM::_onOutputAgentIOPInputsListChange(QList<InputVM*> inputsList)
{
    // If we have a selected agent iop
    if(_outputName.isEmpty() == false)
    {
        // Check that our input list update concern our selected agent iop
        foreach (InputVM* inputVM, inputsList)
        {
            if(inputVM->name() == _outputName)
            {
                updateOutputSelected();
                break;
            }
        }
    }
}

/**
  * @brief Slot on output agent outputs list change
  */
void MappingEffectM::_onOutputAgentIOPOutputsListChange(QList<OutputVM*> outputsList)
{
    // If we have a selected agent iop
    if(_inputName.isEmpty() == false)
    {
        // Check that our output list update concern our selected agent iop
        foreach (OutputVM* outputVM, outputsList)
        {
            if(outputVM->name() == _outputName)
            {
                updateOutputSelected();
                break;
            }
        }
    }
}

/**
* @brief Update the selected input
*/
void MappingEffectM::updateInputSelected()
{
    if(_agent != NULL && _inputName.isEmpty() == false)
    {
        QString agentIopName = _inputName;
        AgentIOPM * newAgentIOP = NULL;

        // Fill with outputs
        foreach (OutputVM* output, _agent->outputsList()->toList())
        {
            if(output->firstModel() != NULL)
            {
                if(newAgentIOP == NULL && agentIopName.isEmpty() == false && agentIopName == output->firstModel()->name())
                {
                    newAgentIOP = output->firstModel();
                }
            }
        }

        // Fill with inputs
        foreach (InputVM* input, _agent->inputsList()->toList())
        {
            if(input->firstModel() != NULL)
            {
                if(agentIopName.isEmpty() == false && agentIopName == input->firstModel()->name())
                {
                    newAgentIOP = input->firstModel();
                }
            }
        }

        // Change the input
        if(newAgentIOP != _input)
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
    if(_agent != NULL && _outputName.isEmpty() == false)
    {
        QString agentIopName = _outputName;
        AgentIOPM * newAgentIOP = NULL;

        // Fill with outputs
        foreach (OutputVM* output, _agent->outputsList()->toList())
        {
            if(output->firstModel() != NULL)
            {
                if(newAgentIOP == NULL && agentIopName.isEmpty() == false && agentIopName == output->firstModel()->name())
                {
                    newAgentIOP = output->firstModel();
                }
            }
        }

        // Fill with inputs
        foreach (InputVM* input, _agent->inputsList()->toList())
        {
            if(input->firstModel() != NULL)
            {
                if(agentIopName.isEmpty() == false && agentIopName == input->firstModel()->name())
                {
                    newAgentIOP = input->firstModel();
                }
            }
        }

        // Change the input
        if(newAgentIOP != _output)
        {
            // Set the new agent
            setoutput(newAgentIOP);
        }
    }
}

