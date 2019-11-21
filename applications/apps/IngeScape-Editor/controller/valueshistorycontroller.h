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
 *
 */

#ifndef VALUESHISTORYCONTROLLER_H
#define VALUESHISTORYCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/ingescapemodelmanager.h>
#include <sortFilter/valueshistorysortfilter.h>


/**
 * @brief The ValuesHistoryController class defines the controller for history of values
 */
class ValuesHistoryController : public QObject
{
    Q_OBJECT

    // List of filtered (and sorted) values
    Q_PROPERTY(ValuesHistorySortFilter* filteredValues READ filteredValues CONSTANT)

    // List with all agent names
    I2_QML_PROPERTY_READONLY(QStringList, allAgentNamesList)

    // List with names of selected agents
    I2_QML_PROPERTY_READONLY(QStringList, selectedAgentNamesList)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ValuesHistoryController(QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ValuesHistoryController();


    /**
     * @brief Get our filtered list of values
     * @return
     */
    ValuesHistorySortFilter* filteredValues()
    {
        return &_filteredValues;
    }


    /**
     * @brief Show the values of agent with name
     * @param agentName
     */
    Q_INVOKABLE void showValuesOfAgent(QString agentName);


    /**
     * @brief Hide the values of agent with name
     * @param agentName
     */
    Q_INVOKABLE void hideValuesOfAgent(QString agentName);


    /**
     * @brief Show the values of all agents
     */
    Q_INVOKABLE void showValuesOfAllAgents();


    /**
     * @brief Hide the values of all agents
     */
    Q_INVOKABLE void hideValuesOfAllAgents();


    /**
     * @brief Return true if the values of the agent are shown
     * @param agentName
     * @return
     */
    Q_INVOKABLE bool areShownValuesOfAgent(QString agentName);


    /**
     * @brief Set both list of agent names
     * @param agentNamesList
     */
    void setAgentNamesList(QStringList agentNamesList);


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot called when a new view model of agents grouped by name has been created
     * @param agentsGroupedByName
     */
    void onAgentsGroupedByNameHasBeenCreated(AgentsGroupedByNameVM* agentsGroupedByName);


    /**
     * @brief Slot called when we have to filter values to show only those of the agent (with the name)
     * @param agentName
     */
    void filterValuesToShowOnlyAgent(QString agentName);


private:
    /**
     * @brief Update the filters on the list of values
     */
    void _updateFilters();


private:

    // List of filtered (and sorted) values
    ValuesHistorySortFilter _filteredValues;

};

QML_DECLARE_TYPE(ValuesHistoryController)

#endif // VALUESHISTORYCONTROLLER_H
