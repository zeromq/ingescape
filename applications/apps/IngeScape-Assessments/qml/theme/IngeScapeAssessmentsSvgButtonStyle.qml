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
    property string releasedID: ""
    property string pressedID: releasedID + "-pressed"
    property string rolloverID: releasedID + "-rollover"
    property string disabledID: releasedID + "-disabled"

    background: Rectangle {
        implicitWidth: svgItem.implicitWidth
        implicitHeight: svgItem.implicitHeight

        color : "transparent"

        I2SvgItem {
            id: svgItem

            svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
            svgFile: ""
            svgElementId: (control.enabled) ? (control.pressed ? pressedID : (control.hovered ? rolloverID : releasedID)) : disabledID

            opaque: false
            fillColor: "transparent"

            anchors {
                fill: parent
            }
        }
    }
}
