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

#include "iopvalueeffectm.h"

#include <QDebug>


//--------------------------------------------------------------
//
// IOPValueEffectM
//
//--------------------------------------------------------------

/**
 * @brief Default constructor
 * @param parent
 */
IOPValueEffectM::IOPValueEffectM(QObject *parent) : ActionEffectM(parent),
    _agentIOP(NULL),
    _agentIOPName(""),
    _value("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
IOPValueEffectM::~IOPValueEffectM()
{
    // Clear our list
    _iopMergedList.clear();

    // reset agent IOP pointer
    setagentIOP(NULL);
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void IOPValueEffectM::copyFrom(ActionEffectM* effect)
{
    // Call mother class
    ActionEffectM::copyFrom(effect);

    IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(effect);
    if (iopEffect != NULL)
    {
        setagentIOP(iopEffect->agentIOP());
        setagentIOPName(iopEffect->agentIOPName());
        _iopMergedList.clear();
        _iopMergedList.append(iopEffect->iopMergedList()->toList());
        setvalue(iopEffect->value());
    }
}


/**
* @brief Custom setter on set agent to fill inputs and outputs
* @param agent
*/
void IOPValueEffectM::setagent(AgentInMappingVM* agent)
{
    AgentInMappingVM* previousAgent = _agent;

    // Call setter of mother class
    ActionEffectM::setagent(agent);

    if (previousAgent != agent)
    {
        if(_agent != NULL)
        {
            disconnect(_agent, &AgentInMappingVM::inputsListWillBeRemoved, this, &IOPValueEffectM::onInputsListChange);
            disconnect(_agent, &AgentInMappingVM::inputsListAdded, this, &IOPValueEffectM::onInputsListChange);
            disconnect(_agent, &AgentInMappingVM::outputsListWillBeRemoved, this, &IOPValueEffectM::onOutputsListChange);
            disconnect(_agent, &AgentInMappingVM::outputsListAdded, this, &IOPValueEffectM::onOutputsListChange);
        }

        setagentIOP(NULL);

        // Clear the list
        _iopMergedList.clear();

        if (_agent != NULL)
        {
            // Fill with inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if (input->firstModel() != NULL)
                {
                    _iopMergedList.append(input->firstModel());
                }
            }

            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if (output->firstModel() != NULL)
                {
                    _iopMergedList.append(output->firstModel());
                }
            }

            // Select the first item
            if (_iopMergedList.count() > 0) {
                setagentIOP(_iopMergedList.at(0));
            }

            connect(_agent, &AgentInMappingVM::inputsListWillBeRemoved, this, &IOPValueEffectM::onInputsListChange);
            connect(_agent, &AgentInMappingVM::inputsListAdded, this, &IOPValueEffectM::onInputsListChange);
            connect(_agent, &AgentInMappingVM::outputsListWillBeRemoved, this, &IOPValueEffectM::onOutputsListChange);
            connect(_agent, &AgentInMappingVM::outputsListAdded, this, &IOPValueEffectM::onOutputsListChange);
        }
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentInMappingVM*, QStringList> IOPValueEffectM::getAgentAndCommandWithParameters()
{
    QPair<AgentInMappingVM*, QStringList> pairAgentAndCommandWithParameters;

    if (_agent != NULL)
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOP->agentIOPType()));

        commandAndParameters << command << _agentIOP->name();

        switch (_agentIOP->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::STRING: {
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            // Simulate a value
            commandAndParameters << "0";
            break;
        }
        case AgentIOPValueTypes::DATA: {
            // Simulate a value
            commandAndParameters << "0";
            break;
        }
        default:
            break;
        }

        pairAgentAndCommandWithParameters.second = commandAndParameters;
    }

    return pairAgentAndCommandWithParameters;
}


/**
  * @brief Slot on agent inputs list change
  */
void IOPValueEffectM::onInputsListChange(QList<InputVM*> inputsList)
{
    // If we have a selected agent iop
    if(_agentIOPName.isEmpty() == false)
    {
        // Check that our input list update concern our selected agent iop
        foreach (InputVM* inputVM, inputsList)
        {
            if(inputVM->name() == _agentIOPName)
            {
                updateAgentIOPSelected();
                break;
            }
        }
    }
}

/**
  * @brief Slot on agent outputs list change
  */
void IOPValueEffectM::onOutputsListChange(QList<OutputVM*> outputsList)
{
    // If we have a selected agent iop
    if(_agentIOPName.isEmpty() == false)
    {
        // Check that our output list update concern our selected agent iop
        foreach (OutputVM* outputVM, outputsList)
        {
            if(outputVM->name() == _agentIOPName)
            {
                updateAgentIOPSelected();
                break;
            }
        }
    }
}

/**
* @brief Update the selected agent iop
*/
void IOPValueEffectM::updateAgentIOPSelected()
{
    if(_agent != NULL && _agentIOPName.isEmpty() == false)
    {
        QString agentIopName = _agentIOPName;
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

        // Reset the agentIOP
        if(newAgentIOP != _agentIOP)
        {
            // Set the new agent
            setagentIOP(newAgentIOP);
        }
    }
}

/**
* @brief Custom setter for agent iop model
* @param agent iop model
*/
void IOPValueEffectM::setagentIOP(AgentIOPM* agentIop)
{
    if(_agentIOP != agentIop)
    {
        if(_agentIOP != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);
        }

        _agentIOP = agentIop;

        if(_agentIOP != NULL)
        {
            setagentIOPName(_agentIOP->name());

            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);
        }

        Q_EMIT agentIOPChanged(agentIop);
    }
}

/**
 * @brief Called when our agent iop model is destroyed
 * @param sender
 */
void IOPValueEffectM::_onAgentIopModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentIOP(NULL);
}


