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

    // Our controller
    property LicensesController controller: null;


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

        if (rootItem.controller)
        {
            rootItem.controller.updateLicensesPath(txtLicensesPath.text);
        }

        // Close our popup
        rootItem.close();
    }


    //--------------------------------------------------------
    //
    //
    // Callbacks
    //
    //
    //--------------------------------------------------------

    onOpened: {
        if (rootItem.controller) {
            txtLicensesPath.text = rootItem.controller.licensesPath;
        }

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

            text: qsTr("Licenses")

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
                text: qsTr("License validity:")

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

            Rectangle {
                id: flag
                width: 20
                height: 20
                radius: 10
                color: rootItem.controller && rootItem.controller.isLicenseValid ? "green" : "red"
            }
        }

        Text {
            text: rootItem.controller ? qsTr("Customer: %1").arg(rootItem.controller.licenseCustomer) : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Text {
            text: rootItem.controller ? qsTr("Order: %1").arg(rootItem.controller.licenseOrder) : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Text {
            text: rootItem.controller ? qsTr("Expiration date: %1").arg(rootItem.controller.licenseExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
                                      : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Item {
            id: space2

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 10
        }

        Row {
            spacing: 10

            Text {
                text: qsTr("Editor license validity:")

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

            Rectangle {
                id: flagEditor
                width: 20
                height: 20
                radius: 10
                color: rootItem.controller && rootItem.controller.isEditorLicenseValid ? "green" : "red"
            }
        }

        Text {
            text: rootItem.controller ? qsTr("Editor owner: %1").arg(rootItem.controller.editorOwner) : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Text {
            text: rootItem.controller ? qsTr("Editor expiration date: %1").arg(rootItem.controller.editorExpirationDate.toLocaleString(Qt.locale(), "dd/MM/yyyy"))
                                      : ""

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Item {
            id: space3

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
                    if (rootItem.controller)
                    {
                        var directoryPath = rootItem.controller.selectLicensesDirectory();
                        if (directoryPath) {
                            txtLicensesPath.text = directoryPath;
                        }
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

            text: "" //rootItem.controller.licensesPath

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

        Item {
            id: space4

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 10
        }

        Text {
            id: errorMessage

            anchors {
                left: parent.left
                right: parent.right
                //verticalCenter: parent.verticalCenter
            }
            wrapMode: Text.WordWrap

            text: rootItem.controller ? rootItem.controller.errorMessageWhenLicenseFailed : ""

            color: IngeScapeTheme.orangeColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
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

                rootItem.validate();
            }
        }
    }
}
