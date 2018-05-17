/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Item {
    id: rootItem

    width: 400
    height: 600

    Rectangle {
        anchors.fill: parent

        color: IngeScapeTheme.greyColor

    }

    Column {
        Text {
            id: title

            text: qsTr("Configure network")
        }

        Text {
            id: port

            text: qsTr("Current port: %1").arg(IngeScapeEditorC.port)
        }

        Text {
            id: networkDevice

            text: qsTr("Current network device: %1").arg(IngeScapeEditorC.networkDevice)
        }

        /*ListView {

        }*/
    }

    Row {
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom : parent.bottom
            bottomMargin: 16
        }
        spacing : 15

        Button {
            id: cancelButton
            activeFocusOnPress: true

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");

            height : boundingBox.height
            width :  boundingBox.width

            text : "Cancel"

            anchors {
                verticalCenter: parent.verticalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "button"
                disabledID : releasedID

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
                labelColorPressed: IngeScapeTheme.blackColor
                labelColorReleased: IngeScapeTheme.whiteColor
                labelColorDisabled: IngeScapeTheme.whiteColor

            }

            onClicked: {
                console.log("Cancel")
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
            height : boundingBox.height
            width :  boundingBox.width

            activeFocusOnPress: true
            text : "OK"

            anchors {
                verticalCenter: parent.verticalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "button"
                disabledID : releasedID

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
                labelColorPressed: IngeScapeTheme.blackColor
                labelColorReleased: IngeScapeTheme.whiteColor
                labelColorDisabled: IngeScapeTheme.whiteColor

            }

            onClicked: {
                console.log("OK")
            }
        }
    }
}
