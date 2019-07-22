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
        radius: 5

        border {
            color: "darkgray"
            width: 2
        }
    }

    Column {
        id: column

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            margins: 10
        }

        spacing: 10

        Text {
            id: titlePlatforms

            anchors.horizontalCenter: parent.horizontalCenter
            height: 30

            text: "Platforms"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 18
            }
        }

        Item {
            anchors {
                left: parent.left
                right: parent.right
            }
            height: 30

            Button {
                id: btnSelectDirectory

                text: qsTr("Select directory...")

                anchors {
                    top: parent.top
                    bottom: parent.bottom
                    left: parent.left
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
            id: txtPlatformsNumber

            text: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.platformsList.count + " platforms in this directory"
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
            bottom: sorts.top
            left: parent.left
            right: parent.right
            margins: 10
        }

        /*style: IngeScapeScrollViewStyle {
        }*/

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds


        ListView {
            id: platformsListView

            model: IngeScapeExpeC.modelManager ? IngeScapeExpeC.modelManager.platformsList : null

            //
            // Transition animations
            //
            add: Transition {
                NumberAnimation { property: "opacity"; from: 0.0; to: 1.0 }
                NumberAnimation { property: "scale"; from: 0.0; to: 1.0 }
            }

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            move: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutBounce }

                // ensure opacity and scale values return to 1.0
                NumberAnimation { property: "opacity"; to: 1.0 }
                NumberAnimation { property: "scale"; to: 1.0 }
            }

            remove: Transition {
                // ensure opacity and scale values return to 0.0
                NumberAnimation { property: "opacity"; to: 0.0 }
                NumberAnimation { property: "scale"; to: 0.0 }
            }

            delegate: Rectangle {
                id: platformItem

                width: scrollView.width
                height: 36

                //color: model.isLoaded ? IngeScapeTheme.orangeColor : "transparent"
                color: (model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.darkGreyColor : "transparent"

                border {
                    color: model.isLoaded ? IngeScapeTheme.orangeColor : "darkgray"
                    width: model.isLoaded ? 3 : 1
                }

                Button {

                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                        margins: 2
                    }

                    visible: !model.isLoaded
                    enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON

                    text: qsTr("LOAD");

                    onClicked: {
                        console.log("QML: Load platform " + model.name);

                        // Open platform
                        IngeScapeExpeC.openPlatform(model.QtObject);
                    }
                }

                Label {
                    id: lblName

                    anchors {
                        left: parent.left
                        leftMargin: 100
                        verticalCenter: parent.verticalCenter
                    }

                    text: model ? model.name : ""

                    color: (model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        //weight: Font.Medium
                        pixelSize: 16
                    }
                }

                Label {

                    anchors {
                        left: lblName.right
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }

                    text: model ? model.currentIndex + " (" + model.indexOfAlphabeticOrder + ")"
                                : ""

                    color: (model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
                    font {
                        family: IngeScapeTheme.textFontFamily
                        //weight: Font.Medium
                        pixelSize: 12
                    }
                }

                Rectangle {
                    id: feedbackRecording

                    anchors {
                        right: parent.right
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    width: 20
                    height: 20
                    radius: 10

                    visible: (model.recordState === RecordStates.RECORDING)

                    color: IngeScapeTheme.redColor

                    border {
                        color: "darkgray"
                        width: 1
                    }
                }
            }
        }
    }


    Row {
        id: sorts

        anchors {
            bottom: parent.bottom
            bottomMargin: 10
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        height: 30

        spacing: 10

        Text {

            text: "Sort"

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
            id: btnRandomizePlatformsList

            text: "Random"

            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: 120

            onClicked: {
                console.log("QML: Randomize the list of platforms...");

                if (IngeScapeExpeC.modelManager)
                {
                    // Randomize the list of platforms
                    IngeScapeExpeC.modelManager.randomizePlatformsList();
                }
            }
        }

        Button {
            id: btnSortInAlphabeticOrder

            text: "Alphabetical"

            anchors {
                top: parent.top
                bottom: parent.bottom
            }
            width: 120

            onClicked: {
                console.log("QML: Sort the list of platforms in alphabetical order...");

                if (IngeScapeExpeC.modelManager)
                {
                    // Sort the list of platforms in alphabetical order
                    IngeScapeExpeC.modelManager.sortPlatformsListInAlphabeticOrder();
                }
            }
        }
    }
}
