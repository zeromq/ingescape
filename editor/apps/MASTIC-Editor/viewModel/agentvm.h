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
#include <QColor>

#include <I2PropertyHelpers.h>

#include "model/agentm.h"
#include "model/definitionm.h"
#include "model/scenario/actionconditionm.h"
#include "model/scenario/actioneffectm.h"


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

    // Flag indicating if our agent has only a definition (never yet appeared on the network)
    I2_QML_PROPERTY_READONLY(bool, hasOnlyDefinition)

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
     * @brief Mute/UN-mute all outputs of our agent
     * @param muteAllOutputs
     */
    Q_INVOKABLE void updateMuteAllOutputs(bool muteAllOutputs);


    /**
     * @brief Freeze/UN-freeze our agent
     * @param freeze
     */
    Q_INVOKABLE void updateFreeze(bool freeze);


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network
     * @param command
     * @param peerIdsList
     */
    void commandAsked(QString command, QStringList peerIdsList);


    /**
     * @brief Signal emitted when a command for an output must be sent on the network
     * @param command
     * @param outputName
     * @param peerIdsList
     */
    void commandAskedForOutput(QString command, QString outputName, QStringList peerIdsList);


public Q_SLOTS:

    /**
     * @brief Slot when a command from an output must be sent on the network
     * @param command
     * @param outputName
     */
    void onCommandAskedForOutput(QString command, QString outputName);


private Q_SLOTS:
    /**
     * @brief Slot when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot when the "Status" of a model changed
     * @param status
     */
    void _onStatusOfModelChanged(AgentStatus::Value status);


    /**
     * @brief Slot when the flag "Is Muted" of a model changed
     * @param isMuted
     */
    void _onIsMutedOfModelChanged(bool isMuted);


    /**
     * @brief Slot when the flag "Is Frozen" of a model changed
     * @param isMuted
     */
    void _onIsFrozenOfModelChanged(bool isFrozen);


private:
    /**
     * @brief Update with the list of models
     */
    void _updateWithModels();


    /**
     * @brief Update the status in function of status of models
     */
    void _updateStatus();


    /**
     * @brief Update the flag "Is Muted" in function of models
     */
    void _updateIsMuted();


    /**
     * @brief Update the flag "Is Frozen" in function of models
     */
    void _updateIsFrozen();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // List of peer ids of our models
    QStringList _peerIdsList;

};

QML_DECLARE_TYPE(AgentVM)

#endif // AGENTVM_H
