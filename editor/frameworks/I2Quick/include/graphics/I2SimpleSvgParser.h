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

#ifndef I2SIMPLESVGPARSER_H
#define I2SIMPLESVGPARSER_H

#include <QXmlDefaultHandler>

#include <QObject>
#include <QTransform>

// To export symbols
#include "i2quick_global.h"

#include "I2PropertyHelpers.h"
#include "graphics/I2VectorShapeItem.h"


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgItemModel
///
////////////////////////////////////////////////////////////////

// Types of SVG item models
I2_ENUM(I2SimpleSvgItemType, SvgRoot, SvgGroup, SvgImage, SvgText, SvgShape)


/**
 * @brief The I2SimpleSvgItemModel class is the base class of SVG item models
 */
class I2QUICK_EXPORT I2SimpleSvgItemModel : public QObject
{
    Q_OBJECT

    // UID of our SVG item (clean-up version without Adobe Illustrator addons)
    I2_CPP_PROPERTY(QString, uid)

    // Raw UID of our SVG item
    I2_CPP_PROPERTY(QString, rawUid)

    // Type of our SVG item
    I2_CPP_PROPERTY(I2SimpleSvgItemType::Value, type)

    // Opacity of our SVG item
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, opacity)

    // Transformation of our SVG item
    I2_CPP_PROPERTY(QTransform, transform)

    // Visibile flag of our SVG item
    I2_CPP_PROPERTY(bool, visible)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param type
     * @param parent
     */
    explicit I2SimpleSvgItemModel(QString uid, I2SimpleSvgItemType::Value type, QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgItemModel();

protected:
    /**
     * @brief Clean-up a UID i.e. remove Adobe Illustrator add-ons
     * @return
     */
    QString _cleanUpUid(QString uid);
};



////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgGroupModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgGroupModel class is used to represent a SVG group
 */
class I2QUICK_EXPORT I2SimpleSvgGroupModel : public I2SimpleSvgItemModel
{
    Q_OBJECT

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgGroupModel(QString uid, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~I2SimpleSvgGroupModel();
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgRootModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgRootModel class is used to represent the root element of an SVG
 */
class I2QUICK_EXPORT I2SimpleSvgRootModel : public I2SimpleSvgGroupModel
{
    Q_OBJECT

    // Abscissa of the top-left corner of the bounding box of our SVG
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, x)

    // Ordinate of the top-left corner of the bounding box of our SVG
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, y)

    // Width of the bounding box of our SVG
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, width)

    // Height of the bounding box of our SVG
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, height)

public:
    /**
     * @brief Default constructor
     * @param parent
     */
    explicit I2SimpleSvgRootModel(QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgRootModel();
};



////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgImageModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgImageModel class is used to represent a SVG image
 */
class I2QUICK_EXPORT I2SimpleSvgImageModel : public I2SimpleSvgItemModel
{
    Q_OBJECT

    // Source of our image
    I2_CPP_PROPERTY(QUrl, source)

    // Abscissa of the top-left corner of our image
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, x)

    // Ordinate of the top-left corner of our image
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, y)

    // Width of our image
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, width)

    // Height of our image
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, height)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgImageModel(QString uid, QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgImageModel();
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgTextModel
///
////////////////////////////////////////////////////////////////

// Font-weight values
I2_ENUM(SvgFontWeight, SvgFontWeightNormal, SvgFontWeightBold)

// Font-style values
I2_ENUM(SvgFontStyle, SvgFontStyleNormal, SvgFontStyleItalic, SvgFontStyleOblique)


/**
 * @brief The I2SimpleSvgTextModel class is used to represent a SVG text
 */
class I2QUICK_EXPORT I2SimpleSvgTextModel : public I2SimpleSvgItemModel
{
    Q_OBJECT

    // Text associated to our SVG text item
    I2_CPP_PROPERTY(QString, text)

    // Abscissa of our anchor point
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, x)

    // Ordinate of our anchor point
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, y)

    // Fill
    I2_CPP_PROPERTY(QColor, fill)

    // Fill-opacity
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, fillOpacity)

    // Font
    I2_CPP_PROPERTY(QFont, font)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgTextModel(QString uid, QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgTextModel();
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgShapeModel
///
////////////////////////////////////////////////////////////////

// Subtypes of SVG shape item models
I2_ENUM(I2SimpleSvgShapeType, SvgRectangle, SvgCircle, SvgEllipse, SvgLine, SvgPath, SvgPolyline)


/**
 * @brief The I2SimpleSvgShapeModel class is the base class of SVG shape items
 */
class I2QUICK_EXPORT I2SimpleSvgShapeModel : public I2SimpleSvgItemModel
{
    Q_OBJECT

    // Shape type
    I2_CPP_PROPERTY(I2SimpleSvgShapeType::Value, shapeType)

    // Fill color
    I2_CPP_PROPERTY(QColor, fill)

    // Fill opacity
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, fillOpacity)

    // Fill rule
    I2_CPP_PROPERTY(I2FillRule::Value, fillRule)

    // Stroke color
    I2_CPP_PROPERTY(QColor, stroke)

    // Stroke opacity
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, strokeOpacity)

    // Stroke width
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, strokeWidth)

    // Stroke linecap - the shape to be used at the end of open subpaths when they are stroked
    I2_CPP_PROPERTY(I2LineCap::Value, strokeLineCap)

    // Stroke linejoin - the shape to be used at the corners of paths or basic shapes when they are stroked
    I2_CPP_PROPERTY(I2LineJoin::Value, strokeLineJoin)

    // Stroke miter limit - to configure the 'miter' linejoin
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, strokeMiterLimit)

    // Stroke dash array - the pattern of dashes and gaps used to stroke paths
    I2_CPP_PROPERTY(QString, strokeDashArray)

    // Stroke dash offset - the distance into the dash pattern to start the dash
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, strokeDashOffset)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param shapeType
     * @param parent
     */
    explicit I2SimpleSvgShapeModel(QString uid, I2SimpleSvgShapeType::Value shapeType, QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgShapeModel();


    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    virtual QString toSVGPath() = 0;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgRectangleModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgRectangleModel class is used to represent a rectangle
 */
class I2QUICK_EXPORT I2SimpleSvgRectangleModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Abscissa of the top-left corner of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, x)

    // Ordinate of the top-left corner of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, y)

    // Width of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, width)

    // Height of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, height)

    // x-axis radius of the ellipse used to round off the corners of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, radiusX)

    // y-axis radius of the ellipse used to round off the corners of our rectangle
    I2_CPP_PROPERTY_FUZZY_COMPARE(qreal, radiusY)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgRectangleModel(QString uid, QObject* parent = nullptr);


    /**
     * @ brief Destructor
     */
    ~I2SimpleSvgRectangleModel();


    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgCircleModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgCircleModel class is used to represent a circle
 */
class I2QUICK_EXPORT I2SimpleSvgCircleModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Abscissa of our center
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, centerX)

    // Ordinate of our center
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, centerY)

    // Radius of our circle
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, radius)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgCircleModel(QString uid, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~I2SimpleSvgCircleModel();

    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgEllipseModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgEllipseModel class is used to represent an ellipse
 */
class I2QUICK_EXPORT I2SimpleSvgEllipseModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Abscissa of our center
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, centerX)

    // Ordinate of our center
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, centerY)

    // x-axis radius of the ellipse
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, radiusX)

    // y-axis radius of the ellipse
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, radiusY)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgEllipseModel(QString uid, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~I2SimpleSvgEllipseModel();

    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgLineModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgLineModel class is used to represent a line
 */
class I2QUICK_EXPORT I2SimpleSvgLineModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Abscissa of our first point
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, x1)

    // Ordinate of our first point
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, y1)

    // Abscissa of our second point
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, x2)

    // Ordinate of our second point
    I2_CPP_PROPERTY_FUZZY_COMPARE (qreal, y2)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgLineModel(QString uid, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~I2SimpleSvgLineModel();

    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgPathModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgPathModel class is used to represent a path
 */
class I2QUICK_EXPORT I2SimpleSvgPathModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Path description
    I2_CPP_PROPERTY_CUSTOM_SETTER(QString, path)

    // First point of our shape
    I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER(QPointF, firstPoint)

    // Last point of our shape
    I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER(QPointF, lastPoint)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgPathModel(QString uid, QObject* parent = nullptr);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgPathModel();


    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;

protected:
    /**
     * @brief Compute internal data
     */
    void _computeData();

protected:
    bool _computedDataInitialized;
};


////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgPolylineModel
///
////////////////////////////////////////////////////////////////

/**
 * @brief The I2SimpleSvgPolylineModel class is used to represent a polyline
 */
class I2QUICK_EXPORT I2SimpleSvgPolylineModel : public I2SimpleSvgShapeModel
{
    Q_OBJECT

    // Points of our polyline (SVG data)
    I2_CPP_PROPERTY_CUSTOM_SETTER(QString, points)

    // Flag indicating if our polyline is closed (polygon)
    I2_CPP_PROPERTY (bool, isClosed)

    // First point of our shape
    I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER(QPointF, firstPoint)

    // Last point of our shape
    I2_CPP_PROPERTY_CUSTOM_GETTER_AND_SETTER(QPointF, lastPoint)

public:
    /**
     * @brief Default constructor
     * @param uid
     * @param parent
     */
    explicit I2SimpleSvgPolylineModel(QString uid, QObject* parent = nullptr);

    /**
     * @brief Destructor
     */
    ~I2SimpleSvgPolylineModel();

    /**
     * @brief Get the SVG path representing our shape
     * @return
     */
    QString toSVGPath() Q_DECL_OVERRIDE;

protected:
    /**
     * @brief Compute internal data
     */
    void _computeData();

protected:
    QString _path;
    bool _computedDataInitialized;
};



////////////////////////////////////////////////////////////////
///
///  I2SimpleSvgParser
///
////////////////////////////////////////////////////////////////


/**
 * @brief The I2SimpleSvgParser class defines a simple SVG parser that can parse a subset of SVG
 *        (SVG Tiny 1.2)
 */
class I2QUICK_EXPORT I2SimpleSvgParser : public QXmlDefaultHandler
{
public:
    /**
     * @brief Default constructor
     * @param url
     */
    explicit I2SimpleSvgParser(QUrl url);


    /**
     * @brief Destructor
     */
    ~I2SimpleSvgParser();

    /**
     * @brief Clear data
     */
    void clearData();

    /**
     * @brief Delete data
     */
    void deleteData();

    /**
     * @brief Get the hashtable of all named SVG elements
     * @return
     */
    QHash<QString, I2SimpleSvgItemModel *> svgElementsHash();

    /**
     * @brief Get the root element of our SVG file
     * @return
     */
    I2SimpleSvgRootModel* svgRoot();

    /**
     * @brief Get the number of SVG elements
     * @return
     */
    int svgElementsCount();


    /**
     * @brief Check if our parsing has succeeded or failed
     * @return
     */
    bool hasParsingSucceeded();

    /**
     * @brief Get a given SVG element
     * @param svgElementId
     * @return
     */
    I2SimpleSvgItemModel* getSvgElement(QString svgElementId);


protected:
    bool startElement(const QString &namespaceURI, const QString &localName, const QString &name, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &name);
    bool characters(const QString &text);
    bool fatalError(const QXmlParseException &exception);


    /**
     * @brief Parse our SVG file
     * @param url
     */
    void _parseFile();


    /**
     * @brief Add a given SVG element to our result
     * @param svgElement
     */
    void _addSvgElement(I2SimpleSvgItemModel* svgElement);

    /**
     * @brief Parse font infos
     * @param baseFont
     * @param attributes
     * @return
     */
    QFont _parseFont(QFont baseFont, const QXmlAttributes &attributes);


protected:
    QUrl _url;
    bool _isInsideDefs;
    bool _isInsideSymbol;
    bool _isInsideClipPath;
    bool _isInsideMask;
    I2SimpleSvgGroupModel* _currentParent;
    I2SimpleSvgTextModel* _currentMainSvgText; // <text>
    I2SimpleSvgTextModel* _currentSvgText; // <text> or <tspan>
    I2SimpleSvgRootModel* _svgRoot;
    QHash<QString, I2SimpleSvgItemModel *> _svgElementsById;
    int _svgElementsCount;
    bool _parsingSucceeded;

    //TODO: convert to a static (common) hash
    QHash<QString, QString> _fontFamiliesSubstitutions;
};

#endif // I2SIMPLESVGPARSER_H
