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

        color: IngeScapeTheme.darkGreyColor

    }

    Column {
        anchors {
            fill: parent
            margins: 20
        }

        spacing: 10

        Text {
            id: title

            text: qsTr("Configure network")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 20
            }
        }

        Text {
            id: currentPort

            text: qsTr("Current port: %1").arg(IngeScapeEditorC.port)

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        TextField {
            id: newPort

            /*anchors {
                left : parent.left
                verticalCenter : parent.verticalCenter
            }*/

            height: 25
            width: 185
            verticalAlignment: TextInput.AlignVCenter
            text: IngeScapeEditorC ? IngeScapeEditorC.temporaryPort : "0"

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.darkBlueGreyColor
                borderColor: IngeScapeTheme.whiteColor;
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 1
                borderWidth: 0;
                borderWidthActive: 1
                textIdleColor: IngeScapeTheme.whiteColor;
                textDisabledColor: IngeScapeTheme.darkGreyColor;

                padding.left: 3
                padding.right: 3

                font {
                    pixelSize:15
                    family: IngeScapeTheme.textFontFamily
                }

            }

            onTextChanged: {
                if (activeFocus && IngeScapeEditorC) {
                    IngeScapeEditorC.temporaryPort = text;
                }
            }

            /*onActiveFocusChanged: {
                if (!activeFocus) {
                    // Move cursor to our first character when we lose focus
                    // (to always display the beginning or our text instead of
                    // an arbitrary part if our text is too long)
                    cursorPosition = 0;
                } else {
                    textFieldName.selectAll();
                }
            }*/


            Binding {
                target: newPort
                property:  "text"
                value: if (IngeScapeEditorC) {
                           IngeScapeEditorC.temporaryPort
                       }
                       else {
                           "0";
                       }
            }
        }


        Text {
            id: currentNetworkDevice

            text: qsTr("Current network device: %1").arg(IngeScapeEditorC.networkDevice)

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
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
