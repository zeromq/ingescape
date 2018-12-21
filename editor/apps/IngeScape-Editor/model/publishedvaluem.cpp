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

#include "publishedvaluem.h"
#include <model/iop/agentiopm.h>

/**
 * @brief Constructor
 * @param time
 * @param agentName
 * @param iopId
 * @param iopValueType
 * @param value
 * @param parent
 */
PublishedValueM::PublishedValueM(QDateTime time,
                                 QString agentName,
                                 QString iopId,
                                 AgentIOPValueTypes::Value iopValueType,
                                 QVariant value,
                                 AgentIOPTypes::Value iopType,
                                 QObject *parent) : QObject(parent),
    _time(time),
    _agentName(agentName),
    _iopType(iopType),
    _iopId(iopId),
    _iopValueType(iopValueType),
    _iopName(""),
    _value(value)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Set the corresponding group
    _iopValueTypeGroup = Enums::getGroupForAgentIOPValueType(_iopValueType);

    // Get the name and the value type of an agent I/O/P from its id
    QPair<QString, AgentIOPValueTypes::Value> pair = AgentIOPM::getNameAndValueTypeFromId(_iopId);

    if (!pair.first.isEmpty() && (pair.second != AgentIOPValueTypes::UNKNOWN))
    {
        _iopName = pair.first;
        //_iopValueType = pair.second;
    }
    else
    {
        qCritical() << "The type of the value" << AgentIOPValueTypes::staticEnumToString(_iopValueType) << "must be included in the id" << _iopId;
    }

    // Get a displayable value: convert a variant into a string (in function of the value type)
    //_displayableValue = Enums::getDisplayableValue(_iopValueType, _value);

    _displayableValue = _value.toString();

    //qDebug() << "New Published Value at" << _time.toString("dd/MM/yy hh:mm:ss.zzz") << "on" << AgentIOPTypes::staticEnumToString(_iopType) << "of" << _agentName << "." << _iopName << "with type" << AgentIOPValueTypes::staticEnumToString(_iopValueType) << ":" << _displayableValue;
}


/**
 * @brief Destructor
 */
PublishedValueM::~PublishedValueM()
{

}

