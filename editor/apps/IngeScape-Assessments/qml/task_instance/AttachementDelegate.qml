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

    height: 40
    color: "transparent"

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
