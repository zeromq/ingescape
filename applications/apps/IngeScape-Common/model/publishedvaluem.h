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

#ifndef PUBLISHEDVALUEM_H
#define PUBLISHEDVALUEM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <model/enums.h>


/**
 * @brief The PublishedValueM class defines a model when a new value is published on an agent output
 */
class PublishedValueM : public QObject
{
    Q_OBJECT

    // Date and time when the value has been published
    I2_QML_PROPERTY_READONLY(QDateTime, time)

    // Name of the agent
    I2_QML_PROPERTY_READONLY(QString, agentName)

    // Type of the agent sub part: Input, Output or Parameter
    I2_QML_PROPERTY_READONLY(AgentIOPTypes::Value, iopType)

    // Identifier with name and value type
    I2_CPP_NOSIGNAL_PROPERTY(QString, iopId)

    // Value type of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, iopValueType)

    // Group of the value type of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypeGroups::Value, iopValueTypeGroup)

    // Name of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(QString, iopName)

    // Value of the Input / Output / Parameter
    I2_CPP_PROPERTY(QVariant, value)

    // Displayable value of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(QString, displayableValue)


public:
    /**
     * @brief
     * @param time
     * @param agentName
     * @param iopId
     * @param iopValueType
     * @param value
     * @param iopType
     * @param parent
     */
    explicit PublishedValueM(QDateTime time,
                             QString agentName,
                             QString iopId,
                             AgentIOPValueTypes::Value iopValueType,
                             QVariant value,
                             AgentIOPTypes::Value iopType = AgentIOPTypes::OUTPUT,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~PublishedValueM();

};

QML_DECLARE_TYPE(PublishedValueM)

#endif // PUBLISHEDVALUEM_H
