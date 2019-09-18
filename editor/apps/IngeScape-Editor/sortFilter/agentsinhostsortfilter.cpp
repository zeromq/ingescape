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

#include "agentsinhostsortfilter.h"
#include <model/agent/agentm.h>

/**
 * @brief Constructor
 * @param parent
 */
AgentsInHostSortFilter::AgentsInHostSortFilter(QObject *parent) : I2SortFilterProxyModel(parent)
{
    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


/**
  * @brief Destructor
  */
/*AgentsInHostSortFilter::~AgentsInHostSortFilter()
{
}*/


/**
 * @brief filterAccepts is used to check if an item should be included in our result list or not
 *
 * @param item
 * @param index
 *
 * @return True if the item should be included in the result, false otherwise (i.e. the item is removed from our list)
 */
bool AgentsInHostSortFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(item)
    Q_UNUSED(index)

    return true;
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
bool AgentsInHostSortFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    AgentM* agent1 = qobject_cast<AgentM*>(item1);
    AgentM* agent2 = qobject_cast<AgentM*>(item2);

    if ((agent1 != nullptr) && (agent2 != nullptr))
    {
        // ON & OFF
        if (agent1->isON() && !agent2->isON())
        {
            return true;
        }
        // OFF & ON
        else if (!agent1->isON() && agent2->isON())
        {
            return false;
        }
        // Both are ON or both are OFF
        else
        {
            int result = QString::compare(agent1->name(), agent2->name(), Qt::CaseInsensitive);

            if (result < 0) {
                return true;
            }
            else if (result > 0) {
                return false;
            }
            else {
                // Default behavior: preserve indexes
                return (indexItem1 < indexItem2);
            }
        }
    }
    else {
        // Default behavior: preserve indexes
        return (indexItem1 < indexItem2);
    }
}
