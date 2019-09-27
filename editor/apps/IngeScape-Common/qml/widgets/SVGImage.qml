import QtQuick 2.8

import I2Quick 1.0

import INGESCAPE 1.0


// SvgImage defines an alternative to I2SvgItem that uses an image
Image {
    id: root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // SVG file cache
    property var svgFileCache: IngeScapeTheme.svgFileIngeScape

    // SVG element ID
    property string svgElementId: ""

    // Flag used to display our whole SVG file instead of a specific SVG item
    property bool displayWholeSvgFile: false

    // Scale applied to our source
    property real sourceScale: 1


    // Abscissa of the bounding box of our SVG element as defined in our SVG file
    readonly property alias svgX: rootPrivate.svgX

    // Ordinate of the bounding box of our SVG element as defined in our SVG file
    readonly property alias svgY: rootPrivate.svgY

    // Width of the bounding box of our SVG element
    readonly property alias svgWidth: rootPrivate.svgWidth

    // Height of the bounding box of our SVG element
    readonly property alias svgHeight: rootPrivate.svgHeight

    // width and height of our source
    // implicitWidth and implicitheight are read-only
    readonly property real implicitSourceWidth: rootPrivate.svgWidth
    readonly property real implicitSourceHeight: rootPrivate.svgHeight



    // Bounding box of our SVG element
    property rect _boundingBoxOfSvgElement: (svgFileCache)
                                            ? svgFileCache.boundsOnElement(svgElementId)
                                            : Qt.rect(0, 0, 1, 1)





    //
    // Private properties
    //
    QtObject {
        id: rootPrivate

        // URL
        property string source: computeSourceUrl(root.svgFileCache, root.svgElementId, root.displayWholeSvgFile)

        // Bounding box of our SVG element
        property rect boundingBoxOfSvgElement: computeSourceSize(root.svgFileCache, root.svgElementId, root.displayWholeSvgFile)

        // Abscissa of the bounding box of our SVG element as defined in our SVG file
        property real svgX: boundingBoxOfSvgElement.x

        // Ordinate of the bounding box of our SVG element as defined in our SVG file
        property real svgY: boundingBoxOfSvgElement.y

        // Width of the bounding box of our SVG element
        property real svgWidth: boundingBoxOfSvgElement.width

        // Height of the bounding box of our SVG element
        property real svgHeight: boundingBoxOfSvgElement.height



        function computeSourceUrl(svgFileCache, svgElementId, displayWholeSvg)
        {
            var url = (svgFileCache) ? svgFileCache.svgFile : svgFile;
            if (url.length !== 0)
            {
                url = replaceUrlPrefix(url);
            }

            if (svgElementId.length !== 0)
            {
                url = url + "#" + svgElementId;
            }
            else if (!displayWholeSvg)
            {
                url = "";
            }

            return url;
        }


        function computeSourceSize(svgFileCache, svgElementId, displayWholeSvg)
        {
            if (svgFileCache)
            {
                if (svgElementId.length !== 0)
                {
                    return svgFileCache.boundsOnElement(svgElementId);
                }
                else if (displayWholeSvg)
                {
                    return Qt.rect(svgFileCache.viewboxX, svgFileCache.viewboxY, svgFileCache.viewboxWidth, svgFileCache.viewboxHeight);
                }
            }

            return Qt.rect(0, 0, 1, 1);
        }


        function replaceUrlPrefix(url)
        {
            return url.toString().replace(/^(qrc:\/{1,2})/, "image://I2svg/");
        }
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    width: rootPrivate.svgWidth
    height: rootPrivate.svgHeight

    source: rootPrivate.source

    sourceSize {
        width: Math.max(1, sourceScale * rootPrivate.svgWidth)
        height: Math.max(1, sourceScale * rootPrivate.svgHeight)
    }
}
