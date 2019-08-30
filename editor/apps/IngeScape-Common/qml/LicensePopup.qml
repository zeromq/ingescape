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

    width: 600
    height: 750

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

    Text {
        id: title

        anchors {
            top: parent.top
            topMargin: 25
            horizontalCenter: parent.horizontalCenter
        }

        text: qsTr("Licenses")

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            weight : Font.Medium
            pixelSize : 23
        }
    }

    Item {
        id: directoryPathItem
        anchors {
            left: parent.left
            right: parent.right
            top: title.bottom
            margins: 25
        }

        height: childrenRect.height

        Text {
            text: qsTr("Directory path")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight : Font.Medium
                pixelSize : 16
            }
        }

        Item {

            anchors {
                left: parent.left
                right: parent.right
            }

            height: childrenRect.height

            TextField {
                id: txtLicensesPath

                anchors {
                    left: parent.left
                    right: btnSelectLicencesDirectory.left
                    rightMargin: 10
                }
                height: btnSelectLicencesDirectory.height
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

            Button {
                id: btnSelectLicencesDirectory

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                anchors {
                    right: parent.right
                }

                height: boundingBox.height
                width: boundingBox.width

                text: "Change..."
                activeFocusOnPress: true

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

                            rootItem.controller.updateLicensesPath(directoryPath);
                        }
                    }
                }
            }
        }

        // Vertical space
        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: 10
        }
    }

    ScrollView {
        anchors {
            top: directoryPathItem.bottom
            left: parent.left
            right: parent.right
            bottom: buttonRow.top
            margins: 25
        }

        contentItem: Column {
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: 16

            LicenseInformationView {
                controller: rootItem.controller
            }

            // Vertical space
            Item {
                anchors {
                    left: parent.left
                    right: parent.right
                }
                height: 10
            }

            Text {
                text: rootItem.controller ? qsTr("Files:") : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }

            // FIXME TODO: list of licenseM

            // Vertical space
            Item {
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
    }

    Row {
        id: buttonRow
        anchors {
            right: parent.right
            rightMargin: 20
            bottom : parent.bottom
            bottomMargin: 20
        }
        spacing : 15

        height: 30

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
