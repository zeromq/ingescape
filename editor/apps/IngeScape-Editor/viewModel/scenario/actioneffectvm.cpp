/*
 *	IngeScape Editor
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

#include "actioneffectvm.h"

#include <QDebug>

/**
 * @brief Enum "ActionEffectTypes" to string
 * @param value
 * @return
 */
QString ActionEffectTypes::enumToString(int value)
{
    switch (value)
    {
    case ActionEffectTypes::VALUE:
        return tr("Value");

    case ActionEffectTypes::AGENT:
        return tr("Agent");

    case ActionEffectTypes::MAPPING:
        return tr("Mapping");

    default:
        return "";
    }
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
    _modelM(NULL),
    _effectType(ActionEffectTypes::VALUE),
    _secondAgentForMapping(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
ActionEffectVM::~ActionEffectVM()
{
    // reset pointer
    setsecondAgentForMapping(nullptr);

    if (_modelM != NULL)
    {
        ActionEffectM* tmp = _modelM;
        setmodelM(nullptr);
        delete tmp;
        tmp = NULL;
    }
}


/**
 * @brief Custom setter on the effect type
 */
void ActionEffectVM::seteffectType(ActionEffectTypes::Value value)
{
    if(_effectType != value)
    {
        _effectType = value;

        // configure the new type
        _configureToType(value);

        Q_EMIT effectTypeChanged(value);
    }
}


/**
 * @brief Configure action effect VM into a specific type
 */
void ActionEffectVM::_configureToType(ActionEffectTypes::Value effectType)
{
    AgentsGroupedByNameVM* agent = nullptr;

    // Delete the old model of effect if exists
    if (_modelM != nullptr)
    {
        // Save the agent
        agent = _modelM->agent();

        ActionEffectM* tmp = _modelM;
        setmodelM(nullptr);
        delete tmp;
    }

    // Create the new model of effect
    switch (effectType)
    {
        case ActionEffectTypes::AGENT: {
            setmodelM(new EffectOnAgentM());
            _modelM->setagent(agent);
            break;
        }
        case ActionEffectTypes::VALUE: {
            setmodelM(new IOPValueEffectM());
            _modelM->setagent(agent);
            break;
        }
        case ActionEffectTypes::MAPPING: {
            MappingEffectM* mappingEffect = new MappingEffectM();
            setmodelM(mappingEffect);
            _modelM->setagent(agent);

            if (_secondAgentForMapping != nullptr) {
                mappingEffect->setoutputAgent(_secondAgentForMapping);
            }
            else {
                mappingEffect->setoutputAgent(agent);
            }
            break;
        }
        default:
            break;
    }
}


