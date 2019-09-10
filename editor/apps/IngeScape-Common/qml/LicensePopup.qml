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
    property LicensesController licenseController: null;


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

        if (rootItem.licenseController)
        {
            rootItem.licenseController.updateLicensesPath(txtLicensesPath.text);
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
        if (rootItem.licenseController) {
            txtLicensesPath.text = rootItem.licenseController.licensesPath;
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
                    if (rootItem.licenseController)
                    {
                        var directoryPath = rootItem.licenseController.selectLicensesDirectory();
                        if (directoryPath) {
                            txtLicensesPath.text = directoryPath;

                            rootItem.licenseController.updateLicensesPath(directoryPath);
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
        id: detailsScrollView
        anchors {
            top: directoryPathItem.bottom
            left: parent.left
            right: parent.right
            bottom: buttonRow.top
            margins: 25
        }

        visible: licenseDetailsRepeater.model && licenseDetailsRepeater.model.count > 0

        contentItem: Column {
            id: detailsColumn
            anchors {
                left: parent.left
                right: parent.right
            }

            spacing: 26

            LicenseInformationView {
                licenseInformation: rootItem.licenseController.mergedLicense
            }

            Text {
                text: rootItem.licenseController ? qsTr("License details:") : ""

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                    bold: true
                }
            }

            Repeater {
                id: licenseDetailsRepeater
                model: rootItem.licenseController ? rootItem.licenseController.licenseDetailsList : 0

                delegate: Column {
                    spacing: 26

                    Item {

                        anchors {
                            left: parent.left
                            right: parent.right
                        }

                        height: childrenRect.height

                        Text {
                            id: licenseFileName
                            text: qsTr("###\n### License file name: %1\n###").arg(model ? model.fileName : "")

                            anchors {
                                left: parent.left
                                right: parent.right
                            }

                            color: IngeScapeTheme.whiteColor
                            elide: Text.ElideRight
                            font {
                                family: IngeScapeTheme.textFontFamily
                                weight: Font.Medium
                                pixelSize: 16
                                italic: true
                            }
                        }

                        Button {
                            text: "DEL"
                            anchors {
                                right: parent.right
                                verticalCenter: licenseFileName.verticalCenter
                            }

                            onClicked: {
                                if (model && rootItem.licenseController) {
                                    rootItem.licenseController.deleteLicense(model.QtObject)
                                }
                            }
                        }
                    }

                    LicenseInformationView {
                        licenseInformation: model ? model.QtObject : null
                    }
                }
            }

            Text {
                id: errorMessage

                anchors {
                    left: parent.left
                    right: parent.right
                    //verticalCenter: parent.verticalCenter
                }
                wrapMode: Text.WordWrap

                text: rootItem.licenseController ? rootItem.licenseController.errorMessageWhenLicenseFailed : "TEST"

                color: IngeScapeTheme.orangeColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight : Font.Medium
                    pixelSize : 16
                }
            }
        }
    }

    Item {
        id: emptyLicenseFeedback
        anchors.fill: detailsScrollView

        visible: !detailsScrollView.visible

        Text {
            id: emptyLicenseFeedbackText

            anchors {
                left: parent.left
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            text: "No license file has been found.\nPlease change the license directory above, drop a license file here or\nuse the \"Import...\" button below."
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter

            color: IngeScapeTheme.whiteColor
            wrapMode: Text.WordWrap
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize : 18
                italic: true
            }
        }

        Button {
            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "Import..."

            anchors {
                top: emptyLicenseFeedbackText.bottom
                topMargin: 25
                horizontalCenter: parent.horizontalCenter
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
                if (rootItem.licenseController) {
                    rootItem.licenseController.importLicense();
                }
            }
        }
    }

    DropArea {
        id: dropZone
        anchors.fill: detailsScrollView

        property bool dragHovering: false

        onEntered: {
            dragHovering = true
        }

        onExited: {
            dragHovering = false
        }

        onDropped: {
            dragHovering = false
            if (drop.hasUrls && rootItem.licenseController)
            {
                rootItem.licenseController.addLicenses(drop.urls)
            }
        }

        // Overlay appearing when the user drags something over the drop zone
        Rectangle {
            id: dropZoneOverlay
            anchors.fill: parent

            color: IngeScapeTheme.veryLightGreyColor
            opacity: dropZone.dragHovering ? 0.65 : 0

            Behavior on opacity {
                NumberAnimation {}
            }
        }
    }

    Row {
        id: buttonRow
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom : parent.bottom
            bottomMargin: 20
        }
        spacing : 15

        height: 30

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true
            text: "OK"

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
