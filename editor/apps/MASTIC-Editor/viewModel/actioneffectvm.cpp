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

#include "actioneffectvm.h"

#include <QDebug>

/**
 * @brief Action effect type
 * @param value
 * @return
 */
QString ActionEffectTypes::enumToString(int value)
{
    QString string = "Action effect type";

    switch (value) {
    case ActionEffectTypes::VALUE:
        string = "Value";
        break;

    case ActionEffectTypes::AGENT:
        string = "Agent";
        break;

    case ActionEffectTypes::MAPPING:
        string = "Mapping";
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
    _modelM(NULL),
    _effectType(ActionEffectTypes::VALUE),
    _secondAgentInMapping(NULL)
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
    setsecondAgentInMapping(NULL);

    if (_modelM != NULL)
    {
        ActionEffectM* tmp = _modelM;
        setmodelM(NULL);
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

        emit effectTypeChanged(value);
    }
}


/**
 * @brief Configure action effect VM into a specific type
 */
void ActionEffectVM::_configureToType(ActionEffectTypes::Value effectType)
{
    AgentInMappingVM* agent = NULL;

    // Delete the old model of effect if exists
    if(_modelM != NULL)
    {
        // Save the agent
        agent = _modelM->agent();

        ActionEffectM* tmp = _modelM;
        setmodelM(NULL);
        delete tmp;
        tmp = NULL;
    }

    // Create the new type effect
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

            if (_secondAgentInMapping != NULL) {
                mappingEffect->setoutputAgent(_secondAgentInMapping);
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


