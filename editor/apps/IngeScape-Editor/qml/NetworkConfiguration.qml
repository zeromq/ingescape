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

// scenario sub-directory
import "scenario" as Scenario

I2PopupBase {
    id: rootItem

    width: 300
    height: 400

    dismissOnOutsideTap: false

    // our main controller
    //property var controller: null;

    onOpened: {
        combobox.selectedItem = IngeScapeEditorC.networkDevice;
    }

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

            text: qsTr("Configure network")

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
            text: qsTr("Port:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        TextField {
            id: txtPort

            height: 25
            width: 200
            verticalAlignment: TextInput.AlignVCenter
            text: IngeScapeEditorC.port

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

            Binding {
                target: txtPort
                property: "text"
                value: IngeScapeEditorC.port
            }
        }

        Rectangle {
            id: space2
            color: "transparent"
            width: 10
            height: 10
        }

        Text {
            text: qsTr("Network device:")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }


        // ComboBox to choose the network device
        Scenario.IngeScapeComboBox {
            id : combobox

            height : 25
            width : 200

            model: IngeScapeEditorC.networkC ? IngeScapeEditorC.networkC.availableNetworkDevices : 0
            useQStringList: true

            //placeholderText: (IngeScapeEditorC.networkC && IngeScapeEditorC.networkC.availableNetworkDevices.count === 0 ? "- No network device -" : "- Select a network device -")
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
                //console.log("Cancel")
                rootItem.close();
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
                //console.log("OK")

                var selectedNetworkDevice = "";

                if (typeof combobox.selectedItem === "string") {
                    selectedNetworkDevice = combobox.selectedItem;
                }
                else {
                    selectedNetworkDevice = combobox.selectedItem.modelData;
                }

                // Re-Start the Network
                var success = IngeScapeEditorC.restartNetwork(txtPort.text, selectedNetworkDevice);
                if (success === true)
                {
                    rootItem.close();
                }
                else {
                    console.error("Network cannot be (re)started on device " + combobox.selectedItem.modelData + " and port " + txtPort.text);
                }
            }
        }
    }
}
