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
 *      Alexandre Lemort   <lemort@ingenuity.io>
 *
 */

#include "abstracttimerangefilter.h"


#include <QDebug>

#include "model/scenario/timetickm.h"
#include "viewModel/scenario/actionvm.h"


/**
 * @brief Default constructor
 * @param parent
 */
AbstractTimeRangeFilter::AbstractTimeRangeFilter(QObject *parent) :
    I2SortFilterProxyModel(parent),
    _startTimeInMilliseconds(-1),
    _endTimeInMilliseconds(-1)
{

    setDynamicSortFilter(false);
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}

/**
 * @brief Fore update of our filter (invalidateFilter is private, thus we need a function to call it)
 */
void AbstractTimeRangeFilter::forceUpdate()
{
    invalidateFilter();
}

/**
 * @brief Set our time range
 * @param startTimeInMilliseconds
 * @param endTimeInMilliseconds
 */
void AbstractTimeRangeFilter::setTimeRange(int startTimeInMilliseconds, int endTimeInMilliseconds)
{
    if ( (_startTimeInMilliseconds != startTimeInMilliseconds)
         ||
         (_endTimeInMilliseconds != endTimeInMilliseconds) )
    {
        // Save values
        _startTimeInMilliseconds = startTimeInMilliseconds;
        _endTimeInMilliseconds = endTimeInMilliseconds;

        // Update filter
        invalidateFilter();
    }
}

/**
 * @brief filterAccepts is used to check if an item should be included in our result list or not
 *
 * @param item
 * @param index
 *
 * @return True if the item should be included in the result, false otherwise (i.e. the item is removed from our list)
 */
bool AbstractTimeRangeFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)

    bool result = false;

    // Try to cast our item as an TimeTickM
    TimeTickM * timeTickItem = dynamic_cast<TimeTickM*>(item);
    if (timeTickItem != nullptr)
    {
        //
        // We have an TimeTickM item
        //
        int timeInMilliseconds = timeTickItem->timeInMilliSeconds();

        // Check if our time is valid
        if (timeInMilliseconds > 0)
        {
            result = ((timeInMilliseconds <= _endTimeInMilliseconds) && (timeInMilliseconds >= _startTimeInMilliseconds));
        }
    }
    else {
        ActionVM * actionVMItem = dynamic_cast<ActionVM*>(item);
        if (actionVMItem != nullptr)
        {
            // Check if our time is valid
            if (actionVMItem->startTime() > 0)
            {
                // Case of forever actions
                result = !( (actionVMItem->startTime() > _endTimeInMilliseconds) || (actionVMItem->endTime() != -1 && actionVMItem->endTime() < _startTimeInMilliseconds));
            }
        }
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
bool AbstractTimeRangeFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    return (indexItem1 < indexItem2);
}
