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

#include "agentiopm.h"

#include <QQmlEngine>
#include <QDebug>


/**
 * @brief Default constructor
 * @param parent
 */
AgentIOPM::AgentIOPM(QObject *parent) : QObject(parent),
    _agentIOPType(AgentIOPTypes::PARAMETER),
    _name(""),
    _agentIOPValueType(AgentIOPValueTypes::STRING),
    _isMuted(false),
    _defaultValue(QVariant()),
    _displayableDefaultValue("")
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
 * @brief Destructor
 */
AgentIOPM::~AgentIOPM()
{

}


/**
 * @brief Mute/UN-mute our Output
 * @param mute
 */
void AgentIOPM::updateMuteOutput(bool mute)
{
    if (mute) {
        Q_EMIT commandAsked("MUTE", _name);
    }
    else {
        Q_EMIT commandAsked("UNMUTE", _name);
    }

    // FIXME
    setisMuted(mute);
}
