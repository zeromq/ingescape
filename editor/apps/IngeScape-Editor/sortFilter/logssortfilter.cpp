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

#include "logssortfilter.h"
#include <model/logm.h>

/**
 * @brief Constructor
 * @param parent
 */
LogsSortFilter::LogsSortFilter(QObject *parent) : I2SortFilterProxyModel(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
  * @brief Destructor
  */
LogsSortFilter::~LogsSortFilter()
{

}


/**
 * @brief Update the filter (with the new list of selected log types)
 * @param selectedLogTypes
 */
void LogsSortFilter::updateFilter(QList<int> selectedLogTypes)
{
    // Update the list of selected log types
    _selectedLogTypes = selectedLogTypes;

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
bool LogsSortFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)

    bool result = false;

    // Try to cast our item as a model of "Log"
    LogM* log = qobject_cast<LogM*>(item);
    if (log != NULL)
    {
        result = _selectedLogTypes.contains(log->logType());
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
bool LogsSortFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    // Default behavior: preserve indexes
    return (indexItem1 < indexItem2);
}

