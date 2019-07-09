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


    Column {
        id: columnHeaders

        anchors {
            left: parent.left
            leftMargin: 20
            top: title.bottom
            topMargin: 20
        }

        spacing: 10

        Row {

            spacing: 10

            Text {
                text: qsTr("Platforms:")

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
                id: btnSelectDirectory

                text: qsTr("Select directory...")

                width: 150
                height: 30

                onClicked: {
                    console.log("QML: Select directory with platform files...");

                    // Select a Directory
                    IngeScapeExpeC.selectDirectory();
                }
            }

            TextField {
                id: txtPlatformUrl

                height: 30
                width: 400

                text: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.currentDirectoryPath : ""

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

            spacing: 10

            Text {
                text: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.platformsList.count + " platforms"
                                                  : ""

                height: 30

                verticalAlignment: Text.AlignVCenter

                color: IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Medium
                    pixelSize: 16
                }
            }


        }
    }


    ScrollView {
        id: scrollView

        anchors {
            left: parent.left
            leftMargin: 20
            top: columnHeaders.bottom
            topMargin: 20
        }
        height: 400
        width: 400

        /*style: IngeScapeScrollViewStyle {
        }*/

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        Column {
            id: columnPlatforms

            /*anchors {
                left: parent.left
                right: parent.right
            }*/

            Repeater {
                model: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.platformsList : null

                delegate: Rectangle {
                    id: platformItem

                    property var isLoaded: rootItem.currentLoadedPlatform && (rootItem.currentLoadedPlatform === model.QtObject)

                    //width: 350
                    width: scrollView.width
                    height: 36

                    //color: rootItem.currentLoadedPlatform && (rootItem.currentLoadedPlatform === model.QtObject) ? IngeScapeTheme.orangeColor : "transparent"
                    color: platformItem.isLoaded ? IngeScapeTheme.orangeColor : "transparent"

                    border {
                        color: "darkgray"
                        width: 1
                    }

                    Label {

                        anchors {
                            left: parent.left
                            leftMargin: 5
                            verticalCenter: parent.verticalCenter
                        }
                        //width: 150

                        text: model.name

                        color: IngeScapeTheme.whiteColor
                        font {
                            family: IngeScapeTheme.textFontFamily
                            //weight: Font.Medium
                            pixelSize: 16
                        }
                    }

                    Button {

                        anchors {
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                            margins: 2
                        }

                        visible: !platformItem.isLoaded
                        enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON

                        text: qsTr("LOAD");

                        onClicked: {
                            console.log("QML: Load platform " + model.name);

                            // Open platform
                            IngeScapeExpeC.openPlatform(model.QtObject);
                        }
                    }
                }
            }
        }
    }
}
