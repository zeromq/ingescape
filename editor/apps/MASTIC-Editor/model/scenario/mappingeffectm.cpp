/*
 *	MappingEffectM
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

#include "mappingeffectm.h"


#include <QDebug>



//--------------------------------------------------------------
//
// MappingEffectM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
MappingEffectM::MappingEffectM(QObject *parent) : ActionEffectM(parent),
    _fromAgentIOP(NULL),
    _toAgentModel(NULL),
    _toAgentIOP(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Initialize effect
    seteffect(ActionEffectValueType::ENABLE);
}


/**
 * @brief Destructor
 */
MappingEffectM::~MappingEffectM()
{
    // Reset FROM agent model
    setfromAgentIOP(NULL);

    // Reset TO agent model
    settoAgentModel(NULL);

    // Reset TO agent iop
    settoAgentIOP(NULL);
}

/**
* @brief Copy from another effect model
* @param effect to copy
*/
void MappingEffectM::copyFrom(ActionEffectM *effect)
{
    ActionEffectM::copyFrom(effect);

    MappingEffectM* mappingEffect = dynamic_cast<MappingEffectM*>(effect);
    if(mappingEffect != NULL)
    {
        setfromAgentIOP(mappingEffect->fromAgentIOP());
        settoAgentIOP(mappingEffect->toAgentIOP());
        settoAgentModel(mappingEffect->toAgentModel());
    }
}

