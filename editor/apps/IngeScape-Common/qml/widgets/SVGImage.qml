import QtQuick 2.8

import I2Quick 1.0

import INGESCAPE 1.0


// SvgImage defines an alternative to I2SvgItem that use an image
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

    // Scale applied to our source
    property real sourceScale: 1


    // width and height of our source
    // implicitWidth and implicitheight are read-only
    readonly property real implicitSourceWidth: _boundingBoxOfSvgElement.width
    readonly property real implicitSourceHeight: _boundingBoxOfSvgElement.height


    // Bounding box of our SVG element
    property rect _boundingBoxOfSvgElement: (svgFileCache)
                                            ? svgFileCache.boundsOnElement(svgElementId)
                                            : Qt.rect(0, 0, 1, 1)


    //--------------------------------
    //
    // Funtions
    //
    //--------------------------------

    function replaceUrl(url)
    {
        return url.toString().replace(/^(qrc:\/{1,2})/, "image://I2svg/");
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    width: _boundingBoxOfSvgElement.width
    height: _boundingBoxOfSvgElement.height

    source: (svgFileCache && (svgElementId.length !== 0))
            ? replaceUrl(svgFileCache.svgFile) + "#" + svgElementId
            : ""

    sourceSize {
        width: Math.max(1, sourceScale * implicitSourceWidth)
        height: Math.max(1, sourceScale * implicitSourceHeight)
    }
}
