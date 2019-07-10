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

    Rectangle {
        id: bg
        anchors.fill: parent
        color: "transparent"

        border {
            color: "darkgray"
            width: 1
        }
    }

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 5
        }

        spacing: 10

        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: 30

            Text {
                id: txtPlatforms

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

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: txtPlatforms.right
                    leftMargin: 10
                }
                width: 150

                onClicked: {
                    console.log("QML: Select directory with platform files...");

                    // Select a Directory
                    IngeScapeExpeC.selectDirectory();
                }
            }

            TextField {
                id: txtCurrentDirectoryPath

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: btnSelectDirectory.right
                    leftMargin: 10
                    right: parent.right
                }

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


    ScrollView {
        id: scrollView

        anchors {
            top: column.bottom
            topMargin: 5
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        /*style: IngeScapeScrollViewStyle {
        }*/

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        Column {
            id: columnPlatforms

            Repeater {
                model: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.platformsList : null

                delegate: Rectangle {
                    id: platformItem

                    property var isLoaded: rootItem.currentLoadedPlatform ? (rootItem.currentLoadedPlatform === model.QtObject) : false

                    width: scrollView.width
                    height: 36

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

                        text: model ? model.name : ""

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
