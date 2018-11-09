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
 *
 */

#ifndef AGENTSGROUPEDBYDEFINITIONVM_H
#define AGENTSGROUPEDBYDEFINITIONVM_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <model/agentm.h>

/**
 * @brief The AgentsGroupedByDefinitionVM class defines a view model of group of agents who have the same name and the same definition
 * Allows to manage several models of agents who have the same name and the same definition
 */
class AgentsGroupedByDefinitionVM : public QObject
{
    Q_OBJECT

    // Name of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, name)

    // Definition of our agent(s)
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(DefinitionM*, definition)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)

    // Hostname(s) on the network of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, hostnames)

    // Flag indicating if our agent can be restarted (by an INGESCAPE launcher)
    I2_QML_PROPERTY_READONLY(bool, canBeRestarted)

    // Flag indicating if our agent is muted
    I2_QML_PROPERTY_READONLY(bool, isMuted)

    // Flag indicating if our agent can be frozen
    I2_QML_PROPERTY_READONLY(bool, canBeFrozen)

    // Flag indicating if our agent is frozen
    I2_QML_PROPERTY_READONLY(bool, isFrozen)

    // Number of clones
    I2_QML_PROPERTY_READONLY(int, clonesNumber)

    // State of our agent
    I2_QML_PROPERTY(QString, state)

    // Flag indicating if our agent has its log in a stream
    I2_QML_PROPERTY_READONLY(bool, hasLogInStream)

    // Flag indicating if our agent has its log in a file
    I2_QML_PROPERTY_READONLY(bool, hasLogInFile)

    // Path of the log file of our agent
    I2_QML_PROPERTY_READONLY(QString, logFilePath)

    // Path of the definition file of our agent
    I2_QML_PROPERTY_READONLY(QString, definitionFilePath)

    // Path of the mapping file of our agent
    I2_QML_PROPERTY_READONLY(QString, mappingFilePath)

    // Flag indicating if the option "View Log Stream" is enabled
    I2_QML_PROPERTY_READONLY(bool, isEnabledViewLogStream)


public:
    /**
     * @brief Constructor
     * @param model
     * @param parent
     */
    explicit AgentsGroupedByDefinitionVM(AgentM* model,
                                         DefinitionM* definition,
                                         QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsGroupedByDefinitionVM();


    /**
     * @brief Get the list of agent models on a host
     * @param hostname
     * @return
     */
    QList<AgentM*> getModelsOnHost(QString hostname);


Q_SIGNALS:

    /**
     * @brief Signal emitted when our view model has become useless (no more model)
     */
    void noMoreModelAndUseless();


public Q_SLOTS:


private Q_SLOTS:

    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();


    /**
     * @brief Slot called when the hostname of a model changed
     * @param hostname
     */
    void _onHostnameOfModelChanged(QString hostname);


    /**
     * @brief Slot called when the flag "is ON" of a model changed
     * @param isON
     */
    void _onIsONofModelChanged(bool isON);


private:
    /**
     * @brief Update with all models of agents
     */
    void _updateWithAllModels();


    /**
     * @brief Update the flag "is ON" in function of flags of models
     */
    void _updateIsON();


private:
    // Previous list of models of agents
    QList<AgentM*> _previousAgentsList;

    // List of peer ids of our models
    QStringList _peerIdsList;

    // Hash table from a hostname to a list of models of agents
    QHash<QString, QList<AgentM*>> _hashFromHostnameToModels;

};

QML_DECLARE_TYPE(AgentsGroupedByDefinitionVM)

#endif // AGENTSGROUPEDBYDEFINITIONVM_H
