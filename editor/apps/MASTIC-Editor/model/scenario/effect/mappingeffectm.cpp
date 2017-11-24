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

#include "mappingeffectm.h"


#include <QDebug>

/**
 * @brief Enum "MappingEffectValues" to string
 * @param value
 * @return
 */
QString MappingEffectValues::enumToString(int value)
{
    QString string = "Mapping Effect Value";

    switch (value)
    {
    case MappingEffectValues::MAPPED:
        string = "MAPPED";
        break;

    case MappingEffectValues::UNMAPPED:
        string = "UNMAPPED";
        break;

    default:
        break;
    }

    return string;
}


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
    _mappingEffectValue(MappingEffectValues::MAPPED),
    _output(NULL),
    _inputAgent(NULL),
    _toAgentIOP(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
MappingEffectM::~MappingEffectM()
{
    // Clear our list
    _toAgentIopList.clear();
    _fromAgentIopList.clear();

    // Reset output
    setoutput(NULL);

    // Reset input agent
    setinputAgent(NULL);

    // Reset TO agent iop
    settoAgentIOP(NULL);
}

/**
* @brief Copy from another effect model
* @param effect to copy
*/
void MappingEffectM::copyFrom(ActionEffectM *effect)
{
    // Call mother class
    ActionEffectM::copyFrom(effect);

    MappingEffectM* mappingEffect = qobject_cast<MappingEffectM*>(effect);
    if(mappingEffect != NULL)
    {
        _toAgentIopList.clear();
        _toAgentIopList.append(mappingEffect->toAgentIopList()->toList());
        _fromAgentIopList.clear();
        _fromAgentIopList.append(mappingEffect->fromAgentIopList()->toList());

        setoutput(mappingEffect->output());
        setinputAgent(mappingEffect->inputAgent());
        settoAgentIOP(mappingEffect->toAgentIOP());
    }
}


/**
* @brief Custom setter on set agent to fill inputs and outputs
* @param agent
*/
void MappingEffectM::setagent(AgentInMappingVM* agent)
{
    AgentInMappingVM* previousAgent = _agent;

    // Call setter of mother class
    ActionEffectM::setagent(agent);

    if (previousAgent != agent)
    {
        // Clear the list
        _fromAgentIopList.clear();
        setoutput(NULL);

        if(_agent != NULL)
        {
            // Fill with outputs
            foreach (OutputVM* output, _agent->outputsList()->toList())
            {
                if(output->firstModel() != NULL)
                {
                    _fromAgentIopList.append(output->firstModel());
                }
            }

            // Select the first item
            if(_fromAgentIopList.count() > 0)
            {
                setoutput(_fromAgentIopList.at(0));
            }
        }
    }
}


/**
* @brief Custom setter for property "input agent" to fill inputs and outputs
* @param value
*/
void MappingEffectM::setinputAgent(AgentInMappingVM* value)
{
    if(_inputAgent != value)
    {
        if(_inputAgent != NULL)
        {
            // UnSubscribe to destruction
            disconnect(_inputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onInputAgentDestroyed);
        }

        // set the new value
        _inputAgent = value;

        // Clear the list
        _toAgentIopList.clear();
        settoAgentIOP(NULL);

        if (_inputAgent != NULL)
        {
            // Fill with inputs
            foreach (InputVM* input, _inputAgent->inputsList()->toList())
            {
                if (input->firstModel() != NULL) {
                    _toAgentIopList.append(input->firstModel());
                }
            }

            // Select the first item
            if (_toAgentIopList.count() > 0) {
                settoAgentIOP(_toAgentIopList.at(0));
            }

            if(_inputAgent != NULL)
            {
                // Subscribe to destruction
                connect(_inputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onInputAgentDestroyed);
            }
        }

        emit inputAgentChanged(value);
    }
}


/**
 * @brief Called when our "input agent" is destroyed
 * @param sender
 */
void MappingEffectM::_onInputAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setinputAgent(NULL);
}

