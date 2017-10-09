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

#ifndef DEFINITIONM_H
#define DEFINITIONM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/iop/agentiopm.h>


/**
 * @brief The DefinitionM class defines a model of definition for an agent
 */
class DefinitionM : public QObject
{
    Q_OBJECT

    // Name of our agent definition
    I2_QML_PROPERTY(QString, name)

    // Version of our agent definition
    I2_QML_PROPERTY(QString, version)

    // Description of our agent definition
    I2_QML_PROPERTY(QString, description)

    // Md5 hash value for the definition string
    I2_QML_PROPERTY_READONLY(QString, md5Hash)

    // Variant of our agent definition
    I2_QML_PROPERTY(QString, variant)

    // List of inputs of our agent definition
    I2_QOBJECT_LISTMODEL(AgentIOPM, inputsList)

    // List of outputs of our agent definition
    I2_QOBJECT_LISTMODEL(AgentIOPM, outputsList)

    // List of parameters of our agent definition
    I2_QOBJECT_LISTMODEL(AgentIOPM, parametersList)


public:
    /**
     * @brief Constructor
     * @param name
     * @param version
     * @param description
     * @param parent
     */
    explicit DefinitionM(QString name, QString version, QString description, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~DefinitionM();

Q_SIGNALS:

public Q_SLOTS:
};

QML_DECLARE_TYPE(DefinitionM)

#endif // DEFINITIONM_H
