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
import QtQuick.Controls 2.0 as Controls2

import I2Quick 1.0

import INGESCAPE 1.0


// theme sub-directory
import "../theme" as Theme;

// parent-directory
import ".." as Editor;

//import "../popup" as Popup


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
            id: startOrStopRecordButton

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 15
            }

            enabled: (controller.currentReplay === null)
            opacity: enabled ? 1.0 : 0.4

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileIngeScape

                toggleCheckedReleasedID: "record-stop";
                toggleCheckedPressedID: toggleCheckedReleasedID + "-pressed";
                toggleUncheckedReleasedID: "record-start";
                toggleUncheckedPressedID: toggleUncheckedReleasedID + "-pressed";

                // No disabled states
                toggleCheckedDisabledID: toggleCheckedPressedID
                toggleUncheckedDisabledID: toggleUncheckedPressedID

                labelMargin: 0;
            }

            onClicked: {
                if (controller) {
                    console.log("QML: Start or Stop to Record");

                    controller.startOrStopToRecord();
                }
            }

            Binding {
                target: startOrStopRecordButton
                property: "checked"
                value: controller ? controller.isRecording : false
            }
        }

        Text {
            id: currentTimeText

            anchors {
                left: startOrStopRecordButton.right
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

            opacity: startOrStopRecordButton.enabled ? 1.0 : 0.4
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

            anchors {
                left: parent.left
                right: parent.right
            }
            height: contentHeight

            model: controller.recordsList

            delegate: componentRecordListItem

            enabled: !controller.isRecording
            opacity: controller.isRecording ? 0.60 : 1.0


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

            property RecordVM model_recordVM: model.QtObject
            property bool _isCurrentReplay: controller && controller.currentReplay && controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid)

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 85


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
                    topMargin: 10
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

                    text: (recordItem.model_recordVM && recordItem.model_recordVM.modelM) ? recordItem.model_recordVM.modelM.name : ""

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


            Controls2.ToolTip {
                delay: 400
                visible: mouseAreaRecordItem.containsMouse
                text: model ? model.modelM.name : ""
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


            // Button "Remove"
            LabellessSvgButton {
                id: removeButton

                anchors {
                    top: parent.top
                    topMargin: 10
                    right: parent.right
                    rightMargin: 10
                }

                visible: mouseAreaRecordItem.containsMouse || removeButton.hovered

                releasedID: "delete"
                pressedID: releasedID + "-pressed"
                disabledID: releasedID


                onClicked: {
                    // Set the record
                    deleteConfirmationPopup.record = model.QtObject;

                    // Open the popup
                    deleteConfirmationPopup.open();
                }
            }


            // Button "Play"
            LabellessSvgButton {
                id: playButton

                anchors {
                    right: rewindButton.left
                    rightMargin: 5
                    bottom: parent.bottom
                    bottomMargin: 35
                }


                visible: controller && ((controller.replayState === ReplayStates.LOADED) || (controller.replayState === ReplayStates.PAUSED))
                         && recordItem._isCurrentReplay

                releasedID: "list-play"
                pressedID: releasedID + "-pressed"
                disabledID: releasedID


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
            LabellessSvgButton {
                id: pauseButton

                anchors {
                    right: rewindButton.left
                    rightMargin: 5
                    bottom: parent.bottom
                    bottomMargin: 35
                }

                visible: rootItem._isPlayingOrResumingReplay
                         && recordItem._isCurrentReplay

                releasedID: "pause"
                pressedID: releasedID + "-pressed"
                disabledID: releasedID


                onClicked: {
                    if (controller)
                    {
                        // Pause the current loaded record (replay)
                        controller.stopOrPauseReplay(false);
                    }
                }
            }


            // Button "Rewind"
            LabellessSvgButton {
                id: rewindButton

                anchors {
                    right: parent.right
                    rightMargin: 10
                    bottom: parent.bottom
                    bottomMargin: 35
                }

                visible: controller && ((controller.replayState === ReplayStates.PAUSED) || rootItem._isPlayingOrResumingReplay)
                         && recordItem._isCurrentReplay

                releasedID: "list-rewind"
                pressedID: releasedID + "-pressed"
                disabledID: releasedID


                onClicked: {
                    if (controller)
                    {
                        // Stop the current loaded record (replay)
                        controller.stopOrPauseReplay(true);
                    }
                }
            }


            // Options button
            LabellessSvgButton {
                id: btnOptions

                anchors {
                    right: parent.right
                    rightMargin: 10
                    bottom: parent.bottom
                    bottomMargin: 10
                }

                releasedID: "button-options"
                pressedID: releasedID + "-pressed"
                disabledID : releasedID


                onClicked: {
                    //console.log("QML: Open options...");

                    // Parent must be record item and not the list to have good x and y value
                    popupOptions.parent = recordItem;

                    if (recordItem.model_recordVM.modelM) {
                        popupOptions.recordId = recordItem.model_recordVM.modelM.uid;
                    }

                    // Open the popup with options
                    popupOptions.openInScreen();
                }
            }
        }
    }


    //
    // Menu popup with options
    //
    MenuPopup  {
        id : popupOptions

        anchors {
            top: rootItem.top
            left: rootItem.right
            leftMargin: 2
        }

        readonly property int optionHeight: 30
        property string recordId: ""

        // Get height from children
        height: popUpBackground.y + popUpBackground.height
        width: 200

        isModal: true;
        layerColor: "transparent"
        dismissOnOutsideTap : true;

        keepRelativePositionToInitialParent : true;

        onClosed: {

        }
        onOpened: {

        }

        Rectangle {
            id: popUpBackground
            height: buttons.y + buttons.height
            anchors {
                right: parent.right
                left: parent.left
            }
            color: IngeScapeTheme.veryDarkGreyColor
            radius: 5
            border {
                color: IngeScapeTheme.blueGreyColor2
                width: 1
            }

            Column {
                id: buttons
                anchors {
                    right: parent.right
                    left: parent.left
                }

                Button {
                    id: optionExport

                    height: popupOptions.optionHeight
                    width: parent.width

                    text: qsTr("Export")
                    enabled: true

                    style: ButtonStyleOfOption {

                    }

                    onClicked: {
                        //console.log("QML: click on option 'Export'");

                        if (rootItem.controller) {
                            rootItem.controller.exportRecord(popupOptions.recordId);
                        }

                        popupOptions.close();
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


    //
    // Popup about "Delete Record" confirmation
    //
    ConfirmationPopup {
        id: deleteConfirmationPopup

        property RecordVM record: null;

        confirmationText: record ? "Do you want to remove " + record.name + "?"
                                 : ""

        onConfirmed: {
            if (rootItem.controller) {
                rootItem.controller.deleteRecord(record);
            }
        }
    }
}

