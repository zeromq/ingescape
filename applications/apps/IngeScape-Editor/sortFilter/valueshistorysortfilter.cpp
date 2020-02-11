/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

#include "valueshistorysortfilter.h"
#include <model/publishedvaluem.h>

/**
 * @brief Constructor
 * @param parent
 */
ValuesHistorySortFilter::ValuesHistorySortFilter(QObject *parent) : I2SortFilterProxyModel(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
  * @brief Destructor
  */
/*ValuesHistorySortFilter::~ValuesHistorySortFilter()
{
}*/


/**
 * @brief Update the filter
 */
void ValuesHistorySortFilter::updateFilter()
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
bool ValuesHistorySortFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)

    bool result = false;

    // Try to cast our item as a model of "Published Value"
    PublishedValueM* publishedValue = qobject_cast<PublishedValueM*>(item);
    if (publishedValue != nullptr)
    {
        result = _selectedAgentNamesList.contains(publishedValue->agentName());
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
bool ValuesHistorySortFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    // Default behavior: preserve indexes
    return (indexItem1 < indexItem2);
}

