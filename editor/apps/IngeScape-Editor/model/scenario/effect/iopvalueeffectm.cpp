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
    _agentIOP(nullptr),
    _agentIOPType(AgentIOPTypes::INPUT),
    _agentIOPName(""),
    _value(""),
    _inputsNumber(0),
    _outputsNumber(0),
    _parametersNumber(0)
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
    setagentIOP(nullptr);
}


/**
* @brief Custom setter for agent iop model
* @param value
*/
void IOPValueEffectM::setagentIOP(AgentIOPVM* value)
{
    if (_agentIOP != value)
    {
        if (_agentIOP != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);
        }

        _agentIOP = value;

        if (_agentIOP != nullptr)
        {
            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueEffectM::_onAgentIopModelDestroyed);

            //qDebug() << "set agent IOP to" << _agentIOP->name();

            if (_agentIOP->firstModel() != nullptr)
            {
                setagentIOPType(_agentIOP->firstModel()->agentIOPType());
                setagentIOPName(_agentIOP->name());
            }
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

    IOPValueEffectM* iopValueEffect = qobject_cast<IOPValueEffectM*>(effect);
    if (iopValueEffect != nullptr)
    {
        setagentIOP(iopValueEffect->agentIOP());
        // iopValueEffect->agentIOP can be NULL, so we have to set agent IOP "Type" and "Name"
        setagentIOPType(iopValueEffect->agentIOPType());
        setagentIOPName(iopValueEffect->agentIOPName());

        setvalue(iopValueEffect->value());

        _iopMergedList.clear();
        _iopMergedList.append(iopValueEffect->iopMergedList()->toList());

        setinputsNumber(iopValueEffect->inputsNumber());
        setoutputsNumber(iopValueEffect->outputsNumber());
        setparametersNumber(iopValueEffect->parametersNumber());
    }
}


/**
* @brief Setter for property "Agent"
* @param agent
*/
void IOPValueEffectM::setagent(AgentsGroupedByNameVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentsGroupedByNameVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionEffectM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != nullptr)
        {
            // DIS-connect to signals from the agents grouped by name
            disconnect(_agent, 0, this, 0);
        }

        qDebug() << "setagent: setagentIOP(nullptr)";

        // Reset the agent IOP
        setagentIOP(nullptr);

        // Clear the list
        _iopMergedList.clear();

        setinputsNumber(0);
        setoutputsNumber(0);
        setparametersNumber(0);

        if (_agent != nullptr)
        {
            // Connect to signals from the agents grouped by name
            connect(_agent, &AgentsGroupedByNameVM::inputsHaveBeenAdded, this, &IOPValueEffectM::_onInputsHaveBeenAdded);
            connect(_agent, &AgentsGroupedByNameVM::outputsHaveBeenAdded, this, &IOPValueEffectM::_onOutputsHaveBeenAdded);
            connect(_agent, &AgentsGroupedByNameVM::parametersHaveBeenAdded, this, &IOPValueEffectM::_onParametersHaveBeenAdded);

            connect(_agent, &AgentsGroupedByNameVM::inputsWillBeRemoved, this, &IOPValueEffectM::_onInputsWillBeRemoved);
            connect(_agent, &AgentsGroupedByNameVM::outputsWillBeRemoved, this, &IOPValueEffectM::_onOutputsWillBeRemoved);
            connect(_agent, &AgentsGroupedByNameVM::parametersWillBeRemoved, this, &IOPValueEffectM::_onParametersWillBeRemoved);

            if (!_agent->inputsList()->isEmpty()) {
                _onInputsHaveBeenAdded(_agent->inputsList()->toList());
            }
            if (!_agent->outputsList()->isEmpty()) {
                _onOutputsHaveBeenAdded(_agent->outputsList()->toList());
            }
            if (!_agent->parametersList()->isEmpty()) {
                _onParametersHaveBeenAdded(_agent->parametersList()->toList());
            }

            // By default, select the first item
            if (!_iopMergedList.isEmpty())
            {
                qDebug() << "setagent: setagentIOP(first item by default)";
                setagentIOP(_iopMergedList.at(0));
            }
         }
    }
}


/**
 * @brief Get a pair with the agent and the command (with parameters) of our effect
 * @return
 */
QPair<AgentsGroupedByNameVM*, QStringList> IOPValueEffectM::getAgentAndCommandWithParameters()
{
    QPair<AgentsGroupedByNameVM*, QStringList> pairAgentAndCommandWithParameters;

    if ((_agent != nullptr) && (_agentIOP != nullptr) && (_agentIOP->firstModel() != nullptr))
    {
        pairAgentAndCommandWithParameters.first = _agent;

        QStringList commandAndParameters;

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOPType));

        commandAndParameters << command << _agentIOP->name();

        switch (_agentIOP->firstModel()->agentIOPValueType())
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

    if ((_agent != nullptr) && (_agentIOP != nullptr) && (_agentIOP->firstModel() != nullptr))
    {
        pairAgentNameAndReverseCommand.first = _agent->name();

        QStringList reverseCommandAndParameters;

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOPType));

        reverseCommandAndParameters << command << _agentIOP->name();

        switch (_agentIOP->firstModel()->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            // FIXME check if conversion to INT succeeded
            //reverseCommandAndParameters << QString::number(_agentIOP->currentValue().toInt());
            reverseCommandAndParameters << _agentIOP->firstModel()->displayableCurrentValue();
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            // FIXME check if conversion to DOUBLE succeeded
            //reverseCommandAndParameters << QString::number(_agentIOP->currentValue().toDouble());
            reverseCommandAndParameters << _agentIOP->firstModel()->displayableCurrentValue();
            break;
        }
        case AgentIOPValueTypes::STRING: {
            reverseCommandAndParameters << _agentIOP->firstModel()->currentValue().toString();
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            if (_agentIOP->firstModel()->currentValue().toBool()) {
                reverseCommandAndParameters << "true";
            }
            else {
                reverseCommandAndParameters << "false";
            }
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOPType) << _agentIOP->name() << "has value type 'IMPULSION', thus the effect is irreversible!";
            break;
        }
        case AgentIOPValueTypes::DATA: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOPType) << _agentIOP->name() << "has value type 'DATA', thus the effect is irreversible!";
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
 * @brief Slot called when some view models of inputs have been added to the agent(s grouped by name)
 * @param newInputs
 */
void IOPValueEffectM::_onInputsHaveBeenAdded(QList<InputVM*> newInputs)
{
    int index = _inputsNumber;

    for (InputVM* input : newInputs)
    {
        if ((input != nullptr) && !input->name().isEmpty())
        {
            qDebug() << "IOPValueEffectM::_onInputsHaveBeenAdded" << input->name();

            _iopMergedList.insert(index, input);
            index++;
        }
    }

    setinputsNumber(_inputsNumber + newInputs.count());
}


/**
 * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
 * @param newOutputs
 */
void IOPValueEffectM::_onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs)
{
    int index = _inputsNumber + _outputsNumber;

    for (OutputVM* output : newOutputs)
    {
        if ((output != nullptr) && !output->name().isEmpty())
        {
            qDebug() << "IOPValueEffectM::_onOutputsHaveBeenAdded" << output->name();

            _iopMergedList.insert(index, output);
            index++;
        }
    }

    setoutputsNumber(_outputsNumber + newOutputs.count());
}


/**
 * @brief Slot called when some view models of parameters have been added to our agent(s grouped by name)
 * @param newParameters
 */
void IOPValueEffectM::_onParametersHaveBeenAdded(QList<ParameterVM*> newParameters)
{
    int index = _inputsNumber + _outputsNumber + _parametersNumber;

    for (ParameterVM* parameter : newParameters)
    {
        if ((parameter != nullptr) && !parameter->name().isEmpty())
        {
            qDebug() << "IOPValueEffectM::_onParametersHaveBeenAdded" << parameter->name();

            _iopMergedList.insert(index, parameter);
            index++;
        }
    }

    setparametersNumber(_parametersNumber + newParameters.count());
}


/**
 * @brief Slot called when some view models of inputs will be removed from the agent(s grouped by name)
 * @param oldInputs
 */
void IOPValueEffectM::_onInputsWillBeRemoved(QList<InputVM*> oldInputs)
{
    for (InputVM* input : oldInputs)
    {
        if ((input != nullptr) && !input->name().isEmpty() && _iopMergedList.contains(input))
        {
            qDebug() << "IOPValueEffectM::_onInputsWillBeRemoved" << input->name();

            // If this input is selected
            if (_agentIOP == input) {
                setagentIOP(nullptr);
            }

            _iopMergedList.remove(input);
        }
    }

    setinputsNumber(_inputsNumber - oldInputs.count());
}


/**
 * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
 * @param oldOutputs
 */
void IOPValueEffectM::_onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs)
{
    for (OutputVM* output : oldOutputs)
    {
        if ((output != nullptr) && !output->name().isEmpty() && _iopMergedList.contains(output))
        {
            qDebug() << "IOPValueEffectM::_onOutputsWillBeRemoved" << output->name();

            // If this output is selected
            if (_agentIOP == output) {
                setagentIOP(nullptr);
            }

            _iopMergedList.remove(output);
        }
    }

    setoutputsNumber(_outputsNumber - oldOutputs.count());
}


/**
 * @brief Slot called when some view models of parameters will be removed from our agent(s grouped by name)
 * @param oldParameters
 */
void IOPValueEffectM::_onParametersWillBeRemoved(QList<ParameterVM*> oldParameters)
{
    for (ParameterVM* parameter : oldParameters)
    {
        if ((parameter != nullptr) && !parameter->name().isEmpty() && _iopMergedList.contains(parameter))
        {
            qDebug() << "IOPValueEffectM::_onParametersWillBeRemoved" << parameter->name();

            // If this parameter is selected
            if (_agentIOP == parameter) {
                setagentIOP(nullptr);
            }

            _iopMergedList.remove(parameter);
        }
    }

    setparametersNumber(_parametersNumber - oldParameters.count());
}


/**
 * @brief Called when our agent iop model is destroyed
 * @param sender
 */
void IOPValueEffectM::_onAgentIopModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    qDebug() << "_on Agent IOP Model Destroyed --> setagentIOP(nullptr)";

    setagentIOP(nullptr);
}
