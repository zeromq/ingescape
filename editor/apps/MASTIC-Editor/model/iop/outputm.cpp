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
    _isMuted(false)
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
