/*
 *	IOPValueConditionM
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
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

    IOPValueConditionM* iopCondition = dynamic_cast<IOPValueConditionM*>(condition);
    if(iopCondition != NULL)
    {
        setagentIOP(iopCondition->agentIOP());
        _agentIopList.clear();
        _agentIopList.append(iopCondition->agentIopList()->toList());
        setvalue(iopCondition->value());
    }
}

/**
* @brief Custom setter on set agent model
*        to fill inputs and outputs
* @param agentModel
*/
void IOPValueConditionM::setagentModel(AgentInMappingVM* agentModel)
{
    ActionConditionM::setagentModel(agentModel);

    // Clear the list
    _agentIopList.clear();
    setagentIOP(NULL);

    if(_agentModel != NULL)
    {
        // Fill with inputs
        foreach (InputVM* input, _agentModel->inputsList()->toList())
        {
            if(input->firstModel() != NULL)
            {
                _agentIopList.append(input->firstModel());
            }
        }

        // Fill with outputs
        foreach (OutputVM* output, _agentModel->outputsList()->toList())
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

        initializeConnections();
    } else {
        resetConnections();
    }
}

/**
  * @brief Initialize the agent connections for the action condition
  */
void IOPValueConditionM::initializeConnections()
{
    if(_agentModel != NULL)
    {
        connect(_agentModel, &AgentInMappingVM::inputsListWillBeRemoved, this, &IOPValueConditionM::onInputsListChange);
        connect(_agentModel, &AgentInMappingVM::inputsListAdded, this, &IOPValueConditionM::onInputsListChange);
        connect(_agentModel, &AgentInMappingVM::outputsListWillBeRemoved, this, &IOPValueConditionM::onOutputsListChange);
        connect(_agentModel, &AgentInMappingVM::outputsListAdded, this, &IOPValueConditionM::onOutputsListChange);
    }
}

/**
  * @brief Reset the agent connections for the action condition
  */
void IOPValueConditionM::resetConnections()
{
    if(_agentModel != NULL)
    {
        disconnect(_agentModel, &AgentInMappingVM::inputsListWillBeRemoved, this, &IOPValueConditionM::onInputsListChange);
        disconnect(_agentModel, &AgentInMappingVM::inputsListAdded, this, &IOPValueConditionM::onInputsListChange);
        disconnect(_agentModel, &AgentInMappingVM::outputsListWillBeRemoved, this, &IOPValueConditionM::onOutputsListChange);
        disconnect(_agentModel, &AgentInMappingVM::outputsListAdded, this, &IOPValueConditionM::onOutputsListChange);
    }
}


/**
  * @brief Slot on agent inputs list change
  */
void IOPValueConditionM::onInputsListChange(QList<InputVM*> inputsList)
{
    Q_UNUSED(inputsList)

    if(_agentModel != NULL && _agentIOP != NULL)
    {
        QString agentIopName = _agentIOP->name();
        AgentIOPM * newAgentIOP = NULL;

        // Fill with inputs
        foreach (InputVM* input, _agentModel->inputsList()->toList())
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
            setagentIOP(newAgentIOP);
        }
    }
}

/**
  * @brief Slot on agent outputs list change
  */
void IOPValueConditionM::onOutputsListChange(QList<OutputVM*> outputsList)
{
    Q_UNUSED(outputsList)

    if(_agentModel != NULL && _agentIOP != NULL)
    {
        QString agentIopName = _agentIOP->name();
        AgentIOPM * newAgentIOP = NULL;

        // Fill with outputs
        foreach (OutputVM* output, _agentModel->outputsList()->toList())
        {
            if(output->firstModel() != NULL)
            {
                if(newAgentIOP == NULL && agentIopName.isEmpty() == false && agentIopName == output->firstModel()->name())
                {
                    newAgentIOP = output->firstModel();
                }
            }
        }

        // Reset the agentIOP
        if(newAgentIOP != _agentIOP)
        {
            setagentIOP(newAgentIOP);
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

            // UnSubscribe to value change
            disconnect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);

            resetConnections();
        }
        setisValid(false);

        _agentIOP = agentIop;

        if(_agentIOP != NULL)
        {
            // Subscribe to destruction
            connect(_agentIOP, &AgentIOPM::destroyed, this, &IOPValueConditionM::_onAgentIopModelDestroyed);

            // Subscribe to value change
            connect(_agentIOP, &AgentIOPM::currentValueChanged, this, &IOPValueConditionM::_onCurrentValueChange);
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
        //agentIOPValueType : INTEGER , DOUBLE, STRING, BOOL, IMPULSION, DATA, MIXED, UNKNOWN
        switch(_agentIOP->agentIOPValueType())
        {
            case AgentIOPValueTypes::INTEGER :
            case AgentIOPValueTypes::DOUBLE :
            {
                double conditionDblValue = valueTrimmed.toDouble();
                switch(_comparison)
                {
                    case ActionComparisonValueType::INFERIOR_TO :
                    {
                        isValid = (currentValue.toDouble() < conditionDblValue);
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = (currentValue.toDouble() > conditionDblValue);
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = (conditionDblValue == currentValue.toDouble());
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
                        isValid = currentValue.toString().compare(valueTrimmed) < 0;
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = currentValue.toString().compare(valueTrimmed) > 0;
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = currentValue.toString().compare(valueTrimmed) == 0;
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
                        isValid = currentValue.toBool() < conditionBoolValue;
                        break;
                    }
                    case ActionComparisonValueType::SUPERIOR_TO :
                    {
                        isValid = currentValue.toBool() > conditionBoolValue;
                        break;
                    }
                    case ActionComparisonValueType::EQUAL_TO :
                    {
                        isValid = currentValue.toBool() == conditionBoolValue;
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
                qDebug() << "IopValueComparison could not be done for "<< AgentIOPValueTypes::staticEnumToString(_agentIOP->agentIOPValueType())<< " type : "<<_agentModel->name()<< "." << _agentIOP->name();
                break;
            }

        }
    }

    // Set final condition validation state
    setisValid(isValid);
}
