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

#ifndef AREAGENTSINPLATFORMFILTER_H
#define AREAGENTSINPLATFORMFILTER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2Quick.h>

#include "I2PropertyHelpers.h"
#include "model/protocol/protocolm.h"


/**
 * @brief The AreAgentsInPlatformFilter class defines a sort/filter on agents list in a platform
 */
class AreAgentsInPlatformFilter: public I2SortFilterProxyModel
{
    Q_OBJECT

    I2_CPP_PROPERTY(ProtocolM*, currentProtocol)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AreAgentsInPlatformFilter(QObject *parent = nullptr);


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


protected:


};

QML_DECLARE_TYPE(AreAgentsInPlatformFilter)

#endif // AREAGENTSINPLATFORMFILTER_H
