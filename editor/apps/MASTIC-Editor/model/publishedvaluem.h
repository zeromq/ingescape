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

    // Name of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(QString, iopName)

    // Value type of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(AgentIOPValueTypes::Value, iopValueType)

    // Identifier with name and value type
    I2_CPP_NOSIGNAL_PROPERTY(QString, id)

    // Value of the Input / Output / Parameter
    I2_CPP_PROPERTY(QVariant, value)

    // Displayable value of the Input / Output / Parameter
    I2_QML_PROPERTY_READONLY(QString, displayableValue)


public:
    /**
     * @brief Constructor
     * @param time
     * @param agentName
     * @param iopName
     * @param iopValueType
     * @param value
     * @param parent
     */
    explicit PublishedValueM(QDateTime time,
                             QString agentName,
                             QString iopName,
                             AgentIOPValueTypes::Value iopValueType,
                             QVariant value,
                             QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~PublishedValueM();


Q_SIGNALS:


public Q_SLOTS:
};

QML_DECLARE_TYPE(PublishedValueM)

#endif // PUBLISHEDVALUEM_H
