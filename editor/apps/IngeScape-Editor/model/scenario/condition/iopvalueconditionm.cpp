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
    _agentIOP(NULL),
    _agentIOPName(""),
    _value(""),
    _valueComparisonType(ValueComparisonTypes::EQUAL_TO)
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
    _agentIopList.clear();

    // reset agent IOP pointer
    setagentIOP(NULL);
}


/**
* @brief Custom setter for agent IOP model
* @param agent iop model
*/
void IOPValueConditionM::setagentIOP(AgentIOPM* agentIop)
{
    if (_agentIOP != agentIop)
    {
        if (_agentIOP != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);
        }

        setisValid(false);

        _agentIOP = agentIop;

        if (_agentIOP != NULL)
        {
            setagentIOPName(_agentIOP->name());

            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);
        }

        Q_EMIT agentIOPChanged(agentIop);
    }
}


/**
* @brief Copy from another condition model
* @param condition to copy
*/
void IOPValueConditionM::copyFrom(ActionConditionM* condition)
{
    // Call our mother class
    ActionConditionM::copyFrom(condition);

    IOPValueConditionM* iopCondition = qobject_cast<IOPValueConditionM*>(condition);
    if (iopCondition != NULL)
    {
        setagentIOP(iopCondition->agentIOP());
        setagentIOPName(iopCondition->agentIOPName());
        setvalue(iopCondition->value());
        setvalueComparisonType(iopCondition->valueComparisonType());

        _agentIopList.clear();
        _agentIopList.append(iopCondition->agentIopList()->toList());
    }
}


/**
 * @brief Setter for property "Agent"
 * @param agent
 */
void IOPValueConditionM::setagent(AgentInMappingVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentInMappingVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionConditionM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != NULL) {
            disconnect(previousAgent, &AgentInMappingVM::modelsOfIOPChanged, this, &IOPValueConditionM::_onModelsOfIOPChanged);
        }

        // Reset the agent IOP
        setagentIOP(NULL);

        // Clear the list
        _agentIopList.clear();

        if (_agent != NULL)
        {
            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if (output->firstModel() != NULL) {
                    _agentIopList.append(output->firstModel());
                }
            }

            // Select the first item
            if (!_agentIopList.isEmpty()) {
                setagentIOP(_agentIopList.at(0));
            }

            connect(_agent, &AgentInMappingVM::modelsOfIOPChanged, this, &IOPValueConditionM::_onModelsOfIOPChanged);
        }
    }
}


/**
  * @brief Initialize the agent connections for the action condition
  */
void IOPValueConditionM::initializeConnections()
{
    if (_agent != NULL)
    {
        // Call our mother class
        ActionConditionM::initializeConnections();

        if (_agentIOP != NULL)
        {
            // Subscribe to value change
            connect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);

            // Update flag "is Valid"
            _onCurrentValueChanged(_agentIOP->currentValue());
        }
    }
}


/**
  * @brief Reset the agent connections for the action condition
  */
void IOPValueConditionM::resetConnections()
{
    if (_agent != NULL)
    {
        // Call our mother class
        ActionConditionM::resetConnections();

        if (_agentIOP != NULL)
        {
            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);
        }
    }
}


/**
 * @brief Slot called when the flag "is ON" of an agent changed
 * @param isON
 */
void IOPValueConditionM::_onAgentIsOnChanged(bool isON)
{
    if (isON) {
        _onModelsOfIOPChanged();
    }
}


/**
  * @brief Slot called when the models of Inputs/Outputs/Parameters changed of the agent in mapping
  */
void IOPValueConditionM::_onModelsOfIOPChanged()
{
    // If we have a selected agent iop
    if (!_agentIOPName.isEmpty())
    {
        _agentIopList.clear();

        // Check that our output list update concern our selected agent iop
        foreach (OutputVM* outputVM, _agent->outputsList()->toList())
        {
            if ((outputVM != NULL) && (outputVM->firstModel() != NULL))
            {
                _agentIopList.append(outputVM->firstModel());

                if (outputVM->name() == _agentIOPName) {
                    _updateAgentIOPSelected(outputVM->firstModel());
                }
            }
        }
    }
}


/**
 * @brief Called when our agent iop model is destroyed
 * @param sender
 */
void IOPValueConditionM::_onAgentIopModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentIOP(NULL);
}


/**
  * @brief Slot called when the current value of our agent iop changed
  */
void IOPValueConditionM::_onCurrentValueChanged(QVariant currentValue)
{
    // Trim the condition value to compare with
    QString valueTrimmed = _value.trimmed();

    bool isValid = false;

    if (_agentIOP != NULL)
    {
        // According to the iop type
        switch(_agentIOP->agentIOPValueType())
        {
            case AgentIOPValueTypes::INTEGER :
            case AgentIOPValueTypes::DOUBLE :
            {
                double dblConditionValue = valueTrimmed.toDouble();
                double dblCurrentValue = currentValue.toDouble();

                switch(_valueComparisonType)
                {
                    case ValueComparisonTypes::INFERIOR_TO:
                    {
                        isValid = ((dblCurrentValue < dblConditionValue) ? true : false);
                        break;
                    }
                    case ValueComparisonTypes::SUPERIOR_TO:
                    {
                        isValid = ((dblCurrentValue > dblConditionValue) ? true : false);
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
            case AgentIOPValueTypes::MIXED :
            case AgentIOPValueTypes::STRING :
            case AgentIOPValueTypes::DATA :
            {
                switch(_valueComparisonType)
                {
                    case ValueComparisonTypes::INFERIOR_TO:
                    {
                        isValid = (currentValue.toString().compare(valueTrimmed) < 0 ? true : false);
                        break;
                    }
                    case ValueComparisonTypes::SUPERIOR_TO:
                    {
                        isValid = (currentValue.toString().compare(valueTrimmed) > 0 ? true : false);
                        break;
                    }
                    case ValueComparisonTypes::EQUAL_TO:
                    {
                        isValid = (currentValue.toString().compare(valueTrimmed) == 0 ? true : false);
                        break;
                    }
                    default :
                    {
                        break;
                    }
                }
                break;
            }
            case AgentIOPValueTypes::BOOL :
            {
                bool boolConditionValue = false;
                if (valueTrimmed.toUpper() == "TRUE" || valueTrimmed.toInt() == 1)
                {
                    boolConditionValue = true;
                }

                switch(_valueComparisonType)
                {
                    case ValueComparisonTypes::INFERIOR_TO :
                    {
                        isValid = (currentValue.toBool() < boolConditionValue ? true : false);
                        break;
                    }
                    case ValueComparisonTypes::SUPERIOR_TO :
                    {
                        isValid = (currentValue.toBool() > boolConditionValue ? true : false);
                        break;
                    }
                    case ValueComparisonTypes::EQUAL_TO :
                    {
                        isValid = (currentValue.toBool() == boolConditionValue ? true : false);
                        break;
                    }
                    default :
                    {
                        break;
                    }
                }

                break;
            }
            default :
            {
                qDebug() << "IOP Value Comparison could not be done for" << AgentIOPValueTypes::staticEnumToString(_agentIOP->agentIOPValueType()) << " type : " << _agent->name() << "." << _agentIOP->name();
                break;
            }

        }
    }

    // Set the flag indicating if our condition is valid
    setisValid(isValid);
}


/**
* @brief Update the selected agent iop
*/
void IOPValueConditionM::_updateAgentIOPSelected(AgentIOPM * newAgentIOP)
{
    // Reset the agent IOP
    if (newAgentIOP != _agentIOP)
    {
        // Disconnect old agent iop
        if (_agentIOP != NULL)
        {
            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);
        }

        // Set the new agent
        setagentIOP(newAgentIOP);

        // Connect new agent iop
        if (newAgentIOP != NULL)
        {
            // Subscribe to value change
            connect(newAgentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);

            // Update valid status
            _onCurrentValueChanged(_agentIOP->currentValue());
        }
    }
}

