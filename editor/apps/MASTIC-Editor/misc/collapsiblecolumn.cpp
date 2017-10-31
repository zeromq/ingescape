/*
 *	MASTIC Editor
 *
 *  Copyright (c) 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */


#include "collapsiblecolumn.h"

#include <QDebug>
#include <QtGlobal>


/**
 * @brief Default constructor
 * @param parent
 */
CollapsibleColumn::CollapsibleColumn(QQuickItem *parent) : QQuickItem(parent),
    _spacing(0.0),
    _value(1.0)
{
    connect(this, &CollapsibleColumn::childrenChanged, this, &CollapsibleColumn::polish);
}


/**
 * @brief Destructor
 */
CollapsibleColumn::~CollapsibleColumn()
{
    disconnect(this, &CollapsibleColumn::childrenChanged, this, &CollapsibleColumn::polish);
}


/**
 * @brief Set spacing
 * @param value
 */
void CollapsibleColumn::setspacing(qreal value)
{
    value = qMax(value, 0.0);
    if (!qFuzzyCompare(_spacing, value))
    {
        _spacing = value;

        polish();

        Q_EMIT spacingChanged(value);
    }
}


/**
 * @brief Set value
 * @param value
 */
void CollapsibleColumn::setvalue(qreal value)
{
    value = qMin(1.0, qMax(0.0, value));
    if (!qFuzzyCompare(_value, value))
    {
        _value = value;

        polish();

        Q_EMIT valueChanged(value);
    }
}


/**
 * @brief Called when our QML instance is completed
 */
void CollapsibleColumn::componentComplete()
{
    //NB: Don't forget to call the base class implementation
    //    Otherwise nothing will show up!
    QQuickItem::componentComplete();

    polish();
}


/**
 * @brief Called to perform any layout as required for this item
 */
void CollapsibleColumn::updatePolish()
{
    qDebug() << Q_FUNC_INFO;

    const QList<QQuickItem *> childrenList = childItems();
    if (childrenList.count() > 0)
    {
        qreal currentY = 0;
        for (QList<QQuickItem *>::const_iterator iterator = childrenList.constBegin(); iterator != childrenList.constEnd(); ++iterator)
        {
            QQuickItem* child = (*iterator);
            if (child != NULL)
            {
                // Filter invisible items and repeaters
                if (child->isVisible() && !child->inherits("QQuickRepeater"))
                {
                    // Set ordinate of our child
                    child->setY(currentY * _value);

                    // Update current ordinate
                    currentY += child->height() + _spacing;
                }
            }
            // Else: should not happen
        }
    }
}


/**
 * @brief Called when change occurs for this item
 * @param change
 * @param value
 */
void CollapsibleColumn::itemChange(ItemChange change, const ItemChangeData& value)
{
    // Check if a child has been added
    if (change == QQuickItem::ItemChildAddedChange)
    {
        QQuickItem* child = value.item;
        if (child != NULL)
        {
            connect(child, &QQuickItem::visibleChanged, this, &CollapsibleColumn::polish, Qt::UniqueConnection);
            connect(child, &QQuickItem::heightChanged, this, &CollapsibleColumn::polish, Qt::UniqueConnection);
        }
    }
    // Check if a child has been removed
    else if (change == QQuickItem::ItemChildRemovedChange)
    {
        QQuickItem* child = value.item;
        if (child != NULL)
        {
            disconnect(child, &QQuickItem::visibleChanged, this, &CollapsibleColumn::polish);
            disconnect(child, &QQuickItem::heightChanged, this, &CollapsibleColumn::polish);
        }
    }
}
