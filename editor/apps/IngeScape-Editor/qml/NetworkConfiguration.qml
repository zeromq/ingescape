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

// Needed to access to ToolTip (https://doc.qt.io/qt-5.11/qml-qtquick-controls2-tooltip.html)
import QtQuick.Controls 2.0 as Controls2

I2PopupBase {
    id: rootItem

    width: 300
    height: 350

    dismissOnOutsideTap: false

    // our main controller
    //property var controller: null;

    onOpened: {
        txtPort.text = IngeScapeEditorC.port;
        combobox.selectedIndex = IngeScapeEditorC.networkC ? IngeScapeEditorC.networkC.availableNetworkDevices.indexOf(IngeScapeEditorC.networkDevice) : -1;
        clearPlatform.checked = true;
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

        spacing: 6

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

        Item {
            id: space1

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 13 // spacing + 13 + spacing
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

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 25
            verticalAlignment: TextInput.AlignVCenter

            text: IngeScapeEditorC.port

            validator: IntValidator {
                bottom: 1;
                top: 65535;
            }

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

        Item {
            id: space2

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 13 // spacing + 13 + spacing
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

        I2ComboboxStringList {
            id: combobox
            model: IngeScapeEditorC.networkC ? IngeScapeEditorC.networkC.availableNetworkDevices : 0

            anchors {
                left: parent.left
                right: parent.right
            }
            height : 25

            style: IngeScapeComboboxStyle {}
            scrollViewStyle: IngeScapeScrollViewStyle {}

            onSelectedItemChanged: {
                var selectedNetworkDevice = "";
                if (combobox.selectedItem)
                {
                    selectedNetworkDevice = combobox.selectedItem;
                }
                okButton.enabled = IngeScapeEditorC.networkC.isAvailableNetworkDevice(selectedNetworkDevice);
            }

            mouseArea.hoverEnabled: true

            Controls2.ToolTip {
                delay: 500
                visible: combobox.containsMouse
                text: combobox.text
            }

            delegate: customDelegate.component

            IngeScapeToolTipComboboxDelegate {
                id: customDelegate

                comboboxStyle: combobox.style
                selection: combobox.selectedIndex

                height: combobox.comboButton.height
                width:  combobox.comboButton.width

                // Called from the component's MouseArea
                // 'index' is the index of the clicked component inside the model.
                function onDelegateClicked(index) {
                    combobox.onDelegateClicked(index)
                }

                // Called from the component to get the text of the current item to display
                // 'index' is the index of the component to be displayed inside the model.
                function getItemText(index) {
                    return combobox.modelToString(combobox.model[index]);
                }
            }
        }

        Item {
            id: space3

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 58 // spacing + 58 + spacing

            Text {
                id: error

                anchors {
                    left: parent.left
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                wrapMode: Text.WordWrap

                text: IngeScapeEditorC.errorMessageWhenConnectionFailed

                color: IngeScapeTheme.orangeColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }
        }

        CheckBox {
            id: clearPlatform

            anchors {
                left: parent.left;
                //top: parent.top
            }

            checked: true
            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 2
                    }

                    text: qsTr("Clear platform when modified")

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                    color: control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor
                }

                indicator: Rectangle {
                    implicitWidth: 14
                    implicitHeight: 14
                    border.width: 0
                    color : IngeScapeTheme.darkBlueGreyColor

                    I2SvgItem {
                        visible: control.checked
                        anchors.centerIn: parent

                        svgFileCache: IngeScapeTheme.svgFileINGESCAPE;
                        svgElementId: "check";
                    }
                }
            }
        }

    }


    Row {
        anchors {
            right: parent.right
            rightMargin: 20
            bottom : parent.bottom
            bottomMargin: 20
        }
        spacing : 15

        Button {
            id: cancelButton
            activeFocusOnPress: true

            property var boundingBox: IngeScapeTheme.svgFileINGESCAPE.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            text: "Cancel"

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
            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "OK"
            //enabled: true

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
                //console.log("OK")

                var selectedNetworkDevice = "";

                if (typeof combobox.selectedItem === "string") {
                    selectedNetworkDevice = combobox.selectedItem;
                }
                else {
                    selectedNetworkDevice = combobox.selectedItem.modelData;
                }

                // Re-Start the Network
                var success = IngeScapeEditorC.restartNetwork(txtPort.text, selectedNetworkDevice, clearPlatform.checked);
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
