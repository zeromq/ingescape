/*
 *	IngeScape Editor
 *
 *  Copyright © 2017-2019 Ingenuity i/o. All rights reserved.
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

    width: 500
    height: 500

    dismissOnOutsideTap: false

    // our main controller
    //property var controller: null;


    //--------------------------------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------------------------------

    //
    // function allowing to validate the form
    //
    function validate() {
        console.log("QML: function validate()");

        // TODO
        /*var success = IngeScapeEditorC.restartNetwork(txtPort.text, selectedNetworkDevice, clearPlatform.checked, txtLicensesPath.text);
        if (success === true)
        {
            rootItem.close();
        }
        else {
            console.error("Network cannot be (re)started on device " + combobox.selectedItem.modelData + " and port " + txtPort.text);
        }*/
    }


    //--------------------------------------------------------
    //
    //
    // Callbacks
    //
    //
    //--------------------------------------------------------

    onOpened: {

        txtLicensesPath.text = IngeScapeEditorC.licensesPath;

        // Set the focus to catch keyboard press on Return/Escape
        rootItem.focus = true;
    }

    Keys.onEscapePressed: {
        //console.log("QML: Escape Pressed");

        rootItem.close();
    }

    Keys.onReturnPressed: {
        //console.log("QML: Return Pressed");

        rootItem.validate();
    }


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

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

            text: qsTr("Configure license")

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
            height: 10
        }

        Row {
            spacing: 10
            Text {
                text: qsTr("License path")

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }

            Button {
                id: btnSelectLicencesDirectory

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                activeFocusOnPress: true

                height: boundingBox.height
                width: boundingBox.width

                text: "Change..."

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

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
                    //fileDialog.open();

                    var directoryPath = IngeScapeEditorC.selectLicensesDirectory();
                    if (directoryPath) {
                        txtLicensesPath.text = directoryPath;
                    }
                }
            }
        }

        TextField {
            id: txtLicensesPath

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 25
            verticalAlignment: TextInput.AlignVCenter

            text: "" //IngeScapeEditorC.licensesPath

            enabled: false

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.darkBlueGreyColor
                backgroundDisabledColor: IngeScapeTheme.darkBlueGreyColor

                borderColor: IngeScapeTheme.whiteColor
                borderDisabledColor: IngeScapeTheme.whiteColor

                borderErrorColor: IngeScapeTheme.redColor

                radiusTextBox: 1
                borderWidth: 0;
                borderWidthActive: 1

                textIdleColor: IngeScapeTheme.whiteColor
                textDisabledColor: IngeScapeTheme.whiteColor

                padding.left: 3
                padding.right: 3

                font {
                    pixelSize:15
                    family: IngeScapeTheme.textFontFamily
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

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            text: "Cancel"

            anchors {
                verticalCenter: parent.verticalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

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
                console.log("QML: Cancel License Popup")

                rootItem.close();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "OK"
            //enabled: true

            anchors {
                verticalCenter: parent.verticalCenter
            }

            style: I2SvgButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

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
                console.log("QML: OK License Popup")

                //rootItem.close();

                rootItem.validate();
            }
        }
    }
}
