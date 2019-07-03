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
    property var controller: null;




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
            enabled: controller.currentReplay === null

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

            anchors {
                left: parent.left
                right: parent.right
            }
            height: 60


            // Selection feedback
            Rectangle {
                visible: controller && (controller.selectedRecord === model.QtObject);

                anchors {
                    left : parent.left
                    top : parent.top
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

                visible: playPauseRecordButton.checked
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

                y: 5

                anchors {
                    left : parent.left
                    right : parent.right
                    leftMargin: 70
                    rightMargin: 12
                }

                spacing : 4

                // Name
                Text {
                    id: recordName

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: model.modelM.name
                    color: playPauseRecordButton.checked ? IngeScapeTheme.veryDarkGreyColor : IngeScapeTheme.whiteColor
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

                    color: playPauseRecordButton.checked ? IngeScapeTheme.veryDarkGreyColor :IngeScapeTheme.lightBlueGreyColor
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


            BusyIndicator {
                id: loadingRecordIndicator

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 25
                }
                width: 20
                height: 20

                running: true

                visible: controller && controller.isLoadingRecord
                         && controller.currentReplay && controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid)
            }

            // Load record button
            Button {
                id: loadRecordButton

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 25
                }

                width: 50
                height: 30
                text: "LOAD"

                //visible: !loadingRecordIndicator.visible
                visible: controller && (controller.replayState === ReplayStates.UNLOADED)

                onClicked: {
                    if (controller && model && model.modelM) {
                        //console.log("Load record " + model.modelM.uid);
                        controller.loadRecord(model.modelM.uid);
                    }
                }
            }

            // Play record button
            Button {
                id: playPauseRecordButton

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 25
                }

                //visible: !loadingRecordIndicator.visible
                visible: controller && ((controller.replayState !== ReplayStates.UNLOADED) || (controller.replayState !== ReplayStates.LOADED))
                         && controller.currentReplay && controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid)

                opacity: !enabled ? 0.3 : 1
                /*enabled: !controller.isRecording && (   (controller.currentReplay === null)
                                                     || (controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid)) )*/

                style: I2SvgToggleButtonStyle {
                    fileCache: IngeScapeTheme.svgFileIngeScape

                    toggleCheckedReleasedID: "pause";
                    toggleCheckedPressedID: "pause-pressed";
                    toggleUncheckedReleasedID: "list-play";
                    toggleUncheckedPressedID: "list-play-pressed";

                    // No disabled states
                    toggleCheckedDisabledID: "list-play-pressed"
                    toggleUncheckedDisabledID: toggleCheckedDisabledID

                    labelMargin: 0
                }

                onClicked: {
                    if (controller) {
                        controller.startOrStopReplay(checked);
                    }
                }

                Connections {
                    target: controller

                    onCurrentReplayChanged: {
                        if (controller.currentReplay && controller.currentReplay.modelM && (controller.currentReplay.modelM.uid === model.modelM.uid))
                        {
                            playPauseRecordButton.checked = controller.isPlayingReplay;
                        }
                        else
                        {
                            playPauseRecordButton.checked = false;
                        }
                    }
                }
            }

            Button {
                id: removeButton

                anchors {
                    top: parent.top
                    topMargin: 10
                    right: parent.right
                    rightMargin: 12
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
}




