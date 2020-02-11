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

#ifndef _I2QSGQPAINTERPATHGEOMETRYNODE_H_
#define _I2QSGQPAINTERPATHGEOMETRYNODE_H_

#include <QSGNode>
#include <QSGGeometryNode>

#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QSGVertexColorMaterial>

#include <QPainterPath>

// To export symbols
#include "i2quick_global.h"



/**
 * @brief The I2QSGQPainterPathGeometryNode class defines a custom geometry node
 *  that uses a QPainterPath to define its geometry
 */
class I2QUICK_EXPORT I2QSGQPainterPathGeometryNode : public QSGGeometryNode
{
public:
    /**
     * @brief Default constructor
     */
    explicit I2QSGQPainterPathGeometryNode();


    /**
     * @brief Destructor
     */
    ~I2QSGQPainterPathGeometryNode();


    /**
     * @brief Set our geometry from a QPainterPath
     * @param path triangulationScaleFactor Scale factor applied to our path before it is triangulated
     * @param antialiasingEnabled Flag indicating if antialiasing is enabled or not
     */
    void setGeometryFromPath(const QPainterPath &path, qreal triangulationScaleFactor = 2, bool antialiasingEnabled = false);


    /**
     * @brief Set color of our material
     * @param color
     */
    void setColor(const QColor &color);


    /**
     * @brief Get the number of vertices created by our application
     * @return
     *
     * NB: debug only
     */
    static int globalNumberOfVertices();


    /**
     * @brief Get the number of vertices used for antialiasing by our application
     * @return
     *
     * NB: debug only
     */
    static int antialiasingNumberOfVertices();


private:
    /**
     * @brief Set our main geometry
     * @param path
     * @param triangulationScaleFactor
     */
    void _setMainGeometry(const QPainterPath &path, qreal triangulationScaleFactor);

private:
    // Geometry of our node
    QSGGeometry _geometry;
    // Materials used by our node
    QSGFlatColorMaterial _material;
    QSGFlatColorMaterial _opaqueMaterial;


    // Flag indicating if our antialiasing node is displayed or not
    bool _antialiasingNodeDisplayed;
    // Additional node used for antialiasing
    QSGGeometryNode _antialiasingNode;
    // Geometry of our antialiasing node
    QSGGeometry _antialiasingGeometry;
    // Materials of our antialiasing node
    QSGVertexColorMaterial _antialiasingMaterial;
    QSGVertexColorMaterial _antialiasingOpaqueMaterial;
};

#endif // _I2QSGQPAINTERPATHGEOMETRYNODE_H_
