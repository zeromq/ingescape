/*
 *	IngeScape Expe
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

Item {

    id: rootItem



    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------


    property PlatformM currentLoadedPlatform: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.currentLoadedPlatform : null;


    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    /*header: ToolBar {
        contentHeight: toolButton.implicitHeight

        ToolButton {
            id: toolButton
            text: stackView.depth > 1 ? "\u25C0" : "\u2630"
            font.pixelSize: Qt.application.font.pixelSize * 1.6
            onClicked: {
                if (stackView.depth > 1) {
                    stackView.pop()
                } else {
                    drawer.open()
                }
            }
        }

        Label {
            text: stackView.currentItem.title
            anchors.centerIn: parent
        }
    }

    Drawer {
        id: drawer
        width: window.width * 0.66
        height: window.height

        Column {
            anchors.fill: parent

            ItemDelegate {
                text: qsTr("Page 1")
                width: parent.width
                onClicked: {
                    stackView.push("Page1Form.ui.qml")
                    drawer.close()
                }
            }
            ItemDelegate {
                text: qsTr("Page 2")
                width: parent.width
                onClicked: {
                    stackView.push("Page2Form.ui.qml")
                    drawer.close()
                }
            }
        }
    }

    StackView {
        id: stackView
        initialItem: "HomeForm.ui.qml"
        anchors.fill: parent
    }*/


    // Title
    Rectangle {
        id: title

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 15
        }
        width: 100
        height: 40

        radius: 5
        color: "transparent"
        border {
            color: "darkgray"
            width: 1
        }

        Label {
            anchors.centerIn: parent

            text: "EXPE"
            color: IngeScapeTheme.whiteColor

            //font: IngeScapeTheme.headingFont
            font {
                family: IngeScapeTheme.labelFontFamily
                pixelSize: 24
                weight: Font.Black
            }
        }
    }


    Row {
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }

        spacing: 5

        // Feedback about IngeScape Editor ON/OFF
        Rectangle {
            width: 100
            height: 40

            radius: 5

            color: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON) ? "green" : "orange"

            Label {
                anchors.centerIn: parent
                color: IngeScapeTheme.whiteColor
                font: IngeScapeTheme.headingFont
                text: "Editor"
            }
        }

        // Feedback about IngeScape Recorder ON/OFF
        Rectangle {
            width: 100
            height: 40

            radius: 5

            color: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isRecorderON) ? "green" : "orange"

            Label {
                anchors.centerIn: parent
                color: IngeScapeTheme.whiteColor
                font: IngeScapeTheme.headingFont
                text: "Recorder"
            }
        }
    }


    //
    // Platforms List
    //
    PlatformsList {
        id: platformsList

        anchors {
            left: parent.left
            leftMargin: 10
            top: title.bottom
            topMargin: 20
            right: loadedPlatform.left
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 10
        }
    }


    //
    // Commands
    //
    Rectangle {
        id: commandsPanel

        anchors {
            right: parent.right
            rightMargin: 10
            top: title.bottom
            topMargin: 20
        }
        width: 350
        height: 200

        color: "transparent"
        radius: 5
        border {
            color: "darkgray"
            width: 2
        }

        Column {
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 10
            }

            spacing: 10

            Text {
                id: titleCommands

                anchors.horizontalCenter: parent.horizontalCenter
                height: 30

                text: "Commands"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Bold
                    pixelSize: 18
                }
            }

            Row {

                height: 30

                spacing: 10

                Text {
                    id: txtRecord

                    text: "Record:"

                    height: 30

                    verticalAlignment: Text.AlignVCenter

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }

                Button {
                    id: btnStartOrStopRecord

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    visible: true
                    activeFocusOnPress: true
                    opacity: enabled ? 1.0 : 0.3
                    enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON && IngeScapeExpeC.modelManager.currentLoadedPlatform
                    checkable: true
                    checked: false

                    style: I2SvgToggleButtonStyle {
                        fileCache: IngeScapeExpeTheme.svgFileIngeScapeExpe

                        toggleCheckedReleasedID: "record-stop";
                        toggleCheckedPressedID: toggleCheckedReleasedID + "-pressed";
                        toggleUncheckedReleasedID: "record-start";
                        toggleUncheckedPressedID: toggleUncheckedReleasedID + "-pressed";

                        // No disabled states
                        toggleCheckedDisabledID: "record-start-pressed"
                        toggleUncheckedDisabledID: toggleCheckedDisabledID

                        labelMargin: 0;
                    }

                    onCheckedChanged: {

                        console.log("QML: Start or Stop Recording");

                        // Start or Stop Recording
                        IngeScapeExpeC.startOrStopRecording(checked);
                    }

                    Binding {
                        target: btnStartOrStopRecord
                        property: "checked"
                        value: IngeScapeExpeC.isRecording
                    }
                }
            }
        }
    }


    //
    // Loaded Platform
    //
    LoadedPlatform {
        id: loadedPlatform

        anchors {
            right: parent.right
            rightMargin: 10
            top: commandsPanel.bottom
            topMargin: 20
            bottom: parent.bottom
            bottomMargin: 10
        }
        width: 350
    }


    //
    // Popup about Stop Confirmation
    //
    ConfirmationPopup {
        id: stopConfirmationPopup

        width: 400

        confirmationText: "The record will be stopped and the timeline will be reseted.\nDo you want to stop the record?"

        onConfirmed: {
            console.log("QML: Stop record confirmed");

            // Stop the TimeLine
            IngeScapeExpeC.stopTimeLine();
        }
    }
}
