/*
 *	IngeScape Common
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort    <lemort@ingenuity.io>
 *
 */

#ifndef DASHEDBAR_H
#define DASHEDBAR_H

#include <QQuickItem>
#include <QtQml>

#include "I2PropertyHelpers.h"


/**
 * @brief The DashedBar class defines a Qt Quick item that displays a dashed bar
 */
class DashedBar : public QQuickItem
{
    Q_OBJECT

    // Orientation
    I2_QML_PROPERTY_CUSTOM_SETTER(Qt::Orientation, orientation)

    // Color
    I2_QML_PROPERTY_CUSTOM_SETTER(QColor, color)

    // Dash pattern
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, dashArray)


public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit DashedBar(QQuickItem* parent = nullptr);


protected:
    /**
     * @brief polish + compute vertices + update
     */
    void needToUpdateVertices();


    /**
     * @brief Called when our geometry has changed
     * @param newGeometry
     * @param oldGeometry
     */
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) Q_DECL_OVERRIDE;


    /**
     * @brief Called on the render thread when it is time to draw our item in the scene graph.
     *  The function is called as a result of QQuickItem::update()
     * @param oldNode The node that was returned the last time the function was called
     * @param paintNodeData The QSGTransformNode associated with our item
     * @return
     */
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* paintNodeData) Q_DECL_OVERRIDE;


    /**
     * @brief Called to perform any layout as required for this item
     */
    void updatePolish() Q_DECL_OVERRIDE;


    /**
     * @brief Called when our component is completed
     */
    void componentComplete() Q_DECL_OVERRIDE;


    /**
     * @brief Called when change occurs for this item
     * @param change
     * @param value
     */
    void itemChange(ItemChange change, const ItemChangeData& value) Q_DECL_OVERRIDE;


    /**
     * @brief Compute a new list of vertices
     */
    void _computeVertices();


protected:
    // Flag indicating if we must update our geometry
    bool _needToUpdateGeometry;

    // Flag indicating if we must update our materials
    bool _needToUpdateMaterial;

    // Flag indicating if we need to update our list of vertices
    bool _needToUpdateVertices;

    // List of vertices
    QList<QPointF> _vertices;
};

QML_DECLARE_TYPE(DashedBar)

#endif // DASHEDBAR_H
