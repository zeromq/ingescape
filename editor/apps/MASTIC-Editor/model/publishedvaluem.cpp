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

    QStringList iopNameAndValueType = _iopId.split(SEPARATOR_IOP_NAME_AND_IOP_VALUE_TYPE);
    if (iopNameAndValueType.count() == 2)
    {
        QString name = iopNameAndValueType.at(0);
        QString valueType = iopNameAndValueType.at(1);

        if (valueType == AgentIOPValueTypes::staticEnumToString(_iopValueType)) {
            _iopName = name;
        }
        else {
            qCritical() << "The type of the value" << AgentIOPValueTypes::staticEnumToString(_iopValueType) << "must be included in the id" << _iopId;
        }
    }

    // Get a displayable value: convert a variant into a string (in function of the value type)
    _displayableValue = Enums::getDisplayableValue(_iopValueType, _value);

    qDebug() << "New Published Value at" << _time.toString("dd/MM/yy hh:mm:ss.zzz") << "on" << AgentIOPTypes::staticEnumToString(_iopType) << "of" << _agentName << "." << _iopName << "with type" << AgentIOPValueTypes::staticEnumToString(_iopValueType) << ":" << _displayableValue;
}


/**
 * @brief Destructor
 */
PublishedValueM::~PublishedValueM()
{

}

