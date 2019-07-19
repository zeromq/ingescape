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
    Label {
        id: title

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 20
        }

        text: "EXPE"

        color: IngeScapeTheme.whiteColor
        font: IngeScapeTheme.headingFont
    }


    // Feedback about IngeScape Editor ON/OFF
    Rectangle {
        anchors {
            top: parent.top
            right: parent.right
            margins: 15
        }
        width: 120
        height: 40

        radius: 5

        color: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON) ? "green" : "orange"

        Label {
            anchors.centerIn: parent
            color: IngeScapeTheme.whiteColor
            font: IngeScapeTheme.headingFont
            text: "EDITOR"
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
        }
        width: (parent.width / 2.0) - 20
        height: parent.height * 0.75
    }


    //
    // Loaded Platform
    //
    LoadedPlatform {
        id: loadedPlatform

        anchors {
            right: parent.right
            rightMargin: 10
            top: title.bottom
            topMargin: 20
        }
        width: (parent.width / 2.0) - 20
        height: parent.height * 0.75
    }

    Item {
        id: itemCommands

        anchors {
            left: loadedPlatform.left
            right: loadedPlatform.right
            top: loadedPlatform.bottom
            topMargin: 10
            bottom: parent.bottom
            bottomMargin: 10
        }

        Rectangle {
            id: bgCommands

            anchors.fill: parent

            color: "transparent"
            border {
                color: "darkgray"
                width: 1
            }

            Column {
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: 5
                }

                spacing: 10

                Row {

                    height: 30

                    spacing: 10

                    Text {
                        id: txtTimeLine

                        text: "TimeLine:"

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
                        id: btnPlayOrPauseTL

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        visible: true
                        activeFocusOnPress: true
                        enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON && IngeScapeExpeC.modelManager.currentLoadedPlatform
                        checkable: true
                        checked: false

                        style: I2SvgToggleButtonStyle {
                            fileCache: IngeScapeExpeTheme.svgFileIngeScapeExpe

                            toggleCheckedReleasedID: "timeline-pause"
                            toggleCheckedPressedID: toggleCheckedReleasedID + "-pressed"
                            toggleUncheckedReleasedID: "timeline-play"
                            toggleUncheckedPressedID: toggleUncheckedReleasedID + "-pressed"

                            // No disabled states
                            toggleCheckedDisabledID: toggleCheckedPressedID
                            toggleUncheckedDisabledID: toggleUncheckedPressedID

                            labelMargin: 0;
                        }

                        onClicked: {
                            //console.log("QML: Play or Pause the timeline");

                            // Play or Pause the TimeLine
                            IngeScapeExpeC.playOrPauseTimeLine(checked);
                        }

                        Binding {
                            target: btnPlayOrPauseTL
                            property: "checked"
                            value: IngeScapeExpeC.isPlayingTimeLine
                        }
                    }

                    Button {
                        id: btnStopTL

                        anchors {
                            top: parent.top
                            bottom: parent.bottom
                        }

                        visible: true
                        enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.currentLoadedPlatform
                        opacity: enabled ? 1.0 : 0.3

                        //text: qsTr("STOP")

                        style: LabellessSvgButtonStyle {
                            fileCache: IngeScapeExpeTheme.svgFileIngeScapeExpe

                            releasedID: "record-stop"
                            pressedID: releasedID + "-pressed"
                            disabledID: pressedID
                        }

                        onClicked: {
                            //console.log("QML: Stop the timeline");

                            // Stop the TimeLine
                            IngeScapeExpeC.stopTimeLine();
                        }
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
    }
}
