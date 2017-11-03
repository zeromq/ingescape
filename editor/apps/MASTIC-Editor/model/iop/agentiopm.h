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

#ifndef AGENTIOPM_H
#define AGENTIOPM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>


/**
  * Types:
  * - Agent Input
  * - Agent Output
  * - Agent Parameter
  */
I2_ENUM(AgentIOPTypes, INPUT, OUTPUT, PARAMETER)


/**
  * Type of the value of an Agent Input / Output / Parameter
  */
I2_ENUM(AgentIOPValueTypes, INTEGER = 1, DOUBLE, STRING, BOOL, IMPULSION, DATA, MIXED, UNKNOWN)


/**
 * @brief The AgentIOPM is the base class for derived class of Model of agent Input / Output / Parameter
 */
class AgentIOPM : public QObject
{
    Q_OBJECT

    // Type of our agent sub part: Input, Output or Parameter
    I2_QML_PROPERTY_READONLY(AgentIOPTypes::Value, agentIOPType)

    // Name of our Input / Output / Parameter
    I2_QML_PROPERTY(QString, name)

    // Value type of our Input / Output / Parameter
    I2_QML_PROPERTY(AgentIOPValueTypes::Value, agentIOPValueType)

    // Identifier with name and value type
    I2_CPP_PROPERTY(QString, id)

    // ######################################################
    // Value of our Input / Output / Parameter
    // Store the value in a Byte Array or in a Variant ?
    // http://doc.qt.io/qt-5/qbytearray.html
    // http://doc.qt.io/qt-5/qvariant.html

    // Default value of our Input / Output / Parameter
    //I2_CPP_PROPERTY(QByteArray, defaultValue)
    I2_CPP_PROPERTY_CUSTOM_SETTER(QVariant, defaultValue)
    // ######################################################

    // Displayable defaut value of our Input / Output / Parameter
    I2_QML_PROPERTY(QString, displayableDefaultValue)

    // Mapping value of our Input / Output / Parameter
    //I2_CPP_PROPERTY(QVariant, mappingValue)

    // Displayable mapping value of our Input / Output / Parameter
    //I2_QML_PROPERTY(QString, displayableMappingValue)

    // current value of our Input / Output / Parameter
    //I2_CPP_PROPERTY(QVariant, currentValue)

    // Displayable current value of our Input / Output / Parameter
    //I2_QML_PROPERTY(QString, displayableCurrentValue)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AgentIOPM(QObject *parent = nullptr);


    /**
     * @brief Constructor
     * @param agentIOPType
     * @param name
     * @param agentIOPValueType
     * @param parent
     */
    AgentIOPM(AgentIOPTypes::Value agentIOPType,
              QString name,
              AgentIOPValueTypes::Value agentIOPValueType,
              QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentIOPM();


Q_SIGNALS:


public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentIOPM)

#endif // AGENTIOPM_H
