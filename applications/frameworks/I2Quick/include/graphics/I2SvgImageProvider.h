/*
 *	I2Quick
 *
 *  Copyright (c) 2015-2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *		Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef _I2SVGIMAGEPROVIDER_H_
#define _I2SVGIMAGEPROVIDER_H_

#include <QQuickImageProvider>
#include <QSvgRenderer>
#include <QHash>

// To export symbols
#include "i2quick_global.h"



/**
 * @brief The I2SVGImageProvider class defines a custom image provider to use SVG content as QML images
 */
class I2QUICK_EXPORT I2SVGImageProvider : public QQuickImageProvider
{
public:

    /**
     * @brief Default constructor
     */
    explicit I2SVGImageProvider();


    /**
     * @brief Destructor
     */
    ~I2SVGImageProvider();


    /**
     * @brief Return the size of the image returned by a given request
     * @param id
     * @return
     */
    static QSize requestImageSize(const QString &id);


    /**
     * @brief Request a QImage
     * @param id
     * @return
     */
    static QImage requestQImage(const QString &id, QSize *size, const QSize &requestedSize);


    /**
     * @brief Return the requested SVG element as a QImage
     * @param id  (Path of SVG file)#(SVG element ID)?(Options: fillColor=Color  and/or opaque=1 or 0 and/or scale=scaleFactor)
     * @param size
     * @param requestedSize
     * @return
     */
    virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);


    /**
     * @brief Return the requested SVG element as a QPixmap
     * @param id  (Path of SVG file)#(SVG element ID)?(Options: fillColor=Color  and/or opaque=1 or 0 and/or scale=scaleFactor)
     * @param size
     * @param requestedSize
     * @return
     */
    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);


    /**
     * @brief Flag used to indicate if we must use a cache for SVG parsing
     * @remark True by default
     */
    static bool useSvgFilesCache;


    /**
     * @brief Flag used to indicate if we must generate error images
     * @remark False by default
     */
    static bool showErrorsAsImage;


    /**
     * @brief Clear our cache of SVG files
     */
    static void clearSvgFilesCache();


private:
    /**
     * @brief Compute an image size
     * @param defaultSize
     * @param requestedSize
     * @param width
     * @param height
     * @param scale
     * @return (size, canUseDevicePixelRatio)
     */
    static QPair<QSize, bool> _computeImageSize(const QSize& defaultSize, const QSize& requestedSize, int width = -1, int height = -1, qreal scale = 1.0);


    /**
     * @brief Get the SVG renderer for a given SVG file
     * @param svgFile
     * @return
     */
    static QSvgRenderer *_getSvgRenderer(const QString &svgFile);


    /**
     * @brief Build an error image
     * @param errorMessage
     * @param size
     * @return
     */
    static QImage _buildErrorImage(const QString& errorMessage, const QSize& size = QSize(100, 100));


    /**
     * @brief Build an empty image
     * @param size
     * @return
     */
    static QImage _buildEmptyImage(const QSize& size = QSize(100, 100));


protected:
    // Cache for SVG files
    static QHash<QString, QSvgRenderer *> _static_svgFilesCache;

    // Debug image default size
    static QSize _static_debugImageSize;
};

#endif // _I2SVGIMAGEPROVIDER_H_
