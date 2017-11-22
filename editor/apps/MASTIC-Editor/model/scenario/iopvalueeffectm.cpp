/*
 *	IOPValueEffectM
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
    _agentIopList.clear();
}

/**
* @brief Copy from another effect model
* @param effect to copy
*/
void IOPValueEffectM::copyFrom(ActionEffectM *effect)
{
    ActionEffectM::copyFrom(effect);

    IOPValueEffectM* iopEffect = dynamic_cast<IOPValueEffectM*>(effect);
    if(iopEffect != NULL)
    {
        setagentIOP(iopEffect->agentIOP());
        _agentIopList.clear();
        _agentIopList.append(iopEffect->agentIopList()->toList());
        setvalue(iopEffect->value());
    }
}

/**
* @brief Custom setter on set agent model
*        to fill inputs and outputs
* @param agentModel
*/
void IOPValueEffectM::setagentModel(AgentInMappingVM* agentModel)
{
    AgentInMappingVM* previousAgentM = _agentModel;
    ActionEffectM::setagentModel(agentModel);

    if(previousAgentM != agentModel)
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
}


