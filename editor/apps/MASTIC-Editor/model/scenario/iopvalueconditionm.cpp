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
bool IOPValueConditionM::setagentModel(AgentInMappingVM* agentModel)
{
    bool hasChanged = ActionConditionM::setagentModel(agentModel);

    if(hasChanged)
    {
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
        }
    }

    return hasChanged;
}


