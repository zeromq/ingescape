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


I2PopupBase {
    id: rootItem

    width: 600
    height: 400

    isModal: true
    dismissOnOutsideTap: true
    keepRelativePositionToInitialParent: false

    // corresponding agent
    property var agent: null;

    /*onOpened: {

    }*/

    Rectangle {
        anchors.fill: parent

        color: IngeScapeTheme.veryDarkGreyColor

        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }

        radius: 5
    }

    Column {
        anchors {
            fill: parent
            margins: 20
        }

        spacing: 10

        Text {
            id: title

            text: (rootItem.agent ? qsTr("Paths for Definition and Mapping of %1").arg(rootItem.agent.name) : "")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 23
            }
        }

        Rectangle {
            id: space1
            color: "transparent"
            width: 10
            height: 10
        }

        Text {
            text: qsTr("Definition file path:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        TextField {
            id: txtDefinitionFilePath

            height: 25
            width: parent.width

            verticalAlignment: TextInput.AlignVCenter
            text: (rootItem.agent ? rootItem.agent.definitionFilePath : "")

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

            /*Binding {
                target: txtDefinitionFilePath
                property: "text"
                value: rootItem.agent.definitionFilePath
            }*/
        }

        Rectangle {
            id: space2
            color: "transparent"
            width: 10
            height: 10
        }

        Text {
            text: qsTr("Mapping file path:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        TextField {
            id: txtMappingFilePath

            height: 25
            width: parent.width

            verticalAlignment: TextInput.AlignVCenter
            text: (rootItem.agent ? rootItem.agent.mappingFilePath : "")

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

            /*Binding {
                target: txtMappingFilePath
                property: "text"
                value: rootItem.agent.mappingFilePath
            }*/
        }


        Rectangle {
            id: space3
            color: "transparent"
            width: 10
            height: 10
        }

        Text {
            text: qsTr("Log file path:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        TextField {
            id: txtLogFilePath

            height: 25
            width: parent.width

            verticalAlignment: TextInput.AlignVCenter
            text: (rootItem.agent ? rootItem.agent.logFilePath : "")

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

            /*Binding {
                target: txtLogFilePath
                property: "text"
                value: rootItem.agent.logFilePath
            }*/
        }
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
                console.log("Cancel");

                rootItem.close();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");
            height: boundingBox.height
            width: 110 // boundingBox.width

            activeFocusOnPress: true
            text : "Save paths"

            anchors {
                verticalCenter: parent.verticalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                pressedID: releasedID + "-pressed"
                releasedID: "button"
                disabledID: releasedID + "-disabled"

                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
                labelColorPressed: IngeScapeTheme.blackColor
                labelColorReleased: IngeScapeTheme.whiteColor
                labelColorDisabled: IngeScapeTheme.greyColor

            }

            onClicked: {
                console.log("Save paths");

                if (rootItem.agent) {
                    // Save the "files paths" of agent
                    rootItem.agent.saveFilesPaths(txtDefinitionFilePath.text, txtMappingFilePath.text, txtLogFilePath.text);
                }

                rootItem.close();
            }
        }
    }
}
