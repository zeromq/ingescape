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
I2_ENUM(AgentIOPValueTypes, INTEGER, DOUBLE, STRING, BOOL, IMPULSION, DATA)



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

    // FIXME: create a class OutputM that inherits from AgentIOPM with this property
    // Flag indicating if our Output is muted
    I2_QML_PROPERTY(bool, isMuted)

    // ######################################################
    // Value of our Input / Output / Parameter
    // Store the value in a Byte Array or in a Variant ?
    // http://doc.qt.io/qt-5/qbytearray.html
    // http://doc.qt.io/qt-5/qvariant.html

    //I2_CPP_PROPERTY(QByteArray, defaultValueByteArray)
    //I2_CPP_PROPERTY(QVariant, defaultValueVariant)

    // Default value of our Input / Output / Parameter
    I2_CPP_PROPERTY(QVariant, defaultValue)

    // Displayable defaut value of our Input / Output / Parameter
    I2_QML_PROPERTY(QString, displayableDefaultValue)
    // ######################################################


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AgentIOPM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentIOPM();


    /**
     * @brief Mute/UN-mute our Output
     * @param mute
     */
    Q_INVOKABLE void updateMuteOutput(bool mute);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param outputName
     */
    void commandAsked(QString command, QString outputName);


public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentIOPM)

#endif // AGENTIOPM_H
