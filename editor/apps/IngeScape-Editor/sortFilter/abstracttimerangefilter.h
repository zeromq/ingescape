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

#ifndef ABSTRACTTIMERANGEFILTER_H
#define ABSTRACTTIMERANGEFILTER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2Quick.h>

#include "I2PropertyHelpers.h"



/**
 * @brief The AbstractTimeRangeFilter class defines a custom QSortFilterProxyModel to filter
 *        a list of objects based on a given time range in milliseconds
 */
class AbstractTimeRangeFilter: public I2SortFilterProxyModel
{
    Q_OBJECT


public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AbstractTimeRangeFilter(QObject *parent = 0);


    /**
     * @brief Set our time range
     * @param startTimeInMilliseconds
     * @param endTimeInMilliseconds
     */
    void setTimeRange(int startTimeInMilliseconds, int endTimeInMilliseconds);


    /**
     * @brief Fore update of our filter (invalidateFilter is private, thus we need a function to call it)
     */
    void forceUpdate();


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
    virtual bool filterAccepts(QObject* item, int index) const Q_DECL_OVERRIDE;


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
    virtual bool isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const Q_DECL_OVERRIDE;


protected:
    // Start time used by our filter
    int _startTimeInMilliseconds;

    // End time used by our filter
    int _endTimeInMilliseconds;

};

QML_DECLARE_TYPE(AbstractTimeRangeFilter)

#endif // ABSTRACTTIMERANGEFILTER_H
