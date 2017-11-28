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
    _outputAgent(NULL),
    _output(NULL),
    _input(NULL)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}


/**
 * @brief Destructor
 */
MappingEffectM::~MappingEffectM()
{
    // Clear our lists
    _inputsList.clear();
    _outputsList.clear();

    // Reset output agent
    setoutputAgent(NULL);

    // Reset output
    setoutput(NULL);

    // Reset input
    setinput(NULL);
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
        _inputsList.clear();
        _inputsList.append(mappingEffect->inputsList()->toList());
        _outputsList.clear();
        _outputsList.append(mappingEffect->outputsList()->toList());

        setoutputAgent(mappingEffect->outputAgent());
        setoutput(mappingEffect->output());
        setinput(mappingEffect->input());

        setmappingEffectValue(mappingEffect->mappingEffectValue());
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

    if (previousAgent != _agent)
    {
        // Clear the inputs list and the selected input
        _inputsList.clear();
        setinput(NULL);

        if (_agent != NULL)
        {
            // Fill inputs
            foreach (InputVM* input, _agent->inputsList()->toList())
            {
                if ((input != NULL) && (input->firstModel() != NULL))
                {
                    _inputsList.append(input->firstModel());
                }
            }

            // Select the first item
            if (_inputsList.count() > 0) {
                setinput(_inputsList.at(0));
            }
        }
    }
}


/**
* @brief Custom setter for property "output agent" to fill inputs and outputs
* @param value
*/
void MappingEffectM::setoutputAgent(AgentInMappingVM* value)
{
    if (_outputAgent != value)
    {
        if (_outputAgent != NULL)
        {
            // UN-subscribe to destruction
            disconnect(_outputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);
        }

        // set the new value
        _outputAgent = value;

        // Clear the list and the selected output
        _outputsList.clear();
        setoutput(NULL);

        if (_outputAgent != NULL)
        {
            // Subscribe to destruction
            connect(_outputAgent, &AgentInMappingVM::destroyed, this, &MappingEffectM::_onOutputAgentDestroyed);

            foreach (OutputVM* output, _outputAgent->outputsList()->toList())
            {
                if ((output != NULL) && (output->firstModel() != NULL)) {
                    _outputsList.append(output->firstModel());
                }
            }

            // Select the first item
            if (_outputsList.count() > 0) {
                setoutput(_outputsList.at(0));
            }
        }

        Q_EMIT outputAgentChanged(value);
    }
}


/**
 * @brief Called when our "output agent" is destroyed
 * @param sender
 */
void MappingEffectM::_onOutputAgentDestroyed(QObject* sender)
{
    Q_UNUSED(sender)

    setoutputAgent(NULL);
}

