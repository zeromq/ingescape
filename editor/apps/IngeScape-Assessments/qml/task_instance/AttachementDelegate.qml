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
 *      Mathieu Soum       <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0

import INGESCAPE 1.0

Rectangle {
    id: rootItem

    property var attachementName: null

    property bool isMouseHovering: mouseArea.containsMouse || openButton.containsMouse || deleteButton.containsMouse || downloadButton.containsMouse

    height: 40
    color: rootItem.isMouseHovering ? IngeScapeTheme.veryLightGreyColor : "transparent"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    Text {
        anchors {
            left: parent.left
            leftMargin: 30
            verticalCenter: parent.verticalCenter
        }

        text: rootItem.attachementName
        color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }

    Row {
        id: buttonsRow

        anchors {
            verticalCenter: parent.verticalCenter
            right: parent.right
            rightMargin: 10
        }
        spacing: 14

        Button {
            id: downloadButton

            anchors.verticalCenter: parent.verticalCenter

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering ? 1 : 0
            enabled: opacity > 0

            width: 40
            height: 30

            style: IngeScapeAssessmentsSvgButtonStyle {
                releasedID: "download-button"
                disabledID: releasedID
            }
        }

        Button {
            id: deleteButton

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
        }

        Button {
            id: openButton

            anchors.verticalCenter: parent.verticalCenter

            property bool containsMouse: __behavior.containsMouse

            opacity: rootItem.isMouseHovering ? 1 : 0
            enabled: opacity > 0

            width: 84
            height: 30

            style: IngeScapeAssessmentsButtonStyle {
                text: "OPEN"
            }
        }
    }

    Rectangle {
        id: bottomSeparator

        anchors {
            left: parent.left
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            bottom: parent.bottom
        }

        color: IngeScapeTheme.veryLightGreyColor
        height: 2
    }

}
