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
    setagentIOP(nullptr);
}


/**
* @brief Custom setter for agent IOP model
* @param agent iop model
*/
void IOPValueConditionM::setagentIOP(AgentIOPM* agentIop)
{
    if (_agentIOP != agentIop)
    {
        if (_agentIOP != nullptr)
        {
            // UnSubscribe to destruction
            disconnect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);
        }

        setisValid(false);

        _agentIOP = agentIop;

        if (_agentIOP != nullptr)
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
    if (iopCondition != nullptr)
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
void IOPValueConditionM::setagent(AgentsGroupedByNameVM* agent)
{
    // Save the previous agent before the call to the setter of our mother class
    AgentsGroupedByNameVM* previousAgent = _agent;

    // Call the setter of our mother class
    ActionConditionM::setagent(agent);

    // Value of agent changed
    if (previousAgent != _agent)
    {
        if (previousAgent != nullptr) {
            disconnect(previousAgent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &IOPValueConditionM::_onModelsOfIOPChanged);
        }

        // Reset the agent IOP
        setagentIOP(nullptr);

        // Clear the list
        _agentIopList.clear();

        if (_agent != nullptr)
        {
            // Fill with outputs
            for (OutputVM* output : _agent->outputsList()->toList())
            {
                if (output->firstModel() != nullptr) {
                    _agentIopList.append(output->firstModel());
                }
            }

            // Select the first item
            if (!_agentIopList.isEmpty()) {
                setagentIOP(_agentIopList.at(0));
            }

            connect(_agent, &AgentsGroupedByNameVM::modelsOfIOPChanged, this, &IOPValueConditionM::_onModelsOfIOPChanged);
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
    if (_agent != nullptr)
    {
        // Call our mother class
        ActionConditionM::resetConnections();

        if (_agentIOP != nullptr)
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
        for (OutputVM* outputVM : _agent->outputsList()->toList())
        {
            if ((outputVM != nullptr) && (outputVM->firstModel() != nullptr))
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

    setagentIOP(nullptr);
}


/**
  * @brief Slot called when the current value of our agent iop changed
  */
void IOPValueConditionM::_onCurrentValueChanged(QVariant currentValue)
{
    // Returns a string that has whitespace removed from the start and the end.
    QString valueTrimmed = _value.trimmed();

    bool isValid = false;

    if (_agentIOP != nullptr)
    {
        // According to the iop type
        switch(_agentIOP->agentIOPValueType())
        {
        /*case AgentIOPValueTypes::IMPULSION:
        {
            isValid = true;
            break;
        }*/
        case AgentIOPValueTypes::INTEGER:
        case AgentIOPValueTypes::DOUBLE:
        {
            bool conversionConditionValueToDoubleSucceeded = false;
            double dblConditionValue = valueTrimmed.toDouble(&conversionConditionValueToDoubleSucceeded);

            bool conversionCurrentValueToDoubleSucceeded = false;
            double dblCurrentValue = currentValue.toDouble(&conversionCurrentValueToDoubleSucceeded);

            if (conversionConditionValueToDoubleSucceeded && conversionCurrentValueToDoubleSucceeded)
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
                isValid = (currentValue.toString().compare(valueTrimmed) < 0) ? true : false;
                break;
            }
            case ValueComparisonTypes::SUPERIOR_TO:
            {
                isValid = (currentValue.toString().compare(valueTrimmed) > 0) ? true : false;
                break;
            }
            case ValueComparisonTypes::EQUAL_TO:
            {
                isValid = (currentValue.toString().compare(valueTrimmed) == 0) ? true : false;
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
            int intValue = valueTrimmed.toInt(&conversionToIntSucceeded);

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
            else if (valueTrimmed.toLower() == "false") {
                boolConditionValue = false;
            }
            // String (bool)
            else if (valueTrimmed.toLower() == "true") {
                boolConditionValue = true;
            }
            else {
                qWarning() << "Bad value" << valueTrimmed << "for value condition of" << _agentIOPName;
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
        if (_agentIOP != nullptr)
        {
            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);
        }

        // Set the new agent
        setagentIOP(newAgentIOP);

        // Connect new agent iop
        if (newAgentIOP != nullptr)
        {
            // Subscribe to value change
            connect(newAgentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChanged);

            // Update valid status
            _onCurrentValueChanged(_agentIOP->currentValue());
        }
    }
}

