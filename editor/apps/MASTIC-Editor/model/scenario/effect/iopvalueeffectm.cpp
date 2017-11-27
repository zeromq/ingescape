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
    _iopMergedList.clear();
}


/**
* @brief Copy from another effect model
* @param effect to copy
*/
void IOPValueEffectM::copyFrom(ActionEffectM* effect)
{
    // Call mother class
    ActionEffectM::copyFrom(effect);

    IOPValueEffectM* iopEffect = qobject_cast<IOPValueEffectM*>(effect);
    if (iopEffect != NULL)
    {
        setagentIOP(iopEffect->agentIOP());
        _iopMergedList.clear();
        _iopMergedList.append(iopEffect->iopMergedList()->toList());
        setvalue(iopEffect->value());
    }
}


/**
* @brief Custom setter on set agent to fill inputs and outputs
* @param agent
*/
void IOPValueEffectM::setagent(AgentInMappingVM* agent)
{
    AgentInMappingVM* previousAgent = _agent;

    // Call setter of mother class
    ActionEffectM::setagent(agent);

    if (previousAgent != agent)
    {
        setagentIOP(NULL);

        // Clear the list
        _iopMergedList.clear();

        if (_agent != NULL)
        {
            // Fill with inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if (input->firstModel() != NULL) {
                    _iopMergedList.append(input->firstModel());
                }
            }

            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if (output->firstModel() != NULL) {
                    _iopMergedList.append(output->firstModel());
                }
            }

            // Select the first item
            if (_iopMergedList.count() > 0) {
                setagentIOP(_iopMergedList.at(0));
            }
        }
    }
}


