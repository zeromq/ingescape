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

RadioButtonStyle {

    label: Text {

        text: control.text

        color: IngeScapeTheme.whiteColor
        font {
            family: IngeScapeTheme.textFontFamily
            //weight : Font.Medium
            pixelSize : 14
        }
    }
}
