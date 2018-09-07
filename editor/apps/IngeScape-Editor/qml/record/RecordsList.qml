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
        anchors.fill: parent

        onClicked: {
            if (controller.selectedRecord)
            {
                controller.selectedRecord = null;
            }
        }
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

        color : IngeScapeTheme.selectedTabsBackgroundColor



        Button {
            id: startStopRecordButton

            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 15
            }

            opacity: !enabled ? 0.3 : 1
            enabled: controller.playingRecord === null

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

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

            text: controller ? controller.currentRecordTime.toLocaleTimeString(Qt.locale(), "hh':'mm':'ss'.'zzz")
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

        color: IngeScapeTheme.leftPanelBackgroundColor

        height: 1
    }



    //
    // Visual representation of a record in our list
    //
    Component {
        id: componentRecordListItem

        Item {
            id: recordItem

            property int margin: 5

            height: 60//recordInfos.height + margin*2

            anchors {
                left : parent.left
                right : parent.right
            }

            // Playing feedback
            Rectangle {

                anchors {
                    fill: parent
                }
                visible: playPauseRecordButton.checked
                color : IngeScapeTheme.selectedAgentColor
            }

            // separator
            Rectangle {
                anchors {
                    bottom: parent.bottom
                    left: parent.left
                    right: parent.right
                }

                color: IngeScapeTheme.leftPanelBackgroundColor

                height: 1
            }

            Column {
                id: recordInfos

                y: margin

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
                    color: playPauseRecordButton.checked ? IngeScapeTheme.agentsListItemBackgroundColor : IngeScapeTheme.agentsListLabelColor
                    font: IngeScapeTheme.headingFont
                }

                // IP address
                Text {
                    id: recordIP

                    anchors {
                        left : parent.left
                        right : parent.right
                    }
                    elide: Text.ElideRight

                    text: qsTr("%1 %2 - %3 %4").arg(Qt.formatDate(model.modelM.beginDateTime, "dd/MM/yyyy"))
                    .arg(Qt.formatTime(model.modelM.beginDateTime, "HH:mm"))
                    .arg(Qt.formatDate(model.modelM.endDateTime, "dd/MM/yyyy"))
                    .arg(Qt.formatTime(model.modelM.endDateTime, "HH:mm"))

                    color: playPauseRecordButton.checked ? IngeScapeTheme.agentsListItemBackgroundColor :IngeScapeTheme.agentsListTextColor
                    font: IngeScapeTheme.normalFont
                }
            }


            // Record can be clicked
            MouseArea
            {
                anchors.fill: parent

                onPressed: {
                    if (controller) {
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

                visible: controller.isLoadingRecord && controller.playingRecord && controller.playingRecord.modelM && (controller.playingRecord.modelM.id === model.modelM.id)
            }

            // Play record button
            Button {
                id: playPauseRecordButton

                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    leftMargin: 25
                }

                visible: !loadingRecordIndicator.visible
                opacity: !enabled ? 0.3 : 1
                enabled: !controller.isRecording && (   (controller.playingRecord === null)
                                                     || (controller.playingRecord.modelM && (controller.playingRecord.modelM.id === model.modelM.id)) )

                style: I2SvgToggleButtonStyle {
                    fileCache: IngeScapeTheme.svgFileINGESCAPE

                    toggleCheckedReleasedID: "record-pause";
                    toggleCheckedPressedID: "record-pause-pressed";
                    toggleUncheckedReleasedID: "record-play";
                    toggleUncheckedPressedID: "record-play-pressed";

                    // No disabled states
                    toggleCheckedDisabledID: "record-play-pressed"
                    toggleUncheckedDisabledID: toggleCheckedDisabledID

                    labelMargin: 0
                }

                onClicked: {
                    if (controller) {
                        controller.controlRecord(model.modelM.id, checked)
                    }
                }

                Connections {
                    target: controller

                    onPlayingRecordChanged: {
                        if (controller.playingRecord && controller.playingRecord.modelM && (controller.playingRecord.modelM.id === model.modelM.id))
                        {
                            playPauseRecordButton.checked = true;
                        }
                        else {
                            playPauseRecordButton.checked = false;
                        }
                    }
                }
            }




            // Selected Record feedback
            Item {
                anchors.fill: parent
                visible: controller && (controller.selectedRecord === model.QtObject);

                Rectangle {
                    anchors {
                        left : parent.left
                        top : parent.top
                        bottom: parent.bottom
                        bottomMargin: 1
                    }

                    width : 6
                    color : IngeScapeTheme.selectedAgentColor
                }

                Button {
                    id: removeButton

                    activeFocusOnPress: true

                    anchors {
                        top: parent.top
                        topMargin: 10
                        right : parent.right
                        rightMargin: 12
                    }

                    style: Theme.LabellessSvgButtonStyle {
                        fileCache: IngeScapeTheme.svgFileINGESCAPE

                        pressedID: releasedID + "-pressed"
                        releasedID: "delete"
                        disabledID: releasedID
                    }

                    onClicked: {

                        if (controller)
                        {
                            // Delete selected record
                            controller.deleteSelectedRecord();
                        }
                    }
                }
            }
        }
    }
}




