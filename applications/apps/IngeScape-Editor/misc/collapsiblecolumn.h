/*
 *	IngeScape Editor
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


#ifndef COLLAPSIBLECOLUMN_H
#define COLLAPSIBLECOLUMN_H

#include <QObject>
#include <QQuickItem>
#include <QtQml>

#include <I2PropertyHelpers.h>


/**
 * @brief The CollapsibleColumn class defines a custom QML item: a column layout that can be collapsed
 */
class CollapsibleColumn : public QQuickItem
{
    Q_OBJECT

    // The amount in pixels left empty between two adjacent items
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, spacing)

    // 0: fully collapsed, 1: fully expanded
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, value)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit CollapsibleColumn(QQuickItem *parent = nullptr);


    /**
     * @brief Destructor
     */
    ~CollapsibleColumn();


protected:
    /**
     * @brief Called when our QML instance is completed
     */
    void componentComplete() Q_DECL_OVERRIDE;


    /**
     * @brief Called to perform any layout as required for this item
     */
    void updatePolish() Q_DECL_OVERRIDE;


    /**
     * @brief Called when change occurs for this item
     * @param change
     * @param value
     */
    void itemChange(ItemChange change, const ItemChangeData& value) Q_DECL_OVERRIDE;
};

QML_DECLARE_TYPE(CollapsibleColumn)

#endif // COLLAPSIBLECOLUMN_H
