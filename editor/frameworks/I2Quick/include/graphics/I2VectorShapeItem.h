/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef _I2VECTORSHAPE_H_
#define _I2VECTORSHAPE_H_

#include <QQuickItem>
#include <QPainter>
#include <QSGOpacityNode>
#include <QVector>
#include <QBasicTimer>

// To export symbols
#include "i2quick_global.h"


#include "graphics/I2QSGQPainterPathGeometryNode.h"
#include "graphics/I2Events.h"

#include "I2PropertyHelpers.h"


#define VECTOR_SHAPE_DEFAULT_IMPLICIT_WIDTH 100
#define VECTOR_SHAPE_DEFAULT_IMPLICIT_HEIGHT 100


/**
 * @brief The I2FillRule class is used to define an enum of fill-rules
 */
I2_ENUM(I2FillRule, NONZERO, EVENODD)


/**
 * @brief The I2LineCap class is used to define an enum of line caps
 * i.e. the shape to be used at the end of open subpaths when they are
 * stroked
 */
I2_ENUM(I2LineCap, BUTT, ROUND, SQUARE)


/**
 * @brief The I2LineJoin class is used to define an enum of line joins
 * i.e. the shape to be used at the corners of paths or basic shapes when
 * they are stroked
 */
I2_ENUM(I2LineJoin, MITER, ROUND, BEVEL)



/**
 * @brief The I2VectorShape class is the base class of all vector shapes
 * (rectangle, circle, ellipse, polyline, path, etc.)
 */
class I2QUICK_EXPORT I2VectorShapeItem : public QQuickItem
{
    Q_OBJECT

    I2_QML_PROPERTY_CUSTOM_SETTER(QColor, fill)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, fillOpacity)
    I2_QML_PROPERTY_CUSTOM_SETTER(I2FillRule::Value, fillRule)

    I2_QML_PROPERTY_CUSTOM_SETTER(QColor, stroke)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, strokeOpacity)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, strokeWidth)
    I2_QML_PROPERTY_CUSTOM_SETTER(I2LineCap::Value, strokeLineCap)
    I2_QML_PROPERTY_CUSTOM_SETTER(I2LineJoin::Value, strokeLineJoin)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, strokeMiterLimit)
    I2_QML_PROPERTY_CUSTOM_SETTER(QString, strokeDashArray)
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, strokeDashOffset)

    /**
     * Indicates if our geometry is defined by svg properties (specific to each shape type)
     * or QML properties (x, y, width, height)
     */
    I2_QML_PROPERTY_CUSTOM_SETTER (bool, useSvgGeometry)

    /**
     * Length of our shape
     */
    I2_QML_PROPERTY_READONLY_FUZZY_COMPARE(qreal, length)

    /**
     * Defines whether mouse events are handled or not
     */
    I2_QML_PROPERTY_CUSTOM_SETTER (bool, mouseEnabled)

    /**
     * Defines whether mouse hover events are handled or not
     */
    I2_QML_PROPERTY_CUSTOM_SETTER (bool, hoverEnabled)

    /**
     * Flag indicating whether any of the mouse buttons are currently pressed
     */
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER (bool, pressed)

    /**
     * This property holds the mouse buttons currently pressed
     */
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER (Qt::MouseButtons, pressedButtons)

    /**
     * This is a convenience property equivalent to pressed && containsMouse
     */
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER (bool, containsPress)

    /**
     * Flag indicating whether the mouse is currently inside our shape
     *
     * NB: MouseArea uses "hoveredChanged" instead of "containsMouseChanged"
     *     To ease interoperability, both signals will be emitted
     */
    I2_QML_PROPERTY_READONLY_CUSTOM_SETTER (bool, containsMouse)

    /**
     * Scale factor applied before triangulation of our fill geometry
     * It can be used to obtain better results i.e. to produce more triangles
     * and thus our shape will support a greater zoom factor
     */
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, fillTriangulationScaleFactor)

    /**
     * Scale factor applied before triangulation of our stroke geometry
     * It can be used to obtain better results i.e. to produce more triangles
     * and thus our shape will support a greater zoom factor
     */
    I2_QML_PROPERTY_CUSTOM_SETTER(qreal, strokeTriangulationScaleFactor)

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2VectorShapeItem(QQuickItem *parent = 0);


   /**
    * @brief Destructor
    */
    ~I2VectorShapeItem();


    /**
      * @brief Check if a given point (in local coordinates) is inside our shape
      * @param point
      * @return True if point is inside our shape, false otherwise
      */
    virtual bool contains(const QPointF &point) const Q_DECL_OVERRIDE;


Q_SIGNALS:

    /**
     * Signals used to notify mouse events
     */
    void pressed(I2MouseEvent *mouse);
    void positionChanged(I2MouseEvent *mouse);
    void released(I2MouseEvent *mouse);
    void clicked(I2MouseEvent *mouse);
    void doubleClicked(I2MouseEvent *mouse);
    void canceled();
    void pressAndHold();


    /**
     * Signals used to notify hover events
     */
    void entered();
    void exited();
    void hoveredChanged(bool value);


protected Q_SLOTS:
    /**
     * @brief Called when our antialiasing property has changed
     * @param value
     */
    void _antialiasingChanged(bool value);


protected:
        /**
         * @brief Check if our pressAndHold signal is connected to any slot
         * @return
         */
        bool isPressAndHoldConnected();

        /**
         * @brief Check if our doubleClicked signal is connected to any slot
         * @return
         */
        bool isDoubleClickedConnected();


       /**
        * @brief Called on the render thread when it is time to draw our item in the scene graph.
        *  The function is called as a result of QQuickItem::update()
        * @param oldNode The node that was returned the last time the function was called
        * @param paintNodeData The QSGTransformNode associated with our item
        * @return
        */
       QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *paintNodeData) Q_DECL_OVERRIDE;


       /**
        * @brief Called when our main geometry (x, y, width, height) has changed
        * @param newGeometry New bounding box of our item
        * @param oldGeometry Previous bounding box of our item
        */
       void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) Q_DECL_OVERRIDE;


       /**
        * @brief This function is called to update the QML geometry
        *  (x, y, width, height, implicitWidth, implicitHeight) when
        *  our shape has changed.
        *
        *  Derived class should reimplement this function
        */
       virtual void updateQmlGeometryProperties();

       /**
        * @brief This function is called to update SVG properties that
        *  the geometry of our shape when its QML geometry has changed
        *
        *  Derived class should reimplement this function
        */
       virtual void updateSvgGeometryProperties();

       /**
        * @brief this function update our shape path based on our properties
        *
        * Derived class should reimplement this function
        */
       virtual void updateShapeQPainterPath();


       /**
        * @brief Derived class must call this function when they change the shape (QPainterPath) of our item
        */
       void shapeQPainterPathChanged();


       /**
        * @brief This function is used to compute our stroked shape
        */
       void updateStrokedShapeQPainterPath();


       /**
        * @brief Resize a QPainterPath to fit a given bounding box
        * @param pathToResize
        * @param x
        * @param y
        * @param width
        * @param height
        * @param keepAspectRatio
        * @return
        */
       QPainterPath resizeQPainterPath(const QPainterPath &pathToResize, qreal x, qreal y, qreal width, qreal height, bool keepAspectRatio);


       /**
        * @brief Check if our shape is filled
        * @return
        */
       bool isFilled() const;


       /**
        * @brief Check if our shape is stroked
        * @return
        */
       bool isStroked() const;


       //----------------------------------------------
       //
       // Mouse events
       //
       //----------------------------------------------
       void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
       void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
       void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
       void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
       void mouseUngrabEvent() Q_DECL_OVERRIDE;


       //---------------------------------------------
       //
       // Hover events
       //
       //---------------------------------------------
       void hoverEnterEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
       void hoverMoveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;
       void hoverLeaveEvent(QHoverEvent *event) Q_DECL_OVERRIDE;


       //---------------------------------------------
       //
       // Timer event used for long press, etc.
       //
       //---------------------------------------------
       void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;



       //---------------------------------------------
       //
       // Touch events
       //
       //---------------------------------------------
        void touchEvent(QTouchEvent *) Q_DECL_OVERRIDE;
        void touchUngrabEvent() Q_DECL_OVERRIDE;




       /**
        * @brief Called when our QML component is completed
        */
       void componentComplete() Q_DECL_OVERRIDE;

protected:
    QPainterPath _shapeQPainterPath;
    QPainterPath _strokedShapeQPainterPath;

    bool _needToUpdateFillGeometry;
    bool _needToUpdateFillMaterial;

    bool _needToUpdateStrokeGeometry;
    bool _needToUpdateStrokeMaterial;

    bool _willTrigerInternalGeometryChanged;

    bool _longPressTriggered;
    bool _doubleClickTriggered;
    QBasicTimer _pressAndHoldTimer;
};

#endif // _I2VECTORSHAPE_H_
