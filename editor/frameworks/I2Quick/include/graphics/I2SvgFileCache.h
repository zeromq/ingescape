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

#ifndef _I2SVGFILE_H_
#define _I2SVGFILE_H_

#include <QQuickItem>

#include <QSvgRenderer>
#include <QPainter>
#include <QMatrix4x4>


// To export symbols
#include "i2quick_global.h"


#include "I2PropertyHelpers.h"




/**
 * @brief The I2SvgFileCache class is used to store the parsing result of a given SVG file.
 *        It should be used with I2SvgItems to avoid multiple parsing of the same SVG file.
 */
class I2QUICK_EXPORT I2SvgFileCache : public QQuickItem
{
    Q_OBJECT

    // URL of our SVG file
    I2_QML_PROPERTY_CUSTOM_SETTER(QUrl, svgFile)

    // Viewbow defined in our SVG file
    I2_QML_PROPERTY_READONLY(int, viewboxX)
    I2_QML_PROPERTY_READONLY(int, viewboxY)
    I2_QML_PROPERTY_READONLY(int, viewboxWidth)
    I2_QML_PROPERTY_READONLY(int, viewboxHeight)

    // Flag indicating if our SVG file is loaded
    I2_QML_PROPERTY_READONLY(bool, svgFileLoaded)


public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit I2SvgFileCache(QQuickItem *parent = 0);


    /**
     * @brief Destructor
     */
    ~I2SvgFileCache();


    /**
     * @brief Get the SVG renderer associated to our cache
     * @return
     */
    QSvgRenderer* svgRenderer();


public Q_SLOTS:
    /**
     * @brief Check if a given SVG element exists in our SVG file
     *
     * @param svgElementID ID of an SVG element
     * @param extraQMLargumentsForUpdates
     *
     * @return Returns true if the element with the given SVG ID exists in the currently parsed SVG file and is a renderable element (i.e. not a gradient, clip, mask, etc.)
     */
    bool elementExists(QString svgElementID, bool extraQMLargumentsForUpdates = false);


    /**
     * @brief Get the bounding of a given SVG element
     *
     * @param svgElementID ID of an SVG element
     * @param extraQMLargumentsForUpdates
     *
     * @return Returns bounding rectangle of the SVG element with the given ID if it exists. Empty rectangle, otherwise.
     */
    QRectF boundsOnElement(QString svgElementID, bool extraQMLargumentsForUpdates = false);


    /**
     * @brief Get the transforamtion matrix of a given SVG element
     *
     * @param svgElementID ID of an SVG element
     * @param extraQMLargumentsForUpdates
     *
     * @return Returns the transformation matrix for the element with the given ID.
     *  The matrix is a product of the transformation of the element's parents.
     *  The transformation of the element itself is not included.
     */
    QMatrix4x4 matrixForElement(QString svgElementID, bool extraQMLargumentsForUpdates = false);


protected:
    /**
     * @brief Try to find a given SVG element based on its name
     *
     * @param svgElementID ID of an SVG element
     *
     * @return
     */
    QString _tryToFindElement(QString svgElementID);


protected:
    // Renderer that stores the parsing result of our SVG file
    QSvgRenderer* _svgRenderer;
};


QML_DECLARE_TYPE(I2SvgFileCache)

#endif // _I2SVGFILE_H_
