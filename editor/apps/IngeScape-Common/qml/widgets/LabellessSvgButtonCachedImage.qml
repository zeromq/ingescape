import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0



Button {
    id: root

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // SVG file cache
    property var fileCache: IngeScapeTheme.svgFileIngeScape

    // IDs of SVG elements
    property string pressedID: ""
    property string releasedID: ""
    property string disabledID: ""

    // Margins used to grow up our input area
    property int maskVerticalMargins : 0
    property int maskHorizontalMargins : 0

    // Scale applied to render SVG elements
    property real sourceScale: 1



    // This property to define what happens when the source image has a different size than the item
    property int fillMode: Image.Stretch

    // This property holds whether the image should be horizontally inverted (effectively displaying a mirrored image)
    property bool mirror: false

    // This property holds whether the image is smoothly filtered when scaled or transformed
    property bool smooth: true

    // This property holds whether the image uses mipmap filtering when scaled or transformed
    property bool mipmap: false




    activeFocusOnPress: true


    //--------------------------------
    //
    // Style
    //
    //--------------------------------

    style: ButtonStyle {
        id: buttonStyle

        // No padding
        padding { left: 0; right: 0; top: 0; bottom: 0; }

        // No label
        label: Item {}

        // Background
        background: Item {
            implicitWidth: svgImage.implicitSourceWidth + 2 * maskHorizontalMargins
            implicitHeight: svgImage.implicitSourceHeight + 2 * maskVerticalMargins

            SvgImage {
                id: svgImage

                anchors {
                    fill: parent
                    topMargin : maskVerticalMargins
                    bottomMargin : maskVerticalMargins
                    leftMargin : maskHorizontalMargins
                    rightMargin : maskHorizontalMargins
                }

                fillMode: root.fillMode
                mirror: root.mirror

                smooth: root.smooth
                mipmap: root.mipmap

                sourceScale: root.sourceScale

                svgFileCache: root.fileCache
                svgElementId:  (control.enabled ? (control.pressed ? pressedID : releasedID) : disabledID)
            }
        }
    }
}

