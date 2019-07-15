/*
 *	IngeScape Editor
 *
 *  Copyright © 2017 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Bruno Lemenicier  <lemenicier@ingenuity.io>
 *
 */

import QtQuick 2.8
import QtQuick.Window 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0


// theme sub-directory
import "../theme" as Theme;

// parent-directory
import ".." as Editor;


Item {
    id: rootItem

    anchors.fill: parent

    //--------------------------------
    //
    // Properties
    //
    //--------------------------------

    // Controller associated to our view
    property RecordsSupervisionController controller: null;

    // Flag indicating if we are playing (or resuming) the current replay
    property bool _isPlayingOrResumingReplay: controller && ((controller.replayState === ReplayStates.PLAYING) || (controller.replayState === ReplayStates.RESUMING));


    //--------------------------------
    //
    // Signals
    //
    //--------------------------------


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected record
    MouseArea {
        id: bgMouseArea

        anchors.fill: parent

        onClicked: {
            if (controller.selectedRecord)
            {
                controller.selectedRecord = null;
            }
        }
    }



    //
    // Header
    //
    Rectangle {
        id: header

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom : recordsListScrollView.top
        }

        color : IngeScapeEditorTheme.selectedTabsBackgroundColor


        Button {
            id: startStopRecordButton

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 15
            }

            opacity: !enabled ? 0.3 : 1
            enabled: (controller.currentReplay === null)

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                toggleCheckedReleasedID: "record-stop";
                toggleCheckedPressedID: "record-stop-pressed";
                toggleUncheckedReleasedID: "record-start";
                toggleUncheckedPressedID: "record-start-pressed";

                // No disabled states
                toggleCheckedDisabledID: "record-start-pressed"
                toggleUncheckedDisabledID: toggleCheckedDisabledID

                labelMargin: 0;
            }

            onCheckedChanged: {
                if (controller) {
                    // FIXME TEST
                    //controller.startOrStopToRecord(checked, false);
                    controller.startOrStopToRecord(checked, true);
                }
            }
        }

        Text {
            id: currentTimeText

            anchors {
                left: startStopRecordButton.right
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }

            text: controller ? controller.currentRecordTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss'.'zzz")
                             : "00:00:00.00"

            color: IngeScapeTheme.whiteColor
            font {
                family: IngeScapeTheme.textFontFamily
                pixelSize: 14
            }
        }
    }


    //
    // Separator
    //
    Rectangle {
        anchors {
            bottom: recordsListScrollView.top
            left: parent.left
            right: parent.right
        }

        color: IngeScapeTheme.blackColor

        height: 1
    }


    //
    // List of records
    //
    ScrollView {
        id: recordsListScrollView

        anchors {
            top: parent.top
            topMargin: 78
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        // Prevent drag overshoot on Windows
        flickableItem.boundsBehavior: Flickable.OvershootBounds

        style: IngeScapeScrollViewStyle {
        }

        // Content of our scrollview
        ListView {
            id: recordsList

            model: controller.recordsList

            delegate: componentRecordListItem

            height: contentHeight

            anchors {
                left: parent.left
                right: parent.right
            }


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




    //
    // Visual representation of a record in our list
    //
    Component {
        id: componentRecordListItem

        Item {
            id: recordItem

            //property var model_record: model.QtObject
            property bool _isCurrentReplay: controller && controller.currentReplay && controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid)

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 60


            // Selection feedback
            Rectangle {
                visible: controller && (controller.selectedRecord === model.QtObject)

                anchors {
                    left: parent.left
                    top: parent.top
                    bottom: parent.bottom
                    bottomMargin: 1
                }

                width: 6
                color: IngeScapeTheme.selectionColor
            }


            // Playing feedback
            Rectangle {
                anchors {
                    fill: parent
                }
                color: IngeScapeTheme.orangeColor

                visible: recordItem._isCurrentReplay && controller && ((controller.replayState === ReplayStates.LOADING) || rootItem._isPlayingOrResumingReplay)
            }

            // Separator
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.blackColor

                height: 1
            }

            Column {
                id: recordInfos

                anchors {
                    top: parent.top
                    topMargin: 5
                    left: parent.left
                    right: parent.right
                    leftMargin: 28
                    rightMargin: 65
                }

                spacing: 4

                // Name
                Text {
                    id: recordName

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: model.modelM.name
                    color: rootItem._isPlayingOrResumingReplay && recordItem._isCurrentReplay ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
                    font: IngeScapeTheme.headingFont
                }

                // Date and Time
                Text {
                    id: recordDateTime

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    /*text: qsTr("%1 %2 - %3 %4").arg(Qt.formatDate(model.modelM.beginDateTime, "dd/MM/yyyy"))
                    .arg(Qt.formatTime(model.modelM.beginDateTime, "HH:mm"))
                    .arg(Qt.formatDate(model.modelM.endDateTime, "dd/MM/yyyy"))
                    .arg(Qt.formatTime(model.modelM.endDateTime, "HH:mm"))*/

                    text: qsTr("%1 %2 - %3").arg(Qt.formatDate(model.modelM.beginDateTime, "dd/MM/yyyy"))
                    .arg(Qt.formatTime(model.modelM.beginDateTime, "HH:mm:ss"))
                    .arg(Qt.formatDateTime(model.modelM.duration, "HH:mm:ss"))

                    color: rootItem._isPlayingOrResumingReplay && recordItem._isCurrentReplay ? IngeScapeTheme.veryDarkGreyColor :IngeScapeTheme.lightBlueGreyColor
                    font: IngeScapeTheme.normalFont
                }
            }


            // Record can be clicked
            MouseArea {
                id: mouseAreaRecordItem

                anchors.fill: parent

                hoverEnabled: true

                onPressed: {
                    if (controller)
                    {
                        // Replay in progress for the selected record...ask confirmation before to UN-load it
                        if (controller.selectedRecord &&
                                ((controller.replayState === ReplayStates.PLAYING) || (controller.replayState === ReplayStates.PAUSED) || (controller.replayState === ReplayStates.RESUMING)) )
                        {
                            if (controller.selectedRecord === model.QtObject)
                            {
                                unloadReplayConfirmationPopup.myReplay = null;
                            }
                            else {
                                unloadReplayConfirmationPopup.myReplay = model.QtObject;
                            }

                            // Open the popup
                            unloadReplayConfirmationPopup.open();
                        }
                        // NO replay in progress
                        else
                        {
                            if (controller.selectedRecord === model.QtObject)
                            {
                                controller.selectedRecord = null;
                            }
                            else {
                                controller.selectedRecord = model.QtObject;
                            }
                        }
                    }
                }
            }


            BusyIndicator {
                id: loadingRecordIndicator

                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 40
                }
                width: 20
                height: 20

                running: true

                visible: controller && (controller.replayState === ReplayStates.LOADING)
                         && recordItem._isCurrentReplay
            }


            // Load record button
            /*Button {
                id: loadRecordButton

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 15
                }
                height: boundingBox.height
                //width: boundingBox.width
                width: 50

                activeFocusOnPress: true

                text: "Load"

                visible: controller && (controller.replayState === ReplayStates.UNLOADED)

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

                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorDisabled: IngeScapeTheme.whiteColor
                }

                onClicked: {
                    if (controller && model && model.modelM)
                    {
                        //console.log("QML: Load record " + model.modelM.uid);

                        // Load record
                        controller.loadRecord(model.modelM.uid);
                    }
                }
            }*/


            // Load record button
            /*Button {
                id: unloadRecordButton

                property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 15
                }
                height: boundingBox.height
                //width: boundingBox.width
                width: 50

                activeFocusOnPress: true

                text: "Quit"

                visible: controller && (controller.replayState !== ReplayStates.UNLOADED) && (controller.replayState !== ReplayStates.LOADING)
                         && recordItem._isCurrentReplay

                enabled: !rootItem._isPlayingOrResumingReplay

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

                    labelColorReleased: IngeScapeTheme.whiteColor
                    labelColorPressed: IngeScapeTheme.blackColor
                    labelColorDisabled: IngeScapeTheme.whiteColor
                }

                onClicked: {
                    if (controller && model && model.modelM)
                    {
                        //console.log("QML: UN-load record " + model.modelM.uid);

                        // UN-load record
                        controller.unloadRecord();
                    }
                }
            }*/


            // Button "Play"
            Button {
                id: playButton

                anchors {
                    //verticalCenter: parent.verticalCenter
                    right: rewindButton.left
                    rightMargin: 5
                    bottom: parent.bottom
                    bottomMargin: 10
                }

                activeFocusOnPress: true

                visible: controller && ((controller.replayState === ReplayStates.LOADED) || (controller.replayState === ReplayStates.PAUSED))
                         && recordItem._isCurrentReplay

                style: LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    releasedID: "list-play"
                    pressedID: releasedID + "-pressed"
                    disabledID: releasedID
                }

                onClicked: {
                    if (controller) {
                        if (controller.replayState === ReplayStates.LOADED)
                        {
                            // Start the current loaded record (replay)
                            controller.startOrResumeReplay(true);
                        }
                        else if (controller.replayState === ReplayStates.PAUSED)
                        {
                            // Resume the current loaded record (replay)
                            controller.startOrResumeReplay(false);
                        }
                    }
                }
            }


            // Button "Pause"
            Button {
                id: pauseButton

                anchors {
                    //verticalCenter: parent.verticalCenter
                    right: rewindButton.left
                    rightMargin: 5
                    bottom: parent.bottom
                    bottomMargin: 10
                }

                visible: rootItem._isPlayingOrResumingReplay
                         && recordItem._isCurrentReplay

                activeFocusOnPress: true

                style: LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    releasedID: "pause"
                    pressedID: releasedID + "-pressed"
                    disabledID: releasedID
                }

                onClicked: {
                    if (controller)
                    {
                        // Pause the current loaded record (replay)
                        controller.stopOrPauseReplay(false);
                    }
                }
            }


            // Button "Rewind"
            Button {
                id: rewindButton

                anchors {
                    //verticalCenter: parent.verticalCenter
                    right: parent.right
                    rightMargin: 20
                    bottom: parent.bottom
                    bottomMargin: 10
                }
                rotation: 180

                visible: controller && ((controller.replayState === ReplayStates.PAUSED) || rootItem._isPlayingOrResumingReplay)
                         && recordItem._isCurrentReplay

                activeFocusOnPress: true

                style: LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    //releasedID: "list-rewind"
                    releasedID: "list-play"
                    pressedID: releasedID + "-pressed"
                    disabledID: releasedID
                }

                onClicked: {
                    if (controller)
                    {
                        // Stop the current loaded record (replay)
                        controller.stopOrPauseReplay(true);
                    }
                }
            }


            // Button "Remove"
            Button {
                id: removeButton

                anchors {
                    top: parent.top
                    topMargin: 10
                    right: parent.right
                    rightMargin: 10
                }

                visible: mouseAreaRecordItem.containsMouse || removeButton.hovered

                activeFocusOnPress: true

                style: LabellessSvgButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    pressedID: releasedID + "-pressed"
                    releasedID: "delete"
                    disabledID: releasedID
                }

                onClicked: {
                    if (controller)
                    {
                        // Delete the record
                        controller.deleteRecord(model.QtObject);
                    }
                }
            }
        }
    }


    //
    // Popup about "Unload Replay" confirmation
    //
    ConfirmationPopup {
        id: unloadReplayConfirmationPopup

        property var myReplay: null;

        confirmationText: "This replay is in progress.\nDo you want to unload it?"

        onConfirmed: {
            if (controller)
            {
                // Select the replay
                controller.selectedRecord = unloadReplayConfirmationPopup.myReplay;
            }
        }
    }
}


