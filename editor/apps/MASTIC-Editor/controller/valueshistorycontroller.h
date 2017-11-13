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

#ifndef VALUESHISTORYCONTROLLER_H
#define VALUESHISTORYCONTROLLER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>

#include <controller/masticmodelmanager.h>

/**
 * @brief The ValuesHistoryController class defines the controller for history of values
 */
class ValuesHistoryController : public QObject
{
    Q_OBJECT

    // List of filtered (and sorted) values
    I2_QOBJECT_SORTFILTERPROXY(PublishedValueM, filteredValues)

    // List with all agent Input/Output/Parameter Types
    I2_ENUM_LISTMODEL(AgentIOPTypes, allAgentIOPTypes)

    // List with all agent names
    I2_QML_PROPERTY_READONLY(QStringList, allAgentNamesList)


public:
    /**
     * @brief Constructor
     * @param modelManager
     * @param parent
     */
    explicit ValuesHistoryController(MasticModelManager* modelManager, QObject *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~ValuesHistoryController();


    /**
     * @brief Show the values of agent Input/Output/Parameter type
     * @param agentIOPType
     */
    Q_INVOKABLE void showValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType);


    /**
     * @brief Hide the values of agent Input/Output/Parameter type
     * @param agentIOPType
     */
    Q_INVOKABLE void hideValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType);


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


Q_SIGNALS:


public Q_SLOTS:

    /**
     * @brief Slot called when a new "Agent in Mapping" is added
     * @param agentName
     */
    void onAgentInMappingAdded(QString agentName);


    /**
     * @brief Slot called when an "Agent in Mapping" is removed
     * @param agentName
     */
    void onAgentInMappingRemoved(QString agentName);


private:
    /**
     * @brief Update the filters on the list of values
     */
    void _updateFilters();


private:
    // Manager for the data model of MASTIC
    MasticModelManager* _modelManager;

    // List with types of selected agent Input/Output/Parameter
    QList<AgentIOPTypes::Value> _selectedAgentIOPTypes;
    //QStringList _selectedAgentIOPTypes;

    // List with names of selected agents
    QStringList _selectedAgentNamesList;

};

QML_DECLARE_TYPE(ValuesHistoryController)

#endif // VALUESHISTORYCONTROLLER_H
