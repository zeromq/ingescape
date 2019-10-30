/*
 *	I2Quick
 *
 *  Copyright (c) 2017-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */


#ifndef I2MATHUTILS_H
#define I2MATHUTILS_H

#include <QObject>
#include <QPointF>
#include <QLineF>
#include <QRectF>

// To export symbols
#include "i2quick_global.h"


/**
 * @brief The I2StringUtils class defines a set of math utility functions
 */
class I2QUICK_EXPORT I2MathUtils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2MathUtils(QObject *parent = nullptr);


    /**
     * @brief Get projected point P' of point P on line L
     * @param P
     * @param L
     * @return
     */
    static QPointF getProjectedPointOnLine(QPointF P, QLineF L);


    /**
     * @brief Get the relative position in % (1 = 100%) of the projected point P' of point P on line L
     * @param P
     * @param L
     * @return
     */
    static qreal getRelativePositionOfProjectedPointOnLine(QPointF P, QLineF L);


    /**
     * @brief Get the intersection between a line segment and a rectangle
     * @param line
     * @param rectangle
     * @param ok Flag indicating if we have an intersection
     *
     * @return
     */
    static QLineF intersectionBetweenLineAndRectangle(QLineF line, QRectF rectangle, bool &ok);


};

#endif // I2MATHUTILS_H
