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
    // Clear our list
    _toAgentIopList.clear();
    _fromAgentIopList.clear();

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
        _toAgentIopList.clear();
        _toAgentIopList.append(mappingEffect->toAgentIopList()->toList());
        _fromAgentIopList.clear();
        _fromAgentIopList.append(mappingEffect->fromAgentIopList()->toList());

        setfromAgentIOP(mappingEffect->fromAgentIOP());
        settoAgentModel(mappingEffect->toAgentModel());
        settoAgentIOP(mappingEffect->toAgentIOP());
    }
}

/**
* @brief Custom setter on set agent model
*        to fill inputs and outputs
* @param agentModel
*/
void MappingEffectM::setagentModel(AgentInMappingVM* agentModel)
{
    AgentInMappingVM* previousAgentM = _agentModel;
    ActionEffectM::setagentModel(agentModel);

     if(previousAgentM != agentModel)
    {
        // Clear the list
        _fromAgentIopList.clear();
        setfromAgentIOP(NULL);

        if(_agentModel != NULL)
        {
            // Fill with outputs
            foreach (OutputVM* output, _agentModel->outputsList()->toList())
            {
                if(output->firstModel() != NULL)
                {
                    _fromAgentIopList.append(output->firstModel());
                }
            }

            // Select the first item
            if(_fromAgentIopList.count() > 0)
            {
                setfromAgentIOP(_fromAgentIopList.at(0));
            }
        }
    }
}

/**
* @brief Custom setter on set to agent model
*        to fill inputs and outputs
* @param agentModel
*/
void MappingEffectM::settoAgentModel(AgentInMappingVM* agentModel)
{
    if(_toAgentModel != agentModel)
    {
        if(_toAgentModel != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_toAgentModel, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onToAgentModelDestroyed);
        }

        // set the new value
        _toAgentModel = agentModel;

        // Clear the list
        _toAgentIopList.clear();
        settoAgentIOP(NULL);

        if(_toAgentModel != NULL)
        {
            // Fill with inputs
            foreach (InputVM* input, _toAgentModel->inputsList()->toList())
            {
                if(input->firstModel() != NULL)
                {
                    _toAgentIopList.append(input->firstModel());
                }
            }

            // Select the first item
            if(_toAgentIopList.count() > 0)
            {
                settoAgentIOP(_toAgentIopList.at(0));
            }

            if(_toAgentModel != NULL)
            {
                // Subscribe to destruction
                connect(_toAgentModel, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onToAgentModelDestroyed);
            }
        }

        emit toAgentModelChanged(_toAgentModel);
    }
}

/**
 * @brief Called when our "to" agent model is destroyed
 * @param sender
 */
void MappingEffectM::_onToAgentModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    settoAgentModel(NULL);
}

