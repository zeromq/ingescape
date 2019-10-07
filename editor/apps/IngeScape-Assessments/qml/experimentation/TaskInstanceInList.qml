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

    property TaskInstanceM modelM: null;



    //--------------------------------
    //
    //
    // Signals
    //
    //
    //--------------------------------

    // Open Task Instance
    signal openTaskInstance();

    // Delete Task Instance
    signal deleteTaskInstance();


    property alias protocolColumnWidth: txtProtocolName.width
    property alias subjectColumnWidth: subjectId.width
    property alias creationDateTimeColumnWidth: creationDateTime.width
    property alias buttonColumnWidth: buttonRow.width

    property bool isMouseHovering: itemMouseArea.containsMouse || btnDelete.containsMouse || btnOpen.containsMouse

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
                rootItem.openTaskInstance(rootItem.modelM)
            }
        }
    }

    Row {
        id: row

        spacing: 0

        anchors {
            left: parent.left
            leftMargin: 15
            verticalCenter: parent.verticalCenter
        }

        Text {
            id: txtSessionName

            width: rootItem.width - row.anchors.leftMargin
                   - protocolColumnWidth
                   - subjectColumnWidth
                   - creationDateTimeColumnWidth
                   - buttonColumnWidth

            text: rootItem.modelM ? rootItem.modelM.name : ""
            elide: Text.ElideRight

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 16
            }
        }

        Text {
            id: txtProtocolName

            width: protocolColumnWidth
            elide: Text.ElideRight
            text: (rootItem.modelM && rootItem.modelM.task) ? rootItem.modelM.task.name : ""

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 16
            }
        }

        Text {
            id: subjectId

            width: subjectColumnWidth
            elide: Text.ElideRight
            text: (rootItem.modelM && rootItem.modelM.subject) ? rootItem.modelM.subject.displayedId : ""

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 16
            }
        }

        Text {
            id: creationDateTime

            width: creationDateTimeColumnWidth
            elide: Text.ElideRight
            text: rootItem.modelM ? rootItem.modelM.startDateTime.toLocaleString(Qt.locale(), "dd/MM/yyyy") + " " + rootItem.modelM.startDateTime.toLocaleString(Qt.locale(), "hh:mm:ss")
                                  : "../../.... ..:..:.."

            color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
            font {
                family: IngeScapeTheme.textFontFamily
                weight: Font.Bold
                pixelSize: 16
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
            id: btnOpen

            anchors.verticalCenter: parent.verticalCenter

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering ? 1 : 0
            enabled: opacity > 0

            width: 85
            height: 30

            style: IngeScapeAssessmentsButtonStyle {
                text: "OPEN"
            }

            onClicked: {
                rootItem.openTaskInstance();
            }
        }

        Button {
            id: btnDelete

            anchors.verticalCenter: parent.verticalCenter

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering ? 1 : 0
            enabled: opacity > 0

            width: 40
            height: 30

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "delete-blue"
                disabledID: releasedID
            }

            onClicked: {
                rootItem.deleteTaskInstance();
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
