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

#include "publishedvaluem.h"

/**
 * @brief Constructor
 * @param time
 * @param agentName
 * @param iopName
 * @param iopValueType
 * @param value
 * @param parent
 */
PublishedValueM::PublishedValueM(QDateTime time,
                                 QString agentName,
                                 QString iopName,
                                 AgentIOPValueTypes::Value iopValueType,
                                 QVariant value,
                                 QObject *parent) : QObject(parent),
    _time(time),
    _agentName(agentName),
    _iopName(iopName),
    _iopValueType(iopValueType),
    _value(value)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Create the unique identifier with name and value type
    _id = QString("%1::%2").arg(_iopName, AgentIOPValueTypes::staticEnumToString(_iopValueType));

    // To String
    _displayableValue = _value.toString();
}


/**
 * @brief Destructor
 */
PublishedValueM::~PublishedValueM()
{

}

