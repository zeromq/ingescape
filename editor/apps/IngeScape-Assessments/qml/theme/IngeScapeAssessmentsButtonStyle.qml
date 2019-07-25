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
    property string text: "BUTTON"
    property int pixelSize: 18

    label: Text {
        id: labelText
        text: root.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: IngeScapeTheme.whiteColor

        //FIXME derived from headerFont
        font {
            family: IngeScapeTheme.labelFontFamily
            pixelSize: root.pixelSize
            weight: Font.Black
        }
    }

    background: Rectangle {
        color: control.pressed ? IngeScapeAssessmentsTheme.blueButton_pressed : IngeScapeAssessmentsTheme.blueButton
        radius: 5
    }
}
