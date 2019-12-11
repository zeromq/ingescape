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

    property TaskInstanceM modelM: null

    // Flag indicating if we are currently selecting the sessions to export
    property bool isSelectingSessionsToExport: false

    // Duration of the animation about appearance of the check box to select sessions
    property int appearanceAnimationDuration: 250

    property real selectionColumnWidth: 0
    property real subjectColumnWidth: 0
    property real protocolColumnWidth: 0
    property real creationDateTimeColumnWidth: 0
    property real sessionColumnWidth: 0

    property bool isMouseHovering: itemMouseArea.containsMouse || btnDelete.containsMouse || btnOpen.containsMouse

    property alias isSelectedSession: checkBoxSelection.checked

    color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : IngeScapeTheme.whiteColor


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


    //--------------------------------
    //
    //
    // Slots
    //
    //
    //--------------------------------

    onIsSelectingSessionsToExportChanged: {
        if (!isSelectingSessionsToExport && checkBoxSelection.checked) {
            checkBoxSelection.checked = false;
        }
    }


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
            verticalCenter: parent.verticalCenter
        }

        Rectangle {
            width: selectionColumnWidth
            //height: rootItem.height - 2
            height: rootItem.height
            clip: true

            color: IngeScapeAssessmentsTheme.blueButton_pressed
            /*border {
                color: "red"
                width: 1
            }*/

            Behavior on width {
                NumberAnimation {
                    duration: rootItem.appearanceAnimationDuration
                }
            }

            CheckBox {
                id: checkBoxSelection

                anchors {
                    fill: parent
                    margins: 5
                }

                opacity: rootItem.isSelectingSessionsToExport ? 1.0 : 0.0
                enabled: opacity

                Behavior on opacity {
                    NumberAnimation {
                        duration: rootItem.appearanceAnimationDuration
                    }
                }
            }
        }

        Item {
            width: subjectColumnWidth
            height: rootItem.height

            /*color: "transparent"
            border {
                color: "red"
                width: 1
            }*/

            Text {
                id: txtSubjectId

                anchors {
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }
                verticalAlignment: Text.AlignVCenter

                elide: Text.ElideRight
                text: (rootItem.modelM && rootItem.modelM.subject) ? rootItem.modelM.subject.displayedId : ""

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Bold
                    pixelSize: 16
                }
            }
        }

        Item {
            width: protocolColumnWidth
            height: rootItem.height

            /*color: "transparent"
            border {
                color: "red"
                width: 1
            }*/

            Text {
                id: txtProtocolName

                anchors {
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }
                verticalAlignment: Text.AlignVCenter

                elide: Text.ElideRight
                text: (rootItem.modelM && rootItem.modelM.task) ? rootItem.modelM.task.name : ""

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Bold
                    pixelSize: 16
                }
            }
        }

        Item {
            width: creationDateTimeColumnWidth
            height: rootItem.height

            /*color: "transparent"
            border {
                color: "red"
                width: 1
            }*/

            Text {
                id: creationDateTime

                anchors {
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }
                verticalAlignment: Text.AlignVCenter

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

        Item {
            width: sessionColumnWidth
            height: rootItem.height

            /*color: "transparent"
            border {
                color: "red"
                width: 1
            }*/

            Text {
                id: txtSessionName

                anchors {
                    fill: parent
                    leftMargin: 15
                    rightMargin: 5
                }
                verticalAlignment: Text.AlignVCenter

                text: rootItem.modelM ? rootItem.modelM.name : ""
                elide: Text.ElideRight

                color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
                font {
                    family: IngeScapeTheme.textFontFamily
                    weight: Font.Bold
                    pixelSize: 16
                }
            }
        }
    }

    Row {
        id: buttonsRow

        spacing: 10

        anchors {
            right: parent.right
            rightMargin: 14     // sessionScrollView.scrollBarSize + sessionScrollView.verticalScrollbarMargin
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
