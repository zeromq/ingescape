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

#include "dashedbar.h"

#include <QtDebug>
#include <QtGlobal>

#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

#include "graphics/I2GraphicsHelpers.h"


/**
 * @brief Default constructor
 * @param parent
 */
DashedBar::DashedBar(QQuickItem* parent)
    : QQuickItem(parent),
      _orientation(Qt::Vertical),
      _color("#000000"),
      _dashArray(""),
      // By default, we need to create every QSGNode
      _needToUpdateGeometry(true),
      // By default, we need to create every QSGNode
      _needToUpdateMaterial(true),
      // By default, we have an empty list of vertices
      _needToUpdateVertices(true)
{
    setFlag(ItemHasContents, true);
}


/**
 * @brief Set our orientation
 * @param value
 */
void DashedBar::setorientation(Qt::Orientation value)
{
    if (_orientation != value)
    {
        // Save value
        _orientation = value;

        // Notify that we must update our element
        needToUpdateVertices();

        // Notify change
        Q_EMIT orientationChanged(value);
    }
}


/**
 * @brief Set color
 * @param value
 */
void DashedBar::setcolor(QColor value)
{
    if (_color != value)
    {
        // Save value
        _color = value;

        // Notify that we must update our element
        _needToUpdateMaterial = true;
        update();

        // Notify change
        Q_EMIT colorChanged(value);
    }
}


/**
 * @briefSet our dash array
 * @param value
 */
void DashedBar::setdashArray(QString value)
{
    if (_dashArray != value)
    {
        // Save value
        _dashArray = value;

        // Notify that we must update our element
        needToUpdateVertices();

        // Notify change
        Q_EMIT dashArrayChanged(value);
    }
}


/**
 * @brief polish + compute vertices + update
 */
void DashedBar::needToUpdateVertices()
{
    _needToUpdateVertices = true;

    // Check if our item is visible
    if (isVisible())
    {
        polish();
        update();
    }
}


/**
 * @brief Called when our geometry has changed
 * @param newGeometry
 * @param oldGeometry
 */
void DashedBar::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    // Check if geometry has really changed
    // NB: we only need to test sizes because (x,y) is converted into a translation by QQuickItem
    if (newGeometry.size() != oldGeometry.size())
    {
        // We need to update our list of vertices
        needToUpdateVertices();
    }

    // Propagate change
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
}


/**
 * @brief Called on the render thread when it is time to draw our item in the scene graph.
 *  The function is called as a result of QQuickItem::update()
 * @param oldNode The node that was returned the last time the function was called
 * @param paintNodeData The QSGTransformNode associated with our item
 * @return
 */
QSGNode* DashedBar::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* paintNodeData)
{
    Q_UNUSED(paintNodeData)

    QSGNode* result = nullptr;

    if ((oldNode == nullptr) || _needToUpdateGeometry || _needToUpdateMaterial)
    {
        // Get our node
        QSGGeometryNode* node = nullptr;
        if (oldNode != nullptr)
        {
            // Reuse our previous node
            node = dynamic_cast<QSGGeometryNode*>(oldNode);
        }
        else
        {
            // Create a new node
            node = new QSGGeometryNode();
            if (node != nullptr)
            {
                // -- geometry
                QSGGeometry* fillGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0, 0);
                fillGeometry->setDrawingMode(QSGGeometry::DrawTriangles);
                node->setGeometry(fillGeometry);
                node->setFlag(QSGNode::OwnsGeometry);

                // -- materials
                QSGFlatColorMaterial* fillMaterial = new QSGFlatColorMaterial();
                node->setMaterial(fillMaterial);
                node->setFlag(QSGNode::OwnsMaterial);

                QSGFlatColorMaterial* fillOpaqueMaterial = new QSGFlatColorMaterial();
                node->setOpaqueMaterial(fillOpaqueMaterial);
                node->setFlag(QSGNode::OwnsOpaqueMaterial);
            }
        }


        // Check if we have a valid visual node
        if (node != nullptr)
        {
            // Check if we must update our materials
            if (_needToUpdateMaterial)
            {
                QSGFlatColorMaterial* material = dynamic_cast<QSGFlatColorMaterial *>(node->material());
                QSGFlatColorMaterial* opaqueMaterial = dynamic_cast<QSGFlatColorMaterial *>(node->opaqueMaterial());

                if (material != nullptr)
                {
                    material->setColor(_color);
                }

                if (opaqueMaterial != nullptr)
                {
                    opaqueMaterial->setColor(_color);
                }

                // Indicate that our materials have changed
                node->markDirty(QSGNode::DirtyMaterial);
                _needToUpdateMaterial = false;
            }


            // Check if we need to update our geometry
            if (_needToUpdateGeometry)
            {
                QSGGeometry* geometry = dynamic_cast<QSGGeometry *>(node->geometry());
                if (geometry != nullptr)
                {
                    int requiredNumberOfVertices = _vertices.count();
                    if (requiredNumberOfVertices != geometry->vertexCount())
                    {
                        geometry->allocate(requiredNumberOfVertices);
                    }

                    if (_vertices.count() > 0)
                    {
                        // Get vertices
                        QSGGeometry::Point2D* geometryVertices = geometry->vertexDataAsPoint2D();
                        for (int index = 0; index < _vertices.count(); index++)
                        {
                            QPointF point = _vertices[index];
                            geometryVertices[index].set(static_cast<float>(point.x()), static_cast<float>(point.y()));
                        }
                    }

                    // Indicate that our geometry has changed
                    node->markDirty(QSGNode::DirtyGeometry);
                    _needToUpdateGeometry = false;
                }
            }
        }


        result = node;
    }
    else
    {
        // Nothing has changed: we will reuse our previous node
        result = oldNode;
    }

    return result;
}


/**
 * @brief Called to perform any layout as required for this item
 */
void DashedBar::updatePolish()
{
    QQuickItem::updatePolish();

    // Check if we must update our list of vertices
    if (_needToUpdateVertices)
    {
        // Reset flag
        _needToUpdateVertices = false;

        // Compute vertices
        _computeVertices();

        // Indicates that we must update our geometry
        _needToUpdateGeometry = true;
    }
}


/**
 * @brief Called when our component is completed
 */
void DashedBar::componentComplete()
{
    // Don't forget to call the method of our parent class
     QQuickItem::componentComplete();

     // Schedules a polish event for this item
     polish();
}


/**
 * @brief Called when change occurs for this item
 * @param change
 * @param value
 */
void DashedBar::itemChange(ItemChange change, const ItemChangeData& value)
{
    // Don't forget to call the method of our parent class
    QQuickItem::itemChange(change, value);

    // Check if it's visibility has changed
    if (change == QQuickItem::ItemVisibleHasChanged)
    {
        // Check if our item is visible
        if (value.boolValue)
        {
            // Update our vertices
            needToUpdateVertices();
        }
    }
}


/**
 * @brief Compute a new list of vertices
 */
void DashedBar::_computeVertices()
{
    // Clear our previous list of vertices
    _vertices.clear();

    // Get dimensions of our item
    qreal currentWidth = width();
    qreal currentHeight = height();

    // Check if we need to draw something
    if (!qFuzzyIsNull(currentWidth) && !qFuzzyIsNull(currentHeight))
    {
        QVector<qreal> dashes;
        if (!_dashArray.isEmpty())
        {
            dashes = I2GraphicsHelpers::parseNumbersList(_dashArray);
            if ((dashes.count() & 1) != 0)
            {
                // If the dash count is odd, the dashes should be duplicated
                dashes <<  QVector<qreal>(dashes);
            }
        }

        // Check if we have dashes
        if (dashes.count() == 0)
        {
            // no dashes => we have a simple rectangle

            // Triangle 1
            _vertices.append(QPointF(0, 0));
            _vertices.append(QPointF(currentWidth, 0));
            _vertices.append(QPointF(0, currentHeight));

            // Triangle 2
            _vertices.append(QPointF(currentWidth, 0));
            _vertices.append(QPointF(currentWidth, currentHeight));
            _vertices.append(QPointF(0, currentHeight));
        }
        else
        {
            // We have dashes

            if (_orientation == Qt::Vertical)
            {
                // Vertical bar

                int indexDashes = 0;
                qreal y0 = 0;
                while ((y0 >= 0) && (y0 < currentHeight))
                {
                    // Get lengths of dash and gap
                    qreal lenghtDash = dashes[indexDashes % dashes.count()];
                    qreal lenghtGap = dashes[(indexDashes + 1) % dashes.count()];

                    // Compute position
                    qreal y1 = qMin(y0 + lenghtDash, currentHeight);

                    // Triangle 1
                    _vertices.append(QPointF(0, y0));
                    _vertices.append(QPointF(currentWidth, y0));
                    _vertices.append(QPointF(0, y1));

                    // Triangle 2
                    _vertices.append(QPointF(currentWidth, y0));
                    _vertices.append(QPointF(currentWidth, y1));
                    _vertices.append(QPointF(0, y1));


                    // Update values
                    indexDashes += 2;
                    y0 = y1 + lenghtGap;
                }
            }
            else
            {
                // Horizontal bar

                int indexDashes = 0;
                qreal x0 = 0;
                while ((x0 >= 0) && (x0 < currentWidth))
                {
                    // Get lengths of dash and gap
                    qreal lenghtDash = dashes[indexDashes % dashes.count()];
                    qreal lenghtGap = dashes[(indexDashes + 1) % dashes.count()];

                    // Compute position
                    qreal x1 = qMin(x0 + lenghtDash, currentWidth);

                    // Triangle 1
                    _vertices.append(QPointF(x0, 0));
                    _vertices.append(QPointF(x1, 0));
                    _vertices.append(QPointF(x0, currentHeight));

                    // Triangle 2
                    _vertices.append(QPointF(x1, 0));
                    _vertices.append(QPointF(x1, currentHeight));
                    _vertices.append(QPointF(x0, currentHeight));


                    // Update values
                    indexDashes += 2;
                    x0 = x1 + lenghtGap;
                }
            }
        }
    }
}
