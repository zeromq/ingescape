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
 *      Bruno Lemenicier <lemenicier@ingenuity.io>
 *      Alexandre Lemort <lemort@ingenuity.io>
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#ifndef HOSTVM_H
#define HOSTVM_H

#include <QObject>

#include <I2PropertyHelpers.h>

#include <model/hostm.h>
#include <model/agent/agentm.h>
#include <sortFilter/agentsinhostsortfilter.h>


/**
 * @brief The HostVM class defines a view model of host in the supervision list
 */
class HostVM : public QObject
{
    Q_OBJECT

    // Name of our host
    I2_QML_PROPERTY(QString, name)

    // Model of our host
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER(HostM*, modelM)

    // Flag indicating if our host is ON (vs OFF) - Launcher status
    I2_QML_PROPERTY_READONLY(bool, isON)

    // (NOT sorted) list of agents executed on our host
    I2_QOBJECT_LISTMODEL(AgentM, agentsList)

    // Flag indicating how many started agents host have
    I2_QML_PROPERTY_READONLY(int, nbAgentsOn)

    // List of (filtered and) sorted agents executed on our host
    Q_PROPERTY(AgentsInHostSortFilter* sortedAgents READ sortedAgents CONSTANT)

    // Flag indicating if our host can provide stream
    I2_QML_PROPERTY_READONLY(bool, canProvideStream)

    // Flag indicating if the host is streaming or not
    I2_QML_PROPERTY(bool, isStreaming)


public:

    /**
     * @brief Constructor
     * @param name
     * @param model
     * @param parent
     */
    explicit HostVM(QString name,
                    HostM* model = nullptr,
                    QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~HostVM();


    /**
     * @brief Get our sorted list of agents
     * @return
     */
    AgentsInHostSortFilter* sortedAgents()
    {
        return &_sortedAgents;
    }


    /**
     * @brief Change the stream state of our host
     */
    Q_INVOKABLE void changeStreamState();


    /**
     * @brief Start an agent
     * @param agent
     */
    Q_INVOKABLE void startAgent(AgentM* agent);


    /**
     * @brief Stop an agent
     * @param agent
     */
    Q_INVOKABLE void stopAgent(AgentM* agent);


    /**
     * @brief Add an agent model to list and connect to its isONChanged signal
     * @param agent
     */
    void addAgentModelToList(AgentM* agent);


    /**
     * @brief Remove an agent model from list and disconnect from its isONChanged signal
     * @param agent
     */
    void removeAgentModelFromList(AgentM* agent);


Q_SIGNALS:


private Q_SLOTS:

    /**
     * @brief Slot called when the status ON/OFF changed in one agent model of _agentsList
     */
    void _onStatusONChanged(bool isOn);


private:
    // List of (filtered and) sorted agents
    AgentsInHostSortFilter _sortedAgents;

};

QML_DECLARE_TYPE(HostVM)

#endif // HOSTVM_H
