/*
 *	ActionEffectM
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

#include "actioneffectm.h"


#include <QDebug>
#include "iopvalueeffectm.h"

/**
 * @brief Effect type for an action
 * @param value
 * @return
 */
QString ActionEffectValueType::enumToString(int value)
{
    QString string = "Effect type";

    switch (value) {
    case ActionEffectValueType::ENABLE:
        string = "Enable";
        break;

    case ActionEffectValueType::DISABLE:
        string = "Disable";
        break;

    case ActionEffectValueType::ON:
        string = "ON";
        break;

    case ActionEffectValueType::OFF:
        string = "OFF";
        break;

    default:
        break;
    }

    return string;
}

//--------------------------------------------------------------
//
// ActionEffectM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionEffectM::ActionEffectM(QObject *parent) : QObject(parent),
    _agentModel(NULL),
    _effect(ActionEffectValueType::ON)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
ActionEffectM::~ActionEffectM()
{
    // Reset agent model to null
    setagentModel(NULL);
}

/**
* @brief Copy from another effect model
* @param effect to copy
*/
void ActionEffectM::copyFrom(ActionEffectM* effect)
{
    if(effect != NULL)
    {
        setagentModel(effect->agentModel());
        seteffect(effect->effect());
    }
}

/**
* @brief Custom setter on the agent model
* @param agent
*/
void ActionEffectM::setagentModel(AgentInMappingVM* agentM)
{
    if(_agentModel != agentM)
    {
        _agentModel = agentM;

        IOPValueEffectM* iopEffect = dynamic_cast<IOPValueEffectM*>(this);
        if(iopEffect != NULL)
        {
            if(_agentModel != NULL)
            {
                // Clear the list
                iopEffect->agentIopList()->clear();

                // Fill with inputs
                foreach (InputVM* input, _agentModel->inputsList()->toList())
                {
                    if(input->firstModel() != NULL)
                    {
                        iopEffect->agentIopList()->append(input->firstModel());
                    }
                }

                // Fill with outputs
                foreach (OutputVM* output, _agentModel->outputsList()->toList())
                {
                    if(output->firstModel() != NULL)
                    {
                        iopEffect->agentIopList()->append(output->firstModel());
                    }
                }

                // Select the first item
                if(iopEffect->agentIopList()->count() > 0)
                {
                    iopEffect->setagentIOP(iopEffect->agentIopList()->at(0));
                }
            }

        }

        emit agentModelChanged(agentM);
    }
}

