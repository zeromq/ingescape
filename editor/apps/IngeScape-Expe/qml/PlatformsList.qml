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
                         && (IngeScapeExpeC.timeLineState === TimeLineStates.STOPPED)

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

            Item {
                id: commands

                anchors {
                    right: parent.right
                    rightMargin: 0
                    verticalCenter: parent.verticalCenter
                }
                height: 30

                visible: model.isLoaded

                Button {
                    id: btnPlayOrPauseTL

                    anchors {
                        right: parent.right
                        rightMargin: 200
                        verticalCenter: parent.verticalCenter
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

                        if (btnPlayOrPauseTL.checked)
                        {
                            // Play the TimeLine
                            IngeScapeExpeC.playTimeLine(checkBoxWithRecord.checked);
                        }
                        else
                        {
                            // Pause the TimeLine
                            IngeScapeExpeC.pauseTimeLine();
                        }
                    }

                    Binding {
                        target: btnPlayOrPauseTL
                        property: "checked"
                        //value: IngeScapeExpeC.isPlayingTimeLine
                        value: (IngeScapeExpeC.timeLineState === TimeLineStates.PLAYING)
                    }
                }

                CheckBox {
                    id: checkBoxWithRecord

                    anchors {
                        right: parent.right
                        rightMargin: 100
                        verticalCenter: parent.verticalCenter
                    }
                    width: 90

                    checked: true

                    enabled: (IngeScapeExpeC.timeLineState === TimeLineStates.STOPPED)

                    style: CheckBoxStyle {
                        label: Text {
                            anchors {
                                verticalCenter: parent.verticalCenter
                                verticalCenterOffset: 2
                            }

                            color: control.checked ? IngeScapeTheme.whiteColor : IngeScapeTheme.lightGreyColor

                            text: "with record"

                            font {
                                family: IngeScapeTheme.textFontFamily
                                pixelSize: 16
                            }
                        }

                        indicator: Rectangle {
                            implicitWidth: 14
                            implicitHeight: 14
                            border.width: 0
                            color: IngeScapeTheme.darkBlueGreyColor

                            I2SvgItem {
                                visible: control.checked
                                anchors.centerIn: parent

                                svgFileCache: IngeScapeTheme.svgFileIngeScape
                                svgElementId: "check";

                            }
                        }
                    }
                }

                Rectangle {
                    id: feedbackRecording

                    anchors {
                        right: parent.right
                        rightMargin: 70
                        verticalCenter: parent.verticalCenter
                    }
                    width: 20
                    height: 20
                    radius: 10

                    visible: (model.recordState === RecordStates.RECORDING)
                    opacity: 1.0

                    color: IngeScapeTheme.redColor

                    SequentialAnimation {
                        //running: true
                        running: (model.recordState === RecordStates.RECORDING)
                        loops: Animation.Infinite

                        NumberAnimation {
                            target: feedbackRecording
                            property: "opacity";
                            from: 0.7
                            to: 1.0
                            duration: 700
                        }
                        NumberAnimation {
                            target: feedbackRecording
                            property: "opacity";
                            to: 0.0
                            duration: 50
                        }
                        PauseAnimation {
                            duration: 500
                        }
                    }
                }

                Button {
                    id: btnStopTL

                    anchors {
                        right: parent.right
                        rightMargin: 10
                        verticalCenter: parent.verticalCenter
                    }

                    enabled: (IngeScapeExpeC.timeLineState !== TimeLineStates.STOPPED)
                    opacity: enabled ? 1.0 : 0.3

                    style: LabellessSvgButtonStyle {
                        fileCache: IngeScapeExpeTheme.svgFileIngeScapeExpe

                        releasedID: "record-stop"
                        pressedID: releasedID + "-pressed"
                        disabledID: pressedID
                    }

                    onClicked: {
                        //console.log("QML: Stop the timeline");

                        if (model.recordState === RecordStates.RECORDING)
                        {
                            // Open the "Stop" confirmation popup
                            stopConfirmationPopup.open();
                        }
                        else
                        {
                            // Stop the TimeLine
                            IngeScapeExpeC.stopTimeLine();
                        }

                    }
                }
            }
        }
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
