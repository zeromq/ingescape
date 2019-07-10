/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2018 Ingenuity i/o. All rights reserved.
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

#include <model/agent/definition/definitionm.h>
#include <model/agent/mapping/agentmappingm.h>


/**
 * @brief The AgentM class defines a model of agent
 */
class AgentM : public QObject
{
    Q_OBJECT

    // Name of our agent
    I2_QML_PROPERTY(QString, name)

    // Peer ID of our agent (unique identifier)
    I2_QML_PROPERTY_READONLY(QString, peerId)

    // IP address of our agent
    I2_QML_PROPERTY_READONLY(QString, address)

    // Hostname of our agent
    I2_QML_PROPERTY_READONLY(QString, hostname)

    // Command line of our agent
    I2_QML_PROPERTY_READONLY(QString, commandLine)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Flag indicating if our agent can be restarted (by an IngeScape launcher)
    I2_QML_PROPERTY_READONLY(bool, canBeRestarted)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Definition of our agent
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(DefinitionM*, definition)

    // Mapping of our agent
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(AgentMappingM*, mapping)

    // State of our agent
    I2_CPP_PROPERTY(QString, state)

    // Port for log in stream
    I2_CPP_PROPERTY(QString, loggerPort)

    // Flag indicating if our agent has its log in a stream
    I2_CPP_PROPERTY(bool, hasLogInStream)

    // Flag indicating if our agent has its log in a file
    I2_CPP_PROPERTY(bool, hasLogInFile)

    // Path of the log file of our agent
    I2_CPP_PROPERTY(QString, logFilePath)

    // Path of the definition file of our agent
    I2_CPP_PROPERTY(QString, definitionFilePath)

    // Path of the mapping file of our agent
    I2_CPP_PROPERTY(QString, mappingFilePath)


public:

    /**
     * @brief Constructor
     * @param name
     * @param peerId optional (empty by default)
     * @param ipAddress optional (empty by default)
     * @param hostname optional (default value)
     * @param commandLine optional (empty by default)
     * @param isON optional (false by default)
     * @param parent
     */
    explicit AgentM(QString name,
                    QString peerId = "",
                    QString ipAddress = "",
                    QString hostname = HOSTNAME_NOT_DEFINED,
                    QString commandLine = "",
                    bool isON = false,
                    QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentM();


    /**
     * @brief Set the flag "is Muted" of an Output of our agent
     * @param isMuted
     * @param outputName
     */
    void setisMutedOfOutput(bool isMuted, QString outputName);


    /**
     * @brief Clear the data about the network of our agent
     */
    void clearNetworkData();


Q_SIGNALS:

    /**
     * @brief Signal emitted when the definition changed (with previous and new values)
     * @param previousValue
     * @param newValue
     */
    void definitionChangedWithPreviousAndNewValues(DefinitionM* previousValue, DefinitionM* newValue);


    /**
     * @brief Signal emitted when the mapping changed (with previous and new values)
     * @param previousValue
     * @param newValue
     */
    void mappingChangedWithPreviousAndNewValues(AgentMappingM* previousValue, AgentMappingM* newValue);


    /**
     * @brief Signal emitted when the flag "is Muted Output" of an output changed
     * @param isMutedOutput
     * @param outputName
     */
    void isMutedOutputChanged(bool isMutedOutput, QString outputName);


    /**
     * @brief Signal emitted when the network data of our agent will be cleared
     * @param peerId
     */
    void networkDataWillBeCleared(QString peerId);


public Q_SLOTS:

};

QML_DECLARE_TYPE(AgentM)

#endif // AGENTM_H
