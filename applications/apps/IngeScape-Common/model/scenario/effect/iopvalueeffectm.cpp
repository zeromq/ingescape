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

    // Reset agent IOP
    setagentIOP(nullptr);

    // Reset agent
    setagent(nullptr);
}


/**
* @brief Setter for property "Agent IOP"
* @param value
*/
/*void IOPValueEffectM::setagentIOP(AgentIOPVM* value)
{
    if (_agentIOP != value)
    {
        if (_agentIOP != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueEffectM::_onAgentIOPDestroyed);
        }

        _agentIOP = value;

        if (_agentIOP != nullptr)
        {
            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueEffectM::_onAgentIOPDestroyed);
        }

        Q_EMIT agentIOPChanged(value);
    }
}*/


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
        _iopMergedList.clear();
        _iopMergedList.append(iopValueEffect->iopMergedList()->toList());

        setinputsNumber(iopValueEffect->inputsNumber());
        setoutputsNumber(iopValueEffect->outputsNumber());
        setparametersNumber(iopValueEffect->parametersNumber());

        setagentIOP(iopValueEffect->agentIOP());

        setvalue(iopValueEffect->value());
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
            disconnect(previousAgent, nullptr, this, nullptr);
        }

        // Reset the agent IOP
        //qDebug() << "IOPValueEffectM::setagentIOP(nullptr)";
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
                //qDebug() << "IOPValueEffectM::setagentIOP(first item by default)";
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

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOP->firstModel()->agentIOPType()));

        QStringList commandAndParameters = {
            command,
            _agentIOP->name()
        };

        switch (_agentIOP->firstModel()->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            // FIXME check that value is an INT
            commandAndParameters.append(_value);
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            // FIXME check that value is a DOUBLE
            commandAndParameters.append(_value);
            break;
        }
        case AgentIOPValueTypes::STRING: {
            commandAndParameters.append(_value);
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            // FIXME check that value is a BOOL
            commandAndParameters.append(_value);
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            // Simulate a value
            commandAndParameters.append("0");
            break;
        }
        case AgentIOPValueTypes::DATA: {
            commandAndParameters.append(_value);
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

        // SET_INPUT / SET_OUTPUT / SET_PARAMETER
        QString command = QString("SET_%1").arg(AgentIOPTypes::staticEnumToString(_agentIOP->firstModel()->agentIOPType()));

        QStringList reverseCommandAndParameters = {
            command,
            _agentIOP->name()
        };

        switch (_agentIOP->firstModel()->agentIOPValueType())
        {
        case AgentIOPValueTypes::INTEGER: {
            // FIXME check if conversion to INT succeeded
            //reverseCommandAndParameters.append(QString::number(_agentIOP->currentValue().toInt());
            reverseCommandAndParameters.append(_agentIOP->firstModel()->displayableCurrentValue());
            break;
        }
        case AgentIOPValueTypes::DOUBLE: {
            // FIXME check if conversion to DOUBLE succeeded
            //reverseCommandAndParameters.append(QString::number(_agentIOP->currentValue().toDouble());
            reverseCommandAndParameters.append(_agentIOP->firstModel()->displayableCurrentValue());
            break;
        }
        case AgentIOPValueTypes::STRING: {
            reverseCommandAndParameters.append(_agentIOP->firstModel()->currentValue().toString());
            break;
        }
        case AgentIOPValueTypes::BOOL: {
            if (_agentIOP->firstModel()->currentValue().toBool()) {
                reverseCommandAndParameters.append("true");
            }
            else {
                reverseCommandAndParameters.append("false");
            }
            break;
        }
        case AgentIOPValueTypes::IMPULSION: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOP->firstModel()->agentIOPType()) << _agentIOP->name() << "has value type 'IMPULSION', thus the effect is irreversible!";
            break;
        }
        case AgentIOPValueTypes::DATA: {
            qWarning() << AgentIOPTypes::staticEnumToString(_agentIOP->firstModel()->agentIOPType()) << _agentIOP->name() << "has value type 'DATA', thus the effect is irreversible!";
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
 * @brief Slot called when our agent IOP is destroyed
 * @param sender
 */
/*void IOPValueEffectM::_onAgentIOPDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentIOP(nullptr);
}*/


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
            //qDebug() << "IOPValueEffectM::_onInputsWillBeRemoved" << input->name();

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
            //qDebug() << "IOPValueEffectM::_onOutputsWillBeRemoved" << output->name();

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
            //qDebug() << "IOPValueEffectM::_onParametersWillBeRemoved" << parameter->name();

            // If this parameter is selected
            if (_agentIOP == parameter) {
                setagentIOP(nullptr);
            }

            _iopMergedList.remove(parameter);
        }
    }

    setparametersNumber(_parametersNumber - oldParameters.count());
}
