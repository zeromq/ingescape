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
            disconnect(_agent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &IOPValueEffectM::onInputsOutputsListChange);
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

            connect(_agent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &IOPValueEffectM::onInputsOutputsListChange);
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

    if ((_agent != NULL) && (_agentIOP != NULL))
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOP->agentIOPType()));

        commandAndParameters << command << _agentIOP->name();

        switch (_agentIOP->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            // FIXME check that value is an INT
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            // FIXME check that value is a DOUBLE
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::STRING: {
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            // FIXME check that value is a BOOL
            commandAndParameters << _value;
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            // Simulate a value
            commandAndParameters << "0";
            break;
        }
        case AgentIOPValueTypes::DATA: {
            commandAndParameters << _value;
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
 * @brief Get a pair with the agent name and the reverse command (with parameters) of our effect
 * @return
 */
QPair<QString, QStringList> IOPValueEffectM::getAgentNameAndReverseCommandWithParameters()
{
    QPair<QString, QStringList> pairAgentNameAndReverseCommand;

    if ((_agent != NULL) && (_agentIOP != NULL))
    {
        pairAgentNameAndReverseCommand.first = _agent->name();

        QStringList reverseCommandAndParameters;

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOP->agentIOPType()));

        reverseCommandAndParameters << command << _agentIOP->name();

        switch (_agentIOP->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            // FIXME check if conversion to INT succeeded
            //reverseCommandAndParameters << QString::number(_agentIOP->currentValue().toInt());
            reverseCommandAndParameters << _agentIOP->displayableCurrentValue();
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            // FIXME check if conversion to DOUBLE succeeded
            //reverseCommandAndParameters << QString::number(_agentIOP->currentValue().toDouble());
            reverseCommandAndParameters << _agentIOP->displayableCurrentValue();
            break;
        }
        case AgentIOPValueTypes::STRING: {
            reverseCommandAndParameters << _agentIOP->currentValue().toString();
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            if (_agentIOP->currentValue().toBool()) {
                reverseCommandAndParameters << "true";
            }
            else {
                reverseCommandAndParameters << "false";
            }
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOP->agentIOPType()) << _agentIOP->name() << "has value type 'IMPULSION', thus the effect is irreversible!";
            break;
        }
        case AgentIOPValueTypes::DATA: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOP->agentIOPType()) << _agentIOP->name() << "has value type 'DATA', thus the effect is irreversible!";
            break;
        }
        default:
            break;
        }

        pairAgentNameAndReverseCommand.second = reverseCommandAndParameters;
    }

    return pairAgentNameAndReverseCommand;
}


/**
  * @brief Slot on agent inputs/outputs lists change
  */
void IOPValueEffectM::onInputsOutputsListChange()
{
    AgentIOPM * newAgentIOP = NULL;

    // If we have a selected agent iop
    if (!_agentIOPName.isEmpty())
    {
        _iopMergedList.clear();

        // Check that our input list update concern our selected agent iop
        foreach (InputVM* inputVM, _agent->inputsList()->toList())
        {
            if(inputVM->firstModel() != NULL)
            {
                _iopMergedList.append(inputVM->firstModel());
                if(inputVM->name() == _agentIOPName)
                {
                    newAgentIOP = inputVM->firstModel();
                }
            }
        }

        // Check that our output list update concern our selected agent iop
        foreach (OutputVM* outputVM, _agent->outputsList()->toList())
        {
            if(outputVM->firstModel() != NULL)
            {
                _iopMergedList.append(outputVM->firstModel());
                if(outputVM->name() == _agentIOPName)
                {
                    newAgentIOP = outputVM->firstModel();
                }
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


