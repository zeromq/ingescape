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


#include "aresubjectandprotocolinsessionfilter.h"

#include <QDebug>

AreSubjectAndProtocolInSessionFilter::AreSubjectAndProtocolInSessionFilter(QObject *parent):  I2SortFilterProxyModel(parent),
    _selectedSubjectIdList(QStringList()),
    _selectedProtocolIdList(QStringList())
{
    setDynamicSortFilter(false);

    // Force ownership of our object, it will prevent Qml from stealing it
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

}

/**
 * @brief Fore update of our filter (invalidateFilter is private, thus we need a function to call it)
 */
void AreSubjectAndProtocolInSessionFilter::forceUpdate()
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
bool AreSubjectAndProtocolInSessionFilter::filterAccepts(QObject* item, int index) const
{
    Q_UNUSED(index)
    SessionM* taskInstanceValue = qobject_cast<SessionM*>(item);

    if (taskInstanceValue != nullptr)
    {
        bool resultSubject = _selectedSubjectIdList.contains(taskInstanceValue->subject()->displayedId());
        bool resultProtocol = _selectedProtocolIdList.contains(taskInstanceValue->task()->name());
        if (resultSubject && resultProtocol){
            return true;
        }
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
bool AreSubjectAndProtocolInSessionFilter::isLessThan(QObject* item1, int indexItem1, QObject* item2, int indexItem2) const
{
    Q_UNUSED(item1)
    Q_UNUSED(item2)

    // Default behavior: preserve indexes
    return (indexItem1 < indexItem2);
}
