/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Dialogs 1.2

import I2Quick 1.0

import INGESCAPE 1.0


AssessmentsPopupBase {
    id: rootPopup

    height: 267
    width: 625

    anchors.centerIn: parent

    title: "NEW PROTOCOL"


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property ProtocolsController protocolsController: null;

    property url selectedPlatformUrl: "";



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------


    //--------------------------------
    //
    //
    // Functions
    //
    //
    //--------------------------------

    //
    // Reset all user inputs and close the popup
    //
    function resetInputsAndClosePopup() {
        console.log("QML: Reset all user inputs and close popup");

        // Reset all user inputs
        txtProtocolName.text = "";
        //txtPlatformUrl.text = "";
        rootPopup.selectedPlatformUrl = "";

        // Close the popup
        rootPopup.close();
    }


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    Item {
        id: rowName

        anchors {
            top: parent.top
            topMargin: 30
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            text: qsTr("Name:")

            height: 30

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtProtocolName

            anchors {
                left: parent.left
                leftMargin: 96
                right: parent.right
                verticalCenter: parent.verticalCenter
            }

            height: 30

            text: ""

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.veryLightGreyColor
                borderColor: IngeScapeAssessmentsTheme.blueButton
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0;
                borderWidthActive: 2
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                padding.left: 16
                padding.right: 16

                font {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }
    }

    Item {
        anchors {
            top: rowName.bottom
            topMargin: 24
            left: parent.left
            leftMargin: 28
            right: parent.right
            rightMargin: 28
        }

        height: 30

        Text {
            text: qsTr("Platform:")

            anchors {
                top: parent.top
                left: parent.left
            }

            height: 30

            verticalAlignment: Text.AlignVCenter

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        TextField {
            id: txtPlatformUrl

            anchors {
                left: parent.left
                leftMargin: 96
                right: btnSelectPlatformFile.left
                rightMargin: 7
                verticalCenter: parent.verticalCenter
            }

            height: 30

            //verticalAlignment: TextInput.AlignVCenter
            text: rootPopup.selectedPlatformUrl

            style: I2TextFieldStyle {
                backgroundColor: IngeScapeTheme.veryLightGreyColor
                borderColor: IngeScapeAssessmentsTheme.blueButton
                borderErrorColor: IngeScapeTheme.redColor
                radiusTextBox: 5
                borderWidth: 0;
                borderWidthActive: 2
                textIdleColor: IngeScapeAssessmentsTheme.regularDarkBlueHeader;
                textDisabledColor: IngeScapeAssessmentsTheme.lighterDarkBlueHeader

                placeholderCustomText: qsTr("Import a platform")
                placeholderMarginLeft: 15
                placeholderColor: IngeScapeTheme.lightGreyColor
                placeholderFont {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                    italic: true
                }

                padding.left: 16
                padding.right: 16

                font {
                    pixelSize: 16
                    family: IngeScapeTheme.textFontFamily
                }
            }
        }

        Button {
            id: btnSelectPlatformFile

            anchors {
                verticalCenter: txtPlatformUrl.verticalCenter
                right: parent.right
            }

            width: 40
            height: 30

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "file"
                disabledID: releasedID
            }

            onClicked: {
                // Open the file dialog
                fileDialog.open();
            }
        }
    }


    Row {
        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        Button {
            id: cancelButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                }

                label: Text {
                    text: "Cancel"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }
            }

            onClicked: {
                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            enabled: ( (txtProtocolName.text.length > 0) && (txtPlatformUrl.text.length > 0)
                      && rootPopup.protocolsController && rootPopup.protocolsController.canCreateProtocolWithName(txtProtocolName.text) )

            style: IngeScapeAssessmentsButtonStyle {
                text: "OK"
            }

            onClicked: {
                if (rootPopup.protocolsController)
                {
                    // Create a new protocol with an IngeScape platform file path
                    rootPopup.protocolsController.createNewProtocolWithIngeScapePlatformFilePath(txtProtocolName.text, txtPlatformUrl.text);
                }

                // Reset all user inputs and close the popup
                rootPopup.resetInputsAndClosePopup();
            }
        }
    }

    FileDialog {
        id: fileDialog

        title: "Select an IngeScape platform file"

        folder: shortcuts.documents + "/IngeScape/platforms"

        nameFilters: [ qsTr("Platform (*.igsplatform *.json)") ]

        onAccepted: {
            // Set the selected platform URL
            rootPopup.selectedPlatformUrl = fileDialog.fileUrl;
        }
    }

}
