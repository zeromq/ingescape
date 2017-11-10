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

#include "valueshistorycontroller.h"

/**
 * @brief Constructor
 * @param modelManager
 * @param parent
 */
ValuesHistoryController::ValuesHistoryController(MasticModelManager* modelManager,
                                                 QObject *parent) : QObject(parent),
    _modelManager(modelManager)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_modelManager != NULL)
    {
        //
        // Link our list to the list of the model manager
        //
        _filteredValues.setSourceModel(_modelManager->publishedValues());
        _filteredValues.setSortProperty("time");

        //_filteredValues.setFilterProperty("currentState");
        //_filteredValues.setFilterFixedString(QString::number(SegmentZoneStates::ENCOMBRE));

        // Fill the list with all enum values
        _allAgentIOPTypes.fillWithAllEnumValues();

        // By default: all types are selected
        //QList<I2EnumListItemData *> L1 = _allAgentIOPTypes.toList();
        //QList<int> L2 = _allAgentIOPTypes.toEnumValuesList();
        _selectedAgentIOPTypes.append(AgentIOPTypes::INPUT);
        _selectedAgentIOPTypes.append(AgentIOPTypes::OUTPUT);
        _selectedAgentIOPTypes.append(AgentIOPTypes::PARAMETER);
    }
}


/**
 * @brief Destructor
 */
ValuesHistoryController::~ValuesHistoryController()
{
    _selectedAgentIOPTypes.clear();
    _allAgentIOPTypes.deleteAllItems();

    _modelManager = NULL;
}


/**
 * @brief Show the values of agent Input/Output/Parameter type
 * @param agentIOPType
 */
void ValuesHistoryController::showValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType)
{
    _selectedAgentIOPTypes.append(agentIOPType);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Hide the values of agent Input/Output/Parameter type
 * @param agentIOPType
 */
void ValuesHistoryController::hideValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType)
{
    _selectedAgentIOPTypes.removeOne(agentIOPType);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Show the values of agent with name
 * @param agentName
 */
void ValuesHistoryController::showValuesOfAgent(QString agentName)
{
    _selectedAgentNamesList.append(agentName);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Hide the values of agent with name
 * @param agentName
 */
void ValuesHistoryController::hideValuesOfAgent(QString agentName)
{
    _selectedAgentNamesList.removeOne(agentName);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Slot called when a new "Agent in Mapping" is added
 * @param agentName
 */
void ValuesHistoryController::onAgentInMappingAdded(QString agentName)
{
    _allAgentNamesList.append(agentName);

    // By default: the agent name is selected
    _selectedAgentNamesList.append(agentName);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Slot called when an "Agent in Mapping" is removed
 * @param agentName
 */
void ValuesHistoryController::onAgentInMappingRemoved(QString agentName)
{
    _allAgentNamesList.removeOne(agentName);

    if (_selectedAgentNamesList.contains(agentName)) {
        _selectedAgentNamesList.removeOne(agentName);

        // Update the filters on the list of values
        _updateFilters();
    }
}


/**
 * @brief Update the filters on the list of values
 */
void ValuesHistoryController::_updateFilters()
{
    qDebug() << "Display Values for type:";
    foreach (AgentIOPTypes::Value agentIOPType, _selectedAgentIOPTypes) {
        qDebug() << AgentIOPTypes::staticEnumToString(agentIOPType);
    }

    qDebug() << "and for agents" << _selectedAgentNamesList;
    /*foreach (QString agentName, _selectedAgentNamesList) {

    }*/
}
