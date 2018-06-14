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

#ifndef LOGSSORTFILTER_H
#define LOGSSORTFILTER_H

#include <QObject>
#include <QtQml>

#include <I2PropertyHelpers.h>
#include <I2Quick.h>

#include <model/logm.h>


/**
 * @brief The LogsSortFilter class defines a specific filter for the logs
 */
class LogsSortFilter : public I2SortFilterProxyModel
{
    Q_OBJECT


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit LogsSortFilter(QObject *parent = nullptr);


    /**
      * @brief Destructor
      */
    ~LogsSortFilter();


    /**
     * @brief Update the filter (with the new list of selected log types)
     * @param selectedLogTypes
     */
    void updateFilter(QList<int> selectedLogTypes);


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


private:

    // List of selected log types
    QList<int> _selectedLogTypes;

};

QML_DECLARE_TYPE(LogsSortFilter)

#endif // LOGSSORTFILTER_H
