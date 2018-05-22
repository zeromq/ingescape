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


/**
 * @brief The AgentVM class defines a view model of agent in the list
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

    // Flag indicating if our agent never yet appeared on the network
    I2_QML_PROPERTY_READONLY(bool, neverAppearedOnNetwork)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Definition of our agent
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(DefinitionM*, definition)

    // Number of clones
    I2_QML_PROPERTY_READONLY(int, clonesNumber)

    // Flag indicating if our agent can be restarted (by a INGESCAPE launcher)
    I2_QML_PROPERTY_READONLY(bool, canBeRestarted)

    // State of our agent
    I2_QML_PROPERTY(QString, state)


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
     * @brief Signal emitted when the definition changed (with previous and new values)
     * @param previousValue
     * @param newValue
     */
    void definitionChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue);


    /**
     * @brief Signal emitted when a command must be sent on the network to a launcher
     * @param command
     * @param hostname
     * @param commandLine
     */
    void commandAskedToLauncher(QString command, QString hostname, QString commandLine);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent
     * @param peerIdsList
     * @param command
     */
    void commandAskedToAgent(QStringList peerIdsList, QString command);


    /**
     * @brief Signal emitted when a command must be sent on the network to an agent about one of its output
     * @param peerIdsList
     * @param command
     * @param outputName
     */
    void commandAskedToAgentAboutOutput(QStringList peerIdsList, QString command, QString outputName);


    /**
     * @brief Signal emitted when we have to open the values history of our agent
     * @param agentName
     */
    void openValuesHistoryOfAgent(QString agentName);


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
     * @brief Slot when the flag "can Be Restarted" of a model changed
     * @param canBeRestarted
     */
    void _onCanBeRestartedOfModelChanged(bool canBeRestarted);


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


    /**
     * @brief Slot when the definition of a model changed
     * @param definition
     */
    void _onDefinitionOfModelChanged(DefinitionM* definition);


    /**
     * @brief Slot when the state of a model changed
     * @param state
     */
    void _onStateOfModelChanged(QString state);


    /**
     * @brief Slot when a command must be sent on the network to an agent about one of its output
     * @param command
     * @param outputName
     */
    void _onCommandAskedToAgentAboutOutput(QString command, QString outputName);


    /**
     * @brief Slot when we have to open the values history of our agent
     */
    void _onOpenValuesHistoryOfAgent();


private:
    /**
     * @brief Update with all models of agents
     */
    void _updateWithAllModels();


    /**
     * @brief Update the flag "is ON" in function of flags of models
     */
    void _updateIsON();


    /**
     * @brief Update the flag "can Be Restarted" in function of flags of models
     */
    void _updateCanBeRestarted();


    /**
     * @brief Update the flag "is Muted" in function of flags of models
     */
    void _updateIsMuted();


    /**
     * @brief Update the flag "is Frozen" in function of flags of models
     */
    void _updateIsFrozen();


    /**
     * @brief Update with the definition of the first model
     */
    void _updateWithDefinitionOfFirstModel();


    /**
     * @brief Update with the state of the first model
     */
    void _updateWithStateOfFirstModel();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // List of peer ids of our models
    QStringList _peerIdsList;

};

QML_DECLARE_TYPE(AgentVM)

#endif // AGENTVM_H
