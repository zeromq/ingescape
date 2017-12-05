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

#include "iopvalueconditionm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// IOPValueConditionM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
IOPValueConditionM::IOPValueConditionM(QObject *parent) : ActionConditionM(parent),
    _agentIOP(NULL),
    _agentIOPName(""),
    _value("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Initialize value comparison type
    setcomparison(ActionComparisonValueType::EQUAL_TO);
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
* @brief Copy from another condition model
* @param condition to copy
*/
void IOPValueConditionM::copyFrom(ActionConditionM* condition)
{
    ActionConditionM::copyFrom(condition);

    IOPValueConditionM* iopCondition = qobject_cast<IOPValueConditionM*>(condition);
    if(iopCondition != NULL)
    {
        setagentIOP(iopCondition->agentIOP());
        setagentIOPName(iopCondition->agentIOPName());
        _agentIopList.clear();
        _agentIopList.append(iopCondition->agentIopList()->toList());
        setvalue(iopCondition->value());
    }
}

/**
* @brief Custom setter on set agent
*        to fill with outputs
* @param agent
*/
void IOPValueConditionM::setagent(AgentInMappingVM* agent)
{
    AgentInMappingVM* previousAgentM = _agent;

    // Call setter of mother class
    ActionConditionM::setagent(agent);

    if(previousAgentM != agent)
    {
        if(previousAgentM != NULL)
        {
            disconnect(previousAgentM, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &IOPValueConditionM::onInputsOutputsListChange);
        }

        // Clear the list
        _agentIopList.clear();
        setagentIOP(NULL);

        if(_agent != NULL)
        {
            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if(output->firstModel() != NULL)
                {
                    _agentIopList.append(output->firstModel());
                }
            }

            // Select the first item
            if(_agentIopList.count() > 0)
            {
                setagentIOP(_agentIopList.at(0));
            }

            connect(_agent, &AgentInMappingVM::modelsOfInputsAndOutputsChanged, this, &IOPValueConditionM::onInputsOutputsListChange);

        }
    }
}

/**
  * @brief Initialize the agent connections for the action condition
  */
void IOPValueConditionM::initializeConnections()
{
    if(_agent != NULL)
    {
        ActionConditionM::initializeConnections();

        if(_agentIOP != NULL)
        {
            // Subscribe to value change
            connect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);

            // Update valid status
            _onCurrentValueChange(_agentIOP->currentValue());
        }
    }
}

/**
  * @brief Reset the agent connections for the action condition
  */
void IOPValueConditionM::resetConnections()
{
    if(_agent != NULL)
    {
        ActionConditionM::resetConnections();

        if(_agentIOP != NULL)
        {
            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);
        }
    }
}


/**
  * @brief Slot on agent outputs list change
  */
void IOPValueConditionM::onInputsOutputsListChange()
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
                    updateAgentIOPSelected(outputVM->firstModel());
                }
            }
        }

        // FIXME: pourquoi on ne remplit pas "_agentIopList" aussi avec la liste "_agent->inputsList()" ?
    }
}


/**
* @brief Update the selected agent iop
*/
void IOPValueConditionM::updateAgentIOPSelected(AgentIOPM * newAgentIOP)
{
    // Reset the agentIOP
    if (newAgentIOP != _agentIOP)
    {
        // Disconnect old agent iop
        if(_agentIOP != NULL)
        {
            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);
        }

        // Set the new agent
        setagentIOP(newAgentIOP);

        // Connect new agent iop
        if(newAgentIOP != NULL)
        {
            // Subscribe to value change
            connect(newAgentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);

            // Update valid status
            _onCurrentValueChange(_agentIOP->currentValue());
        }
    }
}

/**
* @brief Custom setter for agent iop model
* @param agent iop model
*/
void IOPValueConditionM::setagentIOP(AgentIOPM* agentIop)
{
    if(_agentIOP != agentIop)
    {
        if(_agentIOP != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);
        }

        setisValid(false);

        _agentIOP = agentIop;

        if(_agentIOP != NULL)
        {
            setagentIOPName(_agentIOP->name());

            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);
        }

        Q_EMIT agentIOPChanged(agentIop);
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
  * @brief Slot on agent iop value change
  * @param current value
  */
void IOPValueConditionM::_onCurrentValueChange(QVariant currentValue)
{
    // Trim the condition value to compare with
    QString valueTrimmed = _value.trimmed();

    bool isValid = false;
    if(_agentIOP != NULL)
    {
        // According to the iop type
        switch(_agentIOP->agentIOPValueType())
        {
            case AgentIOPValueTypes::INTEGER :
            case AgentIOPValueTypes::DOUBLE :
            {
                double conditionDblValue = valueTrimmed.toDouble();
                double currentValueDblValue = currentValue.toDouble();

                switch(_comparison)
                {
                    case ActionComparisonValueType::INFERIOR_TO :
                    {
                        isValid = (((double)currentValueDblValue < (double)conditionDblValue)? true : false);
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = (((double)currentValueDblValue > (double)conditionDblValue) ? true : false);
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = qFuzzyCompare(conditionDblValue,currentValueDblValue);
                        break;
                    }
                    default :
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
                switch(_comparison)
                {
                    case ActionComparisonValueType::INFERIOR_TO :
                    {
                        isValid = currentValue.toString().compare(valueTrimmed) < 0 ? true : false;
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = currentValue.toString().compare(valueTrimmed) > 0 ? true : false;
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = currentValue.toString().compare(valueTrimmed) == 0 ? true : false;
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
                bool conditionBoolValue = false;
                if(valueTrimmed.toUpper() == "TRUE" || valueTrimmed.toInt() == 1)
                {
                    conditionBoolValue = true;
                }
                switch(_comparison)
                {
                    case ActionComparisonValueType::INFERIOR_TO :
                    {
                        isValid = currentValue.toBool() < conditionBoolValue ? true : false;
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = currentValue.toBool() > conditionBoolValue ? true : false;
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = currentValue.toBool() == conditionBoolValue ? true : false;
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
                // FIXME - REMOVE
                qDebug() << "IopValueComparison could not be done for "<< AgentIOPValueTypes::staticEnumToString(_agentIOP->agentIOPValueType())<< " type : "<<_agent->name()<< "." << _agentIOP->name();
                break;
            }

        }
    }

    // Set final condition validation state
    setisValid(isValid);
}

/**
  * @brief Slot on IsON flag agent change
  */
void IOPValueConditionM::onAgentModelIsOnChange(bool isON)
{
    Q_UNUSED(isON)

    onInputsOutputsListChange();
}
