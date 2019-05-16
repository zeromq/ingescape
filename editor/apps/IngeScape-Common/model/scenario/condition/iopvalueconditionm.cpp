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

#include "iopvalueconditionm.h"


#include <QDebug>

/**
 * @brief Enum "ValueComparisonTypes" to string
 * @param value
 * @return
 */
QString ValueComparisonTypes::enumToString(int value)
{
    switch (value)
    {
    case ValueComparisonTypes::SUPERIOR_TO:
        return tr(">");

    case ValueComparisonTypes::INFERIOR_TO:
        return tr("<");

    case ValueComparisonTypes::EQUAL_TO:
        return tr("=");

    default:
        return "";
    }
}


//--------------------------------------------------------------
//
// IOPValueConditionM
//
//--------------------------------------------------------------

/**
 * @brief Constructor
 * @param parent
 */
IOPValueConditionM::IOPValueConditionM(QObject *parent) : ActionConditionM(parent),
    _agentIOP(nullptr),
    _valueComparisonType(ValueComparisonTypes::EQUAL_TO),
    _comparisonValue(""),
    _inputsNumber(0),
    _outputsNumber(0)
    //_parametersNumber(0)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
IOPValueConditionM::~IOPValueConditionM()
{
    // Clear our list
    _iopMergedList.clear();

    // reset agent IOP pointer
    setagentIOP(nullptr);

    // Reset agent
    //setagent(nullptr);

    // Reset the agent connections for the action condition
    resetConnections();
}


/**
* @brief Setter for property "Agent IOP"
* @param value
*/
/*void IOPValueConditionM::setagentIOP(AgentIOPVM* value)
{
    if (_agentIOP != value)
    {
        if (_agentIOP != nullptr)
        {
            // UN-subscribe to destruction
            disconnect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueConditionM::_onAgentIOPDestroyed);
        }

        setisValid(false);

        _agentIOP = value;

        if (_agentIOP != nullptr)
        {
            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPVM::destroyed, this, &IOPValueConditionM::_onAgentIOPDestroyed);
        }

        Q_EMIT agentIOPChanged(agentIop);
    }
}*/


/**
* @brief Copy from another condition model
* @param condition to copy
*/
void IOPValueConditionM::copyFrom(ActionConditionM* condition)
{
    // Call our mother class
    ActionConditionM::copyFrom(condition);

    IOPValueConditionM* iopValueCondition = qobject_cast<IOPValueConditionM*>(condition);
    if (iopValueCondition != nullptr)
    {
        _iopMergedList.clear();
        _iopMergedList.append(iopValueCondition->iopMergedList()->toList());

        setinputsNumber(iopValueCondition->inputsNumber());
        setoutputsNumber(iopValueCondition->outputsNumber());
        //setparametersNumber(iopValueEffect->parametersNumber());

        setvalueComparisonType(iopValueCondition->valueComparisonType());
        setcomparisonValue(iopValueCondition->comparisonValue());

        setagentIOP(iopValueCondition->agentIOP());
    }
}


/**
 * @brief Setter for property "Agent"
 * @param agent
 */
void IOPValueConditionM::setagent(AgentsGroupedByNameVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentsGroupedByNameVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionConditionM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != nullptr)
        {
            // DIS-connect to signals from the agents grouped by name
            disconnect(previousAgent, nullptr, this, nullptr);
        }

        // Reset the agent IOP
        //qDebug() << "IOPValueConditionM::setagentIOP(nullptr)";
        setagentIOP(nullptr);

        // Clear the list
        _iopMergedList.clear();

        setinputsNumber(0);
        setoutputsNumber(0);
        //setparametersNumber(0);

        if (_agent != nullptr)
        {
            // Connect to signals from the agents grouped by name
            //connect(_agent, &AgentsGroupedByNameVM::inputsHaveBeenAdded, this, &IOPValueConditionM::_onInputsHaveBeenAdded);
            connect(_agent, &AgentsGroupedByNameVM::outputsHaveBeenAdded, this, &IOPValueConditionM::_onOutputsHaveBeenAdded);
            //connect(_agent, &AgentsGroupedByNameVM::parametersHaveBeenAdded, this, &IOPValueConditionM::_onParametersHaveBeenAdded);

            //connect(_agent, &AgentsGroupedByNameVM::inputsWillBeRemoved, this, &IOPValueConditionM::_onInputsWillBeRemoved);
            connect(_agent, &AgentsGroupedByNameVM::outputsWillBeRemoved, this, &IOPValueConditionM::_onOutputsWillBeRemoved);
            //connect(_agent, &AgentsGroupedByNameVM::parametersWillBeRemoved, this, &IOPValueConditionM::_onParametersWillBeRemoved);

            /*if (!_agent->inputsList()->isEmpty()) {
                _onInputsHaveBeenAdded(_agent->inputsList()->toList());
            }*/
            if (!_agent->outputsList()->isEmpty()) {
                _onOutputsHaveBeenAdded(_agent->outputsList()->toList());
            }
            /*if (!_agent->parametersList()->isEmpty()) {
                _onParametersHaveBeenAdded(_agent->parametersList()->toList());
            }*/

            // By default, select the first item
            if (!_iopMergedList.isEmpty())
            {
                //qDebug() << "IOPValueConditionM::setagentIOP(first item by default)";
                setagentIOP(_iopMergedList.at(0));
            }
        }
    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void IOPValueConditionM::initializeConnections()
{
    if (_agent != nullptr)
    {
        // Call our mother class
        ActionConditionM::initializeConnections();

        if (_agentIOP != nullptr)
        {
            // Subscribe to value change
            connect(_agentIOP, &AgentIOPVM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);

            if (_agentIOP->firstModel() != nullptr)
            {
                // Update the flag "is Valid"
                _onCurrentValueChanged(_agentIOP->firstModel()->currentValue());
            }
        }
    }
}


/**
  * @brief Reset the agent connections for the action condition
  */
void IOPValueConditionM::resetConnections()
{
    if (_agent != nullptr)
    {
        // Call our mother class
        ActionConditionM::resetConnections();

        if (_agentIOP != nullptr)
        {
            // UN-subscribe to value change
            disconnect(_agentIOP, &AgentIOPVM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);
        }
    }
}


/**
 * @brief Slot called when the flag "is ON" of the agent changed
 * @param isON
 */
void IOPValueConditionM::_onAgentIsOnChanged(bool isON)
{
    if (isON)
    {
        if ((_agentIOP != nullptr) && (_agentIOP->firstModel() != nullptr))
        {
            // Update the flag "is Valid"
            _onCurrentValueChanged(_agentIOP->firstModel()->currentValue());
        }
    }
}


/**
 * @brief Called when our agent iop model is destroyed
 * @param sender
 */
/*void IOPValueConditionM::_onAgentIOPDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentIOP(nullptr);
}*/


/**
 * @brief Slot called when some view models of outputs have been added to the agent(s grouped by name)
 * @param newOutputs
 */
void IOPValueConditionM::_onOutputsHaveBeenAdded(QList<OutputVM*> newOutputs)
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
 * @brief Slot called when some view models of outputs will be removed from the agent(s grouped by name)
 * @param oldOutputs
 */
void IOPValueConditionM::_onOutputsWillBeRemoved(QList<OutputVM*> oldOutputs)
{
    for (OutputVM* output : oldOutputs)
    {
        if ((output != nullptr) && !output->name().isEmpty() && _iopMergedList.contains(output))
        {
            //qDebug() << "IOPValueConditionM::_onOutputsWillBeRemoved" << output->name();

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
  * @brief Slot called when the current value of our agent iop changed
  */
void IOPValueConditionM::_onCurrentValueChanged(QVariant currentValue)
{
    // Returns a string that has whitespace removed from the start and the end.
    QString comparisonValueTrimmed = _comparisonValue.trimmed();

    bool isValid = false;

    if ((_agentIOP != nullptr) && (_agentIOP->firstModel() != nullptr))
    {
        // According to the iop type
        switch(_agentIOP->firstModel()->agentIOPValueType())
        {
        /*case AgentIOPValueTypes::IMPULSION:
        {
            isValid = true;
            break;
        }*/
        case AgentIOPValueTypes::INTEGER:
        case AgentIOPValueTypes::DOUBLE:
        {
            bool conversionComparisonValueToDoubleSucceeded = false;
            double dblConditionValue = comparisonValueTrimmed.toDouble(&conversionComparisonValueToDoubleSucceeded);

            bool conversionCurrentValueToDoubleSucceeded = false;
            double dblCurrentValue = currentValue.toDouble(&conversionCurrentValueToDoubleSucceeded);

            if (conversionComparisonValueToDoubleSucceeded && conversionCurrentValueToDoubleSucceeded)
            {
                switch(_valueComparisonType)
                {
                case ValueComparisonTypes::INFERIOR_TO:
                {
                    isValid = (dblCurrentValue < dblConditionValue) ? true : false;
                    break;
                }
                case ValueComparisonTypes::SUPERIOR_TO:
                {
                    isValid = (dblCurrentValue > dblConditionValue) ? true : false;
                    break;
                }
                case ValueComparisonTypes::EQUAL_TO:
                {
                    isValid = qFuzzyCompare(dblConditionValue, dblCurrentValue);
                    break;
                }
                default:
                {
                    break;
                }
                }

                break;
            }
        }
        case AgentIOPValueTypes::STRING:
        case AgentIOPValueTypes::DATA:
        {
            switch(_valueComparisonType)
            {
            case ValueComparisonTypes::INFERIOR_TO:
            {
                isValid = (currentValue.toString().compare(comparisonValueTrimmed) < 0) ? true : false;
                break;
            }
            case ValueComparisonTypes::SUPERIOR_TO:
            {
                isValid = (currentValue.toString().compare(comparisonValueTrimmed) > 0) ? true : false;
                break;
            }
            case ValueComparisonTypes::EQUAL_TO:
            {
                isValid = (currentValue.toString().compare(comparisonValueTrimmed) == 0) ? true : false;
                break;
            }
            default:
            {
                break;
            }
            }

            break;
        }
        case AgentIOPValueTypes::BOOL:
        {
            bool boolConditionValue = false;

            bool conversionToIntSucceeded = false;
            int intValue = comparisonValueTrimmed.toInt(&conversionToIntSucceeded);

            // Int
            if (conversionToIntSucceeded)
            {
                if (intValue == 0) {
                    boolConditionValue = false;
                }
                else {
                    boolConditionValue = true;
                }
            }
            // String (bool)
            else if (comparisonValueTrimmed.toLower() == "false") {
                boolConditionValue = false;
            }
            // String (bool)
            else if (comparisonValueTrimmed.toLower() == "true") {
                boolConditionValue = true;
            }
            else {
                qWarning() << "Bad value" << comparisonValueTrimmed << "for value condition of" << _agentIOP->name();
            }

            switch(_valueComparisonType)
            {
            case ValueComparisonTypes::INFERIOR_TO:
            {
                isValid = (currentValue.toBool() < boolConditionValue) ? true : false;
                break;
            }
            case ValueComparisonTypes::SUPERIOR_TO:
            {
                isValid = (currentValue.toBool() > boolConditionValue) ? true : false;
                break;
            }
            case ValueComparisonTypes::EQUAL_TO:
            {
                isValid = (currentValue.toBool() == boolConditionValue) ? true : false;
                break;
            }
            default:
            {
                break;
            }
            }

            break;
        }
        //case AgentIOPValueTypes::MIXED:
        default:
        {
            qDebug() << "IOP Value Comparison could not be done for" << AgentIOPValueTypes::staticEnumToString(_agentIOP->firstModel()->agentIOPValueType()) << " type : " << _agent->name() << "." << _agentIOP->name();
            break;
        }
        }
    }

    // Set the flag indicating if our condition is valid
    setisValid(isValid);
}
