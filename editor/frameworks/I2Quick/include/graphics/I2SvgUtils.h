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


#ifndef _I2SVGUTILS_H_
#define _I2SVGUTILS_H_

#include <QObject>
#include <QtQml>
#include <QQmlEngine>
#include <QJSEngine>

#include <QPair>

// To export symbols
#include "i2quick_global.h"



/**
 * @brief The I2SvgUtils class defines a set of utility functions
 */
class I2QUICK_EXPORT I2SvgUtils : public QObject
{
    Q_OBJECT

public:
    explicit I2SvgUtils(QObject* parent = nullptr);

    /**
     * @brief Method used to provide a singleton to QML
     * @param engine
     * @param scriptEngine
     * @return
     */
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


public Q_SLOTS:

    /**
     * @brief Get a SVG path representation of a given rectangle
     * @param x
     * @param y
     * @param width
     * @param height
     * @return
     */
    QString pathForRectangle(qreal x, qreal y, qreal width, qreal height);


    /**
     * @brief Get a SVG path representation of a given rounded rectangle
     * @param x
     * @param y
     * @param width
     * @param height
     * @return
     */
    QString pathForRoundedRectangle(qreal x, qreal y, qreal width, qreal height, qreal radiusX, qreal radiusY);



    /**
     * @brief Get a SVG path representation of a given circle
     * @param centerX
     * @param centerY
     * @param radius
     * @return
     */
    QString pathForCircle(qreal center, qreal centerY, qreal radius);


    /**
     * @brief Get a SVG path representation of a given ellipse
     * @param centerX
     * @param centerY
     * @param radiusX
     * @param radiusY
     * @return
     */
    QString pathForEllipse(qreal centerX, qreal centerY, qreal radiusX, qreal radiusY);

    /**
     * @brief Get a SVG path representation of a given line
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    QString pathForLine(qreal x1, qreal y1, qreal x2, qreal y2);


    /**
     * @brief Get a SVG path representation of a given polyline
     * @param points
     * @return
     */
    QString pathForPolyline(QString points);


    /**
     * @brief Get a SVG path representation of a given polygon
     * @param points
     * @return
     */
    QString pathForPolygon(QString points);


    /**
     * @brief Get a list of points (QPointF) from a SVG polyline description
     * @param points
     * @return
     */
    QList<QPointF> pointsListForPolyline(QString points);


    /**
     * @brief Get a SVG path representation of a given list of points
     * @param pointsList
     * @return
     */
    QString pathForPolylinePointsList(QList<QPointF> pointsList);


    /**
     * @brief Get the first point of a given path
     * @param path
     * @return
     */
    QPointF getFirstPointOfPath(QString path);


    /**
     * @brief Get the last point of a given path
     * @param path
     * @return
     */
    QPointF getLastPointOfPath(QString path);


    /**
     * @brief Get the first point and the last point of a given path
     * @param path
     * @return
     */
    QPair<QPointF, QPointF> getFirstAndLastPointsOfPath(QString path);


    /**
     * @brief Convert a given SVG ID into an Adobe Illustrator ID
     * @param svgId
     * @return
     */
    QString convertSvgIdToAdobeIllustratorId(QString svgId);


    /**
     * @brief Convert a given Adobe Illustrator ID into a SVG ID
     * @param adobeIllustratorId
     * @return
     */
    QString convertAdobeIllustratorIdToSvgId(QString adobeIllustratorId);


public:

    /**
     * @brief Get a SVG path representation of a given rectangle
     * @param x
     * @param y
     * @param width
     * @param height
     * @return
     */
    static QString static_pathForRectangle(qreal x, qreal y, qreal width, qreal height);


    /**
     * @brief Get a SVG path representation of a given rounded rectangle
     * @param x
     * @param y
     * @param width
     * @param height
     * @return
     */
    static QString static_pathForRoundedRectangle(qreal x, qreal y, qreal width, qreal height, qreal radiusX, qreal radiusY);



    /**
     * @brief Get a SVG path representation of a given circle
     * @param centerX
     * @param centerY
     * @param radius
     * @return
     */
    static QString static_pathForCircle(qreal center, qreal centerY, qreal radius);


    /**
     * @brief Get a SVG path representation of a given ellipse
     * @param centerX
     * @param centerY
     * @param radiusX
     * @param radiusY
     * @return
     */
    static QString static_pathForEllipse(qreal centerX, qreal centerY, qreal radiusX, qreal radiusY);

    /**
     * @brief Get a SVG path representation of a given line
     * @param x1
     * @param y1
     * @param x2
     * @param y2
     * @return
     */
    static QString static_pathForLine(qreal x1, qreal y1, qreal x2, qreal y2);


    /**
     * @brief Get a SVG path representation of a given polyline
     * @param points
     * @return
     */
    static QString static_pathForPolyline(QString points);


    /**
     * @brief Get a SVG path representation of a given polygon
     * @param points
     * @return
     */
    static QString static_pathForPolygon(QString points);


    /**
     * @brief Get a list of points (QPointF) from a SVG polyline description
     * @param points
     * @return
     */
    static QList<QPointF> static_pointsListForPolyline(QString points);


    /**
     * @brief Get a SVG path representation of a given list of points
     * @param pointsList
     * @return
     */
    static QString static_pathForPolylinePointsList(QList<QPointF> pointsList);


    /**
     * @brief Get the first point of a given path
     * @param path
     * @return
     */
    static QPointF static_getFirstPointOfPath(QString path);


    /**
     * @brief Get the last point of a given path
     * @param path
     * @return
     */
    static QPointF static_getLastPointOfPath(QString path);


    /**
     * @brief Get the first point and the last point of a given path
     * @param path
     * @return
     */
    static QPair<QPointF, QPointF> static_getFirstAndLastPointsOfPath(QString path);


    /**
     * @brief Convert a given SVG ID into an Adobe Illustrator ID
     * @param svgId
     * @return
     */
    static QString static_convertSvgIdToAdobeIllustratorId(QString svgId);


    /**
     * @brief Convert a given Adobe Illustrator ID into a SVG ID
     * @param adobeIllustratorId
     * @return
     */
    static QString static_convertAdobeIllustratorIdToSvgId(QString adobeIllustratorId);

};


QML_DECLARE_TYPE(I2SvgUtils)

#endif // _I2SVGUTILS_H
