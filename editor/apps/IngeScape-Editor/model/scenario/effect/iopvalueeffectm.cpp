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
    _agentIOPType(AgentIOPTypes::INPUT),
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
* @brief Custom setter for agent iop model
* @param value
*/
void IOPValueEffectM::setagentIOP(AgentIOPM* value)
{
    if (_agentIOP != value)
    {
        if (_agentIOP != NULL)
        {
            // UN-subscribe to destruction
            disconnect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);
        }

        _agentIOP = value;

        if (_agentIOP != NULL)
        {
            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);

            //qDebug() << "set agent IOP to" << _agentIOP->name();

            setagentIOPType(_agentIOP->agentIOPType());
            setagentIOPName(_agentIOP->name());
        }
        else {
            //qDebug() << "set agent IOP to NULL !!!";

            //setagentIOPType(AgentIOPTypes::INPUT);
            //setagentIOPName("");
        }

        Q_EMIT agentIOPChanged(value);
    }
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void IOPValueEffectM::copyFrom(ActionEffectM* effect)
{
    // Call our mother class
    ActionEffectM::copyFrom(effect);

    IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(effect);
    if (iopEffect != NULL)
    {
        setagentIOP(iopEffect->agentIOP());
        // iopEffect->agentIOP can be NULL, so we have to set agent IOP "Type" and "Name"
        setagentIOPType(iopEffect->agentIOPType());
        setagentIOPName(iopEffect->agentIOPName());

        _iopMergedList.clear();
        _iopMergedList.append(iopEffect->iopMergedList()->toList());

        setvalue(iopEffect->value());
    }
}


/**
* @brief Setter for property "Agent"
* @param agent
*/
void IOPValueEffectM::setagent(AgentInMappingVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentInMappingVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionEffectM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != NULL) {
            disconnect(previousAgent, &AgentInMappingVM::modelsOfIOPChanged, this, &IOPValueEffectM::_onModelsOfIOPChanged);
        }

        qDebug() << "setagent: setagentIOP(NULL)";

        // Reset the agent IOP
        setagentIOP(NULL);

        // Clear the list
        _iopMergedList.clear();

        if (_agent != NULL)
        {
            // Fill with inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if ((input != NULL) && (input->firstModel() != NULL))
                {
                    _iopMergedList.append(input->firstModel());
                }
            }

            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if ((output != NULL) && (output->firstModel() != NULL))
                {
                    _iopMergedList.append(output->firstModel());
                }
            }

            // Fill with parameters
            foreach (ParameterVM* parameter, _agent->parametersList()->toList())
            {
                if ((parameter != NULL) && (parameter->firstModel() != NULL))
                {
                    _iopMergedList.append(parameter->firstModel());
                }
            }

            // By default, select the first item
            if (!_iopMergedList.isEmpty()) {
                qDebug() << "setagent: setagentIOP(first item by default)";
                setagentIOP(_iopMergedList.at(0));
            }

            connect(_agent, &AgentInMappingVM::modelsOfIOPChanged, this, &IOPValueEffectM::_onModelsOfIOPChanged);
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
  * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the agent in mapping
  */
void IOPValueEffectM::_onModelsOfIOPChanged()
{
    AgentIOPM* newAgentIOP = NULL;

    qDebug() << "_on Models of IOP changed" << AgentIOPTypes::staticEnumToString(_agentIOPType) << _agentIOPName;

    // If we have a selected agent iop
    if (!_agentIOPName.isEmpty())
    {
        _iopMergedList.clear();

        // Add inputs to the merged list
        foreach (InputVM* inputVM, _agent->inputsList()->toList())
        {
            if ((inputVM != NULL) && (inputVM->firstModel() != NULL))
            {
                _iopMergedList.append(inputVM->firstModel());

                if ((_agentIOPType == AgentIOPTypes::INPUT) && (inputVM->name() == _agentIOPName)) {
                    newAgentIOP = inputVM->firstModel();
                }
            }
        }

        // Add outputs to the merged list
        foreach (OutputVM* outputVM, _agent->outputsList()->toList())
        {
            if ((outputVM != NULL) && (outputVM->firstModel() != NULL))
            {
                _iopMergedList.append(outputVM->firstModel());

                if ((_agentIOPType == AgentIOPTypes::OUTPUT) && (outputVM->name() == _agentIOPName)) {
                    newAgentIOP = outputVM->firstModel();
                }
            }
        }

        // Add parameters to the merged list
        foreach (ParameterVM* parameterVM, _agent->parametersList()->toList())
        {
            if ((parameterVM != NULL) && (parameterVM->firstModel() != NULL))
            {
                _iopMergedList.append(parameterVM->firstModel());

                if ((_agentIOPType == AgentIOPTypes::PARAMETER) && (parameterVM->name() == _agentIOPName)) {
                    newAgentIOP = parameterVM->firstModel();
                }
            }
        }
    }

    // Update the agent IOP
    if (newAgentIOP != _agentIOP) {
        qDebug() << "setagent: setagentIOP(newAgentIOP)";
        setagentIOP(newAgentIOP);
    }
}


/**
 * @brief Called when our agent iop model is destroyed
 * @param sender
 */
void IOPValueEffectM::_onAgentIopModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    qDebug() << "_on Agent IOP Model Destroyed --> setagentIOP(NULL)";

    setagentIOP(NULL);
}
