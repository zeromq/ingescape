/*
 *	IngeScape Assessments
 *
 *  Copyright © 2019 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Chloé Roumieu   <roumieu@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

AssessmentsPopupBase {
    id: rootPopup

    height: 300
    width: 550

    anchors.centerIn: parent

    title: "WARNING ! Some records can be overloaded !"

    canExitPopup : true

    //--------------------------------------------------------
    //
    // Properties
    //
    //--------------------------------------------------------

    //--------------------------------
    //
    // Signals
    //
    //--------------------------------

    // Signal raised when user confirm that he wants to remove records encountered while recording
    signal removeOtherRecordsWhileRecording()

    // Signal raised when user confirm that he wants to stop its record when it encounters others
    signal stopRecordingWhenEncounterOtherRecords()


    //--------------------------------
    //
    // Slots
    //
    //--------------------------------

    onOpened: {
        checkRemoveOtherRecords.checked = false;
        checkStopRecord.checked = false;
    }


    //--------------------------------
    //
    // Functions
    //
    //--------------------------------


    //--------------------------------
    //
    // Content
    //
    //--------------------------------

    //
    // Body of our popup
    //
    Column {
        anchors {
            left: parent.left
            leftMargin: 30
            top: parent.top
            topMargin: 30
        }
        spacing: 15

        Text {
            id: title

            anchors {
                left: parent.left
            }
            height: 50

            text: qsTr("You are about to record OVER one or more other record(s).\n\nIf your record encounters other records already registered, you can :\n")

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 16
            }
        }

        ExclusiveGroup {
            id: exclusiveGroup
        }

        CheckBox {
            id: checkRemoveOtherRecords

            anchors {
                left: parent.left
            }
            height: 25

            checked: false
            exclusiveGroup: exclusiveGroup
            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                    }

                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    text: "Continue to record and remove other records encountered"
                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                indicator: Rectangle {
                    width: 14
                    height: 14

                    color: IngeScapeTheme.veryDarkGreyColor

                    radius: 8

                    Rectangle {
                        anchors {
                            fill: parent
                            margins: 4
                        }

                        visible: (control.checkedState === Qt.Checked)

                        color: IngeScapeTheme.whiteColor

                        radius: 8
                    }
                }
            }
        }

        CheckBox {
            id: checkStopRecord

            anchors {
                left: parent.left
            }
            height: 25

            checked: false
            exclusiveGroup: exclusiveGroup
            activeFocusOnPress: true;

            style: CheckBoxStyle {
                label: Text {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: 1
                    }

                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    text: "Stop to record at the beginning of the first record encountered"
                    elide: Text.ElideRight

                    font {
                        family: IngeScapeTheme.textFontFamily
                        pixelSize: 16
                    }
                }

                indicator: Rectangle {
                    width: 14
                    height: 14

                    color: IngeScapeTheme.veryDarkGreyColor

                    radius: 8

                    Rectangle {
                        anchors {
                            fill: parent
                            margins: 4
                        }

                        visible: (control.checkedState === Qt.Checked)

                        color: IngeScapeTheme.whiteColor

                        radius: 8
                    }
                }
            }
        }
    }

    //
    // Footer of our popup
    //
    Row {
        anchors {
            right: parent.right
            rightMargin: 28
            bottom : parent.bottom
            bottomMargin: 28
        }
        spacing : 15

        Button {
            id: cancelButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width


            visible: true

            activeFocusOnPress: true

            style: ButtonStyle {
                background: Rectangle {
                    anchors.fill: parent
                    radius: 5
                    color: control.pressed ? IngeScapeTheme.lightGreyColor : (control.hovered ? IngeScapeTheme.veryLightGreyColor : "transparent")
                }

                label: Text {
                    text: "Cancel"
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    color: IngeScapeAssessmentsTheme.regularDarkBlueHeader

                    font {
                        family: IngeScapeTheme.textFontFamily
                        weight: Font.Medium
                        pixelSize: 16
                    }
                }
            }

            onClicked: {
                // Close the popup
                rootPopup.close();
            }
        }

        Button {
            id: okButton

            property var boundingBox: IngeScapeTheme.svgFileIngeScape.boundsOnElement("button");

            anchors {
                verticalCenter: parent.verticalCenter
            }

            height: boundingBox.height
            width: boundingBox.width

            activeFocusOnPress: true

            enabled: checkRemoveOtherRecords.checked || checkStopRecord.checked

            style: IngeScapeAssessmentsButtonStyle {
                text: "Ok"
            }

            onClicked: {
                // Close the popup
                rootPopup.close();

                // Raise appropriate signal
                if (checkRemoveOtherRecords.checked)
                {
                    rootPopup.removeOtherRecordsWhileRecording();
                }
                else if (checkStopRecord.checked)
                {
                    rootPopup.stopRecordingWhenEncounterOtherRecords();
                }
            }
        }
    }
}
