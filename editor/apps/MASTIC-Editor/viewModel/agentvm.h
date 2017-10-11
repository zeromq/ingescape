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

#ifndef AGENTVM_H
#define AGENTVM_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <model/agentm.h>
#include <model/definitionm.h>
#include <viewModel/iop/agentiopvm.h>


/**
 * @brief The AgentVM class defines a view model of agent
 * Allows to manage when several agents have exactly the same name and the same definition
 * Only Peer ID is different (and HostName can also be different)
 */
class AgentVM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, name)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // Address(es) on the network of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, addresses)

    // Model of the agent definition
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(DefinitionM*, definition)

    // Flag indicating if our agent is fictitious (never yet appeared on the network)
    I2_QML_PROPERTY_READONLY(bool, isFictitious)

    // List of VM of inputs
    I2_QOBJECT_LISTMODEL(AgentIOPVM, inputsList)

    // List of VM of outputs
    I2_QOBJECT_LISTMODEL(AgentIOPVM, outputsList)

    // List of VM of parameters
    I2_QOBJECT_LISTMODEL(AgentIOPVM, parametersList)

    // Status: can be ON, OFF, ON Asked or OFF Asked
    I2_QML_PROPERTY(AgentStatus::Value, status)

    // State string defined by the agent
    I2_QML_PROPERTY(QString, state)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY(bool, isFrozen)


public:
    /**
     * @brief Default constructor
     * @param model
     * @param parent
     */
    explicit AgentVM(AgentM* model, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentVM();


    /**
     * @brief Mute our agent
     */
    Q_INVOKABLE void mute();


    /**
     * @brief Freeze our agent
     */
    Q_INVOKABLE void freeze();



Q_SIGNALS:

public Q_SLOTS:

    /**
     * @brief Slot when the list of models changed
     */
    void onModelsChanged();


    /**
     * @brief Slot when the "Status" of a model changed
     * @param status
     */
    void onModelStatusChanged(AgentStatus::Value status);


private:
    /**
     * @brief Update with the list of models
     */
    void _updateWithModels();


    /**
     * @brief Update the status in function of status of models
     */
    void _updateStatus();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

};

QML_DECLARE_TYPE(AgentVM)

#endif // AGENTVM_H
