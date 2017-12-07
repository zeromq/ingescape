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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "outputm.h"

/**
 * @brief Constructor
 * @param name
 * @param agentIOPValueType
 * @param parent
 */
OutputM::OutputM(QString name,
                 AgentIOPValueTypes::Value agentIOPValueType,
                 QObject *parent) : AgentIOPM(AgentIOPTypes::OUTPUT,
                                              name,
                                              agentIOPValueType,
                                              parent),
    _isMuted(false),
    _isMutedOutput(false),
    _isMutedAllOutputs(false)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
OutputM::~OutputM()
{

}


/**
 * @brief Setter for property "is Muted Output"
 * @param value
 */
void OutputM::setisMutedOutput(bool value)
{
    if (_isMutedOutput != value)
    {
        _isMutedOutput = value;

        // Update the flag "Is Muted" (for feedbacks on HMI)
        _updateIsMuted();

        Q_EMIT isMutedOutputChanged(value);
    }
}


/**
 * @brief Setter for property "is Muted All Outputs"
 * @param value
 */
void OutputM::setisMutedAllOutputs(bool value)
{
    if (_isMutedAllOutputs != value)
    {
        _isMutedAllOutputs = value;

        // Update the flag "Is Muted" (for feedbacks on HMI)
        _updateIsMuted();

        Q_EMIT isMutedAllOutputsChanged(value);
    }
}


/**
 * @brief Mute / UN-mute our output
 */
void OutputM::changeMuteOutput()
{
    if (_isMuted) {
        Q_EMIT commandAsked("UNMUTE", _name);
    }
    else {
        Q_EMIT commandAsked("MUTE", _name);
    }
}


/**
 * @brief Update the flag "Is Muted" (for feedbacks on HMI)
 */
void OutputM::_updateIsMuted()
{
    if (_isMutedOutput || _isMutedAllOutputs) {
        setisMuted(true);
    }
    else {
        setisMuted(false);
    }
}
