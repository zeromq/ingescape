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

        // Fill the list with all enum values
        _allAgentIOPTypes.fillWithAllEnumValues();

        // By default: all types are selected
        _selectedAgentIOPTypes.fillWithAllEnumValues();
    }
}


/**
 * @brief Destructor
 */
ValuesHistoryController::~ValuesHistoryController()
{
    _selectedAgentIOPTypes.deleteAllItems();
    _allAgentIOPTypes.deleteAllItems();

    _modelManager = NULL;
}


/**
 * @brief Show the values of agent Input/Output/Parameter type
 * @param agentIOPType
 */
void ValuesHistoryController::showValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType)
{
    _selectedAgentIOPTypes.appendEnumValue(agentIOPType);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Hide the values of agent Input/Output/Parameter type
 * @param agentIOPType
 */
void ValuesHistoryController::hideValuesOfAgentIOPType(AgentIOPTypes::Value agentIOPType)
{
    _selectedAgentIOPTypes.removeEnumValue(agentIOPType);

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
 * @brief Slot called when we have to filter values to show only those of the agent (with the name)
 * @param agentName
 */
void ValuesHistoryController::filterValuesToShowOnlyAgent(QString agentName)
{
    // Clear all names
    _selectedAgentNamesList.clear();

    // Add only this one
    _selectedAgentNamesList.append(agentName);

    // Update the filters on the list of values
    _updateFilters();
}


/**
 * @brief Update the filters on the list of values
 */
void ValuesHistoryController::_updateFilters()
{
    /*qDebug() << "Display Values for type:";
    foreach (int iterator, _selectedAgentIOPTypes.toEnumValuesList()) {
        AgentIOPTypes::Value agentIOPType = static_cast<AgentIOPTypes::Value>(iterator);
        qDebug() << AgentIOPTypes::staticEnumToString(agentIOPType);
    }*/

    qDebug() << "and for agents" << _selectedAgentNamesList;

    // Update the list of agent names of the filter
    _filteredValues.setselectedAgentNamesList(_selectedAgentNamesList);

    // Update the filter
    _filteredValues.updateFilter();
}
