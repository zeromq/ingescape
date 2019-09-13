import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

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


    activeFocusOnPress: true


    //--------------------------------
    //
    // Style
    //
    //--------------------------------

    style: LabellessSvgButtonStyle {
        id: buttonStyle

        fileCache: root.fileCache

        pressedID: root.pressedID
        releasedID: root.releasedID
        disabledID: root.disabledID
    }
}
