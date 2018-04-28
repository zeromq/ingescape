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

#ifndef VALUESHISTORYSORTFILTER_H
#define VALUESHISTORYSORTFILTER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <I2Quick.h>


/**
 * @brief The ValuesHistorySortFilter class defines a specific filter for the history of values
 */
class ValuesHistorySortFilter : public I2SortFilterProxyModel
{
    Q_OBJECT

    // List with names of selected agents
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, selectedAgentNamesList)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit ValuesHistorySortFilter(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~ValuesHistorySortFilter();


    /**
     * @brief Update the filter
     */
    void updateFilter();


Q_SIGNALS:


public Q_SLOTS:


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

#endif // VALUESHISTORYSORTFILTER_H
