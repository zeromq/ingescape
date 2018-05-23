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
    property var controller : null;




    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    // allowing to deselect selected record
    MouseArea {
        anchors.fill: parent
        onClicked:  {
            if(controller.selectedRecord)
            {
                controller.selectedRecord = null;
            }
        }
    }

    //
    // List of records
    //
    ScrollView {
        id : recordsListScrollView

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

            anchors
            {
                left:parent.left
                right:parent.right
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
            id : startStopRecordButton
            anchors
            {
                verticalCenter:parent.verticalCenter
                left:parent.left
                leftMargin:15
            }

            style: I2SvgToggleButtonStyle {
                fileCache: IngeScapeTheme.svgFileINGESCAPE

                toggleCheckedReleasedID :  "stop";
                toggleCheckedPressedID :  "stopped_pressed";
                toggleUncheckedReleasedID : "record";
                toggleUncheckedPressedID : "record_pressed";

                // No disabled states
                toggleCheckedDisabledID: ""
                toggleUncheckedDisabledID: ""

                labelMargin: 0;
            }

            onCheckedChanged: {
                if (controller) {
                    controller.isRecording = checked;
                }
            }
        }

        Text {
            id : currentTimeText
            anchors {
                left : startStopRecordButton.right
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }

            text : controller ? controller.currentRecordTime.toLocaleTimeString(Qt.locale(), "HH':'mm':'ss':'zzz") : "00:00:00.000"
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
    // Visual representation of an record in our list
    //
    Component {
        id: componentRecordListItem

            Item {
                id : recordItem

                property var agentmodel: model.listOfAgents

                property int margin: 5

                height: 60//recordInfos.height + margin*2

                anchors {
                    left : parent.left
                    right : parent.right
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

                Column
                {
                    id:recordInfos
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

                        text: model.recordModel.name
                        color: IngeScapeTheme.agentsListLabelColor
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

                        text: qsTr("%1 %2 - %3 %4").arg(Qt.formatDate(model.recordModel.beginDateTime, "dd/MM/yyyy"))
                                                    .arg(Qt.formatTime(model.recordModel.beginDateTime, "HH:mm"))
                                                    .arg(Qt.formatDate(model.recordModel.endDateTime, "dd/MM/yyyy"))
                                                    .arg(Qt.formatTime(model.recordModel.endDateTime, "HH:mm"))

                        color: IngeScapeTheme.agentsListTextColor
                        font: IngeScapeTheme.normalFont
                    }
                }


                // Record can be clicked
                MouseArea
                {
                    anchors.fill: parent

                    onPressed: {
                        if (controller) {
                            if(controller.selectedRecord === model.QtObject)
                            {
                                controller.selectedRecord = null;
                            } else {
                                controller.selectedRecord = model.QtObject;
                            }
                        }
                    }
                }


                // Play record button
                Button {
                    id : playPauseRecordButton
                    anchors
                    {
                        verticalCenter:parent.verticalCenter
                        left:parent.left
                        leftMargin:25
                    }

                    style: I2SvgToggleButtonStyle {
                        fileCache: IngeScapeTheme.svgFileINGESCAPE

                        toggleCheckedReleasedID :  "pause_actif";
                        toggleCheckedPressedID :  "pause_actif_pressed";
                        toggleUncheckedReleasedID : "play_actif";
                        toggleUncheckedPressedID : "play_actif_pressed";

                        // No disabled states
                        toggleCheckedDisabledID: ""
                        toggleUncheckedDisabledID: ""

                        labelMargin: 0;
                    }

                    onCheckedChanged: {
                        if (controller) {
                            controller.controlRecord(model.recordModel.id, checked)
                        }
                    }

        //            Binding {
        //                target : enabledbutton
        //                property : "checked"
        //                value : (myEffect && myEffect.modelM && myEffect.modelM.mappingEffectValue === MappingEffectValues.MAPPED) ? true : false;
        //            }
                }




                // Selected Record feedback
                Item {
                    anchors.fill: parent
                    visible : controller && (controller.selectedRecord === model.QtObject);

                    Rectangle {
                        anchors {
                            left : parent.left
                            top : parent.top
                            bottom: parent.bottom
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
                            releasedID: "supprimer"
                            disabledID : releasedID
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




