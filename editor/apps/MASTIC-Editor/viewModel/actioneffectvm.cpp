/*
 *	ActionEffectVM
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

#include "actioneffectvm.h"


#include <QDebug>

/**
 * @brief Action effect type
 * @param value
 * @return
 */
QString ActionEffectType::enumToString(int value)
{
    QString string = "Action effect type";

    switch (value) {
    case ActionEffectType::VALUE:
        string = "Value";
        break;

    case ActionEffectType::STATUS:
        string = "Status";
        break;

    case ActionEffectType::LINK:
        string = "Link";
        break;

    default:
        break;
    }

    return string;
}

//--------------------------------------------------------------
//
// ActionEffectVM
//
//--------------------------------------------------------------


/**
 * @brief Default constructor
 * @param parent
 */
ActionEffectVM::ActionEffectVM(QObject *parent) : QObject(parent),
    _effect(NULL),
    _effectType(ActionEffectType::STATUS)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Create an initial action condition
    seteffect(new ActionEffectM(this));
}


/**
 * @brief Destructor
 */
ActionEffectVM::~ActionEffectVM()
{
    if(_effect != NULL)
    {
        ActionEffectM* tmp = _effect;
        seteffect(NULL);
        delete tmp;
        tmp = NULL;
    }
}

/**
 * @brief Custom setter on the effect type
 */
void ActionEffectVM::seteffectType(ActionEffectType::Value value)
{
    if(_effectType != value)
    {
        _effectType = value;

        // configure the new type
        _configureToType(value);

        emit effectTypeChanged(value);
    }
}

/**
 * @brief Configure action effect VM into a specific type
 */
void ActionEffectVM::_configureToType(ActionEffectType::Value value)
{
    AgentM* agent = NULL;

    // Delete the old effect if exists
    if(_effect != NULL)
    {
        // Save the agent
        agent = _effect->agentModel();

        ActionEffectM* tmp = _effect;
        seteffect(NULL);
        delete tmp;
        tmp = NULL;
    }

    // Create the new type effect
    switch (value)
    {
        case ActionEffectType::STATUS :
        {
            seteffect(new ActionEffectM());
            _effect->setagentModel(agent);
            break;
        }

        case ActionEffectType::VALUE :
        {
            seteffect(new IOPValueEffectM());
            _effect->setagentModel(agent);
            break;
        }

        case ActionEffectType::LINK :
        {
            seteffect(new MappingEffectM());
            _effect->setagentModel(agent);
            break;
        }
        default:
            break;
    }
}


