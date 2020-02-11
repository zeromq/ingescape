/*
 *	IngeScape Expe
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */


#include "platformagentsortfilter.h"
#include <viewModel/agentsgroupedbynamevm.h>


/**
 * @brief Constructor
 * @param parent
 */
PlatformAgentSortFilter::PlatformAgentSortFilter(QObject *parent) : I2SortFilterProxyModel(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
  * @brief Destructor
  */
/*PlatformAgentSortFilter::~PlatformAgentSortFilter()
{

}*/


/**
 * @brief Update the filter
 */
void PlatformAgentSortFilter::updateFilter()
{
    // Invalidates the current filtering
    invalidateFilter();
}


/**
 * @brief filterAccepts is used to check if an item should be included in our result list or not
 *
 * @param item
 * @param index
 *
 * @return True if the item should be included in the result, false otherwise (i.e. the item is removed from our list)
 */
bool PlatformAgentSortFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)

    bool result = false;

    // Try to cast our item as a view model of "Agents Grouped by Name"
    AgentsGroupedByNameVM* agent = qobject_cast<AgentsGroupedByNameVM*>(item);
    if (agent != nullptr)
    {
        result = _agentNamesOfPlatform.contains(agent->name());
    }

    return result;
}


/**
 * @brief isLessThan is used to compare items in order to sort them
 *
 * @param item1
 * @param indexItem1
 * @param item2
 * @param indexItem2
 *
 * @return True if item1 is smaller than item2, false otherwise
 */
bool PlatformAgentSortFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    // Default behavior: preserve indexes
    return (indexItem1 < indexItem2);
}

