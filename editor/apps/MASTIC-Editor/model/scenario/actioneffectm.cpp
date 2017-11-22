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
 * @brief Called when our agent model is destroyed
 * @param sender
 */
void ActionEffectM::_onAgentModelDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setagentModel(NULL);
}

/**
* @brief Custom setter for agent model
* @param agent model
*/
void ActionEffectM::setagentModel(AgentInMappingVM* agentModel)
{
    if(_agentModel != agentModel)
    {
        if(_agentModel != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_agentModel, &AgentInMappingVM::destroyed, this, &ActionEffectM::_onAgentModelDestroyed);
        }

        _agentModel = agentModel;

        if(_agentModel != NULL)
        {
            // Subscribe to destruction
            connect(_agentModel, &AgentInMappingVM::destroyed, this, &ActionEffectM::_onAgentModelDestroyed);
        }

        Q_EMIT agentModelChanged(agentModel);
    }
}


