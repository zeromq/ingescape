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

#ifndef I2SVGVECTORFILECACHE_H
#define I2SVGVECTORFILECACHE_H

#include <QObject>
#include <QQuickItem>

// To export symbols
#include "i2quick_global.h"


#include "I2PropertyHelpers.h"

#include "graphics/I2SimpleSvgParser.h"



/**
 * @brief The I2SvgVectorFileCache class defines a cache to store the result of an SVG parsing
 *        It will be used to render vector items
 */
class I2QUICK_EXPORT I2SvgVectorFileCache : public QObject
{
    Q_OBJECT

    // URL of our SVG file
    I2_QML_PROPERTY_CUSTOM_SETTER(QUrl, svgFile)

    // Viewbow defined in our SVG file
    I2_QML_PROPERTY_READONLY(int, viewboxX)
    I2_QML_PROPERTY_READONLY(int, viewboxY)
    I2_QML_PROPERTY_READONLY(int, viewboxWidth)
    I2_QML_PROPERTY_READONLY(int, viewboxHeight)

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2SvgVectorFileCache(QObject* parent = 0);


    /**
     * @brief Destructor
     */
    ~I2SvgVectorFileCache();


    /**
     * @brief Get a given SVG element
     * @param svgElementId
     * @return
     */
    I2SimpleSvgItemModel* getSvgElement(QString svgElementId);


    /**
     * @brief Get the root element of our SVG file
     * @return
     */
    I2SimpleSvgRootModel* getSvgRoot();

protected:
    I2SimpleSvgParser *_svgParser;
};

QML_DECLARE_TYPE(I2SvgVectorFileCache)

#endif // I2SVGVECTORFILECACHE_H
