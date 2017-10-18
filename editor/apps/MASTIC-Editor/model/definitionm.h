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
#include <model/iop/outputm.h>


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

    // Flag indicating if our definition is a variant (same name, same version but I/O/P differents)
    I2_QML_PROPERTY(bool, isVariant)

    // List of inputs of our agent definition
    I2_QOBJECT_LISTMODEL(AgentIOPM, inputsList)

    // List of outputs of our agent definition
    //I2_QOBJECT_LISTMODEL(AgentIOPM, outputsList)
    I2_QOBJECT_LISTMODEL(OutputM, outputsList)

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

    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param outputName
     */
    void commandAsked(QString command, QString outputName);


public Q_SLOTS:


private Q_SLOTS:
    /**
     * @brief Slot when the list of outputs changed
     */
    void _onOutputsListChanged();


    /**
     * @brief Slot when the flag "Is Muted" of an output changed
     * @param isMuted
     */
    //void _onIsMutedChanged(bool isMuted);


private:
    // Previous list of outputs
    QList<OutputM*> _previousOutputsList;

};

QML_DECLARE_TYPE(DefinitionM)

#endif // DEFINITIONM_H
