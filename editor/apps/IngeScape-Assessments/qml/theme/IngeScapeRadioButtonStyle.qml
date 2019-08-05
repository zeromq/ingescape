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

import INGESCAPE 1.0

import I2Quick 1.0

RadioButtonStyle {

    indicator: Item {
        height: control.height
        width: height

        I2SvgItem {
            id: logo

            anchors.centerIn: parent

            svgFileCache: IngeScapeAssessmentsTheme.svgFileIngeScapeAssessments
            svgElementId: control.checked ? "radio-button-checked" : "radio-button-unchecked"
        }
    }

    label: Text {
        height: control.height

        text: control.text
        verticalAlignment: Text.AlignVCenter

        color: IngeScapeAssessmentsTheme.regularDarkBlueHeader
        font {
            family: IngeScapeTheme.textFontFamily
            weight: Font.Medium
            pixelSize: 16
        }
    }
}
