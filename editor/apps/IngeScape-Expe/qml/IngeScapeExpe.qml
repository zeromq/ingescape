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
            margins: 10
        }

        spacing: 10


        // Feedback about IngeScape Editor ON/OFF
        Rectangle {
            width: 120
            height: 35

            radius: 5

            color: "transparent"
            border {
                color: "darkgray"
                width: 1
            }

            Label {
                anchors.centerIn: parent

                text: "Editor"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.labelFontFamily
                    //weight: Font.Normal
                    pixelSize: 16
                }
            }

            Rectangle {
                id: warningNoEditor

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }
                height: 20
                width: height
                radius: height / 2

                color: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON) ? "transparent" : IngeScapeTheme.redColor

                Text {
                    anchors.centerIn: parent

                    text: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON) ? "1" : "0"

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        weight: Font.Black
                        pixelSize: 13
                    }
                }
            }
        }


        // Feedback about IngeScape Recorder ON/OFF
        Rectangle {
            width: 120
            height: 35

            radius: 5

            color: "transparent"
            border {
                color: "darkgray"
                width: 1
            }

            Label {
                anchors.centerIn: parent

                text: "Recorder"

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.labelFontFamily
                    //weight: Font.Normal
                    pixelSize: 16
                }
            }

            Rectangle {
                id: warningNoRecorder

                anchors {
                    right: parent.right
                    rightMargin: 5
                    verticalCenter: parent.verticalCenter
                }
                height: 20
                width: height
                radius: height / 2

                color: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isRecorderON) ? "transparent" : IngeScapeTheme.redColor

                Text {
                    anchors.centerIn: parent

                    text: (IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isRecorderON) ? "1" : "0"

                    color: IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.labelFontFamily
                        weight: Font.Black
                        pixelSize: 13
                    }
                }
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
    // Loaded Platform
    //
    LoadedPlatform {
        id: loadedPlatform

        anchors {
            top: title.bottom
            topMargin: 20
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
            bottomMargin: 10
        }
        width: 350
    }

}
