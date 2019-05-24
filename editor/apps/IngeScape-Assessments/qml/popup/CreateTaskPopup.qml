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
import QtQuick.Dialogs 1.3

import I2Quick 1.0

import INGESCAPE 1.0


I2PopupBase {
    id: rootPopup

    height: 400
    width: 500

    anchors.centerIn: parent

    isModal: true
    dismissOnOutsideTap: false
    keepRelativePositionToInitialParent: false


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property TasksController controller: null;

    property url selectedPlatformUrl: "";



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    //
    //signal cancelTODO();


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onOpened: {

        /*if (experimentation) {
            console.log(experimentation.name + " with " + experimentation.allTasks.count + " tasks");
        }
        else {
            console.log("NO experimentation !!!");
        }*/
    }


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
        txtTaskName.text = "";
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

    Rectangle {

        anchors {
            fill: parent
        }
        radius: 5
        border {
            width: 2
            color: IngeScapeTheme.editorsBackgroundBorderColor
        }
        color: IngeScapeTheme.editorsBackgroundColor


        Text {
            id: title

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: 20
            }

            horizontalAlignment: Text.AlignHCenter

            text: qsTr("New task")

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 20
            }
        }

        Column {
            anchors {
                top: title.bottom
                topMargin: 30
                left: parent.left
                leftMargin: 10
            }

            spacing: 30


            Row {
                id: rowName

                spacing: 10

                Text {
                    text: qsTr("Name:")

                    width: 100
                    height: 30

                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                TextField {
                    id: txtTaskName

                    height: 30
                    width: 250

                    //verticalAlignment: TextInput.AlignVCenter
                    text: ""

                    style: I2TextFieldStyle {
                        backgroundColor: IngeScapeTheme.darkBlueGreyColor
                        borderColor: IngeScapeTheme.whiteColor
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: IngeScapeTheme.whiteColor;
                        textDisabledColor: IngeScapeTheme.darkGreyColor

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
                id: rowPlatform

                spacing: 10

                Text {
                    text: qsTr("Platform:")

                    width: 100
                    height: 30

                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                TextField {
                    id: txtPlatformUrl

                    height: 30
                    width: 250

                    //verticalAlignment: TextInput.AlignVCenter
                    text: rootPopup.selectedPlatformUrl

                    style: I2TextFieldStyle {
                        backgroundColor: IngeScapeTheme.darkBlueGreyColor
                        borderColor: IngeScapeTheme.whiteColor
                        borderErrorColor: IngeScapeTheme.redColor
                        radiusTextBox: 1
                        borderWidth: 0;
                        borderWidthActive: 1
                        textIdleColor: IngeScapeTheme.whiteColor;
                        textDisabledColor: IngeScapeTheme.darkGreyColor

                        padding.left: 3
                        padding.right: 3

                        font {
                            pixelSize:15
                            family: IngeScapeTheme.textFontFamily
                        }
                    }
                }

                Button {
                    id: btnSelectPlatformFile

                    text: qsTr("Select file...")

                    width: 100
                    height: 30

                    onClicked: {
                        //console.log("QML: Select platform file...");

                        // Open the file dialog
                        fileDialog.open();
                    }
                }

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

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                anchors {
                    verticalCenter: parent.verticalCenter
                }

                height: boundingBox.height
                width: boundingBox.width

                activeFocusOnPress: true
                text: "Cancel"

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.whiteColor

                }

                onClicked: {
                    console.log("QML: cancel");

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
                text: "OK"

                enabled: ( (txtTaskName.text.length > 0) && (txtPlatformUrl.text.length > 0)
                          && controller && controller.canCreateTaskWithName(txtTaskName.text) )

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    pressedID: releasedID + "-pressed"
                    releasedID: "button"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.greyColor

                }

                onClicked: {
                    //console.log("QML: create new Task " + txtTaskName.text + " with platform " + txtPlatformUrl.text);

                    if (controller)
                    {
                        // Create a new task with an IngeScape platform file
                        controller.createNewTaskWithIngeScapePlatformFile(txtTaskName.text, txtPlatformUrl.text);
                    }

                    // Reset all user inputs and close the popup
                    rootPopup.resetInputsAndClosePopup();
                }
            }
        }

    }


    FileDialog {
        id: fileDialog

        title: "Select an IngeScape platform file"

        //folder: shortcuts.home
        folder: shortcuts.documents
        //folder: shortcuts.documents + "IngeScape"

        //defaultSuffix: ".json"
        nameFilters: [ "JSON files (*.json)" ]

        onAccepted: {
            //console.log("Selected Platform Url: " + fileDialog.fileUrl)

            // Set the selected platform URL
            rootPopup.selectedPlatformUrl = fileDialog.fileUrl;

            //fileDialog.close();
        }

        onRejected: {
            console.log("Canceled " + shortcuts.documents);

            //fileDialog.close();
        }
    }

}
