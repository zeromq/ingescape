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

    // Hostname(s) on the network of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, hostnames)

    // Flag indicating if our agent has never yet appeared on the network
    I2_QML_PROPERTY_READONLY(bool, hasNeverAppearedOnNetwork)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Status defined by the agent
    //I2_QML_PROPERTY_READONLY(QString, status)


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
     * @brief Change the state of our agent
     */
    Q_INVOKABLE void changeState();


    /**
     * @brief Mute / UN-mute all outputs of our agent
     */
    Q_INVOKABLE void changeMuteAllOutputs();


    /**
     * @brief Freeze / UN-freeze our agent
     */
    Q_INVOKABLE void changeFreeze();


Q_SIGNALS:

    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param executionPath
     */
    void commandAskedToLauncher(QString command, QString hostname, QString executionPath);

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
     * @brief Slot when the flag "is ON" of a model changed
     * @param isON
     */
    void _onIsONofModelChanged(bool isON);


    /**
     * @brief Slot when the flag "is Muted" of a model changed
     * @param isMuted
     */
    void _onIsMutedOfModelChanged(bool isMuted);


    /**
     * @brief Slot when the flag "is Frozen" of a model changed
     * @param isMuted
     */
    void _onIsFrozenOfModelChanged(bool isFrozen);


private:
    /**
     * @brief Update with the list of models
     */
    void _updateWithModels();


    /**
     * @brief Update the flag "is ON" in function of flags of models
     */
    void _updateIsON();


    /**
     * @brief Update the flag "is Muted" in function of flags of models
     */
    void _updateIsMuted();


    /**
     * @brief Update the flag "is Frozen" in function of flags of models
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
