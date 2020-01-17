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

#ifndef AGENTSINHOSTSORTFILTER_H
#define AGENTSINHOSTSORTFILTER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <I2Quick.h>


/**
 * @brief The AgentsInHostSortFilter class defines a specific sort for the agents list in a host
 */
class AgentsInHostSortFilter : public I2SortFilterProxyModel
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent
     */
    explicit AgentsInHostSortFilter(QObject *parent = nullptr);


protected:

    /**
     * @brief filterAccepts is used to check if an item should be included in our result list or not
     *
     * @param item
     * @param index
     *
     * @return True if the item should be included in the result, false otherwise (i.e. the item is removed from our list)
     */
    bool filterAccepts(QObject* item, int index) const Q_DECL_OVERRIDE;


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
    bool isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const Q_DECL_OVERRIDE;

};

QML_DECLARE_TYPE(AgentsInHostSortFilter)

#endif // AGENTSINHOSTSORTFILTER_H
