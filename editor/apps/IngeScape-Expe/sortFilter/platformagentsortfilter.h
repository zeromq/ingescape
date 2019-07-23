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

#ifndef PLATFORMAGENTSORTFILTER_H
#define PLATFORMAGENTSORTFILTER_H

#include <QObject>
#include <I2PropertyHelpers.h>
#include <I2Quick.h>


/**
 * @brief The PlatformAgentSortFilter class defines a specific filter for the agents of a platform
 */
class PlatformAgentSortFilter : public I2SortFilterProxyModel
{
    Q_OBJECT

    // List with agent names of the platform
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, agentNamesOfPlatform)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit PlatformAgentSortFilter(QObject *parent = nullptr);

    /**
      * @brief Destructor
      */
    //~PlatformAgentSortFilter();


    /**
     * @brief Update the filter
     */
    void updateFilter();


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

QML_DECLARE_TYPE(PlatformAgentSortFilter)

#endif // PLATFORMAGENTSORTFILTER_H
