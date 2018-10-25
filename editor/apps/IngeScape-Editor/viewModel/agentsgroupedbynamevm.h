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

#ifndef AGENTSGROUPEDBYNAMEVM_H
#define AGENTSGROUPEDBYNAMEVM_H

#include <QObject>

#include <I2PropertyHelpers.h>
#include <model/agentm.h>
#include <model/publishedvaluem.h>


/**
 * @brief The AgentsGroupedByNameVM class defines a view model of group of agents who have the same name
 * Allows to manage several models of agents who have the same name
 */
class AgentsGroupedByNameVM : public QObject
{
    Q_OBJECT

    // Name of our agent(s)
    I2_QML_PROPERTY_READONLY(QString, name)

    // List of models of agents
    I2_QOBJECT_LISTMODEL(AgentM, models)

    // Hostname(s) on the network of our agent(s)
    //I2_QML_PROPERTY_READONLY(QString, hostnames)

    // Flag indicating if our agent is ON (vs OFF)
    I2_QML_PROPERTY_READONLY(bool, isON)


public:
    /**
     * @brief Constructor
     * @param model
     * @param parent
     */
    explicit AgentsGroupedByNameVM(AgentM* model, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~AgentsGroupedByNameVM();


    /**
     * @brief Manage when a model of agent entered on our network
     * @param model
     */
    void manageAgentEnteredNetwork(AgentM* model);


    /**
     * @brief Manage when a model of agent exited from our network
     * @param model
     */
    void manageAgentExitedNetwork(AgentM* model);


    /**
     * @brief Update the current value of an I/O/P of our agent(s)
     * @param publishedValue
     */
    void updateCurrentValueOfIOP(PublishedValueM* publishedValue);


Q_SIGNALS:

public Q_SLOTS:

private Q_SLOTS:

    /**
     * @brief Slot called when the list of models changed
     */
    void _onModelsChanged();


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
    //QStringList _peerIdsList;

    // Hash table from a hostname to a list of models of agents
    //QHash<DefinitionM*, AgentsGroupedByDefinitionVM> _hashFromDefinitionTo...;

    // Hash table from a hostname to a list of models of agents
    //QHash<QString, QList<AgentM*>> _hashFromHostnameToModels;

};

QML_DECLARE_TYPE(AgentsGroupedByNameVM)

#endif // AGENTSGROUPEDBYNAMEVM_H
