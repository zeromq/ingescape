/*
 *	IngeScape Editor
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Natanael Vaugien   <vaugien@ingenuity.io>
 *
 */

#include "areagentsinplatformfilter.h"


#include <QDebug>
#include <viewModel/agentsgroupedbynamevm.h>


/**
 * @brief Default constructor
 * @param parent
 */
AreAgentsInPlatformFilter::AreAgentsInPlatformFilter(QObject *parent) : I2SortFilterProxyModel(parent),
  _currentProtocol(nullptr)
{
    setDynamicSortFilter(false);

    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}

/**
 * @brief Fore update of our filter (invalidateFilter is private, thus we need a function to call it)
 */
void AreAgentsInPlatformFilter::forceUpdate()
{
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
bool AreAgentsInPlatformFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)

    AgentsGroupedByNameVM* agent = dynamic_cast<AgentsGroupedByNameVM*>(item);
    if ((_currentProtocol != nullptr) && (agent != nullptr))
    {
        return _currentProtocol->isAgentNameInProtocol(agent->name());
    }
    return false;
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
bool AreAgentsInPlatformFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    return (indexItem1 < indexItem2);
}
