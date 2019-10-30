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


    activeFocusOnPress: true


    //--------------------------------
    //
    // Style
    //
    //--------------------------------

    style: I2SvgButtonStyle {
        id: buttonStyle

        fileCache: root.fileCache

        pressedID: root.pressedID
        releasedID: root.releasedID
        disabledID: root.disabledID

        // No padding
        padding { left: 0; right: 0; top: 0; bottom: 0; }

        // No label
        label: Item {}
    }
}
