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

            delegate: componentPlatform


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


    //
    // Component Platform
    //
    Component {
        id: componentPlatform

        Rectangle {
            id: platformBackground

            width: scrollView.width
            height: model.isLoaded ? 55 : 40

            color: (model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.darkGreyColor : "transparent"

            border {
                color: model.isLoaded ? IngeScapeTheme.orangeColor : "darkgray"
                width: model.isLoaded ? 3 : 1
            }

            Behavior on height {
                NumberAnimation {
                    duration: 300
                }
            }

            Button {
                id: btnLoad

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                height: boundingBox.height
                width: boundingBox.width

                anchors {
                    left: parent.left
                    leftMargin: 5
                    verticalCenter: parent.verticalCenter
                }

                visible: !model.isLoaded
                enabled: IngeScapeExpeC.modelManager && IngeScapeExpeC.modelManager.isEditorON
                opacity: (model.recordState === RecordStates.RECORDED) ? 0.75 : 1.0
                activeFocusOnPress: true

                text: (model.recordState === RecordStates.RECORDED) ? "Re-load" : "Load"

                style: I2SvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    releasedID: "button"
                    pressedID: releasedID + "-pressed"
                    disabledID: releasedID + "-disabled"

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight : Font.Medium
                        pixelSize : 16
                    }
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorDisabled: IngeScapeTheme.lightGreyColor

                }

                onClicked: {
                    console.log("QML: Load platform " + model.name);

                    // Open platform
                    IngeScapeExpeC.openPlatform(model.QtObject);
                }
            }

            Label {
                id: lblIndex

                anchors {
                    left: btnLoad.right
                    leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                width: 50

                text: model ? model.currentIndex + " (" + model.indexOfAlphabeticOrder + ")"
                            : ""

                color: IngeScapeTheme.lightGreyColor //(model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    //weight: Font.Medium
                    pixelSize: 12
                }
            }

            Label {
                id: lblName

                anchors {
                    left: lblIndex.right
                    //leftMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                width: 250

                text: model ? model.name : ""

                color: (model.recordState === RecordStates.RECORDED) ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
                font {
                    family: IngeScapeTheme.textFontFamily
                    //weight: Font.Medium
                    pixelSize: 16
                }
            }

            /*Rectangle {
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
            }*/

            Row {
                id: commands

                anchors {
                    right: parent.right
                    rightMargin: 10
                    verticalCenter: parent.verticalCenter
                }
                height: 30

                visible: model.isLoaded

                spacing: 10

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
                        //IngeScapeExpeC.stopTimeLine();

                        // Open the "Stop" confirmation popup
                        stopConfirmationPopup.open();
                    }
                }
            }
        }
    }
}
