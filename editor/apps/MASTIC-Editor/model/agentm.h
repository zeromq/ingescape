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

#ifndef AGENTM_H
#define AGENTM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>



/**
 * @brief The AgentM class defines a model of agent
 */
class AgentM : public QObject
{
    Q_OBJECT

    // Name of our agent (unique identifier)
    I2_QML_PROPERTY(QString, name)

    // Network device of our agent
    I2_QML_PROPERTY_READONLY(QString, networkDevice)

    // IP address of our agent
    I2_QML_PROPERTY_READONLY(QString, ipAddress)

    // Peer of our agent
    I2_QML_PROPERTY_READONLY(QString, peer)

    // Port of our agent
    I2_QML_PROPERTY_READONLY(int, port)

    // Version of our agent
    I2_QML_PROPERTY(QString, version)

    // Description of our agent
    I2_QML_PROPERTY(QString, description)

    // List of inputs of our agent
    I2_QOBJECT_LISTMODEL(AgentIOPM, inputsList)

    // List of outputs of our agent
    I2_QOBJECT_LISTMODEL(AgentIOPM, outputsList)

    // List of parameters of our agent
    I2_QOBJECT_LISTMODEL(AgentIOPM, parametersList)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Md5 hash value for the definition string
    I2_QML_PROPERTY_READONLY(QString, md5Hash)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AgentM(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(AgentM)

#endif // AGENTM_H
