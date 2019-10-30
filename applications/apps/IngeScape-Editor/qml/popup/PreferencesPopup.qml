import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: root

    //----------------------------------
    //
    // Properties
    //
    //----------------------------------

    width: 500
    height: 500


    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false



    //----------------------------------
    //
    // Content
    //
    //----------------------------------

    Rectangle {
        id: background

        anchors.fill: parent

        color: IngeScapeTheme.veryDarkGreyColor

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        radius: 5


        Text {
            id: title

            anchors {
                top: parent.top
                topMargin: 25
                horizontalCenter: parent.horizontalCenter
            }

            text: qsTr("Preferences")

            color: IngeScapeTheme.whiteColor

            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize : 23
            }
        }
    }

}
