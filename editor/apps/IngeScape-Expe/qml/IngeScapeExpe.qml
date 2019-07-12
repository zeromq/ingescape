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

            Row {
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: 5
                }
                height: 30

                spacing: 10

                Button {
                    id: btnPlayOrPause

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    visible: true
                    enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.currentLoadedPlatform
                    checkable: true
                    checked: false

                    text: checked ? qsTr("PAUSE") : qsTr("PLAY")

                    onClicked: {
                        //console.log("QML: Play or Pause the timeline (checked = " + checked + ")");

                        // Play or Pause the TimeLine
                        IngeScapeExpeC.playOrPauseTimeLine(checked);
                    }
                }

                Button {
                    id: btnStop

                    anchors {
                        top: parent.top
                        bottom: parent.bottom
                    }

                    visible: true
                    enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.currentLoadedPlatform

                    text: qsTr("STOP")

                    onClicked: {
                        //console.log("QML: Stop the timeline");

                        // Stop the TimeLine
                        IngeScapeExpeC.stopTimeLine();
                    }
                }
            }
        }

        //command_UpdateTimeLineState
    }

}
