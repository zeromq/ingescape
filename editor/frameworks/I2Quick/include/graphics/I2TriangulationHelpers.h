/*
 *	I2Quick
 *
 *  Copyright (c) 2016-2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef I2TRIANGULATIONHELPERS_H
#define I2TRIANGULATIONHELPERS_H

#include <QObject>
#include <QList>
#include <QVector>

#include <QSGGeometry>

// To export symbols
#include "i2quick_global.h"




/**
 * @brief The I2TriangluationResult struct is used to store the result of a triangulation
 */
struct I2TriangulationResult {
    // Drawing mode
    GLenum drawingMode;

    // Vertices
    QList<QPointF> vertices;

    // Indices of our vertices
    QVector<quint32> indices;
};





/**
 * @brief The I2TriangulationHelpers class defines a set of utility functions to triangulate shapes
 */
class I2QUICK_EXPORT I2TriangulationHelpers : public QObject
{
    Q_OBJECT


public:
    explicit I2TriangulationHelpers(QObject *parent = 0);


    /**
     * @brief Compute vertices for a given polyline
     *
     * @param polylinePoints
     * @param strokeWidth
     * @param strokeLinecap
     * @param drawingMode Must be GL_TRIANGLES or GL_TRIANGLE_STRIP
     *
     * @return List of vertices
     */
    static QList<QPointF> computeVerticesForPolyline(QList<QPointF> polylinePoints, qreal strokeWidth, Qt::PenCapStyle strokeLinecap = Qt::FlatCap, GLenum drawingMode = GL_TRIANGLE_STRIP);


    /**
     * @brief Compute geometry for a given dahsed polyline
     *
     * @param polylinePoints
     * @param strokeWidth
     * @param strokeDashArray
     * @param strokeDashOffset
     *
     * @return Triangulation result (vertices and indexes)
     */
    static I2TriangulationResult computeGeometryForDashedPolyline(QList<QPointF> polylinePoints, qreal strokeWidth, QString strokeDashArray, qreal strokeDashOffset = 0.0, Qt::PenCapStyle strokeLinecap = Qt::FlatCap);



    /**
     * @brief Compute a round linecap
     * @param center center of our round linecap
     * @param previousOrNextPoint Previous (end linecap) or next point (start linecap) in our polyline: it is used to compute a direction
     * @param radius radius of our round linecap
     * @param vertex1
     * @param vertex2
     * @param drawingMode Must be GL_TRIANGLES or GL_TRIANGLE_STRIP
     *
     * @return
     */
    static QList<QPointF> computeRoundLinecap(QPointF center, QPointF previousOrNextPoint, qreal radius, QPointF vertex1, QPointF vertex2, GLenum drawingMode = GL_TRIANGLE_STRIP);


    /**
     * @brief Compute vertices of the exterior offset contours for a given polyline
     *
     * @param polyline
     * @param offset (offset must be positive)
     * @param transparentVertices
     *
     * @return List of vertices (coordinates, true if vertex must be transparent)
     */
    static QList<QPair<QPointF, bool>> computeVerticesForExteriorOffsetContour(QList<QPointF> polyline, qreal offset, bool transparentVertices);


    /**
     * @brief Compute vertices of the interior offset contours for a given polyline
     *
     * @param polyline
     * @param offset (offset must be positive)
     * @param transparentVertices
     *
     * @return List of vertices (coordinates, true if vertex must be transparent)
     */
    static QList<QPair<QPointF, bool>> computeVerticesForInteriorOffsetContour(QList<QPointF> polyline, qreal offset, bool transparentVertices);



protected:

    /**
     * @brief Compute vertices of the interior offset contours for a given polyline
     *
     * @param polyline
     * @param offset (offset must be positive)
     * @param computeExteriorOffsetContour
     * @param computeInteriorOffsetContour
     * @param transparentVertices
     *
     * @return List of vertices (coordinates, true if vertex must be transparent)
     */
    static QList<QPair<QPointF, bool>> _computeVerticesForOffsetContour(QList<QPointF> polyline, qreal offset, bool computeExteriorOffsetContour, bool computeInteriorOffsetContour, bool transparentVertices);


    /**
     * @brief Compute a smooth round linejoin
     *
     * @param center
     * @param p0
     * @param p1
     * @param nextPointInLine
     * @param transparentVertices
     *
     * @return
     */
    static QList<QPair<QPointF, bool>> _computeSmoothRoundLineJoin(QPointF center, QPointF p0, QPointF p1, QPointF nextPointInLine, bool transparentVertices);


    /**
     * @brief Clean a given polyline (remove redundant points)
     * @param polygon
     * @return
     */
    static QList<QPointF> _cleanPolyline(QList<QPointF> polyline);


    /**
     * @brief Check orientation of a given polyline
     * @param polyline
     * @return
     */
    static bool _checkPolylineIsClockwise(QList<QPointF> polyline);


    /**
     * @brief Check if a given point is between to other points
     *
     * @param point
     * @param pointA
     * @param pointB
     *
     * @return
     */
    static bool _pointIsInSegment(QPointF point, QPointF pointA, QPointF pointB);



    /**
     * @brief Compute the intersection point between two line segments
     *
     * @param p0
     * @param p1
     * @param p2
     * @param p3
     * @param intersection
     *
     * @return
     */
    static bool _computeIntersection(QPointF p0, QPointF p1, QPointF p2, QPointF p3, QPointF& intersection);

//FIXME
public:
    /**
     * @brief Offset a given polyline
     *
     * @param polyline
     * @param offset
     *
     * @return
     */
    static QList<QPointF> _shiftPolylineCoordinates(QList<QPointF> polyline, qreal offset);



    /**
     * @brief Compute the angle between three points (p0, p1, p2) and the bisector angle
     *
     * @param p0
     * @param p1
     * @param p2
     *
     * @return
     */
    static QPointF _vertexAngleAndAngleBisector(QPointF p0, QPointF p1, QPointF p2);




};

#endif // I2TRIANGULATIONHELPERS_H
