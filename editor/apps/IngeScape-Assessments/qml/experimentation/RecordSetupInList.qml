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
 *      Vincent Peyruqueou <peyruqueou@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

//import "theme" as Theme


Rectangle {
    id: rootItem
    height: 40


    //--------------------------------------------------------
    //
    //
    // Properties
    //
    //
    //--------------------------------------------------------

    property RecordSetupM modelM: null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Open Record Setup
    signal openRecordSetup();

    // Delete Record Setup
    signal deleteRecordSetup();


    property alias taskColumnWidth: taskName.width
    property alias subjectColumnWidth: subjectId.width
    property alias startDateColumnWidth: startDate.width
    property alias startTimeColumnWidth: startTime.width
    property alias durationColumnWidth: duration.width
    property alias buttonColumnWidth: buttonRow.width

    property bool isMouseHovering: itemMouseArea.containsMouse || btnDelete.hovered || btnOpen.hovered

    color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor

    //--------------------------------------------------------
    //
    //
    // Content
    //
    //
    //--------------------------------------------------------

    MouseArea {
        id: itemMouseArea
        anchors.fill: parent
        hoverEnabled: true
        onDoubleClicked: {
            if (rootItem.modelM)
            {
                rootItem.openRecordSetup(rootItem.modelM)
            }
        }
    }

    Row {
        spacing: 0

        anchors {
            left: parent.left
            leftMargin: 15
            verticalCenter: parent.verticalCenter
        }

        Text {
            id: recordName
            width: rootItem.width
                   - taskColumnWidth
                   - subjectColumnWidth
                   - startDateColumnWidth
                   - startTimeColumnWidth
                   - durationColumnWidth
                   - buttonColumnWidth

            text: rootItem.modelM ? rootItem.modelM.name : ""
            elide: Text.ElideRight

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Medium
                pixelSize: 14
            }
        }

        Text {
            id: taskName

            text: (rootItem.modelM && rootItem.modelM.task) ? rootItem.modelM.task.name : ""

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

        Text {
            id: subjectId

            text: (rootItem.modelM && rootItem.modelM.subject) ? rootItem.modelM.subject.displayedId : ""

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

        Text {
            id: startDate

            text: rootItem.modelM ? rootItem.modelM.startDateTime.toLocaleString(Qt.locale(), "dd/MM/yyyy")
                                  : "../../...."

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

        Text {
            id: startTime

            text: rootItem.modelM ? rootItem.modelM.startDateTime.toLocaleString(Qt.locale(), "hh:mm:ss")
                                  : "..:..:.."

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

        // FIXME TODO: record.duration
        Text {
            id: duration

            /*text: rootItem.modelM ? rootItem.modelM.duration.toLocaleString(Qt.locale(), "hh:mm:ss.zzz")
                                        : "00:00:00.000"*/
            text: "00:00:00.000"

            color: IngeScapeTheme.blackColor
            font {
                family: IngeScapeTheme.textFontFamily
                //weight: Font.Medium
                pixelSize: 14
            }
        }

    }

    Row {
        id: buttonRow
        spacing: 14

        anchors {
            right: parent.right
        }
        height: parent.height

        Button {
            id: btnDelete

            anchors.verticalCenter: parent.verticalCenter

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
            }

            width: 40
            height: 30

            onClicked: {
                rootItem.deleteRecordSetup();
            }
        }

        Button {
            id: btnOpen

            anchors.verticalCenter: parent.verticalCenter

            width: 86
            height: 30

            onClicked: {
                rootItem.openRecordSetup();
            }

            style: IngeScapeAssessmentsButtonStyle {
                text: "OPEN"
            }
        }
    }

    Rectangle {
        id: bottomSeparator
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 2
        color: IngeScapeTheme.veryLightGreyColor
    }
}
