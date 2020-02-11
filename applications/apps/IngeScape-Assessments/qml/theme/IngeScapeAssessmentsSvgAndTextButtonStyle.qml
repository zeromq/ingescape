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
 *      Matheu Soum <soum@ingenuity.io>
 *
 */

import QtQuick 2.9
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

import I2Quick 1.0
import INGESCAPE 1.0

ButtonStyle {
    id: root

    // Text properties
    property string text: "BUTTON"
    property int pixelSize: 20

    // SVG properties
    property string releasedID: ""
    property string pressedID: releasedID + "-pressed"
    property string rolloverID: releasedID + "-rollover"
    property string disabledID: releasedID + "-disabled"

    label: Item {
        anchors.fill: parent
        Row {
            anchors.centerIn: parent
            spacing: 16

            I2SvgItem {
                id: svgItem

                svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
                svgFile: ""
                svgElementId: (control.enabled) ? (control.pressed ? pressedID : (control.hovered ? rolloverID : releasedID)) : disabledID

                opaque: false
                fillColor: "transparent"

                anchors {
                    verticalCenter: parent.verticalCenter
                }
            }

            Text {
                anchors {
                    top: parent.top
                    bottom: parent.bottom
                }

                id: labelText
                text: root.text
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                color: IngeScapeTheme.whiteColor

                font {
                    family: IngeScapeTheme.labelFontFamily
                    pixelSize: root.pixelSize
                    weight: Font.Black
                }
            }
        }
    }

    background: Rectangle {
        color: control.enabled ? (control.pressed ? IngeScapeAssessmentsTheme.blueButton_pressed : (control.hovered ? IngeScapeAssessmentsTheme.blueButton_rollover : IngeScapeAssessmentsTheme.blueButton)) : IngeScapeAssessmentsTheme.blueButton_disabled
        radius: 5
    }
}
