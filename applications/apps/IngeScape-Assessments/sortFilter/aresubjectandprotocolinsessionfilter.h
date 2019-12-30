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

#ifndef ARESUBJECTANDPROTOCOLINSESSIONFILTER_H
#define ARESUBJECTANDPROTOCOLINSESSIONFILTER_H

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <I2Quick.h>

#include "I2PropertyHelpers.h"

#include "model/sessionm.h"
#include "model/subject/subjectm.h"
#include "model/protocol/protocolm.h"


class AreSubjectAndProtocolInSessionFilter : public I2SortFilterProxyModel
{
    Q_OBJECT

    // List with id of selected subjects
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, selectedSubjectIdList)

    // List with id of selected protocols
    I2_CPP_NOSIGNAL_PROPERTY(QStringList, selectedProtocolIdList)

public:

    /**
     * @brief Default constructor
     * @param parent
     */
    explicit AreSubjectAndProtocolInSessionFilter(QObject *parent = nullptr);


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

QML_DECLARE_TYPE(AreSubjectAndProtocolInSessionFilter)

#endif // ARESUBJECTANDPROTOCOLINSESSIONFILTER_H
